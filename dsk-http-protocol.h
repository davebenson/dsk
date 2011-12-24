/* TODO: this file should be sorted according to the order
 * in RFC 2616.
 */


typedef struct _DskHttpRequestClass DskHttpRequestClass;
typedef struct _DskHttpRequest DskHttpRequest;
typedef struct _DskHttpResponseClass DskHttpResponseClass;
typedef struct _DskHttpResponse DskHttpResponse;

/* The DskHttpStatus's value is the numerical status code, so
   often for well-known codes like 200 or 404, it's
   easier just to use the code... but i recommend the
   enum even in those cases, to make your intent clear. */
typedef enum
{
  DSK_HTTP_STATUS_CONTINUE                      = 100,
  DSK_HTTP_STATUS_SWITCHING_PROTOCOLS           = 101,
  DSK_HTTP_STATUS_OK                            = 200,
  DSK_HTTP_STATUS_CREATED                       = 201,
  DSK_HTTP_STATUS_ACCEPTED                      = 202,
  DSK_HTTP_STATUS_NONAUTHORITATIVE_INFO         = 203,
  DSK_HTTP_STATUS_NO_CONTENT                    = 204,
  DSK_HTTP_STATUS_RESET_CONTENT                 = 205,
  DSK_HTTP_STATUS_PARTIAL_CONTENT               = 206,
  DSK_HTTP_STATUS_MULTIPLE_CHOICES              = 300,
  DSK_HTTP_STATUS_MOVED_PERMANENTLY             = 301,
  DSK_HTTP_STATUS_FOUND                         = 302,
  DSK_HTTP_STATUS_SEE_OTHER                     = 303,
  DSK_HTTP_STATUS_NOT_MODIFIED                  = 304,
  DSK_HTTP_STATUS_USE_PROXY                     = 305,
  DSK_HTTP_STATUS_TEMPORARY_REDIRECT            = 306,
  DSK_HTTP_STATUS_BAD_REQUEST                   = 400,
  DSK_HTTP_STATUS_UNAUTHORIZED                  = 401,
  DSK_HTTP_STATUS_PAYMENT_REQUIRED              = 402,
  DSK_HTTP_STATUS_FORBIDDEN                     = 403,
  DSK_HTTP_STATUS_NOT_FOUND                     = 404,
  DSK_HTTP_STATUS_METHOD_NOT_ALLOWED            = 405,
  DSK_HTTP_STATUS_NOT_ACCEPTABLE                = 406,
  DSK_HTTP_STATUS_PROXY_AUTH_REQUIRED           = 407,
  DSK_HTTP_STATUS_REQUEST_TIMEOUT               = 408,
  DSK_HTTP_STATUS_CONFLICT                      = 409,
  DSK_HTTP_STATUS_GONE                          = 410,
  DSK_HTTP_STATUS_LENGTH_REQUIRED               = 411,
  DSK_HTTP_STATUS_PRECONDITION_FAILED           = 412,
  DSK_HTTP_STATUS_ENTITY_TOO_LARGE              = 413,
  DSK_HTTP_STATUS_URI_TOO_LARGE                 = 414,
  DSK_HTTP_STATUS_UNSUPPORTED_MEDIA             = 415,
  DSK_HTTP_STATUS_BAD_RANGE                     = 416,
  DSK_HTTP_STATUS_EXPECTATION_FAILED            = 417,
  DSK_HTTP_STATUS_INTERNAL_SERVER_ERROR         = 500,
  DSK_HTTP_STATUS_NOT_IMPLEMENTED               = 501,
  DSK_HTTP_STATUS_BAD_GATEWAY                   = 502,
  DSK_HTTP_STATUS_SERVICE_UNAVAILABLE           = 503,
  DSK_HTTP_STATUS_GATEWAY_TIMEOUT               = 504,
  DSK_HTTP_STATUS_UNSUPPORTED_VERSION           = 505
} DskHttpStatus;
const char *dsk_http_status_get_message (DskHttpStatus status_code);

/*
 * The Verb is the first text transmitted
 * (from the user-agent to the server).
 */
typedef enum
{
  DSK_HTTP_VERB_GET,
  DSK_HTTP_VERB_POST,
  DSK_HTTP_VERB_PUT,
  DSK_HTTP_VERB_HEAD,
  DSK_HTTP_VERB_OPTIONS,
  DSK_HTTP_VERB_DELETE,
  DSK_HTTP_VERB_TRACE,
  DSK_HTTP_VERB_CONNECT
} DskHttpVerb;
const char *dsk_http_verb_name (DskHttpVerb verb);

dsk_boolean dsk_http_has_response_body (DskHttpVerb request_verb,
                                        DskHttpStatus response_status_code);

/* A single `Cookie' or `Set-Cookie' header.
 *
 * See RFC 2109, HTTP State Management Mechanism 
 */
typedef struct _DskHttpCookie DskHttpCookie;
struct _DskHttpCookie
{
  const char *key;
  const char *value;
  const char *domain;
  const char *path;
  const char *expire_date;
  const char *comment;
  int         max_age;
  dsk_boolean secure;               /* default is FALSE */
  unsigned    version;              /* default is 0, unspecified */
};

