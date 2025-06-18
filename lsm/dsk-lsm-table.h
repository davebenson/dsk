
typedef struct DskLsmBuffer DskLsmBuffer;
typedef struct DskLsmTableConfig DskLsmTableConfig;
typedef struct DskLsmReader DskLsmReader;
typedef struct DskLsmReaderBoundSpec DskLsmReaderBoundSpec;
typedef struct DskLsmReaderStreamInit DskLsmReaderStreamInit;
typedef struct DskLsmTableClass DskLsmTableClass;
typedef struct DskLsmTable DskLsmTable;

#define DSK_LSM_TABLE(object) DSK_OBJECT_CAST(DskLsmTable, object, &dsk_lsm_table_class)
#define DSK_LSM_TABLE_GET_CLASS(object) DSK_OBJECT_CAST_GET_CLASS(DskStream, object, &dsk_lsm_table_class)


typedef int (*DskLsmCompare) (size_t         a_len,
			      const uint8_t *a,
			      size_t         b_len,
			      const uint8_t *b,
			      void          *compare_data);

typedef enum
{
  DSK_LSM_TRUNCATED_COMPARE_LESS,
  DSK_LSM_TRUNCATED_COMPARE_GREATER,
  DSK_LSM_TRUNCATED_COMPARE_EQUAL,
  DSK_LSM_TRUNCATED_COMPARE_INDETERMINATE
} DskLsmTruncatedCompareResult;
typedef int (*DskLsmTruncatedCompare)(size_t         a_len,
                                      size_t         a_trunc_len,
                                      const uint8_t *a_trunc,
                                      size_t         b_len,
                                      const uint8_t *b,
                                      void          *compare_data);

typedef enum {
  DSK_LSM_MERGE_RETURN_A,
  DSK_LSM_MERGE_RETURN_B,
  DSK_LSM_MERGE_RETURN_BUFFER,
  DSK_LSM_MERGE_RETURN_NONE
} DskLsmMergeReturn;

typedef DskLsmMergeReturn (*DskLsmMerge)(size_t         a_len,
                                         const uint8_t *a,
                                         size_t         b_len,
                                         const uint8_t *b,
                                         DskFlatBuffer *buffer,
                                         void          *merge_data);

struct DskLsmTableConfig {
  // 
  DskLsmCompare compare;
  DskLsmTruncatedCompare truncated_compare;
  void *compare_data;
  DskDestroyNotify compare_data_destroy;
  DskLsmFastCompare compare;
  
  // If NULL, we use the second value.
  DskLsmMerge merge;
  void *merge_data;
  DskDestroyNotify merge_data_destroy;
  size_t merge_result_init_size;

  uint8_t fast_index_reduction_log2;  // 0 means 'no fast index'
  uint32_t fast_index_key_max;

  DskLsmFinalFilter final_filter;
  void *final_filter_data;
  DskDestroyNotify final_filter_data_destroy;

  size_t in_memory_max_records;
  size_t in_memory_max_bytes;
}; 

struct DskLsmReader {
  bool (*advance)(DskLsmReader *reader, DskError **error);
  void (*destroy)(DskLsmReader *reader);

  // These values are 'protected'.
  // They are read-only unless you are implementing a subclass.
  size_t key_len;
  const uint8_t *key_data;
  size_t value_len;
  const uint8_t *value_data;
  DskError *last_error;
};

typedef enum {
  DSK_LSM_READER_RANGE_NO_BOUND,
  DSK_LSM_READER_RANGE_INCLUSIVE,
  DSK_LSM_READER_RANGE_EXCLUSIVE
} DskLsmReaderBoundType;

struct DskLsmReaderBoundSpec {
  DskLsmReaderBoundType bound_type;
  size_t key_len;
  const uint8_t *key_data;
};

struct DskLsmReaderStreamInit {
  DskLsmReaderBoundSpec start_bound;
  DskLsmReaderBoundSpec end_bound;
  unsigned skip_memory_table : 1;
};

struct DskLsmTableClass {
  DskObjectClass base_class;
  bool           (*add)   (DskLsmTable   *table,
                           size_t         key_len,
                           const uint8_t *key_data,
                           size_t         value_len,
                           const uint8_t *value_data,
                           DskError **error);
  bool           (*lookup)(DskLsmTable   *table,
                           size_t         key_len,
                           const uint8_t *key_data,
                           DskLsmBuffer   *output,
                           DskError      **error);
  DskLsmReader   * (*read)(DskLsmTable    *table,
                           DskLsmReaderStreamInit *reader_init,
                           DskError      **error);
  DskLsmWriter   * (*create_transaction) 
                          (DskLsmTable     *table,
                           DskLsmTableTransactionHints *hints,
                           DskError      **error);

};
  
