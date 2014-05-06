
typedef struct _DskTableCheckpointInterface DskTableCheckpointInterface;
typedef struct _DskTableFileInterface DskTableFileInterface;
typedef struct _DskTable DskTable;

typedef struct _DskTableBuffer DskTableBuffer;
struct _DskTableBuffer
{
  unsigned length;
  uint8_t *data;
  unsigned alloced;
};
#define DSK_TABLE_BUFFER_INIT {0,NULL,0}
DSK_INLINE_FUNC uint8_t *
dsk_table_buffer_set_size (DskTableBuffer *buffer,
                           unsigned        length);

typedef enum
{
  DSK_TABLE_MERGE_RETURN_A_FINAL,
  DSK_TABLE_MERGE_RETURN_A,
  DSK_TABLE_MERGE_RETURN_B_FINAL,
  DSK_TABLE_MERGE_RETURN_B,
  DSK_TABLE_MERGE_RETURN_BUFFER_FINAL,
  DSK_TABLE_MERGE_RETURN_BUFFER,
  DSK_TABLE_MERGE_DROP
} DskTableMergeResult;

typedef DskTableMergeResult (*DskTableMergeFunc)  (unsigned       key_length,
                                                   const uint8_t *key_data,
                                                   unsigned       a_length,
                                                   const uint8_t *a_data,
                                                   unsigned       b_length,
                                                   const uint8_t *b_data,
						   DskTableBuffer *buffer,
						   dsk_boolean    complete,
						   void          *merge_data);
typedef int                 (*DskTableCompareFunc)(unsigned       key_a_length,
                                                   const uint8_t *key_a_data,
                                                   unsigned       key_b_length,
                                                   const uint8_t *key_b_data,
						   void          *compare_data);
  
typedef struct _DskTableConfig DskTableConfig;
struct _DskTableConfig
{
  DskTableCompareFunc compare;
  void *compare_data;
  DskTableMergeFunc merge;
  void *merge_data;
  dsk_boolean chronological_lookup_merges;
  DskDir *dir;
  DskTableFileInterface *file_interface;
  DskTableCheckpointInterface *cp_interface;

  /* quantile-estimation.  For some apps, it is helpful
     to be able to get a reasonable current distribution of
     the keyspace.  In particular this is good for implementing "DHTs"
     that support range queries. */
  unsigned quantiles_n;
};
#define DSK_TABLE_CONFIG_INIT                                       \
{                                                                      \
  NULL, NULL,           /* standard comparator */                      \
  NULL, NULL,           /* standard replacement merge */               \
  DSK_FALSE,            /* anti-chronological lookups */               \
  NULL,                 /* directory (DskDir) */                       \
  NULL,                 /* default to trivial table-file */            \
  NULL,                 /* default to trivial checkpoint interface */  \
  0                     /* by default, disable quantile estimation */  \
}

DskTable   *dsk_table_new          (DskTableConfig *config,
                                    DskError      **error);

/* Returns FALSE on error or on "not found". */
dsk_boolean dsk_table_lookup       (DskTable       *table,
                                    unsigned        key_length,
                                    const uint8_t  *key_data,
                                    unsigned       *value_len_out,
                                    const uint8_t **value_data_out,
                                    DskError      **error);
dsk_boolean dsk_table_insert       (DskTable       *table,
                                    unsigned        key_length,
                                    const uint8_t  *key_data,
                                    unsigned        value_length,
                                    const uint8_t  *value_data,
                                    DskError      **error);
void        dsk_table_destroy      (DskTable       *table);
void        dsk_table_destroy_erase(DskTable       *table);

DskDir     *dsk_table_peek_dir     (DskTable       *table);

typedef struct _DskTableReader DskTableReader;
struct _DskTableReader
{
  /* Readonly public data */
  dsk_boolean at_eof;
  unsigned key_length;
  unsigned value_length;
  const uint8_t *key_data;
  const uint8_t *value_data;

  /* Virtual functions */
  dsk_boolean (*advance)     (DskTableReader *reader,
                              DskError      **error);
  void        (*destroy)     (DskTableReader *reader);
};

DskTableReader  *dsk_table_new_reader(DskTable       *table,
                                      DskError      **error);

/* returns all entries for which the supplied 'compare' returns 0. */
DskTableReader  *dsk_table_dump_range    (DskTable       *table,
                                          DskTableCompareFunc compare,
                                          void           *compare_data,
                                          DskDestroyNotify compare_data_destroy,
                                          DskError      **error);

/* Iterating a DskTableReader.
   
   // create a new reader.  for an empty table,
   // this returns non-NULL, but reader->at_eof==TRUE.
   reader = dsk_table_new_reader (table, &error);
   if (reader == NULL)
     dsk_die ("%s", error->message);

   while (!reader->at_eof)
     {
       // process key/value
       ...

       // advance only returns FALSE on error.
       DskError *error = NULL;
       if (!reader->advance (reader, error))
         dsk_die ("%s", error->message);
     }

 */

/* internals */
DskTableReader *dsk_table_reader_new_merge2 (DskTable       *table,
                                             DskTableReader *a,
                                             DskTableReader *b,
                                             DskError      **error);

#if DSK_CAN_INLINE || defined(DSK_IMPLEMENT_INLINES)
DSK_INLINE_FUNC uint8_t *
dsk_table_buffer_set_size (DskTableBuffer *buffer,
                           unsigned        length)
{
  if (buffer->alloced < length)
    {
      unsigned alloced = buffer->alloced == 0 ? 16 : (buffer->alloced * 2);
      while (alloced < length)
        alloced *= 2;
      buffer->data = dsk_realloc (buffer->data, alloced);
      buffer->alloced = alloced;
    }
  buffer->length = length;
  return buffer->data;
}
#endif
