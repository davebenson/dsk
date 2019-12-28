
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

#define DSK_TLS_SERVER_CONNECTION(o) DSK_OBJECT_CAST(DskTlsServerConnection, o, &dsk_tls_server_connection_class)
extern DskTlsServerConnectionClass dsk_tls_server_connection_class;

struct DskTlsServerConnectionClass
{
  DskTlsBaseConnectionClass base_class;
};
struct DskTlsServerConnection
{
  DskTlsBaseConnection base_instance;
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

typedef struct DskTlsStreamListener DskTlsStreamListener;
struct DskTlsStreamListener
{
  DskStreamListener base_instance;

  DskStreamListener *underlying;
  DskTlsServerContext *context;
};

DskTlsStreamListener *
dsk_tls_stream_listener_new   (DskStreamListener   *underlying,
                               DskTlsServerContext *context,
                               DskError           **error);


//
// User-Callbacks for Context.
//
typedef void (*DskTlsServerSelectPSKFunc) (DskTlsServerHandshake *handshake,
                                           const DskTlsOfferedPresharedKeys *offered_keys,
                                           void *server_find_psk_data);
void dsk_tls_server_handshake_chose_psk   (DskTlsServerHandshake     *hs_info,
                                           DskTlsPSKKeyExchangeMode   exchange_mode,
                                           unsigned                   which_psk,
                                           const DskTlsCipherSuite   *cipher);
void dsk_tls_server_handshake_chose_no_psk(DskTlsServerHandshake     *hs_info);
void dsk_tls_server_handshake_error_choosing_psk (DskTlsServerHandshake *hs_info,
                                                  DskError              *error);

typedef bool (*DskTlsServerValidateCertFunc) (DskTlsServerHandshake *handshake,

                                           size_t n_certificates,
                                           const DskTlsCertificateEntry *certificates,
                                           void *server_validate_cert_data,
                                           DskError           **error);

//
// DskTlsServerContext.
//
// Sort-of a factory for connections.
//
typedef struct DskTlsServerContextOptions DskTlsServerContextOptions;
struct DskTlsServerContextOptions
{
  size_t n_certificates;
  DskTlsKeyPair **certificates;

  size_t n_application_layer_protocols;
  const char **application_layer_protocols;
  bool application_layer_protocol_negotiation_required;

  DskTlsServerSelectPSKFunc server_select_psk;
  void * server_select_psk_data;

  DskTlsServerValidateCertFunc validate_cert_func;
  void *validate_cert_data;
};





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

  const char *server_hostname;

  unsigned must_send_hello_retry_request : 1;
  unsigned sent_hello_retry_request : 1;
  unsigned receiving_early_data : 1;
  unsigned request_client_certificate : 1;
  unsigned received_hello_retry_request : 1;

  const DskTlsCipherSuite *cipher_suite;
  DskTlsKeySchedule *key_schedule;

  const DskTlsKeyShareMethod *key_share_method;
  uint8_t *public_key;
  uint8_t *private_key;
  uint8_t *shared_key;

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
  DskTlsKeyShareEntry *found_keyshare;
  bool has_best_supported_group;
  DskTlsNamedGroup best_supported_group;

  const DskTlsOfferedPresharedKeys *offered_psks;

  bool has_psk;
  DskTlsPSKKeyExchangeMode psk_key_exchange_mode;
  unsigned psk_index;           // in offered_psks

  // Handshakes as they are available.
  DskTlsHandshakeMessage  *client_hello;
  DskTlsHandshakeMessage  *server_hello;
  DskTlsHandshakeMessage  *old_client_hello;      // if HelloRetryRequest case
  DskTlsHandshakeMessage  *certificate_hs;
  DskTlsSignatureScheme certificate_scheme;
  DskTlsKeyPair *certificate_key_pair;
  DskTlsX509Certificate *certificate;
  DskTlsHandshakeMessage *cert_req_hs;
  DskTlsHandshakeMessage *cert_hs;
};
