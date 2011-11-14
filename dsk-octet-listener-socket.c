#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>

#include "dsk-common.h"
#include "dsk-mem-pool.h"
#include "dsk-hook.h"
#include "dsk-object.h"
#include "dsk-error.h"
#include "dsk-buffer.h"
#include "dsk-octet-io.h"
#include "dsk-octet-listener.h"
#include "dsk-fd.h"
#include "dsk-ip-address.h"
#include "dsk-dispatch.h"
#include "dsk-main.h"
#include "dsk-octet-fd.h"
#include "dsk-octet-listener-socket.h"

static DskIOResult
dsk_octet_listener_socket_accept (DskOctetListener        *listener,
                                  DskOctetStream         **stream_out,
			          DskOctetSource         **source_out,
			          DskOctetSink           **sink_out,
                                  DskError               **error)
{
  DskOctetListenerSocket *s = DSK_OCTET_LISTENER_SOCKET (listener);
  struct sockaddr addr;
  socklen_t addr_len = sizeof (addr);
  DskFileDescriptor fd = accept (s->listening_fd, &addr, &addr_len);
  if (fd < 0)
    {
      if (errno == EINTR || errno == EAGAIN)
        return DSK_IO_RESULT_AGAIN;
      dsk_set_error (error, "error accepting connection from listening fd %d: %s",
                     (int) s->listening_fd, strerror (errno));
      return DSK_IO_RESULT_ERROR;
    }


  if (!dsk_octet_stream_new_fd (fd, DSK_FILE_DESCRIPTOR_IS_READABLE|
                                    DSK_FILE_DESCRIPTOR_IS_WRITABLE|
                                    DSK_FILE_DESCRIPTOR_IS_POLLABLE,
                                    (DskOctetStreamFd **) stream_out,
                                    (DskOctetStreamFdSource **) source_out,
                                    (DskOctetStreamFdSink **) sink_out,
                                    error))
    {
      close (fd);
      return DSK_IO_RESULT_ERROR;
    }
  return DSK_IO_RESULT_SUCCESS;
}

static void
dsk_octet_listener_socket_shutdown (DskOctetListener *listener)
{
  DskOctetListenerSocket *s = (DskOctetListenerSocket *) listener;
  if (s->listening_fd >= 0)
    listen (s->listening_fd, 0);
  dsk_hook_clear (&listener->incoming);
}

static void
listener_handle_fd_readable (DskFileDescriptor   fd,
                             unsigned            events,
                             void               *callback_data)
{
  DskOctetListener *listener = DSK_OCTET_LISTENER (callback_data);
  DskOctetListenerSocket *sock = DSK_OCTET_LISTENER_SOCKET (callback_data);
  dsk_assert (fd == sock->listening_fd);
  dsk_assert (events & DSK_EVENT_READABLE);
  dsk_hook_notify (&listener->incoming);
}

static void
dsk_octet_listener_socket_set_poll (DskOctetListenerSocket *listener_socket,
                                    dsk_boolean             poll)
{
  if (listener_socket->listening_fd < 0)
    return;

  if (poll)
    {
      dsk_main_watch_fd (listener_socket->listening_fd,
                         DSK_EVENT_READABLE,
                         listener_handle_fd_readable,
                         listener_socket);
    }
  else
    {
      dsk_main_watch_fd (listener_socket->listening_fd, 0, NULL, NULL);
    }
}


static DskHookFuncs listener_incoming_hook_funcs =
{
  (DskHookObjectFunc) dsk_object_ref_f,
  (DskHookObjectFunc) dsk_object_unref_f,
  (DskHookSetPoll) dsk_octet_listener_socket_set_poll
};
   

static void
dsk_octet_listener_socket_init (DskOctetListenerSocket *s)
{
  s->listening_fd = -1;
  dsk_hook_set_funcs (&s->base_instance.incoming, &listener_incoming_hook_funcs);
}
static void
dsk_octet_listener_socket_finalize (DskOctetListenerSocket *s)
{
  dsk_main_close_fd (s->listening_fd);
  s->listening_fd = -1;

  /* XXX: this is horribly broken if the current-working directory
     changes and s->path depends on it. */
  if (s->path)
    unlink (s->path);

  dsk_free (s->path);
  dsk_free (s->bind_iface);
}

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskOctetListenerSocket);
const DskOctetListenerSocketClass dsk_octet_listener_socket_class =
{ { DSK_OBJECT_CLASS_DEFINE(DskOctetListenerSocket,
                            &dsk_octet_listener_class,
                            dsk_octet_listener_socket_init,
                            dsk_octet_listener_socket_finalize),
    dsk_octet_listener_socket_accept,
    dsk_octet_listener_socket_shutdown
} };

