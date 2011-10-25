
/* DskNetworkInterfaceList:  find information about network devices on this host. */


/* NOTE: /sbin/ifconfig on linux looks at /proc/net/dev which sometimes
   includes down interfaces that are not in our list. */
typedef struct _DskNetworkInterface DskNetworkInterface;
typedef struct _DskNetworkInterfaceList DskNetworkInterfaceList;


struct _DskNetworkInterface
{
  const char *ifname;

  /* whether this interface is "virtual" -- just connects back to this host */
  unsigned is_loopback : 1;

  /* whether this interface supports broadcasting. */
  unsigned supports_multicast : 1;

  /* whether this interface is receiving packets not intended for it. */
  unsigned is_promiscuous : 1;

  unsigned is_up : 1;

  unsigned has_hw_address : 1;
  unsigned is_p2p : 1;
  unsigned supports_broadcast : 1;

  /* ip-address if the interface is up. */
  DskIpAddress address;

  /* if !is_loopback, this is the device's MAC address. */
  DskEthernetAddress hw_address;

  /* if is_point_to_point, this is the address of the other end of
   * the connection.
   */
  DskIpAddress p2p_address;

  /* if supports_broadcast, this is the broadcast address. */
  DskIpAddress broadcast;
};

struct _DskNetworkInterfaceList
{
  unsigned n_interfaces;
  DskNetworkInterface *interfaces;
};

DskNetworkInterfaceList *dsk_network_interface_list_peek_global (void);
