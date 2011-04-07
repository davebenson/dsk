
/* Websockets are a packet-based system,
   with each packet being all UTF-8.
   However, in the future it may get
   binary support.. in fact, it seems likely. */

typedef struct _DskWebsocket DskWebsocket;
typedef struct _DskWebsocketPacket DskWebsocketPacket;
struct _DskWebsocket
{
  DskObject base_instance;
  DskHook readable;
  DskHook error_hook;

  /* underlying structures */
  DskOctetSource *source;
  DskHookTrap *read_trap;
  DskOctetSink *sink;
  DskHookTrap *write_trap;

  /* if error_hook triggers, this is the error. */
  DskError *error;

  /* buffers */
  DskBuffer incoming, outgoing;

  /* Pending packet-queue */
  unsigned n_pending;
  unsigned first_pending;
  DskWebsocketPacket *pending;
  unsigned max_pending;
};
extern DskObjectClass dsk_websocket_class;


/* Receive returning FALSE means "no packet ready";
   you might check if 'websocket->error' is set. */
dsk_boolean dsk_websocket_retrieve (DskWebsocket *websocket,
                                    unsigned     *length_out,
                                    uint8_t     **data_out);

/* Data is supposed to be UTF-8, but it's not a fatal error if not. */
void        dsk_websocket_send     (DskWebsocket *websocket,
                                    unsigned      length,
                                    uint8_t      *data);

void        dsk_websocket_shutdown (DskWebsocket *websocket);


/* transfers ownership */
void _dsk_websocket_server_init (DskWebsocket *websocket,
                                 DskOctetSource *source,
                                 DskOctetSink *sink);
