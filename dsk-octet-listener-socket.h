
typedef struct _DskOctetListenerSocketClass DskOctetListenerSocketClass;
typedef struct _DskOctetListenerSocket DskOctetListenerSocket;

struct _DskOctetListenerSocketClass
{
  DskOctetListenerClass base_class;
};
struct _DskOctetListenerSocket
{
  DskOctetListener base_instance;

  dsk_boolean is_local;

  /* for local (unix-domain) sockets */
  char *path;

  /* for TCP/IP sockets */
  DskIpAddress bind_address;
  unsigned bind_port;
  char *bind_iface;
  
  /* the underlying listening file descriptor */
  DskFileDescriptor listening_fd;
};

typedef struct _DskOctetListenerSocketOptions DskOctetListenerSocketOptions;
struct _DskOctetListenerSocketOptions
{
  dsk_boolean is_local;
  const char *local_path;
  DskIpAddress bind_address;
  int bind_port;
  const char *bind_iface;
  unsigned max_pending_connections;
};
#define DSK_OCTET_LISTENER_SOCKET_OPTIONS_INIT               \
{                                                               \
  DSK_FALSE,                                                    \
  NULL,                                 /* local_path */        \
  DSK_IP_ADDRESS_INIT,               /* bind_address */      \
  0,                                    /* bind_port */         \
  NULL,                                 /* bind_iface */        \
  128                                   /* max_pending_connections */ \
}

DskOctetListener *dsk_octet_listener_socket_new (const DskOctetListenerSocketOptions *options,
                                                 DskError **error);

extern const DskOctetListenerSocketClass dsk_octet_listener_socket_class;
#define DSK_OCTET_LISTENER_SOCKET(object) DSK_OBJECT_CAST(DskOctetListenerSocket, object, &dsk_octet_listener_socket_class)


