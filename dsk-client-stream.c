#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <sys/socket.h>
#include "dsk.h"

/* Begin connecting to the given resolved address. */
static void begin_connecting           (DskClientStream *stream);

static inline void
ping_idle_disconnect_timer (DskClientStream *stream)
{
  if (stream->idle_disconnect_time_ms >= 0
   && stream->is_connected)
    dsk_dispatch_adjust_timer_millis (stream->idle_disconnect_timer,
                                      stream->idle_disconnect_time_ms);
}



static dsk_boolean ip_address_is_default (DskIpAddress *address)
{
  unsigned i;
  if (address->type != 0)
    return DSK_FALSE;
  for (i = 0; i < 16; i++)
    if (address->address[i])
      return DSK_FALSE;
  return DSK_TRUE;
}

dsk_boolean
dsk_client_stream_new       (DskClientStreamOptions *options,
                             DskClientStream **stream_out,
                             DskOctetSink    **sink_out,
                             DskOctetSource  **source_out,
                             DskError        **error)
{
  DskClientStream *rv;
  dsk_boolean has_address = !ip_address_is_default (&options->address);

  /* check trivial usage considerations */
  dsk_warn_if_fail (!(options->hostname != NULL && has_address),
                    "ignoring ip-address because symbolic name given");
  if (options->hostname != NULL || has_address)
    {
      if (options->port == 0)
        {
          dsk_set_error (error,
                         "port must be non-zero for client (hostname is '%s')",
                         options->hostname);
          return DSK_FALSE;
        }
      dsk_warn_if_fail (options->path == NULL,
                        "cannot decide between tcp and local client");
    }

  rv = dsk_object_new (&dsk_client_stream_class);
  rv->base_instance.sink = dsk_object_new (&dsk_client_stream_sink_class);
  rv->base_instance.source = dsk_object_new (&dsk_client_stream_source_class);
  rv->base_instance.sink->stream = dsk_object_ref (rv);
  rv->base_instance.source->stream = dsk_object_ref (rv);
  rv->reconnect_time_ms = -1;
  rv->idle_disconnect_time_ms = -1;
  rv->fd = -1;

  if (options->hostname != NULL)
    {
      if (dsk_hostname_looks_numeric (options->hostname))
        rv->is_numeric_name = 1;
      rv->name = dsk_strdup (options->hostname);
      rv->port = options->port;
    }
  else if (has_address)
    {
      rv->is_numeric_name = 1;
      rv->name = dsk_ip_address_to_string (&options->address);
      rv->port = options->port;
    }
  else if (options->path != NULL)
    {
      rv->is_local_socket = 1;
      rv->name = dsk_strdup (options->path);
    }
  rv->idle_disconnect_time_ms = options->idle_disconnect_time;
  rv->reconnect_time_ms = options->reconnect_time;
  begin_connecting (rv);
  if (options->idle_disconnect_time >= 0)
    dsk_client_stream_set_max_idle_time (rv, options->idle_disconnect_time);
  if (options->reconnect_time >= 0)
    dsk_client_stream_set_reconnect_time (rv, options->reconnect_time);
  if (source_out)
    *source_out = rv->base_instance.source;
  else if (rv->base_instance.source)
    dsk_object_unref (rv->base_instance.source);
  if (sink_out)
    *sink_out = rv->base_instance.sink;
  else if (rv->base_instance.sink)
    dsk_object_unref (rv->base_instance.sink);
  if (stream_out)
    *stream_out = rv;
  else
    dsk_object_unref (rv);
  return DSK_TRUE;
}

static void
handle_reconnect_timer_expired (void *data)
{
  DskClientStream *stream = data;
  stream->reconnect_timer = NULL;
  begin_connecting (stream);
}

