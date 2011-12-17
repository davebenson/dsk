#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../dsk.h"
unsigned cmdline_max = 10;
unsigned cmdline_sleep = 1;
char *cmdline_hello = "hello";

unsigned iter = 0;

static void
handle_timeout (void *data)
{
  DSK_UNUSED (data);
  if (cmdline_max == iter++)
    dsk_main_quit ();
  else
    {
      printf ("%s\n", cmdline_hello);
      fflush (stdout);
      dsk_main_add_timer_millis (cmdline_sleep, handle_timeout, NULL);
    }
}

int main(int argc, char **argv)
{
  dsk_cmdline_init ("test daemonize", "test of daemonize command-line args",
                    NULL, 0);
  dsk_cmdline_add_uint ("max", "maximum number of times to print", "MAX",
                        0, &cmdline_max);
  dsk_cmdline_add_uint ("sleep", "seconds between each print", "SECONDS",
                        0, &cmdline_sleep);
  dsk_cmdline_add_string ("hello", "what to print", "MESSAGE",
                        0, &cmdline_hello);
  dsk_daemon_add_cmdline_args (DSK_TRUE);
  dsk_cmdline_process_args (&argc, &argv);

  dsk_daemon_handle ();

  dsk_main_add_timer_millis (cmdline_sleep, handle_timeout, NULL);

  return dsk_main_run ();
}
