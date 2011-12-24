
typedef struct _DskHttpClient DskHttpClient;
typedef struct _DskHttpClientClass DskHttpClientClass;
typedef struct _DskHttpClientRequestOptions DskHttpClientRequestOptions;

typedef struct _DskHttpClientTransfer DskHttpClientTransfer;

typedef struct _DskHttpClientRequestFuncs DskHttpClientRequestFuncs;
struct _DskHttpClientRequestFuncs
{
  /* handle_done/handle_fail:
     one of these two functions is always invoked once. */

  /* handle_done:
   * All the response header and body have been downloaded
     and processed successfully.
   */
  void (*handle_done)     (DskHttpClientTransfer *xfer,
                           void                  *func_data);

  /* handle_fail:
   * An error -- or many errors -- have caused the endeavor
   * to receive a successful response to fail.
   *
   * We will failures to connect, retry 504 responses
   * and all things that look like network noise (bad MD5,
   * cut-off in the middle of content).
   * But there is a maximum number of retries.
   *
   * And things like 404 headers fail after one try.
   *
   * And requests other than HEAD and GET are not retried.
   * (NOT FINAL)
   */
  void (*handle_fail)     (DskHttpClientTransfer *xfer,
                           DskError              *error,
                           void                  *func_data);

  /* --- low-level notifications (called before done/fail) --- */
  void (*handle_response)    (DskHttpClientTransfer *xfer,
                              void                  *func_data);
  void (*handle_stream)      (DskHttpClientTransfer *xfer,
                              void                  *func_data);

  void (*handle_redirecting) (DskHttpClientTransfer *xfer,
                              void                  *func_data);

  void (*handle_retrying)    (DskHttpClientTransfer *xfer,
                              void                  *func_data);

  /* error: this may or may not be fatal; use the handle_fail()
     to trap terminate errors */
  void (*handle_error)       (DskHttpClientTransfer *xfer,
                              DskError              *error,
                              void                  *func_data);


  /* called after handle_{done,fail} */
  void (*handle_destroy)  (DskHttpClientTransfer *xfer,
                           void                  *func_data);
};

struct _DskHttpClientClass
{
  DskObjectClass base_class;
};
struct _DskHttpClient
{
  DskObject base_instance;
};

typedef struct _DskHttpClientOptions DskHttpClientOptions;
struct _DskHttpClientOptions
{
  unsigned max_pipelined;
  unsigned max_connections;
  unsigned max_connections_per_host;
  unsigned max_connections_total;

  /* logging options?  or maybe a "trap" system that can be used for it? */
};

/* TODO: provide some sort of evidence that these are good numbers */
#define DSK_HTTP_CLIENT_OPTIONS_INIT               \
{                                                     \
  0,        /* max_pipelined */                       \
  100,      /* max_connections */                     \
  5,        /* max_connections_per_host */            \
  10000     /* max_connections_total */               \
}

DskHttpClient *dsk_http_client_new (DskHttpClientOptions *options);

/* host specific configuration */
typedef struct _DskHttpClientHostOptions DskHttpClientHostOptions;
struct _DskHttpClientHostOptions
{
  int max_pipelined;
  int max_connections;
};
#define DSK_HTTP_CLIENT_HOST_OPTIONS_INIT         \
{                                                    \
  -1,                                                \
  -1                                                 \
}
void dsk_http_client_configure_host (DskHttpClient *client,
                                     const char    *name,
                                     unsigned       port,
                                     DskHttpClientHostOptions *options);


struct _DskHttpClientRequestOptions
{
  /* Select location */
  /* by URL ... */
  char *url;

  /* ... or by URL pieces */
  char *host;
  unsigned port;                /* 0 = unspecified */
  char *path;
  char *query;

  /* hint to skip DNS lookups, give a direct address instead */
  dsk_boolean has_ip_address;
  DskIpAddress ip_address;

  /* use a local socket (aka a unix-domain socket) 
     instead of the actual IP address/dns lookup.
     This is mostly for testing. */
  char *local_socket_path;

  /* GET CGI variables: will be added to query string if it exists,
     and a query string (starting with a '?') will be added otherwise. */
  unsigned n_extra_get_cgi_variables;
  DskCgiVariable *extra_get_cgi_variables;

  dsk_boolean always_pipeline;
  dsk_boolean never_pipeline;

  /* These are ignored unless VERB=POST.
     They are also ignored if post_cgi_vars==NULL (note that
     n_post_cgi_vars==0 && post_cgi_vars == NULL => no mime-multipart;
     n_post_cgi_vars==0 && post_cgi_vars != NULL => mime-multipart w/ 0 parts)
   */
  unsigned n_post_cgi_variables;
  DskCgiVariable *post_cgi_variables;

  /* A stream of POST/PUT data. */
  DskOctetSource *request_body;

  /* content-retrieval modes:
      - normal stream: may have fatal error reading stream
      - stream with restart: may have nonfatal error reading stream, in which case
        a new stream may appear (handle_stream and handle response
        will be called multiple times)
      - safe mode: download and verify contents before. */
  dsk_boolean safe_mode;

  dsk_boolean may_restart_stream;

  /* --- unparsed headers --- */
  unsigned n_unparsed_headers;
  char **unparsed_headers;          /* key-value pairs */
  DskHttpHeaderMisc *unparsed_misc_headers;

  /* Number of milliseconds to keepalive this connection */
  int keepalive_millis;

  /* Force connection-close */
  dsk_boolean connection_close;

  /* Allow Content-Encoding gzip (will be TRUE once supported) */
  dsk_boolean allow_gzip;

  /* POST data support */

  /* Provide POST-data MD5Sum */
  dsk_boolean has_postdata_md5sum;
  uint8_t postdata_md5sum_binary[16];
  char *postdata_md5sum_hex;

  /* Force the POST data to be gzipped. */
  dsk_boolean gzip_post_data;

  /* MD5Sum checking support */
  dsk_boolean check_md5sum;

  /* Retry support */
  int max_retries;
  unsigned retry_sleep_millis;

  /* Redirect support */
  unsigned max_redirects;

  /* --- timeouts --- */

  /* max for a single HTTP request (from the DNS lookup starting
     to the response finishing) */
  int max_request_time_millis;

  /* max time for the content download to start */
  int max_start_millis;

  /* max time for the content download to finish */
  int max_millis;

  /* In "safe" mode, where we download the stream
     before notifying the user, these are the limits
     of the largest payload we may accept. */
  size_t safe_max_memory;
  uint64_t safe_max_disk;
  
  /* Cookies to send */
  unsigned n_cookies;
  DskHttpCookie *cookies;

  DskHttpClientRequestFuncs   *funcs;
  void *func_data;
  DskDestroyNotify destroy;

  /* TODO:
     - caching support
     - authentification support
     - SSL support */
};

/* If this returns DSK_FALSE, then NONE
   of your callbacks has been/will be run */
dsk_boolean  dsk_http_client_request  (DskHttpClient               *client,
				       DskHttpClientRequestOptions *options,
				       DskOctetSource              *post_data,
                                       DskError                   **error);

