#include "dsk.h"
#include <unistd.h>
#include <string.h>


struct DskStreamDebugClass
{
  DskStreamClass base_class;
};
struct DskStreamDebug
{
  DskStream base_instance;
  DskStreamDebugOptions options;
  DskHookTrap *write_trap;
  DskHookTrap *read_trap;
  char *tag;
  DskStream *substream;


  // Where debug text is written out temporarily.
  // debug_flush() will write that to the output medium.
  DskBuffer debug_buffer;

  DskStreamDebugWriteFunc write_func;
  void *write_data;
  DskDestroyNotify write_data_destroy;
};

static bool
handle_substream_readable (void *underlying, void *data)
{
  DskStreamDebug *d = data;
  assert (d->substream == underlying);
  dsk_hook_notify (&d->base_instance.readable_hook);
  return true;
}

static void
dsk_stream_debug_set_poll_read (void *obj, bool is_trapped)
{
  DskStreamDebug *d = obj;
  if (is_trapped && d->read_trap == NULL)
    {
      d->read_trap = dsk_hook_trap (&d->substream->readable_hook,
                                     handle_substream_readable,
                                     d, NULL);
    }
  else if (!is_trapped && d->read_trap != NULL)
    {
      dsk_hook_trap_destroy (d->read_trap);
      d->read_trap = NULL;
    }
}

static bool
handle_substream_writable (void *underlying, void *data)
{
  DskStreamDebug *d = data;
  assert (d->substream == underlying);
  dsk_hook_notify (&d->base_instance.writable_hook);
  return true;
}

static void
dsk_stream_debug_set_poll_write (void *obj, bool is_trapped)
{
  DskStreamDebug *d = obj;
  if (is_trapped && d->write_trap == NULL)
    {
      d->write_trap = dsk_hook_trap (&d->substream->writable_hook,
                                     handle_substream_writable,
                                     d, NULL);
    }
  else if (!is_trapped && d->write_trap != NULL)
    {
      dsk_hook_trap_destroy (d->write_trap);
      d->write_trap = NULL;
    }
}

static void
debug_write_quoted_string (DskStreamDebug *d, size_t n, const uint8_t *data)
{
  DskBuffer *out = &d->debug_buffer;
  const char hex[16] = "0123456789abcdef";
  for (size_t i = 0; i < n; i++)
    {
      char s[2] = { hex[*data >> 4], hex[*data & 0x0f] };
      dsk_buffer_append_small (out, 2, s);
      data++;
    }
}

static void
debug_flush (DskStreamDebug *d)
{
  if (d->debug_buffer.size > 0
   && dsk_buffer_get_last_byte (&d->debug_buffer) != '\n')
    dsk_buffer_append_byte (&d->debug_buffer, '\n');
  d->write_func (&d->debug_buffer, d->write_data);
  dsk_warn_if_fail (d->debug_buffer.size == 0, "write function failed");
}

static void
line_start_vprintf (DskStreamDebug *d,
                    const char *format,
                    va_list args)
{
  DskBuffer *out = &d->debug_buffer;
  dsk_buffer_append_string (out, d->tag == NULL ? "StreamDebug" : d->tag);
  dsk_buffer_append_small (out, 2, ": ");
  dsk_buffer_vprintf (out, format, args);
}

static void
line_start_printf (DskStreamDebug *d, 
                   const char *format,
                   ...) DSK_GNUC_PRINTF(2,3);
static void
line_start_printf (DskStreamDebug *d, 
                   const char *format,
                   ...)
{
  va_list args;
  va_start (args, format);
  line_start_vprintf (d, format, args);
  va_end (args);
}

#if 0
static void
flush_buffer (DskBuffer *buffer)
{
  if (buffer->size == 0)
    return;

  if (dsk_buffer_get_last_byte (buffer) != '\n')
    dsk_buffer_append_byte (buffer, '\n');

  dsk_buffer_writev (buffer, STDERR_FILENO);
  dsk_buffer_clear (buffer);
}
#endif

