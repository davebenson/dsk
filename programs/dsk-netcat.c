#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../dsk.h"

/* configuration */
static unsigned port = 0;
static char *hostname = NULL;
static dsk_boolean do_listen = DSK_FALSE;
static char *local_path = NULL;

int main(int argc, char **argv)
{
  DskClientStreamOptions options = DSK_CLIENT_STREAM_OPTIONS_INIT;
  dsk_cmdline_init ("connect to a server or listen for a connection",
                    "Connect to a server or listen for a client; write data from socket to standard-output; any data collected on standard-input will be sent to the connection.",
                    "[HOST:PORT]",
                    0);
  dsk_cmdline_permit_extra_arguments (DSK_TRUE);
  dsk_cmdline_add_string ("host", "Hostname to connect to", "HOSTNAME", 0, &hostname);
  dsk_cmdline_add_uint ("port", "Port to connect to or to listen on", "PORT", 0, &port);
  dsk_cmdline_add_string ("local", "Use local (aka unix-domain) socket", "PATH", 0, &local_path);
  dsk_cmdline_add_boolean ("listen", "Listen for a connection instead of connecting", NULL, 0, &do_listen);
  dsk_cmdline_process_args (&argc, &argv);

  if (hostname != NULL && local_path != NULL)
    dsk_error ("--host and --local are mutually exclusive");
  if (hostname != NULL && do_listen)
    dsk_error ("--host and --listen are mutually exclusive");
  if (argc == 1)
    {
      if ((hostname == NULL && local_path == NULL) || do_listen)
        dsk_error ("no hostname or path given and not listening");
    }
  else if (argc > 2)
    dsk_error ("too many arguments (expected at most HOST:PORT, besides options)");
  else
    {
      if (hostname || local_path)
        dsk_error ("hostname or local-path given as option and there's another arg (%s)", argv[1]);
      if (argv[1][0] == '/')
        {
          local_path = argv[1];
        }
      else
        {
          const char *colon = strchr (argv[1], ':');
          if (colon == NULL)
            dsk_error ("argument to dsk-netcat must be /PATH or HOST:PORT");
          port = atoi (colon + 1);
          if (port == 0)
            dsk_error ("error parsing port (from '%s')", colon+1);
          hostname = dsk_strdup_slice (argv[1], colon);
        }
    }

  if (do_listen)
    {
      dsk_error ("sorry: --listen is not supported yet");         /* XXX */
    }
  else
    {
      DskClientStream *client_stream;
      DskOctetSource *std_input;
      DskOctetSink *std_output;
      DskOctetSource *csource;
      DskOctetSink *csink;
      DskError *error = NULL;
      options.hostname = hostname;
      options.port = port;
      options.path = local_path;
      if (!dsk_client_stream_new (&options,
                                  &client_stream,
                                  &csink,
                                  &csource,
                                  &error))
        dsk_die ("error creating client-stream: %s", error->message);

      std_input = dsk_octet_source_new_stdin ();
      std_output = dsk_octet_sink_new_stdout ();
      dsk_octet_connect (std_input, csink, NULL);
      dsk_octet_connect (csource, std_output, NULL);

      /* keep running until i/o is done ?!? */
      dsk_main_add_object (csink);
      dsk_main_add_object (csource);
      dsk_object_unref (csource);
      dsk_object_unref (csink);
      return dsk_main_run ();
    }
  return 1;             /* should not reach here */
}
