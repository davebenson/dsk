
struct _DskDbHelperFile
{
  DskDbHelper *owner;
  ...
 
  /* Sorted by position. */
  DskDbHelperTransactionEntry *transactions;
}

struct _DskDbHelperPoolAllocator
{
  unsigned n_sizes;
  size_t *sizes;
  DskDbHelperFile **files;              /* created lazily */
  DskDbHelperPoolAllocator *next_allocator;
};

struct _DskDbHelperPoolPosition
{
  unsigned file_index;
  uint32_t length;
  uint64_t offset;
};

struct _DskDbHelperTransactionEntry
{
  DskDbHelperFile *file;
  uint64_t position;
  unsigned length;
  uint8_t *data;

  DskDbHelperTransactionEntry *prev_helper_entry, *next_helper_entry;

  /* per-file tree, sorted by start position. */
  DskDbHelperTransactionEntry *file_left, *file_right, *file_parent;
  dsk_boolean file_is_red;
};

struct _DskDbHelper
{
  DskDir *dir;

  DskDbHelperFile *files_by_name;
  DskMemPoolFixed file_pool;

  DskDbHelperTransactionEntry *transaction_entries;
  DskMemPoolFixed transaction_pool;
  DskDbHelperFile *transaction_files;
  int openat_fd;
};

DskDbHelper     *dsk_db_helper_new                (const char      *dir,
                                                   DskError       **error);
void             dsk_db_helper_begin_transaction  (DskDbHelper     *helper);
void             dsk_db_helper_abort_transaction  (DskDbHelper     *helper);
dsk_boolean      dsk_db_helper_end_transaction    (DskDbHelper     *helper,
                                                   DskError       **error);

/* --- file api --- */
DskDbHelperFile *dsk_db_helper_create_file        (DskDbHelper     *helper,
                                                   const char      *filename);
DskDbHelperFile *dsk_db_helper_create_file_printf (DskDbHelper     *helper,
                                                   const char      *filename_format,
						   ...);
dsk_boolean      dsk_db_helper_file_peek_read     (DskDbHelperFile *file,
                                                   uint64_t         offset,
			                           unsigned         length,
                                                   uint8_t        **data_out,
                                                   DskError       **error);

void             dsk_db_helper_file_write         (DskDbHelperFile *file,
                                                   uint64_t         offset,
			                           unsigned         length,
			                           const uint8_t   *data);
void             dsk_db_helper_file_write_thru    (DskDbHelperFile *file,
                                                   uint64_t         offset,
			                           unsigned         length,
			                           const uint8_t   *data);
/* --- allocator api --- */
struct _DskDbHelperPoolConfig
{
  unsigned n_lengths;
  uint32_t *lengths;
  dsk_boolean builtin;
};
extern DskDbHelperPoolConfig dsk_db_helper_pool__powers_of_2;
extern DskDbHelperPoolConfig dsk_db_helper_pool__powers_of_sqrt2;
extern DskDbHelperPoolConfig dsk_db_helper_pool__powers_of_2__sesqui;
DskDbHelperPoolAllocator *
                 dsk_db_helper_pool_allocator_new (DskDbHelper     *helper,
                                                   DskDbHelperPoolConfig *config);
dsk_boolean      dsk_db_helper_pool_alloc         (DskDbHelperPoolAllocator *allocator,
                                                   uint32_t         size,
                                                   DskDbHelperPoolPosition *position_out,
                                                   DskError       **error);



struct _DskDiskhash
{
  DskDbHelper *helper;
  unsigned     n_buckets_log2;
  uint64_t     occupancy;

  uint64_t (*hash_key)(unsigned key_length,
                       const uint8_t *key_data);

  unsigned n_chain_fixed_sizes;
  unsigned *chain_fixed_sizes;
  unsigned n_value_fixed_sizes;
  unsigned *value_fixed_sizes;

  DskDbHelperFile *table;
  DskDbHelperFile **chains;
  uint64_t *chain_counts; /* cached sizes of the chains */
  DskDbHelperFile **values;

  /* While resizing */
  dsk_boolean resizing;
  unsigned resize_bucket_at;  /* index in starting size array */
  DskDbHelperFile *new_table;
  DskDbHelperFile **new_chains;
  uint64_t *new_chain_counts;

  /* While backing up */
  dsk_boolean backing_up;
  unsigned backup_bucket_at;
  DskDbHelperFile *backup_table;
  DskDbHelperFile **backup_chains;
  DskDbHelperFile **backup_values;
};

