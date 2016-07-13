#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "dsk.h"

static const char *dsk_daemon_log_template = NULL;
static unsigned    dsk_daemon_log_interval = 3600;
static char       *dsk_daemon_pid_filename = NULL;
static dsk_boolean dsk_daemon_watchdog     = DSK_FALSE;
static dsk_boolean dsk_daemon_do_fork      = DSK_FALSE;
static int         dsk_daemon_tzoffset     = 0;

static char       *dsk_daemon_alert_script = 0;
static unsigned    dsk_daemon_alert_interval = 3600;
static unsigned    dsk_daemon_n_alerts_suppressed = 0;

#ifndef WCOREDUMP
#define WCOREDUMP 0
#endif

static DSK_CMDLINE_CALLBACK_DECLARE(handle_dsk_log_template)
{
  DSK_UNUSED (arg_name);
  DSK_UNUSED (callback_data);
  DSK_UNUSED (error);
  dsk_daemon_log_template = arg_value;

  /* TODO: verify it's a valid format string,
     and verify that log_interval is short enough. */
  return DSK_TRUE;
}

static DSK_CMDLINE_CALLBACK_DECLARE(handle_dsk_log_timezone)
{
  char *end;
  DSK_UNUSED (callback_data);
  if (!dsk_date_parse_timezone (arg_value, &end, &dsk_daemon_tzoffset))
    {
      dsk_set_error (error, "bad timezone argument '%s' to --%s",
                     arg_value, arg_name);
      return DSK_FALSE;
    }
  dsk_daemon_tzoffset *= 60;
  return DSK_TRUE;
}

static void
maybe_redirect_stdouterr (void)
{
  if (dsk_daemon_log_template == NULL)
    return;

  time_t t = time (NULL);
  char buf[2048];
  struct tm tm;
  t += dsk_daemon_tzoffset;
  gmtime_r (&t, &tm);
  strftime (buf, sizeof (buf), dsk_daemon_log_template, &tm);

  int fd;
  dsk_boolean made_dir = DSK_FALSE;
retry_open:
  fd = open (buf, O_WRONLY|O_APPEND|O_CREAT, 0666);
  if (fd < 0)
    {
      if (errno == EINTR)
        goto retry_open;
      dsk_fd_creation_failed (errno);
      if (errno == ENOENT && !made_dir)
        {
          char *slash = strrchr (buf, '/');
          if (slash != NULL)
            {
              char *dir = dsk_strdup_slice (buf, slash);
              DskError *error = NULL;
              if (!dsk_mkdir_recursive (dir, 0777, &error))
                dsk_die ("error making directory %s: %s", dir, error->message);
              dsk_free (dir);
            }
          made_dir = DSK_TRUE;
          goto retry_open;
        }
      dsk_die ("error creating %s: %s", buf, strerror (errno));
    }
  fflush (stdout);
  fflush (stderr);
  dup2 (fd, STDOUT_FILENO);
  dup2 (fd, STDERR_FILENO);
  close (fd);
}
static void
add_maybe_redirect_timer (void)
{
  if (dsk_daemon_log_template == NULL)
    return;

  maybe_redirect_stdouterr ();

  unsigned time = dsk_dispatch_default ()->last_dispatch_secs;
  time -= time % dsk_daemon_log_interval;
  time += dsk_daemon_log_interval;

  dsk_main_add_timer (time, 0, (DskTimerFunc) add_maybe_redirect_timer, NULL);
}

#define TIME_STR_LENGTH    64

static void
make_time_str (char *out)
{
  time_t t = time (NULL);
  struct tm tm;
  t += dsk_daemon_tzoffset;
  gmtime_r (&t, &tm);
  strftime (out, TIME_STR_LENGTH, "%Y-%m-%d %H:%M:%S", &tm);
}

void dsk_daemon_add_cmdline_args (dsk_boolean with_dsk_prefix)
{
  unsigned skip = with_dsk_prefix ? 0 : strlen ("dsk-");

#define MAYBE_SKIP_DSK_PREFIX(static_string, skip) \
  &(static_string[skip])

  dsk_cmdline_add_boolean (MAYBE_SKIP_DSK_PREFIX("dsk-fork", skip),
                           "fork and run in the background",
			   NULL, 0, &dsk_daemon_do_fork);

  dsk_cmdline_add_boolean (MAYBE_SKIP_DSK_PREFIX("dsk-watchdog", skip),
                           "restart if the program dies",
			   NULL, 0, &dsk_daemon_watchdog);

  dsk_cmdline_add_string  (MAYBE_SKIP_DSK_PREFIX("dsk-pid-filename", skip),
                           "file to write PID to, lock",
			   "FILENAME", 0, &dsk_daemon_pid_filename);

  dsk_cmdline_add_func    (MAYBE_SKIP_DSK_PREFIX("dsk-log-template", skip),
                           "redirect stdout and stderr to the given file",
			   "STRFTIME_FILENAME", 0, handle_dsk_log_template, NULL);
  dsk_cmdline_add_func    (MAYBE_SKIP_DSK_PREFIX("dsk-log-timezone", skip),
                           "use timezone for filenames and logs",
			   "TZSPEC", 0, handle_dsk_log_timezone, NULL);

  dsk_cmdline_add_string  (MAYBE_SKIP_DSK_PREFIX("dsk-alert-script", skip),
                           "for the watchdog to run if the subprocess terminates",
			   "SCRIPT", 0, &dsk_daemon_alert_script);
  dsk_cmdline_add_uint    (MAYBE_SKIP_DSK_PREFIX("dsk-alert-interval", skip),
                           "minimum time between invocation of alert script",
			   "SECONDS", 0, &dsk_daemon_alert_interval);

#undef MAYBE_SKIP_DSK_PREFIX
}

