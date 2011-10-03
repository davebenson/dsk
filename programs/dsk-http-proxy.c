#include "../dsk.h"

static dsk_boolean
handle_http_stream_request_available (DskHttpServerStream *stream)
{
  /* receive a new request */
  DskHttpServerStreamTransfer *xfer;
  xfer = dsk_http_server_stream_get_request (stream);
  if (xfer == NULL)
    return DSK_TRUE;

  /* make remote request */
  DskHttpServerStreamResponseOptions resp_options
    = DSK_HTTP_SERVER_STREAM_RESPONSE_OPTIONS_DEFAULT;
  DskError *error = NULL;
  ...
  if (!dsk_http_server_stream_respond (xfer, &resp_options, &error))
    {
      ...
    }

  return DSK_TRUE;
}

static dsk_boolean
handle_incoming_connection (DskOctetListener *listener)
{
  DskHttpServerStream *stream;

  /* accept connection */
  switch (dsk_octet_listener_accept (listener, NULL,
                                     &source, &sink, &error))
    {
    case DSK_IO_RESULT_SUCCESS:
      /* create http-server stream */
      stream = dsk_http_server_stream_new (sink, source, &server_stream_options);
      dsk_assert (stream != NULL);
      dsk_hook_trap (&stream->request_available,
                     (DskHookFunc) handle_http_stream_request_available,
                     stream,
                     dsk_object_unref);
      break;
    case DSK_IO_RESULT_AGAIN:
      break;
    case DSK_IO_RESULT_ERROR:
      dsk_main_exit (1);
      dsk_warning ("cannot accept a new socket: %s",
                   error->message);
      dsk_error_unref (error);
      error = NULL;
      return DSK_FALSE;
    case DSK_IO_RESULT_EOF:
      dsk_assert_not_reached ();
    }

  /* invoke this handler when the next incoming connection is available. */
  return DSK_TRUE;
}

static dsk_boolean
handle_port (const char *arg_name,
             const char *arg_value,
             void       *callback_data,
             DskError  **error)
{
  DskOctetListenerSocketOptions options
    = DSK_OCTET_LISTENER_SOCKET_OPTIONS_DEFAULT;
  DskOctetListener *listener;
  options.bind_port = strtoul (arg_value, &end, 10);
  if (arg_value == end || *end != 0)
    {
      dsk_set_error (error, "error parsing integer from '%s'", arg_value);
      return DSK_FALSE;
    }

  listener = dsk_octet_listener_socket_new (&options, error);
  if (listener == NULL)
    return DSK_FALSE;
  dsk_hook_trap (&listener->incoming,
                 (DskHookFunc) handle_incoming_connection,
                 NULL,
                 NULL);
  dsk_main_add_object (listener);
  return DSK_TRUE;
}

int main(int argc, char **argv)
{
  /* Process command-line arguments. */
  dsk_cmdline_init ("a ??? HTTP-Proxy Server",
                    "Run a simple HTTP Proxy.",
                    NULL, 0);
  dsk_cmdline_add_func ("port", "port to listen on", "PORT",
                        DSK_CMDLINE_MANDATORY,
                        handle_port, NULL);
  dsk_cmdline_process_args (&argc, &argv);

  /* Run forever. */
  return dsk_main_run ();
}
