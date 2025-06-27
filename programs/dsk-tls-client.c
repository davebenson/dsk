#include "../dsk.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static bool debug_dump_encrypted_data = false;

static void
print_outgoing_transferred_data (size_t transferred_len,
                                 const uint8_t *transferred_data,
                                 void *transfer_func_data)
{
  (void) transferred_data;
  (void) transfer_func_data;
  dsk_warning ("transferred %u bytes", (unsigned) transferred_len);
}

int main(int argc, char **argv)
{
  dsk_cmdline_init ("connect with TLS",
                    "Make a connection to a remote server with debugging enabled",
                    "HOST PORT",
                    DSK_CMDLINE_PERMIT_ARGUMENTS);
  dsk_cmdline_add_boolean ("dump-encrypted",
                           "Dump hex of the raw encrypted data", NULL, 0,
                           &debug_dump_encrypted_data);

  dsk_cmdline_process_args (&argc, &argv);
  if (argc != 3)
    {
      dsk_cmdline_print_usage();
      return 1;
    }
  const char *host = argv[1];
  unsigned port = strtoul (argv[2], NULL, 10);
  DskError *error = NULL;

  DskTlsClientContextOptions ctx_options = DSK_TLS_CLIENT_CONTEXT_OPTIONS_INIT;
  DskTlsClientContext *ctx = dsk_tls_client_context_new (&ctx_options, &error);
  if (ctx == NULL)
    dsk_die ("error creating TLS client context: %s", error->message);
  
  //
  // Create an underlying raw transport.
  // XXX: should this be possible just naturally from
  // dsk_tls_client_connection_new().
  //
  DskClientStreamOptions client_stream_options = DSK_CLIENT_STREAM_OPTIONS_INIT;
  client_stream_options.hostname = host;
  client_stream_options.port = port;
  DskClientStream *client_stream = dsk_client_stream_new (&client_stream_options, &error);
  if (client_stream == NULL)
    dsk_die ("client stream creation failed: %s", error->message);

  DskTlsClientConnection *tls_client = dsk_tls_client_connection_new (DSK_STREAM (client_stream), ctx, &error);
  if (tls_client == NULL)
    dsk_die ("TLS client stream creation failed: %s", error->message);

  DskStream *conn = DSK_STREAM (tls_client);
  if (debug_dump_encrypted_data)
    {
      DskStreamDebugOptions stream_debug_opts = DSK_STREAM_DEBUG_OPTIONS_INIT;
      conn = dsk_stream_debug_new_take (conn, &stream_debug_opts);
    }

  DskStream *stdio_inout = dsk_stream_new_stdio ();
  
  DskStreamConnectionOptions stream_conn_options = DSK_STREAM_CONNECTION_OPTIONS_INIT;
  stream_conn_options.transfer_func = print_outgoing_transferred_data;
  dsk_stream_connect (stdio_inout, conn, &stream_conn_options);
  dsk_stream_connect (conn, stdio_inout, &stream_conn_options);

  return dsk_main_run();
}
