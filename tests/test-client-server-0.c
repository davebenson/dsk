#include <string.h>
#include <stdio.h>
#include "../dsk.h"

/* --- implement a simple echo server --- */
static dsk_boolean cmdline_is_tcp = DSK_FALSE;
static char *cmdline_local_path = (char *) "./test-client-server-0.socket";
static unsigned cmdline_tcp_port = 10101;
static dsk_boolean cmdline_debug_server = DSK_FALSE;

typedef struct _EchoConnection EchoConnection;
struct _EchoConnection
{
  DskBuffer pending;

  DskHookTrap *read_trap;
  DskOctetSource *source;

  DskHookTrap *write_trap;
  DskOctetSink *sink;
};

static dsk_boolean
handle_sink_writable (DskOctetSink *sink,
                      EchoConnection *ec)
{
  DskError *error = NULL;
  if (cmdline_debug_server)
    dsk_warning ("server: handle_sink_writable: pending size = %u", (unsigned)(ec->pending.size));
  switch (dsk_octet_sink_write_buffer (sink, &ec->pending, &error))
    {
    case DSK_IO_RESULT_SUCCESS:
      if (ec->pending.size == 0)
        {
          ec->write_trap = NULL;
          if (ec->source == NULL)
            {
              /* the end */
              dsk_octet_sink_shutdown (sink);
              dsk_object_unref (sink);
              ec->sink = NULL;
            }
          return DSK_FALSE;
        }
      else
        return DSK_TRUE;
    case DSK_IO_RESULT_AGAIN:
      return DSK_TRUE;
    case DSK_IO_RESULT_EOF:
      dsk_assert_not_reached ();
    case DSK_IO_RESULT_ERROR:
      dsk_die ("error writing: %s", error->message);
    }
  return DSK_TRUE;
}

static dsk_boolean
handle_source_readable (DskOctetSource *source,
                        EchoConnection *ec)
{
  DskError *error = NULL;
  if (cmdline_debug_server)
    dsk_warning ("handle_source_readable (initial pending=%u)", ec->pending.size);
  switch (dsk_octet_source_read_buffer (source, &ec->pending, &error))
    {
    case DSK_IO_RESULT_SUCCESS:
      if (ec->write_trap == NULL && ec->pending.size > 0)
        {
          if (cmdline_debug_server)
            dsk_warning ("server: creating write-trap (%u bytes received)",
                         (unsigned)ec->pending.size);
          ec->write_trap = dsk_hook_trap (&ec->sink->writable_hook,
                                          (DskHookFunc) handle_sink_writable,
                                          ec,
                                          NULL);
        }
      return DSK_TRUE;
    case DSK_IO_RESULT_AGAIN:
      return DSK_TRUE;
    case DSK_IO_RESULT_EOF:
      {
        ec->source = NULL;
        ec->read_trap = NULL;
        dsk_object_unref (source);
        if (ec->pending.size == 0)
          {
            dsk_assert (ec->write_trap == NULL);
            dsk_octet_sink_shutdown (ec->sink);
            dsk_object_unref (ec->sink);
            dsk_free (ec);
          }
        return DSK_FALSE;
      }
    case DSK_IO_RESULT_ERROR:
      dsk_die ("read failed: %s", error->message);              /* eep! */
    }
  return DSK_FALSE;
}

static dsk_boolean
handle_incoming_connection (DskOctetListener *listener)
{
  DskError *error = NULL;
  DskOctetSource *source;
  DskOctetSink *sink;
  switch (dsk_octet_listener_accept (listener, NULL, &source, &sink, &error))
    {
    case DSK_IO_RESULT_SUCCESS:
      {
        EchoConnection *ec = dsk_malloc (sizeof (EchoConnection));
        dsk_buffer_init (&ec->pending);
        ec->source = source;
        ec->read_trap = dsk_hook_trap (&source->readable_hook,
                                       (DskHookFunc) handle_source_readable,
                                       ec,
                                       NULL);
        ec->sink = sink;
        ec->write_trap = NULL;
        break;
      }
    case DSK_IO_RESULT_AGAIN:
      return DSK_TRUE;
    case DSK_IO_RESULT_EOF:
      dsk_warning ("EOF from listening socket?");
      return DSK_TRUE;
    case DSK_IO_RESULT_ERROR:
      dsk_die ("error accepting incoming connection: %s", error->message);
    }
  return DSK_TRUE;
}

