
/* Websockets are a packet-based system,
   with each packet being all UTF-8.
   However, in the future it may get
   binary support.. in fact, it seems likely. */

typedef struct _DskWebsocket DskWebsocket;
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
};
extern DskObjectClass dsk_websocket_class;


/* Receive returning FALSE means "no packet ready";
   you might check if 'websocket->error' is set. */
dsk_boolean dsk_websocket_retrieve (DskWebsocket *websocket,
                                    unsigned     *length_out,
                                    uint8_t     **data_out);

/* Send returns FALSE if packet isn't UTF-8, which is required
 * at the moment.  Use 0 for DskWebsocketSendFlags until we have some.
 */
dsk_boolean dsk_websocket_send     (DskWebsocket *websocket,
                                    unsigned      length,
                                    uint8_t      *data,
                                    DskError    **error);

void        dsk_websocket_shutdown (DskWebsocket *websocket);


/* transfers ownership */
void _dsk_websocket_server_init (DskWebsocket *websocket,
                                 DskOctetSource *source,
                                 DskOctetSink *sink);
