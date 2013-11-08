#include "dsk.h"


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

typedef struct _WriterFile WriterFile;
struct _WriterFile
{
  dsk_boolean memory_mapped;
  int fd;
  uint8_t *buf;
  unsigned buf_at;
  unsigned buf_length;
  uint64_t buf_offset;
};

static dsk_boolean
writer_file_new (DskDir *dir,
                 const char *filename,
                 DskDirOpenfdFlags open_flags,
                 dsk_boolean memory_mapped,
                 unsigned buf_length,
                 WriterFile *out,
                 DskError **error)
{
  int fd = dsk_dir_openfd (dir, filename,
                           open_flags, 0666,
                           error);
  if (fd < 0)
    {
      dsk_set_error ("error creating %s/%s: %s",
                     dsk_dir_get_str (dir),
                     filename,
                     strerror (errno));
      return DSK_FALSE;
    }
  out->memory_mapped = memory_mapped;
  out->fd = fd;
  out->buf_length = buf_length;
  out->buf_offset = 0;
  out->buf_at = 0;
  if (memory_mapped)
    {
      /* if the file is too short, ftruncate it */
      if ((open_flags & DSK_DIR_OPENFD_TRUNCATE)
       || fd_get_file_size (fd) < (uint64_t) buf_length)
        {
          if (ftruncate (fd, buf_length) < 0)
            {
              dsk_set_error (error, "error resizing file: %s",
                             strerror (errno));
              return DSK_FALSE;
            }
        }
      void *ptr = mmap (NULL,
                        buf_length,
                        PROT_READ|PROT_WRITE,
                        MAP_SHARED,
                        fd,
                        0);
      if (ptr == MAP_FAILED) 
        {
          dsk_warning ("mmap failed: %s", strerror (errno));
          out->memory_mapped = DSK_FALSE;
          out->buf = dsk_malloc (buf_length);
        }
      else
        out->buf = ptr;
    }
  else
    {
      out->buf = dsk_malloc (buf_length);
    }
  return DSK_TRUE;
}

static dsk_boolean
do_writen (int fd,
           unsigned length,
           const uint8_t *data,
           DskError **error)
{
  /* flush to fd, if not mmapped */
  unsigned n_written = 0;
  while (n_written < writer->buf_length)
    {
      int nw = write (writer->fd,
                      writer->buf + n_written,
                      writer->buf_length - n_written);
      if (nw < 0)
        {
          if (errno == EINTR)
            continue;
          dsk_set_error (error, "error writing: %s", strerror (errno));
          return DSK_FALSE;
        }
      dsk_assert (nw > 0);
      n_written += nw;
    }
  return DSK_TRUE;
}

static dsk_boolean
writer_file_write (WriterFile    *writer,
                   unsigned       length,
                   const uint8_t *data,
                   DskError     **error)
{
  while (length > 0)
    {
      unsigned buf_write = DSK_MIN (length, out->buf_length - out->at);
      memcpy (writer->buf + writer->at, data, buf_write);
      length -= buf_write;
      writer->at += writer->buf_write;
      if (writer->at == writer->buf_length)
        {
          if (!writer->memory_mapped)
            {
              if (!do_writen (writer->fd, writer->buf_length, writer->buf, error))
                return DSK_FALSE;
            }

          writer->at = 0;
          writer->buf_offset += writer->buf_length;
          if (!writer->memory_mapped)
            writer->file_size = DSK_MAX (writer->file_size, writer->buf_offset);

          if (length >= writer->buf_length)
            {
              /* call write() directly on multiples of buf_length */
              unsigned w = length - length % writer->buf_length;
              if (!do_writen (writer->fd, w, data, error))
                return DSK_FALSE;

              writer->buf_offset += w;
              length -= w;
              data += w;
            }

          if (writer->memory_mapped)
            {
              uint64_t min_size = writer->buf_offset + writer->buf_length;
              if (munmap (writer->buf, writer->buf_length) < 0)
                dsk_die ("munmap failed: %s", strerror (errno));
              if (writer->file_size < min_size)
                {
                  if (ftruncate (writer->fd, min_size) < 0)
                    {
                      dsk_set_error (error, "ftruncate failed: %s",
                                     strerror (errno));
                      return DSK_FALSE;
                    }
                  writer->file_size = min_size;
                }
              void *ptr = mmap (NULL,
                                writer->buf_length,
                                PROT_READ|PROT_WRITE,
                                MAP_SHARED,
                                fd,
                                writer->buf_offset);
              if (ptr == MAP_FAILED)
                {
                  writer->buf = NULL;
                  dsk_set_error (error, "mmapped failed: %s",
                                 strerror (errno));
                  return DSK_FALSE;
                }
              writer->buf = ptr;
            }
        }
    }
  return DSK_TRUE;
}

