/*

.BUCKETS = hash-bucket -> chain mapping
.CHAINS## = keys, pointer to value, pointer to next key in bucket
            divided into files by size
.VALUES## = values, divided into files by size,
.BIGVALUES = a buddy-system allocator, operating in multiples of some large size
.TRANSACTION = a file used to indicate that it the database is midway through 
              a transaction.  on startup, if we detect that the transaction 
	      was in progress, then we replay the transaction, just to be sure.
.STATE = gives state of a table resize

.tBUCKETS = transitional BUCKETS: used during table resize
.tCHAINS## = keys, pointer to value, pointer to next key in bucket
             divided into files by size

XXX Also: we can be writing out to a snapshot

How hash-table resizing is handled:

Algorithm: BEGIN_HASH_TABLE_INCREASE

...

Algorithm: BEGIN_HASH_TABLE_DECREASE

never to be implemented???

Algorithm: INSERT

In a transaction:
  1. compute hash
  2. figure out if the data is in the main heap, or if it is in the transitional hash
  3. search to find it (see LOOKUP)
  4. if not found,
        a.  we should have convenient
        b.  allocate from the CHAINS heap
	c.  

     if found,
        a. we should have a reference to a location in a BUCKETS file
	b. can we reuse the value slot?
	   i. if so, save the value slot
	   ii. otherwise, allocate a new value slot and free the old one

Algorithm: LOOKUP
  1. compute hash
  2. figure out if the data is in the main heap, or if it is in the transitional hash ???
  3. read entry from .BUCKETS (or .tBUCKETS).  It should have a pointer into CHAINS (or NULL):
     a. if a pointer, read chain value which should have key, next_chain_node, value_size, value_location.
        If key matches, return value.  Otherwise, repeat 3 with next_chain_node.
     b. if NULL, stop: NOT FOUND.

Algorithm: TRANSACTION  (Used whenever "In a transaction" is specified)
  1. create transaction
  2. evaluate code, writing to the transaction record instead of to disk
  3. set the bit that means: running_transaction
  4. perform the writes
  5. clear the bit that means: running_transaction:  we are done with the transaction

Algorithm: STARTUP
  1.  if the bit that means running_transaction is set:
     a. perform transaction
     b. clear bit

Type: TRANSACTION
  1.  
  */

#if 0
typedef enum
{
  DSK_DISKHEAP_FIXED_SIZES,
  DSK_DISKHEAP_BUDDY_SYSTEM
} DskDiskheapType;

struct _DskDiskheapConfig
{
  DskDiskhashHeapStrategy key_heap_strategy;
  unsigned n_key_heap_sizes;
  dsk_boolean key_heap_use_existing_strategy;
};

#define DSK_DISKHEAP_HANDLE_NULL {{0ULL,0ULL}}
#define DSK_DISKHEAP_HANDLE_IS_NULL(handle)    ((handle)->v[0]==0 && (handle)->v[1]==0)
struct _DskDiskheapHandle
{
  uint64_t v[2];
};

DskDiskheap       *dsk_diskheap_new    (DskDirectory      *dir,
                                        const char        *base_filename,
			                DskDiskheapConfig *config);
DskDiskheapHandle  dsk_diskheap_alloc  (DskDiskheap       *heap,
                                        size_t             size,
					const uint8_t     *init_data);
void               dsk_diskheap_set    (DskDiskheap       *heap,
                                        DskDiskheapHandle  handle,
                                        size_t             size,
					const uint8_t     *data);
void               dsk_diskheap_write  (DskDiskheap       *heap,
                                        DskDiskheapHandle  handle,
                                        size_t             offset,
                                        size_t             length,
					const uint8_t     *data);
void               dsk_diskheap_append (DskDiskheap       *heap,
                                        DskDiskheapHandle  handle,
                                        size_t             offset,
                                        size_t             length,
					const uint8_t     *data);
#endif


struct _DskDiskhashConfig
{
  uint32_t (*hash) (unsigned length,
                    const uint8_t *data,
		    void *hash_data);
  void *hash_data;


  DskDiskhashHeapStrategy
void dsk_diskhash
