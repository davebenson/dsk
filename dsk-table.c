#include "dsk.h"
#include "dsk-table-checkpoint.h"
#include "dsk-rbtree-macros.h"
#include "dsk-list-macros.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <sys/file.h>
#include <sys/time.h>    /* gettimeofday() for making tmp filename */

/* AUDIT: n_entries versus entry_count (make sure we have it right all over) */

#define DSK_TABLE_CPDATA_MAGIC   0xf423514e

/* Checkpointing/journalling Algorithm:
   - start an "async-cp" checkpoint with the cp data,
     begin sync of all required files
   - add to journal as needed
   - when the sync finishes:
     - sync the checkpoint/journal file,
       move the checkpoint from async-cp to sync-cp,
       and sync the directory.
       (Delete any unused files)
     - if no or few entries were added to journal,
       wait for enough to flush the journal,
       then begin the sync process again.
     - if a lot of entries were added to the journal,
       flush immediately to a file and begin syncing again.
 */

#define MAX_ID_LENGTH     20
#define FILE_BASENAME_FORMAT   "%"PRIx64

typedef struct _Merge Merge;
typedef struct _PossibleMerge PossibleMerge;
typedef struct _File File;
typedef struct _TreeNode TreeNode;

struct _File
{
  uint64_t id;
  DskTableFileSeeker *seeker;           /* created on demand */

  /* These are the number of items, in the order they were received,
     ignoring merging. */
  uint64_t first_entry_index;
  uint64_t n_entries;

  /* Actual number of entries (post deletion and merging) */
  uint64_t entry_count;

  /* If this has an active merge job on it. */
  Merge *merge;

  /* If this doesn't have an active merge job,
     these are merge job sorted by ratio,
     then number of elements. */
  PossibleMerge *prev_merge;
  PossibleMerge *next_merge;

  dsk_boolean used_by_last_checkpoint;

  File *prev, *next;
};

struct _Merge
{
  File *a, *b;
  DskTableReader *a_reader;
  DskTableReader *b_reader;
  uint64_t out_id;
  DskTableFileWriter *writer;
  uint64_t entries_written;
  uint64_t inputs_remaining;
  unsigned is_complete : 1;
  unsigned finishing_writer : 1;

  Merge *next;
};

struct _PossibleMerge
{
  /* log2(a->entry_count / b->entry_count) * 1024 */
  int actual_entry_count_ratio_log2_b10;
  File *a, *b;

  /* possible merges, ordered by actual_entry_count_ratio_log2_b10,
     then a->n_entries then a->first_entry_index */
  dsk_boolean is_red;
  PossibleMerge *left, *right, *parent;
};

struct _TreeNode
{
  unsigned key_length;
  unsigned value_length;
  unsigned value_alloced;
  TreeNode *left, *right, *parent;
  dsk_boolean is_red;
};

struct _DskTable
{
  DskTableCompareFunc compare;
  void *compare_data;
  DskTableMergeFunc merge;
  void *merge_data;
  dsk_boolean chronological_lookup_merges;
  DskDir *dir;
  DskTableFileInterface *file_interface;
  DskTableCheckpointInterface *cp_interface;

  DskTableCheckpoint *cp;
  uint64_t cp_first_entry_index;
  unsigned cp_n_entries;
  unsigned cp_flush_period;

  DskTableBuffer merge_buffers[2];
  uint64_t next_id;

  /* All existing files, sorted chronologically */
  File *oldest_file, *newest_file;

  /* Merge jobs that have begun, sorted by the number of
     inputs remaining to process.  (a proxy for "time remaining") */
  Merge *running_merges;
  unsigned n_merge_jobs;
  unsigned max_merge_jobs;

  /* small in-memory tree for fast lookups on the most recent values */
  TreeNode *small_tree;
  unsigned tree_size;

  /* stack of defunct files (those referenced by a checkpoint,
     but otherwise not needed); linked together by their next pointer. */
  File *defunct_files;

  PossibleMerge *possible_merge_tree;

  char basename[MAX_ID_LENGTH];
};
/* NOTE NOTE: GET_SMALL_TREE() uses the local variable "table"!!! */
#define COMPARE_SMALL_TREE_NODES(a,b, rv) \
  rv = table->compare (a->key_length, (const uint8_t*)(a+1), \
                       b->key_length, (const uint8_t*)(b+1), \
                       table->compare_data)
#define GET_IS_RED(a)   (a)->is_red
#define SET_IS_RED(a,v) (a)->is_red = (v)
#define GET_SMALL_TREE() \
  (table)->small_tree, TreeNode *, GET_IS_RED, SET_IS_RED, \
  parent, left, right, COMPARE_SMALL_TREE_NODES

#define COMPARE_POSSIBLE_MERGES(A,B, rv) \
  rv = (A->actual_entry_count_ratio_log2_b10 < B->actual_entry_count_ratio_log2_b10) ? -1 \
     : (A->actual_entry_count_ratio_log2_b10 > B->actual_entry_count_ratio_log2_b10) ? +1 \
     : (A->a->n_entries < B->a->n_entries) ? -1 \
     : (A->a->n_entries > B->a->n_entries) ? +1 \
     : (A->a->first_entry_index < B->a->first_entry_index) ? -1 \
     : (A->a->first_entry_index > B->a->first_entry_index) ? +1 \
     : 0
#define GET_POSSIBLE_MERGE_TREE() \
  (table)->possible_merge_tree, PossibleMerge *, GET_IS_RED, SET_IS_RED, \
  parent, left, right, COMPARE_POSSIBLE_MERGES


#define GET_FILE_LIST() \
  File *, table->oldest_file, table->newest_file, prev, next

static inline void
set_table_file_basename (DskTable *table,
                         uint64_t  id)
{
  snprintf (table->basename, MAX_ID_LENGTH, FILE_BASENAME_FORMAT, id);
}

static TreeNode *
lookup_tree_node (DskTable           *table,
                  unsigned            key_length,
                  const uint8_t      *key_data)
{
  TreeNode *result;
#define COMPARE_KEY_TO_NODE(a,b, rv)           \
  rv = table->compare (key_length,             \
                       key_data,               \
                       b->key_length,          \
                       (const uint8_t*)(b+1),  \
                       table->compare_data)
  DSK_RBTREE_LOOKUP_COMPARATOR (GET_SMALL_TREE (), unused,
                                COMPARE_KEY_TO_NODE, result);
#undef COMPARE_KEY_TO_NODE
  return result;
}

static void
set_node_value (DskTable      *table,
                TreeNode      *node,
                unsigned       length,
                const uint8_t *data)
{
  if (node->value_alloced < length)
    {
      unsigned value_alloced = length + 16 - node->key_length % 8;
      TreeNode *new_node = dsk_malloc (sizeof (TreeNode)
                                       + node->key_length + value_alloced);
      new_node->key_length = node->key_length;
      new_node->value_alloced = value_alloced;
      memcpy (new_node + 1, node + 1, node->key_length);
      DSK_RBTREE_REPLACE_NODE (GET_SMALL_TREE (), node, new_node);
      dsk_free (node);
      node = new_node;
    }
  memcpy ((uint8_t*)(node + 1) + node->key_length, data, length);
  node->value_length = length;
}

