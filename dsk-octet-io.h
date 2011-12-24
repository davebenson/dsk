typedef struct _DskOctetSourceClass DskOctetSourceClass;
typedef struct _DskOctetSinkClass DskOctetSinkClass;
typedef struct _DskOctetSource DskOctetSource;
typedef struct _DskOctetSink DskOctetSink;
typedef struct _DskOctetStreamClass DskOctetStreamClass;
typedef struct _DskOctetStream DskOctetStream;

struct _DskOctetSourceClass
{
  DskObjectClass base_class;

  DskIOResult (*read)        (DskOctetSource *source,
                              unsigned        max_len,
                              void           *data_out,
                              unsigned       *bytes_read_out,
                              DskError      **error);
  DskIOResult (*read_buffer) (DskOctetSource *source,
                              DskBuffer      *read_buffer,
                              DskError      **error);

  /* not always implemented */
  void        (*shutdown)    (DskOctetSource *source);
};

struct _DskOctetSource
{
  DskObject base_instance;
  DskOctetStream *stream;         /* optional */
  DskHook readable_hook;
};

struct _DskOctetSinkClass
{
  DskObjectClass base_class;

  DskIOResult (*write)        (DskOctetSink   *sink,
                               unsigned        max_len,
                               const void     *data_out,
                               unsigned       *n_written_out,
                               DskError      **error);
  DskIOResult (*write_buffer) (DskOctetSink   *sink,
                               DskBuffer      *write_buffer,
                               DskError      **error);

  /* not always implemented */
  void        (*shutdown)     (DskOctetSink   *sink);
};
struct _DskOctetSink
{
  DskObject base_instance;
  DskOctetStream *stream;         /* optional */
  DskHook writable_hook;
};


struct _DskOctetStreamClass
{
  DskObjectClass base_class;
};
struct _DskOctetStream
{
  DskObject base_instance;
  /* NOTE: source/sink hold a reference to Stream, NOT
     the other way around. */
  DskOctetSource *source;
  DskOctetSink *sink;
  DskHook error_hook;
  DskError *latest_error;
};
void dsk_octet_stream_set_last_error (DskOctetStream  *stream,
                                      const char      *format,
                                      ...) DSK_GNUC_PRINTF(2,3);
void dsk_octet_stream_set_error      (DskOctetStream  *stream,
                                      DskError        *error);

#define DSK_OCTET_SOURCE(object) DSK_OBJECT_CAST(DskOctetSource, object, &dsk_octet_source_class)
#define DSK_OCTET_SINK(object) DSK_OBJECT_CAST(DskOctetSink, object, &dsk_octet_sink_class)
#define DSK_OCTET_STREAM(object) DSK_OBJECT_CAST(DskOctetStream, object, &dsk_octet_stream_class)
#define DSK_OCTET_FILTER(object) DSK_OBJECT_CAST(DskOctetFilter, object, &dsk_octet_filter_class)
#define DSK_OCTET_SOURCE_GET_CLASS(object) DSK_OBJECT_CAST_GET_CLASS(DskOctetSource, object, &dsk_octet_source_class)
#define DSK_OCTET_SINK_GET_CLASS(object) DSK_OBJECT_CAST_GET_CLASS(DskOctetSink, object, &dsk_octet_sink_class)
#define DSK_OCTET_STREAM_GET_CLASS(object) DSK_OBJECT_CAST_GET_CLASS(DskOctetStream, object, &dsk_octet_stream_class)
#define DSK_OCTET_FILTER_GET_CLASS(object) DSK_OBJECT_CAST_GET_CLASS(DskOctetFilter, object, &dsk_octet_filter_class)

DSK_INLINE_FUNC DskIOResult dsk_octet_source_read (void         *octet_source,
                                                   unsigned      max_len,
                                                   void         *data_out,
                                                   unsigned     *n_read_out,
                                                   DskError    **error);
DSK_INLINE_FUNC DskIOResult dsk_octet_source_read_buffer (void           *octet_source,
                                                  DskBuffer      *read_buffer,
                                                  DskError      **error);
DSK_INLINE_FUNC void dsk_octet_source_shutdown   (void           *octet_source);
DSK_INLINE_FUNC void dsk_octet_source_detach       (DskOctetSource *source);
DSK_INLINE_FUNC DskIOResult dsk_octet_sink_write (void           *octet_sink,
                                                  unsigned        max_len,
                                                  const void     *data,
                                                  unsigned       *n_written_out,
                                                  DskError      **error);
DSK_INLINE_FUNC DskIOResult dsk_octet_sink_write_buffer  (void           *octet_sink,
                                                  DskBuffer      *write_buffer,
                                                  DskError      **error);
