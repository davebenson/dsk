typedef struct _DskHttpServerStreamClass DskHttpServerStreamClass;
typedef struct _DskHttpServerStream DskHttpServerStream;
typedef struct _DskHttpServerStreamFuncs DskHttpServerStreamFuncs;
typedef struct _DskHttpServerStreamTransfer DskHttpServerStreamTransfer;
typedef struct _DskHttpServerStreamOptions DskHttpServerStreamOptions;
typedef struct _DskHttpServerStreamResponseOptions DskHttpServerStreamResponseOptions;

struct _DskHttpServerStreamClass
{
  DskObjectClass base_instance;
};
struct _DskHttpServerStream
{
  DskObject base_instance;
  DskOctetSink *sink;
  DskOctetSource *source;
  DskBuffer incoming_data;
  DskBuffer outgoing_data;
  DskHookTrap *read_trap;
  DskHookTrap *write_trap;

  unsigned wait_for_content_complete : 1;
  unsigned no_more_transfers : 1;
  unsigned deferred_shutdown : 1;
  unsigned shutdown : 1;

  /* front of list */
  DskHttpServerStreamTransfer *first_transfer;

  /* end of list */
  DskHttpServerStreamTransfer *last_transfer;

  DskHttpServerStreamTransfer *read_transfer;

  /* to be returned by dsk_http_server_stream_get_request() */
  DskHttpServerStreamTransfer *next_request;
  DskHook request_available;

  unsigned max_header_size;
  unsigned max_pipelined_requests;
  uint64_t max_post_data_size;
  unsigned max_outgoing_buffer_size;
};

struct _DskHttpServerStreamOptions
{
  dsk_boolean wait_for_content_complete;
  unsigned max_header_size;
  unsigned max_pipelined_requests;
  unsigned max_post_data_pending;
  uint64_t max_post_data_size;
  unsigned max_outgoing_buffer_size;
};
#define DSK_HTTP_SERVER_STREAM_OPTIONS_INIT          \
{                                                       \
  DSK_TRUE,             /* wait_for_content_complete */ \
  16*1024,              /* max_header_size */           \
  6,                    /* max_pipelined_requests */    \
  8192,                 /* max_post_data_pending */     \
  64*1024,              /* max_post_data_size */        \
  64*1024               /* max_outgoing_buffer_size */  \
}

/* internals */
typedef enum
{
  DSK_HTTP_SERVER_STREAM_READ_NEED_HEADER,
  DSK_HTTP_SERVER_STREAM_READ_IN_POST,
  DSK_HTTP_SERVER_STREAM_READ_IN_XFER_CHUNKED_HEADER,
  DSK_HTTP_SERVER_STREAM_READ_IN_XFER_CHUNKED_HEADER_EXTENSION,
  DSK_HTTP_SERVER_STREAM_READ_IN_XFER_CHUNK,
  DSK_HTTP_SERVER_STREAM_READ_AFTER_XFER_CHUNK,
  DSK_HTTP_SERVER_STREAM_READ_XFER_CHUNK_TRAILER,
  DSK_HTTP_SERVER_STREAM_READ_DONE,
  DSK_HTTP_SERVER_STREAM_READ_WEBSOCKET
} DskHttpServerStreamReadState;
struct _DskHttpServerStreamTransfer
{
  DskHttpServerStream *owner;
  DskHttpRequest *request;
  DskMemorySource *post_data;      
  DskHttpResponse *response;
  DskOctetSource *content;
  DskHookTrap *content_readable_trap;
  DskHookTrap *buffer_low_trap;         /* notify us when we are unblocked */
  unsigned returned : 1;   /* has this transfer been returned by get_request? */
  unsigned responded : 1;  /* dsk_http_server_stream_respond called */
  unsigned failed : 1;
  unsigned blocked_content : 1;
  DskHttpServerStreamReadState read_state;
  /* branch of union depends on 'read_state' */
  union {
    /* number of bytes we've already checked for end of header. */
    struct { unsigned checked; } need_header;

    /* number of bytes remaining in content-length */
    struct { uint64_t remaining; } in_post;

    /* number of bytes remaining in current chunk */
    struct { uint64_t remaining; } in_xfer_chunk;

    struct { unsigned checked; } in_xfer_chunk_trailer;
    /* no data for DONE */
  } read_info;

  DskHttpServerStreamFuncs *funcs;
  void *func_data;

  /*< private >*/
  DskHttpServerStreamTransfer *next;
};

struct _DskHttpServerStreamFuncs
{
  void (*error_notify)       (DskHttpServerStreamTransfer *transfer,
                              DskError                    *error);
  void (*post_data_complete) (DskHttpServerStreamTransfer *transfer);
  void (*post_data_failed)   (DskHttpServerStreamTransfer *transfer);
  void (*destroy)            (DskHttpServerStreamTransfer *transfer);
};

DskHttpServerStream *
dsk_http_server_stream_new     (DskOctetSink        *sink,
                                DskOctetSource      *source,
                                DskHttpServerStreamOptions *options);

/* You may only assume the transfer is alive until
   you call transfer_respond() below.
   You must dsk_object_ref() post_data if you want to keep it around;
   we only retain a weak-reference */
DskHttpServerStreamTransfer *
dsk_http_server_stream_get_request (DskHttpServerStream *stream);
void
dsk_http_server_stream_transfer_set_funcs (DskHttpServerStreamTransfer *xfer,
                                           DskHttpServerStreamFuncs    *funcs,
                                           void                        *data);

void
dsk_http_server_stream_shutdown (DskHttpServerStream *stream);

struct  _DskHttpServerStreamResponseOptions
{
  /* Provide either an HTTP response or the Options you want to copy into the new response */
  DskHttpResponseOptions *header_options;
  DskHttpResponse *header;

  /* Content handling */
  DskOctetSource *content_stream;
  int64_t content_length;            /* -1 means "no content_data */
  const uint8_t *content_body;
};
#define DSK_HTTP_SERVER_STREAM_RESPONSE_OPTIONS_INIT \
{                                                       \
  NULL,                 /* header_options */            \
  NULL,                 /* header */                    \
  NULL,                 /* content_stream */            \
  -1LL,                 /* content_length */            \
  NULL,                 /* content_body */              \
}


/* Even if this returns FALSE the transfer is still destroyed */
dsk_boolean
dsk_http_server_stream_respond (DskHttpServerStreamTransfer *transfer,
                                DskHttpServerStreamResponseOptions *options,
                                DskError **error);

/* Respond to a websocket connection request. */
dsk_boolean
dsk_http_server_stream_respond_websocket
                               (DskHttpServerStreamTransfer *transfer,
                                const char        *protocol,
                                DskWebsocket     **websocket_out);

/* See also: dsk_http_server_stream_respond_proxy() in
 * dsk-http-server-stream-proxy.h */

extern DskHttpServerStreamClass dsk_http_server_stream_class;
