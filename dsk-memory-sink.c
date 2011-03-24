#include "dsk.h"

static void
dsk_memory_sink_init (DskMemorySink *sink)
{
  dsk_hook_init (&sink->buffer_nonempty, sink);
  dsk_hook_set_idle_notify (&sink->base_instance.writable_hook, DSK_TRUE);
}

static void
dsk_memory_sink_finalize (DskMemorySink *sink)
{
  dsk_buffer_clear (&sink->buffer);
  dsk_hook_clear (&sink->buffer_nonempty);
}

static DskIOResult
dsk_memory_sink_write (DskOctetSink   *sink,
                       unsigned        max_len,
                       const void     *data_out,
                       unsigned       *n_written_out,
                       DskError      **error)
{
  DskMemorySink *msink = DSK_MEMORY_SINK (sink);
  DSK_UNUSED (error);
  if (msink->buffer.size >= msink->max_buffer_size)
    return DSK_IO_RESULT_AGAIN;
  if (msink->buffer.size + max_len <= msink->max_buffer_size)
    *n_written_out = max_len;
  else
    *n_written_out = msink->max_buffer_size - msink->buffer.size;
  dsk_buffer_append (&msink->buffer, *n_written_out, data_out);
  dsk_hook_set_idle_notify (&msink->buffer_nonempty, DSK_TRUE);
  return DSK_IO_RESULT_SUCCESS;
}

static DskIOResult
dsk_memory_sink_write_buffer (DskOctetSink   *sink,
                              DskBuffer      *buffer,
                              DskError      **error)
{
  DskMemorySink *msink = DSK_MEMORY_SINK (sink);
  unsigned max_xfer = msink->max_buffer_size - msink->buffer.size;
  DSK_UNUSED (error);
  if (msink->buffer.size >= msink->max_buffer_size)
    return DSK_IO_RESULT_AGAIN;
  dsk_buffer_transfer (&msink->buffer, buffer, max_xfer);
  dsk_hook_set_idle_notify (&msink->buffer_nonempty, DSK_TRUE);
  return DSK_IO_RESULT_SUCCESS;
}
static void
dsk_memory_sink_shutdown (DskOctetSink   *sink)
{
  DSK_MEMORY_SINK (sink)->got_shutdown = DSK_TRUE;
}

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA (DskMemorySink);
DskMemorySinkClass dsk_memory_sink_class =
{
  {
    DSK_OBJECT_CLASS_DEFINE(DskMemorySink,
                            &dsk_octet_sink_class,
                            dsk_memory_sink_init,
                            dsk_memory_sink_finalize),
    dsk_memory_sink_write,
    dsk_memory_sink_write_buffer,
    dsk_memory_sink_shutdown
  }
};

void dsk_memory_sink_drained (DskMemorySink *sink)
{
  if (sink->buffer.size == 0)
    dsk_hook_set_idle_notify (&sink->buffer_nonempty, DSK_FALSE);
}
