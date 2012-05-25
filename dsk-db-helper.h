
struct _DskDbHelper
{
  int openat_fd;
  unsigned n_files;
  DskDbHelperFile **files;
};

DskDbHelper     *dsk_db_helper_new                (const char      *dir,
                                                   DskError       **error);
void             dsk_db_helper_begin_transaction  (DskDbHelper     *helper);
DskDbHelperFile *dsk_db_helper_create_file_printf (DskDbHelper     *helper,
                                                   const char      *filename_format,
						   ...);
void             dsk_db_helper_file_write         (DskDbHelperFile *file,
                                                   uint64_t         offset,
			                           unsigned         length,
			                           const uint8_t   *data);


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
  //uint32_t cur_extra_hash = dsk_uint32_parse_le (table_entry_data + 4);
  uint64_t cur_bucket_offset = dsk_uint64_parse_le (table_entry_data + 8);
  uint32_t cur_key_length = ...;

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
              ...
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