DSK_INLINE_FUNC void dsk_octet_sink_shutdown     (void           *octet_sink);
DSK_INLINE_FUNC void dsk_octet_sink_detach       (DskOctetSink   *sink);


int64_t dsk_octet_source_get_length (DskOctetSource *source); /* BIG HACK; may return -1 */

typedef struct _DskOctetConnectionClass DskOctetConnectionClass;
typedef struct _DskOctetConnection DskOctetConnection;
typedef struct _DskOctetConnectionOptions DskOctetConnectionOptions;

struct _DskOctetConnectionClass
{
  DskObjectClass base_class;
};
struct _DskOctetConnection
{
  DskObject base_instance;
  DskBuffer buffer;
  DskOctetSource *source;
  DskOctetSink *sink;
  DskHookTrap *read_trap;
  DskHookTrap *write_trap;

  unsigned max_buffer;
  unsigned shutdown_on_read_error : 1;
  unsigned shutdown_on_write_error : 1;

  /* tracking the latest error */
  unsigned last_error_from_reading : 1;
  DskError *last_error;
};

struct _DskOctetConnectionOptions
{
  unsigned max_buffer;
  dsk_boolean shutdown_on_read_error;
  dsk_boolean shutdown_on_write_error;
};
#define DSK_OCTET_CONNECTION_OPTIONS_INIT \
{ 4096,                         /* max_buffer */                \
  DSK_TRUE,                     /* shutdown_on_read_error */    \
  DSK_TRUE                      /* shutdown_on_write_error */   \
}

DSK_INLINE_FUNC void dsk_octet_connect       (DskOctetSource *source,
                                              DskOctetSink   *sink,
                                              DskOctetConnectionOptions *opt);

DskOctetConnection *dsk_octet_connection_new (DskOctetSource *source,
                                              DskOctetSink   *sink,
                                              DskOctetConnectionOptions *opt);
void                dsk_octet_connection_shutdown (DskOctetConnection *);
void                dsk_octet_connection_disconnect (DskOctetConnection *);

/* --- pipes --- */
/* Pipes are the opposite of connections, in some sense.
   In a connection, it tries to read from a source and write on a sink.
   In a pipe, data written to the sink will appear on a source;
*/
/* Specify pipe_buffer_size==0 to get the default buffer size */
void          dsk_octet_pipe_new (unsigned       pipe_buffer_size,
                                  DskOctetSink **sink_out,
                                  DskOctetSource **source_out);

/* --- filters --- */
/* non-blocking data processing --- */
typedef struct _DskOctetFilterClass DskOctetFilterClass;
typedef struct _DskOctetFilter DskOctetFilter;
struct _DskOctetFilterClass
{
  DskObjectClass base_class;
  dsk_boolean (*process) (DskOctetFilter *filter,
                          DskBuffer      *out,
                          unsigned        in_length,
                          const uint8_t  *in_data,
                          DskError      **error);
  dsk_boolean (*finish)  (DskOctetFilter *filter,
                          DskBuffer      *out,
                          DskError      **error);
};
struct _DskOctetFilter
{
  DskObject base_instance;
};
DSK_INLINE_FUNC dsk_boolean dsk_octet_filter_process (DskOctetFilter *filter,
                                                      DskBuffer      *out,
                                                      unsigned        in_length,
                                                      const uint8_t  *in_data,
                                                      DskError      **error);
dsk_boolean          dsk_octet_filter_process_buffer (DskOctetFilter *filter,
                                                      DskBuffer      *out,
                                                      unsigned        in_len,
                                                      DskBuffer      *in,
                                                      dsk_boolean     discard,
                                                      DskError      **error);
DSK_INLINE_FUNC dsk_boolean dsk_octet_filter_finish  (DskOctetFilter *filter,
                                                      DskBuffer      *out,
                                                      DskError      **error);



/* Filtering sources or sinks */
DskOctetSource *dsk_octet_filter_source (DskOctetSource *source,
                                         DskOctetFilter *filter);
DskOctetSink   *dsk_octet_filter_sink   (DskOctetSource *sink,
                                         DskOctetFilter *filter);


/* hackneyed helper function: unrefs the filter. */
dsk_boolean dsk_filter_to_buffer  (unsigned length,
                                   const uint8_t *data,
                                   DskOctetFilter *filter,
                                   DskBuffer *output,
                                   DskError **error);
char       *dsk_filter_to_string  (unsigned length,
                                   const uint8_t *data,
                                   DskOctetFilter *filter,
                                   unsigned  *output_string_length_out_opt,
                                   DskError **error);
uint8_t    *dsk_filter_to_data    (unsigned length,
                                   const uint8_t *data,
                                   DskOctetFilter *filter,
                                   unsigned  *output_string_length_out,
                                   DskError **error);
/* Defining subclasses of DskOctetFilter is easy and fun;
 */

