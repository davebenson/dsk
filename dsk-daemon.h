
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


void dsk_daemon_handle (void);
