typedef struct DskStreamClass DskStreamClass;
typedef struct DskStream DskStream;

#define DSK_STREAM(object) DSK_OBJECT_CAST(DskStream, object, &dsk_stream_class)
#define DSK_STREAM_GET_CLASS(object) DSK_OBJECT_CAST_GET_CLASS(DskStream, object, &dsk_stream_class)

struct DskStreamClass
{
  DskObjectClass base_class;

  DskIOResult (*read)        (DskStream      *stream,
                              unsigned        max_len,
                              void           *data_out,
                              unsigned       *bytes_read_out,
                              DskError      **error);
  DskIOResult (*read_buffer) (DskStream      *stream,
                              DskBuffer      *read_buffer,
                              DskError      **error);

  /* not always implemented */
  void        (*shutdown_read)(DskStream      *stream);
  DskIOResult (*write)        (DskStream      *stream,
                               unsigned        max_len,
                               const void     *data_out,
                               unsigned       *n_written_out,
                               DskError      **error);
  DskIOResult (*write_buffer) (DskStream      *stream,
                               DskBuffer      *write_buffer,
                               DskError      **error);

  /* not always implemented */
  void        (*shutdown_write)(DskStream   *stream);
};
struct DskStream
{
  DskObject base_instance;
  DskHook readable_hook;
  DskHook writable_hook;
  DskHook error_hook;
  DskError *latest_error;
};
void dsk_stream_set_last_error       (DskStream       *stream,
                                      const char      *format,
                                      ...) DSK_GNUC_PRINTF(2,3);
void dsk_stream_set_error            (DskStream       *stream,
                                      DskError        *error);

DSK_INLINE_FUNC DskIOResult dsk_stream_read (void         *stream,
                                                   unsigned      max_len,
                                                   void         *data_out,
                                                   unsigned     *n_read_out,
                                                   DskError    **error);
DSK_INLINE_FUNC DskIOResult dsk_stream_read_buffer (void           *stream,
                                                  DskBuffer      *read_buffer,
                                                  DskError      **error);
DSK_INLINE_FUNC void dsk_stream_shutdown_read   (void           *stream);
DSK_INLINE_FUNC DskIOResult dsk_stream_write (void           *stream,
                                                  unsigned        max_len,
                                                  const void     *data,
                                                  unsigned       *n_written_out,
                                                  DskError      **error);
DSK_INLINE_FUNC DskIOResult dsk_stream_write_buffer  (void           *stream,
                                                  DskBuffer      *write_buffer,
                                                  DskError      **error);
DSK_INLINE_FUNC void dsk_stream_shutdown_write     (void           *stream);

DSK_INLINE_FUNC bool dsk_stream_is_readable       (void           *stream);
DSK_INLINE_FUNC bool dsk_stream_is_writable       (void           *stream);


int64_t dsk_stream_get_length (DskStream *stream); /* BIG HACK; may return -1 */

typedef struct _DskStreamConnectionClass DskStreamConnectionClass;
typedef struct _DskStreamConnection DskStreamConnection;
typedef struct _DskStreamConnectionOptions DskStreamConnectionOptions;

struct _DskStreamConnectionClass
{
  DskObjectClass base_class;
};
struct _DskStreamConnection
{
  DskObject base_instance;
  DskBuffer buffer;
  DskStream *source;
  DskStream *sink;
  DskHookTrap *read_trap;
  DskHookTrap *write_trap;

  unsigned max_buffer;
  unsigned shutdown_on_read_error : 1;
  unsigned shutdown_on_write_error : 1;

  /* tracking the latest error */
  unsigned last_error_from_reading : 1;
  DskError *last_error;
};

struct _DskStreamConnectionOptions
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

DSK_INLINE_FUNC void dsk_stream_connect       (DskStream *source,
                                              DskStream   *sink,
                                              DskStreamConnectionOptions *opt);

DskStreamConnection *dsk_stream_connection_new (DskStream *source,
                                                DskStream   *sink,
                                                DskStreamConnectionOptions *opt);
void                dsk_stream_connection_shutdown (DskStreamConnection *);
void                dsk_stream_connection_disconnect (DskStreamConnection *);

/* --- pipes --- */
/* Pipes are the opposite of connections, in some sense.
   In a connection, it tries to read from a source and write on a sink.
   In a pipe, data written to the sink will appear on a source;
*/
/* Specify pipe_buffer_size==0 to get the default buffer size */
void          dsk_pipe_new (unsigned       pipe_buffer_size,
                            DskStream **sink_out,
                            DskStream **source_out);


extern const DskStreamClass dsk_stream_class;
extern const DskStreamConnectionClass dsk_stream_connection_class;

#if DSK_CAN_INLINE || defined(DSK_IMPLEMENT_INLINES)
DSK_INLINE_FUNC DskIOResult dsk_stream_read (void         *stream,
                                           unsigned      max_len,
                                           void         *data_out,
                                           unsigned     *n_read_out,
                                           DskError    **error)
{
  DskStreamClass *c = DSK_STREAM_GET_CLASS (stream);
  return c->read (stream, max_len, data_out, n_read_out, error);
}
DSK_INLINE_FUNC DskIOResult dsk_stream_read_buffer (void           *stream,
                                                  DskBuffer      *read_buffer,
                                                  DskError      **error)
{
  DskStreamClass *c = DSK_STREAM_GET_CLASS (stream);
  return c->read_buffer (stream, read_buffer, error);
}
DSK_INLINE_FUNC void dsk_stream_shutdown_read (void           *stream)
{
  DskStreamClass *c = DSK_STREAM_GET_CLASS (stream);
  if (c->shutdown_read != NULL)
    c->shutdown_read (stream);
}
DSK_INLINE_FUNC DskIOResult dsk_stream_write (void           *stream,
                                                  unsigned        max_len,
                                                  const void     *data,
                                                  unsigned       *n_written_out,
                                                  DskError      **error)
{
  DskStreamClass *c = DSK_STREAM_GET_CLASS (stream);
  return c->write (stream, max_len, data, n_written_out, error);
}
DSK_INLINE_FUNC DskIOResult dsk_stream_write_buffer  (void           *stream,
                                                  DskBuffer      *write_buffer,
                                                  DskError      **error)
{
  DskStreamClass *c = DSK_STREAM_GET_CLASS (stream);
  return c->write_buffer (stream, write_buffer, error);
}
DSK_INLINE_FUNC void dsk_stream_shutdown_write     (void           *stream)
{
  DskStreamClass *c = DSK_STREAM_GET_CLASS (stream);
  if (c->shutdown_write != NULL)
    c->shutdown_write (stream);
}
DSK_INLINE_FUNC void dsk_stream_connect       (DskStream *source,
                                               DskStream   *sink,
                                               DskStreamConnectionOptions *opt)
{
  dsk_object_unref (dsk_stream_connection_new (source, sink, opt));
}
DSK_INLINE_FUNC bool dsk_stream_is_readable       (void           *stream)
{
  return !dsk_hook_is_cleared (&DSK_STREAM (stream)->readable_hook);
}
DSK_INLINE_FUNC bool dsk_stream_is_writable       (void           *stream)
{
  return !dsk_hook_is_cleared (&DSK_STREAM (stream)->writable_hook);
}
DSK_INLINE_FUNC bool dsk_stream_is_writable       (void           *stream);
#endif
