#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#include "dsk.h"

DskOctetSource *dsk_octet_source_new_stdin (void)
{
  static dsk_boolean has_returned = DSK_FALSE;
  DskOctetStreamFdSource *source;
  DskError *error = NULL;
  if (has_returned)
    return NULL;
  has_returned = DSK_TRUE;
  if (!dsk_octet_stream_new_fd (STDIN_FILENO,
                                DSK_FILE_DESCRIPTOR_IS_NOT_WRITABLE
                                |DSK_FILE_DESCRIPTOR_IS_READABLE
                                |DSK_FILE_DESCRIPTOR_DO_NOT_CLOSE,
                                NULL, &source, NULL, &error))
    dsk_error ("standard-input not available: %s", error->message);
  return DSK_OCTET_SOURCE (source);
}

DskOctetSink *dsk_octet_sink_new_stdout (void)
{
  static dsk_boolean has_returned = DSK_FALSE;
  DskError *error = NULL;
  DskOctetStreamFdSink *sink;
  if (has_returned)
    return NULL;
  has_returned = DSK_TRUE;
  if (!dsk_octet_stream_new_fd (STDOUT_FILENO,
                                DSK_FILE_DESCRIPTOR_IS_NOT_READABLE
                                |DSK_FILE_DESCRIPTOR_IS_WRITABLE
                                |DSK_FILE_DESCRIPTOR_DO_NOT_CLOSE,
                                NULL, NULL, &sink,
                                &error))
    dsk_error ("standard-input not available: %s", error->message);
  return DSK_OCTET_SINK (sink);
}

static void
handle_fd_ready (DskFileDescriptor   fd,
                 unsigned       events,
                 void          *callback_data)
{
  DskOctetStreamFd *stream = DSK_OCTET_STREAM_FD (callback_data);
  dsk_assert (stream->fd == fd);
  dsk_object_ref (stream);
  if ((events & DSK_EVENT_READABLE) != 0 && stream->base_instance.source != NULL)
    dsk_hook_notify (&stream->base_instance.source->readable_hook);
  if ((events & DSK_EVENT_WRITABLE) != 0 && stream->base_instance.sink != NULL)
    dsk_hook_notify (&stream->base_instance.sink->writable_hook);
  dsk_object_unref (stream);
}

static void
stream_set_poll (DskOctetStreamFd *stream)
{
  unsigned flags = 0;
  if (stream->base_instance.sink != NULL
   && dsk_hook_is_trapped (&stream->base_instance.sink->writable_hook))
    flags |= DSK_EVENT_WRITABLE;
  if (stream->base_instance.source != NULL
   && dsk_hook_is_trapped (&stream->base_instance.source->readable_hook))
    flags |= DSK_EVENT_READABLE;
  dsk_main_watch_fd (stream->fd, flags, handle_fd_ready, stream);
}
static void
sink_set_poll (DskOctetStreamFdSink *sink, dsk_boolean is_trapped)
{
  DSK_UNUSED (is_trapped);
  stream_set_poll (DSK_OCTET_STREAM_FD (sink->base_instance.stream));
}
static void
source_set_poll (DskOctetStreamFdSource *source, dsk_boolean is_trapped)
{
  DSK_UNUSED (is_trapped);
  stream_set_poll (DSK_OCTET_STREAM_FD (source->base_instance.stream));
}

static DskHookFuncs sink_pollable_funcs =
{
  (DskHookObjectFunc) dsk_object_ref_f,
  (DskHookObjectFunc) dsk_object_unref_f,
  (DskHookSetPoll) sink_set_poll
};
static DskHookFuncs source_pollable_funcs =
{
  (DskHookObjectFunc) dsk_object_ref_f,
  (DskHookObjectFunc) dsk_object_unref_f,
  (DskHookSetPoll) source_set_poll
};