#define DSK_OCTET_FILTER_SUBCLASS_DEFINE(class_static, ClassName, class_name) \
DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(ClassName);                                \
class_static ClassName##Class class_name ## _class = { {                      \
  DSK_OBJECT_CLASS_DEFINE(ClassName, &dsk_octet_filter_class,                 \
                          class_name ## _init,                                \
                          class_name ## _finalize),                           \
  class_name ## _process,                                                     \
  class_name ## _finish                                                       \
} }

extern const DskOctetSourceClass dsk_octet_source_class;
extern const DskOctetSinkClass dsk_octet_sink_class;
extern const DskOctetStreamClass dsk_octet_stream_class;
extern const DskOctetConnectionClass dsk_octet_connection_class;
extern const DskOctetFilterClass dsk_octet_filter_class;

#if DSK_CAN_INLINE || defined(DSK_IMPLEMENT_INLINES)
DSK_INLINE_FUNC DskIOResult dsk_octet_source_read (void         *octet_source,
                                           unsigned      max_len,
                                           void         *data_out,
                                           unsigned     *n_read_out,
                                           DskError    **error)
{
  DskOctetSourceClass *c = DSK_OCTET_SOURCE_GET_CLASS (octet_source);
  return c->read (octet_source, max_len, data_out, n_read_out, error);
}
DSK_INLINE_FUNC DskIOResult dsk_octet_source_read_buffer (void           *octet_source,
                                                  DskBuffer      *read_buffer,
                                                  DskError      **error)
{
  DskOctetSourceClass *c = DSK_OCTET_SOURCE_GET_CLASS (octet_source);
  return c->read_buffer (octet_source, read_buffer, error);
}
DSK_INLINE_FUNC void dsk_octet_source_shutdown (void           *octet_source)
{
  DskOctetSourceClass *c = DSK_OCTET_SOURCE_GET_CLASS (octet_source);
  if (c->shutdown != NULL)
    c->shutdown (octet_source);
}
DSK_INLINE_FUNC void dsk_octet_source_detach     (DskOctetSource *source)
{
  if (source->stream)
    {
      DskOctetStream *stream = source->stream;
      stream->source = NULL;
      source->stream = NULL;
      dsk_object_unref (stream);
    }
}
DSK_INLINE_FUNC DskIOResult dsk_octet_sink_write (void           *octet_sink,
                                                  unsigned        max_len,
                                                  const void     *data,
                                                  unsigned       *n_written_out,
                                                  DskError      **error)
{
  DskOctetSinkClass *c = DSK_OCTET_SINK_GET_CLASS (octet_sink);
  return c->write (octet_sink, max_len, data, n_written_out, error);
}
DSK_INLINE_FUNC DskIOResult dsk_octet_sink_write_buffer  (void           *octet_sink,
                                                  DskBuffer      *write_buffer,
                                                  DskError      **error)
{
  DskOctetSinkClass *c = DSK_OCTET_SINK_GET_CLASS (octet_sink);
  return c->write_buffer (octet_sink, write_buffer, error);
}
DSK_INLINE_FUNC void dsk_octet_sink_shutdown     (void           *octet_sink)
{
  DskOctetSinkClass *c = DSK_OCTET_SINK_GET_CLASS (octet_sink);
  if (c->shutdown != NULL)
    c->shutdown (octet_sink);
}
DSK_INLINE_FUNC void dsk_octet_sink_detach     (DskOctetSink *sink)
{
  if (sink->stream)
    {
      DskOctetStream *stream = sink->stream;
      stream->sink = NULL;
      sink->stream = NULL;
      dsk_object_unref (stream);
    }
}
DSK_INLINE_FUNC void dsk_octet_connect       (DskOctetSource *source,
                                              DskOctetSink   *sink,
                                              DskOctetConnectionOptions *opt)
{
  dsk_object_unref (dsk_octet_connection_new (source, sink, opt));
}
DSK_INLINE_FUNC dsk_boolean dsk_octet_filter_process (DskOctetFilter *filter,
                                                      DskBuffer      *out,
                                                      unsigned        in_length,
                                                      const uint8_t  *in_data,
                                                      DskError      **error)
{
  DskOctetFilterClass *c = DSK_OCTET_FILTER_GET_CLASS (filter);
  return c->process (filter, out, in_length, in_data, error);
}

DSK_INLINE_FUNC dsk_boolean dsk_octet_filter_finish  (DskOctetFilter *filter,
                                                      DskBuffer      *out,
                                                      DskError      **error)
{
  DskOctetFilterClass *c = DSK_OCTET_FILTER_GET_CLASS (filter);
  if (c->finish == NULL)
    return DSK_TRUE;
  return c->finish (filter, out, error);
}

#endif