struct _WriterIndexLevel
{
  WriterFile index_file;
  WriterFile index_heap_file;

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
  unsigned max_common = DSK_MIN (key_length, prefix_buffer.length);
  unsigned common;
  uint8 header_buf[10];
  unsigned header_length = 0;
  for (common = 0; common < max_common; common++)
    if (key_data[common] != prefix_buffer.data[common])
      break;

  unsigned header_length1 = encode_uint32_b128 (common, header_buf);
  unsigned header_length2 = encode_uint32_b128 (key_length - common, header_buf + header_length1);
  unsigned header_length = header_length1 + header_length2;

  uint8_t value_header[5];
  uint8_t vheader_length = encode_uint32_b128 (value_length, value_header);

  unsigned entry_size = header_length
                      + (key_length - common)
                      + vheader_length
                      + value_length;

  unsigned old_length = writer->incoming.length;
  dsk_table_buffer_set_size (&writer->incoming, old_length + entry_size);
  uint8_t *at = writer->incoming.data + old_length;
  memcpy (at, header_buf, header_length);
  at += header_length;
  memcpy (at, key_data + common, key_length - common);
  at += key_length - common;
  memcpy (at, value_header, vheader_length);
  at += vheader_length;
  memcpy (at, value_data, value_length);
}

static void 
append_to_incoming__raw               (TableFileWriter    *writer,
                                       unsigned            key_length,
                                       const uint8_t      *key_data,
                                       unsigned            value_length,
                                       const uint8_t      *value_data)
{
  uint8_t kheader_buf[5];
  uint8_t vheader_buf[5];
  unsigned kheader_length = encode_uint32_b128 (key_length, kheader_buf);
  unsigned vheader_length = encode_uint32_b128 (value_length, vheader_buf);
  unsigned entry_size = kheader_length + key_length
                      + vheader_length + value_length;
  unsigned old_length = writer->incoming.length;
  dsk_table_buffer_set_size (&writer->incoming, old_length + entry_size);
  uint8_t *at = writer->incoming.data + old_length;
  memcpy (at, kheader_buf, kheader_length);
  at += kheader_length;
  memcpy (at, key_data, key_length);
  at += key_length;
  memcpy (at, vheader_buf, vheader_length);
  at += vheader_length;
  memcpy (at, value_data, value_length);
}

static dsk_boolean
write_index0_entry           (TableFileWriter *writer,
                              uint64_t         first_key_offset,
                              unsigned         uncompressed_data_length,
                              uint64_t         compressed_heap_offset,
                              DskError       **error)
{
  /* pack index0 entry */
  uint8_t packed_entry[B128_UINT64_MAX_SIZE * 2 + 8];
  unsigned packed_entry_len = encode_uint64_b128 (packed_entry, first_key_offset);
  packed_entry_len += encode_uint64_b128 (packed_entry + packed_entry_len,
                                          compressed_heap_offset);

  /* is it bigger than the current size? */
  WriterIndexLevel *level0 = w->index_levels + 0;
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
  if (!writer_file_write (&level0->index_file,
                          packed_entry_len,
                          packed_entry,
                          error))
    return DSK_FALSE;

  return DSK_TRUE;
}

