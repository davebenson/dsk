#include <netinet/in.h>         /* for sockaddr_in etc */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>              /* for snprintf() */
#include "dsk-common.h"
#include "dsk-fd.h"
#include "dsk-ip-address.h"


#define SKIP_WHITESPACE(str)   while (*str == ' ' || *str == '\t') str++
#define C_IN_RANGE(c,min,max)   ((min) <= (c) && (c) <= (max))
#define ASCII_IS_DIGIT(c)  C_IN_RANGE(c,'0','9')
#define ASCII_IS_XDIGIT(c)  ( C_IN_RANGE(c,'0','9') || C_IN_RANGE(c,'a','f') || C_IN_RANGE(c,'A','F') )

bool
dsk_hostname_looks_numeric (const char *str)
{
  unsigned i;
  const char *at;
  at = str;
  SKIP_WHITESPACE (at);
  for (i = 0; i < 3; i++)
    {
      if (!ASCII_IS_DIGIT (*at))
        goto is_not_ipv4;
      while (ASCII_IS_DIGIT (*at))
        at++;
      SKIP_WHITESPACE (at);
      if (*at != '.')
        goto is_not_ipv4;
      at++;
      SKIP_WHITESPACE (at);
    }
  if (!ASCII_IS_DIGIT (*at))
    goto is_not_ipv4;
  while (ASCII_IS_DIGIT (*at))
    at++;
  SKIP_WHITESPACE (at);
  if (*at != 0)
    goto is_not_ipv4;
  return true;              /* ipv4 */

is_not_ipv4:
  for (i = 0; i < 4; i++)
    {
      unsigned j;
      for (j = 0; j < 4; j++)
        {
          if (!ASCII_IS_XDIGIT (*at))
            goto is_not_ipv6;
          at++;
        }
      if (*at != ':')
        goto is_not_ipv6;
      at++;
    }
  return true;

is_not_ipv6:
  return false;
}

unsigned dsk_ip_address_type_get_length (DskIpAddressType type)
{
  switch (type)
    {
    case DSK_IP_ADDRESS_IPV4: return 4;
    case DSK_IP_ADDRESS_IPV6: return 16;
    }
  return 0;
}

bool
dsk_ip_address_parse_numeric (const char *str,
                               DskIpAddress *out)
{
  if (strchr (str, '.') == NULL)
    {
      unsigned n = 0;
      out->type = DSK_IP_ADDRESS_IPV6;
      while (*str)
        {
          long v;
          char *end;
          if (*str == ':')
            str++;
          if (*str == 0)
            break;
          v = strtoul (str, &end, 16);
          if (n == 16)
            return false;
          out->address[n++] = v>>8;
          out->address[n++] = v;
          str = end;
        }
      while (n < 16)
        out->address[n++] = 0;
    }
  else
    {
      /* dotted quad notation */
      char *end;
      out->type = DSK_IP_ADDRESS_IPV4;
      out->address[0] = strtoul (str, &end, 10);
      if (*end != '.')
        return false;
      str = end + 1;
      out->address[1] = strtoul (str, &end, 10);
      if (*end != '.')
        return false;
      str = end + 1;
      out->address[2] = strtoul (str, &end, 10);
      if (*end != '.')
        return false;
      str = end + 1;
      out->address[3] = strtoul (str, &end, 10);
    }
  return true;
}

char *
dsk_ip_address_to_string (const DskIpAddress *addr)
{
  char buf[16*3+1];
  unsigned i;
  static const char hex_chars[16] = "0123456789abcdef";
  switch (addr->type)
    {
    case DSK_IP_ADDRESS_IPV4:
      snprintf (buf, sizeof (buf), "%d.%d.%d.%d", addr->address[0], addr->address[1], addr->address[2], addr->address[3]);
      break;
    case DSK_IP_ADDRESS_IPV6:
      for (i = 0; i < 16; i++)
        {
          buf[3*i+0] = hex_chars[addr->address[i] >> 4];
          buf[3*i+1] = hex_chars[addr->address[i] & 0xf];
        }
      for (i = 0; i < 15; i++)
        buf[3*i+2] = ':';
      buf[3*i+2] = 0;
      break;
    default:
      dsk_assert_not_reached ();
    }
  return dsk_strdup (buf);
}
bool dsk_ip_addresses_equal (const DskIpAddress *a,
                                    const DskIpAddress *b)
{
  unsigned size;
  if (a->type != b->type)
    return false;
  size = a->type == DSK_IP_ADDRESS_IPV4 ? 4 : 16;
  return memcmp (a->address, b->address, size) == 0;
}