/* numeric hostnames */
/* use -1 to disable these timeouts */
void
dsk_client_stream_set_reconnect_time (DskClientStream *client,
                                      int              millis)
{
  /* short-circuit no-op cases */
  if (millis < 0)
    {
      if (client->reconnect_time_ms == -1)
        return;
      millis = -1;
    }
  else if (client->reconnect_time_ms == millis)
    return;

  /* if we have a valid file-descriptor or we are resolving the name,
     there then the reconnect_time_ms is not currently relevant:
     set it and go */
  if (client->fd != -1 || client->is_resolving_name)
    {
      client->reconnect_time_ms = millis;
      return;
    }

  if (millis == -1)
    {
      /* handle timer removal */
      if (client->reconnect_timer)
        {
          dsk_dispatch_remove_timer (client->reconnect_timer);
          client->reconnect_timer = NULL;
        }
      else
        dsk_warn_if_reached ("no reconnect timer?");
    }
  else
    {
      if (client->reconnect_time_ms >= 0)
        dsk_dispatch_remove_timer (client->reconnect_timer);
      else
        dsk_assert (client->reconnect_timer == NULL);
      client->reconnect_time_ms = millis;

      /* TODO: subtract elapsed time from last disconnect / failed to connect */

      client->reconnect_timer
        = dsk_dispatch_add_timer_millis (dsk_dispatch_default (),
                                         millis,
                                         handle_reconnect_timer_expired,
                                         client);
    }
}

static void
handle_idle_too_long (void *data)
{
  DskClientStream *stream = data;
  dsk_client_stream_disconnect (stream);
}

void
dsk_client_stream_set_max_idle_time  (DskClientStream *client,
                                      int              millis)
{
  if (millis < 0)
    millis = -1;
  if (millis == client->idle_disconnect_time_ms)
    return;
  if (client->idle_disconnect_timer != NULL)
    {
      dsk_dispatch_remove_timer (client->idle_disconnect_timer);
      client->idle_disconnect_timer = NULL;
    }
  if (millis >= 0
   && client->is_connected)
    client->idle_disconnect_timer = dsk_dispatch_add_timer_millis (dsk_dispatch_default (),
                                                                   millis,
                                                                   handle_idle_too_long,
                                                                   client);
  client->idle_disconnect_time_ms = millis;
}

/* --- connecting & dns lookups --- */
int
dsk_errno_from_fd (int fd)
{
  socklen_t size_int = sizeof (int);
  int value = EINVAL;
  if (getsockopt (fd, SOL_SOCKET, SO_ERROR, &value, &size_int) < 0)
    {
      /* Note: this behavior is vaguely hypothetically broken,
       *       in terms of ignoring getsockopt's error;
       *       however, this shouldn't happen, and EINVAL is ok if it does.
       *       Furthermore some broken OS's return an error code when
       *       fetching SO_ERROR!
       */
      return value;
    }

  return value;
}
static void
handle_fd_events (DskFileDescriptor   fd,
                  unsigned            events,
                  void               *callback_data)
{
  DskClientStream *stream = callback_data;
  DSK_UNUSED (fd);
  if ((events & DSK_EVENT_READABLE) != 0
   && stream->base_instance.source != NULL)
    dsk_hook_notify (&stream->base_instance.source->readable_hook);
  if ((events & DSK_EVENT_WRITABLE) != 0
   && stream->base_instance.sink != NULL)
    dsk_hook_notify (&stream->base_instance.sink->writable_hook);
}
static void
stream_do_watch_fd (DskClientStream *stream)
{
  if (stream->is_connected)
    {
      int events = 0;
      if (stream->base_instance.source != NULL
       && dsk_hook_is_trapped (&stream->base_instance.source->readable_hook))
        events |= DSK_EVENT_READABLE;
      if (stream->base_instance.sink != NULL
       && dsk_hook_is_trapped (&stream->base_instance.sink->writable_hook))
        events |= DSK_EVENT_WRITABLE;
      dsk_dispatch_watch_fd (dsk_dispatch_default (), stream->fd, events,
                             handle_fd_events, stream);
    }
}


