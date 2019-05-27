#include "dsk.h"

/* TODO: if Source shuts down, we should also disconnect */

#define DSK_PIPE(object) DSK_OBJECT_CAST(DskPipe, object, &dsk_pipe_class)
#define DSK_PIPE_GET_CLASS(object) DSK_OBJECT_CAST_GET_CLASS(DskPipe, object, &dsk_pipe_class)

typedef struct DskPipeClass DskPipeClass;
struct DskPipeClass
{
  DskStreamClass base_class;
};
typedef struct DskPipe DskPipe;
struct DskPipe
{
  DskStream base_instance;
  DskPipe *peer;
  DskHookTrap *buffer_low_trap;
  DskBuffer readable_data;
};
static DskIOResult dsk_pipe_write         (DskStream   *sink,
                                                unsigned        max_len,
                                                const void     *data_out,
                                                unsigned       *n_written_out,
                                                DskError      **error);
static DskIOResult dsk_pipe_write_buffer       (DskStream   *sink,
                                                DskBuffer      *write_buffer,
                                                DskError      **error);
static void        dsk_pipe_shutdown_write   (DskStream   *sink);
static DskIOResult dsk_pipe_read          (DskStream   *source,
                                                unsigned        max_len,
                                                void           *data_out,
                                                unsigned       *n_written_out,
                                                DskError      **error);
static DskIOResult dsk_pipe_read_buffer       (DskStream   *source,
                                                DskBuffer      *read_buffer,
                                                DskError      **error);
static void        dsk_pipe_shutdown_read   (DskStream   *source);
static void        dsk_pipe_finalize         (DskPipe    *pipe);

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskPipe);
static DskPipeClass dsk_pipe_class =
{ {
  DSK_OBJECT_CLASS_DEFINE(DskPipe, &dsk_stream_class,
                          NULL,
                          dsk_pipe_finalize),
  dsk_pipe_read,
  dsk_pipe_read_buffer,
  dsk_pipe_shutdown_read,
  dsk_pipe_write,
  dsk_pipe_write_buffer,
  dsk_pipe_shutdown_write,
} };

static bool
handle_source_buffer_low (DskPipe         *source,
                          DskPipe         *pipe_sink)
{
  dsk_assert (source->peer == pipe_sink);
  dsk_assert (pipe_sink->peer == source);
  dsk_hook_set_idle_notify (&pipe_sink->base_instance.writable_hook, true);
  pipe_sink->buffer_low_trap = NULL;
  return false;
}