static inline dsk_boolean
find_fixed_size (unsigned n_fixed_sizes,
                 const unsigned *fixed_sizes,
		 unsigned size,
		 unsigned *index_out)
{
  if (size > fixed_sizes[n_fixed_sizes-1])
    return DSK_FALSE;
  unsigned min = 0, count = n_fixed_sizes;
  while (count > 1)
    {
      if (fixed_sizes[mid + count / 2] > size)
        count /= 2; 
      else
        {
	  count = min + count - mid;
	  min = mid;
	}
    }
  *index_out = min;
  return DSK_TRUE;
}

#define MASK_HASH(hashcode, log2) \
  (hashcode & ((1ULL << (log2)) - 1ULL))
dsk_boolean dsk_diskhash_insert (DskDiskhash   *hash,
                                 unsigned       key_length,
			         const uint8_t *key_data,
                                 unsigned       value_length,
			         const uint8_t *value_data,
                                 DskError     **error)
{
  uint64_t hashcode = hash->hash_key (key_length, key_data);
  unsigned key_bucket, value_bucket;
  if (!find_fixed_size (hash->n_chain_fixed_sizes, hash->chain_fixed_sizes, &which_chain_size_index))
    {
      dsk_set_error (error, "key too large (%u bytes)", key_length);
      return DSK_FALSE;
    }
  unsigned donate_cycles = 32;
  uint64_t n_buckets = 1ULL << hash->n_buckets_log2;

  /* Both resize/backup try to avoid writing to the transaction log as much as possible. */
  if (hash->resizing)
    {
      while (donate_cycles > 0 && hash->resize_bucket_at < n_buckets)
        {
          unsigned n_cycles_used;
          if (!resize_split_chain (hash, hash->resize_bucket_at, &n_cycles_used, error))
            return DSK_FALSE;
          n_cycles_used += 1;           /* count the chain as a cycle too */
          hash->resize_bucket_at += 1;
          if (n_cycles_used >= donate_cycles)
            donate_cycles = 0;
          else
            donate_cycles -= n_cycles_used;
        }
      if (hash->resize_bucket_at == n_buckets)
        {
          if (hash->backing_up)
            {
              /* the backup always uses the hash-table it started with,
                 so just suspend the resize until we are done with the backup.
                 This usually has no effect (since we will use the new hash-table exclusively),
                 except it prevents a resize from being scheduled. */
            }
          else
            {
              resize_done (hash);               /* may begin another resize */
              n_buckets <<= 1;
            }
        }
    }
  if (hash->backing_up && donate_cycles > 0)
    {
      while (donate_cycles > 0 && hash->backup_bucket_at < n_buckets)
        {
          unsigned n_cycles_used;
          if (!backup_chain (hash, hash->backup_bucket_at, &n_cycles_used, error))
            return DSK_FALSE;
          n_cycles_used += 1;           /* count the chain as a cycle too */
          hash->backup_bucket_at += 1;
          if (n_cycles_used >= donate_cycles)
            donate_cycles = 0;
          else
            donate_cycles -= n_cycles_used;
        }

      /* if done with backup, may be done with resize, if resize was blocked on backup */
      if (hash->backup_bucket_at == n_buckets)
        {
          /* finish backup */
          if (!backup_done (hash, error))
            return DSK_FALSE;

          if (hash->resizing && hash->resize_bucket_at == n_buckets)
            {
              resize_done (hash);               /* may begin another resize */
              n_buckets <<= 1;
            }
        }
    }

  /* Does this entry go in the new hash-table or the old hash-table? */
  dsk_boolean use_new_hash_table = (hash->is_resizing && hash_table_index < hash->resize_bucket_at);
  DskDbHelperFile *table;
  uint64_t table_index;
  DskDbHelperFile **chains;
  if (use_new_hash_table)
    {
      table = hash->new_table;
      table_index = MASK_HASH (hashcode, hash->n_buckets_log2 + 1);
      //extra_hash = hashcode >> (hash->n_buckets_log2 + 1);
      chains = hash->new_chains;
    }
  else
    {
      table = hash->table;
      table_index = MASK_HASH (hashcode, hash->n_buckets_log2);
      //extra_hash = hashcode >> (hash->n_buckets_log2);
      chains = hash->chains;
    }

  uint8_t *table_entry_data;
  if (!dsk_db_helper_file_peek_read (hash->table, hash_table_index * HASH_ENTRY_SIZE + HASH_HEADER_SIZE, HASH_ENTRY_SIZE, &table_entry_data, error))
    return DSK_FALSE;
  uint32_t cur_which_chain_size_index = dsk_uint32_parse_le (table_entry_data + 0);
  uint32_t cur_key_length = dsk_uint32_parse_le (table_entry_data + 4);
  uint64_t cur_bucket_offset = dsk_uint64_parse_le (table_entry_data + 8);

  while (cur_which_chain_size_index != 0)
    {
      if (cur_which_chain_size_index > hash->n_chain_fixed_sizes)
        {
          dsk_set_error (error, "chain corruption detected (0x%llx/0x%llx)",
                         table_index, 1ULL<<hash->n_buckets_log2);
          return DSK_FALSE;
        }
      unsigned size_index = cur_which_chain_size_index - 1;

      /* read key and pointer to value */
      DskDbHelperFile *chain_file = hash->chains[size_index];
      if (chain_file == NULL)
        {
          chain_file = dsk_db_helper_open_file_printf (hash->helper,
                                                       DSK_DB_HELPER_OPEN_FILE_EXISTS,
                                                       error,
                                                       "CHAIN.%02x",
                                                       size_index);
          if (chain_file == NULL)
            return DSK_FALSE;
          hash->chains[size_index] = chain_file;
        }

      chain_element_size = CHAIN_FILE_ENTRY_SIZE (hash->chain_fixed_sizes[size_index]);
      if (!dsk_db_helper_file_peek_read (chain_file, cur_bucket_offset * chain_element_size + CHAIN_FILE_HEADER_SIZE, chain_element_size, &chain_element, error))
        return DSK_FALSE;

      if (key_length == cur_key_length
       //&& cur_extra_hash == extra_hash
       && memcmp (key_data, cur_key_data, key_length) == 0)
        {
          unsigned value_bucket_size = hash->value_fixed_sizes[value_file_index];
          if (value_bucket_size >= value_length)
            {
              /* set it in-place, using transaction */
              uint64_t value_offset = dsk_uint64_parse_le (table_entry_data + 16);
              dsk_db_helper_begin_transaction (hash->helper);
              value_entry_size = VALUE_FILE_ENTRY_SIZE (value_bucket_size);
              dsk_db_helper_file_write (hash->value_files[value_file_index],
                                        VALUE_FILE_HEADER_SIZE + value_offset * value_entry_size,
                                        value_length, value_data);
              dsk_db_helper_file_write (chain_file, chain_offset + CHAIN_OFFSET_VALUE_LENGTH,
                                        4, new_value_length_bytes);
              dsk_db_helper_end_transaction (hash->helper);
            }
          else
            {
              /* allocate new location, free old location:
                 if done in that order, value write can evade transaction */
              dsk_db_helper_begin_transaction (hash->helper);
              if (!allocate_location (hash->helper,
                                      hash->n_value_fixed_sizes,
                                      hash->value_fixed_sizes
                                      hash->value_files,
                                      &new_value_file, &new_value_offset,
                                      error))
                {
                  dsk_db_helper_abort_transaction (hash->helper);
                  return DSK_FALSE;
                }
              if (!free_location (hash->helper,
                                  hash->n_value_fixed_sizes,
                                  hash->value_fixed_sizes
                                  hash->value_files,
                                  old_value, old_offset,
                                  error))
                {
                  dsk_db_helper_abort_transaction (hash->helper);
                  return DSK_FALSE;
                }

              /* NOTE: this function is only called if the two bucket sizes differ,
                 therefore, there is no way this write (to a newly allocated area)
                 can need to interact with the transaction. (write_thru asserts that
                 the location does not conflict with the transaction's writes) */
              dsk_db_helper_file_write_thru (...);

              dsk_db_helper_end_transaction (hash->helper);
            }

          return DSK_TRUE;
        }


      cur_which_chain_size_index = dsk_uint32_parse_le (chain_element + 0);
      cur_bucket_offset = ...;
      cur_key_length = ...;
      //cur_extra_hash = ...;
    }

  /* Entry does not exist. */

  dsk_db_helper_begin_transaction (hash->helper);

  /* Allocate key-chain */
  ...

  /* Allocate value area. */
  ...

  /* Write value (can evade transaction) */
  dsk_db_helper_file_write_thru (...);

  /* Commit transaction */
  dsk_db_helper_end_transaction (hash->helper);

  return DSK_TRUE;
}