static void
handle_fd_connected (DskClientStream *stream)
{
  stream_do_watch_fd (stream);
  dsk_assert (stream->idle_disconnect_timer == NULL);
  if (stream->idle_disconnect_time_ms >= 0)
    stream->idle_disconnect_timer = dsk_dispatch_add_timer_millis (dsk_dispatch_default (),
                                                                   stream->idle_disconnect_time_ms,
                                                                   handle_idle_too_long,
                                                                   stream);
}

static void
maybe_set_autoreconnect_timer (DskClientStream *stream)
{
  dsk_assert (stream->reconnect_timer == NULL);
  if (stream->reconnect_time_ms >= 0)
    stream->reconnect_timer
      = dsk_dispatch_add_timer_millis (dsk_dispatch_default (),
                                       stream->reconnect_time_ms,
                                       handle_reconnect_timer_expired,
                                       stream);
}

static void
handle_fd_connecting (DskFileDescriptor   fd,
                      unsigned            events,
                      void               *callback_data)
{
  int err = dsk_errno_from_fd (fd);
  DskClientStream *stream = callback_data;
  DSK_UNUSED (events);
  if (err == 0)
    {
      stream->is_connecting = DSK_FALSE;
      stream->is_connected = DSK_TRUE;
      handle_fd_connected (stream);             /* sets the watch on the fd */
      return;
    }

  if (err != EINTR && err != EAGAIN)
    {
      dsk_octet_stream_set_last_error (&stream->base_instance,
                                       "error finishing connection to %s: %s",
                                       stream->name, strerror (err));
      dsk_dispatch_close_fd (dsk_dispatch_default (), stream->fd);
      stream->fd = -1;
      stream->is_connecting = DSK_FALSE;
      maybe_set_autoreconnect_timer (stream);
      return;
    }

  /* wait for another notification */
  return;
}

static void
begin_connecting_sockaddr (DskClientStream *stream,
                           unsigned          addr_len,
                           struct sockaddr   *addr)
{
  int fd;
  dsk_assert (stream->fd == -1);
  dsk_assert (!stream->is_connecting);
  dsk_assert (!stream->is_connected);
retry_sys_socket:
  fd = socket (addr->sa_family, SOCK_STREAM, 0);
  if (fd < 0)
    {
      int e = errno;
      if (e == EINTR)
        goto retry_sys_socket;
      if (dsk_fd_creation_failed (e))
        goto retry_sys_socket;

      /* set error */
      dsk_octet_stream_set_last_error (&stream->base_instance,
                                       "error invoking socket(2) system-call: %s",
                                       strerror (e));

      goto handle_error;
    }

  /* set non-blocking */
  dsk_fd_set_nonblocking (fd);
  dsk_fd_set_close_on_exec (fd);

  /* call connect() */
retry_sys_connect:
  if (connect (fd, addr, addr_len) < 0)
    {
      int e = errno;
      if (e == EINTR)
        goto retry_sys_connect;
      
      if (e == EAGAIN || e == EINPROGRESS)
        {
          stream->is_connecting = DSK_TRUE;
          stream->fd = fd;
          dsk_dispatch_watch_fd (dsk_dispatch_default (), fd,
                                 DSK_EVENT_WRITABLE|DSK_EVENT_READABLE,
                                 handle_fd_connecting, stream);
          return;
        }

      /* set error */
      close (fd);
      dsk_octet_stream_set_last_error (&stream->base_instance,
                                       "error connecting to %s: %s",
                                       stream->name, strerror (e));
      goto handle_error;
    }

