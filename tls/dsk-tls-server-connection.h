
typedef enum DskTlsServerConnectionState
{
  DSK_TLS_SERVER_CONNECTION_START,
  DSK_TLS_SERVER_CONNECTION_RECEIVED_CLIENT_HELLO,
  DSK_TLS_SERVER_CONNECTION_SELECTING_PSK,
  DSK_TLS_SERVER_CONNECTION_NEGOTIATED,
  DSK_TLS_SERVER_CONNECTION_WAIT_END_OF_EARLY_DATA,
  DSK_TLS_SERVER_CONNECTION_WAIT_FLIGHT2,
  DSK_TLS_SERVER_CONNECTION_WAIT_CERT,
  DSK_TLS_SERVER_CONNECTION_WAIT_CERT_VERIFY,
  DSK_TLS_SERVER_CONNECTION_WAIT_FINISHED,
  DSK_TLS_SERVER_CONNECTION_CONNECTED,
  DSK_TLS_SERVER_CONNECTION_FAILED,
} DskTlsServerConnectionState;

typedef struct DskTlsServerConnectionClass DskTlsServerConnectionClass;
typedef struct DskTlsServerConnection DskTlsServerConnection;
typedef struct DskTlsServerContext DskTlsServerContext;
typedef struct DskTlsServerHandshake DskTlsServerHandshake;

struct DskTlsServerConnectionClass
{
  DskStreamClass base_class;
};
struct DskTlsServerConnection
{
  DskStream base_instance;
  DskStream *underlying;
  DskTlsServerContext *context;
  DskHookTrap *write_trap;
  DskHookTrap *read_trap;

  // This object is deleted after handshaking.
  DskTlsServerHandshake *handshake;

  // State-Machine State.
  DskTlsServerConnectionState state;

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
};

DskTlsServerConnection *
dsk_tls_server_connection_new (DskStream           *underlying,
                               DskTlsServerContext *context,
                               DskError           **error);

//
// Information used only when handshaking.
//
// All memory allocated here should come out
// of DskTlsHandshakeNegotiation.mem_pool.
//
// The final result of handshaking must be stored
// in the DskTlsConnection since the entire handshake
// will be freed once handshaking is complete.
//
struct DskTlsServerHandshake
{
  DskTlsHandshakeMessage *first_handshake;
  DskTlsHandshakeMessage *last_handshake;
  DskMemPool mem_pool;
  DskTlsServerConnection *conn;

  const char *server_hostname;

  unsigned must_send_hello_retry_request : 1;
  unsigned sent_hello_retry_request : 1;
  unsigned allow_early_data : 1;
  unsigned request_client_certificate : 1;
  unsigned received_hello_retry_request : 1;

  const DskTlsCipherSuite *cipher_suite;
  DskTlsKeySchedule *key_schedule;
  size_t shared_key_length;
  uint8_t *shared_key;

  void *transcript_hash_instance;
  DskTlsHandshakeMessageType transcript_hash_last_message_type;

  //
  // Helpers for breaking up the construction of a
  // handshake message.
  //
  DskTlsHandshakeMessage  *received_handshake;

  // Handshake currently begin constructed.
  DskTlsHandshakeMessage  *currently_under_construction;
  unsigned          n_extensions;
  DskTlsExtension **extensions;
  unsigned          max_extensions;

  uint8_t ks_result;
  DskTlsExtension_KeyShare *keyshare_response;
  DskTlsNamedGroup best_supported_group;

  // Handshakes as they are available.
  DskTlsHandshakeMessage  *client_hello;
  DskTlsHandshakeMessage  *server_hello;
  DskTlsHandshakeMessage  *old_client_hello;      // if HelloRetryRequest case
  DskTlsHandshakeMessage  *certificate_hs;
  DskTlsSignatureScheme certificate_scheme;
  DskTlsX509Certificate *certificate;
  DskTlsHandshakeMessage *cert_req_hs;
  DskTlsHandshakeMessage *cert_hs;
};

