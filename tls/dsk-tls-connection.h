typedef struct DskTlsConnection DskTlsConnection;
typedef enum DskTlsConnectionState
{
  DSK_TLS_CONNECTION_CLIENT_START = 0x100,
  DSK_TLS_CONNECTION_CLIENT_DOING_PSK_LOOKUP,
  DSK_TLS_CONNECTION_CLIENT_WAIT_SERVER_HELLO,
  DSK_TLS_CONNECTION_CLIENT_WAIT_ENCRYPTED_EXTENSIONS,
  DSK_TLS_CONNECTION_CLIENT_WAIT_CERT_CR,
  DSK_TLS_CONNECTION_CLIENT_WAIT_CERT,
  DSK_TLS_CONNECTION_CLIENT_WAIT_CV,
  DSK_TLS_CONNECTION_CLIENT_WAIT_FINISHED,
  DSK_TLS_CONNECTION_CLIENT_CONNECTED,
  DSK_TLS_CONNECTION_CLIENT_FAILED,

  DSK_TLS_CONNECTION_SERVER_START = 0x200,
  DSK_TLS_CONNECTION_SERVER_RECEIVED_CLIENT_HELLO,
  DSK_TLS_CONNECTION_SERVER_SELECTING_PSK,
  DSK_TLS_CONNECTION_SERVER_NEGOTIATED,
  DSK_TLS_CONNECTION_SERVER_WAIT_END_OF_EARLY_DATA,
  DSK_TLS_CONNECTION_SERVER_WAIT_FLIGHT2,
  DSK_TLS_CONNECTION_SERVER_WAIT_CERT,
  DSK_TLS_CONNECTION_SERVER_WAIT_CERT_VERIFY,
  DSK_TLS_CONNECTION_SERVER_WAIT_FINISHED,
  DSK_TLS_CONNECTION_SERVER_CONNECTED,
  DSK_TLS_CONNECTION_SERVER_FAILED,
} DskTlsConnectionState;


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

