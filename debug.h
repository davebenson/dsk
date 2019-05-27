
#ifndef DSK_ENABLE_DEBUGGING
#define DSK_ENABLE_DEBUGGING 0
#endif

#if DSK_ENABLE_DEBUGGING

extern bool dsk_debug_object_lifetimes;
extern bool dsk_debug_hooks;
extern bool dsk_debug_connections;

#else

#define dsk_debug_object_lifetimes  false
#define dsk_debug_hooks             false
#define dsk_debug_connections       false

#endif
