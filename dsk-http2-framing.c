//
// See https://tools.ietf.org/html/rfc7540
//
typedef struct DskHttp2ServerClass DskHttp2ServerClass;
typedef struct DskHttp2Server DskHttp2Server;
struct DskHttp2ServerClass
{
  DskObjectClass base_class;
};

struct DskHttp2Server
{
  DskObject base_instance;
  DskOctetListener *listener;
  DskHoopTrap *listener_trap;
  DskHttp2ServerCallbacks *callbacks;
  void *callback_data;
};

typedef enum
{
  CONNECTION_FRAMING_STATE_INIT,
  CONNECTION_FRAMING_STATE_EXPECTING_HTTP1_HEADER,
  CONNECTION_FRAMING_STATE_WAITING_FOR_HEADER,
  CONNECTION_FRAMING_STATE_GOT_HEADER,
} ConnectionFramingState;

static void
dsk_http2_server_finalize (DskHttp2Server *server)
{
  if (server->listener_trap != NULL)
    {
      DskHookTrap *trap = server->listener_trap;
      server->listener_trap = NULL;
      dsk_hook_untrap (trap);
    }
  if (server->listener != NULL)
    {
      dsk_object_unref (server->listener);
      server->listener = NULL;
    }
}

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskHttp2Server);
static DskHttp2ServerClass server_class = {
  DSK_OBJECT_CLASS_DEFINE(DskHttp2Server, &dsk_object_class, NULL, dsk_http2_server_finalize)
};

#define DSK_OBJECT_CLASS_DEFINE(name, parent_class, init_func, finalize_func) \

typedef struct {
  uint32_t length;
  DskHttp2FrameType type;
  DskHttp2FrameFlags flags;
  uint32_t stream_id;
} DskHttp2FrameHeader;


typedef struct Connection Connection;
struct Connection
{
  DskHttp2Connection base;
  DskBuffer incoming;
  DskBuffer outgoing;
};



#define DSK_HTTP2_FRAME_HEADER_LENGTH   9

typedef struct DskHttp2Header DskHttp2Header;
struct DskHttp2Header
{
  const char *key;
  const char *value;
  DskHttp2HeaderCode code;
};
  

typedef struct Http2StreamSinkClass Http2StreamSinkClass;
typedef struct Http2StreamSink Http2StreamSink;
typedef struct Http2StreamSourceClass Http2StreamSourceClass;
typedef struct Http2StreamSource Http2StreamSource;

struct Http2StreamSinkClass
{
  DskOctetSinkClass base_class;
};
struct Http2StreamSink
{
  DskOctetSink base_instance;
};
struct Http2StreamSourceClass
{
  DskOctetSourceClass base_class;
};
struct Http2StreamSource
{
  DskOctetSource base_instance;
};


struct DskHttp2Stream
{
  DskOctetStream base_stream;

  uint32_t stream_id;
  unsigned n_request_headers;
  DskHttp2Header **request_headers;
  unsigned n_response_headers;
  DskHttp2Header **response_headers;
  DskHttp2StreamCallbacks *callbacks;
  void *callback_data;
  DskHttp2Stream *next_in_hash;
  DskHttp2Connection *connection;

  unsigned request_headers_alloced;
  unsigned response_headers_alloced;
};

typedef struct {
  size_t n_stream_bins;
  DskHttp2Stream **streams_by_bin;
  size_t n_streams;
  DskHttp2Server *server;               // if it's a server-connection
} DskHttp2Connection;

typedef struct DskHttp2Server DskHttp2Server;
struct DskHttp2ServerCallbacks
{
  void (*connection_started) (DskHttp2Server     *server,
                              DskHttp2Connection *connection,
                              void               *callback_data);
  void (*stream_started)     (DskHttp2Server     *server,
                              DskHttp2Connection *connection,
                              DskHttp2Stream     *stream,
                              void               *callback_data);
};

struct DskHttp2ServerClass
{
  DskObjectClass base_class;
};