dsk_boolean
dsk_octet_stream_new_fd (DskFileDescriptor fd,
                         DskFileDescriptorStreamFlags flags,
                         DskOctetStreamFd **stream_out,
                         DskOctetStreamFdSource **source_out,
                         DskOctetStreamFdSink **sink_out,
                         DskError        **error)
{
  int getfl_flags;
  struct stat stat_buf;
  DskOctetStreamFd *rv;
  DskOctetStreamFdSource *source = NULL;
  DskOctetStreamFdSink *sink = NULL;
  dsk_boolean is_pollable, is_readable, is_writable;
  if (fstat (fd, &stat_buf) < 0)
    {
      dsk_set_error (error, "error calling fstat on file-descriptor %u",
                     (unsigned) fd);
      return DSK_FALSE;
    }
  getfl_flags = fcntl (fd, F_GETFL);
  is_pollable = S_ISFIFO (stat_buf.st_mode)
             || S_ISSOCK (stat_buf.st_mode)
             || S_ISCHR (stat_buf.st_mode)
             || isatty (fd);
  is_readable = (getfl_flags & O_ACCMODE) == O_RDONLY
             || (getfl_flags & O_ACCMODE) == O_RDWR;
  is_writable = (getfl_flags & O_ACCMODE) == O_WRONLY
             || (getfl_flags & O_ACCMODE) == O_RDWR;
  if ((flags & DSK_FILE_DESCRIPTOR_IS_NOT_READABLE) != 0)
    {
      dsk_assert ((flags & DSK_FILE_DESCRIPTOR_IS_READABLE) == 0);
#if 0                   /* maybe? */
      if (is_readable)
        shutdown (fd, SHUT_RD);
#endif
      is_readable = DSK_FALSE;
    }
  else
    {
      if ((flags & DSK_FILE_DESCRIPTOR_IS_READABLE) != 0
        && !is_readable)
        {
          dsk_set_error (error, "file-descriptor %u is not readable",
                         (unsigned) fd);
          return DSK_FALSE;
        }
    }
  if ((flags & DSK_FILE_DESCRIPTOR_IS_NOT_WRITABLE) != 0)
    {
      dsk_assert ((flags & DSK_FILE_DESCRIPTOR_IS_WRITABLE) == 0);
#if 0                   /* maybe? */
      if (is_writable)
        shutdown (fd, SHUT_WR);
#endif
      is_writable = DSK_FALSE;
    }
  else
    {
      if ((flags & DSK_FILE_DESCRIPTOR_IS_WRITABLE) != 0
        && !is_writable)
        {
          dsk_set_error (error, "file-descriptor %u is not writable",
                         (unsigned) fd);
          return DSK_FALSE;
        }
    }
  if (flags & DSK_FILE_DESCRIPTOR_IS_POLLABLE)
    is_pollable = DSK_TRUE;
  else if (flags & DSK_FILE_DESCRIPTOR_IS_NOT_POLLABLE)
    is_pollable = DSK_FALSE;

  rv = dsk_object_new (&dsk_octet_stream_fd_class);
  rv->fd = fd;
  if (is_pollable)
    rv->is_pollable = 1;
  if (flags & DSK_FILE_DESCRIPTOR_DO_NOT_CLOSE)
    rv->do_not_close = 1;
  if (is_readable)
    {
      source = dsk_object_new (&dsk_octet_stream_fd_source_class);
      rv->base_instance.source = DSK_OCTET_SOURCE (source);
      source->base_instance.stream = DSK_OCTET_STREAM (rv);
      dsk_object_ref (rv);
    }
  if (is_writable)
    {
      sink = dsk_object_new (&dsk_octet_stream_fd_sink_class);
      rv->base_instance.sink = DSK_OCTET_SINK (sink);
      sink->base_instance.stream = DSK_OCTET_STREAM (rv);
      dsk_object_ref (rv);
    }
  if (is_pollable)
    {
      if (sink != NULL)
        dsk_hook_set_funcs (&sink->base_instance.writable_hook,
                            &sink_pollable_funcs);
      if (source != NULL)
        dsk_hook_set_funcs (&source->base_instance.readable_hook,
                            &source_pollable_funcs);
    }
  else
    {
      if (sink != NULL)
        dsk_hook_set_idle_notify (&sink->base_instance.writable_hook,
                                  DSK_TRUE);
      if (source != NULL)
        dsk_hook_set_idle_notify (&source->base_instance.readable_hook,
                                  DSK_TRUE);
    }

  if (source)
    {
      if (source_out)
        *source_out = source;
      else
        dsk_object_unref (source);
    }
  else if (source_out != NULL)
    *source_out = NULL;

  if (sink)
    {
      if (sink_out)
        *sink_out = sink;
      else
        dsk_object_unref (sink);
    }
  else if (sink_out != NULL)
    *sink_out = NULL;

  if (stream_out != NULL)
    *stream_out = rv;
  else
    dsk_object_unref (rv);

  return DSK_TRUE;
}

