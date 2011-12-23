

typedef struct _DskHttpsServerOptions DskHttpsServerOptions;
struct _DskHttpsServerOptions
{
  DskIpAddress          bind_address;
  unsigned              port;
  DskSslContext        *context;
};

dsk_boolean dsk_http_server_bind_https         (DskHttpServer        *server,
                                                DskHttpsServerOptions *options,
                                                DskError            **error);