/* (maintainers: DskHttpHeaderMisc must never change from
   being a simple key/value pair. 
   see casts in handling unparsed_headers/unparsed_misc_headers) */
typedef struct _DskHttpHeaderMisc DskHttpHeaderMisc;
struct _DskHttpHeaderMisc
{
  char *key;
  char *value;
};

struct _DskHttpRequestClass
{
  DskObjectClass base_class;
};

struct _DskHttpRequest
{
  DskObject base_object;

  //DskHttpConnection             connection_type;
  //DskHttpContentEncoding        content_encoding_type;
  DskHttpVerb verb;       /* the command: GET, PUT, POST, HEAD, etc */
  /* Note that HTTP/1.1 servers must accept the entire
   * URL being included in `path'! (maybe including http:// ... */
  char *path;
  unsigned char http_minor_version;
  unsigned char http_major_version;

  unsigned transfer_encoding_chunked : 1;       /* for POST data */
  unsigned has_date : 1;           /* Date (see date member) */
  unsigned connection_close : 1;
  unsigned connection_upgrade : 1;
  unsigned content_encoding_gzip : 1;
  unsigned supports_content_encoding_gzip : 1;
  unsigned is_websocket_request : 1;

  char *content_type;  /* Content-Type: text/plain or text/plain/UTF-8 */
  char **content_type_kv_pairs;
  dsk_time_t date;        /* Date header, if "has_date" */

  int64_t content_length; /* From the Content-Length header; -1 to disable */

  char *host;                 /* Host */
  char *user_agent;           /* User-Agent */
  char *referrer;             /* Referer */

  unsigned n_unparsed_headers;
  DskHttpHeaderMisc *unparsed_headers;

  char *_slab;
};
const char * dsk_http_request_get (DskHttpRequest *request,
                                   const char     *header);

struct _DskHttpResponseClass
{
  DskObjectClass base_class;
};
struct _DskHttpResponse
{
  DskObject base_object;

  uint8_t http_major_version;             /* always 1 */
  uint8_t http_minor_version;

  DskHttpStatus status_code;

  unsigned connection_close : 1;
  unsigned connection_upgrade : 1;
  unsigned transfer_encoding_chunked : 1;
  unsigned accept_ranges : 1; /* Accept-Ranges */
  unsigned has_date : 1;           /* Date (see date member) */
  unsigned content_encoding_gzip : 1;
  unsigned has_last_modified : 1;
  unsigned has_expires : 1;

  /* Content-Type */
  char *content_type;
  char **content_type_kv_pairs;

  /* the 'Date' header, parsed into unix-time, i.e.
     seconds since epoch (if the has_date flag is set) */
  dsk_time_t date;

  /* From the Content-Length header; -1 to disable */
  dsk_time_t content_length;

  unsigned n_unparsed_headers;
  DskHttpHeaderMisc *unparsed_headers;
  

  unsigned                  has_md5sum : 1;
  unsigned char             md5sum[16];           /* Content-MD5 (14.15) */

#if 0
  /* List of Set-Cookie: headers. */
  unsigned                  n_set_cookies;
  DskHttpCookie            *set_cookies;
#endif

  /* The `Location' to redirect to. */
  char                     *location;

  dsk_time_t                expires;

#if 0
  DskHttpAuthenticate      *proxy_authenticate;

  /* This is the WWW-Authenticate: header line. */
  DskHttpAuthenticate      *authenticate;
#endif

  /* The Last-Modified header.  If != -1, this is the unix-time
   * the message-body-contents were last modified. (RFC 2616, section 14.29)
   */
  dsk_time_t                last_modified;

  char                     *server;        /* The Server: header */

  char *_slab;
};
extern DskHttpRequestClass dsk_http_request_class;
extern DskHttpResponseClass dsk_http_response_class;

DskHttpRequest  *dsk_http_request_parse_buffer  (DskBuffer *buffer,
                                                 unsigned   header_len,
                                                 DskError **error);
DskHttpResponse *dsk_http_response_parse_buffer (DskBuffer *buffer,
                                                 unsigned   header_len,
                                                 DskError **error);
void             dsk_http_request_print_buffer  (DskHttpRequest *request,
                                                 DskBuffer *buffer);
void             dsk_http_response_print_buffer  (DskHttpResponse *response,
                                                 DskBuffer *buffer);

/* --- construction --- */
typedef struct _DskHttpRequestOptions DskHttpRequestOptions;
typedef struct _DskHttpResponseOptions DskHttpResponseOptions;
struct _DskHttpRequestOptions
{
  DskHttpVerb verb;

  unsigned http_major_version;
  unsigned http_minor_version;

  /* specify either the full path */
  char *full_path;

  /* ... or its components */
  char *path;
  char *query;

  /* --- host --- */
  char *host;
  
  /* --- post-data --- */
  /* text/plain or text/plain/utf-8 */
  char *content_type;

  /* ... or by components */
  char *content_main_type;
  char *content_sub_type;
  char *content_charset;

  /* additional content-type information */
  char **content_type_kv_pairs;