static void
add_to_tree (DskTable           *table,
             unsigned            key_length,
             const uint8_t      *key_data,
             unsigned            value_length,
             const uint8_t      *value_data)
{
  TreeNode *node = lookup_tree_node (table, key_length, key_data);

  if (node == NULL)
    {
      unsigned value_alloced = value_length + 16 - key_length % 8;
      TreeNode *node = dsk_malloc (sizeof (TreeNode) + key_length + value_alloced);
      TreeNode *conflict;
      memcpy (node + 1, key_data, key_length);
      memcpy ((uint8_t*)(node+1) + key_length, value_data, value_length);
      node->key_length = key_length;
      node->value_length = value_length;
      node->value_alloced = value_alloced;
      DSK_RBTREE_INSERT (GET_SMALL_TREE (), node, conflict);
      table->tree_size += 1;
      dsk_assert (conflict == NULL);
    }
  else
    {
      /* perform merge */
      switch (table->merge (key_length, key_data,
                            node->value_length,
                            (uint8_t *) (node+1) + node->key_length,
                            value_length, value_data,
                            &table->merge_buffers[0],
                            DSK_FALSE,
                            table->merge_data))
        {
        case DSK_TABLE_MERGE_RETURN_A_FINAL:
        case DSK_TABLE_MERGE_RETURN_A:
          break;

        case DSK_TABLE_MERGE_RETURN_B_FINAL:
        case DSK_TABLE_MERGE_RETURN_B:
          /* NOTE: may delete 'node' */
          set_node_value (table, node, value_length, value_data);
          break;

        case DSK_TABLE_MERGE_RETURN_BUFFER_FINAL:
        case DSK_TABLE_MERGE_RETURN_BUFFER:
          /* NOTE: may delete 'node' */
          set_node_value (table, node,
                          table->merge_buffers[0].length,
                          table->merge_buffers[0].data);
          break;

        case DSK_TABLE_MERGE_DROP:
          DSK_RBTREE_REMOVE (GET_SMALL_TREE (), node);
          table->tree_size -= 1;
          dsk_free (node);
          break;
        }
    }
}
static dsk_boolean
handle_checkpoint_replay_element (unsigned            key_length,
                                  const uint8_t      *key_data,
                                  unsigned            value_length,
                                  const uint8_t      *value_data,
                                  void               *replay_data,
                                  DskError          **error)
{
  DskTable *table = replay_data;
  DSK_UNUSED (error);
  add_to_tree (table, key_length, key_data, value_length, value_data);
  table->cp_n_entries += 1;
  return DSK_TRUE;
}

static void
create_possible_merge (DskTable *table,
                       File     *a)
{
  File *b = a->next;
  PossibleMerge *pm;
  PossibleMerge *conflict;
  int lg2_10;
  dsk_assert (b != NULL);
  if (a->entry_count == 0 && b->entry_count == 0)
    {
      lg2_10 = 0;
    }
  else if (a->entry_count == 0)
    {
      lg2_10 = INT_MIN;
    }
  else if (b->entry_count == 0)
    {
      lg2_10 = INT_MAX;
    }
  else
    {
      double ratio = (double)a->entry_count / (double)b->entry_count;
      double lg_ratio = log (ratio) * M_LN2 * 1024;
      lg2_10 = (int) lg_ratio;
    }
  pm = DSK_NEW (PossibleMerge);
  pm->actual_entry_count_ratio_log2_b10 = lg2_10;
  pm->a = a;
  pm->b = b;
  dsk_assert (a->next_merge == NULL);
  dsk_assert (b->prev_merge == NULL);
  a->next_merge = b->prev_merge = pm;
  DSK_RBTREE_INSERT (GET_POSSIBLE_MERGE_TREE (), pm, conflict);
  dsk_assert (conflict == NULL);
}

static void
free_small_tree_recursive (TreeNode *node)
{
  if (node->left != NULL)
    free_small_tree_recursive (node->left);
  if (node->right != NULL)
    free_small_tree_recursive (node->right);
  dsk_free (node);
}

static uint32_t
parse_uint32_le (const uint8_t *data)
{
#if BYTE_ORDER == LITTLE_ENDIAN
  uint32_t v;
  memcpy (&v, data, 4);
  return v;
#else
  return (((uint32_t) data[0]))
       | (((uint32_t) data[1]) << 8)
       | (((uint32_t) data[2]) << 16)
       | (((uint32_t) data[3]) << 24);
#endif
}
static uint64_t
parse_uint64_le (const uint8_t *data)
{
#if BYTE_ORDER == LITTLE_ENDIAN
  uint64_t v;
  memcpy (&v, data, 8);
  return v;
#else
  uint32_t lo = parse_uint32_le (data);
  uint32_t hi = parse_uint32_le (data+4);
  return ((uint64_t)lo) | (((uint64_t)hi) << 32);
#endif
}

static dsk_boolean
parse_checkpoint_data (DskTable *table,
                       unsigned  len,
                       const uint8_t *data,
                       DskError  **error)
{
  uint32_t magic, version;
  unsigned i, n_files;
  if (len < 8)
    {
      dsk_set_error (error, "checkpoint too small (must be 8 bytes, got %u)",
                     len);
      return DSK_FALSE;
    }
  magic = parse_uint32_le (data + 0);
  version = parse_uint32_le (data + 4);
  if (magic != DSK_TABLE_CPDATA_MAGIC)
    {
      dsk_set_error (error, "checkpoint data magic invalid");
      return DSK_FALSE;
    }
  if (version != 0)
    {
      dsk_set_error (error, "bad version of checkpoint data");
      return DSK_FALSE;
    }

  if ((len - 8) % 32 != 0)
    {
      dsk_set_error (error, "checkpoint data should be a multiple of 32 bytes");
      return DSK_FALSE;
    }
  n_files = (len - 8) / 32;
  for (i = 0; i < n_files; i++)
    {
      File *file = DSK_NEW (File);
      const uint8_t *info = data + 8 + 32 * i;
      file->id = parse_uint64_le (info + 0);
      file->seeker = NULL;
      file->first_entry_index = parse_uint64_le (info + 8);
      file->n_entries = parse_uint64_le (info + 16);
      file->entry_count = parse_uint64_le (info + 24);
      file->merge = NULL;
      file->prev_merge = NULL;
      file->next_merge = NULL;
      file->used_by_last_checkpoint = DSK_TRUE;
      DSK_LIST_APPEND (GET_FILE_LIST (), file);
      if (file->id >= table->next_id)
        table->next_id = file->id + 1;
    }
  return DSK_TRUE;
}

static void
write_uint32_le (uint8_t *data, uint32_t value)
{
#if BYTE_ORDER == LITTLE_ENDIAN
  memcpy (data, &value, 4);
#else
  data[0] = value;
  data[1] = value >> 8;
  data[2] = value >> 16;
  data[3] = value >> 24;
#endif
}
static void
write_uint64_le (uint8_t *data, uint64_t value)
{
#if BYTE_ORDER == LITTLE_ENDIAN
  memcpy (data, &value, 8);
#else
  uint32_t lo = value;
  uint32_t hi = (value>>32);
  write_uint32_le (data, lo);
  write_uint32_le (data+4, hi);
#endif
}

