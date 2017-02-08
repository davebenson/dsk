typedef struct _DskHttpClientStreamClass DskHttpClientStreamClass;
typedef struct _DskHttpClientStream DskHttpClientStream;
typedef struct _DskHttpClientStreamFuncs DskHttpClientStreamFuncs;
typedef struct _DskHttpClientStreamTransfer DskHttpClientStreamTransfer;
typedef struct _DskHttpClientStreamRequestOptions DskHttpClientStreamRequestOptions;

struct _DskHttpClientStreamClass
{
  DskObjectClass base_class;
};
struct _DskHttpClientStream
{
  DskObject base_instance;
  DskOctetSink *sink;
  DskHookTrap *write_trap;
  DskOctetSource *source;
  DskHookTrap *read_trap;
  DskBuffer incoming_data;
  DskBuffer outgoing_data;
  DskHttpClientStreamTransfer *first_transfer, *last_transfer;
  DskHttpClientStreamTransfer *outgoing_data_transfer;

  DskError *latest_error;
  DskHook error_hook;

  /* invariant: this is the index of 'outgoing_data_transfer' in the xfer list */
  unsigned n_pending_outgoing_requests;

  /* config */
  unsigned max_header_size;
  unsigned max_pipelined_requests;
  unsigned max_outgoing_data;

  unsigned strict_keepalive : 1;
  unsigned print_warnings : 1;
};

typedef struct _DskHttpClientStreamOptions DskHttpClientStreamOptions;
struct _DskHttpClientStreamOptions
{
  unsigned max_header_size;
  unsigned max_pipelined_requests;
  unsigned max_outgoing_data;
  dsk_boolean strict_keepalive;
  dsk_boolean print_warnings;
};

#define DSK_HTTP_CLIENT_STREAM_OPTIONS_INIT              \
{                                                           \
  8192,                 /* max_header_size */               \
  4,                    /* max_pipelined_requests */        \
  8192,                 /* max_outgoing_data */             \
  DSK_FALSE,            /* strict_keepalive */              \
  DSK_DEBUG             /* print_warnings */                \
}

DskHttpClientStream *
dsk_http_client_stream_new     (DskOctetSink        *sink,
                                DskOctetSource      *source,
                                const DskHttpClientStreamOptions *options);
//void dsk_http_client_stream_shutdown (DskHttpClientStream *stream);

typedef enum
{
  DSK_HTTP_CLIENT_STREAM_ERROR_PREMATURE_SHUTDOWN,
  DSK_HTTP_CLIENT_STREAM_ERROR_BAD_HEADER,
  DSK_HTTP_CLIENT_STREAM_ERROR_BAD_CONTENT,
} DskHttpClientStreamError;

struct _DskHttpClientStreamFuncs
{
  /* called once the http header is received.
     content may or may not be complete. */
  void (*handle_response)         (DskHttpClientStreamTransfer *transfer);

  /* called once content is completely received */
  void (*handle_content_complete) (DskHttpClientStreamTransfer *transfer);

  /* call for any number of errors */
  void (*handle_error)            (DskHttpClientStreamTransfer *transfer);

  /* always called exactly once after all other functions done */
  void (*destroy)                 (DskHttpClientStreamTransfer *transfer);
};

struct _DskHttpClientStreamRequestOptions
{
  /* only one of these should be set */
  DskHttpRequestOptions *request_options;
  DskHttpRequest *request;

  /* typically only for POST, PUT, etc.
   * post_data_len may be combined with post_data or post_data_slab.
   * if post_data_len > 0, either post_data or post_data_slab is non-null.
   * if post_data_len == -1 and post_Data is non-null,
   * then we use transfer-encoding chunked.
   */
  DskOctetSource *post_data;
  int64_t post_data_length;
  const uint8_t *post_data_slab;
  const char *content_type;
  int gzip_compression_level;

  /* content-encoding gzip for the post-data */
  dsk_boolean gzip_compress_post_data;          /* gzip post-data internally */
  dsk_boolean post_data_is_gzipped;             /* assume post-data is already gzipped */

  /* Content-encoding gzip for the content-body is
     governed by 'uncompress_content':
     - If true, then Content-Encoding: gzip will be handled automatically.
     - If false, then you must expect gzipped data, or, from within
       funcs->handle_response, the request can call
       dsk_http_client_stream_transfer_add_content_filter()
   */
  dsk_boolean uncompress_content;       

  /* Cookies */
  unsigned n_cookies;
  DskHttpCookie *cookies;

  /* websocket support */
  dsk_boolean is_websocket_request;
  const char *websocket_protocols;

  /* functions and user-data */
  DskHttpClientStreamFuncs *funcs;
  void *user_data;


};

