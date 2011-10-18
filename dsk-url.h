
typedef enum
{
  DSK_URL_SCHEME_UNKNOWN,

  DSK_URL_SCHEME_HTTP,
  DSK_URL_SCHEME_HTTPS,
  DSK_URL_SCHEME_FTP,
  DSK_URL_SCHEME_FILE,
  DSK_URL_SCHEME_MAILTO
  /* NOTE: we may add more schemes later */
} DskUrlScheme;
dsk_boolean dsk_url_scheme_parse (const char   *url,
                                  unsigned     *scheme_length_out,
                                  DskUrlScheme *scheme_out,
                                  DskError    **error);

typedef struct _DskUrlScanned DskUrlScanned;
struct _DskUrlScanned
{
  const char *scheme_start, *scheme_end;
  DskUrlScheme scheme;
  const char *username_start, *username_end;
  const char *password_start, *password_end;
  const char *host_start, *host_end;
  const char *port_start, *port_end;
  int port;
  const char *path_start, *path_end;
  const char *query_start, *query_end;
  const char *fragment_start, *fragment_end;
};

dsk_boolean  dsk_url_scan  (const char     *url_string,
                            DskUrlScanned  *out,
                            DskError      **error);
