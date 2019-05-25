#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "../dsk.h"

/* dummy server configuration */
unsigned cmdline_max = 10;
unsigned cmdline_sleep_ms = 1;
const char *cmdline_hello = "hello";
unsigned cmdline_terminate_signal = 0;


/* dummy server state */
unsigned iter = 0;


/* our dummy server implementation */
static void
handle_timeout (void *data)
{
  DSK_UNUSED (data);
  if (cmdline_max == iter++)
    {
      if (cmdline_terminate_signal != 0)
        raise (cmdline_terminate_signal);
      dsk_main_quit ();
    }
  else
    {
      printf ("%s\n", cmdline_hello);
      fflush (stdout);
      dsk_main_add_timer_millis (cmdline_sleep_ms, handle_timeout, NULL);
    }
}


static dsk_boolean
handle_arg_kill_signal  (const char *arg_name,
                         const char *arg_value,
                         void       *callback_data,
                         DskError  **error)
{
  DSK_UNUSED(arg_name);

  // is number?
  char *end;
  long v = strtol (arg_value, &end, 0);
  if (arg_value != end)
    {
      if (*end != '\0')
        {
          *error = dsk_error_new ("bad number for signal: '%s'", arg_value);
          return DSK_FALSE;
        }
      * (int *) callback_data = (int) v;
      return DSK_TRUE;
    }

  // match (case-insensitively) against sys_signame[] elements.
  for (size_t i = 0; i < NSIG; i++)
    {
      if (dsk_ascii_strcasecmp (strsignal (i), arg_value) == 0)
        {
          * (int *) callback_data = i;
          return DSK_TRUE;
        }
    }
  *error = dsk_error_new ("unknown signal '%s'", arg_value);
  return DSK_FALSE;
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
  dsk_cmdline_add_func ("kill-signal", "signal to raise prior instead of exiting", "SIGNAL",
                        0, handle_arg_kill_signal, &cmdline_terminate_signal);
                        
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