static void
make_checkpoint_state (DskTable *table, 
                       unsigned *cp_data_len_out, 
                       uint8_t **cp_data_out)
{
  File *file;
  unsigned n_files = 0;
  unsigned size;
  uint8_t *cp;
  uint8_t *at;
  for (file = table->oldest_file; file != NULL; file = file->next)
    n_files++;

  size = 8 + 32 * n_files;
  cp = dsk_malloc (size);
  write_uint32_le (cp + 0, DSK_TABLE_CPDATA_MAGIC);
  write_uint32_le (cp + 4, 0);          /* version */
  at = cp + 8;
  for (file = table->oldest_file; file != NULL; file = file->next)
    {
      write_uint64_le (at, file->id);
      write_uint64_le (at + 8, file->first_entry_index);
      write_uint64_le (at + 16, file->n_entries);
      write_uint64_le (at + 24, file->entry_count);
      at += 32;
    }
  *cp_data_len_out = size;
  *cp_data_out = cp;
}


static void
kill_possible_merge (DskTable *table,
                     PossibleMerge *possible)
{
  dsk_assert (possible->a->next_merge == possible);
  dsk_assert (possible->b->prev_merge == possible);
  DSK_RBTREE_REMOVE (GET_POSSIBLE_MERGE_TREE (), possible);
  possible->a->next_merge = possible->b->prev_merge = NULL;
  dsk_free (possible);
}

static Merge *
start_merge_job (DskTable *table,
                 PossibleMerge *possible,
                 DskError **error)
{
  Merge *merge;
  Merge **p_next;
  DskTableFileInterface *file_iface = table->file_interface;

  if (possible->a->prev_merge)
    kill_possible_merge (table, possible->a->prev_merge);
  if (possible->b->next_merge)
    kill_possible_merge (table, possible->b->next_merge);

  merge = DSK_NEW (Merge);

  /* setup info about input 'a' */
  merge->a = possible->a;
  set_table_file_basename (table, merge->a->id);
  merge->a_reader = file_iface->new_reader (file_iface,
                                            table->dir,
                                            table->basename, error);

  /* setup info about input 'b' */
  merge->b = possible->b;
  set_table_file_basename (table, merge->b->id);
  merge->b_reader = file_iface->new_reader (file_iface,
                                            table->dir,
                                            table->basename, error);

  /* setup info about output */
  merge->out_id = table->next_id++;
  set_table_file_basename (table, merge->out_id);
  merge->writer = file_iface->new_writer (file_iface,
                                          table->dir,
                                          table->basename, error);

  merge->entries_written = 0;
  merge->inputs_remaining = merge->a->entry_count + merge->b->entry_count;

  /* TODO: it is also complete if all preceding files are of 0 length.
     Not sure if it is worth checking for that. */
  merge->is_complete = merge->a->first_entry_index == 0ULL;

  merge->finishing_writer = DSK_FALSE;

  /* insert sorted into list of running merges */
  p_next = &table->running_merges;
  while (*p_next != NULL
      && merge->inputs_remaining > (*p_next)->inputs_remaining)
    p_next = &((*p_next)->next);
  merge->next = *p_next;
  *p_next = merge;
  table->n_merge_jobs += 1;

  /* set files' 'merge' members */
  possible->a->merge = possible->b->merge = merge;

  kill_possible_merge (table, possible);
  return merge;
}

static dsk_boolean
maybe_start_merge_jobs (DskTable *table,
                        DskError **error)
{
  PossibleMerge *best;
  while (table->n_merge_jobs < table->max_merge_jobs)
    {
      DSK_RBTREE_FIRST (GET_POSSIBLE_MERGE_TREE (), best);
      if (best == NULL)
        return DSK_TRUE;

      /* TODO: make this tunable */
      if (best->actual_entry_count_ratio_log2_b10 > 1024)
        break;

      if (start_merge_job (table, best, error) == NULL)
        return DSK_FALSE;
    }
  return DSK_TRUE;
}
static DskTableMergeResult
dsk_table_std_merge (unsigned       key_length,
                     const uint8_t *key_data,
                     unsigned       a_length,
                     const uint8_t *a_data,
                     unsigned       b_length,
                     const uint8_t *b_data,
                     DskTableBuffer *buffer,
                     dsk_boolean    complete,
                     void          *merge_data)
{
  DSK_UNUSED (key_length);
  DSK_UNUSED (key_data);
  DSK_UNUSED (a_length);
  DSK_UNUSED (a_data);
  DSK_UNUSED (b_length);
  DSK_UNUSED (b_data);
  DSK_UNUSED (buffer);
  DSK_UNUSED (complete);
  DSK_UNUSED (merge_data);

  return DSK_TABLE_MERGE_RETURN_B_FINAL;
}

static int
dsk_table_std_compare (unsigned       key_a_length,
                       const uint8_t *key_a_data,
                       unsigned       key_b_length,
                       const uint8_t *key_b_data,
                       void          *compare_data)
{
  int rv;
  DSK_UNUSED (compare_data);
  if (key_a_length < key_b_length)
    {
      rv = memcmp (key_a_data, key_b_data, key_a_length);
      if (rv == 0)
        rv = -1;
    }
  else if (key_a_length > key_b_length)
    {
      rv = memcmp (key_a_data, key_b_data, key_b_length);
      if (rv == 0)
        rv = 1;
    }
  else
    rv = memcmp (key_a_data, key_b_data, key_a_length);
  return rv;
}