  stream->is_connected = DSK_TRUE;
  stream->fd = fd;
  handle_fd_connected (stream);
  return;

handle_error:
  maybe_set_autoreconnect_timer (stream);
  return;
}
#if 0
static void
begin_connecting_dns_entry (DskClientStream *stream,
                            DskIpAddress   *address,
                            unsigned         port)
{
  unsigned addr_len;
  struct sockaddr_storage addr;
  stream->is_connecting = 1;
  dsk_ip_address_to_sockaddr (address, port, &addr_len, &addr);
  begin_connecting_sockaddr (stream, addr_len, (struct sockaddr *) &addr);
}
#endif
static void
handle_dns_done (DskDnsLookupResult *result,
                 void               *callback_data)
{
  DskClientStream *stream = callback_data;

  stream->is_resolving_name = 0;
  switch (result->type)
    {
    case DSK_DNS_LOOKUP_RESULT_FOUND:
      {
        struct sockaddr_storage addr;
        unsigned addr_len;
        dsk_ip_address_to_sockaddr (result->addr, stream->port, &addr, &addr_len);
        begin_connecting_sockaddr (stream, addr_len, (struct sockaddr *) &addr);
      }
      break;
    case DSK_DNS_LOOKUP_RESULT_NOT_FOUND:
      dsk_octet_stream_set_last_error (&stream->base_instance,
                             "dns entry for %s not found",
                             stream->name);
      maybe_set_autoreconnect_timer (stream);
      break;
    case DSK_DNS_LOOKUP_RESULT_TIMEOUT:
      dsk_octet_stream_set_last_error (&stream->base_instance,
                                       "dns lookup for %s timed out",
                                       stream->name);
      maybe_set_autoreconnect_timer (stream);
      break;
    case DSK_DNS_LOOKUP_RESULT_BAD_RESPONSE:
      dsk_octet_stream_set_last_error (&stream->base_instance,
                                       "dns lookup for %s failed: %s",
                                       stream->name, result->message);
      maybe_set_autoreconnect_timer (stream);
      break;
    default:
      dsk_assert_not_reached ();
    }

  dsk_object_unref (stream);
}

static void
begin_connecting (DskClientStream *stream)
{
  if (stream->is_local_socket)
    {
      struct sockaddr_un addr;
      unsigned length = strlen (stream->name);
      if (length > sizeof (addr.sun_path))
        {
          /* name too long */
          /* TODO: catch this in constructor */
          dsk_octet_stream_set_last_error (&stream->base_instance,
                                           "name too long for local socket");

          return;
        }
      addr.sun_family = AF_LOCAL;
      memcpy (addr.sun_path, stream->name,
              length == sizeof (addr.sun_path) ? length : length + 1);
      begin_connecting_sockaddr (stream, sizeof (addr), (struct sockaddr *) &addr);
    }
  else if (stream->is_numeric_name)
    {
      struct sockaddr_storage addr;
      unsigned addr_len;
      DskIpAddress address;

      /* parse name into addr/addr_len */
      if (!dsk_ip_address_parse_numeric (stream->name, &address))
        dsk_die ("dsk_ip_address_parse_numeric failed on %s", stream->name);
      dsk_ip_address_to_sockaddr (&address, stream->port, &addr, &addr_len);
      begin_connecting_sockaddr (stream, addr_len, (struct sockaddr *) &addr);
    }
  else
    {
      /* begin dns lookup */
      stream->is_resolving_name = 1;
      dsk_object_ref (stream);
      dsk_dns_lookup (stream->name,
                      DSK_FALSE,                /* is_ipv6? should be be needed */
                      handle_dns_done, stream);
    }
}

static void
dsk_client_stream_init (DskClientStream *stream)
{
  dsk_hook_init (&stream->disconnect_hook, stream);
  dsk_hook_init (&stream->connect_hook, stream);
}

