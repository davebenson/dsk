#include <string.h>
#include <fcntl.h>
#include <alloca.h>
#include <unistd.h>
#include "dsk.h"

struct _DskLogger
{
  char *fmt;
  unsigned max_length;  /* max length of fmt's rendered form */
  unsigned period;
  int tz_offset;
  DskDispatchTimer *timer;
  int fd;
  DskBuffer buffer;
  unsigned max_buffer;
  DskDir *dir;

  unsigned next_rotate_time;
};

static dsk_boolean
logger_open_fd (DskLogger *logger,
                DskError **error)
{
  char *pad = alloca (logger->max_length);
  DskDate date;
  dsk_unixtime_to_date (dsk_dispatch_default ()->last_dispatch_secs
                        + logger->tz_offset, &date);
  dsk_date_strftime (&date, logger->fmt, logger->max_length, pad);


  int fd = dsk_dir_openfd (logger->dir, pad, DSK_DIR_OPENFD_APPEND, 0666, error);
  if (fd < 0)
    {
      dsk_add_error_prefix (error, "logger");
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
  logger->dir = dsk_dir_ref (options->dir);
  logger->tz_offset = options->tz_offset;
  logger->max_buffer = options->max_buffer;
  logger->period = options->period;
  logger->max_length = dsk_strftime_max_length (options->format) + 1;
 
  dsk_buffer_init (&logger->buffer);
  logger->fd = -1;

  if (!logger_open_fd (logger, error))
    {
      dsk_free (logger->fmt);
      dsk_dir_unref (logger->dir);
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
  dsk_dir_unref (logger->dir);
  dsk_free (logger);
}