DskTable   *dsk_table_new          (DskTableConfig *config,
                                    DskError      **error)
{
  DskTable rv;
  dsk_boolean is_new = DSK_FALSE;
  memset (&rv, 0, sizeof (rv));
  rv.compare = config->compare;
  rv.compare_data = config->compare_data;
  if (rv.compare == NULL)
    rv.compare = dsk_table_std_compare;
  rv.merge = config->merge;
  rv.merge_data = config->merge_data;
  if (rv.merge == NULL)
    rv.merge = dsk_table_std_merge;
  rv.chronological_lookup_merges = config->chronological_lookup_merges;
  rv.max_merge_jobs = 16;
  if (config->dir != NULL)
    {
      dsk_warning ("loading from %s", dsk_dir_get_str(config->dir));
system(dsk_strdup_printf("ls -ltr %s", dsk_dir_get_str(config->dir)));
      rv.dir = dsk_dir_ref (config->dir);
    }
  else if (config->dir != NULL)
    {
      rv.dir = dsk_dir_ref (config->dir);
      if (rv.dir == NULL)
        return NULL;
    }
  else
    {
      const char *tmp = dsk_get_tmp_dir ();
      unsigned tmp_len = strlen (tmp);
      struct timeval tv;
      unsigned pid = getpid ();

      /* tmpdir will "table-", time in microseconds (10+6 digits);
         "-", process-id  ==> 34 chars or less  */
      unsigned alloc = tmp_len + 40;
      char *dirname = alloca (alloc);
      gettimeofday (&tv, NULL);
      snprintf (dirname, alloc,
                "%s/table-%lu%06u-%u",
                tmp, (unsigned long)(tv.tv_sec), (unsigned)(tv.tv_usec),
                pid);
      rv.dir = dsk_dir_new (NULL, dirname, DSK_DIR_NEW_MAYBE_CREATE, error);
      if (rv.dir == NULL)
        return NULL;
    }
  is_new = dsk_dir_did_create (rv.dir);
  if (config->file_interface == NULL)
    rv.file_interface = &dsk_table_file_interface_trivial;
  else
    rv.file_interface = config->file_interface;

  if (config->cp_interface == NULL)
    rv.cp_interface = &dsk_table_checkpoint_trivial;
  else
    rv.cp_interface = config->cp_interface;
  rv.cp_n_entries = 0;
  rv.cp_first_entry_index = 0;
  rv.cp_flush_period = 1024;
  if (dsk_dir_did_create (rv.dir))
    {
      /* create initial empty checkpoint */
      rv.cp = (*rv.cp_interface->create) (rv.cp_interface,
                                          rv.dir, "CP",
                                          0, NULL, 
                                          NULL,    /* no prior checkpoint */
                                          error);
      rv.next_id = 1;
      if (rv.cp == NULL)
        goto cp_open_failed;
    }
  else
    {
      unsigned cp_data_len;
      uint8_t *cp_data;
      {
        int alive_fd = dsk_dir_openfd (rv.dir, "ALIVE", 0, O_RDONLY, NULL);
        if (alive_fd >= 0)
          {
            dsk_warning ("table was unexpectedly shutdown");
            close (alive_fd);
            alive_fd = -1;
          }
      }
      rv.cp = (*rv.cp_interface->open) (rv.cp_interface,
                                        rv.dir,
                                        "CP",
                                        &cp_data_len, &cp_data,
                                        handle_checkpoint_replay_element, &rv,
                                        error);
      if (rv.cp == NULL)
        goto cp_open_failed;

      /* open files. */
      if (cp_data_len != 0)
        {
          if (!parse_checkpoint_data (&rv, cp_data_len, cp_data, error))
            {
              dsk_free (cp_data);
              goto cp_open_failed;
            }
        }
      dsk_free (cp_data);

      /* create possible merge jobs */
      if (rv.oldest_file != NULL)
        {
          File *file;
          for (file = rv.oldest_file; file->next != NULL; file = file->next)
            create_possible_merge (&rv, file);
        }
      if (rv.newest_file)
        rv.cp_first_entry_index = rv.newest_file->first_entry_index + rv.newest_file->n_entries;
    }

  /* Stuff which requires having the real Table pointer */
  {
    DskTable *table;
    table = dsk_memdup (sizeof (rv), &rv);
    if (!maybe_start_merge_jobs (table, error))
      {
        dsk_table_destroy (table);
        return NULL;
      }
    return table;
  }

cp_open_failed:
  dsk_dir_unref (rv.dir);
  if (rv.small_tree != NULL)
    free_small_tree_recursive (rv.small_tree);
  return NULL;
}

DskDir     *dsk_table_peek_dir     (DskTable       *table)
{
  return table->dir;
}

/* Returns:
     -1 if the key is before the range we are searching for.
      0 if the key is in the range we are searching for.
     +1 if the key is after the range we are searching for.
 */
typedef struct _FindInfo FindInfo;
struct _FindInfo
{
  DskTable *table;
  unsigned search_length;
  const uint8_t *search_data;
};

static int seeker_find_function (unsigned           key_len,
                                 const uint8_t     *key_data,
                                 void              *user_data)
{
  FindInfo *info = user_data;
  DskTable *table = info->table;
  return table->compare (key_len, key_data, info->search_length, info->search_data, table->compare_data);
}

static void
lookup_do_merge_or_set (DskTable *table,
                        unsigned     key_length,
                        const uint8_t *key_data,
                        unsigned     value_length,
                        const uint8_t *value_data,
                        int      *res_index_inout,
                        dsk_boolean *is_done_out,
                        dsk_boolean  is_final)
{
  if (*res_index_inout < 0)
    {
      *res_index_inout = 0;
      memcpy (dsk_table_buffer_set_size (&table->merge_buffers[0],
                                         value_length),
              value_data,
              value_length);
    }
  else
    {
      unsigned a_len = value_length;
      const uint8_t *a_data = value_data;
      DskTableBuffer *res = &table->merge_buffers[*res_index_inout];
      unsigned b_len = res->length;
      const uint8_t *b_data = res->data;
      dsk_boolean is_correct;
      if (table->chronological_lookup_merges)
        {
          { unsigned swap = a_len; a_len = b_len; b_len = swap; }
          { const uint8_t *swap = a_data; a_data = b_data; b_data = swap; }
        }
      switch (table->merge (key_length, key_data,
                            a_len, a_data,
                            b_len, b_data,
                            &table->merge_buffers[1 - *res_index_inout],
                            is_final,
                            table->merge_data))
        {
        case DSK_TABLE_MERGE_RETURN_A_FINAL:
          is_correct = a_data == res->data;
          break;
        case DSK_TABLE_MERGE_RETURN_A:
          is_correct = a_data == res->data;
          *is_done_out = DSK_TRUE;
          break;
        case DSK_TABLE_MERGE_RETURN_B_FINAL:
          is_correct = b_data == res->data;
          break;
        case DSK_TABLE_MERGE_RETURN_B:
          is_correct = b_data == res->data;
          *is_done_out = DSK_TRUE;
          break;
        case DSK_TABLE_MERGE_RETURN_BUFFER_FINAL:
          *res_index_inout = 1 - *res_index_inout;
          *is_done_out = DSK_TRUE;
          return;
        case DSK_TABLE_MERGE_RETURN_BUFFER:
          *res_index_inout = 1 - *res_index_inout;
          return;
        case DSK_TABLE_MERGE_DROP:
          *res_index_inout = -1;
          return;
        default:
          dsk_return_if_reached ("bad ret-val from merge");
        }
      if (!is_correct)
        {
          memcpy (dsk_table_buffer_set_size (res, value_length),
                  value_data, value_length);
        }
    }
}

/* *res_index_inout is -1 if no result has been located yet.
   it may be 0 or 1 depending on which merge_buffer has the result.
 */
static dsk_boolean
do_file_lookup (DskTable    *table,
                File        *file,
                unsigned     key_length,
                const uint8_t *key_data,
                int         *res_index_inout,
                dsk_boolean *is_done_out,
                dsk_boolean  is_final,
                DskError   **error)
{
  DskError *e = NULL;
  unsigned value_length;
  const uint8_t *value_data;
  DskTableFileSeeker *seeker;
  FindInfo find_info = { table, key_length, key_data };
  if (file->seeker == NULL)
    {
      DskTableFileInterface *iface = table->file_interface;
      set_table_file_basename (table, file->id);
      file->seeker = iface->new_seeker (iface,
                                        table->dir,
                                        table->basename,
                                        error);
      if (file->seeker == NULL)
        return DSK_FALSE;
    }
  seeker = file->seeker;
  if (!seeker->find (seeker, seeker_find_function, &find_info,
                     DSK_TABLE_FILE_FIND_ANY,
                     NULL, NULL, &value_length, &value_data,
                     &e))
    {
      if (e)
        {
          if (error)
            *error = e;
          else
            dsk_error_unref (e);
          return DSK_FALSE;
        }
      return DSK_TRUE;
    }

  lookup_do_merge_or_set (table,
                          key_length, key_data,
                          value_length, value_data,
                          res_index_inout, is_done_out, is_final);
  return DSK_TRUE;
}