static DskIOResult
dsk_stream_debug_read  (DskStream      *stream,
                        unsigned        max_len,
                        void           *data_out,
                        unsigned       *bytes_read_out,
                        DskError      **error)
{
  DskStreamDebug *d = DSK_STREAM_DEBUG (stream);
  assert (d->debug_buffer.size == 0);
  DskError *e = NULL;
  DskIOResult res = dsk_stream_read (d->substream, max_len, data_out, bytes_read_out, &e);
  switch (res)
    {
    case DSK_IO_RESULT_SUCCESS:
      if (d->options.debug_read_content)
        {
          line_start_printf (d, "read %u out of %u bytes: ",
                        *bytes_read_out, max_len);
          debug_write_quoted_string (d, *bytes_read_out, data_out);
          dsk_buffer_append_byte (&d->debug_buffer, '\n');
        }
      else if (d->options.debug_read)
        line_start_printf (d, "read %u out of %u bytes\n",
                      *bytes_read_out, max_len);
      break;
    case DSK_IO_RESULT_ERROR:
      if (d->options.debug_read_content || d->options.debug_read)
        line_start_printf (d, "read error: %s\n", e->message);
      if (error)
        *error = e;
      else
        dsk_error_unref (e);
      break;
    case DSK_IO_RESULT_EOF:
      if (d->options.debug_read_content || d->options.debug_read)
        line_start_printf (d, "read end-of-file\n");
      break;
    case DSK_IO_RESULT_AGAIN:
      if (d->options.debug_read_content || d->options.debug_read)
        line_start_printf (d, "read: would block\n");
      break;
    }
  debug_flush (d);
  return res;
}

static DskIOResult
dsk_stream_debug_read_buffer (DskStream      *stream,
                              DskBuffer      *read_buffer,
                              DskError      **error)
{
  DskStreamDebug *d = DSK_STREAM_DEBUG (stream);
  size_t init_buffer_size = read_buffer->size;
  DskBufferFragment *frag = read_buffer->last_frag;
  size_t init_frag_length = frag ? frag->buf_length : 0;
  DskIOResult res = dsk_stream_read_buffer (d->substream, read_buffer, error);
  if (frag == NULL)
    frag = read_buffer->first_frag;
  if (frag == NULL)
    return res;

  if (init_buffer_size != read_buffer->size)
    {
      // Write initial fragment, which may be partial.
      if (frag != NULL)
        {
          if (frag->buf_length > init_frag_length)
            {
              size_t new_length = frag->buf_length - init_frag_length;
              const uint8_t *new_data = frag->buf
                                      + frag->buf_start
                                      + init_frag_length;
              debug_write_quoted_string (d, new_length, new_data);
            }
          frag = frag->next;
        }

      // Write remaining full segments.
      while (frag != NULL)
        {
          dsk_buffer_append_small (&d->debug_buffer, 2, ", ");
          debug_write_quoted_string (d, frag->buf_length,
                                     frag->buf + frag->buf_start);
          frag = frag->next;
        }
    }

  return res;
}

static void
dsk_stream_debug_shutdown_read(DskStream      *stream)
{
  DskStreamDebug *d = DSK_STREAM_DEBUG (stream);
  if (d->options.debug_read_content || d->options.debug_read)
    {
      line_start_printf (d, "shutdown read");
    }
  dsk_stream_shutdown_read (d->substream);
  debug_flush (d);
}

static DskIOResult
dsk_stream_debug_write        (DskStream      *stream,
                               unsigned        max_len,
                               const void     *data_out,
                               unsigned       *n_written_out,
                               DskError      **error)
{
  DskStreamDebug *d = DSK_STREAM_DEBUG (stream);
  DskIOResult rv = dsk_stream_write (d->substream, max_len, data_out, n_written_out, error);
  if (rv == DSK_IO_RESULT_SUCCESS && d->options.debug_write_content)
    {
      line_start_printf (d, "wrote bytes: ");
      debug_write_quoted_string (d, *n_written_out, data_out);
      debug_flush (d);
    }
  else if (d->options.debug_write_content || d->options.debug_write)
    {
      switch (rv)
        {
          case DSK_IO_RESULT_ERROR:
            line_start_printf (d, "write failed: %s", (*error)->message);
            break;
          case DSK_IO_RESULT_AGAIN:
            line_start_printf (d, "write again");
            break;
          case DSK_IO_RESULT_EOF:
            line_start_printf (d, "write eof?");
            break;
          case DSK_IO_RESULT_SUCCESS:
            line_start_printf (d, "wrote %u of %u bytes",
                               (unsigned) *n_written_out,
                               (unsigned) max_len);
            break;
        }
      debug_flush (d);
    }

  return rv;
}

