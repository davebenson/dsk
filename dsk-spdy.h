

/* --- one TCP connection corresponds to a SPDY session; 
       this is the raw framing layer --- */
       
typedef struct _DskSpdySession DskSpdySession;
struct _DskSpdySession
{
  DskObject       base_instance;

  /* underlying transport */
  DskOctetStream *stream;
  DskOctetSource *source;
  DskOctetSink   *sink;

  DskBuffer       incoming;
  DskBuffer       outgoing;

  DskHook         new_stream;
  DskHook         error_hook;
  DskError       *error;

  uint32_t        next_stream_id;
  uint32_t        next_ping_id;

  /*< private >*/

  /* a rbtree of streams */
  DskSpdyStream  *streams_by_id;

  unsigned n_pending_streams;
  DskSpdyStream  *first_pending_stream, *last_pending_stream;
};
#define DSK_SPDY_SESSION_IS_CLIENT(session) ((session)->next_stream_id & 1)

DskSpdySession*dsk_spdy_session_new                   (dsk_boolean     is_client_side,
                                                       DskOctetStream *stream,
                                                       DskOctetSource *source,
                                                       DskOctetSink   *sink,
						       DskBuffer      *initial_data,
						       DskError      **error);

DskSpdySession*dsk_spdy_session_new_client_tcp        (const char     *name,
                                                       DskIpAddress   *address,
						       unsigned        port,
						       DskError      **error);

/* Returns the number of stream requests from the other side,
   which may be larger than max_return. */
unsigned       dsk_spdy_session_get_pending_stream_ids(DskSpdySession *session,
                                                       unsigned        max_return,
                                                       uint32_t       *stream_ids_out);

/* Returns a reference that you must unref */
DskSpdyStream  *dsk_spdy_session_get_stream           (DskSpdySession *session,
                                                       uint32_t        stream_id,
                                                       DskSpdyHeaders *reply_headers);

DskSpdyHeaders *dsk_spdy_session_peek_stream_headers  (DskSpdySession *session,
                                                       uint32_t        stream_id);


typedef struct _DskSpdyStreamClass DskSpdyStreamClass;
typedef struct _DskSpdyStream DskSpdyStream;
typedef enum
{
  /* Request originating from our side has not gotten SYN_REPLY or RST_STREAM. */
  DSK_SPDY_STREAM_STATE_WAITING_FOR_ACK,

  /* Request originating from the other side; we have not called get_stream()
     or reject_stream(). */
  DSK_SPDY_STREAM_STATE_PENDING,

  /* Running stream (directionality can be figured by looking
     at 'stream->source' and 'stream->sink') */
  DSK_SPDY_STREAM_STATE_OK,

  /* Closed in both directions */
  DSK_SPDY_STREAM_STATE_CLOSED,

  /* Since cancellation has no ack, there's no "cancelling" state.
     It could have been us, or the remote side that cancelled. */
  DSK_SPDY_STREAM_STATE_CANCELLED,

  /* Something the other side sent was bad:
     effect is similar to the remote side cancelling. */
  DSK_SPDY_STREAM_STATE_PROTOCOL_ERROR,

  /* All streams are put in this state if
     the underlying stream goes down,
     or if the framing-layer goes bad. */
  DSK_SPDY_STREAM_STATE_TRANSPORT_ERROR
} DskSpdyStreamState;

struct _DskSpdyStreamClass
{
  DskObjectClass base_class;
};
struct _DskSpdyStream
{
  DskObject base_instance;
  DskSpdyHeaders *request_header;
  DskSpdyHeaders *response_header;
  DskSpdyStreamState state;
  DskOctetSource *source;
  DskOctetSink *sink;
  zstream header_compressor;
  zstream header_decompressor;

  DskSpdyStream *left, *right, *parent;
  dsk_boolean is_red;
  DskSpdyStream *prev_pending, *next_pending;
};
extern DskSpdyStreamClass dsk_spdy_stream_class;
#define DSK_SPDY_STREAM(stream) DSK_OBJECT_CAST(DskSpdyStream, stream, &dsk_spdy_stream_class)

DskSpdyStream *dsk_spdy_session_make_stream           (DskSpdySession *session,
						       DskSpdyHeaders *request_headers,
                                                       DskSpdyResponseFunc func,
                                                       void           *func_data);

void           dsk_spdy_stream_cancel                 (DskSpdyStream  *stream);

/* --- low-level HTTP support --- */
DskSpdyHeaders *dsk_spdy_headers_from_http_request    (DskHttpRequest *request);
DskSpdyHeaders *dsk_spdy_headers_from_http_response   (DskHttpResponse*response);
DskHttpRequest *dsk_spdy_headers_to_http_request      (DskSpdyHeaders *request,
                                                       DskError      **error);
DskHttpResponse*dsk_spdy_headers_to_http_response     (DskSpdyHeaders *response,
                                                       DskError      **error);
