#include "dsk.h"
#include "dsk-table-helper.h"


/* === Generic TableFileInterface code === */


/* === Standard TableFileInterface implementation === */

/* --- helper code (b128) --- */

#define B128_UINT32_MAX_SIZE            5
#define B128_UINT64_MAX_SIZE            9

static inline unsigned
encode_uint32_b128 (uint32_t value,
                    uint8_t *data)
{
  unsigned len = 0;
  if (*value >= 128)
    {
      *data++ = value | 128;
      value >>= 7;
      len++;
      if (*value >= 128)
        {
          *data++ = value | 128;
          value >>= 7;
          len++;
          if (*value >= 128)
            {
              *data++ = value | 128;
              value >>= 7;
              len++;
              if (*value >= 128)
                {
                  *data++ = value | 128;
                  value >>= 7;
                  len++;
                }
            }
        }
    }

  *data = value;
  return len + 1;
}
static inline unsigned
encode_uint64_b128 (uint64_t value,
                    uint8_t *data)
{
  unsigned len = 0;
  if (*value >= 128)
    {
      *data++ = value | 128;
      value >>= 7;
      len++;
      if (*value >= 128)
        {
          *data++ = value | 128;
          value >>= 7;
          len++;
          if (*value >= 128)
            {
              *data++ = value | 128;
              value >>= 7;
              len++;
              if (*value >= 128)
                {
                  *data++ = value | 128;
                  value >>= 7;
                  len++;
                  if (*value >= 128)
                    {
                      *data++ = value | 128;
                      value >>= 7;
                      len++;
                      if (*value >= 128)
                        {
                          *data++ = value | 128;
                          value >>= 7;
                          len++;
                          if (*value >= 128)
                            {
                              *data++ = value | 128;
                              value >>= 7;
                              len++;
                              if (*value >= 128)
                                {
                                  *data++ = value | 128;
                                  value >>= 7;
                                  len++;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
  *data++ = value;
  return len + 1;
}
static inline unsigned
decode_uint32_b128 (const uint8_t *data,
                    uint32_t *value_out)
{
  unsigned len = 0;
  unsigned shift = 0;
  while (shift < 4*7 && (*data & 0x80))
    {
      value |= (*data & 0x7f) << shift;
      len++;
      shift += 7;
      data++;
    }
  value |= (uint32_t) (*data) << shift;
  *value_out = value;
  return len + 1;
}
static inline unsigned
decode_uint64_b128 (const uint8_t *data,
                    uint32_t *value_out)
{
  unsigned len = 0;
  unsigned shift = 0;
  uint64_t value = 0;
  while (shift < 8*7 && (*data & 0x80))
    {
      value |= (uint64_t)(*data & 0x7f) << shift;
      len++;
      shift += 7;
      data++;
    }
  value |= (uint64_t) (*data) << shift;
  *value_out = value;
  return len + 1;
}


/* --- The TableFileWriter interface --- */

struct _WriterIndexLevel
{
  FILE *index_out;
  FILE *index_heap_out;

  /* For a level-0 index, we need:
   *     - key-length of first key                         (max 4 bytes)
   *     - key-offset in index-heap file                   (max 8 bytes)
   *     - compressed-data-length in compressed-heap file  (max 5 bytes)
   *     - compressed-data-offset in compressed-heap file  (max 8 bytes)
   *   min..max = 8 .. 25; use multiples of 4 bytes.
   * For a non-level-0 index, we need:
   *     - key-length of first key                         (max 4 bytes)
   *     - key-offset in index-heap file                   (max 8 bytes)
   *   min..max == 2 .. 12; use multiples of 2 bytes.
   */
  uint64_t index_entry_size_to_count[6];

  /* index in index_entry_size_to_count[] array for current size. */
  unsigned size_index;

  /* maximum size of index-entry's */
  unsigned max_size;
};
#define IE_SIZE_TO_COUNT_INDEX__LEVEL0(size)        ((size) < 8 ? 0 : ((size)-8+3)/4)
#define IE_SIZE_TO_COUNT_INDEX__LEVELNON0(size)     (((size)-2+1)/2)
#define IE_COUNT_INDEX_TO_MAX_SIZE__LEVEL0(index)   ((index) * 4 + 8 + 3)
#define IE_COUNT_INDEX_TO_MAX_SIZE__LEVELNON0(size) ((index) * 2 + 2 + 1)

typedef struct _TableFileWriter TableFileWriter;
struct _TableFileWriter
{
  DskTableFileWriter base;

  /* slab of data to compress and write */
  DskTableBuffer incoming;
  DskTableBuffer prefix_buffer;
  void (*append_to_incoming) (TableFileWriter    *writer,
                              unsigned            key_length,
                              const uint8_t      *key_data,
                              unsigned            value_length,
                              const uint8_t      *value_data);
  unsigned n_entries_in_incoming;
  DskTableFileCompressor *compressor;

  int compressed_heap_fd;
  DskBuffer compressed_heap_buffer;
  uint64_t compressed_heap_offset;

  unsigned n_index_levels;
  WriterIndexLevel *index_levels;
};

static void 
append_to_incoming__prefix_compressed (TableFileWriter    *writer,
                                       unsigned            key_length,
                                       const uint8_t      *key_data,
                                       unsigned            value_length,
                                       const uint8_t      *value_data)
{
  ...
}

static void 
append_to_incoming__raw               (TableFileWriter    *writer,
                                       unsigned            key_length,
                                       const uint8_t      *key_data,
                                       unsigned            value_length,
                                       const uint8_t      *value_data)
{
  ...
}

static dsk_boolean
write_index0_entry           (TableFileWriter *writer,
                              unsigned         first_key_length,
                              uint64_t         first_key_offset,
                              unsigned         compressed_length,
                              uint64_t         compressed_heap_offset,
                              DskError       **error)
{
  /* pack index0 entry */
  uint8_t packed_entry[B128_UINT32_MAX_SIZE + B128_UINT64_MAX_SIZE
                     + B128_UINT32_MAX_SIZE + 8];
  unsigned packed_entry_len = encode_uint32_b128 (packed_entry, first_key_length);
  packed_entry_len += encode_uint64_b128 (packed_entry + packed_entry_len,
                                          first_key_offset);
  packed_entry_len += encode_uint32_b128 (packed_entry + packed_entry_len,
                                          compressed_length);
  packed_entry_len += encode_uint64_le (packed_entry + packed_entry_len,
                                        compressed_heap_offset);

  /* is it bigger than the current size? */
  level0 = w->index_levels + 0;
  index = IE_SIZE_TO_COUNT_INDEX__LEVEL0 (packed_entry_len);
  if (level0->size_index < index)
    {
      level0->max_size = IE_COUNT_INDEX_TO_MAX_SIZE__LEVEL0 (index);
      level0->size_index = index;
    }

  /* increase count of this size of index-entry */
  level0->index_entry_size_to_count[level0->size_index] += 1;

  if (level0->max_size > packed_entry_len)
    {
      /* zero pad */
      unsigned pad_len = level0->max_size - packed_entry_len;
      memset (packed_entry + packed_entry_len, 0, pad_len);
      packed_entry_len = level0->max_size;
    }

  /* write packed (and padded) entry */
  if (FWRITE (packed_entry, packed_entry_len, 1, level0->index_fp) != 1)
    {
      dsk_set_error (error, "error writing to level0 index");
      return DSK_FALSE;
    }

  return DSK_TRUE;
}

static dsk_boolean
write_indexnon0_entry        (TableFileWriter *writer,
                              unsigned         first_key_length,
                              uint64_t         first_key_offset,
                              DskError       **error)
{
  ....
}

/* hackery:  for the "write" function we use a virtual function
   especially for the first invocation, since that's when we handle
   so many unusual edge-cases that it's difficult to code them together.

   at the end of that first write call, we set the virtual function to
   the non-first write implementation. */
static dsk_boolean table_file_writer__write  (DskTableFileWriter *writer,
                                              unsigned            key_length,
                                              const uint8_t      *key_data,
                                              unsigned            value_length,
                                              const uint8_t      *value_data,
                                              DskError          **error);


/* special "write" method for first invocation.
   see "hackery" comment above. */
static dsk_boolean
table_file_writer__first_write  (DskTableFileWriter *writer,
                                 unsigned            key_length,
                                 const uint8_t      *key_data,
                                 unsigned            value_length,
                                 const uint8_t      *value_data,
                                 DskError          **error)
{
  /* Handle prefix-compression to the "incoming" buffer. */
  w->append_to_incoming (w, key_length, key_data, value_length, value_data);
  w->n_entries_in_incoming = 1;

  /* write key and key_length to level0 index */
  if (key_length != 0
   && FWRITE (key_data, key_length, 1, w->index_levels[0].index_heap_fp) != 1)
    {
      dsk_set_error (error, "error writing initial key to index level 0");
      return DSK_FALSE;
    }

  /* write key/key_length to potential_new_level{_keys} */
  w->n_potential_new_level_keys = 1;
  w->potential_new_level[0] = key_length;
  dsk_buffer_append (&w->potential_new_level_keys, key_length, key_data);

  /* no longer use custom first-element "write" method, so hackily switch
     over to the implementation for non-first elements. */
  writer->write = table_file_writer__write;

  return DSK_TRUE;
}
static dsk_boolean
do_compress (TableFileWriter *w,
             unsigned        *vli_len_out,
             unsigned        *outgoing_len_out,
             DskError       **error)
{
  /* data (compressed if possible) going out to the heap file. */
  unsigned outgoing_len;
  const uint8_t *outgoing_data;

  /* uncompressed length, or 0 when compression is disabled. */
  unsigned vli_len;        

  if (w->compressor == NULL || w->incoming.size == 0)
    {
      outgoing_len = w->incoming.size;
      vli_header = 0;
      outgoing_data = w->incoming.data;
    }
  else
    {
      /* The compressed data should fit in less space,
         or else we really should use the uncompressed version,
         which we have to support anyway. */
      dsk_table_buffer_set_size (&w->compression_buffer, w->incoming.size);

      /* Run the compressor.  If the compressed data doesn't fit
         in the size of the uncompressed data, just use the
         uncompressed data. */
      if (!w->compressor->compress (w->compressor,
                                    w->incoming.size, w->incoming.data,
                                    &w->compression_buffer.size,
                                    w->compression_buffer.data))
        {
          /* use uncompressed data */
          vli_header = 0;
          outgoing_len = w->incoming.size;
          outgoing_data = w->incoming.data;
        }
      else
        {
          /* use the compressed data */
          vli_header = w->incoming.size;
          outgoing_len = w->compression_buffer.size;
          outgoing_data = w->compression_buffer.data;
        }
    }
  heap_offset = w->compressed_heap_offset;
  vli_len = encode_uint32_b128 (vli, outgoing_len);
  dsk_buffer_append (&w->compressed_heap_buffer, vli_len, vli);
  dsk_buffer_append (&w->compressed_heap_buffer, outgoing_len, outgoing_data);
  while (w->compressed_heap_buffer > MAX_COMPRESSED_HEAP_BUFFER)
    {
      if (dsk_buffer_writev (&w->compressed_heap_buffer, w->compressed_heap_fd) < 0)
        {
          if (errno == EINTR)
            continue;
          ...
        }
    }
  w->compressed_heap_offset += vli_len + outgoing_len;
  *vli_len_out = vli_len;
  *outgoing_len_out = outgoing_len;
  return DSK_TRUE;
}

static dsk_boolean
table_file_writer__write  (DskTableFileWriter *writer,
                           unsigned            key_length,
                           const uint8_t      *key_data,
                           unsigned            value_length,
                           const uint8_t      *value_data,
                           DskError          **error)
{
  TableFileWriter *w = (TableFileWriter *) writer;

  /* Handle prefix-compression to the "incoming" buffer. */
  w->append_to_incoming (w, key_length, key_data, value_length, value_data);
  w->n_entries_in_incoming += 1;
  if (w->n_entries_in_incoming < w->n_compress)
    return DSK_TRUE;

  /* --- perform compression, write to file --- */

  /* data (compressed if possible) going out to the heap file. */
  unsigned outgoing_len;

  /* uncompressed length, or 0 when compression is disabled. */
  unsigned vli_len;        

  if (!do_compress (w, &vli_len, &outgoing_len, error))
    return DSK_FALSE;

  /* write to index file, possible propagating up to higher indexes */
  if (!write_index0_entry (w,
                           w->index0_first_key.key_length, key_heap_offset,
                           vli_len + outgoing_len, heap_offset,
                           error))
    return DSK_FALSE;
  if (++w->compressed_level_counter < w->index_ratio)
    goto done_handling_index_levels;
  w->compressed_level_counter = 0;
  for (ilevel = 1; ilevel < w->n_index_levels; ilevel++)
    {
      WriterIndexLevel *level = w->index_levels + ilevel;

      if (!write_indexnon0_entry (w, level,
                                  key_length, level->key_heap_offset,
                                  error))
        return DSK_FALSE;

      if (key_length != 0
       && FWRITE (key_data, key_length, 1, level->key_heap_fp) != 1)
        {
          dsk_set_error (error, "error writing key to index level %u", ilevel);
          return DSK_FALSE;
        }
      level->key_heap_offset += key_length;

      if (++level->next_level_cycle == w->index_ratio)
        /* advance to the next level */
        level->next_level_cycle = 0;
      else
        goto done_handling_index_levels;
    }
  /* append to potential new index level */
  w->potential_new_level[w->n_potential_new_level] = key_length;
  dsk_buffer_append (w->potential_new_level_keys, key_length, key_data);
  w->n_potential_new_level += 1;

  /* if we have index_ratio/2 entries, create the new index level */
  if (w->n_potential_new_level == w->index_ratio / 2)
    {
      WriterIndexLevel *new_level;
      char suffix[64];
      snprintf (suffix, 64, "K%u", w->n_index_levels);
      kh_fd = dsk_table_helper_openat (w->location,
                                       w->base_filename, suffix,
                                       O_CREAT|O_TRUNC, 0666,
                                       error);
      if (kh_fd < 0)
        {
          dsk_add_error_prefix (error, "creating table's key file");
          return DSK_FALSE;
        }
      suffix[0] = 'I';
      ki_fd = dsk_table_helper_openat (w->location,
                                       w->base_filename, suffix,
                                       O_CREAT|O_TRUNC, 0666,
                                       error);
      if (ki_fd < 0)
        {
          dsk_add_error_prefix (error, "creating table's index file");
          return DSK_FALSE;
        }
      w->index_levels = dsk_realloc (w->index_levels,
                                     (w->n_index_levels+1) * sizeof (WriterIndexLevel));
      new_level = w->index_levels + w->n_index_levels++;
      new_level->key_heap_fp = fdopen (kh_fd, "wb");
      dsk_assert_runtime (new_level->key_heap_fp != NULL);
      new_level->index_fp = fdopen (ki_fd, "wb");
      dsk_assert_runtime (new_level->index_fp != NULL);

      /* dump key-heap */
      for (frag = w->potential_new_level_keys.first_frag;
           frag != NULL;
           frag = frag->next)
        {
          if (frag->buf_length != 0
           && FWRITE (frag->buf + frag->buf_start,
                      frag->buf_length,
                      1, new_level->key_heap_fp) != 1)
            {
              dsk_set_error ("error writing initial key-data for new index level");
              return DSK_FALSE;
            }
        }
      new_level->key_heap_offset = w->potential_new_level_keys.size;

      /* dump key-index-entries */
      for (i = 0; i < w->n_potential_new_level_keys; i++)
        {
          unsigned key_length = w->potential_new_level[i];
          if (!write_indexnon0_entry (w, new_level,
                                      key_length, key_offset, error))
            return DSK_FALSE;
          key_offset += key_length;
        }

      w->n_potential_new_level_keys = 1;
      dsk_buffer_truncate (&w->potential_new_level_keys,
                           w->potential_new_level[0]);
    }
  
  /* reset pre-compressed entries */
  w->n_entries_in_incoming = 0;
  w->incoming.size = w->incoming.prefix_buffer = 0;

  return DSK_TRUE;
}

static dsk_boolean
table_file_writer__close  (DskTableFileWriter *writer,
                           DskError          **error)
{
  TableFileWriter *w = (TableFileWriter *) writer;

  w->closed = DSK_TRUE;

  if (w->incoming.size > 0)
    {
      /* compress remaining data, write index entry/entries */
      unsigned outgoing_len;
      unsigned vli_len;        
      if (!do_compress (w, &vli_len, &outgoing_len, error))
        return DSK_FALSE;
      if (!write_index0_entry (w,
                               w->index0_first_key.key_length, key_heap_offset,
                               vli_len + outgoing_len, heap_offset,
                               error))
        return DSK_FALSE;

      /* flush out any higher level indices */
      ...
    }

  /* write a sentinel element to the index level 0. */
  ...

  /* close all index-levels (close index and heap files,
     and write the level-sizes arrays to disk) */
  ...

  /* close the compressed-heap */
  ...

  return DSK_TRUE;
}

static void
table_file_writer__destroy(DskTableFileWriter *writer)
{
  TableFileWriter *w = (TableFileWriter *) writer;
  if (!w->closed)
    {
      DskError *error = NULL;
      if (!table_file_writer__close (writer, &error))
        dsk_die ("error closing table-file-writer on destroy: %s",
                 error->message);
    }

  /* clean up memory */
  dsk_free (w->index_levels);
  dsk_free (w);
}

static TableFileWriter *
table_file_writer_new (DskTableLocation        *location
                       const char              *base_filename,
                       DskTableFileCompressor *compressor,
                       dsk_boolean              prefix_compress,
                       unsigned                 n_compress,
                       unsigned                 index_ratio,
                       DskError                **error)
{
  unsigned base_filename_len = strlen (base_filename);
  TableFileWriter *w = dsk_malloc (sizeof (TableFileWriter)
                                   + base_filename_len
                                   + FILENAME_EXTENSION_MAX
                                   + 1);
  w->location = location;
  w->base_filename = memcpy (w + 1, base_filename, base_filename_len);
  dsk_table_buffer_init (&w->incoming);
  dsk_table_buffer_init (&w->prefix_buffer);
  w->append_to_incoming = prefix_compress ? append_to_incoming__prefix_compressed
                                          : append_to_incoming__raw;
  w->n_entries_in_incoming = 0;
  w->compressor = compressor;

  w->compressed_heap_fd = dsk_table_helper_openat (w->location,
                                                ..., ...,
                                                O_CREAT|O_TRUNC|O_WRONLY,
                                                0666, error);
  if (w->compressed_heap_fd < 0)
    {
      ...
    }
  dsk_buffer_init (&w->compressed_heap_buffer);
  w->compressed_heap_offset = 0ULL;

  w->n_index_levels = 1;
  w->index_levels = DSK_NEW (WriterIndexLevel);
  w->index_levels[0]...
  return w;
}


/* --- TableFileInterface interface constructor itself --- */
typedef struct _TableFileInterface TableFileInterface;
struct _TableFileInterface
{
  DskTableFileInterface base_iface;
  DskTableFileCompressor *compressor;
  unsigned n_compress;
  unsigned index_ratio;
};

static DskTableFileWriter *
file_interface__new_writer  (DskTableFileInterface   *iface,
                             DskTableLocation        *location,
                             const char              *base_filename,
                             DskError               **error)
{
  return (DskTableFileWriter *) table_file_writer_new (location, base_filename,
                                                       I->prefix_compress,
                                                       I->n_compress,
                                                       I->index_ratio,
                                                       error);
}
static DskTableReader *
file_interface__new_reader  (DskTableFileInterface   *iface,
                             const char              *openat_dir,
                             int                      openat_fd,
                             const char              *base_filename,
                             DskError               **error)
{
  ...
}
static DskTableFileSeeker *
file_interface__new_seeker  (DskTableFileInterface   *iface,
                             const char              *openat_dir,
                             int                      openat_fd,
                             const char              *base_filename,
                             DskError               **error)
{
  ...
}
static dsk_boolean
file_interface__delete_file (DskTableFileInterface   *iface,
                             const char              *openat_dir,
                             int                      openat_fd,
                             const char              *base_filename,
                             DskError               **error)
{
  ...
}

static void
file_interface__destroy     (DskTableFileInterface   *iface)
{
  dsk_free (iface);
}


DskTableFileInterface *
dsk_table_file_interface_new (DskTableFileNewOptions *options)
{
  TableFileInterface *iface = DSK_NEW (TableFileInterface);
  dsk_assert (options->index_ratio >= 4);
  iface->base_iface.new_reader = file_interface__new_reader;
  iface->base_iface.new_writer = file_interface__new_writer;
  iface->base_iface.new_seeker = file_interface__new_seeker;
  iface->base_iface.delete_file = file_interface__delete_file;
  iface->base_iface.destroy = file_interface__destroy;
  iface->compressor = compressor;
  iface->n_compress = options->n_compress;
  iface->index_ratio = options->index_ratio;
  iface->prefix_compress = options->prefix_compress;
  return iface;
}
