
/* --- filters --- */
/* non-blocking data processing --- */
typedef struct _DskSyncFilterClass DskSyncFilterClass;
typedef struct _DskSyncFilter DskSyncFilter;
#define DSK_SYNC_FILTER(object)           DSK_OBJECT_CAST(DskSyncFilter, object, &dsk_sync_filter_class)
#define DSK_SYNC_FILTER_CLASS(object)     DSK_OBJECT_CLASS_CAST(DskSyncFilter, object, &dsk_sync_filter_class)
#define DSK_SYNC_FILTER_GET_CLASS(object) DSK_OBJECT_CAST_GET_CLASS(DskSyncFilter, object, &dsk_sync_filter_class)
struct _DskSyncFilterClass
{
  DskObjectClass base_class;
  dsk_boolean (*process) (DskSyncFilter *filter,
                          DskBuffer      *out,
                          unsigned        in_length,
                          const uint8_t  *in_data,
                          DskError      **error);
  dsk_boolean (*finish)  (DskSyncFilter *filter,
                          DskBuffer      *out,
                          DskError      **error);
};
struct _DskSyncFilter
{
  DskObject base_instance;
};
DSK_INLINE_FUNC dsk_boolean dsk_sync_filter_process (DskSyncFilter *filter,
                                                      DskBuffer      *out,
                                                      unsigned        in_length,
                                                      const uint8_t  *in_data,
                                                      DskError      **error);
dsk_boolean          dsk_sync_filter_process_buffer (DskSyncFilter *filter,
                                                      DskBuffer      *out,
                                                      unsigned        in_len,
                                                      DskBuffer      *in,
                                                      dsk_boolean     discard,
                                                      DskError      **error);
DSK_INLINE_FUNC dsk_boolean dsk_sync_filter_finish  (DskSyncFilter *filter,
                                                      DskBuffer      *out,
                                                      DskError      **error);



/* Filtering sources or sinks */
DskStream *dsk_stream_filter_read_end (DskOctetSource *source,
                                            DskSyncFilter *filter);
DskStream *dsk_stream_filter_write_end(DskOctetSource *sink,
                                            DskSyncFilter *filter);

/* hackneyed helper function: unrefs the filter. */
dsk_boolean dsk_filter_to_buffer  (unsigned length,
                                   const uint8_t *data,
                                   DskSyncFilter *filter,
                                   DskBuffer *output,
                                   DskError **error);
char       *dsk_filter_to_string  (unsigned length,
                                   const uint8_t *data,
                                   DskSyncFilter *filter,
                                   unsigned  *output_string_length_out_opt,
                                   DskError **error);
uint8_t    *dsk_filter_to_data    (unsigned length,
                                   const uint8_t *data,
                                   DskSyncFilter *filter,
                                   unsigned  *output_string_length_out,
                                   DskError **error);

#if DSK_CAN_INLINE || defined(DSK_IMPLEMENT_INLINES)
DSK_INLINE_FUNC dsk_boolean dsk_sync_filter_process (DskSyncFilter *filter,
                                                      DskBuffer      *out,
                                                      unsigned        in_length,
                                                      const uint8_t  *in_data,
                                                      DskError      **error)
{
  DskSyncFilterClass *c = DSK_SYNC_FILTER_GET_CLASS (filter);
  return c->process (filter, out, in_length, in_data, error);
}

DSK_INLINE_FUNC dsk_boolean dsk_sync_filter_finish  (DskSyncFilter *filter,
                                                      DskBuffer      *out,
                                                      DskError      **error)
{
  DskSyncFilterClass *c = DSK_SYNC_FILTER_GET_CLASS (filter);
  if (c->finish == NULL)
    return DSK_TRUE;
  return c->finish (filter, out, error);
}

#endif
