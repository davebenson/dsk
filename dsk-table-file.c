#include "dsk.h"


/* === Generic TableFileInterface code === */


/* === Standard TableFileInterface implementation === */

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
};
#define IE_SIZE_TO_COUNT_INDEX__LEVEL0(size)   ((size) < 8 ? 0 : ((size)-8+3)/4)
#define IE_SIZE_TO_COUNT_INDEX__LEVELNON0(size) (((size)-2+1)/2)

typedef struct _TableFileWriter TableFileWriter;
struct _TableFileWriter
{
  DskTableFileWriter base;

  /* slab of data to compress and write */
  DskTableBuffer incoming;
  DskTableBuffer prefix_buffer;
  void (*append_to_incoming) (TableFileWriter *writer,
                              unsigned            key_length,
                              const uint8_t      *key_data,
                              unsigned            value_length,
                              const uint8_t      *value_data);
  unsigned n_entries_in_incoming;
  DskTableFileCompressor *compressor;
  FILE *compressed_heap;
  uint64_t compressed_heap_offset;

  unsigned n_index_levels;
  WriterIndexLevel *index_levels;
};

static dsk_boolean
table_file_writer__write  (DskTableFileWriter *writer,
                           unsigned            key_length,
                           const uint8_t      *key_data,
                           unsigned            value_length,
                           const uint8_t      *value_data,
                           DskError          **error)
{
  TableFileWriter *w = (TableFileWriter *) writer;
  w->append_to_incoming (w, key_length, key_data, value_length, value_data);
  w->n_entries_in_incoming += 1;
  if (w->n_entries_in_incoming < w->n_compress)
    return DSK_TRUE;

  /* perform compression, write to file */
  if (w->compressor == NULL)
    {
      comp_len = w->incoming.size;
      vli_header = comp_len;
      comp_data = w->incoming.data;
    }
  else
    {
      dsk_table_buffer_set_size (&w->compression_buffer,
                                 w->incoming.size + 32);
      if (!w->compressor->compress (w->compressor,
                                    w->incoming.size, w->incoming.data,
                                    &w->compression_buffer.size,
                                    w->compression_buffer.data))
        {
          vli_header = 0;
          comp_len = w->incoming.size;
          comp_data = w->incoming.data;
        }
      else
        {
          vli_header = comp_len = w->compression_buffer.size;
          comp_data = w->compression_buffer.data;
        }
    }
  heap_offset = w->compressed_heap_offset;
  vli_len = encode_uint32_b7 (....);
  if (FWRITE (vli, vli_len, 1, w->compressed_heap) != 1
   || FWRITE (comp_data, comp_len, 1, w->compressed_heap) != 1)
    {
      ...
    }
  w->compressed_heap_offset += vli_len + comp_len;

  /* write to index file, possible propagating up to higher indexes */
  ...
  
  /* reset pre-compressed entries */
  w->n_entries_in_incoming = 0;
  w->incoming.size = w->incoming.prefix_buffer = 0;

  return DSK_TRUE;
}

static dsk_boolean
table_file_writer__close  (DskTableFileWriter *writer,
                           DskError          **error)
{
  ...
}

static void
table_file_writer__destroy(DskTableFileWriter *writer)
{
  ...
}

/* the --- TableFileInterface interface constructor itself --- */
typedef struct _TableFileStdInterface TableFileStdInterface;
struct _TableFileStdInterface
{
  DskTableFileInterface base_iface;
  DskTableFileCompressor *compressor;
  unsigned n_compress;
  unsigned index_ratio;
};

static DskTableFileWriter *
file_interface__new_writer  (DskTableFileInterface   *iface,
                             const char              *openat_dir,
                             int                      openat_fd,
                             const char              *base_filename,
                             DskError               **error)
{
  ...
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
  TableFileStdInterface *iface = dsk_malloc (sizeof (TableFileStdInterface));
  iface->base_iface.new_reader = file_interface__new_reader;
  iface->base_iface.new_writer = file_interface__new_writer;
  iface->base_iface.new_seeker = file_interface__new_seeker;
  iface->base_iface.delete_file = file_interface__delete_file;
  iface->base_iface.destroy = file_interface__destroy;
  iface->compressor = compressor;
  iface->n_compress = options->n_compress;
  iface->index_ratio = options->index_ratio;
  return iface;
}