/* returns -1 on error */
static DskIOResult
dsk_pipe_write              (DskStream      *s,
                             unsigned        max_len,
                             const void     *data_out,
                             unsigned       *n_written_out,
                             DskError      **error)
{
  DskPipe *pipe = DSK_PIPE (s);
  unsigned length;
  DskPipe *peer = pipe->peer;
  if (peer == NULL || !dsk_stream_is_readable (peer))
    {
      dsk_set_error (error, "write to disconnected pipe");
      return DSK_IO_RESULT_ERROR;
    }
  if (max_len == 0)
    return DSK_IO_RESULT_SUCCESS;
  dsk_buffer_append (&peer->readable_data, max_len, data);
  *n_written_out = max_len;
  if (peer->buffer.size >= peer->buffer_low_amount
   && peer->buffer_low_trap != NULL)
    {
      dsk_hook_trap_destroy (peer->buffer_low_trap);
    
    return DSK_IO_RESULT_AGAIN;
  length = psink->source->buffer_low_amount - psink->source->buffer.size;
  if (length > max_len)
    length = max_len;
  *n_written_out = length;
  dsk_buffer_append (&msource->buffer, length, data_out);
  if (psink->source->buffer.size == psink->source->buffer_low_amount)
    {
      dsk_hook_set_idle_notify (&sink->writable_hook, false);
      if (psink->buffer_low_trap == NULL)
        psink->buffer_low_trap = dsk_hook_trap (&psink->source->buffer_low,
                                                (DskHookFunc) handle_source_buffer_low,
                                                psink, NULL);
    }
  dsk_memory_source_added_data (msource);
  return DSK_IO_RESULT_SUCCESS;
}

static DskIOResult
dsk_pipe_write_buffer  (DskStream *s,
                             DskBuffer      *write_buffer,
                             DskError      **error)
{
  DskPipeSink *psink = DSK_PIPE_SINK (sink);
  DskMemorySource *msource = psink->source;
  unsigned length;
  if (msource == NULL)
    {
      dsk_set_error (error, "write to disconnected pipe");
      return DSK_IO_RESULT_ERROR;
    }
  if (write_buffer->size == 0)
    return DSK_IO_RESULT_SUCCESS;
  if (psink->source->buffer.size >= psink->source->buffer_low_amount)
    return DSK_IO_RESULT_AGAIN;
  length = psink->source->buffer_low_amount - psink->source->buffer.size;
  if (length > write_buffer->size)
    length = write_buffer->size;
  dsk_buffer_transfer (&msource->buffer, write_buffer, length);
  if (psink->source->buffer.size == psink->source->buffer_low_amount)
    {
      dsk_hook_set_idle_notify (&sink->writable_hook, false);
      if (psink->buffer_low_trap == NULL)
        psink->buffer_low_trap = dsk_hook_trap (&psink->source->buffer_low,
                                                (DskHookFunc) handle_source_buffer_low,
                                                psink, NULL);
    }
  dsk_memory_source_added_data (msource);
  return DSK_IO_RESULT_SUCCESS;
}

static void
source_finalized (void *data)
{
  DskPipeSink *psink = DSK_PIPE_SINK (data);
  psink->buffer_low_trap = NULL;
  psink->source = NULL;

  /* notify the user so they get an error ("epipe") */
  dsk_hook_set_idle_notify (&psink->base_instance.writable_hook, true);
}

static void
dsk_pipe_shutdown_write      (DskStream *s)
{
  /* NB: this is called by dsk_pipe_sink_finalize()- so it must
     never ref/unref 'sink' */
  DskPipe *pipe = DSK_PIPE (s);
  if (psink->source)
    {
      dsk_object_untrap_finalize (DSK_OBJECT (psink->source),
                                  source_finalized, psink);
      if (psink->buffer_low_trap)
        dsk_hook_trap_destroy (psink->buffer_low_trap);
      dsk_memory_source_done_adding (psink->source);
      psink->source = NULL;
    }
}

static void
dsk_pipe_sink_finalize (DskPipe *pipe)
{
  if (pipe->buffer_low_trap != NULL)
    {
      dsk_hook_trap_destroy (pipe->buffer_low_trap);
      pipe->buffer_low_trap = NULL;
    }
  if (sink->peer != NULL)
    {
      sink->peer->peer = NULL;
      sink->peer = NULL;
    }
  dsk_buffer_clear (&pipe->readable_data);
}

void
dsk_pipe_new (unsigned pipe_size,
              DskStream **sink_out,
              DskStream **source_out)
{
  DskPipe *sink = dsk_object_new (&dsk_pipe_class);
  DskPipe *source = dsk_object_new (&dsk_pipe_class);
  sink->peer = source;
  source->peer = sink;
  DskMemorySource *source = dsk_memory_source_new ();
  if (pipe_size == 0)
    source->buffer_low_amount = 4096;           /* default */
  else
    source->buffer_low_amount = pipe_size;
  *source_out = DSK_OCTET_SOURCE (source);
  *sink_out = DSK_OCTET_SINK (sink);
  sink->source = source;
  dsk_hook_set_idle_notify (&sink->base_instance.writable_hook, true);
  dsk_object_trap_finalize (DSK_OBJECT (source), source_finalized, sink);
}