static void
dsk_client_stream_finalize (DskClientStream *stream)
{
  dsk_hook_clear (&stream->disconnect_hook);
  dsk_hook_clear (&stream->connect_hook);
  if (stream->idle_disconnect_timer)
    dsk_dispatch_remove_timer (stream->idle_disconnect_timer);
  if (stream->reconnect_timer)
    dsk_dispatch_remove_timer (stream->reconnect_timer);
  if (stream->fd >= 0)
    {
      dsk_main_close_fd (stream->fd);
      stream->fd = -1;
    }
  dsk_free (stream->name);
}

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskClientStream);
DskClientStreamClass dsk_client_stream_class =
{
  { DSK_OBJECT_CLASS_DEFINE(DskClientStream, &dsk_octet_stream_class,
                            dsk_client_stream_init,
                            dsk_client_stream_finalize) }
};

void dsk_client_stream_disconnect (DskClientStream *stream)
{
  if (!stream->is_connected)
    return;

  /* close fd */
  dsk_main_close_fd (stream->fd);
  stream->is_connected = DSK_FALSE;
  stream->fd = -1;

  /* emit disconnect hook */
  dsk_hook_notify (&stream->disconnect_hook);

  /* maybe plan reconnect */
  maybe_set_autoreconnect_timer (stream);
}

/* === Implementation of octet-source class === */
static void
dsk_client_stream_source_set_poll (DskClientStreamSource *source)
{
  if (source->base_instance.stream != NULL)
    stream_do_watch_fd (DSK_CLIENT_STREAM (source->base_instance.stream));
}
static void
dsk_client_stream_source_init (DskClientStreamSource *source)
{
  static DskHookFuncs client_stream_source_funcs =
    {
      (DskHookObjectFunc) dsk_object_ref_f,
      (DskHookObjectFunc) dsk_object_unref_f,
      (DskHookSetPoll) dsk_client_stream_source_set_poll
    };
  dsk_hook_set_funcs (&source->base_instance.readable_hook,
                      &client_stream_source_funcs);
}
static DskIOResult
dsk_client_stream_source_read (DskOctetSource *source,
                               unsigned        max_len,
                               void           *data_out,
                               unsigned       *bytes_read_out,
                               DskError      **error)
{
  int n_read;
  DskClientStream *stream;
  if (source->stream == NULL)
    {
      dsk_set_error (error, "write to dead client stream");
      return DSK_IO_RESULT_ERROR;
    }
  stream = DSK_CLIENT_STREAM (source->stream);
  if (stream->fd < 0)
    {
      dsk_set_error (error, "no file-descriptor");
      return DSK_IO_RESULT_ERROR;
    }
  if (stream->is_connecting)
    {
      dsk_set_error (error, "file-descriptor %d not connected yet", stream->fd);
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
    return DSK_IO_RESULT_EOF;
  *bytes_read_out = n_read;
  ping_idle_disconnect_timer (stream);
  return DSK_IO_RESULT_SUCCESS;
}

static DskIOResult
dsk_client_stream_source_read_buffer  (DskOctetSource *source,
                                       DskBuffer      *read_buffer,
                                       DskError      **error)
{
  int rv;
  DskClientStream *stream;
  if (source->stream == NULL)
    {
      dsk_set_error (error, "read from dead stream");
      return DSK_IO_RESULT_ERROR;
    }
  stream = DSK_CLIENT_STREAM (source->stream);
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
    return DSK_IO_RESULT_EOF;
  ping_idle_disconnect_timer (stream);
  return DSK_IO_RESULT_SUCCESS;
}

static void
dsk_client_stream_source_shutdown (DskOctetSource *source)
{
  DskClientStream *stream;
  if (source->stream == NULL)
    return;
  stream = DSK_CLIENT_STREAM (source->stream);
  if (stream->fd >= 0)
    shutdown (stream->fd, SHUT_RD);

  stream->shutdown_read = 1;
  dsk_hook_clear (&source->readable_hook);

  stream_do_watch_fd (stream);
}


DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskClientStreamSource);
DskClientStreamSourceClass dsk_client_stream_source_class =
{
  { DSK_OBJECT_CLASS_DEFINE (DskClientStreamSource, &dsk_octet_source_class,
                             dsk_client_stream_source_init, NULL),
    dsk_client_stream_source_read,
    dsk_client_stream_source_read_buffer,
    dsk_client_stream_source_shutdown,
  }
};

