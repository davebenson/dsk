

typedef void (*DskStreamDebugWriteFunc) (DskBuffer *buffer, void *write_data);

typedef struct DskStreamDebugOptions DskStreamDebugOptions;
struct DskStreamDebugOptions
{
  unsigned debug_write : 1;
  unsigned debug_read : 1;
  unsigned debug_write_content : 1;
  unsigned debug_read_content : 1;
  unsigned debug_shutdown_write : 1;
  unsigned debug_shutdown_read : 1;
  unsigned debug_finalize : 1;

  DskStreamDebugWriteFunc write_buffer;
  void *write_buffer_data;
  DskDestroyNotify write_buffer_data_destroy;
};

#define DSK_STREAM_DEBUG_OPTIONS_INIT         \
(DskStreamDebugOptions) {                     \
  1,    /* debug_write */                     \
  1,    /* debug_read */                      \
  1,    /* debug_write_content */             \
  1,    /* debug_read_content */              \
  1,    /* debug_shutdown_write */            \
  1,    /* debug_shutdown_read */             \
  1,    /* debug_finalize */                  \
  dsk_stream_debug_write_to_fd,               \
  (void *) 2,   /* stderr */                  \
  NULL,         /* no destructor for write data */ \
}
DskStream *dsk_stream_debug_new_take (DskStream *underlying,
                                      const DskStreamDebugOptions *options);

typedef struct { DskStreamClass base_class; } DskStreamDebugClass;
typedef struct DskStreamDebug DskStreamDebug;
extern DskStreamDebugClass dsk_stream_debug_class;

#define DSK_STREAM_DEBUG(object) DSK_OBJECT_CAST(DskStreamDebug, object, &dsk_stream_debug_class)


void dsk_stream_debug_set_writer  (DskStreamDebug *debug,
                                   DskStreamDebugWriteFunc write,
                                   void *write_data,
                                   DskDestroyNotify write_destroy);



void dsk_stream_debug_write_to_fd (DskBuffer *buffer, void *write_data);
