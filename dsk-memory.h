
typedef struct _DskMemorySourceClass DskMemorySourceClass;
typedef struct _DskMemorySinkClass DskMemorySinkClass;
typedef struct _DskMemorySource DskMemorySource;
typedef struct _DskMemorySink DskMemorySink;

#define DSK_MEMORY_SOURCE(object) DSK_OBJECT_CAST(DskMemorySource, object, &dsk_memory_source_class)
#define DSK_MEMORY_SINK(object) DSK_OBJECT_CAST(DskMemorySink, object, &dsk_memory_sink_class)

struct _DskMemorySourceClass
{
  DskStreamClass base_class;
};
struct _DskMemorySource
{
  DskStream base_instance;
  DskBuffer buffer;

  /* emit 'buffer_low' while buffer.size <= buffer_low_amount
     (so buffer_low_amount==0 means the hook
     is emitted iff the buffer is empty) */
  DskHook buffer_low;
  unsigned buffer_low_amount;

  DskError *error;

  unsigned done_adding : 1;
  unsigned got_shutdown : 1;
};
struct _DskMemorySinkClass
{
  DskStreamClass base_class;
};
struct _DskMemorySink
{
  DskStream base_instance;
  DskBuffer buffer;
  DskHook buffer_nonempty;
  bool got_shutdown;
  unsigned max_buffer_size;
};

#define dsk_memory_source_new()  (DskMemorySource *) dsk_object_new (&dsk_memory_source_class)
#define dsk_memory_sink_new()  (DskMemorySink *) dsk_object_new (&dsk_memory_sink_class)

/* used to pump data into the DskMemorySource */
void dsk_memory_source_done_adding (DskMemorySource *source);
void dsk_memory_source_added_data  (DskMemorySource *source);
void dsk_memory_source_add_error   (DskMemorySource *source,
                                    DskError        *error);
void dsk_memory_source_take_error  (DskMemorySource *source,
                                    DskError        *error);

/* used to indicate that data has been taken out of the memory-sink.
 * (It doesn't need to be completely empty) */
void dsk_memory_sink_drained       (DskMemorySink   *sink);

extern DskMemorySourceClass dsk_memory_source_class;
extern DskMemorySinkClass dsk_memory_sink_class;