void dsk_daemon_maybe_fork (void)
{
  int fork_pipe_fds[2] = {-1,-1};
  if (dsk_daemon_do_fork)
    {
      int pid;
    retry_pipe:
      if (pipe (fork_pipe_fds) < 0)
        {
	  if (errno == EINTR)
	    goto retry_pipe;
          dsk_fd_creation_failed (errno);
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
          setsid ();
	}
    }
  int pid_file_fd = -1;
  if (dsk_daemon_pid_filename)
    {
      dsk_boolean must_truncate = DSK_FALSE;
      dsk_boolean made_dir = DSK_FALSE;

retry_outer_pid_file_open:
      if ((pid_file_fd=open (dsk_daemon_pid_filename, O_CREAT|O_EXCL|O_WRONLY, 0666)) < 0)
        {
          if (errno == EINTR)
            goto retry_outer_pid_file_open;
          else if (errno == EEXIST)
            {
              /* open / lock-nonblocking / rewrite we get lock */
retry_inner_pid_file_open:
              if ((pid_file_fd=open (dsk_daemon_pid_filename, O_WRONLY, 0666)) < 0)
                {
                  if (errno == EINTR)
                    goto retry_inner_pid_file_open;
                  dsk_die ("daemonize: error opening lock file %s: %s", dsk_daemon_pid_filename,
                           strerror (errno));
                }
              must_truncate = DSK_TRUE;
            }
          else if (errno == ENOENT && !made_dir)
            {
              /* make directories, retry */
              char *slash = strrchr (dsk_daemon_pid_filename, '/');
              if (slash == NULL)
                dsk_die ("daemonize: error creating %s: no such file or dir (cwd does not exist?)", dsk_daemon_pid_filename);
              char *dir = dsk_strdup_slice (dsk_daemon_pid_filename, slash);
              DskError *error = NULL;
              if (!dsk_mkdir_recursive (dir, 0777, &error))
                dsk_die ("error making directory %s: %s", dir, error->message);
              dsk_free (dir);
              made_dir = DSK_TRUE;
              goto retry_outer_pid_file_open;
            }
          else
            {
              dsk_fd_creation_failed (errno);
              dsk_die ("daemonize: error creating PID file %s: %s",
                       dsk_daemon_pid_filename, strerror (errno));
            }
        }
retry_flock:
      if (flock (pid_file_fd, LOCK_EX|LOCK_NB) < 0)
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
      if (must_truncate)
        {
          ftruncate (pid_file_fd, 0);
        }
      char buf[32];
      snprintf (buf, sizeof (buf), "%u\n", (unsigned)getpid ());
      unsigned len = strlen (buf);
      unsigned written = 0;
      while (written < len)
        {
          int write_rv = write (pid_file_fd, buf + written, len - written);
          if (write_rv < 0)
            {
              if (errno == EINTR)
                continue;
              dsk_die ("error writing pid file %s", dsk_daemon_pid_filename);
            }
          written += write_rv;
        }
    }
  if (fork_pipe_fds[1] != -1)
    {
      close (fork_pipe_fds[1]);
    }

  if (dsk_daemon_watchdog)
    {
      int alert_pid = 0;
      unsigned last_alert_time = 0;
      for (;;)
        {
	  /* NOTE: must never die, i guess */
          int pid;
          int status;

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
              if (pid_file_fd >= 0)
                close (pid_file_fd);
              maybe_redirect_stdouterr ();
              add_maybe_redirect_timer ();
	      return;
	    }
	  maybe_redirect_stdouterr ();
          char time_str[TIME_STR_LENGTH];
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
	  maybe_redirect_stdouterr ();
	  make_time_str (time_str);
	  if (WIFEXITED (status))
	    fprintf (stderr, "%s: watchdog: process %u exited with status %u\n",
		     time_str, pid, WEXITSTATUS (status));
	  else if (WIFSIGNALED (status))
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
              unsigned clamped_delta = time_delta < 0 ? 0 : time_delta;
              if (alert_pid > 0)
                {
                  int rv = waitpid (alert_pid, &status, WNOHANG);
                  if (rv < 0)
                    {
                      if (errno == EINTR)
                        goto retry_waitpid;
                      else
                        dsk_die ("error waiting for alert process");
                    }
                  else if (rv == 0)
                    {
                      /* process has not terminated */
                    }
                  else
                    {
                      /* process terminated (ignore status?) */
                      alert_pid = 0;
                    }
                }
	      if (alert_pid == 0 && clamped_delta > dsk_daemon_alert_interval)
	        {
                  retry_alert_fork:
                  alert_pid = fork ();
                  if (alert_pid < 0)
                    {
                      if (errno == EINTR)
                        goto retry_alert_fork;
                      dsk_warning ("error forking alert process: %s", strerror (errno));
                      alert_pid = 0;
                    }
                  else if (alert_pid == 0)
                    {
                      execl ("/bin/sh", "/bin/sh", "-c", dsk_daemon_alert_script, NULL);
                      _exit (127);
                    }
                  else
                    dsk_daemon_n_alerts_suppressed = 0;
		}
              else
	        ++dsk_daemon_n_alerts_suppressed;
	    }
	}
    }
}