#define DSK_HTTP_CLIENT_STREAM_REQUEST_OPTIONS_INIT \
{                                                      \
  NULL,                /* request_options */           \
  NULL,                /* request */                   \
  NULL,                /* post_data */                 \
  -1LL,                /* post_data_len */             \
  NULL,                /* post_data_slab */            \
  NULL,                /* content_type */              \
  3,                   /* gzip_compression_level */    \
  DSK_FALSE,           /* gzip_compress_post_data */   \
  DSK_FALSE,           /* post_data_is_gzipped */      \
  DSK_TRUE,            /* uncompress_content */        \
  0,                   /* n_cookies */                 \
  NULL,                /* cookies */                   \
  DSK_FALSE,           /* is_websocket_request */      \
  NULL,                /* websocket_protocols */       \
  NULL,                /* funcs */                     \
  NULL                 /* user_data */                 \
}

/* note that 'funcs' must exist for the duration of the request.
 * usually this is done by having a static DskHttpClientStreamFuncs.
 * You could also free 'funcs' in the 'destroy' method.
 */
DskHttpClientStreamTransfer *
dsk_http_client_stream_request (DskHttpClientStream      *stream,
                                DskHttpClientStreamRequestOptions *options,
                                DskError                **error);

/* internals */
typedef enum
{
  DSK_HTTP_CLIENT_STREAM_READ_NEED_HEADER,
  DSK_HTTP_CLIENT_STREAM_READ_IN_BODY,
  DSK_HTTP_CLIENT_STREAM_READ_IN_BODY_EOF,
  DSK_HTTP_CLIENT_STREAM_READ_IN_XFER_CHUNKED_HEADER,
  DSK_HTTP_CLIENT_STREAM_READ_IN_XFER_CHUNKED_HEADER_EXTENSION,
  DSK_HTTP_CLIENT_STREAM_READ_IN_XFER_CHUNK,
  DSK_HTTP_CLIENT_STREAM_READ_AFTER_XFER_CHUNK,
  DSK_HTTP_CLIENT_STREAM_READ_AFTER_XFER_CHUNKED, /* after final chunk */
  DSK_HTTP_CLIENT_STREAM_READ_XFER_CHUNK_TRAILER,
  //DSK_HTTP_CLIENT_STREAM_READ_XFER_CHUNK_FINAL_NEWLINE,
  DSK_HTTP_CLIENT_STREAM_READ_DONE,
  DSK_HTTP_CLIENT_STREAM_READ_WAITING_FOR_WEBSOCKET_HEADER
} DskHttpClientStreamReadState;
typedef enum
{
  DSK_HTTP_CLIENT_STREAM_WRITE_INIT,
  DSK_HTTP_CLIENT_STREAM_WRITE_CONTENT,         /* in post/put data */
  DSK_HTTP_CLIENT_STREAM_WRITE_DONE
} DskHttpClientStreamWriteState;

struct _DskHttpClientStreamTransfer
{
  /*< public >*/
  DskHttpClientStream *owner;
  DskHttpRequest *request;
  DskOctetSource *post_data;
  DskHttpResponse *response;
  DskMemorySource *content;      
  DskHttpClientStreamTransfer *next;
  DskHttpClientStreamFuncs *funcs;
  DskWebsocket *websocket;
  void *user_data;
  unsigned failed : 1;
  unsigned uncompress_content : 1;

  /* for transparent handling of content-encoding gzip */
  DskOctetFilter *content_decoder;

  /*< private >*/
  DskHttpClientStreamReadState read_state;
  /* branch of union depends on 'read_state' */
  union {
    /* number of bytes we've already checked for end of header. */
    struct { unsigned checked; } need_header;

    /* number of bytes remaining in content-length */
    struct { uint64_t remaining; } in_body;

    /* number of bytes remaining in current chunk */
    /* same structure for in_xfer_chunk_header */
    struct { uint64_t remaining; } in_xfer_chunk;

    struct { unsigned checked; } in_xfer_chunk_trailer;
    /* no data for DONE */
  } read_info;

  DskHttpClientStreamWriteState write_state;
  union {
    struct { DskHookTrap *post_data_trap; uint64_t bytes; } in_content;
  } write_info;

  struct {
    uint8_t key3[8];
    uint8_t response[16];
  } websocket_info;
};

void dsk_http_client_stream_shutdown (DskHttpClientStream *stream);

/* --- interface to CONNECT --- */
typedef void (*DskHttpClientStreamConnectFunc) (DskOctetSource  *source,
                                                DskOctetSink    *sink,
                                                DskHttpResponse *response,
                                                void            *data);
typedef void (*DskHttpClientStreamConnectFail) (DskError        *error,
                                                DskHttpResponse *response,
                                                void            *data);

