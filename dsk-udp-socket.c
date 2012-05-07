#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include "dsk.h"

static void
handle_dispatch_events (DskFileDescriptor   fd,
                        unsigned            events,
                        void               *callback_data)
{
  DskUdpSocket *socket = callback_data;
  dsk_assert (socket->fd == fd);
  if (events & DSK_EVENT_READABLE)
    dsk_hook_notify (&socket->readable);
  if (events & DSK_EVENT_WRITABLE)
    dsk_hook_notify (&socket->writable);
}

static void handle_set_poll (DskUdpSocket *socket)
{
  unsigned events = 0;
  if (socket->fd == -1)
    return;
  if (dsk_hook_is_trapped (&socket->readable))
    events |= DSK_EVENT_READABLE;
  if (dsk_hook_is_trapped (&socket->writable))
    events |= DSK_EVENT_WRITABLE;
  dsk_dispatch_watch_fd (dsk_dispatch_default (),
                         socket->fd,
                         events,
                         handle_dispatch_events,
                         socket);
}

static DskHookFuncs udp_socket_read_hook_funcs =
{
  (DskHookObjectFunc) dsk_object_ref,
  (DskHookObjectFunc) dsk_object_unref,
  (DskHookSetPoll) handle_set_poll
};

static DskHookFuncs udp_socket_write_hook_funcs =
{
  (DskHookObjectFunc) dsk_object_ref,
  (DskHookObjectFunc) dsk_object_unref,
  (DskHookSetPoll) handle_set_poll
};

static void
dsk_udp_socket_init (DskUdpSocket *socket)
{
  socket->fd = -1;
  dsk_hook_init (&socket->readable, socket);
  dsk_hook_init (&socket->writable, socket);
  dsk_hook_set_funcs (&socket->readable, &udp_socket_read_hook_funcs);
  dsk_hook_set_funcs (&socket->writable, &udp_socket_write_hook_funcs);
}

static void
dsk_udp_socket_finalize (DskUdpSocket *socket)
{
  if (socket->fd != -1)
    close (socket->fd);
}

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskUdpSocket);
DskUdpSocketClass dsk_udp_socket_class =
{
  DSK_OBJECT_CLASS_DEFINE(DskUdpSocket,
                          &dsk_object_class,
                          dsk_udp_socket_init,
                          dsk_udp_socket_finalize)
};

DskUdpSocket *
dsk_udp_socket_new     (dsk_boolean  is_ipv6,
                        DskError   **error)
{
  DskFileDescriptor fd;
  int domain = AF_INET;
  DskUdpSocket *rv;
  if (is_ipv6)
    {
#if DSK_HAVE_IPV6
      domain = AF_INET6;
#else
      dsk_set_error (error, "IPv6 not supported");
      return NULL;
#endif
    }
  fd = socket (domain, SOCK_DGRAM, 0);
  if (fd < 0)
    {
      int e = errno;
      dsk_fd_creation_failed (e);
      dsk_set_error (error, "error creating file-descriptor: %s",
                     strerror (e));
      return NULL;
    }
  rv = dsk_object_new (&dsk_udp_socket_class);
  rv->fd = fd;
  dsk_fd_set_nonblocking (fd);
  return rv;
}
static inline DskIOResult
handle_truncated (unsigned rv,
                  unsigned length,
                  DskError **error)
{
  dsk_set_error (error,
                 "data truncated sending udp packet (%u of %u bytes sent)",
                 (unsigned) rv, (unsigned) length);
  return DSK_IO_RESULT_ERROR;
}

DskIOResult
dsk_udp_socket_send    (DskUdpSocket  *socket,
                        unsigned       length,
                        const uint8_t *data,
                        DskError     **error)
{
  ssize_t rv;
  if (!socket->is_connected)
    {
      dsk_set_error (error, "cannot send udp packet without being connected");
      return DSK_IO_RESULT_ERROR;
    }
  rv = send (socket->fd, data, length, 0);
  if (rv < 0)
    {
      if (errno == EINTR || errno == EAGAIN)
        return DSK_IO_RESULT_AGAIN;
      dsk_set_error (error, "error sending udp packet: %s", strerror (errno));
      return DSK_IO_RESULT_ERROR;
    }
  if ((size_t) rv < length)
    return handle_truncated (rv, length, error);
  return DSK_IO_RESULT_SUCCESS;
}

DskIOResult
dsk_udp_socket_send_to_ip      (DskUdpSocket  *socket,
                                const DskIpAddress  *address,
                                unsigned       port,
                                unsigned       length,
                                const uint8_t *data,
                                DskError     **error)
{
  struct sockaddr_storage addr;
  unsigned addr_len;
  ssize_t rv;
  dsk_ip_address_to_sockaddr (address, port, &addr, &addr_len);
  rv = sendto (socket->fd, data, length, 0,
               (struct sockaddr *) &addr, addr_len);
  if (rv < 0)
    {
      int e = errno;
      if (e == EINTR || e == EAGAIN)
        return DSK_IO_RESULT_AGAIN;
      dsk_set_error (error, "error sending udp packet: %s", strerror (errno));
      return DSK_IO_RESULT_ERROR;
    }
  if ((unsigned) rv < length)
    return handle_truncated (rv, length, error);
  return DSK_IO_RESULT_SUCCESS;
}

typedef struct _SendBlockingDnsData SendBlockingDnsData;
struct _SendBlockingDnsData
{
  DskUdpSocket *socket;
  unsigned port;
  unsigned length;
  uint8_t *send_data;
};

