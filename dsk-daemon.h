
/* --dsk-fork: fork and run in the background
   --dsk-watchdog: restart if the program dies
   --dsk-pid-filename FILENAME: file to write PID to, lock
   --dsk-log-template STRFTIME_FILENAME: rotating file for output
   --dsk-log-timezone TZ: use timezone when computing logfile names
   --dsk-alert-script SCRIPT: run sh script if subprocess dies
   --dsk-alert-interval SECONDS: do not run alert-script more than once 
                                 in this number of seconds.
 */


void dsk_daemon_add_cmdline_args (dsk_boolean with_dsk_prefix);

/* After you run this function, you should go about the non-terminating
   stuff your process presumably does.
   
   dsk_daemon_maybe_fork() will call fork as needed,
   and this call may be returned from multiple
   times in different processes.  We do guarantee that we won't return
   until the previous process (that is, that last process we returned in),
   if any, has terminated. */
void dsk_daemon_maybe_fork (void);
#define dsk_daemon_handle dsk_daemon_maybe_fork
