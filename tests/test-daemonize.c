#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../dsk.h"

/* dummy server configuration */
unsigned cmdline_max = 10;
unsigned cmdline_sleep_ms = 1;
char *cmdline_hello = "hello";


/* dummy server state */
unsigned iter = 0;


/* our dummy server implementation */
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
      dsk_main_add_timer_millis (cmdline_sleep_ms, handle_timeout, NULL);
    }
}


/* --- main --- */
int main(int argc, char **argv)
{
  /* pre-forking setup */
  dsk_cmdline_init ("test daemonize", "test of daemonize command-line args",
                    NULL, 0);
  dsk_cmdline_add_uint ("max", "maximum number of times to print", "MAX",
                        0, &cmdline_max);
  dsk_cmdline_add_uint ("sleep",
                        "milliseconds between each print",
                        "MILLISECONDS",
                        0,
                        &cmdline_sleep_ms);
  dsk_cmdline_add_string ("hello", "what to print", "MESSAGE",
                        0, &cmdline_hello);
  dsk_daemon_add_cmdline_args (DSK_TRUE);
  dsk_cmdline_process_args (&argc, &argv);

  /* this function possibly forks, but then go about
     your serverly business. */
  dsk_daemon_handle ();

  /* dummy server just uses a timer */
  /* XXX: maybe an idle function is better? */
  dsk_main_add_timer_millis (cmdline_sleep_ms, handle_timeout, NULL);

  return dsk_main_run ();
}
