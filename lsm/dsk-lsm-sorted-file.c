#define MAX_EXT_LEN 4

// .fast
// .idx
// .keys
// .vals
//

struct DskLsmSortedFileIdxHeader
{
  uint64_t magic;
  uint64_t app_magic;
  uint32_t version;
  uint32_t app_version;
  uint32_t flags;
  uint32_t fastidx_prefix_size;
  uint32_t fastidx_reduction_ratio;
  uint64_t n_keys;
  uint64_t n_unique_keys;
  uint64_t key_heap_size;
  uint64_t n_values;
  uint64_t value_heap_size;
};
#define LSM_FAST_MAGIC    0x99e9840ef27a5cd9ad808d0b9f6de1bbULL
#define LSM_IDX_MAGIC     0x400ae310f706083b800a5b358d0b207eULL
#define LSM_KEYS_MAGIC    0x57958d21559d7458d84f47fae3a1cec1ULL
#define LSM_VALS_MAGIC    0x62d3eae6da6728893969027fa0623ff2ULL

// idx Header:
//   8  magic
//   8  app_magic
//   4  version  (currently 1)
//   4  flags
//   4  fastidx prefix size
//   4  fastidx reduction ratio   -- if 0, no fastidx
//   8  n_keys			  -- these fields are only updated at the end
//   8  n_unique_keys
//   8  key_heap_size
//   8  n_values
//   8  value_heap_size
//
// fast, keys, and values just have an 8-byte magic

struct DskLsmSortedFileWriter {
  int idx_fd;
  int key_heap_fd;
  int value_heap_fd;
  int fastidx_fd;	         // if -1, no fastidx

  uint8_t *idx_data;	         // idx mmapped
  uint8_t *key_heap_data;	 // key_heap mmapped
  uint8_t *value_heap_data;	 // value_heap mmapped
  uint8_t *fast_data;

  uint64_t app_magic;

  uint64_t init_index_offset;
  uint64_t init_heap_offset;
  uint64_t cur_fast_index_offset;
  uint64_t n_fast_index_entries;
  // index_offset = init_index_offset + n_entries*INDEX_ENTRY_SIZE
  uint64_t key_heap_offset;
  uint64_t value_heap_offset;
};

DskLsmSortedFileWriter *
dsk_lsm_sorted_file_writer_new (const DskLsmSortedFileOptions *opts);
{
  DskLsmSortedFileWriter *writer = dsk_new (DskLsmSortedFileWriter);
  unsigned filename_len = strlen(opts->filename);
  char *fname_buf = alloca (filename_len + 1 + MAX_EXT_LEN + 1);
  memcpy (fname_buf, opts->filename, filename_len);

  strcpy (fname_buf + filename_len, ".idx");
  writer->idx_fd = dsk_dir_sys_open (opts->dir, fname_buf, O_CREAT | O_TRUNC | O_RDWR, 0666);
  LsmSortedFileIdxHeader idx_header = {
    .magic = LSM_IDX_MAGIC,
    .app_magic = opts->app_magic,
    .version = 1,
    .app_version = opts->app_version,
    .flags = 0,
    .fastidx_prefix_size = ...,
    .fastidx_reduction_ratio = ...,
    .n_keys = 0,
    .n_unique_keys = 0,
    .key_heap_size = 8,
    .n_values = 0,
    .value_heap_size = 8,
  };

  if (opts->use_mmap)
    {
       size_t max_idx_size = sizeof(LsmSortedFileIdxHeader)
		           + SIZEOF_INDEX_ENTRY * ops->max_entries;
       if (ftruncate (writer->idx_fd, max_idx_size) < 0)
         {
	   // ignore errors.
	   dsk_dir_sys_rm (opts->dir, fname_buf);
	   dsk_free (writer);
	   return NULL;
	 }

       void *rv = mmap (...);
       if (rv == MAP_FAILED || rv == NULL)
         {
	   ...
         }
       writer->idx_data = rv;
       WRITE HEADER
    }
  else
    {
       WRITE HEADER
       writer->idx_data = NULL;
    }

  strcpy (fname_buf + filename_len, ".values");
  writer->value_heap_fd = dsk_dir_sys_open (opts->dir, fname_buf, O_CREAT | O_TRUNC | O_RDWR, 0666);
  WRITE HEADER

  strcpy (fname_buf + filename_len, ".keys");
  writer->key_heap_fd = dsk_dir_sys_open (opts->dir, fname_buf, O_CREAT | O_TRUNC | O_RDWR, 0666);
  WRITE HEADER

  if (... has fast idx?)
    {
    }
  else
    {
      writer->fastidx_fd = -1;
    }

  return writer;
}
