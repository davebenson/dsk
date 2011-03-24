typedef struct _DskUdpSocketClass DskUdpSocketClass;
typedef struct _DskUdpSocket DskUdpSocket;

struct _DskUdpSocketClass
{
  DskObjectClass base_class;
};

struct _DskUdpSocket
{
  DskObject base_instance;
  DskHook readable;
  DskHook writable;

  unsigned is_bound : 1;
  unsigned is_connected : 1;
  unsigned is_ipv6 : 1;
  DskFileDescriptor fd;

  DskIpAddress bound_address;
  DskIpAddress connect_address;

  uint8_t *recv_slab;
  unsigned recv_slab_len;
};


DskUdpSocket * dsk_udp_socket_new     (dsk_boolean  is_ipv6,
                                       DskError   **error);
DskIOResult    dsk_udp_socket_send    (DskUdpSocket  *socket,
                                       unsigned       length,
                                       const uint8_t *data,
			               DskError     **error);
DskIOResult    dsk_udp_socket_send_to (DskUdpSocket  *socket,
                                       const char    *name,
		                       unsigned       port,
			               unsigned       length,
			               const uint8_t *data,
			               DskError     **error);
DskIOResult    dsk_udp_socket_send_to_ip(DskUdpSocket  *socket,
                                       const DskIpAddress *addr,
		                       unsigned       port,
			               unsigned       length,
			               const uint8_t *data,
			               DskError     **error);
dsk_boolean    dsk_udp_socket_bind    (DskUdpSocket  *socket,
                                       DskIpAddress *bind_addr,
				       unsigned       port,
			               DskError     **error);
DskIOResult    dsk_udp_socket_receive (DskUdpSocket  *socket,
                                       DskIpAddress *addr_out,
			               unsigned      *port_out,
			               unsigned      *len_out,
			               uint8_t      **data_out,
			               DskError     **error);

