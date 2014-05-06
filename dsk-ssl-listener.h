

typedef struct _DskSslListener DskSslListener;
struct _DskSslListener
{
  DskOctetListener base_instance;
  DskOctetListener *underlying;
  DskHookTrap *underlying_trap;
  DskSslContext *ssl_context;
};

typedef struct _DskSslListenerOptions DskSslListenerOptions;
struct _DskSslListenerOptions
{
  DskOctetListenerSocketOptions underlying;
  DskSslContext *ssl_context;
};

#define DSK_SSL_LISTENER_OPTIONS_INIT               \
{                                                   \
  DSK_OCTET_LISTENER_SOCKET_OPTIONS_INIT,        \
  NULL                                              \
}

DskOctetListener *dsk_ssl_listener_new (DskSslListenerOptions *options,
                                        DskError             **error);