  /* --- content-length --- */
  int64_t content_length;
  dsk_boolean content_encoding_gzip;
  dsk_boolean transfer_encoding_chunked;

  /* --- date --- */
  dsk_boolean has_date;
  dsk_time_t date;

  /* --- various headers, mostly uninterpreted --- */
  char *referrer;
  char *user_agent;

  /* --- needed for websockets --- */
  dsk_boolean connection_upgrade;

  /* --- unparsed headers --- */
  unsigned n_unparsed_headers;
  char **unparsed_headers;          /* key-value pairs */
  DskHttpHeaderMisc *unparsed_misc_headers;

  /* --- parser interface --- */
  dsk_boolean parsed;
  dsk_boolean parsed_transfer_encoding_chunked;
  dsk_boolean parsed_connection_close;
};
DskHttpRequest *dsk_http_request_new (DskHttpRequestOptions *options,
                                      DskError             **error);
void            dsk_http_request_init_options (DskHttpRequest *request,
                                               DskHttpRequestOptions *out);

#define DSK_HTTP_REQUEST_OPTIONS_INIT                           \
{                                                               \
  DSK_HTTP_VERB_GET,            /* status_code */               \
  1,                            /* http_major_version */        \
  1,                            /* http_minor_version */        \
  NULL,                         /* full_path */                 \
  NULL,                         /* path */                      \
  NULL,                         /* query */                     \
  NULL,                         /* host */                      \
  NULL,                         /* content_type */              \
  NULL,                         /* content_main_type */         \
  NULL,                         /* content_sub_type */          \
  NULL,                         /* content_charset */           \
  NULL,                         /* content_type_kv_pairs */     \
  -1LL,                         /* content_length */            \
  DSK_FALSE,                    /* content_encoding_gzip */     \
  DSK_FALSE,                    /* transfer_encoding_chunked */ \
  DSK_FALSE, 0LL,               /* has_date, date */            \
  NULL,                         /* referrer */                  \
  NULL,                         /* user_agent */                \
  DSK_FALSE,                    /* connection_upgrade */        \
  0, NULL,                      /* n_unparsed_headers, unparsed_headers  */\
  NULL,                         /* unparsed_misc_headers */     \
  DSK_FALSE,                    /* parsed */                    \
  DSK_FALSE,                    /* parsed_transfer_encoding_chunked */ \
  DSK_FALSE,                    /* parsed_connection_close   */ \
}
struct _DskHttpResponseOptions
{
  DskHttpRequest *request;

  DskHttpStatus status_code;
  unsigned http_major_version;
  unsigned http_minor_version;
  dsk_boolean connection_close;
  dsk_boolean connection_upgrade;

  /* --- post-data --- */
  /* text/plain or text/plain/utf-8 */
  const char *content_type;

  /* ... or by components */
  const char *content_main_type;
  const char *content_sub_type;
  const char *content_charset;

  /* additional content-type information */
  char **content_type_kv_pairs;

  /* --- content-length --- */
  int64_t content_length;

  /* --- content-encoding --- */
  dsk_boolean content_encoding_gzip;

  /* --- date --- */
  dsk_boolean has_date;
  dsk_time_t date;

  /* --- expires --- */
  dsk_boolean has_expires;
  dsk_time_t expires;

  /* --- unparsed strings --- */
  char *server;
  char *location;

  /* --- unparsed headers --- */
  unsigned n_unparsed_headers;
  char **unparsed_headers;          /* key-value pairs */
  DskHttpHeaderMisc *unparsed_misc_headers;     /* alternate to unparsed_headers */

  /* --- parser interface --- */
  dsk_boolean parsed;
  dsk_boolean parsed_transfer_encoding_chunked;
  dsk_boolean parsed_connection_close;
};
#define DSK_HTTP_RESPONSE_OPTIONS_INIT                          \
{                                                               \
  NULL,                         /* request */                   \
  200,                          /* status_code */               \
  1,                            /* http_major_version */        \
  1,                            /* http_minor_version */        \
  DSK_FALSE,                    /* connection_close */          \
  DSK_FALSE,                    /* connection_upgrade */        \
  NULL,                         /* content_type */              \
  NULL,                         /* content_main_type */         \
  NULL,                         /* content_sub_type */          \
  NULL,                         /* content_charset */           \
  NULL,                         /* content_type_kv_pairs */     \
  -1LL,                         /* content_length */            \
  DSK_FALSE,                    /* content_encoding_gzip */     \
  DSK_FALSE,                    /* has_date */                  \
  0LL,                          /* date */                      \
  DSK_FALSE,                    /* has_expires */               \
  0LL,                          /* expires */                   \
  NULL,                         /* server */                    \
  NULL,                         /* location */                  \
  0, NULL,                      /* n_unparsed_headers, unparsed_headers  */\
  NULL,                         /* unparsed_misc_headers */     \
  DSK_FALSE,                    /* parsed */                    \
  DSK_FALSE,                    /* parsed_transfer_encoding_chunked */ \
  DSK_FALSE,                    /* parsed_connection_close   */ \
}
DskHttpResponse *dsk_http_response_new (DskHttpResponseOptions *options,
                                        DskError              **error);
