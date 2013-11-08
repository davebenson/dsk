#include <alloca.h>
#include "dsk.h"
#include "dsk-config.h"
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


#if DSK_IS_LITTLE_ENDIAN
#  define UINT64_TO_LE(val)  (val)
#  define UINT32_TO_LE(val)  (val)
#elif DSK_IS_BIG_ENDIAN
#  define UINT64_TO_LE(val)  bswap64(val)
#  define UINT32_TO_LE(val)  bswap32(val)
#else
#  error unknown endianness
#endif

#if DSK_HAS_UNLOCKED_STDIO_SUPPORT
# define UNLOCKED_FREAD  fread_unlocked
# define UNLOCKED_FWRITE fwrite_unlocked
#else
# define UNLOCKED_FREAD  fread
# define UNLOCKED_FWRITE fwrite
#endif

/* synonyms provided for clarity */
#define UINT64_FROM_LE UINT64_TO_LE
#define UINT32_FROM_LE UINT32_TO_LE

/* NOTE: we assume that this structure can be fwritten on little-endian
   architectures.  on big-endian, swaps are required.
   in all cases, the data must be packed in the order given. */
typedef struct _IndexEntry IndexEntry;
struct _IndexEntry
{
  uint64_t heap_offset;
  uint32_t key_length;
  uint32_t value_length;
};
#define SIZEOF_INDEX_ENTRY sizeof(IndexEntry)


/* === Writer === */
typedef struct _TableFileTrivialWriter TableFileTrivialWriter;
struct _TableFileTrivialWriter
{
  DskTableFileWriter base_instance;
  FILE *index_fp, *heap_fp;
  uint64_t heap_fp_offset;
};


static dsk_boolean 
table_file_trivial_writer__write  (DskTableFileWriter *writer,
                                   unsigned            key_length,
                                   const uint8_t      *key_data,
                                   unsigned            value_length,
                                   const uint8_t      *value_data,
                                   DskError          **error)
{
  TableFileTrivialWriter *wr = (TableFileTrivialWriter *) writer;
  
  IndexEntry ie;
  ie.heap_offset = UINT64_TO_LE (wr->heap_fp_offset);
  ie.key_length = UINT32_TO_LE (key_length);
  ie.value_length = UINT32_TO_LE (value_length);
  if (UNLOCKED_FWRITE (&ie, 16, 1, wr->index_fp) != 1)
    {
      dsk_set_error (error, "error writing entry to index file: %s",
                     strerror (errno));
      return DSK_FALSE;
    }
  if (key_length != 0
   && UNLOCKED_FWRITE (key_data, key_length, 1, wr->heap_fp) != 1)
    {
      dsk_set_error (error, "error writing key to heap file: %s",
                     strerror (errno));
      return DSK_FALSE;
    }
  if (value_length != 0
   && UNLOCKED_FWRITE (value_data, value_length, 1, wr->heap_fp) != 1)
    {
      dsk_set_error (error, "error writing value to heap file: %s",
                     strerror (errno));
      return DSK_FALSE;
    }

  wr->heap_fp_offset += key_length + value_length;
  return DSK_TRUE;
}

static dsk_boolean 
table_file_trivial_writer__close  (DskTableFileWriter *writer,
                                   DskError          **error)
{
  TableFileTrivialWriter *wr = (TableFileTrivialWriter *) writer;
  if (fclose (wr->index_fp) != 0)
    {
      dsk_set_error (error, "error closing index file: %s",
                     strerror (errno));
      wr->index_fp = NULL;
      return DSK_FALSE;
    }
  wr->index_fp = NULL;
  if (fclose (wr->heap_fp) != 0)
    {
      dsk_set_error (error, "error closing heap file: %s",
                     strerror (errno));
      wr->heap_fp = NULL;
      return DSK_FALSE;
    }
  wr->heap_fp = NULL;
  return DSK_TRUE;
}

static void        
table_file_trivial_writer__destroy(DskTableFileWriter *writer)
{
  TableFileTrivialWriter *wr = (TableFileTrivialWriter *) writer;
  if (wr->index_fp)
    fclose (wr->index_fp);
  if (wr->heap_fp)
    fclose (wr->heap_fp);
  dsk_free (wr);
}