static DskIOResult
dsk_stream_debug_write_buffer (DskStream      *stream,
                               DskBuffer      *write_buffer,
                               DskError      **error)
{
  DskStreamDebug *d = DSK_STREAM_DEBUG (stream);
  DskBuffer copy = DSK_BUFFER_INIT;
  size_t initial_size = write_buffer->size;
  if (d->options.debug_write_content)
    dsk_buffer_append_buffer (&copy, write_buffer);
  DskIOResult rv = dsk_stream_write_buffer (d->substream, write_buffer, error);
  if (rv == DSK_IO_RESULT_SUCCESS && d->options.debug_write_content)
    {
      size_t written = copy.size - write_buffer->size;
      line_start_printf (d, "write buffer succeeded: wrote ");
      DskBufferFragment *at = copy.first_frag;
      size_t rem = written;
      while (rem > 0)
        {
          assert (at != NULL);
          if (at->buf_length < rem)
            {
              debug_write_quoted_string (d, at->buf_length, at->buf + at->buf_start);
              rem -= at->buf_length;
            }
          else
            {
              debug_write_quoted_string (d, rem, at->buf + at->buf_start);
              rem = 0;
            }
          at = at->next;
        }
    }
  else if (d->options.debug_write || d->options.debug_write_content)
    {
      size_t written = initial_size - write_buffer->size;
      switch (rv)
        {
          case DSK_IO_RESULT_ERROR:
            line_start_printf (d, "write_buffer failed: %s", (*error)->message);
            break;
          case DSK_IO_RESULT_AGAIN:
            line_start_printf (d, "write_buffer again");
            break;
          case DSK_IO_RESULT_EOF:
            line_start_printf (d, "write_buffer eof?");
            break;
          case DSK_IO_RESULT_SUCCESS:
            line_start_printf (d, "wrote_buffer %u of %u bytes",
                               (unsigned) written,
                               (unsigned) initial_size);
            break;
        }
    }
  dsk_buffer_clear (&copy);
  debug_flush (d);
  return rv;
}

static void
dsk_stream_debug_shutdown_write(DskStream   *stream)
{
  DskStreamDebug *d = DSK_STREAM_DEBUG (stream);
  if (d->options.debug_write_content || d->options.debug_write)
    {
      line_start_printf (d, "shutdown write");
    }
  dsk_stream_shutdown_write (d->substream);
  debug_flush (d);
}

static DskHookFuncs debug_read_funcs =
{
  (DskHookObjectFunc) dsk_object_ref_f,
  dsk_object_unref_f,
  dsk_stream_debug_set_poll_read
};

static DskHookFuncs debug_write_funcs =
{
  (DskHookObjectFunc) dsk_object_ref_f,
  dsk_object_unref_f,
  dsk_stream_debug_set_poll_write
};

static void
dsk_stream_debug_init (DskStreamDebug *d)
{
  dsk_hook_set_funcs (&d->base_instance.readable_hook,
                      &debug_read_funcs);
  dsk_hook_set_funcs (&d->base_instance.writable_hook,
                      &debug_write_funcs);
}
static void
dsk_stream_debug_finalize (DskStreamDebug *d)
{
  dsk_buffer_clear (&d->debug_buffer);
  if (d->substream != NULL)
    dsk_object_unref (d->substream);
  if (d->write_data_destroy != NULL)
    d->write_data_destroy (d->write_data);
}

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskStreamDebug);

DskStreamDebugClass dsk_stream_debug_class = {
  {                     // DskStream
    DSK_OBJECT_CLASS_DEFINE (
      DskStreamDebug,
      &dsk_stream_class,
      dsk_stream_debug_init,
      dsk_stream_debug_finalize
    ),
    dsk_stream_debug_read,
    dsk_stream_debug_read_buffer,
    dsk_stream_debug_shutdown_read,
    dsk_stream_debug_write,
    dsk_stream_debug_write_buffer,
    dsk_stream_debug_shutdown_write,
  }
};

DskStream *
dsk_stream_debug_new_take (DskStream *underlying,
                          const DskStreamDebugOptions *options)
{
  DskStreamDebug *rv = dsk_object_new (&dsk_stream_debug_class);
  rv->substream = underlying;
  rv->options = *options;
  return (DskStream *) rv;
}

void dsk_stream_debug_set_writer  (DskStreamDebug *debug,
                                   DskStreamDebugWriteFunc write,
                                   void *write_data,
                                   DskDestroyNotify write_destroy)
{
  if (debug->write_data_destroy != NULL)
    debug->write_data_destroy (debug->write_data);
  debug->write_func = write;
  debug->write_data = write_data;
  debug->write_data_destroy = write_destroy;
}


void dsk_stream_debug_write_to_fd (DskBuffer *buffer, void *write_data)
{
  int fd = (int) write_data;
  while (buffer->size > 0)
    {
      if (dsk_buffer_writev (buffer, fd) < 0)
        {
          dsk_die ("dsk_stream_debug_write_to_fd: %s", strerror (errno));
        }
    }
}
