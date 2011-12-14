#include "dsk.h"

typedef struct _DskSslListenerClass DskSslListenerClass;
struct _DskSslListenerClass
{
  DskOctetListenerClass base_class;
};

static void
dsk_ssl_listener_init (DskSslListener *listener)
{
  dsk_hook_set_funcs (&rv->base_instance.incoming,
                      &dsk_ssl_listener_hook_funcs);
}

static void
dsk_ssl_listener_finalize (DskSslListener *listener)
{
  if (listener->underlying_trap)
    dsk_hook_trap_free (listener->underlying_trap);
  if (listener->underlying)
    dsk_object_unref (listener->underlying);
}

static DskIOResult dsk_ssl_listener_accept (DskOctetListener        *listener,
                                            DskOctetStream         **stream_out,
                                            DskOctetSource         **source_out,
                                            DskOctetSink           **sink_out,
                                            DskError               **error);
static void dsk_ssl_listener_shutdown(DskOctetListener       *listener);

static DskSslListenerClass dsk_ssl_listener_class =
{
  {
    DSK_OBJECT_CLASS_DEFINE (DskSslListener, &dsk_octet_listener_class,
                             dsk_ssl_listener_init, dsk_ssl_listener_finalize),
    dsk_ssl_listener_accept,
    dsk_ssl_listener_shutdown
  }
};

static dsk_boolean
handle_underlying_incoming (DskOctetListener *underlying_listener,
                            DskSslListener   *ssl_listener)
{
  dsk_hook_notify (&ssl_listener->base_instance.incoming);
  return DSK_TRUE;
}

static void
dsk_ssl_listener_set_poll (DskSslListener *listener,
                           dsk_boolean     trap)
{
  if (listener->underlying == NULL)
    return;
  if (trap && listener->underlying_trap == NULL)
    {
      listener->underlying_trap = dsk_hook_trap (&listener->underlying->incoming,
                                                 (DskHookFunc) handle_underlying_incoming,
                                                 listener,
                                                 (DskHookDestroy) handle_underlying_incoming_destroy);
    }
  else if (trap && listener->underlying_trap != NULL)
    {
      dsk_hook_trap_free (listener->underlying_trap);
      listener->underlying_trap = NULL;
    }
}

static DskHookFuncs dsk_ssl_listener_hook_funcs =
{
  (DskHookObjectFunc) dsk_object_ref_r,
  (DskHookObjectFunc) dsk_object_unref_r,
  (DskHookSetPoll) dsk_ssl_listener_set_poll
};

DskOctetListener *
dsk_ssl_listener_new (DskSslListenerOptions *options,
                      DskError             **error)
{
  DskOctetListener *underlying;
  DskSslListener *rv;
  underlying = dsk_octet_listener_socket_new (&options->underlying, error);
  if (underlying == NULL)
    return NULL;
  rv = dsk_object_new (&dsk_ssl_listener_class);
  rv->underlying = underlying;
  return rv;
}

static DskIOResult dsk_ssl_listener_accept (DskOctetListener        *listener,
                                            DskOctetStream         **stream_out,
                                            DskOctetSource         **source_out,
                                            DskOctetSink           **sink_out,
                                            DskError               **error)
{
  DskSslListener *slis = DSK_SSL_LISTENER (listener);
  ...
}
static void dsk_ssl_listener_shutdown(DskOctetListener       *listener)
{
  DskSslListener *slis = DSK_SSL_LISTENER (listener);
  ...
}

