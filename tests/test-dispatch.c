#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/stat.h>
#include "../dsk.h"

static dsk_boolean cmdline_verbose = DSK_FALSE;

static void set_boolean_true (void *data) { * (dsk_boolean *) data = DSK_TRUE; }

static void
test_signal_handling (void)
{
  DskDispatchSignal *sig;
  dsk_boolean sig_triggered = DSK_FALSE;
  dsk_boolean timer_triggered = DSK_FALSE;
  sig = dsk_main_add_signal (SIGUSR1, set_boolean_true, &sig_triggered);
  dsk_main_add_timer_millis (50, set_boolean_true, &timer_triggered);
  while (!timer_triggered)
    dsk_main_run_once ();
  dsk_assert (!sig_triggered);
  raise (SIGUSR1);
  while (!sig_triggered)
    dsk_main_run_once ();
  sig_triggered = DSK_FALSE;
  raise (SIGUSR1);
  while (!sig_triggered)
    dsk_main_run_once ();
  dsk_main_remove_signal (sig);
}
static void
handle_child_exit (DskDispatchChildInfo *info, void *func_data)
{
  dsk_assert (!info->killed);
  *(int*)func_data = info->value;
}
static void
test_child_exit (void)
{
  int pid;
  int exit_status = -1;
retry_fork:
  pid = fork ();
  if (pid < 0)
    {
      sleep(1);
      goto retry_fork;
    }
  if (pid == 0)
    _exit(16);
  dsk_main_add_child (pid, handle_child_exit, &exit_status);
  while (exit_status == -1)
    dsk_main_run_once ();
  dsk_assert (exit_status == 16);
}
static void
test_child_exit2 (void)
{
  int pid;
  int exit_status = -1;
retry_fork:
  pid = fork ();
  if (pid < 0)
    {
      sleep(1);
      goto retry_fork;
    }
  if (pid == 0)
    {
      struct timeval tv = { 0, 100*1000 };
      select (0, NULL, NULL, NULL, &tv);
      _exit(16);
    }
  dsk_main_add_child (pid, handle_child_exit, &exit_status);
  while (exit_status == -1)
    dsk_main_run_once ();
  dsk_assert (exit_status == 16);
}
static void
handle_child_killed (DskDispatchChildInfo *info, void *func_data)
{
  dsk_assert (info->killed);
  *(int*)func_data = info->value;
}
static void
test_child_killed (void)
{
  int pid;
  int sig = -1;
retry_fork:
  pid = fork ();
  if (pid < 0)
    {
      sleep(1);
      goto retry_fork;
    }
  if (pid == 0)
    {
      raise (SIGTERM);
    }
  dsk_main_add_child (pid, handle_child_killed, &sig);
  while (sig == -1)
    dsk_main_run_once ();
  dsk_assert (sig == SIGTERM);
}

static void
test_child_killed2 (void)
{
  int pid;
  int sig = -1;
retry_fork:
  pid = fork ();
  if (pid < 0)
    {
      sleep(1);
      goto retry_fork;
    }
  if (pid == 0)
    {
      struct timeval tv = { 0, 100*1000 };
      select (0, NULL, NULL, NULL, &tv);
      raise (SIGTERM);
    }
  dsk_main_add_child (pid, handle_child_killed, &sig);
  while (sig == -1)
    dsk_main_run_once ();
  dsk_assert (sig == SIGTERM);
}

static struct 
{
  const char *name;
  void (*test)(void);
} tests[] =
{
  { "signal handling", test_signal_handling },
  { "test child immediate exit", test_child_exit },
  { "test child exit", test_child_exit2 },
  { "test child immediate kill", test_child_killed },
  { "test child killed", test_child_killed2 },
};
int main(int argc, char **argv)
{
  unsigned i;

  dsk_cmdline_init ("test dispatch code",
                    "Test the dispatch code",
                    NULL, 0);
  dsk_cmdline_add_boolean ("verbose", "extra logging", NULL, 0,
                           &cmdline_verbose);
  dsk_cmdline_process_args (&argc, &argv);

  for (i = 0; i < DSK_N_ELEMENTS (tests); i++)
    {
      fprintf (stderr, "Test: %s... ", tests[i].name);
      tests[i].test ();
      fprintf (stderr, " done.\n");
    }
  dsk_cleanup ();
  return 0;
}
