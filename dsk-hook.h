
typedef dsk_boolean (*DskHookFunc)    (void       *object,
                                       void       *callback_data);
typedef void        (*DskHookDestroy) (void       *callback_data);
typedef void        (*DskHookSetPoll) (void       *object,
                                       dsk_boolean is_trapped);
typedef void        (*DskHookObjectFunc) (void *object);


typedef struct _DskHookTrap DskHookTrap;
typedef struct _DskHook DskHook;

typedef struct _DskHookFuncs DskHookFuncs;
struct _DskHookFuncs
{
  DskHookObjectFunc ref;
  DskHookObjectFunc unref;
  DskHookSetPoll set_poll;
};

struct _DskHookTrap
{
  DskHookFunc callback;		/* != NULL */
  void *callback_data;
  DskHookDestroy callback_data_destroy;
  DskHook *owner;
  DskHookTrap *next;
  unsigned is_notifying : 1;
  unsigned destroy_in_notify : 1;
  unsigned short block_count;
};
  
struct _DskHook
{
  /* NOTE: these must be first, because we zero out everything
     after the first two pointers in dsk_hook_init() */
  void *object;
  DskHookFuncs *funcs;

  /* the remaining methods are zeroed initially */
  unsigned char is_idle_notify : 1;
  unsigned char is_cleared : 1;
  unsigned char is_notifying : 1;
  unsigned char clear_in_notify : 1;
  unsigned char in_set_poll : 1;
  unsigned char magic;
  unsigned short trap_count;
  DskHookTrap trap;
  DskHook *idle_prev, *idle_next;
};

#define DSK_HOOK_MAGIC          0x81

DSK_INLINE_FUNC dsk_boolean  dsk_hook_is_trapped   (DskHook       *hook);
DSK_INLINE_FUNC DskHookTrap *dsk_hook_trap         (DskHook       *hook,
                                                    DskHookFunc    func,
						    void          *hook_data,
						    DskHookDestroy destroy);
                void         dsk_hook_trap_destroy (DskHookTrap   *trap);
DSK_INLINE_FUNC void         dsk_hook_trap_block   (DskHookTrap   *trap);
DSK_INLINE_FUNC void         dsk_hook_trap_unblock (DskHookTrap   *trap);

/* Like dsk_hook_trap_destroy(), but does not call 'destroy' callback. */
DSK_INLINE_FUNC void         dsk_hook_trap_free    (DskHookTrap   *trap);

/* ????? for use by the underlying polling mechanism
 * (for hooks not using idle-notify) */
/* TODO: dsk_hook_init_zeroed() for use in object constructors? */
DSK_INLINE_FUNC void         dsk_hook_init         (DskHook       *hook,
                                                    void          *object);
DSK_INLINE_FUNC void         dsk_hook_set_funcs    (DskHook       *hook,
                                                    DskHookFuncs  *static_funcs);
DSK_INLINE_FUNC void         dsk_hook_set_idle_notify(DskHook       *hook,
                                                      dsk_boolean    idle_notify);
                void         dsk_hook_notify       (DskHook       *hook);
                void         dsk_hook_clear        (DskHook       *hook);

extern DskHookFuncs dsk_hook_funcs_default;
extern DskMemPoolFixed dsk_hook_trap_pool;

void _dsk_hook_trap_count_nonzero (DskHook *);
void _dsk_hook_trap_count_zero (DskHook *);
void _dsk_hook_add_to_idle_notify_list (DskHook *hook);
void _dsk_hook_remove_from_idle_notify_list (DskHook *hook);

#if DSK_CAN_INLINE || defined(DSK_IMPLEMENT_INLINES)
DSK_INLINE_FUNC dsk_boolean
dsk_hook_is_trapped (DskHook *hook)
{
  return hook->trap_count > 0;
} 
DSK_INLINE_FUNC void
_dsk_hook_incr_trap_count         (DskHook       *hook)
{
  _dsk_inline_assert (hook->magic == DSK_HOOK_MAGIC);
  if (++(hook->trap_count) == 1)
    _dsk_hook_trap_count_nonzero (hook);
} 
DSK_INLINE_FUNC void
_dsk_hook_decr_trap_count         (DskHook       *hook)
{
  _dsk_inline_assert (hook->magic == DSK_HOOK_MAGIC);
  if (--(hook->trap_count) == 0)
    _dsk_hook_trap_count_zero (hook);
} 

DSK_INLINE_FUNC void         dsk_hook_init         (DskHook       *hook,
                                                    void          *object)
{
  dsk_bzero_pointers ((void**)hook + 2, sizeof (DskHook)/sizeof(void*) - 2);
  hook->object = object;
  hook->funcs = &dsk_hook_funcs_default;
  hook->magic = DSK_HOOK_MAGIC;
}

DSK_INLINE_FUNC DskHookTrap *
dsk_hook_trap         (DskHook       *hook,
                       DskHookFunc    func,
                       void          *data,
                       DskHookDestroy destroy)
{
  DskHookTrap *trap;
  _dsk_inline_assert (hook->magic == DSK_HOOK_MAGIC);
  _dsk_inline_assert (func != NULL);
  if (hook->trap.callback == NULL)
    {
      trap = &hook->trap;
    }
  else
    {
      DskHookTrap *last = &hook->trap;
      trap = dsk_mem_pool_fixed_alloc (&dsk_hook_trap_pool);
      while (last->next != NULL)
        last = last->next;
      last->next = trap;
      trap->next = NULL;
    }
  
  trap->callback = func;
  trap->callback_data = data;
  trap->callback_data_destroy = destroy;
  trap->owner = hook;
  trap->is_notifying = 0;
  trap->block_count = 0;
  trap->destroy_in_notify = 0;
  _dsk_hook_incr_trap_count (hook);

  return trap;
}
DSK_INLINE_FUNC void
dsk_hook_trap_block   (DskHookTrap   *trap)
{
  _dsk_inline_assert (trap->owner->magic == DSK_HOOK_MAGIC);
  _dsk_inline_assert (trap->callback != NULL);
  _dsk_inline_assert (trap->block_count != 0xffff);
  if (++(trap->block_count) == 1)
    _dsk_hook_decr_trap_count (trap->owner);
}

DSK_INLINE_FUNC void
dsk_hook_trap_unblock (DskHookTrap   *trap)
{
  _dsk_inline_assert (trap->owner->magic == DSK_HOOK_MAGIC);
  _dsk_inline_assert (trap->callback != NULL);
  _dsk_inline_assert (trap->block_count != 0);
  if (--(trap->block_count) == 0)
    _dsk_hook_incr_trap_count (trap->owner);
}

DSK_INLINE_FUNC void
dsk_hook_set_idle_notify(DskHook       *hook,
                         dsk_boolean    idle_notify)
{
  _dsk_inline_assert (hook->magic == DSK_HOOK_MAGIC);
  if (idle_notify)
    idle_notify = 1;
  if (idle_notify == hook->is_idle_notify)
    return;
  hook->is_idle_notify = idle_notify;
  if (hook->trap_count > 0)
    {
      if (idle_notify)
        _dsk_hook_add_to_idle_notify_list (hook);
      else
        _dsk_hook_remove_from_idle_notify_list (hook);
    }
}

DSK_INLINE_FUNC void
dsk_hook_set_funcs    (DskHook       *hook,
                       DskHookFuncs  *static_funcs)
{
  hook->funcs = static_funcs;
}

DSK_INLINE_FUNC void
dsk_hook_trap_free (DskHookTrap   *trap)
{
  trap->callback_data_destroy = NULL;
  dsk_hook_trap_destroy (trap);
}

#endif


