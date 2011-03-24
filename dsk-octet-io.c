#include "dsk.h"

static void
dsk_octet_source_init (DskOctetSource *source)
{
  dsk_hook_init (&source->readable_hook, source);
}

static void
dsk_octet_source_finalize (DskOctetSource *source)
{
  DskOctetStream *stream = source->stream;
  if (!source->readable_hook.is_cleared)
    dsk_hook_clear (&source->readable_hook);
  if (stream != NULL)
    {
      dsk_assert (stream->source == source);
      stream->source = NULL;
      source->stream = NULL;
      dsk_object_unref (stream);
    }
}

static void
dsk_octet_sink_init (DskOctetSink *sink)
{
  dsk_hook_init (&sink->writable_hook, sink);
}

static void
dsk_octet_sink_finalize (DskOctetSink *sink)
{
  DskOctetStream *stream = sink->stream;
  if (!sink->writable_hook.is_cleared)
    dsk_hook_clear (&sink->writable_hook);
  if (stream != NULL)
    {
      dsk_assert (stream->sink == sink);
      stream->sink = NULL;
      sink->stream = NULL;
      dsk_object_unref (stream);
    }
}

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA (DskOctetSink);
const DskOctetSinkClass dsk_octet_sink_class =
{
  DSK_OBJECT_CLASS_DEFINE (DskOctetSink, &dsk_object_class, 
                           dsk_octet_sink_init,
                           dsk_octet_sink_finalize),
  NULL,                 /* no default write impl */
  NULL,                 /* no default write_buffer impl */
  NULL,                 /* no default shutdown impl */
};

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA (DskOctetSource);
const DskOctetSourceClass dsk_octet_source_class =
{
  DSK_OBJECT_CLASS_DEFINE (DskOctetSource, &dsk_object_class, 
                           dsk_octet_source_init,
                           dsk_octet_source_finalize),
  NULL,                 /* no default read impl */
  NULL,                 /* no default read_buffer impl */
  NULL,                 /* no default shutdown impl */
};

static void
dsk_octet_stream_init (DskOctetStream *stream)
{
  dsk_hook_init (&stream->error_hook, stream);
}

static void
dsk_octet_stream_finalize (DskOctetStream *stream)
{
  if (!stream->error_hook.is_cleared)
    dsk_hook_clear (&stream->error_hook);
  if (stream->latest_error)
    dsk_error_unref (stream->latest_error);
  dsk_assert (stream->sink == NULL);
  dsk_assert (stream->source == NULL);
}

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA (DskOctetStream);
const DskOctetStreamClass dsk_octet_stream_class =
{
  DSK_OBJECT_CLASS_DEFINE (DskOctetStream, &dsk_object_class, 
                           dsk_octet_stream_init,
                           dsk_octet_stream_finalize)
};

void
dsk_octet_stream_set_last_error (DskOctetStream  *stream,
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
dsk_octet_source_get_length (DskOctetSource *source)
{
  void *class = DSK_OBJECT_GET_CLASS (source);
  if (class == &dsk_memory_source_class)
    {
      DskMemorySource *msource = DSK_MEMORY_SOURCE (source);
      if (msource->done_adding)
        return msource->buffer.size;
    }
  return -1;
}
