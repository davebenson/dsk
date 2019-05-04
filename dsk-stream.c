#include "dsk.h"

static void
dsk_stream_init (DskStream *stream)
{
  dsk_hook_init (&stream->readable_hook, stream);
  dsk_hook_init (&stream->writable_hook, stream);
}

static void
dsk_stream_finalize (DskStream *stream)
{
  if (!dsk_hook_is_cleared (&stream->readable_hook))
    dsk_hook_clear (&stream->readable_hook);
  if (!dsk_hook_is_cleared (&stream->writable_hook))
    dsk_hook_clear (&stream->writable_hook);
  if (stream->latest_error)
    dsk_error_unref (stream->latest_error);
}

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA (DskStream);
const DskStreamClass dsk_stream_class =
{
  DSK_OBJECT_CLASS_DEFINE (DskStream, &dsk_object_class, 
                           dsk_stream_init,
                           dsk_stream_finalize),
  NULL,                 /* no default read impl */
  NULL,                 /* no default read_buffer impl */
  NULL,                 /* no default shutdown_read impl */
  NULL,                 /* no default write impl */
  NULL,                 /* no default write_buffer impl */
  NULL,                 /* no default shutdown_write impl */
};

void
dsk_stream_set_last_error (DskStream  *stream,
                           const char      *format,
                           ...)
{
  va_list args;
  va_start (args, format);
  if (stream->latest_error)
    dsk_error_unref (stream->latest_error);
  stream->latest_error = dsk_error_new_valist (format, args);
  va_end (args);
}


/* HACK */
int64_t
dsk_stream_get_length (DskStream *stream)
{
  void *class = DSK_OBJECT_GET_CLASS (stream);
  if (class == &dsk_memory_source_class)
    {
      DskMemorySource *msource = DSK_MEMORY_SOURCE (stream);
      if (msource->done_adding)
        return msource->buffer.size;
    }
  return -1;
}