struct DskHttp2Server
{
  DskObject base_instance;
  DskOctetListener *listener;
  DskHttp2ServerCallbacks *callbacks;
  void *callback_data;
};
DskHttp2Server *
dsk_http2_server_new (DskHttp2ServerOptions   *options,
                      DskError               **error)
{
  DskOctetListener *listener;
  if (options->listener != NULL)
    listener = dsk_object_ref (options->listener);
  else if (options->use_ssl)
    {
      listener = ...
      if (listener == NULL)
        return NULL;
    }
  else
    {
      listener = dsk_octet_listener_socket_new (&opts, error);
      if (listener == NULL)
        return NULL;
    }

  DskHttp2Server *rv = dsk_object_new (dsk_http2_stream_class);
  rv->callbacks = callbacks;
  rv->callback_data = callback_data;
  rv->listener = listener;
  rv->listener_trap = dsk_hook_trap (&listener->incoming,
                                     handle_octet_listener_ready,
                                     rv,
                                     NULL);
  return rv;
}

static bool
handle_connection_stream_readable (DskOctetSource *source,
                                   Connection     *conn)
{
  size_t orig_incoming_size = conn.incoming.size;
  DskBufferFragment *new_data_start_frag = conn->incoming.last_frag;
  const uint8_t *new_data_start_ptr = NULL;
  if (new_data_start_frag != NULL)
    {
      new_data_start_ptr = new_data_start_frag->buf
                         + new_data_start_frag->buf_start
                         + new_data_start_frag->buf_length;
    }
  switch (dsk_octet_source_read_buffer (source, &conn->incoming, &conn->error))
    {
    case DSK_IO_RESULT_SUCCESS:
      break;
    case DSK_IO_RESULT_AGAIN:
      return true;
    case DSK_IO_RESULT_EOF:
      if (conn->incoming.size > 0)
        {
          // partial frame
          ...
        }
      if (!connection_close_all_streams (conn))
        return false;
      return true;
    case DSK_IO_RESULT_ERROR:
      ...
    }
  if (new_data_start_frag == NULL)
    {
      new_data_start_frag = conn->incoming.first_frag;

      // treat no data as a non-event, even though it's not clear it
      // should ever happen.
      if (new_data_start_frag == NULL)
        return true;

      new_data_start_ptr = new_data_start_frag->buf + new_data_start_frag->buf_start;
    }
  switch (conn->framing_state)
    {
      case CONN_FRAMING_STATE_EXPECTING_HTTP1_HEADER:
        {
          DskBufferIterator iter;
          if (orig_incoming_size > 0)
            {
              ...
            }
          else
            {
              ...
            }
          
          ...
        }
      case CONN_FRAMING_STATE_EXPECTING_HTTP1_HEADER_LAST_WAS_NEWLINE:
        {
        }

      case CONNECTION_FRAMING_STATE_WAITING_FOR_HEADER:
      waiting_for_header:
        {
        }

      case CONNECTION_FRAMING_STATE_GOT_HEADER:
      got_header:
        {
        }
    }
  assert(false);
}

static Connection *
new_connection (DskOctetStream *transport,
                DskOctetSink   *sink,
                DskOctetSource *source)
{
  Connection *conn = DSK_NEW0 (Connection);
  conn->transport = transport;
  conn->transport_sink = sink;
  conn->transport_source = source;
  conn->read_trap = dsk_hook_trap (&transport->source->readable_hook,
                                   (DskHookTrapFunc) handle_connection_stream_readable,
                                   conn,
                                   NULL);
  return conn;
}

DskHttp2Connection    *
dsk_http2_client_connection_new (DskHttp2ClientConnectionOptions *options)
{
  DskOctetStream *stream;
  if (options->use_ssl)
    {
      if (!dsk_ssl_stream_new (options->ssl_client_options,
                               &stream, &sink, &source,
                               error))
        {
          ...
        }
    }
  else
    {
      if (!dsk_client_stream_new (options->client_options, 
                                  &stream, &sink, &source,
                                  error))
        {
          ...
        }
    }

  Connection *rv = new_connection (stream, sink, source);
  if (options->do_http1_upgrade)
    {
      ...
      rv->expected_http1_response = true;
    }
  return rv;
}

/* --- connection API --- */
void                   dsk_http2_connection_go_away    (DskHttp2Connection *conn);
DskHttp2Stream        *dsk_http2_connection_create_stream(DskHttp2Connection *connection);

/* --- stream APIs --- */
void                   dsk_http2_stream_add_headers    (DskHttp2Stream     *stream,
                                                        unsigned            n_headers,
                                                        DskHttp2Header     *headers,
                                                        bool                done_adding);