static DskIOResult
dsk_octet_stream_fd_sink_write  (DskOctetSink   *sink,
                                 unsigned        max_len,
                                 const void     *data_out,
                                 unsigned       *n_written_out,
                                 DskError      **error)
{
  DskOctetStreamFd *stream = DSK_OCTET_STREAM_FD (sink->stream);
  int rv;
  if (stream->fd < 0)
    {
      dsk_set_error (error, "writing to defunct file-descriptor");
      return DSK_IO_RESULT_ERROR;
    }
  if (max_len == 0)
    {
      *n_written_out = 0;
      return DSK_IO_RESULT_SUCCESS;
    }
  rv = write (stream->fd, data_out, max_len);
  if (rv < 0)
    {
      if (errno == EINTR || errno == EAGAIN)
        return DSK_IO_RESULT_AGAIN;
      dsk_set_error (error, "error writing file-descriptor %u: %s",
                     stream->fd, strerror (errno));
      return DSK_IO_RESULT_ERROR;
    }
  *n_written_out = rv;
  return DSK_IO_RESULT_SUCCESS;
}

static DskIOResult
dsk_octet_stream_fd_sink_write_buffer (DskOctetSink   *sink,
                                       DskBuffer      *write_buffer,
                                       DskError      **error)
{
  int rv;
  DskOctetStreamFd *stream = DSK_OCTET_STREAM_FD (sink->stream);
  if (stream == NULL)
    {
      dsk_set_error (error, "write to dead stream");
      return DSK_IO_RESULT_ERROR;
    }
  rv = dsk_buffer_writev (write_buffer, stream->fd);
  if (rv < 0)
    {
      if (errno == EINTR || errno == EAGAIN)
        return DSK_IO_RESULT_AGAIN;
      dsk_set_error (error, "error writing data to fd %u: %s",
                     stream->fd, strerror (errno));
      return DSK_IO_RESULT_ERROR;
    }
  return DSK_IO_RESULT_SUCCESS;
}


static void       
dsk_octet_stream_fd_sink_shutdown     (DskOctetSink   *sink)
{
  DskOctetStreamFd *stream = DSK_OCTET_STREAM_FD (sink->stream);
  if (stream->fd < 0)
    return;
  shutdown (stream->fd, SHUT_WR);
  dsk_hook_clear (&sink->writable_hook);
}


DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskOctetStreamFdSink);
const DskOctetStreamFdSinkClass dsk_octet_stream_fd_sink_class =
{
  {
    DSK_OBJECT_CLASS_DEFINE(DskOctetStreamFdSink,
                            &dsk_octet_sink_class,
                            NULL,
                            NULL),
    dsk_octet_stream_fd_sink_write,
    dsk_octet_stream_fd_sink_write_buffer,
    dsk_octet_stream_fd_sink_shutdown
  }
};

