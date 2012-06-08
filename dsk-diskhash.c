
typedef enum
{
  DSK_DISKHASH_FILE_TABLE,              /* the array of buckets: a single flat file */
  DSK_DISKHASH_FILE_KEY,                /* one of the fixed-length key files */
  DSK_DISKHASH_FILE_VALUE,              /* one of the fixed-length value files */
  DSK_DISKHASH_FILE_RESIZE_KEY,         /* fixed-length key files, post resize */
  DSK_DISKHASH_FILE_RESIZE_TABLE        /* the array of buckets, post resize */
} DskDiskhashFileType;

typedef struct _DskDiskhashTransaction DskDiskhashTransaction;
typedef struct _DskDiskhashFile DskDiskhashFile;

struct _DskDiskhashFile
{
  DskDiskhashFileType type;
  unsigned index;
  int fd;
  uint64_t length;
  void *mmapped;
};

typedef struct _FixedLengthFileset FixedLengthFileset;
struct _FixedLengthFileset
{
  unsigned n_fixed_sizes;
  unsigned *fixed_sizes;
  DskDiskhashFile *files;
};

typedef struct _FixedLengthFilesetPosition FixedLengthFilesetPosition;
struct _FixedLengthFilesetPosition
{
  uint32_t file_index;
  uint32_t length;
  uint64_t offset;
};
#define FILE_INDEX_EOL   0xffffffff
#define HASH_TABLE_ENTRY_SIZE   16
#define HASH_TABLE_HEADER_SIZE  16

struct _DskDiskhash
{
  DskObject base_instance;
  DskDir *dir;
  unsigned     n_buckets_log2;
  uint64_t     occupancy;

  uint64_t (*hash_key)(unsigned key_length,
                       const uint8_t *key_data);

  unsigned n_value_fixed_sizes;
  unsigned *value_fixed_sizes;

  DskDiskhashFile table;
  FixedLengthFileset keys;
  FixedLengthFileset values;

  /* While resizing */
  dsk_boolean resizing;
  unsigned resize_bucket_at;  /* index in starting size array */
  DskDiskhashFile *new_table;
  FixedLengthFileset new_keys;

  /* While backing up */
  dsk_boolean backing_up;
  unsigned backup_bucket_at;
  FixedLengthFileset backup_keys;
  FixedLengthFileset backup_values;
  DskDiskhashFile backup_table;

  unsigned n_transaction;
  DskDiskhashTransaction *transaction;
  unsigned transaction_alloced;
  DskMemPool transaction_pool;
};

struct _DskDiskhashTransaction
{
  DskDiskhashFile *file;
  uint64_t offset;
  uint32_t length;
  void *data;
};

static void
begin_transaction (DskDiskhashFile *file)
{
  /* TODO: use a cache of memory */
  dsk_mem_pool_reset (&file->transaction_pool);
}

static void
resize_transaction_array  (DskDiskhash *diskhash)
{
  unsigned new_alloced = diskhash->transaction_alloced;
  if (new_alloced)
    new_alloced *= 2;
  else
    new_alloced = 4;
  diskhash->transaction = DSK_RENEW (diskhash->transaction, DskDiskhashTransaction, new_alloced);
  diskhash->transaction_alloced = new_alloced;
}

static inline void
add_transaction (DskDiskhash *diskhash,
                 DskDiskhashFile *file,
                 uint64_t offset,
                 uint32_t length,
                 void *data)
{
  DskDiskhashTransaction *xaction;
  if (DSK_UNLIKELY (diskhash->n_transaction == diskhash->transaction_alloced))
    resize_transaction_array (diskhash);
  xaction = diskhash->transaction + diskhash->n_transaction++;
  xaction->file = file;
  xaction->offset = offset;
  xaction->length = length;
  xaction->data = data;
}

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

static dsk_boolean
read_table_entry (DskDiskhash  *diskhash,
                  uint64_t      index,
                  dsk_boolean   from_resizing_table,
                  FixedLengthFilesetPosition   *out,
                  DskError    **error)
{
  uint64_t offset = index * HASH_TABLE_ENTRY_SIZE + HASH_TABLE_HEADER_SIZE;
  DskDiskhashFile *table = from_resizing_table ? &diskhash->new_table : &diskhash->table;
  uint8_t *data_packed;
  if (!peek_read (diskhash, table, offset, HASH_TABLE_ENTRY_SIZE, &data_packed, error))
    return DSK_FALSE;
  out->file_index = dsk_uint32_unpack (data_packed + 0);
  out->length = dsk_uint32_unpack (data_packed + 4);
  out->offset = dsk_uint64_unpack (data_packed + 8);

  /* TODO: check file-index??? */

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

  FixedLengthFilesetPosition position;
  if (!read_table_entry (diskhash, use_new_hash_table, table_index, &position, error))
    return DSK_FALSE;

  while (position.file_index != FILE_INDEX_EOL)
    {
      if (position.file_index > hash->n_chain_fixed_sizes)
        {
          dsk_set_error (error, "chain corruption detected (0x%llx/0x%llx)",
                         table_index, 1ULL<<hash->n_buckets_log2);
          return DSK_FALSE;
        }
      if (!read_chain_entry (diskhash, use_new_hash_table, position, &next_position, &value_position, &tmp_key_data, error))
        return DSK_FALSE;

      if (position.length == key_length && memcmp (key_data, tmp_key_data, key_length) == 0)
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

void
resize_split_chain (DskDiskhash *diskhash,
                    unsigned     old_bucket,
                    unsigned    *n_elements_moved,
                    DskError   **error)
{
  /* 
  ...
}
