
/* DskNetworkInterfaceList:  find information about network devices on this host. */

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

  /* ip-address if the interface is up. */
  DskSocketAddress *address;

  /* if !is_loopback, this is the device's MAC address. */
  DskSocketAddress *hw_address;

  /* if is_point_to_point, this is the address of the other end of
   * the connection.
   */
  DskSocketAddress *p2p_address;

  /* if supports_broadcast, this is the broadcast address. */
  DskSocketAddress *broadcast;
};

struct _DskNetworkInterfaceList
{
  unsigned n_interfaces;
  DskNetworkInterface *interfaces;
};

DskNetworkInterfaceList *dsk_network_interface_list_peek_global (void);