static void
do_tree_lookup (DskTable *table,
                unsigned        key_length,
                const uint8_t  *key_data,
                int      *res_index_inout,
                dsk_boolean *is_done_out,
                dsk_boolean  is_final)
{
  TreeNode *node = lookup_tree_node (table, key_length, key_data);
  if (node == NULL)
    return;

  lookup_do_merge_or_set (table,
                          key_length, key_data,
                          node->value_length,
                          (const uint8_t*)(node+1) + node->key_length,
                          res_index_inout, is_done_out, is_final);
}

dsk_boolean
dsk_table_lookup       (DskTable       *table,
                        unsigned        key_len,
                        const uint8_t  *key_data,
                        unsigned       *value_len_out,
                        const uint8_t **value_data_out,
                        DskError      **error)
{
  int res_index = -1;
  dsk_boolean is_done = DSK_FALSE;
  if (table->chronological_lookup_merges)
    {
      /* iterate files from oldest to newest */
      File *file = table->oldest_file;
      while (!is_done && file != NULL)
        {
          if (!do_file_lookup (table, file,
                               key_len, key_data,
                               &res_index, &is_done, DSK_FALSE,
                               error))
            return DSK_FALSE;
          file = file->next;
        }
      if (!is_done)
        do_tree_lookup (table, key_len, key_data, &res_index, &is_done, DSK_TRUE);
    }
  else
    {
      File *file = table->newest_file;
      do_tree_lookup (table, key_len, key_data, &res_index, &is_done, DSK_FALSE);
      while (!is_done && file != NULL)
        {
          if (!do_file_lookup (table, file,
                               key_len, key_data,
                               &res_index, &is_done, file->prev == NULL,
                               error))
            return DSK_FALSE;
          file = file->prev;
        }
    }
  if (res_index < 0)
    return DSK_FALSE;
  if (value_len_out != NULL)
    *value_len_out = table->merge_buffers[res_index].length;
  if (value_data_out != NULL)
    *value_data_out = table->merge_buffers[res_index].data;
  return DSK_TRUE;
}

static dsk_boolean
dump_tree_to_writer (DskTableFileWriter *writer,
                     TreeNode           *at,
                     DskError          **error)
{
  const uint8_t *key = (const uint8_t *) (at + 1);
  if (at->left)
    {
      if (!dump_tree_to_writer (writer, at->left, error))
        return DSK_FALSE;
    }
  if (!writer->write(writer, at->key_length, key, at->value_length, key + at->key_length, error))
    return DSK_FALSE;
  if (at->right)
    {
      if (!dump_tree_to_writer (writer, at->right, error))
        return DSK_FALSE;
    }
  return DSK_TRUE;
}

static dsk_boolean
merge_passthrough (Merge *merge,
                   DskTableReader *reader,
                   DskError **error)
{
  /* pass-through b_reader data */
  if (!merge->writer->write (merge->writer,
                             reader->key_length,
                             reader->key_data,
                             reader->value_length,
                             reader->value_data,
                             error))
    return DSK_FALSE;
  merge->entries_written += 1;
  return DSK_TRUE;
}

static dsk_boolean
delete_file (DskTable *table,
             uint64_t  id,
             DskError **error)
{
  set_table_file_basename (table, id);
  return table->file_interface->delete_file (table->file_interface,
                                             table->dir,
                                             table->basename,
                                             error);
}

static dsk_boolean
merge_job_readers_at_eof (DskTable  *table,
                          Merge     *merge,
                          DskError **error)
{
  File *new;
  unsigned i;
  if (merge->writer->finish != NULL)
    {
      switch (merge->writer->finish (merge->writer, error))
        {
        case DSK_TABLE_FILE_WRITER_FINISHED:
          break;
        case DSK_TABLE_FILE_WRITER_FINISHING:
          merge->finishing_writer = DSK_TRUE;
          return DSK_TRUE;
        case DSK_TABLE_FILE_WRITER_FINISH_FAILED:
          return DSK_FALSE;
        }
    }

  /* close writer */
  if (!merge->writer->close (merge->writer, error))
    {
      return DSK_FALSE;
    }

  /* make new File */
  new = DSK_NEW (File);
  new->id = merge->out_id;
  new->seeker = NULL;
  new->first_entry_index = merge->a->first_entry_index;
  new->n_entries = merge->a->n_entries + merge->b->n_entries;
  new->entry_count = merge->entries_written;
  new->merge = NULL;
  new->prev_merge = new->next_merge = NULL;
  new->used_by_last_checkpoint = DSK_FALSE;
  new->prev = merge->a->prev;
  new->next = merge->b->next;
  if (new->prev == NULL)
    table->oldest_file = new;
  else
    new->prev->next = new;
  if (new->next == NULL)
    table->newest_file = new;
  else
    new->next->prev = new;

  /* for each old file:
     - if it is used by the last checkpoint,
       add File to "defunct" list.
     - otherwise, delete it.
   */
  for (i = 0; i < 2; i++)
    {
      File *f = i ? merge->b : merge->a;
      if (f->seeker != NULL)
        {
          f->seeker->destroy (f->seeker);
          f->seeker = NULL;
        }
      if (f->used_by_last_checkpoint)
        {
          /* file must not be deleted but is defunct */
          f->next = table->defunct_files;
          table->defunct_files = f;
        }
      else
        {
          /* file should be deleted */
          /* TODO: currently we ignore errors deleting,
             but what if that is masking programming bugs. */
          delete_file (table, f->id, NULL);
          if (f->seeker)
            f->seeker->destroy (f->seeker);
          dsk_free (f);
        }
    }

  dsk_assert (merge == table->running_merges);
  table->running_merges = merge->next;
  merge->a_reader->destroy (merge->a_reader);
  merge->b_reader->destroy (merge->b_reader);
  merge->writer->destroy (merge->writer);
  dsk_free (merge);


  /* deal with new possible merge jobs */
  if (new->prev != NULL && new->prev->merge == NULL)
    create_possible_merge (table, new->prev);
  if (new->next != NULL && new->next->merge == NULL)
    create_possible_merge (table, new);

  /* maybe time to start another merge job running */
  if (!maybe_start_merge_jobs (table, error))
    return DSK_FALSE;

  return DSK_TRUE;
}