static void
handle_send_blocking_dns_data (DskDnsLookupResult *result,
                               void               *callback_data)
{
  SendBlockingDnsData *sbdd = callback_data;
  if (result->type == DSK_DNS_LOOKUP_RESULT_FOUND)
    {
      dsk_udp_socket_send_to_ip (sbdd->socket, result->addr,
                                 sbdd->port, sbdd->length, sbdd->send_data,
                                 NULL);
    }
  else
    {
      dsk_warning ("dns failure looking up address for udp packet: %s",
                   result->message);
    }
  dsk_object_unref (sbdd->socket);
  dsk_free (sbdd);
}

DskIOResult
dsk_udp_socket_send_to (DskUdpSocket  *socket,
                        const char    *name,
                        unsigned       port,
                        unsigned       length,
                        const uint8_t *data,
                        DskError     **error)
{
  DskIpAddress address;
  switch (dsk_dns_lookup_nonblocking (name, &address, socket->is_ipv6, error))
    {
    case DSK_DNS_LOOKUP_NONBLOCKING_NOT_FOUND:
      dsk_set_error (error, "hostname %s not found", name);
      return DSK_IO_RESULT_ERROR;
    case DSK_DNS_LOOKUP_NONBLOCKING_MUST_BLOCK:
      {
        SendBlockingDnsData *sbdd = dsk_malloc (sizeof (SendBlockingDnsData) + length);
        dsk_object_ref (socket);
        sbdd->port = port;
        sbdd->length = length;
        sbdd->send_data = (uint8_t*)(sbdd + 1);
        memcpy (sbdd->send_data, data, length);
        sbdd->socket = socket;
        dsk_dns_lookup (name, socket->is_ipv6, handle_send_blocking_dns_data, sbdd);
        return DSK_IO_RESULT_SUCCESS;
      }
    case DSK_DNS_LOOKUP_NONBLOCKING_FOUND:
      return dsk_udp_socket_send_to_ip (socket, &address, port,
                                        length, data, error);
    case DSK_DNS_LOOKUP_NONBLOCKING_ERROR:
      return DSK_IO_RESULT_ERROR;
    }
  dsk_assert_not_reached ();
  return DSK_IO_RESULT_ERROR;
}

dsk_boolean
dsk_udp_socket_bind    (DskUdpSocket  *socket,
                        DskIpAddress *bind_addr,
                        unsigned       port,
                        DskError     **error)
{
  struct sockaddr_storage addr;
  unsigned addr_len;
  if (bind_addr == NULL)
    {
      static DskIpAddress default_bind = DSK_IP_ADDRESS_INIT;
      bind_addr = &default_bind;
    }
  dsk_ip_address_to_sockaddr (bind_addr, port, &addr, &addr_len);
  if (bind (socket->fd, (struct sockaddr *) &addr, addr_len) < 0)
    {
      dsk_set_error (error, "error binding: %s", strerror (errno));
      return DSK_FALSE;
    }
  socket->is_bound = 1;
  return DSK_TRUE;
}

DskIOResult
dsk_udp_socket_receive (DskUdpSocket  *socket,
                        DskIpAddress *addr_out,
                        unsigned      *port_out,
                        unsigned      *len_out,
                        uint8_t      **data_out,
                        DskError     **error)
{
  void *buf = socket->recv_slab;
  unsigned length = socket->recv_slab_len;
  ssize_t rv;
  struct sockaddr_storage sysaddr;
  struct msghdr msg;
  struct iovec iov;
  if (buf == NULL)
    {
      int value;
      if (ioctl (socket->fd, FIONREAD, &value) < 0)
        {
          if (errno == EINTR || errno == EAGAIN)
            return DSK_IO_RESULT_AGAIN;
          dsk_set_error (error, "error getting udp packet size from socket");
          return DSK_IO_RESULT_ERROR;
        }
      if (value == 0)
        return DSK_IO_RESULT_AGAIN;
      buf = dsk_malloc (value);
      length = value;
    }
  if (addr_out != NULL || port_out != NULL)
    {
      msg.msg_name = &sysaddr;
      msg.msg_namelen = sizeof (sysaddr);
    }
  else
    {
      msg.msg_name = NULL;
      msg.msg_namelen = 0;
    }
  iov.iov_base = buf;
  iov.iov_len = length;
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  msg.msg_control = NULL;
  msg.msg_controllen = 0;
  msg.msg_flags = 0;
  rv = recvmsg (socket->fd, &msg, 0);
  if (rv < 0)
    {
      int e = errno;
      if (socket->recv_slab == NULL)
        dsk_free (buf);
      if (e == EINTR || e == EAGAIN)
        return DSK_IO_RESULT_AGAIN;
      dsk_set_error (error, "error reading from udp socket: %s",
                     strerror (e));
      return DSK_IO_RESULT_ERROR;
    }
  else if (rv == 0)
    return DSK_IO_RESULT_EOF;

  if (addr_out != NULL || port_out != NULL)
    {
      if (!dsk_sockaddr_to_ip_address (msg.msg_namelen, msg.msg_name,
                                        addr_out, port_out))
        dsk_error ("dsk_sockaddr_to_ip_address failed in udp code");
    }
  *len_out = rv;
  if (socket->recv_slab == NULL)
    *data_out = buf;
  else
    *data_out = dsk_memdup (rv, socket->recv_slab);
  return DSK_IO_RESULT_SUCCESS;
}