static dsk_boolean
maybe_delete_stale_socket (const char *path,
                           unsigned addr_len,
                           struct sockaddr *addr,
                           DskError **error)
{
  int fd;
  struct stat statbuf;
  if (stat (path, &statbuf) < 0)
    return DSK_TRUE;
  if (!S_ISSOCK (statbuf.st_mode))
    {
      dsk_set_error (error, "%s existed but was not a socket\n", path);
      return DSK_FALSE;
    }

  fd = socket (PF_UNIX, SOCK_STREAM, 0);
  if (fd < 0)
    return DSK_TRUE;
  dsk_fd_set_nonblocking (fd);
  if (connect (fd, addr, addr_len) < 0)
    {
      if (errno == EINPROGRESS)
        {
          close (fd);
          return DSK_TRUE;
        }
    }
  else
    {
      close (fd);
      return DSK_TRUE;
    }

  /* ok, we should delete the stale socket */
  close (fd);
  if (unlink (path) < 0)
    {
      dsk_set_error (error, "unable to delete %s: %s", path, strerror(errno));
      return DSK_FALSE;
    }
  return DSK_TRUE;
}


DskOctetListener *
dsk_octet_listener_socket_new (const DskOctetListenerSocketOptions *options,
                               DskError **error)
{
  union {
    struct sockaddr_storage storage;
    struct sockaddr addr;
    struct sockaddr_un un;
  } storage;
  struct sockaddr *addr = &storage.addr;
  socklen_t addr_len = sizeof (storage);
  DskFileDescriptor fd;
  DskOctetListenerSocket *rv;
  /* check options for validity */
  if (options->is_local)
    {
      if (options->local_path == NULL)
        {
          dsk_set_error (error, "path must be given to bind to local socket");
          return NULL;
        }
    }

  /* set-up socket address */
  if (options->is_local)
    {
      struct sockaddr_un *un = &storage.un;
      addr_len = sizeof (struct sockaddr_un);
      memset (un, 0, addr_len);
      un->sun_family = PF_LOCAL;
      strncpy (un->sun_path, options->local_path, sizeof (un->sun_path));
      if (!maybe_delete_stale_socket (options->local_path, addr_len, addr, error))
        return NULL;
    }
  else
    {
      unsigned al = addr_len;
      dsk_ip_address_to_sockaddr (&options->bind_address, options->bind_port,
                                  addr, &al);
      addr_len = al;
    }

  /* create bound file-descriptor */
retry_socket_syscall:
  fd = socket (addr->sa_family, SOCK_STREAM, 0);
  if (fd < 0)
    {
      if (errno == EINTR)
        goto retry_socket_syscall;
      dsk_set_error (error, "error creating socket: %s", strerror (errno));
      return NULL;
    }

  {
    int one = 1;
    if (setsockopt (fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof (one)) < 0)
      {
        dsk_warning ("setsockopt(SO_REUSEADDR) failed: %s", strerror (errno));
        /* ignore the failure. */
      }
  }
retry_bind_syscall:
  if (bind (fd, addr, addr_len) < 0)
    {
      if (errno == EINTR)
        goto retry_bind_syscall;
      if (options->is_local)
        dsk_set_error (error, "error binding socket (fd=%u) to path %s: %s", fd, options->local_path, strerror (errno));
      else
        dsk_set_error (error, "error binding socket (fd=%u) to port %u: %s", fd, options->bind_port, strerror (errno));
      close (fd);
      return NULL;
    }
retry_listen_syscall:
  if (listen (fd, options->max_pending_connections) < 0)
    {
      if (errno == EINTR)
        goto retry_listen_syscall;
      dsk_set_error (error, "error setting size of listen queue to %u: %s",
                     options->max_pending_connections, strerror (errno));
      close (fd);
      return NULL;
    }

#if 0
  /* if the user lets the OS choose the port, read the port back in. */
  if (!options->is_local && options->bind_port == 0)
    {
      ...
    }
#endif

  dsk_fd_set_nonblocking (fd);
  dsk_fd_set_close_on_exec (fd);

  rv = dsk_object_new (&dsk_octet_listener_socket_class);
  rv->is_local = options->is_local;
  rv->path = dsk_strdup (options->local_path);
  rv->bind_address = options->bind_address;
  rv->bind_port = options->bind_port;
  rv->bind_iface = dsk_strdup (options->bind_iface);
  rv->listening_fd = fd;
  return &rv->base_instance;
}