static dsk_boolean
run_first_merge_job (DskTable *table,
                     DskError **error)
{
  Merge *merge = table->running_merges;
  if (merge->finishing_writer)
    return merge_job_readers_at_eof (table, merge, error);
  if (merge->a_reader->at_eof)
    {
      if (merge->b_reader->at_eof)
        {
          /* done */
          if (!merge_job_readers_at_eof (table, merge, error))
            return DSK_FALSE;
        }
      else
        {
          if (!merge_passthrough (merge, merge->b_reader, error))
            return DSK_FALSE;
        }
    }
  else
    {
      if (merge->b_reader->at_eof)
        {
          if (!merge_passthrough (merge, merge->a_reader, error))
            return DSK_FALSE;
        }
      else
        {
          int cmp = table->compare (merge->a_reader->key_length,
                                    merge->a_reader->key_data,
                                    merge->b_reader->key_length,
                                    merge->b_reader->key_data,
                                    table->compare_data);
          if (cmp < 0)
            {
              if (!merge_passthrough (merge, merge->a_reader, error))
                return DSK_FALSE;
              merge->inputs_remaining -= 1;
              if (!merge->b_reader->advance (merge->a_reader, error))
                return DSK_FALSE;
            }
          else if (cmp > 0)
            {
              if (!merge_passthrough (merge, merge->b_reader, error))
                return DSK_FALSE;
              merge->inputs_remaining -= 1;
              if (!merge->b_reader->advance (merge->b_reader, error))
                return DSK_FALSE;
            }
          else
            {
              /* do actual merge */
              switch (table->merge (merge->a_reader->key_length,
                                    merge->a_reader->key_data,
                                    merge->a_reader->value_length,
                                    merge->a_reader->value_data,
                                    merge->b_reader->value_length,
                                    merge->b_reader->value_data,
                                    table->merge_buffers,
                                    merge->is_complete,
                                    table->merge_data))
                {
                case DSK_TABLE_MERGE_RETURN_A_FINAL:
                case DSK_TABLE_MERGE_RETURN_A:
                  if (!merge_passthrough (merge, merge->a_reader, error))
                    return DSK_FALSE;
                  break;
                case DSK_TABLE_MERGE_RETURN_B_FINAL:
                case DSK_TABLE_MERGE_RETURN_B:
                  if (!merge_passthrough (merge, merge->b_reader, error))
                    return DSK_FALSE;
                  break;
                case DSK_TABLE_MERGE_RETURN_BUFFER_FINAL:
                case DSK_TABLE_MERGE_RETURN_BUFFER:
                  if (!merge->writer->write (merge->writer,
                                             merge->a_reader->key_length,
                                             merge->a_reader->key_data,
                                             table->merge_buffers[0].length,
                                             table->merge_buffers[0].data,
                                             error))
                    return DSK_FALSE;
                  merge->entries_written += 1;
                  break;
                case DSK_TABLE_MERGE_DROP:
                  break;
                }

              merge->inputs_remaining -= 2;
              if (!merge->a_reader->advance (merge->a_reader, error)
               || !merge->b_reader->advance (merge->b_reader, error))
                return DSK_FALSE;
            }
        }
    }
  return DSK_TRUE;
}

dsk_boolean
dsk_table_insert       (DskTable       *table,
                        unsigned        key_length,
                        const uint8_t  *key_data,
                        unsigned        value_length,
                        const uint8_t  *value_data,
                        DskError      **error)
{
  /* add to tree */
  add_to_tree (table, key_length, key_data, value_length, value_data);

  /* add to checkpoint */
  if (!table->cp->add (table->cp,
                       key_length, key_data,
                       value_length, value_data,
                       error))
    return DSK_FALSE;
  table->cp_n_entries++;

  /* maybe flush tree, creating new checkpoint */
  if (table->cp_n_entries >= table->cp_flush_period)
    {
      /* write tree to disk */
      DskTableFileWriter *writer;
      uint64_t id = table->next_id++;
      File *file;
      DskTableCheckpoint *new_cp;
      unsigned cp_data_len;
      uint8_t *cp_data;
      set_table_file_basename (table, id);
      writer = table->file_interface->new_writer (table->file_interface,
                                                  table->dir,
                                                  table->basename, error);
      if (writer == NULL)
        return DSK_FALSE;
      if (!dump_tree_to_writer (writer, table->small_tree, error))
        {
          writer->destroy (writer);
          return DSK_FALSE;
        }
      if (!writer->close (writer, error))
        {
          writer->destroy (writer);
          return DSK_FALSE;
        }
      writer->destroy (writer);
      file = DSK_NEW (File);
      file->id = id;
      file->seeker = NULL;
      file->first_entry_index = table->cp_first_entry_index;
      file->n_entries = table->cp_n_entries;
      file->entry_count = table->tree_size;
      file->merge = NULL;
      file->prev_merge = file->next_merge = NULL;
      file->used_by_last_checkpoint = DSK_FALSE;
      DSK_LIST_APPEND (GET_FILE_LIST (), file);
      if (file->prev)
        create_possible_merge (table, file->prev);

      free_small_tree_recursive (table->small_tree);
      table->small_tree = NULL;
      table->tree_size = 0;
      table->cp_first_entry_index += table->cp_n_entries;
      table->cp_n_entries = 0;

      /* create new checkpoint */
      make_checkpoint_state (table, &cp_data_len, &cp_data);
      new_cp = table->cp_interface->create (table->cp_interface,
                                            table->dir,
                                            "NEW_CP",
                                            cp_data_len, cp_data,
                                            table->cp,
                                            error);
      dsk_free (cp_data);
      if (new_cp == NULL)
        {
          return DSK_FALSE;
        }

      /* destroy old checkpoint */
      table->cp->destroy (table->cp);
      table->cp = new_cp;

      /* move new checkpoint into place (atomic) */
      if (dsk_dir_sys_rename (table->dir, "NEW_CP", "CP") < 0)
        {
          dsk_set_error (error, "error renaming checkout (from NEW_CP to CP): %s", strerror (errno));
          return DSK_FALSE;
        }

      /* delete any defunct files */
      while (table->defunct_files != NULL)
        {
          File *f = table->defunct_files;
          table->defunct_files = f->next;

          /* delete f */

          /* TODO: currently we ignore errors deleting,
             but what if that is masking programming bugs. */
          delete_file (table, f->id, NULL);
          dsk_free (f);
        }
    }

  /* work on any running merge jobs */
  {
  unsigned n_processed;
  n_processed = 0;
  while (n_processed < 512 && table->running_merges != NULL)
    {
      if (!run_first_merge_job (table, error))
        return DSK_FALSE;
      n_processed++;
    }

  /* start more jobs */
  if (!maybe_start_merge_jobs (table, error))
    return DSK_FALSE;

  /* do more work, if there's something to do */
  while (n_processed < 512 && table->running_merges != NULL)
    {
      if (!run_first_merge_job (table, error))
        return DSK_FALSE;
      n_processed++;
    }
  }
  return DSK_TRUE;
}

/* create a Reader corresponding to the small tree. */
typedef struct _SmallTreeReader SmallTreeReader;
struct _SmallTreeReader
{
  DskTableReader base;
  unsigned remaining;
  unsigned *kv_len_pairs;
  const uint8_t *kv_data;
};

