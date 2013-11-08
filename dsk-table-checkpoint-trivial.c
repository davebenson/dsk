/* A trivial checkpoint class using mmap. */

#include "dsk.h"
#include "dsk-table-checkpoint.h"
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DEFAULT_MMAP_SIZE       (1<<14)
#define MMAP_GRANULARITY        4096
#define MMAP_MIN_RESIZE_GROW    4096

typedef struct _TrivialTableCheckpoint TrivialTableCheckpoint;
struct _TrivialTableCheckpoint
{
  DskTableCheckpoint base;
  int fd;

  /* declared as volatile do writes cannot be reordered. */
  volatile char *mmapped;

  size_t mmapped_size;
  size_t cur_size;              /* always a multiple of 4;
                                   at 'cur_size' sits the 0xffffffff mark */
};

#define TRIVIAL_CP_MAGIC                0xde1414df

/* Format: header:
               magic            (TRIVIAL_CP_MAGIC as uint32le)
               version number   (should be 1, uint32le)
               cp_data size     (uint32le)
               cp_data          (binary-data)
               padding          (to multiple of 4 bytes)
           a list of
               key_length       (!= MAX_UINT32) (uint32le)
               value_length     (uint32le)
               key_data         (binary-data, no padding between key/value))
               value_data       (binary-data)
               0s               (padding to round to multiple of 4 bytes)
           0xff 0xff 0xff 0xff  (to mark the end-of-file)

   This format is designed so we can add atomically to
   the mmapped file by writing all members except the key-length,
   which will still be 0xffffffff, marking EOF until
   the last write of a uint32.  Hopefully, heh, this last write is atomic.
 */

#if (BYTE_ORDER == LITTLE_ENDIAN)
#  define UINT64_TO_LE(val)  (val)
#  define UINT32_TO_LE(val)  (val)
#elif (BYTE_ORDER == BIG_ENDIAN)
#  define UINT64_TO_LE(val)  bswap64(val)
#  define UINT32_TO_LE(val)  bswap32(val)
#else
#  error unknown endianness
#endif
#define UINT64_FROM_LE(val)  UINT64_TO_LE(val)
#define UINT32_FROM_LE(val)  UINT32_TO_LE(val)


static dsk_boolean
resize_mmap (TrivialTableCheckpoint *cp,
             unsigned                min_needed,
             DskError              **error)
{
  unsigned new_size = (min_needed + MMAP_MIN_RESIZE_GROW + MMAP_GRANULARITY - 1)
                    / MMAP_GRANULARITY
                    * MMAP_GRANULARITY;
  void *mmapped;
  if (munmap ((void*) cp->mmapped, cp->mmapped_size) < 0)
    dsk_warning ("error calling munmap(): %s", strerror (errno));
  if (ftruncate (cp->fd, new_size) < 0)
    {
      dsk_set_error (error, "error expanding file to %u bytes: %s",
                     new_size, strerror (errno));
      return DSK_FALSE;
    }
  mmapped = mmap (NULL, new_size, PROT_READ|PROT_WRITE,
                  MAP_SHARED, cp->fd, 0);
  if (mmapped == MAP_FAILED)
    {
      dsk_set_error (error, "mmap of %u bytes failed: %s",
                     new_size, strerror (errno));
      return DSK_FALSE;
    }
  cp->mmapped = mmapped;
  cp->mmapped_size = new_size;
  return DSK_TRUE;
}

static dsk_boolean
table_checkpoint_trivial__add  (DskTableCheckpoint *checkpoint,
                                unsigned            key_length,
                                const uint8_t      *key_data,
                                unsigned            value_length,
                                const uint8_t      *value_data,
                                DskError          **error)
{
  TrivialTableCheckpoint *cp = (TrivialTableCheckpoint *) checkpoint;
  unsigned to_add = 8           /* key/value lengths */
                  + (key_length+value_length+3)/4*4;  /*key/value + padding */
  if (cp->cur_size + to_add + 4 > cp->mmapped_size)
    {
      /* must grow mmap area */
      if (!resize_mmap (cp, cp->cur_size + to_add + 4, error))
        return DSK_FALSE;
    }

  /* write value length */
  * (uint32_t *) (cp->mmapped + cp->cur_size + 4) = UINT32_TO_LE (value_length);

  /* write key */
  memcpy ((char*) cp->mmapped + cp->cur_size + 8, key_data, key_length);

  /* write value */
  memcpy ((char*) cp->mmapped + cp->cur_size + 8 + key_length,
          value_data, value_length);

  if ((key_length + value_length) % 4 != 0)
    {
      /* zero padding */
      memset ((char*) cp->mmapped + cp->cur_size + 8 + key_length + value_length,
              0, 4 - (key_length + value_length) % 4);
    }

  /* write terminal 0xffffffff */
  * (uint32_t *) (cp->mmapped + cp->cur_size + to_add) = 0xffffffff;

  /* atomically add the entry to the journal. */
  * (uint32_t *) (cp->mmapped + cp->cur_size) = UINT32_TO_LE (key_length);

  /* update in-memory info for next add */
  cp->cur_size += to_add;
  return DSK_TRUE;
}