static DskOctetListener *global_listener = NULL;


static void
create_server (void)
{
  DskOctetListenerSocketOptions opts = DSK_OCTET_LISTENER_SOCKET_OPTIONS_INIT;
  DskOctetListener *listener;
  DskError *error = NULL;
  opts.is_local = !cmdline_is_tcp;
  opts.local_path = cmdline_local_path; 
  opts.bind_port = cmdline_tcp_port;

  dsk_assert (global_listener == NULL);

  listener = dsk_octet_listener_socket_new (&opts, &error);
  if (listener == NULL)
    dsk_die ("error creating listener: %s", error->message);
  dsk_hook_trap (&listener->incoming, (DskHookFunc) handle_incoming_connection, NULL, NULL);
  global_listener = listener;
}
static void
kill_server (void)
{
  dsk_assert (global_listener != NULL);
  dsk_object_unref (global_listener);
  global_listener = NULL;
}
/* --- client implementation --- */
static DskClientStreamSink *client_sink;
static DskClientStreamSource *client_source;
static DskClientStream *client_stream;
static unsigned cmdline_wait_time = 250;
static dsk_boolean cmdline_debug_client = DSK_FALSE;

static void
create_client (void)
{
  DskClientStreamOptions options = DSK_CLIENT_STREAM_OPTIONS_INIT;
  DskError *error = NULL;
  if (cmdline_is_tcp)
    {
      dsk_ip_address_localhost (&options.address);
      options.port = cmdline_tcp_port;
    }
  else
    options.path = cmdline_local_path;
  options.reconnect_time = 10;
  if (!dsk_client_stream_new (&options,
                              &client_stream,
                              (DskOctetSink **) &client_sink,
                              (DskOctetSource **) &client_source,
                              &error))
    dsk_die ("failed creating client-stream: %s", error->message);
}
static void
kill_client (void)
{
  dsk_object_unref (client_sink);
  dsk_object_unref (client_source);
  dsk_object_unref (client_stream);
  client_sink = NULL;
  client_source = NULL;
  client_stream = NULL;
}
static dsk_boolean set_boolean_true (void *object, void *data)
{
  DSK_UNUSED (object);
  * (dsk_boolean *) data = DSK_TRUE;
  return DSK_FALSE;
}

static void handle_timeout (void *data)
{
  * (dsk_boolean *) data = DSK_TRUE;
}

static dsk_boolean
write_then_read (unsigned write_len,
                 const uint8_t *write_data,
                 unsigned *read_len_out,
                 uint8_t **read_data_out,
                 DskError **error_out,
                 unsigned time_millis)
{
  unsigned n_written = 0;
  dsk_boolean timer_expired = DSK_FALSE;
  DskDispatchTimer *timer;
  DskBuffer readbuf = DSK_BUFFER_INIT;
  if (cmdline_debug_client)
    dsk_warning ("write_then_read: trying to write %u bytes; timeout=%u millis", write_len, time_millis);
  timer = dsk_main_add_timer_millis (time_millis, handle_timeout, &timer_expired);

  /* write data */
  while (!timer_expired && n_written < write_len)
    {
      dsk_boolean is_writable = DSK_FALSE;
      DskHookTrap *trap = dsk_hook_trap (&client_sink->base_instance.writable_hook, set_boolean_true, &is_writable, NULL);
      unsigned nw;
      while (!is_writable && !timer_expired)
        dsk_main_run_once ();
      if (timer_expired && !is_writable)
        dsk_hook_trap_destroy (trap);
      switch (dsk_octet_sink_write (DSK_OCTET_SINK (client_sink), write_len - n_written, write_data + n_written, &nw, error_out))
        {
        case DSK_IO_RESULT_SUCCESS:
          n_written += nw;
          break;
        case DSK_IO_RESULT_AGAIN:
          break;
        case DSK_IO_RESULT_EOF:
          dsk_assert_not_reached ();
        case DSK_IO_RESULT_ERROR:
          return DSK_FALSE;
        }
    }
  if (n_written < write_len)
    {
      dsk_set_error (error_out, "timer expired before data written");
      return DSK_FALSE;
    }

  /* read til EOF */
  while (!timer_expired)
    {
      DskHookTrap *trap;
      dsk_boolean is_readable = DSK_FALSE;
      if (cmdline_debug_client)
        dsk_warning ("client: trapping readability");
      trap = dsk_hook_trap (&client_source->base_instance.readable_hook, set_boolean_true, &is_readable, NULL);
      while (!is_readable && !timer_expired)
        dsk_main_run_once ();
      if (timer_expired && !is_readable)
        dsk_hook_trap_destroy (trap);
      switch (dsk_octet_source_read_buffer (DSK_OCTET_SOURCE (client_source), &readbuf, error_out))
        {
        case DSK_IO_RESULT_SUCCESS:
          if (cmdline_debug_client)
            dsk_warning ("client: got success reading (pending-size=%u)",
                         (unsigned)readbuf.size);
          break;
        case DSK_IO_RESULT_AGAIN:
          break;
        case DSK_IO_RESULT_EOF:
          dsk_warning ("unexpected eof");
          dsk_main_remove_timer (timer);
          timer = NULL;
          goto done_reading;
        case DSK_IO_RESULT_ERROR:
          dsk_buffer_reset (&readbuf);
          dsk_main_remove_timer (timer);
          return DSK_FALSE;
        }
    }

  timer = NULL;

  /* return result buffer */
done_reading:
  *read_len_out = readbuf.size;
  *read_data_out = dsk_malloc (readbuf.size);
  dsk_buffer_read (&readbuf, *read_len_out, *read_data_out);

  return DSK_TRUE;
}

