#include "dsk.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>

/* needed under solaris */
#define BSD_COMP

#include <net/if.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>



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
	  g_warning ("The ip protocol was not found in /etc/services...");
	  proto = 0;
	}
      else
	{
	  proto = entry->p_proto;
	}
    }
  return proto;
}

DskNetworkInterfaceList *
dsk_network_interface_list_peek_global(void)
{
  GArray *ifreq_array;
  GArray *rv;
  int tmp_socket;
  guint i;
  tmp_socket = socket (AF_INET, SOCK_DGRAM, get_IPPROTO_IP ());
  if (tmp_socket < 0)
    {
      g_warning ("gsk_network_interface: error creating internal ns socket: %s",
		 g_strerror (errno));
      return NULL;
    }
  ifreq_array = g_array_new (FALSE, FALSE, sizeof (struct ifreq));
  g_array_set_size (ifreq_array, 16);
  for (;;)
    {
      struct ifconf all_interface_config;
      guint num_got;
      all_interface_config.ifc_len = ifreq_array->len * sizeof (struct ifreq);
      all_interface_config.ifc_buf = ifreq_array->data;
      if (ioctl (tmp_socket, SIOCGIFCONF, (char *) &all_interface_config) < 0)
	{ 
	  g_warning ("gsk_network_interface:"
		     "error getting interface configuration: %s",
		     g_strerror (errno));
	  close (tmp_socket);
	  g_array_free (ifreq_array, TRUE);
	  return NULL;
	}
      num_got = all_interface_config.ifc_len / sizeof (struct ifreq);
      if (num_got == ifreq_array->len)
	g_array_set_size (ifreq_array, ifreq_array->len * 2);
      else
	{
	  g_array_set_size (ifreq_array, num_got);
	  break;
	}
    }

  /* now query each of those interfaces. */
  rv = g_array_new (FALSE, FALSE, sizeof (GskNetworkInterface));
  for (i = 0; i < ifreq_array->len; i++)
    {
      struct ifreq *req_array = (struct ifreq *)(ifreq_array->data) + i;
      struct ifreq tmp_req;
      gboolean is_up;
      gboolean is_loopback;
      gboolean has_broadcast;
      gboolean has_multicast;
      gboolean is_p2p;
      guint if_flags;
      GskNetworkInterface interface;

      /* XXX: we don't at all no how to handle a generic interface. */
      /* XXX: is this an IPv6 problem? */
      if (req_array->ifr_addr.sa_family != AF_INET)
	continue;

      memcpy (tmp_req.ifr_name, req_array->ifr_name, sizeof (tmp_req.ifr_name));
      if (ioctl (tmp_socket, SIOCGIFFLAGS, (char *) &tmp_req) < 0) 
	{	
	  g_warning ("error getting information about interface %s",
		     tmp_req.ifr_name);
	  continue;
	}

      if_flags = tmp_req.ifr_flags;
      is_up = (if_flags & IFF_UP) == IFF_UP;
      is_loopback = (if_flags & IFF_LOOPBACK) == IFF_LOOPBACK;
      has_broadcast = (if_flags & IFF_BROADCAST) == IFF_BROADCAST;
      has_multicast = (if_flags & IFF_MULTICAST) == IFF_MULTICAST;
      is_p2p = (if_flags & IFF_POINTOPOINT) == IFF_POINTOPOINT;

      interface.supports_multicast = has_multicast ? 1 : 0;
      interface.is_promiscuous = (if_flags & IFF_PROMISC) ? 1 : 0;

      if (is_up)
	{
	  struct sockaddr *saddr;
	  if (ioctl (tmp_socket, SIOCGIFADDR, (char *) &tmp_req) < 0)
	    {
	      g_warning ("error getting the ip address for interface %s",
			 tmp_req.ifr_name);
	      continue;
	    }
	  saddr = &tmp_req.ifr_addr;
	  interface.address = gsk_socket_address_from_native (saddr, sizeof (*saddr));
	}
      else
	interface.address = NULL;

      interface.is_loopback = is_loopback ? 1 : 0;
#ifdef SIOCGIFHWADDR
      if (!interface.is_loopback)
	interface.hw_address = NULL;
      else
	{
	  if (ioctl (tmp_socket, SIOCGIFHWADDR, (char *) &tmp_req) < 0)
	    {
	      g_warning ("error getting the hardware address for interface %s",
			 tmp_req.ifr_name);
	      continue;
	    }
	  interface.hw_address = gsk_socket_address_ethernet_new ((guint8*)tmp_req.ifr_addr.sa_data);
	}
#else
      interface.hw_address = NULL;
#endif

      if (is_p2p)
	{
	  struct sockaddr *saddr;
	  if (ioctl (tmp_socket, SIOCGIFDSTADDR, (char *) &tmp_req) < 0)
	    {
	      g_warning ("error getting the ip address for interface %s",
			 tmp_req.ifr_name);
	      continue;
	    }
	  saddr = &tmp_req.ifr_addr;
	  interface.p2p_address = gsk_socket_address_from_native (saddr,
								  sizeof (struct sockaddr));
	}
      else
	interface.p2p_address = NULL;

      if (has_broadcast)
	{
	  struct sockaddr *saddr;
	  if (ioctl (tmp_socket, SIOCGIFBRDADDR, (char *) &tmp_req) < 0)
	    {
	      g_warning ("error getting the broadcast address for interface %s",
			 tmp_req.ifr_name);
	      continue;
	    }
	  saddr = &tmp_req.ifr_addr;
          interface.broadcast = gsk_socket_address_from_native (saddr,
								sizeof (struct sockaddr));
	}
      else
	interface.broadcast = NULL;

      interface.ifname = g_strdup (tmp_req.ifr_name);
      g_array_append_val (rv, interface);
    }

  close (tmp_socket);

  g_array_free (ifreq_array, TRUE);
  {
    GskNetworkInterfaceSet *set;
    set = g_new (GskNetworkInterfaceSet, 1);
    set->num_interfaces = rv->len;
    set->interfaces = (GskNetworkInterface *) rv->data;
    return set;
  }
}

