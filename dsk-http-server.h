

typedef struct _DskHttpServerRequest DskHttpServerRequest;
typedef struct _DskHttpServerBindInfo DskHttpServerBindInfo;
typedef struct _DskHttpServer DskHttpServer;

struct _DskHttpServerRequest
{
  DskHttpServer *server;
  DskHttpServerStreamTransfer *transfer;
  DskHttpRequest *request_header;
  DskHttpServerBindInfo *bind_info;

  dsk_boolean cgi_variables_computed;
  unsigned n_cgi_variables;
  DskCgiVariable *cgi_variables;

  DskIpAddress client_ip_address;
  unsigned client_ip_port;

  dsk_boolean has_raw_post_data;
  size_t raw_post_data_size;
  uint8_t *raw_post_data;
};

typedef dsk_boolean (*DskHttpServerTestFunc)   (DskHttpServerRequest *request,
                                                void                 *func_data);
typedef void (*DskHttpServerStreamingPostFunc) (DskHttpServerRequest *request,
                                                DskOctetSource       *post_data,
                                                void                 *func_data);
typedef void (*DskHttpServerCgiFunc)           (DskHttpServerRequest *request,
                                                void                 *func_data);
#if 0
typedef enum 
{
  DSK_HTTP_SERVER_TRY_OK,
  DSK_HTTP_SERVER_TRY_PASS,
  DSK_HTTP_SERVER_TRY_ERROR
} DskHttpServerTryResult;
typedef DskHttpServerTryResult (*DskHttpServerTryFunc)
                                               (DskHttpServerRequest *request,
                                                void                *func_data,
                                                DskError           **error);
#endif

/* callback for websockets.  Must respond with 
   dsk_http_server_request_websocket_success()
   or dsk_http_server_request_respond_error(). */
typedef void (*DskHttpServerWebsocketFunc)   (DskHttpServerRequest*request,
                                              char               **protocols,
                                              void                *func_data);

/* MOST OF THESE FUNCTIONS CAN ONLY BE CALLED BEFORE THE SERVER IS STARTED */

typedef enum
{
  DSK_HTTP_SERVER_MATCH_VERB,
  DSK_HTTP_SERVER_MATCH_PATH,
  DSK_HTTP_SERVER_MATCH_HOST,
  DSK_HTTP_SERVER_MATCH_USER_AGENT,
  DSK_HTTP_SERVER_MATCH_BIND_PORT,
  DSK_HTTP_SERVER_MATCH_BIND_PATH,
} DskHttpServerMatchType;

DskHttpServer * dsk_http_server_new (void);

void dsk_http_server_add_match                 (DskHttpServer        *server,
                                                DskHttpServerMatchType type,
                                                const char           *pattern);
void dsk_http_server_add_match_verb            (DskHttpServer        *server,
                                                DskHttpVerb           verb);
void dsk_http_server_match_save                (DskHttpServer        *server);
void dsk_http_server_match_restore             (DskHttpServer        *server);

/* TODO */
//void dsk_http_server_match_add_auth          (DskHttpServer        *server,
                                                //????);
 
dsk_boolean dsk_http_server_bind_tcp           (DskHttpServer        *server,
                                                DskIpAddress         *bind_addr,
                                                unsigned              port,
                                                DskError            **error);
dsk_boolean dsk_http_server_bind_local         (DskHttpServer        *server,
                                                const char           *path,
                                                DskError            **error);
void        dsk_http_server_bind               (DskHttpServer        *server,
                                                DskOctetListener     *listener);


void
dsk_http_server_register_streaming_post_handler (DskHttpServer *server,
                                                 DskHttpServerStreamingPostFunc func,
                                                 void          *func_data,
                                                 DskHookDestroy destroy);

void
dsk_http_server_register_cgi_handler            (DskHttpServer *server,
                                                 DskHttpServerCgiFunc func,
                                                 void          *func_data,
                                                 DskHookDestroy destroy);

void
dsk_http_server_register_websocket_handler      (DskHttpServer *server,
                                                 DskHttpServerWebsocketFunc func,
                                                 void          *func_data,
                                                 DskHookDestroy destroy);



/* One of these functions should be called by any handler */
typedef struct _DskHttpServerResponseOptions DskHttpServerResponseOptions;
struct _DskHttpServerResponseOptions
{
  /* Various ways of specifying the content-body:
       - as a stream ("source") and optional content-length;
       - from a file ("source_filename");
       - from a buffer ("source_buffer");
       - from a slab of memory ("content_body") with content_length set.
   */
  DskOctetSource *source;
  const char *source_filename;
  DskBuffer *source_buffer;
  const uint8_t *content_body;
  int64_t content_length;

  /* single string content_type: eg "text/plain" or "text/plain/utf-8" */
  const char *content_type;

  /* ... or by components */
  const char *content_main_type;
  const char *content_sub_type;
  const char *content_charset;

};
#define DSK_HTTP_SERVER_RESPONSE_OPTIONS_INIT                   \
{                                                               \
  NULL,                 /* source */                            \
  NULL,                 /* source_filename */                   \
  NULL,                 /* source_buffer */                     \
  NULL,                 /* content_body */                      \
  -1LL,                 /* content_length */                    \
  NULL,                 /* content_type */                      \
  NULL,                 /* content_main_type */                 \
  NULL,                 /* content_sub_type */                  \
  NULL,                 /* content_charset */                   \
}

void dsk_http_server_request_respond          (DskHttpServerRequest *request,
                                               DskHttpServerResponseOptions *options);
void dsk_http_server_request_respond_error    (DskHttpServerRequest *request,
                                               DskHttpStatus         status,
                                               const char           *message);
void dsk_http_server_request_redirect         (DskHttpServerRequest *request,
                                               DskHttpStatus         status,
                                               const char           *location);
void dsk_http_server_request_internal_redirect(DskHttpServerRequest *request,
                                               const char           *new_path);
void dsk_http_server_request_pass             (DskHttpServerRequest *request);
dsk_boolean
     dsk_http_server_request_respond_websocket(DskHttpServerRequest *request,
                                               const char           *protocol,
                                               DskWebsocket        **sock_out);

DskCgiVariable *dsk_http_server_request_lookup_cgi (DskHttpServerRequest *request,
                                               const char           *name);
