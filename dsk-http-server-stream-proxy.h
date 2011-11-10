
typedef struct _DskHttpServerStreamProxyOptions DskHttpServerStreamProxyOptions;
struct _DskHttpServerStreamProxyOptions
{
  dsk_boolean allow_http;
  dsk_boolean allow_https;             /* UNIMPLEMENTED */
  dsk_boolean allow_ftp;               /* UNIMPLEMENTED */

  char *destination_url;

  /* add Via header (technically required by HTTP protocol; practically,
     this is used to operate in "stealth" mode,
     e.g. for evading IP address quotas) */
  dsk_boolean include_via;

  /* hostname for the via header; otherwise use gethostname() */
  const char *via_hostname;

  /* to issue a full-on different HTTP request */
  DskHttpClientRequestOptions *client_options;
};

/* Even if this returns FALSE the transfer is still destroyed */
dsk_boolean
dsk_http_server_stream_respond_proxy
                               (DskHttpServerStreamTransfer *transfer,
                                DskHttpServerStreamProxyOptions *options,
                                DskError **error);