static unsigned
get_data_size_recursive (TreeNode *node)
{
  return (node->left ? get_data_size_recursive (node->left) : 0)
       + node->key_length + node->value_length
       + (node->right ? get_data_size_recursive (node->right) : 0);
}
static void
small_tree_reader_init_recursive (TreeNode *tree,
                                  unsigned **kv_len_pairs_inout,
                                  uint8_t **kv_data_inout)
{
  if (tree->left)
    small_tree_reader_init_recursive (tree->left, kv_len_pairs_inout, kv_data_inout);
  memcpy (*kv_data_inout, 
          ((uint8_t*)(tree+1)),
          tree->key_length + tree->value_length);
  *kv_data_inout += tree->key_length + tree->value_length;
  (*kv_len_pairs_inout)[0] = tree->key_length;
  (*kv_len_pairs_inout)[1] = tree->value_length;
  (*kv_len_pairs_inout) += 2;
  if (tree->right)
    small_tree_reader_init_recursive (tree->right, kv_len_pairs_inout, kv_data_inout);
}

static dsk_boolean
small_tree_reader_advance (DskTableReader *reader,
                           DskError      **error)
{
  SmallTreeReader *s = (SmallTreeReader *) reader;
  DSK_UNUSED (error);
  if (s->remaining <= 1)
    {
      s->remaining = 0;
      reader->at_eof = DSK_TRUE;
      return DSK_TRUE;
    }
  s->remaining -= 1;
  s->kv_data += s->kv_len_pairs[0] + s->kv_len_pairs[1];
  s->kv_len_pairs += 2;
  reader->key_length = s->kv_len_pairs[0];
  reader->value_length = s->kv_len_pairs[1];
  reader->key_data = s->kv_data;
  reader->value_data = s->kv_data + s->kv_len_pairs[0];
  return DSK_TRUE;
}

static DskTableReader *
make_small_tree_reader (DskTable *table)
{
  unsigned data_size = get_data_size_recursive (table->small_tree);
  unsigned size = table->tree_size * 8;
  SmallTreeReader misc_fields =
  {
    {
      DSK_FALSE,        /* never empty at this point */
      0,0,NULL,NULL,    /* filled in below */
      small_tree_reader_advance,
      (void (*)(DskTableReader*)) dsk_free
    },
    table->tree_size,
    NULL,
    NULL
  };
  SmallTreeReader *rv = dsk_malloc (sizeof(SmallTreeReader)
                                    + size
                                    + data_size);
  unsigned *kv_len_pairs = (unsigned*)(rv+1);
  uint8_t *kv_data = (uint8_t *) (kv_len_pairs + table->tree_size * 2);
  *rv = misc_fields;
  rv->kv_len_pairs = kv_len_pairs;
  rv->kv_data = kv_data;
  small_tree_reader_init_recursive (table->small_tree,
                                    &kv_len_pairs, &kv_data);
  rv->base.key_length = rv->kv_len_pairs[0];
  rv->base.value_length = rv->kv_len_pairs[1];
  rv->base.key_data = rv->kv_data;
  rv->base.value_data = rv->kv_data + rv->kv_len_pairs[0];
  return &rv->base;
}

static dsk_boolean empty_advance (DskTableReader *reader,
                                  DskError      **error)
{
  DSK_UNUSED (reader);
  DSK_UNUSED (error);
  return DSK_TRUE;
}
static void empty_destroy (DskTableReader *reader)
{
  DSK_UNUSED (reader);
}
static DskTableReader *
make_empty_reader (void)
{
  static DskTableReader empty =
    {
      DSK_TRUE,         /* always at EOF */
      0,0,NULL,NULL,    /* unused */
      empty_advance,
      empty_destroy
    };
  return &empty;
}

typedef struct _ReaderPlanNode ReaderPlanNode;
struct _ReaderPlanNode
{
  DskTableReader *reader;
  uint64_t est_entry_count;
};

DskTableReader *
dsk_table_new_reader (DskTable    *table,
                      DskError   **error)
{
  /* Create an array of readers */
  ReaderPlanNode *planner;
  unsigned n;
  File *file;

  /* Figure out the size of the planner (must match the next sections) */
  n = 0;
  for (file = table->oldest_file; file; file = file->next)
    if (file->entry_count > 0)
      n++;
  if (table->small_tree != NULL)
    n++;

  /* Initialize the planner from the Files. */
  planner = alloca (sizeof (ReaderPlanNode) * n);
  n = 0;
  for (file = table->oldest_file; file; file = file->next)
    if (file->entry_count > 0)
      {
        DskTableFileInterface *iface = table->file_interface;
        planner[n].est_entry_count = file->entry_count;
        set_table_file_basename (table, file->id);
        planner[n].reader = iface->new_reader (iface,
                                               table->dir,
                                               table->basename, error);
        if (planner[n].reader == NULL)
          {
            unsigned i;
            for (i = 0; i < n; i++)
              planner[i].reader->destroy (planner[i].reader);
            return NULL;
          }
        n++;
      }

  /* Create a planner entry for the tree */
  if (table->small_tree)
    {
      planner[n].est_entry_count = table->tree_size;
      planner[n].reader = make_small_tree_reader (table);
      n++;
    }
#if 0
  dsk_warning("dsk_table_new_reader: n planner entries %u", n);
  for (unsigned i = 0 ; i < n; i++)
    dsk_warning("dsk_table_new_reader: planner[%u] = %llu", i, planner[i].est_entry_count);
#endif

  /* Merge together readers with the least ratio a/b. */
  /* XXX: this could obviously be done way
     more efficiently (probably O(n log n) instead of O(n^2));
     given that the n is typically less than 10, and near plausibly more that 20.

     Another point is that we could advance the merge jobs
     when the reader gets to that point.  */
  while (n > 1)
    {
      /* find the pair with the least a/b ratio */
      unsigned i;
      unsigned best_i;
      unsigned least_ab;
      for (i = 0; i + 1 < n; i++)
        {
          double ab = (double) planner[i].est_entry_count
                    / (double) planner[i+1].est_entry_count;
          if (i == 0 || ab < least_ab)
            {
              best_i = i;
              least_ab = ab;
            }
        }

      /* merge best_i and best_i+1 */

      planner[best_i].reader
        = dsk_table_reader_new_merge2 (table,
                                       planner[best_i].reader,
                                       planner[best_i+1].reader,
                                       error);
      if (planner[best_i].reader == NULL)
        {
          for (i = 0; i < best_i; i++)
            planner[i].reader->destroy (planner[i].reader);
          for (i = best_i + 2; i < n; i++)
            planner[i].reader->destroy (planner[i].reader);
          return NULL;
        }

      /* NOTE: other estimators might work better,
         but it is critical that the estimate never be 0,
         lest you get a divide-by-zero error above. */
      planner[best_i].est_entry_count
        = planner[best_i].est_entry_count
        + planner[best_i+1].est_entry_count;

      /* Remove best_i+1 from the array */
      memmove (planner + best_i + 1,
               planner + best_i + 2,
               (n - (best_i + 2)) * sizeof (ReaderPlanNode));
      n--;
    }
  if (n == 0)
    {
      return make_empty_reader ();
    }
  else
    {
      return planner[0].reader;
    }
}

static void
destroy_file_list (File *file)
{
  while (file != NULL)
    {
      File *next = file->next;
      if (file->seeker)
        file->seeker->destroy (file->seeker);
      dsk_free (file);
      file = next;
    }
}