static DskTableFileWriter *
table_file_trivial__new_writer (DskTableFileInterface   *iface,
                                DskDir                  *dir,
                                const char              *base_filename,
                                DskError               **error)
{
  TableFileTrivialWriter wr =
  {
    {
      table_file_trivial_writer__write,
      NULL,                     /* no finish */
      table_file_trivial_writer__close,
      table_file_trivial_writer__destroy
    },
    NULL, NULL, 0ULL
  };
  int fd;
  DSK_UNUSED (iface);
  unsigned base_filename_len = strlen (base_filename);
  char *filename_buf = alloca (base_filename_len + 10);
  memcpy (filename_buf, base_filename, base_filename_len);
  strcpy (filename_buf + base_filename_len, ".index");
  fd = dsk_dir_openfd (dir, filename_buf,
                       DSK_DIR_OPENFD_WRITABLE|DSK_DIR_OPENFD_MAY_CREATE|DSK_DIR_OPENFD_TRUNCATE,
                       0666, error);
  if (fd < 0)
    return DSK_FALSE;
  wr.index_fp = fdopen (fd, "r+b");
  if (wr.index_fp == NULL)
    dsk_warning ("fdopen(%d) failed: %s", fd, strerror (errno));
  dsk_assert (wr.index_fp);
  strcpy (filename_buf + base_filename_len, ".heap");
  fd = dsk_dir_openfd (dir, filename_buf,
                       DSK_DIR_OPENFD_WRITABLE|DSK_DIR_OPENFD_MAY_CREATE|DSK_DIR_OPENFD_TRUNCATE,
                       0666, error);
  if (fd < 0)
    {
      fclose (wr.index_fp);
      return DSK_FALSE;
    }
  wr.heap_fp = fdopen (fd, "wb");
  dsk_assert (wr.heap_fp);

  return dsk_memdup (sizeof (wr), &wr);
}

/* === Reader === */
typedef struct _TableFileTrivialReader TableFileTrivialReader;
struct _TableFileTrivialReader
{
  DskTableReader base_instance;
  FILE *index_fp, *heap_fp;
  uint64_t next_heap_offset;
  unsigned slab_alloced;
  uint8_t *slab;
};

static dsk_boolean
read_next_index_entry (TableFileTrivialReader *reader,
                       DskError              **error)
{
  IndexEntry ie;
  size_t nread;
  unsigned kv_len;
  nread = UNLOCKED_FREAD (&ie, 1, sizeof (IndexEntry), reader->index_fp);
  if (nread == 0)
    {
      /* at EOF */
      reader->base_instance.at_eof = DSK_TRUE;
      return DSK_TRUE;
    }
  else if (nread < sizeof (IndexEntry))
    {
      if (ferror (reader->index_fp))
        {
          /* actual error */
          dsk_set_error (error, "error reading index file: %s",
                         strerror (errno));
        }
      else
        {
          /* file truncated */
          dsk_set_error (error,
                         "index file truncated (partial record encountered)");
        }
      return DSK_FALSE;
    }
  ie.heap_offset = UINT64_FROM_LE (ie.heap_offset);
  ie.key_length = UINT32_FROM_LE (ie.key_length);
  ie.value_length = UINT32_FROM_LE (ie.value_length);

  if (ie.heap_offset != reader->next_heap_offset)
    {
      dsk_set_error (error, "heap offset from index file was %"PRIu64" instead of expected %"PRIu64"",
                     ie.heap_offset, reader->next_heap_offset);
      return DSK_FALSE;
    }

  kv_len = ie.key_length + ie.value_length;
  if (reader->slab_alloced < kv_len)
    {
      unsigned new_alloced = reader->slab_alloced;
      if (new_alloced == 0)
        new_alloced = 16;
      while (new_alloced < kv_len)
        new_alloced *= 2;
      reader->slab = dsk_realloc (reader->slab, new_alloced);
      reader->slab_alloced = new_alloced;
    }
  if (kv_len != 0 && UNLOCKED_FREAD (reader->slab, kv_len, 1, reader->heap_fp) != 1)
    {
      dsk_set_error (error, "error reading key/value from heap file");
      return DSK_FALSE;
    }
  reader->base_instance.key_length = ie.key_length;
  reader->base_instance.value_length = ie.value_length;
  reader->base_instance.key_data = reader->slab;
  reader->base_instance.value_data = reader->slab + ie.key_length;
  reader->next_heap_offset += kv_len;
  return DSK_TRUE;
}

static dsk_boolean
table_file_trivial_reader__advance (DskTableReader     *reader,
                                    DskError          **error)
{
  return read_next_index_entry ((TableFileTrivialReader *) reader, error);
}

