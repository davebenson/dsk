#include "dsk.h"

typedef struct _DskSslListenerClass DskSslListenerClass;
struct _DskSslListenerClass
{
  DskOctetListenerClass base_class;
};

static void dsk_ssl_listener_init (DskSslListener *listener);

static void dsk_ssl_listener_finalize (DskSslListener *listener);

static DskIOResult dsk_ssl_listener_accept (DskOctetListener        *listener,
                                            DskOctetStream         **stream_out,
                                            DskOctetSource         **source_out,
                                            DskOctetSink           **sink_out,
                                            DskError               **error);
static void dsk_ssl_listener_shutdown(DskOctetListener       *listener);

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskSslListener);
static DskSslListenerClass dsk_ssl_listener_class =
{
  {
    DSK_OBJECT_CLASS_DEFINE (DskSslListener, &dsk_octet_listener_class,
                             dsk_ssl_listener_init, dsk_ssl_listener_finalize),
    dsk_ssl_listener_accept,
    dsk_ssl_listener_shutdown
  }
};
#define DSK_SSL_LISTENER(lis) DSK_OBJECT_CAST(DskSslListener, lis, &dsk_ssl_listener_class)

static dsk_boolean
handle_underlying_incoming (DskOctetListener *underlying_listener,
                            DskSslListener   *ssl_listener)
{
  DSK_UNUSED (underlying_listener);
  dsk_hook_notify (&ssl_listener->base_instance.incoming);
  return DSK_TRUE;
}
static void
handle_underlying_incoming_destroy (DskSslListener   *ssl_listener)
{
  ssl_listener->underlying_trap = NULL;
  dsk_hook_clear (&ssl_listener->base_instance.incoming);
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
  (DskHookObjectFunc) dsk_object_ref_f,
  (DskHookObjectFunc) dsk_object_unref_f,
  (DskHookSetPoll) dsk_ssl_listener_set_poll
};

static void dsk_ssl_listener_init (DskSslListener *listener)
{
  dsk_hook_set_funcs (&listener->base_instance.incoming,
                      &dsk_ssl_listener_hook_funcs);
}
static void dsk_ssl_listener_finalize (DskSslListener *listener)
{
  if (listener->underlying_trap)
    dsk_hook_trap_free (listener->underlying_trap);
  if (listener->underlying)
    dsk_object_unref (listener->underlying);
}
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
  rv->ssl_context = dsk_object_ref (options->ssl_context);
  return DSK_OCTET_LISTENER (rv);
}

static DskIOResult dsk_ssl_listener_accept (DskOctetListener        *listener,
                                            DskOctetStream         **stream_out,
                                            DskOctetSource         **source_out,
                                            DskOctetSink           **sink_out,
                                            DskError               **error)
{
  DskSslListener *slis = DSK_SSL_LISTENER (listener);
  DskOctetStream *underlying_stream;
  DskOctetSource *underlying_source;
  DskOctetSink *underlying_sink;
  if (slis->underlying == NULL)
    {
      dsk_set_error (error, "SSL-listener: underlying listener does not exist");
      return DSK_IO_RESULT_ERROR;
    }
  switch (dsk_octet_listener_accept (slis->underlying,
                                     &underlying_stream,
                                     &underlying_source,
                                     &underlying_sink,
                                     error))
    {
    case DSK_IO_RESULT_SUCCESS:
      break;
    case DSK_IO_RESULT_AGAIN:
      return DSK_IO_RESULT_AGAIN;
    case DSK_IO_RESULT_EOF:
      return DSK_IO_RESULT_EOF;
    case DSK_IO_RESULT_ERROR:
      return DSK_IO_RESULT_ERROR;
    }

  DskSslStreamOptions options = DSK_SSL_STREAM_OPTIONS_INIT;
  options.is_client = DSK_FALSE;
  options.context = dsk_object_ref (slis->ssl_context);
  options.sink = underlying_sink;
  options.source = underlying_source;
  DskSslStream *ssl_stream;
  if (!dsk_ssl_stream_new (&options,
                           &ssl_stream,
                           source_out, sink_out,
                           error))
    {
      dsk_object_unref (underlying_stream);
      dsk_object_unref (underlying_source);
      dsk_object_unref (underlying_sink);
      return DSK_IO_RESULT_ERROR;
    }
  if (stream_out)
    *stream_out = DSK_OCTET_STREAM (ssl_stream);
  else
    dsk_object_unref (ssl_stream);
  dsk_object_unref (underlying_stream);
  dsk_object_unref (underlying_source);
  dsk_object_unref (underlying_sink);
  return DSK_IO_RESULT_SUCCESS;
}

static void dsk_ssl_listener_shutdown(DskOctetListener       *listener)
{
  DskSslListener *slis = DSK_SSL_LISTENER (listener);
  if (slis->underlying)
    dsk_octet_listener_shutdown (slis->underlying);
}