static DskIOResult
dsk_octet_stream_fd_source_read (DskOctetSource *source,
                               unsigned        max_len,
                               void           *data_out,
                               unsigned       *bytes_read_out,
                               DskError      **error)
{
  int n_read;
  DskOctetStreamFd *stream = DSK_OCTET_STREAM_FD (source->stream);
  if (stream->fd < 0)
    {
      dsk_set_error (error, "no file-descriptor");
      return DSK_IO_RESULT_ERROR;
    }
  if (max_len == 0)
    {
      *bytes_read_out = 0;
      return DSK_IO_RESULT_SUCCESS;
    }
  n_read = read (stream->fd, data_out, max_len);
  if (n_read < 0)
    {
      if (errno == EINTR || errno == EAGAIN)
        return DSK_IO_RESULT_AGAIN;
      dsk_set_error (error, "error reading from client stream (fd %d): %s",
                     stream->fd, strerror (errno));
      return DSK_IO_RESULT_ERROR;
    }
  if (n_read == 0)
    {
      dsk_hook_clear (&source->readable_hook);
      return DSK_IO_RESULT_EOF;
    }
  *bytes_read_out = n_read;
  return DSK_IO_RESULT_SUCCESS;
}

static DskIOResult
dsk_octet_stream_fd_source_read_buffer  (DskOctetSource *source,
                                       DskBuffer      *read_buffer,
                                       DskError      **error)
{
  int rv;
  DskOctetStreamFd *stream = DSK_OCTET_STREAM_FD (source->stream);
  if (stream == NULL)
    {
      dsk_set_error (error, "read from dead stream");
      return DSK_IO_RESULT_ERROR;
    }
  if (stream->fd < 0)
    {
      dsk_set_error (error, "read from stream with no file-descriptor");
      return DSK_IO_RESULT_ERROR;
    }
  rv = dsk_buffer_readv (read_buffer, stream->fd);
  if (rv < 0)
    {
      if (errno == EINTR || errno == EAGAIN)
        return DSK_IO_RESULT_AGAIN;
      dsk_set_error (error, "error reading data from fd %u: %s",
                     stream->fd, strerror (errno));
      return DSK_IO_RESULT_ERROR;
    }
  if (rv == 0)
    {
      dsk_hook_clear (&source->readable_hook);
      return DSK_IO_RESULT_EOF;
    }
  return DSK_IO_RESULT_SUCCESS;
}

static void
dsk_octet_stream_fd_source_shutdown (DskOctetSource *source)
{
  DskOctetStreamFd *stream = DSK_OCTET_STREAM_FD (source->stream);
  if (stream == NULL)
    return;
  shutdown (stream->fd, SHUT_RD);

  dsk_hook_clear (&source->readable_hook);
}


DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskOctetStreamFdSource);
const DskOctetStreamFdSourceClass dsk_octet_stream_fd_source_class =
{
  { DSK_OBJECT_CLASS_DEFINE(DskOctetStreamFdSource,
                            &dsk_octet_source_class,
                            NULL,
                            NULL),
    dsk_octet_stream_fd_source_read,
    dsk_octet_stream_fd_source_read_buffer,
    dsk_octet_stream_fd_source_shutdown
  }
};

static void
dsk_octet_stream_fd_init (DskOctetStreamFd *stream)
{
  stream->fd = -1;
}

static void
dsk_octet_stream_fd_finalize (DskOctetStreamFd *stream)
{
  if (stream->fd >= 0 && !stream->do_not_close)
    {
      DskFileDescriptor fd = stream->fd;
      stream->fd = -1;
      close (fd);
    }
}

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskOctetStreamFd);
const DskOctetStreamFdClass dsk_octet_stream_fd_class =
{
  {
    DSK_OBJECT_CLASS_DEFINE(DskOctetStreamFd,
                            &dsk_octet_stream_class,
                            dsk_octet_stream_fd_init,
                            dsk_octet_stream_fd_finalize)
  }
};

