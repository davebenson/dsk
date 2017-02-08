/* needed under solaris */
#define BSD_COMP

/* needed under certain glibc versions */
#define _BSD_SOURCE

#include "dsk.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>

#include <sys/ioctl.h>
#include <net/if.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>

/* TODO: open /proc/net/dev on linux to find more (down) interfaces. */

static int
get_IPPROTO_IP ()
{
  static int proto = -1;
  if (proto < 0)
    {
      struct protoent *entry;
      entry = getprotobyname ("ip");
      if (entry == NULL)
	{
	  dsk_warning ("The ip protocol was not found in /etc/services...");
	  proto = 0;
	}
      else
	{
	  proto = entry->p_proto;
	}
    }
  return proto;
}

static DskNetworkInterfaceList *global_interface_list = NULL;

static void
dsk_ip_address_from_native (struct sockaddr *addr, DskIpAddress *out)
{
  if (addr->sa_family == AF_INET)
    {
      out->type = DSK_IP_ADDRESS_IPV4;
      memcpy (out->address, (uint8_t*)(&((struct sockaddr_in *)addr)->sin_addr), 4);
    }
  else if (addr->sa_family == AF_INET6)
    {
      out->type = DSK_IP_ADDRESS_IPV6;
      memcpy (out->address, (uint8_t*)(&((struct sockaddr_in6 *)addr)->sin6_addr), 16);
    }
  else
    {
      dsk_warning ("bad socket-address family in dsk_ip_address_from_native: %u", addr->sa_family);
    }
}

static DskNetworkInterfaceList *
make_interface_list (void)
{
  unsigned ifreq_alloced = 16;
  struct ifreq *ifreq_array = DSK_NEW_ARRAY (ifreq_alloced, struct ifreq);
  unsigned n_ifreq;
  DskNetworkInterface *rv;
  int tmp_socket;
  unsigned i;
  tmp_socket = socket (AF_INET, SOCK_DGRAM, get_IPPROTO_IP ());
  if (tmp_socket < 0)
    {
      dsk_warning ("dsk_network_interface: error creating internal ns socket: %s",
		   strerror (errno));
      return NULL;
    }
  //ifreq_array = g_array_new (FALSE, FALSE, sizeof (struct ifreq));
  //g_array_set_size (ifreq_array, 16);
  for (;;)
    {
      struct ifconf all_interface_config;
      all_interface_config.ifc_len = ifreq_alloced * sizeof (struct ifreq);
      all_interface_config.ifc_buf = (void *) ifreq_array;
      if (ioctl (tmp_socket, SIOCGIFCONF, (char *) &all_interface_config) < 0)
	{ 
	  dsk_warning ("dsk_network_interface:"
		       "error getting interface configuration: %s",
		       strerror (errno));
	  close (tmp_socket);
	  dsk_free (ifreq_array);
	  return NULL;
	}
      n_ifreq = all_interface_config.ifc_len / sizeof (struct ifreq);
      if (n_ifreq == ifreq_alloced)
        {
          ifreq_alloced *= 2;
          ifreq_array = DSK_RENEW (struct ifreq, ifreq_array, ifreq_alloced);
        }
      else
        break;
    }
  dsk_warning ("n_ifreq=%u",n_ifreq);

  /* now query each of those interfaces. */
  rv = DSK_NEW_ARRAY (n_ifreq, DskNetworkInterface);
  unsigned n_interfaces = 0;
  for (i = 0; i < n_ifreq; i++)
    {
      struct ifreq tmp_req;
      unsigned if_flags;
      DskNetworkInterface interface;

      /* XXX: we don't at all no how to handle a generic interface. */
      /* XXX: is this an IPv6 problem? */
      if (ifreq_array[i].ifr_addr.sa_family != AF_INET)
	continue;

      memcpy (tmp_req.ifr_name, ifreq_array[i].ifr_name, sizeof (tmp_req.ifr_name));
      if (ioctl (tmp_socket, SIOCGIFFLAGS, (char *) &tmp_req) < 0) 
	{	
	  dsk_warning ("error getting information about interface %s",
		     tmp_req.ifr_name);
	  continue;
	}


      memset (&interface, 0, sizeof (interface));
      if_flags = tmp_req.ifr_flags;
      interface.is_up = (if_flags & IFF_UP) == IFF_UP;
      interface.is_loopback = (if_flags & IFF_LOOPBACK) == IFF_LOOPBACK;
      interface.supports_broadcast = (if_flags & IFF_BROADCAST) == IFF_BROADCAST;
      interface.supports_multicast = (if_flags & IFF_MULTICAST) == IFF_MULTICAST;
      interface.is_p2p = (if_flags & IFF_POINTOPOINT) == IFF_POINTOPOINT;
      interface.is_promiscuous = (if_flags & IFF_PROMISC) ? 1 : 0;

      if (interface.is_up)
	{
	  struct sockaddr *saddr;
	  if (ioctl (tmp_socket, SIOCGIFADDR, (char *) &tmp_req) < 0)
	    {
	      dsk_warning ("error getting the ip address for interface %s",
			 tmp_req.ifr_name);
	      continue;
	    }
	  saddr = &tmp_req.ifr_addr;
	  dsk_ip_address_from_native (saddr, &interface.address);
	}

#ifdef SIOCGIFHWADDR
      if (interface.is_loopback)
        interface.has_hw_address = DSK_FALSE;
      else
	{
	  if (ioctl (tmp_socket, SIOCGIFHWADDR, (char *) &tmp_req) < 0)
	    {
	      dsk_warning ("error getting the hardware address for interface %s",
			 tmp_req.ifr_name);
	      continue;
	    }
          interface.has_hw_address = DSK_TRUE;
	  memcpy (interface.hw_address.address, (uint8_t*)tmp_req.ifr_addr.sa_data, 6);
	}
#else
      interface.has_hw_address = DSK_FALSE;
#endif

      if (interface.is_p2p)
	{
	  struct sockaddr *saddr;
	  if (ioctl (tmp_socket, SIOCGIFDSTADDR, (char *) &tmp_req) < 0)
	    {
	      dsk_warning ("error getting the ip address for interface %s",
			 tmp_req.ifr_name);
	      continue;
	    }
	  saddr = &tmp_req.ifr_addr;
	  dsk_ip_address_from_native (saddr, &interface.p2p_address);
	}

      if (interface.supports_broadcast)
	{
	  struct sockaddr *saddr;
	  if (ioctl (tmp_socket, SIOCGIFBRDADDR, (char *) &tmp_req) < 0)
	    {
	      dsk_warning ("error getting the broadcast address for interface %s",
			   tmp_req.ifr_name);
	      continue;
	    }
	  saddr = &tmp_req.ifr_addr;
          dsk_ip_address_from_native (saddr, &interface.broadcast);
	}

      interface.ifname = dsk_strdup (tmp_req.ifr_name);
      rv[n_interfaces++] = interface;
    }

  close (tmp_socket);

  dsk_free (ifreq_array);

  DskNetworkInterfaceList *list = DSK_NEW (DskNetworkInterfaceList);
  list->n_interfaces = n_interfaces;
  list->interfaces = rv;
  return list;
}

DskNetworkInterfaceList *
dsk_network_interface_list_peek_global(void)
{
  if (global_interface_list == NULL)
    global_interface_list = make_interface_list ();
  return global_interface_list;
}