void dsk_ip_address_to_sockaddr (const DskIpAddress *address,
                                 unsigned            port,
                                 void               *out,
                                 unsigned           *out_len)
{
  switch (address->type)
    {
    case DSK_IP_ADDRESS_IPV4:
      *out_len = sizeof (struct sockaddr_in);
      break;
    case DSK_IP_ADDRESS_IPV6:
      *out_len = sizeof (struct sockaddr_in6);
      break;
    default:
      dsk_assert_not_reached ();
    }
  memset (out, 0, *out_len);
  switch (address->type)
    {
    case DSK_IP_ADDRESS_IPV4:
      {
        struct sockaddr_in *s = out;
        s->sin_family = PF_INET;
        s->sin_port = htons (port);
        memcpy (&s->sin_addr, address->address, 4);
        break;
      }
    case DSK_IP_ADDRESS_IPV6:
      {
        struct sockaddr_in6 *s = out;
        s->sin6_family = PF_INET6;
        s->sin6_port = htons (port);
        memcpy (&s->sin6_addr, address->address, 16);
        break;
      }
    }
}

bool dsk_sockaddr_to_ip_address  (unsigned     addr_len,
                                         const void   *addr,
                                         DskIpAddress *out,
                                         unsigned     *port_out)
{
  if (addr_len < sizeof (struct sockaddr))
    return false;           /* too short */
  switch (((const struct sockaddr *) addr)->sa_family)
    {
    case PF_INET:
      {
        const struct sockaddr_in *s = addr;
        if (addr_len < sizeof (*s))
          return false;
        out->type = DSK_IP_ADDRESS_IPV4;
        memcpy (out->address, &s->sin_addr, 4);
        *port_out = htons (s->sin_port);
        break;
      }
    case PF_INET6:
      {
        const struct sockaddr_in6 *s = addr;
        if (addr_len < sizeof (*s))
          return false;
        out->type = DSK_IP_ADDRESS_IPV6;
        memcpy (out->address, &s->sin6_addr, 16);
        *port_out = htons (s->sin6_port);
        break;
      }
    default:
      return false;          /* unhandled protocol */
    }
  return true;
}

/* XXX: implement using network-interface api */
void
dsk_ip_address_localhost (DskIpAddress *out)
{
  out->type = DSK_IP_ADDRESS_IPV4;
  out->address[0] = 127;
  out->address[1] = 0;
  out->address[2] = 0;
  out->address[3] = 1;
}

bool dsk_getsockname (DskFileDescriptor fd,
                             DskIpAddress     *addr_out,
                             unsigned         *port_out)
{
  struct sockaddr_storage addr;
  socklen_t addrlen = sizeof (addr);
retry:
  if (getsockname (fd, (struct sockaddr *) &addr, &addrlen) < 0)
    {
      if (errno == EINTR)
        goto retry;
      return false;
    }
  return dsk_sockaddr_to_ip_address (addrlen, &addr, addr_out, port_out);
}
bool dsk_getpeername (DskFileDescriptor fd,
                             DskIpAddress     *addr_out,
                             unsigned         *port_out)
{
  struct sockaddr_storage addr;
  socklen_t addrlen = sizeof (addr);
retry:
  if (getpeername (fd, (struct sockaddr *) &addr, &addrlen) < 0)
    {
      if (errno == EINTR)
        goto retry;
      return false;
    }
  return dsk_sockaddr_to_ip_address (addrlen, &addr, addr_out, port_out);
}
