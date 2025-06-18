

typedef struct DskLsmSortedFileOptions DskLsmSortedFileOptions;
typedef struct DskLsmSortedFileWriter DskLsmSortedFileWriter;

struct DskLsmSortedFileOptions
{
  DskDir *dir;
  const char *filename_base;
  unsigned use_mmap : 1;
  uint64_t max_entries;
  uint64_t max_value_size;
  uint64_t max_key_size;
};


DskLsmSortedFileWriter *
dsk_lsm_sorted_file_writer_new (const DskLsmSortedFileOptions *opts);

bool
dsk_lsm_sorted_file_writer_add (DskLsmSortedFileWriter *writer,
		                size_t 

