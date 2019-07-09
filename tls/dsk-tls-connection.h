typedef struct DskTlsConnection DskTlsConnection;

#define DSK_TLS_CONNECTION_STATE_IS_CLIENT(st)   (((st) >> 8) == 1)
#define DSK_TLS_CONNECTION_STATE_IS_SERVER(st)   (((st) >> 8) == 2)

typedef struct DskTlsContext DskTlsContext;
typedef struct DskTlsConnectionClass DskTlsConnectionClass;
typedef struct DskTlsHandshakeNegotiation DskTlsHandshakeNegotiation;
struct DskTlsConnectionClass
{
  DskStreamClass base_class;
};
struct DskTlsConnection
{
  DskStream base_instance;
  DskStream *underlying;
  DskTlsContext *context;
  DskHookTrap *write_trap;
  DskHookTrap *read_trap;


  // This object is deleted after handshaking.
  DskTlsHandshakeNegotiation *handshake_info;

  // State-Machine State.
  DskTlsConnectionState state;

  // Negotiated state.
  DskTlsProtocolVersion version;

  // Incoming/Outgoing raw data, and 
  DskBuffer incoming_raw, outgoing_raw;
  DskFlatBuffer incoming_plaintext, outgoing_plaintext;

  // If cipher_suite != NULL, then we send
  // records encrypted.
  const DskTlsCipherSuite *cipher_suite;
  void *cipher_suite_read_instance;
  void *cipher_suite_write_instance;

  DskError *failed_error;
};

DskTlsConnection *dsk_tls_connection_new (DskStream     *underlying,
                                          bool           is_server,
                                          DskTlsContext *context,
                                          DskError     **error);

