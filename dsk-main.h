
/* dispatch wrappers */
void              dsk_main_watch_fd        (DskFileDescriptor   fd,
                                            unsigned            events,
                                            DskFDFunc           callback,
                                            void               *callback_data);
void              dsk_main_close_fd        (DskFileDescriptor   fd);
void              dsk_main_fd_closed       (DskFileDescriptor   fd);

DskDispatchTimer *dsk_main_add_timer       (unsigned            timeout_secs,
                                            unsigned            timeout_usecs,
                                            DskTimerFunc        func,
                                            void               *func_data);
DskDispatchTimer *dsk_main_add_timer_millis(unsigned            milliseconds,
                                            DskTimerFunc        func,
                                            void               *func_data);
void              dsk_main_adjust_timer    (DskDispatchTimer   *timer,
                                            unsigned            timeout_secs,
                                            unsigned            timeout_usecs);
void              dsk_main_adjust_timer_millis (DskDispatchTimer *timer,
                                            unsigned            milliseconds);
void              dsk_main_remove_timer    (DskDispatchTimer   *timer);
DskDispatchIdle  *dsk_main_add_idle        (DskIdleFunc         func,
                                            void               *func_data);
void              dsk_main_remove_idle     (DskDispatchIdle    *idle);
DskDispatchSignal*dsk_main_add_signal      (int                 signal_number,
                                            DskSignalHandler    func,
                                            void               *func_data);
void              dsk_main_remove_signal   (DskDispatchSignal  *signal);
DskDispatchChild *dsk_main_add_child       (int                process_id,
                                            DskChildHandler    func,
                                            void              *func_data);
void              dsk_main_remove_child    (DskDispatchChild  *handler);

#define dsk_main_run_once()  dsk_dispatch_run(dsk_dispatch_default())


/* program termination (terminate when ref-count gets to 0);
 * many programs leave 0 refs the whole time.
 */
void              dsk_main_add_object      (void               *object);
void              dsk_main_add_ref         (void);
void              dsk_main_remove_ref      (void);

/* running until termination (ie until we get to 0 refs) */
int               dsk_main_run             (void);

/* terminate dsk_main_run() */
void              dsk_main_exit            (int                exit_status);

/* same as dsk_main_exit(0) -- useful as a callback */
void              dsk_main_quit            (void);
