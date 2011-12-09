
#ifndef DSK_ENABLE_DEBUGGING
#define DSK_ENABLE_DEBUGGING 0
#endif

#if DSK_ENABLE_DEBUGGING

extern dsk_boolean dsk_debug_object_lifetimes;
extern dsk_boolean dsk_debug_hooks;
extern dsk_boolean dsk_debug_connections;

#else

#define dsk_debug_object_lifetimes  DSK_FALSE
#define dsk_debug_hooks             DSK_FALSE
#define dsk_debug_connections       DSK_FALSE

#endif