/* === Implementation of octet-sink class === */
static void
dsk_client_stream_sink_set_poll (DskClientStreamSink *sink)
{
  if (sink->base_instance.stream!= NULL)
    stream_do_watch_fd (DSK_CLIENT_STREAM (sink->base_instance.stream));
}
static void
dsk_client_stream_sink_init (DskClientStreamSink *sink)
{
  static DskHookFuncs client_stream_sink_funcs =
    {
      (DskHookObjectFunc) dsk_object_ref_f,
      (DskHookObjectFunc) dsk_object_unref_f,
      (DskHookSetPoll) dsk_client_stream_sink_set_poll
    };
  dsk_hook_set_funcs (&sink->base_instance.writable_hook,
                      &client_stream_sink_funcs);
}
static DskIOResult
dsk_client_stream_sink_write  (DskOctetSink   *sink,
                               unsigned        max_len,
                               const void     *data_out,
                               unsigned       *n_written_out,
                               DskError      **error)
{
  int wrote;
  DskClientStream *stream;
  if (sink->stream == NULL)
    {
      dsk_set_error (error, "write to dead client stream");
      return DSK_IO_RESULT_ERROR;
    }
  stream = DSK_CLIENT_STREAM (sink->stream);
  if (stream->fd < 0)
    {
      dsk_set_error (error, "no file-descriptor");
      return DSK_IO_RESULT_ERROR;
    }
  if (stream->is_connecting)
    {
      dsk_set_error (error, "file-descriptor %d not connected yet", stream->fd);
      return DSK_IO_RESULT_ERROR;
    }
  if (max_len == 0)
    {
      *n_written_out = 0;
      return DSK_IO_RESULT_SUCCESS;
    }
  wrote = write (stream->fd, data_out, max_len);
  if (wrote < 0)
    {
      if (errno == EINTR || errno == EAGAIN)
        return DSK_IO_RESULT_AGAIN;
      dsk_set_error (error, "error writing to client stream (fd %d): %s",
                     stream->fd, strerror (errno));
      return DSK_IO_RESULT_ERROR;
    }
  *n_written_out = wrote;
  ping_idle_disconnect_timer (stream);
  return DSK_IO_RESULT_SUCCESS;
}

static DskIOResult
dsk_client_stream_sink_write_buffer  (DskOctetSink   *sink,
                                      DskBuffer      *write_buffer,
                                      DskError      **error)
{
  int rv;
  DskClientStream *stream;
  if (sink->stream == NULL)
    {
      dsk_set_error (error, "write to dead stream");
      return DSK_IO_RESULT_ERROR;
    }
  stream = DSK_CLIENT_STREAM (sink->stream);
  if (stream->fd < 0)
    {
      dsk_set_error (error, "write to stream with no file-descriptor");
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
  ping_idle_disconnect_timer (stream);
  return DSK_IO_RESULT_SUCCESS;
}

static void
dsk_client_stream_sink_shutdown   (DskOctetSink   *sink)
{
  DskClientStream *stream;
  if (sink->stream == NULL)
    return;
  stream = DSK_CLIENT_STREAM (sink->stream);
  if (stream->fd >= 0)
    shutdown (stream->fd, SHUT_WR);

  stream->shutdown_write = 1;
  dsk_hook_clear (&sink->writable_hook);

  stream_do_watch_fd (stream);
}

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskClientStreamSink);
DskClientStreamSinkClass dsk_client_stream_sink_class =
{
  { DSK_OBJECT_CLASS_DEFINE (DskClientStreamSink, &dsk_octet_sink_class,
                             dsk_client_stream_sink_init, NULL),
    dsk_client_stream_sink_write,
    dsk_client_stream_sink_write_buffer,
    dsk_client_stream_sink_shutdown,
  }
};

