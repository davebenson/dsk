#define _ATFILE_SOURCE
#include <string.h>
#include <fcntl.h>
#include <alloca.h>
#include <unistd.h>
#include "dsk.h"

struct _DskLogger
{
  char *fmt;
  unsigned max_length;  /* max length of fmt's rendered form + openat_dir if needed */
  unsigned period;
  int tz_offset;
  DskDispatchTimer *timer;
  int fd;
  DskBuffer buffer;
  unsigned max_buffer;
  char *openat_dir;
  int openat_fd;

  unsigned next_rotate_time;
};

static dsk_boolean
logger_open_fd (DskLogger *logger,
                DskError **error)
{
  char *pad = alloca (logger->max_length);
  char *at = pad;
  DskDate date;
  int fd;

  if (logger->openat_dir)
    {
      strcpy (pad, logger->openat_dir);
      at = strchr (pad, 0);
    }

  dsk_unixtime_to_date (dsk_dispatch_default ()->last_dispatch_secs
                        + logger->tz_offset, &date);
  dsk_date_strftime (&date, logger->fmt, pad + logger->max_length - at, at);

  dsk_boolean did_mkdir = DSK_FALSE;

retry_open:
#if DSK_HAS_ATFILE_SUPPORT
  if (logger->openat_fd >= 0)
    fd = openat (logger->openat_fd, pad, O_CREAT|O_APPEND|O_WRONLY, 0666);
  else
#endif
    fd = open (pad, O_CREAT|O_APPEND|O_WRONLY, 0666);
  if (fd < 0)
    {
      if (errno == EINTR)
        goto retry_open;
      if (errno == ENOENT && !did_mkdir)
        {
          char *slash = strrchr (pad, '/');
          if (slash)
            {
              /* perhaps make the directory */
              *slash = 0;
#if DSK_HAS_ATFILE_SUPPORT
              if (logger->openat_fd >= 0)
                {
                  if (!dsk_mkdir_recursive_at (logger->openat_fd, pad,
                                               0777, error))
                    return DSK_FALSE;
                }
              else
#endif
                {
                  if (!dsk_mkdir_recursive (pad, 0777, error))
                    return DSK_FALSE;
                }
              *slash = '/';
              did_mkdir = DSK_TRUE;
              goto retry_open;
            }
        }
      dsk_set_error (error, "error opening/creating file %s: %s",
                     pad, strerror (errno));
      return DSK_FALSE;
    }

  if (logger->fd >= 0)
    close (logger->fd);
  logger->fd = fd;
  return DSK_TRUE;
}


DskLogger *dsk_logger_new         (DskLoggerOptions *options,
                                   DskError        **error)
{
  DskLogger *logger = DSK_NEW (DskLogger);
  logger->fmt = dsk_strdup (options->format);
#if DSK_HAS_ATFILE_SUPPORT
  logger->openat_fd = options->openat_fd;
  logger->openat_dir = NULL;
#else
  logger->openat_fd = -1;
  logger->openat_dir = dsk_strdup (options->openat_dir);
#endif
  logger->tz_offset = options->tz_offset;
  logger->max_buffer = options->max_buffer;
  logger->period = options->period;
  dsk_buffer_init (&logger->buffer);
  logger->fd = -1;

  if (!logger_open_fd (logger, error))
    {
      dsk_free (logger->fmt);
      dsk_free (logger->openat_dir);
      dsk_free (logger);
      return NULL;
    }

  logger->next_rotate_time = (dsk_dispatch_default()->last_dispatch_secs
                            + logger->period - 1)
                           / logger->period
                           * logger->period;

  return logger;
}

static void
flush_buffer (DskLogger *logger)
{
  while (logger->buffer.size > 0)
    {
      dsk_buffer_writev (&logger->buffer, logger->fd);
    }
}

DskBuffer *
dsk_logger_peek_buffer (DskLogger        *logger)
{
  if (dsk_dispatch_default()->last_dispatch_secs >= logger->next_rotate_time)
    {
      DskError *error = NULL;
      flush_buffer (logger);
      if (!logger_open_fd (logger, &error))
        {
          dsk_die ("error opening logger file: %s", error->message);
        }

      /* TODO: handle very long jumps in time differently. */
      do
        logger->next_rotate_time += logger->period;
      while (logger->next_rotate_time < dsk_dispatch_default()->last_dispatch_secs);
    }
  return &logger->buffer;
}

void
dsk_logger_done_buffer (DskLogger        *logger)
{
  if (logger->buffer.size > logger->max_buffer)
    flush_buffer (logger);
}

void
dsk_logger_destroy     (DskLogger        *logger)
{
  flush_buffer (logger);
  if (logger->fd >= 0)
    close (logger->fd);
  dsk_free (logger->fmt);
  dsk_free (logger->openat_dir);
  dsk_free (logger);
}