static void
table_file_trivial_reader__destroy (DskTableReader     *reader)
{
  TableFileTrivialReader *t = (TableFileTrivialReader *) reader;
  fclose (t->index_fp);
  fclose (t->heap_fp);
  dsk_free (t->slab);
  dsk_free (t);
}


/* Create a new trivial reader, at an arbitrary offset. */
static DskTableReader     *
new_reader  (DskDir                  *dir,
             const char              *base_filename,
             uint64_t                 index_offset,
             uint64_t                 heap_offset,
             DskError               **error)
{
  TableFileTrivialReader reader = {
    {
      DSK_FALSE, 0, 0, NULL, NULL,

      table_file_trivial_reader__advance,
      table_file_trivial_reader__destroy
    },
    NULL, NULL, 0ULL, 0, NULL
  };
  unsigned base_filename_len = strlen (base_filename);
  char *filename_buf = alloca (base_filename_len + 10);
  memcpy (filename_buf, base_filename, base_filename_len);
  strcpy (filename_buf + base_filename_len, ".index");
  int fd = dsk_dir_openfd (dir, filename_buf,
                                    DSK_DIR_OPENFD_READONLY, 0, error);
  if (fd < 0)
    return NULL;

  if (index_offset != 0ULL)
    {
      if (lseek (fd, index_offset, SEEK_SET) != (off_t) index_offset)
        {
          dsk_set_error (error, "error seeking in index file to offset %"PRIu64"",
                         index_offset);
          close (fd);
          return NULL;
        }
    }
  reader.index_fp = fdopen (fd, "rb");
  dsk_assert (reader.index_fp != NULL);
  strcpy (filename_buf + base_filename_len, ".heap");
  fd = dsk_dir_openfd (dir, filename_buf,
                                DSK_DIR_OPENFD_READONLY, 0, error);
  if (fd < 0)
    {
      fclose (reader.index_fp);
      return NULL;
    }
  if (heap_offset != 0ULL)
    {
      if (lseek (fd, heap_offset, SEEK_SET) != (off_t) heap_offset)
        {
          dsk_set_error (error, "error seeking in heap file to offset %"PRIu64"",
                         heap_offset);
          close (fd);
          fclose (reader.index_fp);
          return NULL;
        }
    }
  reader.heap_fp = fdopen (fd, "rb");
  dsk_assert (reader.heap_fp != NULL);

  if (!read_next_index_entry (&reader, error))
    {
      fclose (reader.index_fp);
      fclose (reader.heap_fp);
      return NULL;
    }
  return dsk_memdup (sizeof (reader), &reader);
}

static DskTableReader     *
table_file_trivial__new_reader (DskTableFileInterface   *iface,
                                DskDir                  *dir,
                                const char              *base_filename,
                                DskError               **error)
{
  DSK_UNUSED (iface);
  return new_reader (dir, base_filename, 0ULL, 0ULL, error);
}

/* === Seeker === */
typedef struct _TableFileTrivialSeeker TableFileTrivialSeeker;
struct _TableFileTrivialSeeker
{
  DskTableFileSeeker base_instance;
  int index_fd, heap_fd;
  uint8_t *slab;
  size_t slab_alloced;
  uint64_t count;

  /* required to create new readers */
  DskDir *dir;
  char *base_filename;
};

static inline dsk_boolean
check_index_entry_lengths (const IndexEntry *ie)
{
  return ie->key_length < (1<<30)
      && ie->value_length < (1<<30);
}

static inline dsk_boolean
read_index_entry (DskTableFileSeeker *seeker,
                  uint64_t            index,
                  IndexEntry         *out,
                  DskError          **error)
{
  ssize_t nread;
  TableFileTrivialSeeker *s = (TableFileTrivialSeeker *) seeker;
  IndexEntry ie;
  nread = pread (s->index_fd, &ie,
                                  SIZEOF_INDEX_ENTRY,
                                  index * SIZEOF_INDEX_ENTRY);
  if (nread < 0)
    {
      dsk_set_error (error, "error reading index entry %"PRIu64": %s",
                     index, strerror (errno));
      return DSK_FALSE;
    }
  if (nread < (int) SIZEOF_INDEX_ENTRY)
    {
      dsk_set_error (error, "too short reading index entry %"PRIu64"", index);
      return DSK_FALSE;
    }
  ie.heap_offset = UINT64_FROM_LE (ie.heap_offset);
  ie.key_length = UINT32_FROM_LE (ie.key_length);
  ie.value_length = UINT32_FROM_LE (ie.value_length);

  if (!check_index_entry_lengths (&ie))
    {
      dsk_set_error (error, "corrupted index entry %"PRIu64"", index);
      return DSK_FALSE;
    }
  *out = ie;
  return DSK_TRUE;
}

