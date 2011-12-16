#include "dsk.h"

static char       *dsk_daemon_log_template = NULL;
static unsigned    dsk_daemon_log_interval = 3600;
static char       *dsk_daemon_pid_filename = NULL;
static dsk_boolean dsk_daemon_watchdog     = DSK_FALSE;
static dsk_boolean dsk_daemon_do_fork      = DSK_FALSE;

static char       *dsk_daemon_alert_script = 0;
static unsigned    dsk_daemon_alert_interval = 3600;
static unsigned    dsk_daemon_n_alerts_suppressed = 0;

#ifndef WCOREDUMP
#define WCOREDUMP 0
#endif

static DSK_CMDLINE_ARG_CALLBACK(handle_dsk_log_template)
{
  dsk_daemon_log_template = arg_value;

  /* TODO: verify it's a valid format string,
     and verify that log_interval is short enough. */
  return DSK_TRUE;
}

void dsk_daemon_add_cmdline_args (dsk_boolean with_dsk_prefix)
{

  unsigned skip = with_dsk_prefix ? 0 : strlen ("dsk-");

  dsk_cmdline_add_boolean ("dsk-fork" + skip,
                           "fork and run in the background",
			   NULL, 0, &dsk_daemon_do_fork);

  dsk_cmdline_add_boolean ("dsk-watchdog" + skip,
                           "restart if the program dies",
			   NULL, 0, &dsk_daemon_watchdog);

  dsk_cmdline_add_string  ("dsk-pid-filename" + skip,
                           "file to write PID to, lock",
			   "FILENAME", 0, &dsk_daemon_pid_filename);

  dsk_cmdline_add_func    ("dsk-log-template" + skip,
                           "redirect stdout and stderr to the given file",
			   "STRFTIME_FILENAME", 0, handle_dsk_log_template);

  dsk_cmdline_add_string  ("dsk-alert-script" + skip,
                           "for the watchdog to run if the subprocess terminates",
			   "SCRIPT", 0, &dsk_daemon_alert_script);
  dsk_cmdline_add_uint    ("dsk-alert-interval" + skip,
                           "minimum time between invocation of alert script",
			   "SECONDS", 0, &dsk_daemon_alert_interval);
}

void dsk_daemon_handle (void)
{
  int fork_pipe_fds[2] = {-1,-1};
  if (dsk_daemon_do_fork)
    {
    retry_pipe:
      if (pipe (fork_pipe_fds) < 0)
        {
	  if (errno == EINTR)
	    goto retry_pipe;
	  dsk_die ("error creating pipe: %s", strerror (errno));
	}
    retry_daemon_fork:
      pid = fork ();
      if (pid < 0)
        {
	  if (errno == EINTR)
	    goto retry_daemon_fork;
	  dsk_die ("error forking daemon: %s", strerror (errno));
	}
      else if (pid > 0)
        {
	  /* wait for EOF on pipe */
	  close (fork_pipe_fds[1]);
	  char buf[1];
	  for (;;)
	    {
	      int nread = read (fork_pipe_fds[0], buf, 1);
	      if (nread < 0)
	        {
		  if (errno == EINTR)
		    continue;
		  dsk_die ("error reading from semaphore pipe: %s", strerror (errno));
		}
              else if (nread > 0)
		dsk_die ("somehow got data on semaphore pipe: %s:%u", __FILE__, __LINE__);
	      else
	        break;
	    }
	  _exit (0);
        }
      else
        {
	  /* child process: continue as the non-forking case. */
	  close (fork_pipe_fds[0]);
	}
    }
  if (dsk_daemon_pid_filename)
    {
retry_outer_pid_file_open:
      if ((fd=open (dsk_daemon_pid_filename, O_CREAT|O_EXCL|O_WRONLY, 0666)) < 0)
        {
          if (errno == EINTR)
            goto retry_outer_pid_file_open;
          else if (errno == EEXIST)
            {
              /* open / lock-nonblocking / rewrite we get lock */
retry_inner_pid_file_open:
              if ((fd=open (dsk_daemon_pid_filename, O_WRONLY, 0666)) < 0)
                {
                  if (errno == EINTR)
                    goto retry_inner_pid_file_open;
                  dsk_die ("daemonize: error opening lock file %s: %s", dsk_daemon_pid_filename,
                           strerror (errno));
                }
            }
          else if (errno == ENOENT)
            {
              /* make directories, retry */
              ...
            }
          else
            dsk_die ("daemonize: error creating PID file %s: %s",
                     dsk_daemon_pid_filename, strerror (errno));
        }
      if (flock (fd, LOCK_EX|LOCK_NB) < 0)
        {
          if (errno == EINTR)
            goto retry_flock;
          if (errno == EWOULDBLOCK)
            {
              /* TODO: print PID */
              dsk_die ("daemonize: process already running");
            }
          dsk_die ("daemonize: error locking: %s", strerror (errno));
        }
    }
  if (fork_pipe_fds[1] != -1)
    {
      close (fork_pipe_fds[1]);
    }

  if (dsk_daemon_watchdog)
    {
      for (;;)
        {
	  /* NOTE: must never die, i guess */

retry_watchdog_fork:
	  pid = fork ();
	  if (pid < 0)
	    {
	      if (errno == EINTR)
	        goto retry_watchdog_fork;
              dsk_die ("error forking watchdogged process: %s", strerror (errno));
	    }
	  else if (pid == 0)
	    {
	      return;
	    }
	  (void) maybe_redirect_stdouterr (NULL);
	  make_time_str (time_str);
	  fprintf (stderr, "%s: watchdog: forked process %u\n",
	           time_str, (unsigned) pid);
retry_waitpid:
          if (waitpid (pid, &status, 0) < 0)
	    {
	      if (errno == EINTR)
		goto retry_waitpid;
	      dsk_die ("error running waitpid %u: %s", pid, strerror (errno));
	    }
	  (void) maybe_redirect_stdouterr (NULL);
	  make_time_str (time_str);
	  if (WIFEXITED (status))
	    fprintf (stderr, "%s: watchdog: process %u exited with status %u\n",
		     time_str, pid, WEXITSTATUS (status));
	  else if (WIFTERMSIG (status))
	    fprintf (stderr, "%s: watchdog: process %u killed by signal %u%s\n",
		     time_str, pid, WTERMSIG (status),
		     WCOREDUMP (status) ? " [core dumped]" : "");
          else
	    fprintf (stderr, "%s: watchdog: process %u died in some creative way\n",
		     time_str, pid);

          /* configurable? */
          sleep (1);

	  /* send alert */
	  if (dsk_daemon_alert_script)
	    {
	      int time_delta = time (NULL) - last_alert_time;
	      if (time_delta < 0)
	        time_delta = 0;
	      if (time_delta > dsk_daemon_alert_interval)
	        {
                  /// XXX: switch to fork/exec
		  if (system (dsk_daemon_alert_script) != 0)
		    ...
		  dsk_daemon_n_alerts_suppressed = 0;
		}
              else
	        ++dsk_daemon_n_alerts_suppressed;
	    }
	}
    }
}
