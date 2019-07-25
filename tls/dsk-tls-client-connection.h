
typedef enum
{
  DSK_TLS_CLIENT_CONNECTION_START,
  DSK_TLS_CLIENT_CONNECTION_DOING_PSK_LOOKUP,
  DSK_TLS_CLIENT_CONNECTION_WAIT_SERVER_HELLO,
  DSK_TLS_CLIENT_CONNECTION_WAIT_ENCRYPTED_EXTENSIONS,
  DSK_TLS_CLIENT_CONNECTION_WAIT_CERT_CR,
  DSK_TLS_CLIENT_CONNECTION_WAIT_CERT,
  DSK_TLS_CLIENT_CONNECTION_WAIT_CV,
  DSK_TLS_CLIENT_CONNECTION_WAIT_FINISHED,
  DSK_TLS_CLIENT_CONNECTION_CONNECTED,
  DSK_TLS_CLIENT_CONNECTION_FAILED,
} DskTlsClientConnectionState;

typedef struct DskTlsClientConnectionClass DskTlsClientConnectionClass;
typedef struct DskTlsClientConnection DskTlsClientConnection;
typedef struct DskTlsClientContext DskTlsClientContext;
typedef struct DskTlsClientHandshake DskTlsClientHandshake;
struct DskTlsClientConnectionClass
{
  DskTlsBaseConnectionClass base_class;
};
struct DskTlsClientConnection
{
  DskTlsBaseConnection base_instance;
  DskStream *underlying;
  DskTlsClientContext *context;
  DskHookTrap *write_trap;
  DskHookTrap *read_trap;

  // This object is deleted after handshaking.
  DskTlsClientHandshake *handshake; 

  // State-Machine State.
  DskTlsClientConnectionState state;

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

DskTlsClientConnection *dsk_tls_client_connection_new (DskStream     *underlying,
                                                       DskTlsClientContext *context,
                                                       DskError     **error);

//
// Called to store and then lookup which Pre-Shared-Keys are available;
// In order to avoid abbreviation or long names,
// we'll refer to Pre-Shared-Keys as Sessions.
//
// This function must eventually call either dsk_tls_handshake_client_found_psks()
// or dsk_tls_handshake_client_error_finding_psks().
//

// This function may succeed or fail;
// the implementation doesn't care.
typedef struct {
  size_t binder_length;
  const uint8_t *binder_data;
  size_t identity_length;
  const uint8_t *identity_data;
  size_t state_length;
  const uint8_t *state_data;
} DskTlsClientSessionInfo;
  
typedef void (*DskTlsClientStoreSessionFunc) (DskTlsClientHandshake *handshake,
                                              const DskTlsClientSessionInfo *session,
                                              void *client_store_session_data);

typedef void (*DskTlsClientLookupSessionsFunc)(DskTlsClientHandshake *handshake,
                                              void *client_lookup_session_data);
void dsk_tls_handshake_client_found_sessions (DskTlsClientHandshake *handshake,
                                              size_t n_sessions_found,
                                              const DskTlsClientSessionInfo *sessions,
                                              const uint8_t *state_length);
void dsk_tls_handshake_client_session_not_found  (DskTlsClientHandshake *handshake);

typedef struct DskTlsClientContextOptions DskTlsClientContextOptions;
struct DskTlsClientContextOptions
{
  size_t n_certificates;
  DskTlsKeyPair **certificates;

  size_t n_application_layer_protocols;
  const char **application_layer_protocols;
  bool application_layer_protocol_negotiation_required;

  // A comma-sep list of key-shares whose
  // public/private keys should be computed
  // in the initial ClientHello.
  const char *offered_key_share_groups;

  DskTlsClientLookupSessionsFunc lookup_sessions_func;
  void *lookup_sessions_data;
};


DskTlsClientContext *dsk_tls_client_context_new   (DskTlsClientContextOptions  *options,
                                                   DskError                   **error);
DskTlsClientContext *dsk_tls_client_context_ref   (DskTlsClientContext         *context);
void                 dsk_tls_client_context_unref (DskTlsClientContext         *context);


typedef struct DskTlsPublicPrivateKeyShare DskTlsPublicPrivateKeyShare;
struct DskTlsPublicPrivateKeyShare
{
  const DskTlsKeyShareMethod *method;
  uint8_t *private_key;
  uint8_t *public_key;
};
struct DskTlsClientHandshake
{
  DskTlsHandshakeMessage *first_handshake;
  DskTlsHandshakeMessage *last_handshake;
  DskMemPool mem_pool;
  DskTlsClientConnection *conn;

  unsigned allow_early_data : 1;
  unsigned received_hello_retry_request : 1;

  //
  // In theory, the client could present multiple key-shares.
  // In practice, most will just emit X25519.
  //
  unsigned n_key_shares;
  DskTlsPublicPrivateKeyShare *key_shares;
  int selected_key_share;

  const DskTlsCipherSuite *cipher_suite;
  DskTlsKeySchedule *key_schedule;
  size_t shared_key_length;
  uint8_t *shared_key;

  // PSKs shared, if client.
  unsigned n_psks;
  DskTlsClientSessionInfo *psks;

  // Was a Pre-Shared Key successfully negotiated?
  // The other fields should only be used of has_psk is true.
  const DskTlsOfferedPresharedKeys *offered_psks;
  bool has_psk;
  DskTlsPSKKeyExchangeMode psk_key_exchange_mode;
  const DskTlsPresharedKeyIdentity *psk_identity;
  int psk_index; // index in PreSharedKey extension of ClientHello

  void *transcript_hash_instance;
  DskTlsHandshakeMessageType transcript_hash_last_message_type;

  //
  // Used transiently to handle handshakes.
  //
  DskTlsHandshakeMessage  *received_handshake;

  // Handshake currently begin constructed.
  DskTlsHandshakeMessage  *currently_under_construction;
  unsigned          n_extensions;
  DskTlsExtension **extensions;
  unsigned          max_extensions;

  DskTlsExtension_KeyShare *keyshare_response;

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