static void
free_possible_merge_tree_recursive (PossibleMerge *pm)
{
  if (pm->left)
    free_possible_merge_tree_recursive (pm->left);
  if (pm->right)
    free_possible_merge_tree_recursive (pm->right);
  dsk_free (pm);
}

void
dsk_table_destroy      (DskTable       *table)
{
  table->cp->close (table->cp, NULL);
  table->cp->destroy (table->cp);
  destroy_file_list (table->oldest_file);
  if (table->small_tree)
    free_small_tree_recursive (table->small_tree);
  while (table->running_merges)
    {
      Merge *merge = table->running_merges;
      table->running_merges = merge->next;

      merge->a_reader->destroy (merge->a_reader);
      merge->b_reader->destroy (merge->b_reader);
      merge->writer->destroy (merge->writer);
      dsk_free (merge);
    }
  destroy_file_list (table->defunct_files);
  if (table->possible_merge_tree)
    free_possible_merge_tree_recursive (table->possible_merge_tree);
  dsk_dir_unref (table->dir);
  dsk_free (table->merge_buffers[0].data);
  dsk_free (table->merge_buffers[1].data);
  dsk_free (table);
}

void
dsk_table_destroy_erase(DskTable       *table)
{
  dsk_dir_set_erase_on_destroy (table->dir, DSK_TRUE);
  dsk_table_destroy (table);
}

/* --- dsk_table_reader_new_merge2 --- */
typedef struct _TableReaderMerge2 TableReaderMerge2;
typedef enum
{
  TABLE_READER_MERGE2_A,
  TABLE_READER_MERGE2_B,
  TABLE_READER_MERGE2_BOTH,
  TABLE_READER_MERGE2_EOF
} TableReaderMerge2State;

struct _TableReaderMerge2
{
  DskTableReader base;
  DskTable       *table;
  DskTableReader *a;
  DskTableReader *b;
  DskTableBuffer buffer;
  TableReaderMerge2State state;
};
static void
merge2_copy_kv_data (DskTableReader *dst,
                     DskTableReader *src)
{
  dst->key_length = src->key_length;
  dst->key_data = src->key_data;
  dst->value_length = src->value_length;
  dst->value_data = src->value_data;
}

static dsk_boolean
merge2_setup_output (TableReaderMerge2 *merge2)
{
  DskTableReader *a = merge2->a;
  DskTableReader *b = merge2->b;
  DskTable *table = merge2->table;
  int cmp;

  /* compare a,b; possibly merge */
  cmp = table->compare (a->key_length, a->key_data,
                        b->key_length, b->key_data,
                        table->compare_data);
  if (cmp < 0)
    {
      merge2->state = TABLE_READER_MERGE2_A;
      merge2_copy_kv_data (&merge2->base, a);
    }
  else if (cmp > 0)
    {
      merge2->state = TABLE_READER_MERGE2_B;
      merge2_copy_kv_data (&merge2->base, b);
    }
  else
    {
      merge2->state = TABLE_READER_MERGE2_BOTH;
      switch (table->merge (merge2->a->key_length,
                            merge2->a->key_data,
                            merge2->a->value_length,
                            merge2->a->value_data,
                            merge2->b->value_length,
                            merge2->b->value_data,
                            &merge2->buffer,
                            DSK_FALSE,          /* complete? */
                            table->merge_data))
        {
        case DSK_TABLE_MERGE_RETURN_A_FINAL:
        case DSK_TABLE_MERGE_RETURN_A:
          merge2_copy_kv_data (&merge2->base, a);
          break;
        case DSK_TABLE_MERGE_RETURN_B_FINAL:
        case DSK_TABLE_MERGE_RETURN_B:
          merge2_copy_kv_data (&merge2->base, b);
          break;
        case DSK_TABLE_MERGE_RETURN_BUFFER_FINAL:
        case DSK_TABLE_MERGE_RETURN_BUFFER:
          merge2->base.key_length = a->key_length;
          merge2->base.key_data = a->key_data;
          merge2->base.value_length = merge2->buffer.length;
          merge2->base.value_data = merge2->buffer.data;
          break;
        case DSK_TABLE_MERGE_DROP:
          merge2->state = TABLE_READER_MERGE2_BOTH;
          return DSK_FALSE;
        }
    }
  return DSK_TRUE;
}

static dsk_boolean
merge2_advance (DskTableReader *reader,
                DskError      **error)
{
  TableReaderMerge2 *merge2 = (TableReaderMerge2 *) reader;
restart_advance:
  switch (merge2->state)
    {
    case TABLE_READER_MERGE2_A:
      if (!merge2->a->advance (merge2->a, error))
        return DSK_FALSE;
      break;
    case TABLE_READER_MERGE2_B:
      if (!merge2->b->advance (merge2->b, error))
        return DSK_FALSE;
      break;
    case TABLE_READER_MERGE2_BOTH:
      if (!merge2->a->advance (merge2->a, error)
       || !merge2->b->advance (merge2->b, error))
        return DSK_FALSE;
      break;
    case TABLE_READER_MERGE2_EOF:
      return DSK_TRUE;
    }

  if (merge2->a->at_eof && merge2->b->at_eof)
    {
      merge2->base.at_eof = DSK_TRUE;
      merge2->state = TABLE_READER_MERGE2_EOF;
    }
  else if (merge2->a->at_eof)
    {
      merge2_copy_kv_data (&merge2->base, merge2->b);
      merge2->state = TABLE_READER_MERGE2_B;
    }
  else if (merge2->b->at_eof)
    {
      merge2_copy_kv_data (&merge2->base, merge2->a);
      merge2->state = TABLE_READER_MERGE2_A;
    }
  else
    {
      if (!merge2_setup_output (merge2))
        goto restart_advance;
    }
  return DSK_TRUE;
}

static void
merge2_destroy (DskTableReader *reader)
{
  TableReaderMerge2 *merge2 = (TableReaderMerge2 *) reader;
  merge2->a->destroy (merge2->a);
  merge2->b->destroy (merge2->b);
  dsk_free (merge2->buffer.data);
  dsk_free (merge2);
}

DskTableReader *
dsk_table_reader_new_merge2 (DskTable *table,
                             DskTableReader *a,
                             DskTableReader *b,
                             DskError      **error)
{
  TableReaderMerge2 *merge2;

  /* dispense with some trivial cases */
  if (a->at_eof && b->at_eof)
    {
      a->destroy (a);
      return b;
    }
  else if (a->at_eof)
    {
      a->destroy (a);
      return b;
    }
  else if (b->at_eof)
    {
      b->destroy (b);
      return a;
    }

  merge2 = DSK_NEW (TableReaderMerge2);
  merge2->base.advance = merge2_advance;
  merge2->base.destroy = merge2_destroy;
  merge2->base.at_eof = DSK_FALSE;
  merge2->table = table;
  merge2->a = a;
  merge2->b = b;
  merge2->buffer.length = 0;
  merge2->buffer.data = NULL;
  merge2->buffer.alloced = 0;

  if (!merge2_setup_output (merge2))
    {
      if (!merge2_advance (&merge2->base, error))
        {
          merge2_destroy (&merge2->base);
          return NULL;
        }
    }

  return &merge2->base;
}