static dsk_boolean
table_checkpoint_trivial__sync (DskTableCheckpoint *checkpoint,
		                DskError          **error)
{
  /* no sync implementation yet */
  DSK_UNUSED (checkpoint);
  DSK_UNUSED (error);
  return DSK_TRUE;
}

static dsk_boolean
table_checkpoint_trivial__close(DskTableCheckpoint *checkpoint,
		                DskError          **error)
{
  //TrivialTableCheckpoint *cp = (TrivialTableCheckpoint *) checkpoint;
  DSK_UNUSED (checkpoint);
  DSK_UNUSED (error);
  return DSK_TRUE;
}

static void
table_checkpoint_trivial__destroy(DskTableCheckpoint *checkpoint)
{
  TrivialTableCheckpoint *cp = (TrivialTableCheckpoint *) checkpoint;
  if (munmap ((void*) cp->mmapped, cp->mmapped_size) < 0)
    dsk_warning ("error calling munmap(): %s", strerror (errno));
  close (cp->fd);
  dsk_free (cp);
}


static DskTableCheckpoint table_checkpoint_trivial__vfuncs =
{
  table_checkpoint_trivial__add,
  table_checkpoint_trivial__sync,
  table_checkpoint_trivial__close,
  table_checkpoint_trivial__destroy
};

static DskTableCheckpoint *
table_checkpoint_trivial__create (DskTableCheckpointInterface *iface,
                                  DskDir             *dir,
                                  const char         *basename,
                                  unsigned            cp_data_len,
                                  const uint8_t      *cp_data,
                                  DskTableCheckpoint *prior,  /* optional */
                                  DskError          **error)
{
  unsigned mmapped_size;
  int fd;
  void *mmapped;
  TrivialTableCheckpoint *rv;

  DSK_UNUSED (iface);
  
  if (prior == NULL)
    mmapped_size = DEFAULT_MMAP_SIZE;
  else
    {
      dsk_assert (prior->add == table_checkpoint_trivial__add);
      mmapped_size = ((TrivialTableCheckpoint *) prior)->mmapped_size;
    }

  if (mmapped_size < cp_data_len + 32)
    {
      mmapped_size = cp_data_len + 32;
      mmapped_size += MMAP_MIN_RESIZE_GROW;
      mmapped_size += MMAP_GRANULARITY - 1;
      mmapped_size /= MMAP_GRANULARITY;
      mmapped_size *= MMAP_GRANULARITY;
    }

  /* create fd */
  fd = dsk_dir_openfd (dir, basename,
                       DSK_DIR_OPENFD_WRITABLE|DSK_DIR_OPENFD_TRUNCATE|DSK_DIR_OPENFD_MAY_CREATE,
                       0666, error);
  if (fd < 0)
    return NULL;

  /* truncate / fallocate */
  if (ftruncate (fd, mmapped_size) < 0)
    {
      dsk_set_error (error, "error expanding file %s to %u bytes: %s",
                     basename, mmapped_size, strerror (errno));
      close (fd);
      return NULL;
    }

  /* mmap */
  mmapped = mmap (NULL, mmapped_size, PROT_READ|PROT_WRITE,
                  MAP_SHARED, fd, 0);
  if (mmapped == MAP_FAILED)
    {
      dsk_set_error (error, "mmap of %u bytes failed: %s",
                     mmapped_size, strerror (errno));
      return DSK_FALSE;
    }

  rv = DSK_NEW (TrivialTableCheckpoint);
  rv->base = table_checkpoint_trivial__vfuncs;
  rv->fd = fd;
  rv->mmapped = mmapped;
  rv->mmapped_size = mmapped_size;
  rv->cur_size = (12 + cp_data_len + 3) / 4 * 4;

  ((uint32_t *) (mmapped))[0] = UINT32_TO_LE (TRIVIAL_CP_MAGIC);
  ((uint32_t *) (mmapped))[1] = UINT32_TO_LE (1);  /* version */
  ((uint32_t *) (mmapped))[2] = UINT32_TO_LE (cp_data_len);  /* version */
  memcpy (mmapped + 12, cp_data, cp_data_len);
  if (cp_data_len % 4 != 0)
    memset (mmapped + 12 + cp_data_len, 0, 4 - cp_data_len % 4);
  * ((uint32_t *) (mmapped+rv->cur_size)) = 0xffffffff;        /* end-marker */

  return &rv->base;
}

