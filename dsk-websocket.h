
/* Websockets are a packet-based system,
   with each packet being all UTF-8.
   However, in the future it may get
   binary support.. in fact, it seems likely. */

typedef struct _DskWebsocket DskWebsocket;

typedef enum
{
  DSK_WEBSOCKET_MODE_RETURN_ERROR,
  DSK_WEBSOCKET_MODE_SHUTDOWN,
  DSK_WEBSOCKET_MODE_DROP
} DskWebsocketMode;

struct _DskWebsocket
{
  DskObject base_instance;
  DskHook readable;

  /* underlying structures */
  DskOctetSource *source;
  DskHookTrap *read_trap;
  DskOctetSink *sink;
  DskHookTrap *write_trap;

  /* buffers */
  DskBuffer incoming, outgoing;

  uint64_t to_discard;
  unsigned is_deferred_shutdown : 1;
  unsigned is_shutdown : 1;

  /* tunable:  set be dsk_websocket_tune() */
  DskWebsocketMode bad_packet_type_mode;
  DskWebsocketMode too_long_mode;
  unsigned max_length;
};
extern DskObjectClass dsk_websocket_class;

/* Receive returning FALSE means "no packet ready";
   you might check if 'websocket->error' is set. */
DskIOResult dsk_websocket_receive  (DskWebsocket *websocket,
                                    unsigned     *length_out,
                                    uint8_t     **data_out,
                                    DskError    **error);

/* Data is supposed to be UTF-8, but it's not a fatal error if not. */
void        dsk_websocket_send     (DskWebsocket *websocket,
                                    unsigned      length,
                                    const uint8_t*data);

/* to be called whenever any of the 'tunable' parameters above are
   changed. */
void        dsk_websocket_tune      (DskWebsocket *websocket,
                                     DskWebsocketMode bad_packet_type_mode,
                                     DskWebsocketMode too_long_mode,
                                     unsigned max_length);

void        dsk_websocket_shutdown (DskWebsocket *websocket);


/* transfers ownership */
void _dsk_websocket_server_init (DskWebsocket *websocket,
                                 DskOctetSource *source,
                                 DskOctetSink *sink);
void _dsk_websocket_client_init (DskWebsocket *websocket,
                                 DskOctetSource *source,
                                 DskOctetSink *sink,
                                 DskBuffer    *extra_incoming_data);

/* used by the server to compute a response to the client.
   semi-public b/c it is useful for test code. */
dsk_boolean
_dsk_websocket_compute_response (const char *key1,  /* NUL-terminated */
                                 const char *key2,  /* NUL-terminated */
                                 const uint8_t *key3,  /* 8 bytes long */
                                 uint8_t    *resp,  /* 16 bytes long */
                                 DskError  **error);
