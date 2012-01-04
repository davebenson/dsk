#ifndef __DSK_DISPATCH_H_
#define __DSK_DISPATCH_H_

typedef struct _DskDispatch DskDispatch;
typedef struct _DskDispatchTimer DskDispatchTimer;
typedef struct _DskDispatchIdle DskDispatchIdle;
typedef struct _DskDispatchSignal DskDispatchSignal;
typedef struct _DskDispatchChild DskDispatchChild;

typedef enum
{
  DSK_EVENT_READABLE = (1<<0),
  DSK_EVENT_WRITABLE = (1<<1)
} Dsk_Events;

/* Create or destroy a Dispatch */
DskDispatch        *dsk_dispatch_new (void);
void                dsk_dispatch_free(DskDispatch *dispatch);

DskDispatch        *dsk_dispatch_default (void);


/* Registering file-descriptors to watch. */
typedef void (*DskFDFunc)  (DskFileDescriptor fd,
                            unsigned          events,
                            void             *callback_data);
void  dsk_dispatch_watch_fd (DskDispatch        *dispatch,
                             DskFileDescriptor   fd,
                             unsigned            events,
                             DskFDFunc           callback,
                             void               *callback_data);
void  dsk_dispatch_close_fd (DskDispatch        *dispatch,
                             DskFileDescriptor   fd);
void  dsk_dispatch_fd_closed(DskDispatch        *dispatch,
                             DskFileDescriptor   fd);

/* Timers */
typedef void (*DskTimerFunc) (void              *func_data);
DskDispatchTimer *
      dsk_dispatch_add_timer       (DskDispatch       *dispatch,
                                    unsigned           timeout_secs,
                                    unsigned           timeout_usecs,
                                    DskTimerFunc       func,
                                    void              *func_data);
void  dsk_dispatch_adjust_timer    (DskDispatchTimer *timer,
                                    unsigned           timeout_secs,
                                    unsigned           timeout_usecs);

/* if possible, these are keyed off a clock that ignores clock-resets */
DskDispatchTimer *
      dsk_dispatch_add_timer_millis(DskDispatch       *dispatch,
                                    uint64_t           milliseconds,
                                    DskTimerFunc       func,
                                    void              *func_data);
void  dsk_dispatch_adjust_timer_millis (DskDispatchTimer *timer,
                                        uint64_t          milliseconds);

void  dsk_dispatch_remove_timer (DskDispatchTimer *);


/* querying the timer (UNIMPLEMENTED) */

dsk_boolean  dsk_dispatch_timer_is_wallclock (DskDispatchTimer *);
uint64_t     dsk_dispatch_timer_get_expiration_millis (DskDispatchTimer *);


/* see effective-timers.txt
   
 */
dsk_boolean  dsk_supports_effective_timers (void);

/* Idle functions */
typedef void (*DskIdleFunc)   (void               *func_data);
DskDispatchIdle *
      dsk_dispatch_add_idle (DskDispatch *dispatch,
                                    DskIdleFunc func,
                                    void               *func_data);
void  dsk_dispatch_remove_idle (DskDispatchIdle *);

/* Signal handling */
typedef void (*DskSignalHandler) (void *func_data);
DskDispatchSignal *
      dsk_dispatch_add_signal    (DskDispatch     *dispatch,
                                  int              signal_number,
                                  DskSignalHandler func,
                                  void            *func_data);
void  dsk_dispatch_remove_signal (DskDispatchSignal *signal);

/* Process termination */
typedef struct _DskDispatchChildInfo DskDispatchChildInfo;
struct _DskDispatchChildInfo
{
  int process_id;
  dsk_boolean killed;           /* killed by signal */
  int value;                    /* exit status or signal number */
};
typedef void (*DskChildHandler) (DskDispatchChildInfo  *info,
                                 void                  *func_data);
DskDispatchChild *
      dsk_dispatch_add_child    (DskDispatch       *dispatch,
                                 int                process_id,
                                 DskChildHandler    func,
                                 void              *func_data);
void  dsk_dispatch_remove_child (DskDispatchChild  *handler);

/* --- API for use in standalone application --- */
/* Where you are happy just to run poll(2). */

/* dsk_dispatch_run() 
 * Run one main-loop iteration, using poll(2) (or some system-level event system).
 */
void  dsk_dispatch_run      (DskDispatch *dispatch);



/* --- API for those who want to embed a dispatch into their own main-loop --- */
typedef struct _DskFileDescriptorNotify DskFileDescriptorNotify;
struct _DskFileDescriptorNotify
{
  DskFileDescriptor fd;
  Dsk_Events events;
};

typedef struct _DskFileDescriptorNotifyChange DskFileDescriptorNotifyChange;
struct _DskFileDescriptorNotifyChange
{
  DskFileDescriptor fd;
  Dsk_Events old_events;
  Dsk_Events events;
};

void  dsk_dispatch_dispatch (DskDispatch             *dispatch,
                             size_t                   n_notifies,
                             DskFileDescriptorNotify *notifies);
void  dsk_dispatch_clear_changes (DskDispatch *);


struct _DskDispatch
{
  /* changes to the events you are interested in. */
  /* (this handles closed file-descriptors 
     in a manner agreeable to epoll(2) and kqueue(2)) */
  size_t n_changes;
  DskFileDescriptorNotifyChange *changes;

  /* the complete set of events you are interested in. */
  size_t n_notifies_desired;
  DskFileDescriptorNotify *notifies_desired;

  /* If TRUE, return immediately. */
  dsk_boolean has_idle;

  /* number of milliseconds to wait if no events occur */
  dsk_boolean has_timeout;
  unsigned long timeout_secs;
  unsigned timeout_usecs;

  unsigned long last_dispatch_secs;
  unsigned last_dispatch_usecs;

  /* private data follows (see RealDispatch structure in .c file) */
};

void dsk_dispatch_destroy_default (void);

#endif