static void
seeker_ensure_slab_size (TableFileTrivialSeeker *s,
                         size_t                  size)
{
  size_t new_size;
  if (s->slab_alloced >= size)
    return;
  new_size = s->slab_alloced;
  if (new_size == 0)
    new_size = 16;
  else
    new_size *= 2;
  while (new_size < size)
    new_size *= 2;
  dsk_free (s->slab);
  s->slab = dsk_malloc (new_size);
  s->slab_alloced = new_size;
}
static dsk_boolean
run_cmp (TableFileTrivialSeeker *s,
         uint64_t               index,
         DskTableSeekerFindFunc func,
         void                  *func_data,
         int                   *cmp_rv_out,
         IndexEntry            *ie_out,
         DskError             **error)
{
  if (!read_index_entry ((DskTableFileSeeker*)s, index, ie_out, error))
    return DSK_FALSE;
  if (ie_out->key_length != 0)
    {
      ssize_t nread;
      seeker_ensure_slab_size (s, ie_out->key_length);
      nread = pread (s->heap_fd, s->slab,
                                      ie_out->key_length,
                                      ie_out->heap_offset);
      if (nread < (ssize_t) ie_out->key_length)
        {
          if (nread < 0)
            dsk_set_error (error, "error reading from heap file at offset %"PRIu64": %s",
                           ie_out->heap_offset, strerror (errno));
          else
            dsk_set_error (error, "premature EOF reading key from heap file");
          return DSK_FALSE;
        }
    }
  *cmp_rv_out = func (ie_out->key_length, s->slab, func_data);
  return DSK_TRUE;
}

static dsk_boolean
find_index (DskTableFileSeeker    *seeker,
            DskTableSeekerFindFunc func,
            void                  *func_data,
            DskTableFileFindMode   mode,
            uint64_t              *index_out,
            IndexEntry            *ie_out,
            DskError             **error)
{
  TableFileTrivialSeeker *s = (TableFileTrivialSeeker *) seeker;
  uint64_t start = 0, n = s->count;
  while (n > 0)
    {
      int cmp_rv;
      uint64_t mid = start + n / 2;
      IndexEntry ie;
      if (!run_cmp (s, mid, func, func_data, &cmp_rv, &ie, error))
        return DSK_FALSE;
      if (cmp_rv > 0)
        {
          /* mid is after func_data; hence the new interval
             should be [start,mid-1] */
          n = mid - start;
        }
      else if (cmp_rv < 0)
        {
          /* mid is before func_data; hence the new interval
             should be [start+1,end] (where end==start+n-1) */
          n = (start + n) - (mid + 1);
          start = mid + 1;
        }
      else /* cmp_rv == 0 */
        {
          switch (mode)
            {
            case DSK_TABLE_FILE_FIND_FIRST:
              {
                n = mid + 1 - start;

                /* bsearch, knowing that (start+n-1)==end is in
                   the range of elements to return. */
                while (n > 1)
                  {
                    uint64_t mid2 = start + n / 2;
                    IndexEntry ie2;
                    if (!run_cmp (s, mid2, func, func_data, &cmp_rv, &ie2, error))
                      return DSK_FALSE;
                    dsk_assert (cmp_rv <= 0);
                    if (cmp_rv == 0)
                      {
                        n = mid2 + 1 - start;
                        ie = ie2;
                      }
                    else
                      {
                        n = (start + n) - (mid2 + 1);
                        start = mid2 + 1;
                      }
                  }
                *index_out = start;
                *ie_out = ie;
                return DSK_TRUE;
              }

            case DSK_TABLE_FILE_FIND_ANY:
              {
                *index_out = mid;
                *ie_out = ie;
                return DSK_TRUE;
              }
            case DSK_TABLE_FILE_FIND_LAST:
              {
                n = start + n - mid;
                start = mid;

                /* bsearch, knowing that start is in
                   the range of elements to return. */
                while (n > 1)
                  {
                    uint64_t mid2 = start + n / 2;
                    IndexEntry ie2;
                    if (!run_cmp (s, mid2, func, func_data, &cmp_rv, &ie2, error))
                      return DSK_FALSE;
                    dsk_assert (cmp_rv <= 0);
                    if (cmp_rv == 0)
                      {
                        /* possible return value is the range
                           starting at mid2. */
                        n = start + n - mid2;
                        start = mid2;
                        ie = ie2;
                      }
                    else
                      {
                        n = mid2 - start;
                      }
                  }
                *index_out = start;
                *ie_out = ie;
                return DSK_TRUE;
              }
            }
        }
    }

  /* not found. */
  return DSK_FALSE;
}