static void
test_echo_server_up (void)
{
  unsigned len;
  uint8_t *data;
  DskError *error = NULL;
  if (!write_then_read (6, (uint8_t *) "hello\n", &len, &data, &error, cmdline_wait_time))
    dsk_die ("write_then_read failed: %s", error->message);
  dsk_assert (len == 6);
  dsk_assert (memcmp (data, "hello\n", 6) == 0);
  dsk_free (data);
}

static void
pause_a_moment (void)
{
  dsk_boolean timer_expired = DSK_FALSE;
  dsk_main_add_timer_millis (cmdline_wait_time, handle_timeout, &timer_expired);
  while (!timer_expired)
    dsk_main_run_once ();
}

static void
test_client_defunct (void)
{
  unsigned len;
  uint8_t *data;
  DskError *error = NULL;
  if (!write_then_read (6, (uint8_t*) "hello\n", &len, &data, &error, cmdline_wait_time))
    {
      dsk_error_unref (error);
      return;
    }
  dsk_assert (len == 0);
  dsk_free (data);
}

int main(int argc, char **argv)
{
  /* command-line options */
  dsk_cmdline_add_boolean ("tcp", "use TCP instead of unix-domain sockets",
                           NULL, 0, &cmdline_is_tcp);
  dsk_cmdline_add_uint ("port", "TCP port to use for serving", "PORT",
                        0, &cmdline_tcp_port);
  dsk_cmdline_add_string ("local-path", "Unix-domain path to use for serving", "PATH",
                          0, &cmdline_local_path);
  dsk_cmdline_add_boolean ("debug-server", "enabler debugging prints in the server implementation",
                           NULL, 0, &cmdline_debug_server);
  dsk_cmdline_add_boolean ("debug-client", "enabler debugging prints in the client implementation",
                           NULL, 0, &cmdline_debug_client);
  dsk_cmdline_process_args (&argc, &argv);

  /* create client */
  printf ("creating client\n");
  create_client ();
  printf ("testing defunct client\n");
  test_client_defunct ();

  /* create server */
  printf ("creating server\n");
  create_server ();
  printf ("testing echo server (through client)\n");
  test_echo_server_up ();
  printf ("testing echo server again\n");
  test_echo_server_up ();
  printf ("and again\n");
  test_echo_server_up ();

  /* destroy server */
  printf ("killing server\n");
  kill_server ();
  dsk_client_stream_disconnect (client_stream);
  printf ("testing defunct client again\n");
  test_client_defunct ();

  /* re-create server */
  printf ("creating server again\n");
  create_server ();
  printf ("dramatic pause\n");
  pause_a_moment ();
  printf ("testing echo server up\n");
  test_echo_server_up ();
  printf ("killing server again\n");
  kill_server ();
  dsk_client_stream_disconnect (client_stream);
  printf ("testing defunct client for the third time\n");
  test_client_defunct ();

  printf ("killing client\n");
  kill_client ();

  dsk_cleanup ();


  return 0;
}
