
typedef struct _DskSocketStreamListenerClass DskSocketStreamListenerClass;
typedef struct _DskSocketStreamListener DskSocketStreamListener;

struct _DskSocketStreamListenerClass
{
  DskStreamListenerClass base_class;
};
struct _DskSocketStreamListener
{
  DskStreamListener base_instance;

  bool is_local;

  /* for local (unix-domain) sockets */
  char *path;

  /* for TCP/IP sockets */
  DskIpAddress bind_address;
  unsigned bind_port;
  char *bind_iface;
  
  /* the underlying listening file descriptor */
  DskFileDescriptor listening_fd;
};

typedef struct _DskSocketStreamListenerOptions DskSocketStreamListenerOptions;
struct _DskSocketStreamListenerOptions
{
  dsk_boolean is_local;
  const char *local_path;
  DskIpAddress bind_address;
  int bind_port;
  const char *bind_iface;
  unsigned max_pending_connections;
};
#define DSK_SOCKET_STREAM_LISTENER_OPTIONS_INIT               \
{                                                               \
  false,                                                    \
  NULL,                                 /* local_path */        \
  DSK_IP_ADDRESS_INIT,               /* bind_address */      \
  0,                                    /* bind_port */         \
  NULL,                                 /* bind_iface */        \
  128                                   /* max_pending_connections */ \
}

DskStreamListener *
dsk_socket_stream_listener_new (const DskSocketStreamListenerOptions *options,
                                DskError **error);

extern const DskSocketStreamListenerClass dsk_socket_stream_listener_class;
#define DSK_SOCKET_STREAM_LISTENER(object) DSK_OBJECT_CAST(DskSocketStreamListener, object, &dsk_socket_stream_listener_class)


