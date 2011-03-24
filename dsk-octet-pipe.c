#include "dsk-common.h"
#include "dsk-mem-pool.h"
#include "dsk-hook.h"
#include "dsk-object.h"
#include "dsk-error.h"
#include "dsk-buffer.h"
#include "dsk-octet-io.h"
#include "dsk-memory.h"

/* TODO: if Source shuts down, we should also disconnect */

typedef struct _DskPipeSinkClass DskPipeSinkClass;
struct _DskPipeSinkClass
{
  DskOctetSinkClass base_class;
};

typedef struct _DskPipeSink DskPipeSink;
struct _DskPipeSink
{
  DskOctetSink base_instance;
  DskMemorySource *source;
  DskHookTrap *buffer_low_trap;
};
static DskIOResult dsk_pipe_sink_write         (DskOctetSink   *sink,
                                                unsigned        max_len,
                                                const void     *data_out,
                                                unsigned       *n_written_out,
                                                DskError      **error);
static DskIOResult dsk_pipe_sink_write_buffer  (DskOctetSink   *sink,
                                                DskBuffer      *write_buffer,
                                                DskError      **error);
static void        dsk_pipe_sink_shutdown      (DskOctetSink   *sink);
static void        dsk_pipe_sink_finalize      (DskPipeSink    *sink);

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskPipeSink);
static DskPipeSinkClass dsk_pipe_sink_class =
{ {
  DSK_OBJECT_CLASS_DEFINE(DskPipeSink, &dsk_octet_sink_class,
                          NULL,
                          dsk_pipe_sink_finalize),
  dsk_pipe_sink_write,
  dsk_pipe_sink_write_buffer,
  dsk_pipe_sink_shutdown
} };

#define DSK_PIPE_SINK(object) DSK_OBJECT_CAST(DskPipeSink, object, &dsk_pipe_sink_class)


static dsk_boolean
handle_source_buffer_low (DskMemorySource *source,
                          DskPipeSink     *pipe_sink)
{
  dsk_assert (pipe_sink->source == source);
  dsk_hook_set_idle_notify (&pipe_sink->base_instance.writable_hook, DSK_TRUE);
  pipe_sink->buffer_low_trap = NULL;
  return DSK_FALSE;
}

/* returns -1 on error */
static DskIOResult
dsk_pipe_sink_write         (DskOctetSink   *sink,
                             unsigned        max_len,
                             const void     *data_out,
                             unsigned       *n_written_out,
                             DskError      **error)
{
  DskPipeSink *psink = DSK_PIPE_SINK (sink);
  unsigned length;
  DskMemorySource *msource = psink->source;
  if (msource == NULL || msource->got_shutdown)
    {
      dsk_set_error (error, "write to disconnected pipe");
      return DSK_IO_RESULT_ERROR;
    }
  if (max_len == 0)
    return DSK_IO_RESULT_SUCCESS;
  if (psink->source->buffer.size >= psink->source->buffer_low_amount)
    return DSK_IO_RESULT_AGAIN;
  length = psink->source->buffer_low_amount - psink->source->buffer.size;
  if (length > max_len)
    length = max_len;
  *n_written_out = length;
  dsk_buffer_append (&msource->buffer, length, data_out);
  if (psink->source->buffer.size == psink->source->buffer_low_amount)
    {
      dsk_hook_set_idle_notify (&sink->writable_hook, DSK_FALSE);
      if (psink->buffer_low_trap == NULL)
        psink->buffer_low_trap = dsk_hook_trap (&psink->source->buffer_low,
                                                (DskHookFunc) handle_source_buffer_low,
                                                psink, NULL);
    }
  dsk_memory_source_added_data (msource);
  return DSK_IO_RESULT_SUCCESS;
}

static DskIOResult
dsk_pipe_sink_write_buffer  (DskOctetSink   *sink,
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
      dsk_hook_set_idle_notify (&sink->writable_hook, DSK_FALSE);
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
  dsk_hook_set_idle_notify (&psink->base_instance.writable_hook, DSK_TRUE);
}

static void
dsk_pipe_sink_shutdown      (DskOctetSink   *sink)
{
  /* NB: this is called by dsk_pipe_sink_finalize()- so it must
     never ref/unref 'sink' */
  DskPipeSink *psink = DSK_PIPE_SINK (sink);
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
dsk_pipe_sink_finalize (DskPipeSink *sink)
{
  if (sink->source != NULL)
    dsk_pipe_sink_shutdown (DSK_OCTET_SINK (sink));
}

void
dsk_octet_pipe_new (unsigned pipe_size,
                    DskOctetSink **sink_out,
                    DskOctetSource **source_out)
{
  DskPipeSink *sink = dsk_object_new (&dsk_pipe_sink_class);
  DskMemorySource *source = dsk_memory_source_new ();
  if (pipe_size == 0)
    source->buffer_low_amount = 4096;           /* default */
  else
    source->buffer_low_amount = pipe_size;
  *source_out = DSK_OCTET_SOURCE (source);
  *sink_out = DSK_OCTET_SINK (sink);
  sink->source = source;
  dsk_hook_set_idle_notify (&sink->base_instance.writable_hook, DSK_TRUE);
  dsk_object_trap_finalize (DSK_OBJECT (source), source_finalized, sink);
}
