#include "dsk.h"

DskIOResult
dsk_octet_source_read_buffer (void         *octet_source,
                              DskBuffer    *read_buffer,
                              DskError    **error)
{
  DskOctetSourceClass *c = DSK_OCTET_SOURCE_GET_CLASS (octet_source);
  if (c->read_buffer)
    {
      return c->read_buffer (octet_source, read_buffer, error);
    }
  else if (c->readv)
    {
      unsigned n_data;
      DskData data[2];
      DskBufferFragment *f = read_buffer->last_frag;
      if (f != NULL
       && !f->is_foreign
       && f->buf_start + f->buf_length < f->buf_max_size)
        {
          data[0].data = f->buf + f->buf_start + f->buf_length;
          data[0].length = f->buf_max_size - f->buf_start - f->buf_length;
          n_data = 1;
        }
      else
        n_data = 0;
      f = dsk_buffer_fragment_alloc ();
      data[n_data].data = f->buf;
      data[n_data].length = f->buf_max_size;
      n_data++;

      size_t bytes_read;
      DskIOResult result = c->readv (octet_source, n_data, data, &bytes_read, error);
      if (result == DSK_IO_RESULT_SUCCESS && bytes_read != 0)
        {
          if (n_data == 2)
            {
              /* handle data written to existing fragment */
              if (bytes_read <= data[0].length)
                {
                  read_buffer->size += bytes_read;
                  read_buffer->last_frag->buf_length += bytes_read;
                  dsk_buffer_fragment_free (f);
                  return DSK_IO_RESULT_SUCCESS;
                }
              else
                {
                  read_buffer->last_frag->buf_length += data[0].length;
                  f->buf_length = bytes_read - data[0].length;
                  read_buffer->size += data[0].length;
                }
            }
          else
            f->buf_length = bytes_read;
          dsk_buffer_append_fragment (read_buffer, f);
        }
      else
        dsk_buffer_fragment_free (f);
      return result;
    }
  else
    {
      /* use read() */
      ...
    }
}

DskIOResult
dsk_octet_source_readv(void         *octet_source,
                       unsigned      n_data,
                       DskData      *data,
                       size_t       *n_bytes_read_out,
                       DskError    **error)
{
  DskOctetSourceClass *c = DSK_OCTET_SOURCE_GET_CLASS (octet_source);
  dsk_boolean did_read = DSK_FALSE;
  if (c->readv)
    return c->readv (octet_source, n_data, data, n_bytes_read_out, error);
  while (n_data > 0)
    {
      switch (c->read (octet_source, data->length, data->data, &n_read, error))
        {
        case DSK_IO_RESULT_SUCCESS:
          *n_bytes_read_out += n_read;
          did_read = DSK_TRUE;
          break;
        case DSK_IO_RESULT_AGAIN:
          return did_read ? DSK_IO_RESULT_SUCCESS : DSK_IO_RESULT_AGAIN;
        case DSK_IO_RESULT_ERROR:
          return DSK_IO_RESULT_ERROR;
        case DSK_IO_RESULT_EOF:
          return did_read ? DSK_IO_RESULT_SUCCESS : DSK_IO_RESULT_EOF;
        }
      if (n_read < data->length)
        return DSK_IO_RESULT_SUCCESS;
    }
  return DSK_IO_RESULT_SUCCESS;
}

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


#define MAX_WRITEV 32
DskIOResult         dsk_octet_sink_write_buffer  (void           *octet_sink,
                                                  DskBuffer      *write_buffer,
                                                  DskError      **error)
{
  DskData data[MAX_WRITEV];
  unsigned n_data = 0;
  DskBufferFragment *at;
  for (at = write_buffer->first_frag; at && n_data < MAX_WRITEV; at = at->next, n_data++)
    {
      data[n_data].data = at->buf + at->buf_start + at->buf_length;
      data[n_data].length = at->buf_max_size - at->buf_start - at->buf_length;
    }
  if (n_data == 0)
    return DSK_IO_RESULT_SUCCESS;
  size_t bytes_written;
  DskIOResult rv = dsk_octet_sink_writev (octet_sink, n_data, data, &bytes_written, error);
  if (rv == DSK_IO_RESULT_SUCCESS)
    {
      dsk_assert (bytes_written <= write_buffer->size);
      dsk_buffer_discard (write_buffer, bytes_written);
    }
  return rv;
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