static dsk_boolean 
table_file_trivial_seeker__find  (DskTableFileSeeker    *seeker,
                                  DskTableSeekerFindFunc func,
                                  void                  *func_data,
                                  DskTableFileFindMode   mode,
                                  unsigned              *key_len_out,
                                  const uint8_t        **key_data_out,
                                  unsigned              *value_len_out,
                                  const uint8_t        **value_data_out,
                                  DskError             **error)
{
  uint64_t index;
  IndexEntry ie;
  TableFileTrivialSeeker *s = (TableFileTrivialSeeker *) seeker;
  if (!find_index (seeker, func, func_data, mode, &index, &ie, error))
    return DSK_FALSE;

  if (key_len_out || key_data_out || value_len_out || value_data_out)
    {
      /* load key/value */
      unsigned kv_len = ie.key_length + ie.value_length;
      seeker_ensure_slab_size (s, kv_len);
      if (kv_len > 0
       && (key_data_out != NULL || value_data_out != NULL))
        {
          ssize_t nread = pread (s->heap_fd, s->slab,
                                                  kv_len, ie.heap_offset);
          if (nread < (ssize_t) kv_len)
            {
              if (nread < 0)
                dsk_set_error (error, "error reading from heap file at offset %"PRIu64": %s",
                               ie.heap_offset, strerror (errno));
              else
                dsk_set_error (error, "premature EOF reading key/value from heap file");
              return DSK_FALSE;
            }
        }
      if (key_len_out != NULL)
        *key_len_out = ie.key_length;
      if (value_len_out != NULL)
        *value_len_out = ie.value_length;
      if (key_data_out != NULL)
        *key_data_out = s->slab;
      if (value_data_out != NULL)
        *value_data_out = s->slab + ie.key_length;
    }
  return DSK_TRUE;
}


static DskTableReader     * 
table_file_trivial_seeker__find_reader(DskTableFileSeeker    *seeker,
                                       DskTableSeekerFindFunc func,
                                       void                  *func_data,
                                       DskError             **error)
{
  uint64_t index;
  IndexEntry ie;
  TableFileTrivialSeeker *s = (TableFileTrivialSeeker *) seeker;
  if (!find_index (seeker, func, func_data, 
                   DSK_TABLE_FILE_FIND_FIRST,
                   &index, &ie, error))
    return NULL;
  return new_reader (s->dir,
                     s->base_filename, index * SIZEOF_INDEX_ENTRY,
                     ie.heap_offset, error);
}

static dsk_boolean 
table_file_trivial_seeker__index (DskTableFileSeeker    *seeker,
                                  uint64_t               index,
                                  unsigned              *key_len_out,
                                  const void           **key_data_out,
                                  unsigned              *value_len_out,
                                  const void           **value_data_out,
                                  DskError             **error)
{
  TableFileTrivialSeeker *s = (TableFileTrivialSeeker *) seeker;
  IndexEntry ie;
  unsigned kv_len;
  if (!read_index_entry (seeker, index, &ie, error))
    return DSK_FALSE;
  kv_len = ie.key_length + ie.value_length;
  if (kv_len > 0)
    {
      ssize_t nread;
      seeker_ensure_slab_size (s, kv_len);
      nread = pread (s->heap_fd, s->slab,
                                      kv_len, ie.heap_offset);
      if (nread < 0)
        {
          dsk_set_error (error, "error reading heap entry %"PRIu64": %s",
                         index, strerror (errno));
          return DSK_FALSE;
        }
      if (nread < (int) kv_len)
        {
          dsk_set_error (error, "too short reading heap entry %"PRIu64" (got %u of %u bytes)",
                         index, (unsigned) nread, kv_len);
          return DSK_FALSE;
        }
    }
  if (key_len_out != NULL)
    *key_len_out = ie.key_length;
  if (key_data_out != NULL)
    *key_data_out = s->slab;
  if (value_len_out != NULL)
    *value_len_out = ie.value_length;
  if (value_data_out != NULL)
    *value_data_out = s->slab + ie.key_length;
  return DSK_TRUE;
}

