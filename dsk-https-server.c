#include "dsk.h"

dsk_boolean dsk_http_server_bind_https         (DskHttpServer        *server,
                                                DskHttpsServerOptions *options,
                                                DskError            **error)
{
  DskSslListenerOptions lis_options = DSK_SSL_LISTENER_OPTIONS_INIT;
  lis_options.underlying.bind_address = options->bind_address;
  lis_options.underlying.bind_port = options->port;
  lis_options.ssl_context = options->context;
  DskOctetListener *listener = dsk_ssl_listener_new (&lis_options, error);
  if (listener == NULL)
    return DSK_FALSE;
  dsk_http_server_bind (server, listener);
  dsk_object_unref (listener);
  return DSK_TRUE;
}