static DskTableCheckpoint *
table_checkpoint_trivial__open   (DskTableCheckpointInterface *iface,
                                  DskDir             *dir,
                                  const char         *basename,
                                  unsigned           *cp_data_len_out,
                                  uint8_t           **cp_data_out,
                                  DskTableCheckpointReplayFunc func,
                                  void               *func_data,
                                  DskError          **error)
{
  int fd = -1;
  struct stat stat_buf;
  void *mmapped = NULL;
  TrivialTableCheckpoint *rv;
  unsigned version, cp_data_len;
  unsigned at;

  DSK_UNUSED (iface);

  /* open fd */
  fd = dsk_dir_openfd (dir, basename,
                       DSK_DIR_OPENFD_WRITABLE, 0, error);
  if (fd < 0)
    return NULL;

  /* fstat */
  if (fstat (fd, &stat_buf) < 0)
    {
      dsk_set_error (error, "fstat of %s failed: %s",
                     basename, strerror (errno));
      goto error_cleanup;
    }

  /* mmap */
  mmapped = mmap (NULL, stat_buf.st_size, PROT_READ|PROT_WRITE,
                  MAP_SHARED, fd, 0);
  if (mmapped == MAP_FAILED)
    {
      dsk_set_error (error, "mmap of %u bytes (file %s) failed: %s",
                     (unsigned) stat_buf.st_size, basename, strerror (errno));
      goto error_cleanup;
    }
  
  /* check format */
  if (((uint32_t*)mmapped)[0] != UINT32_TO_LE (TRIVIAL_CP_MAGIC))
    {
      dsk_set_error (error, "checkpoint file %s has bad magic", basename);
      goto error_cleanup;
    }
  version = UINT32_FROM_LE (((uint32_t*)mmapped)[1]);
  if (version != 1)
    {
      dsk_set_error (error, "checkpoint file %s has bad version %u",
                     basename, version);
      goto error_cleanup;
    }
  cp_data_len = UINT32_FROM_LE (((uint32_t*)mmapped)[2]);

  /* replay */
  at = 12 + (cp_data_len + 3) / 4 * 4;
  if (at + 4 > stat_buf.st_size)
    {
      dsk_set_error (error, "checkpoint data length (%u) too big for file's length (%u)",
                     cp_data_len, (unsigned) stat_buf.st_size);
      goto error_cleanup;
    }
  while (* (uint32_t*) (mmapped+at) != 0xffffffff)
    {
      uint32_t key_len, value_len;
      uint32_t kv_len, kvp_len;
      if (at + 8 > stat_buf.st_size)
        {
          dsk_set_error (error, "checkpoint entry header too long");
          goto error_cleanup;
        }
      key_len = UINT32_FROM_LE (((uint32_t *) (mmapped+at))[0]);
      value_len = UINT32_FROM_LE (((uint32_t *) (mmapped+at))[1]);
      kv_len = key_len + value_len;
      if (kv_len < key_len)
        {
          dsk_set_error (error, "key+value length greater than 4G");
          goto error_cleanup;
        }
      kvp_len = (kv_len + 3) / 4 * 4;
      if (at + 8 + kvp_len + 4 > stat_buf.st_size)
        {
          dsk_set_error (error, "checkpoint entry data too long");
          goto error_cleanup;
        }
      if (!func (key_len, mmapped + at + 8,
                 value_len, mmapped + at + 8 + key_len,
                 func_data, error))
        {
          if (error && !*error)
            dsk_set_error (error, "replay handler returned false but didn't set error");
          goto error_cleanup;
        }
      at += 8 + kvp_len;
    }

  /* copy cp_data */
  if (cp_data_len_out != NULL)
    *cp_data_len_out = cp_data_len;
  if (cp_data_out != NULL)
    {
      *cp_data_out = dsk_malloc (cp_data_len);
      memcpy (*cp_data_out, mmapped + 12, cp_data_len);
    }
  rv = DSK_NEW (TrivialTableCheckpoint);
  rv->base = table_checkpoint_trivial__vfuncs;
  rv->fd = fd;
  rv->mmapped = mmapped;
  rv->mmapped_size = stat_buf.st_size;
  rv->cur_size = at;
  return &rv->base;

error_cleanup:
  if (mmapped != NULL && munmap ((void*) mmapped, stat_buf.st_size) < 0)
    dsk_warning ("error calling munmap(): %s", strerror (errno));
  if (fd >= 0)
    close (fd);
  return NULL;
}

DskTableCheckpointInterface dsk_table_checkpoint_trivial =
{
  table_checkpoint_trivial__create,
  table_checkpoint_trivial__open
};