static dsk_boolean
write_indexnon0_entry        (TableFileWriter *writer,
                              unsigned         index_level,
                              uint64_t         first_key_offset,
                              DskError       **error)
{
  uint8_t packed_entry[B128_UINT32_MAX_SIZE + B128_UINT64_MAX_SIZE];
  unsigned packed_entry_len = encode_uint32_b128 (packed_entry, first_key_length);
  packed_entry_len += encode_uint64_b128 (packed_entry + packed_entry_len,
                                          first_key_offset);

  /* is it bigger than the current size? */
  WriterIndexLevel *level = w->index_levels + index_level;
  unsigned index = IE_SIZE_TO_COUNT_INDEX__LEVELNON0 (packed_entry_len);
  if (level->size_index < index)
    {
      level->max_size = IE_COUNT_INDEX_TO_MAX_SIZE__LEVELNON0 (index);
      level->size_index = index;
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
  if (!writer_file_write (&level->index_file,
                          packed_entry_len,
                          packed_entry,
                          error))
    {
      return DSK_FALSE;
    }

  return DSK_TRUE;
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
  if (!writer_file_write (&w->index_levels[0].index_heap_file,
                          key_length, key_data, error))
    return DSK_FALSE;

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
          dsk_set_error (error, "error writing compressed buffer to disk: %s",
                         strerror (errno));
          return DSK_FALSE;
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

  /* Append key_Data to the initial buffers that are empty
     (stopping at the first non-empty one) */
  {
    WriterIndexLevel *level = w->index_levels + 0;
    WriterIndexLevel *end = w->index_levels + w->n_index_levels;
    while (level < end && level->n_entries == 0)
      {
        /* append to .K# */
        if (!writer_file_write (&level->key_heap_file, key_length, key_data, error))
          return DSK_FALSE;
        level->first_key_length = key_length;
        level++;
      }
  }

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
  w->index_levels[0].n_entries++;
  if (w->index_levels[0].n_entries < w->index0_ratio)
    goto done_handling_index_levels;
  if (!write_index0_entry (w,
                           level->key_heap_offset - level->first_key_length,
                           vli_len,
                           writer_file_offset (&w->compressed_heap_file) - outgoing_len,
                           error))
    return DSK_FALSE;
  for (ilevel = 1; ilevel < w->n_index_levels; ilevel++)
    {
      WriterIndexLevel *level = w->index_levels + ilevel;

      if (!write_indexnon0_entry (w, level,
                                  writer_file_offset (&level->key_heap_file) - level->first_key_length,
                                  error))
        return DSK_FALSE;

      if (++level->n_entries == w->index_ratio)
        {
          /* advance to the next level */
          level->n_entries = 0;
          if (ilevel + 1 == w->n_index_levels)
            {
              w->index_levels = DSK_RENEW (WriterIndexLevel,
                                           w->index_levels,
                                           w->n_index_levels + 1)
              WriterIndexLevel *lev = w->index_levels + w->n_index_levels;
              if (!writer_file_new (w->dir, ...filename,
                                    DSK_DIR_OPENFD_TRUNCATE|DSK_DIR_OPENFD_MAY_CREATE,
                                    mmapped, BUFFER_SIZE,
                                    &lev->index_file, error))
                {
                  return DSK_FALSE;
                }
              if (!writer_file_new (w->dir, ...filename,
                                    DSK_DIR_OPENFD_TRUNCATE|DSK_DIR_OPENFD_MAY_CREATE,
                                    mmapped, BUFFER_SIZE,
                                    &lev->index_heap_file, error))
                {
                  writer_file_destruct (&lev->index_file);
                  return DSK_FALSE;
                }
              memset (&lev->index_entry_size_to_count, 0,
                      sizeof (lev->index_entry_size_to_count));
              lev->size_index = 0;
              lev->max_size = IE_COUNT_INDEX_TO_MAX_SIZE__LEVELNON0 (0);

              w->n_index_levels += 1;
            }
        }
      else
        goto done_handling_index_levels;
    }
  /* append to potential new index level */
  w->potential_new_level[w->n_potential_new_level] = key_length;
  dsk_buffer_append (w->potential_new_level_keys, key_length, key_data);
  w->n_potential_new_level += 1;

  /* if we have too many entries (defined here as min (index_ratio/2, 32)),
     create the new index level */
  if (w->n_potential_new_level == DSK_MIN (32, w->index_ratio / 2))
    {
      WriterIndexLevel *new_level;
      char suffix[64];
      snprintf (suffix, 64, "K%u", w->n_index_levels);
      kh_fd = dsk_dir_openfd (w->dir,
                              w->base_filename, suffix,
                              DSK_DIR_OPENFD_MAY_CREATE|DSK_DIR_OPENFD_TRUNCATE, 0666,
                              error);
      if (kh_fd < 0)
        {
          dsk_add_error_prefix (error, "creating table's key file");
          return DSK_FALSE;
        }
      suffix[0] = 'I';
      ki_fd = dsk_dir_openfd (w->dir,
                              w->base_filename, suffix,
                              DSK_DIR_OPENFD_MAY_CREATE|DSK_DIR_OPENFD_TRUNCATE, 0666,
                              error);
      if (ki_fd < 0)
        {
          dsk_add_error_prefix (error, "creating table's index file");
          return DSK_FALSE;
        }
      w->index_levels = dsk_realloc (w->index_levels,
                                     (w->n_index_levels+1) * sizeof (WriterIndexLevel));
      new_level = w->index_levels + w->n_index_levels++;
      new_level->key_heap_file = fdopen (kh_fd, "wb");
      dsk_assert_runtime (new_level->key_heap_fp != NULL);
      new_level->index_file = fdopen (ki_fd, "wb");
      dsk_assert_runtime (new_level->index_fp != NULL);

      /* dump key-heap */
      for (frag = w->potential_new_level_keys.first_frag;
           frag != NULL;
           frag = frag->next)
        {
          if (!writer_file_write (&new_level->key_heap_file,
                                  frag->buf_length,
                                  frag->buf + frag->buf_start,
                                  error))
            return DSK_FALSE;
        }
      new_level->key_heap_offset = w->potential_new_level_keys.size;

      /* dump key-index-entries */
      for (i = 0; i < w->n_potential_new_level_keys; i++)
        {
          unsigned key_length = w->potential_new_level[i];
          if (!write_indexnon0_entry (w, new_level,
                                      key_offset, error))
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

  if (w->n_entries_in_incoming > 0)
    {
      /* compress remaining data, write index entry/entries */
      if (!do_compress (w, error))
        return DSK_FALSE;

      /* write a sentinel element to the index level 0. */
      if (!write_index0_entry (w,
                               writer_file_offset (&w->index_levels[0].key_heap_offset),
                               0,
                               writer_file_offset (&w->compressed_heap_file);
                               error))
        return DSK_FALSE;

      /* write sentinels to higher levels */
      unsigned i;
      for (i = 1; i < w->n_index_levels; i++)
        if (!write_indexnon0_entry (w,
                                    writer_file_offset (&w->index_levels[i].key_heap_offset),
                                    error))
          return DSK_FALSE;
    }

  /* close all index-levels (close index and heap files,
     and write the level-sizes arrays to disk) */
  for (li = 0; li < w->n_index_levels; li++)
    {
      uint8_t sizes_packed[N_LEVEL_SIZES*8];
      unsigned s;
      WriterIndexLevel *level = w->index_levels + li;
      for (s = 0; s < N_LEVEL_SIZES; s++)
        dsk_uint64le_pack (level->index_entry_size_to_count[s],
                           sizes_packed + 8*s);
      if (!writer_file_pwrite (&level->index_out, LEVEL_SIZES_FILE_OFFSET,
                               sizeof (sizes_packed), sizes_packed,
                               error))
        return DSK_FALSE;
    }
  for (li = 0; li < w->n_index_levels; li++)
    {
      WriterIndexLevel *level = w->index_levels + li;
      if (!writer_file_close (&level->index_file, error)
      ||  !writer_file_close (&level->key_heap_file, error))
        return DSK_FALSE;
    }

  /* close the compressed-heap */
  if (!writer_file_close (&level->compressed_heap_file, error))
    return DSK_FALSE;

  return DSK_TRUE;
}

static void
table_file_writer__destroy(DskTableFileWriter *writer)
{
  TableFileWriter *w = (TableFileWriter *) writer;
  if (w->closed)
    {
      /* This is only really necessary if close failed. */
      unsigned li;
      for (li = 0; li < w->n_index_levels; li++)
        {
          WriterIndexLevel *level = w->index_levels + li;
          if (level->index_out.fd >= 0)
            writer_file_destruct (&level->index_out);
          if (level->index_heap_out.fd >= 0)
            writer_file_destruct (&level->index_heap_out);
        }
      writer_file_destruct (&w->compressed_heap_file);
    }
  else
    {
      /* This is to enable the pattern where you destroy()
         w/o closing --  That's fine as long as you are comfortable w/
         deadly error-handling. */
      DskError *error = NULL;
      if (!table_file_writer__close (writer, &error))
        {
          dsk_die ("error closing table-file-writer on destroy: %s",
                   error->message);
        }
    }


  /* clean up memory */
  dsk_free (w->index_levels);
  dsk_free (w);
}

static TableFileWriter *
table_file_writer_new (DskDir                  *dir,
                       const char              *base_filename,
                       DskTableFileNewOptions  *options,
                       dsk_boolean              prefix_compress,
                       unsigned                 n_compress,
                       unsigned                 index_ratio,
                       DskError               **error)
{
  unsigned base_filename_len = strlen (base_filename);
  TableFileWriter *w = dsk_malloc (sizeof (TableFileWriter)
                                   + base_filename_len
                                   + FILENAME_EXTENSION_MAX
                                   + 1);
  w->dir = dir;
  w->base_filename = memcpy (w + 1, base_filename, base_filename_len);
  dsk_table_buffer_init (&w->incoming);
  dsk_table_buffer_init (&w->prefix_buffer);
  if (options->prefix_compress)
    w->append_to_incoming = append_to_incoming__prefix_compressed;
  else
    w->append_to_incoming = append_to_incoming__raw;
  w->n_entries_in_incoming = 0;
  w->compressor = compressor;

  w->compressed_heap_fd = dsk_dir_openfd (dir,
                                          path..., 0666,
                                          DSK_DIR_OPENFD_MAY_CREATE|DSK_DIR_OPENFD_TRUNCATE,
                                          error);
  if (w->compressed_heap_fd < 0)
    {
      dsk_free (w);
      return NULL;
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
                             DskTableDir             *location,
                             const char              *base_filename,
                             DskError               **error)
{
  TableFileInterface *I = (TableFileInterface *) iface;
  return (DskTableFileWriter *) table_file_writer_new (dir, base_filename,
                                                       I->prefix_compress,
                                                       I->n_compress,
                                                       I->index_ratio,
                                                       error);
}
static DskTableReader *
file_interface__new_reader  (DskTableFileInterface   *iface,
                             DskDir                  *dir,
                             const char              *base_filename,
                             DskError               **error)
{
  ...
}
static DskTableFileSeeker *
file_interface__new_seeker  (DskTableFileInterface   *iface,
                             DskDir                  *dir,
                             const char              *base_filename,
                             DskError               **error)
{
  ...
}
static dsk_boolean
file_interface__delete_file (DskTableFileInterface   *iface,
                             DskDir                  *dir,
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
