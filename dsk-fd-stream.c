#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#include "dsk.h"

DskStream *dsk_stream_new_stdin (void)
{
  static dsk_boolean has_returned = DSK_FALSE;
  DskFdStream *stream;
  DskError *error = NULL;
  if (has_returned)
    return NULL;
  has_returned = DSK_TRUE;
  stream = dsk_fd_stream_new (STDIN_FILENO,
                              DSK_FILE_DESCRIPTOR_IS_NOT_WRITABLE
                              |DSK_FILE_DESCRIPTOR_IS_READABLE
                              |DSK_FILE_DESCRIPTOR_DO_NOT_CLOSE,
                              &error);
  if (stream == NULL)
    dsk_error ("standard-input not available: %s", error->message);
  return DSK_STREAM (stream);
}

DskStream *dsk_stream_new_stdout (void)
{
  static dsk_boolean has_returned = DSK_FALSE;
  DskError *error = NULL;
  DskFdStream *stream;
  if (has_returned)
    return NULL;
  has_returned = DSK_TRUE;
  stream = dsk_fd_stream_new (STDOUT_FILENO,
                              DSK_FILE_DESCRIPTOR_IS_NOT_READABLE
                              |DSK_FILE_DESCRIPTOR_IS_WRITABLE
                              |DSK_FILE_DESCRIPTOR_DO_NOT_CLOSE,
                              &error);
  if (stream == NULL)
    dsk_error ("standard-input not available: %s", error->message);
  return DSK_STREAM (stream);
}

static void
handle_fd_ready (DskFileDescriptor   fd,
                 unsigned       events,
                 void          *callback_data)
{
  DskFdStream *stream = DSK_FD_STREAM (callback_data);
  dsk_assert (stream->fd == fd);
  dsk_object_ref (stream);
  if ((events & DSK_EVENT_READABLE) != 0)
    dsk_hook_notify (&stream->base_instance.readable_hook);
  if ((events & DSK_EVENT_WRITABLE) != 0)
    dsk_hook_notify (&stream->base_instance.writable_hook);
  dsk_object_unref (stream);
}

static void
stream_set_poll (DskFdStream *stream, bool is_trapped)
{
  unsigned flags = 0;
  DSK_UNUSED (is_trapped);
  if (dsk_hook_is_trapped (&stream->base_instance.writable_hook))
    flags |= DSK_EVENT_WRITABLE;
  if (dsk_hook_is_trapped (&stream->base_instance.readable_hook))
    flags |= DSK_EVENT_READABLE;
  dsk_main_watch_fd (stream->fd, flags, handle_fd_ready, stream);
}

static DskHookFuncs pollable_funcs =
{
  (DskHookObjectFunc) dsk_object_ref_f,
  (DskHookObjectFunc) dsk_object_unref_f,
  (DskHookSetPoll) stream_set_poll
};

DskFdStream *
dsk_fd_stream_new       (DskFileDescriptor fd,
                         DskFileDescriptorStreamFlags flags,
                         DskError        **error)
{
  int getfl_flags;
  struct stat stat_buf;
  DskFdStream *rv;
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

  rv = dsk_object_new (&dsk_fd_stream_class);
  rv->fd = fd;
  if (is_pollable)
    rv->is_pollable = 1;
  if (flags & DSK_FILE_DESCRIPTOR_DO_NOT_CLOSE)
    rv->do_not_close = 1;
  if (is_readable)
    dsk_hook_set_funcs (&rv->base_instance.readable_hook,
                        &pollable_funcs);
  else
    dsk_hook_clear (&rv->base_instance.readable_hook);
  if (is_writable)
    dsk_hook_set_funcs (&rv->base_instance.writable_hook,
                        &pollable_funcs);
  else
    dsk_hook_clear (&rv->base_instance.writable_hook);
  if (!is_pollable)
    {
      if (is_writable)
        dsk_hook_set_idle_notify (&rv->base_instance.writable_hook,
                                  DSK_TRUE);
      if (is_readable)
        dsk_hook_set_idle_notify (&rv->base_instance.readable_hook,
                                  DSK_TRUE);
    }

  return rv;
}

static DskIOResult
dsk_fd_stream_write             (DskStream      *s,
                                 unsigned        max_len,
                                 const void     *data_out,
                                 unsigned       *n_written_out,
                                 DskError      **error)
{
  DskFdStream *stream = DSK_FD_STREAM (s);
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
dsk_fd_stream_write_buffer (DskStream *s,
                                       DskBuffer      *write_buffer,
                                       DskError      **error)
{
  int rv;
  DskFdStream *stream = DSK_FD_STREAM (s);
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
dsk_fd_stream_shutdown_write     (DskStream *s)
{
  DskFdStream *stream = DSK_FD_STREAM (s);
  if (stream->fd < 0)
    return;
  shutdown (stream->fd, SHUT_WR);
  dsk_hook_clear (&s->writable_hook);
}

static DskIOResult
dsk_fd_stream_read (DskStream *s,
                               unsigned        max_len,
                               void           *data_out,
                               unsigned       *bytes_read_out,
                               DskError      **error)
{
  int n_read;
  DskFdStream *stream = DSK_FD_STREAM (s);
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
      dsk_hook_clear (&s->readable_hook);
      return DSK_IO_RESULT_EOF;
    }
  *bytes_read_out = n_read;
  return DSK_IO_RESULT_SUCCESS;
}

static DskIOResult
dsk_fd_stream_read_buffer  (DskStream *s,
                                       DskBuffer      *read_buffer,
                                       DskError      **error)
{
  int rv;
  DskFdStream *stream = DSK_FD_STREAM (s);
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
      dsk_hook_clear (&s->readable_hook);
      return DSK_IO_RESULT_EOF;
    }
  return DSK_IO_RESULT_SUCCESS;
}

static void
dsk_fd_stream_shutdown_read (DskStream *s)
{
  DskFdStream *stream = DSK_FD_STREAM (s);
  shutdown (stream->fd, SHUT_RD);
  dsk_hook_clear (&s->readable_hook);
}

static void
dsk_fd_stream_init (DskFdStream *stream)
{
  stream->fd = -1;
}

static void
dsk_fd_stream_finalize (DskFdStream *stream)
{
  if (stream->fd >= 0 && !stream->do_not_close)
    {
      DskFileDescriptor fd = stream->fd;
      stream->fd = -1;
      close (fd);
    }
}

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskFdStream);
const DskFdStreamClass dsk_fd_stream_class =
{
  {
    DSK_OBJECT_CLASS_DEFINE(DskFdStream,
                            &dsk_stream_class,
                            dsk_fd_stream_init,
                            dsk_fd_stream_finalize),
    dsk_fd_stream_read,
    dsk_fd_stream_read_buffer,
    dsk_fd_stream_shutdown_read,
    dsk_fd_stream_write,
    dsk_fd_stream_write_buffer,
    dsk_fd_stream_shutdown_write,
  }
};