static DskTableReader     * 
table_file_trivial_seeker__index_reader(DskTableFileSeeker    *seeker,
                                        uint64_t               index,
                                        DskError             **error)
{
  TableFileTrivialSeeker *s = (TableFileTrivialSeeker *) seeker;
  IndexEntry ie;
  if (!read_index_entry (seeker, index, &ie, error))
    return DSK_FALSE;
  return new_reader (s->dir, s->base_filename, index * SIZEOF_INDEX_ENTRY,
                     ie.heap_offset, error);
}


static void         
table_file_trivial_seeker__destroy  (DskTableFileSeeker    *seeker)
{
  TableFileTrivialSeeker *s = (TableFileTrivialSeeker *) seeker;
  dsk_free (s->slab);
  close (s->index_fd);
  close (s->heap_fd);
  dsk_free (s);
}

static DskTableFileSeeker *
table_file_trivial__new_seeker (DskTableFileInterface   *iface,
                                DskDir                  *dir,
                                const char              *base_filename,
                                DskError               **error)
{
  TableFileTrivialSeeker s = {
    { table_file_trivial_seeker__find,
      table_file_trivial_seeker__find_reader,
      table_file_trivial_seeker__index,
      table_file_trivial_seeker__index_reader,
      table_file_trivial_seeker__destroy },
    -1, -1,             /* fds */
    NULL, 0,            /* slab */
    0ULL,               /* count */
    dir,
    NULL                /* base_filename */
  };
  struct stat stat_buf;
  TableFileTrivialSeeker *rv;
  DSK_UNUSED (iface);
  unsigned base_filename_len = strlen (base_filename);
  char *filename_buf = alloca (base_filename_len + 10);
  memcpy (filename_buf, base_filename, base_filename_len);
  strcpy (filename_buf + base_filename_len, ".index");
  s.index_fd = dsk_dir_openfd (dir, filename_buf,
                               DSK_DIR_OPENFD_READONLY, 0, error);
  if (s.index_fd < 0)
    return NULL;
  if (fstat (s.index_fd, &stat_buf) < 0)
    {
      dsk_set_error (error, "could not stat index file to find its size");
      close (s.index_fd);
      return NULL;
    }
  s.count = stat_buf.st_size / SIZEOF_INDEX_ENTRY;

  strcpy (filename_buf + base_filename_len, ".heap");
  s.heap_fd = dsk_dir_openfd (dir, filename_buf,
                              DSK_DIR_OPENFD_READONLY, 0, error);
  if (s.heap_fd < 0)
    {
      close (s.index_fd);
      return NULL;
    }

  rv = dsk_malloc (sizeof (TableFileTrivialSeeker)
                    + strlen (base_filename) + 1
                    + strlen (base_filename) + 1);
  *rv = s;
  rv->base_filename = (char*) (rv + 1);
  strcpy (rv->base_filename, base_filename);
  return (DskTableFileSeeker *) rv;
}

static dsk_boolean
table_file_trivial__delete_file (DskTableFileInterface *iface,
                                 DskDir                  *dir,
                                 const char              *base_filename,
                                 DskError               **error)
{
  static const char *suffixes[] = { "heap", "index" };
  unsigned i;
  unsigned base_len = strlen (base_filename);
  char *fname = dsk_malloc (base_len + 10);
  DSK_UNUSED (iface);
  memcpy (fname, base_filename, base_len);
  fname[base_len] = '.';
  for (i = 0; i < DSK_N_ELEMENTS (suffixes); i++)
    {
      strcpy (fname + base_len + 1, suffixes[i]);
      if (!dsk_dir_rm (dir, fname, 0, NULL, error))
        {
          dsk_free (fname);
          return DSK_FALSE;
        }
    }
  dsk_free (fname);
  return DSK_TRUE;
}

/* No destructor required for the static interface */
#define table_file_trivial__destroy  NULL

DskTableFileInterface dsk_table_file_interface_trivial =
{
  table_file_trivial__new_writer,
  table_file_trivial__new_reader,
  table_file_trivial__new_seeker,
  table_file_trivial__delete_file,
  table_file_trivial__destroy,
};
