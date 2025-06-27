
typedef enum
{
  DSK_TLS_CLIENT_CONNECTION_START,
  DSK_TLS_CLIENT_CONNECTION_FINDING_PSKS,
  DSK_TLS_CLIENT_CONNECTION_DONE_FINDING_PSKS,
  DSK_TLS_CLIENT_CONNECTION_WAIT_SERVER_HELLO,
  DSK_TLS_CLIENT_CONNECTION_WAIT_ENCRYPTED_EXTENSIONS,
  DSK_TLS_CLIENT_CONNECTION_WAIT_CERT_CR,
  DSK_TLS_CLIENT_CONNECTION_FINDING_CLIENT_CERT,
  DSK_TLS_CLIENT_CONNECTION_DONE_FINDING_CLIENT_CERT,
  DSK_TLS_CLIENT_CONNECTION_WAIT_CERT,
  DSK_TLS_CLIENT_CONNECTION_WAIT_CERT_VALIDATE,
  DSK_TLS_CLIENT_CONNECTION_CERT_VALIDATE_SUCCEEDED,
  DSK_TLS_CLIENT_CONNECTION_CERT_VALIDATE_FAILED,
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

  // State-Machine State.
  DskTlsClientConnectionState state;

  // Negotiated state.
  DskTlsProtocolVersion version;

  DskTlsClientContext *context;
};
extern DskTlsClientConnectionClass dsk_tls_client_connection_class;

DskTlsClientConnection *dsk_tls_client_connection_new (DskStream     *underlying,
                                                       DskTlsClientContext *context,
                                                       DskError     **error);

//
//                    Function to lookup PSKs
//

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
  uint64_t expiration;          // unixtime
  size_t binder_length;
  const uint8_t *binder_data;
  size_t identity_length;
  const uint8_t *identity_data;
  size_t state_length;
  const uint8_t *state_data;
  uint32_t session_create_time;
  uint32_t ticket_age_add;
} DskTlsClientSessionInfo;
  
typedef void (*DskTlsClientStoreSessionFunc) (DskTlsClientHandshake *handshake,
                                              const DskTlsClientSessionInfo *session,
                                              void *client_store_session_data);

typedef void (*DskTlsClientLookupSessionsFunc)(DskTlsClientConnection *conn,
                                              void *client_lookup_session_data);
void dsk_tls_client_connection_found_sessions (DskTlsClientConnection *conn,
                                              size_t n_sessions_found,
                                              const DskTlsClientSessionInfo *sessions,
                                              const uint8_t *state_length);
void dsk_tls_client_connection_session_not_found  (DskTlsClientConnection *conn);

//
//                    Function to lookup Certificates
//

typedef struct {
  DskTlsClientConnection *conn;
  DskTlsClientHandshake *handshake;
  unsigned n_oid_filters;
  DskTlsOIDFilter *oid_filters;
  unsigned n_certificate_authorities;
  DskTlsX509DistinguishedName *certificate_authorities;
  unsigned n_schemes;
  const DskTlsSignatureScheme *schemes;
} DskTlsClientCertificateLookup;
typedef void (*DskTlsClientCertificateLookupFunc) (DskTlsClientCertificateLookup *info,
                                                   void *lookup_data);
typedef struct {
  size_t ref_count;
  size_t length;
  DskTlsCertificateEntry *chain;
  DskTlsKeyPair *key_pair;
} DskTlsCertChain;
DskTlsCertChain *dsk_tls_cert_chain_new (size_t n_certs,
                                         DskTlsX509Certificate **certs,
                                         DskTlsKeyPair *key_pair);
DskTlsCertChain *dsk_tls_cert_chain_ref (DskTlsCertChain *);
void             dsk_tls_cert_chain_unref (DskTlsCertChain *);

void dsk_tls_handshake_client_certificate_found (DskTlsClientHandshake *handshake,
                                                 DskTlsCertChain *chain);
void dsk_tls_handshake_client_certificate_not_found (DskTlsClientHandshake *handshake);

typedef struct {
  unsigned ok : 1;

  unsigned cert_chain_found : 1;
  unsigned in_time_range : 1;
  unsigned self_signed : 1;

  // certificate chain, if found
  unsigned n_certs;
  DskTlsCertificateEntry *certs;

  unsigned n_db_certs;
  DskTlsX509Certificate **db_certs;

  //
  // number of certificates that came from the
  // server in a Certificate message.
  //
  //     1 <= n_certs_from_server <= Certificate.entries.length
  //
  unsigned n_certs_from_server;

  // cert from server
  DskTlsX509Certificate *cert;

  DskTlsClientConnection *connection;
} DskTlsClientVerifyServerCertInfo;
typedef void (*DskTlsClientVerifyServerCertFunc)
                            (DskTlsClientVerifyServerCertInfo *info,
                             void *cert_verify_data);

void dsk_tls_client_connection_server_cert_notify_ok
                         (DskTlsClientConnection *conn);
void dsk_tls_client_connection_server_cert_notify_error
                         (DskTlsClientConnection *conn,
                          DskError *error);


//
//               Construct an immutable TlsClientContext
//

typedef bool (*DskTlsClientWarnHandler) (DskTlsClientConnection  *conn, 
                                         DskTlsAlertDescription   description,
                                         void                    *warn_data,
                                         DskError               **error);
typedef void (*DskTlsClientFatalHandler)(DskTlsClientConnection  *conn, 
                                         DskTlsAlertDescription   description,
                                         void                    *warn_data);

typedef struct DskTlsClientContextOptions DskTlsClientContextOptions;
struct DskTlsClientContextOptions
{
  size_t n_certificates;
  DskTlsKeyPair **certificates;

  size_t n_application_layer_protocols;
  const char **application_layer_protocols;
  bool application_layer_protocol_negotiation_required;

  bool support_early_data;              // requires Pre-Shared Key

  // A comma-sep list of key-shares whose
  // public/private keys should be computed
  // in the initial ClientHello.
  const char *offered_key_share_groups;

  DskTlsClientLookupSessionsFunc lookup_sessions_func;
  void *lookup_sessions_data;

  DskTlsCertDatabase *cacert_database;
  const char *cacert_file;
  const char *cacert_dir;

  size_t n_certificate_authorities;
  DskTlsX509DistinguishedName *certificate_authorities;

  const char *server_name;

  bool allow_self_signed;
  DskTlsX509Certificate *pinned_cert;

  DskTlsClientVerifyServerCertFunc verify_server_cert_func;
  void *verify_server_cert_data;

  DskTlsClientWarnHandler client_warn_handler;
  void *client_warn_data;
  DskDestroyNotify client_warn_destroy;

  DskTlsClientFatalHandler fatal_handler;
  void *fatal_handler_data;
  DskDestroyNotify fatal_handler_destroy;
};
#define DSK_TLS_CLIENT_CONTEXT_OPTIONS_INIT (DskTlsClientContextOptions){ \
        .n_certificates = 0, \
}

DskTlsClientContext *dsk_tls_client_context_new   (DskTlsClientContextOptions  *options,
                                                   DskError                   **error);
DskTlsClientContext *dsk_tls_client_context_ref   (DskTlsClientContext         *context);
void                 dsk_tls_client_context_unref (DskTlsClientContext         *context);


//
//            A public-private key-pair.
//
// The private key is never transmitted.
//
typedef struct DskTlsPublicPrivateKeyShare DskTlsPublicPrivateKeyShare;
struct DskTlsPublicPrivateKeyShare
{
  const DskTlsKeyShareMethod *method;
  uint8_t *private_key;
  uint8_t *public_key;
};

//
//       TlsClientHandshake - negotiating the crypto parameters
//
struct DskTlsClientHandshake
{
  DskTlsBaseHandshake base;

  DskRand *rng;
  DskError *tmp_error;
  DskBuffer *early_data;

  unsigned allow_early_data : 1;
  unsigned received_hello_retry_request : 1;
  unsigned doing_client_cert_lookup : 1;
  unsigned testing_cert : 1;
  unsigned invoking_user_handler : 1;

  unsigned psk_allow_wo_dh : 1;
  unsigned psk_allow_w_dh : 1;

  //
  // In theory, the client could present multiple key-shares.
  // In practice, most will just emit X25519.
  //
  unsigned n_key_shares;
  DskTlsPublicPrivateKeyShare *key_shares;
  int selected_key_share;

  DskTlsKeySchedule *key_schedule;
  size_t shared_key_length;
  uint8_t *shared_key;

  // PSKs shared, if client.
  unsigned n_psks;
  DskTlsClientSessionInfo *psks;

  // Server name.  This is the name the server sent us,
  // not the name we (optionally) requested (in the ClientHello message).
  // (Sent in the EncryptedExtensions message).
  const char *server_name;

  // Was a Pre-Shared Key successfully negotiated?
  // The other fields should only be used of has_psk is true.
  const DskTlsOfferedPresharedKeys *offered_psks;
  bool has_psk;
  DskTlsPSKKeyExchangeMode psk_key_exchange_mode;
  const DskTlsPresharedKeyIdentity *psk_identity;
  int psk_index; // index in PreSharedKey extension of ClientHello

  // Handshake currently begin constructed.
  unsigned          n_extensions;
  DskTlsExtension **extensions;
  unsigned          max_extensions;

  DskTlsExtension_KeyShare *keyshare_response;

  // Handshakes as they are available.
  DskTlsHandshakeMessage  *client_hello;
  DskTlsHandshakeMessage  *server_hello;
  DskTlsHandshakeMessage  *old_client_hello;      // if HelloRetryRequest case
  DskTlsHandshakeMessage  *old_server_hello;      // if HelloRetryRequest case
  DskTlsHandshakeMessage  *certificate_hs;
  DskTlsHandshakeMessage *cert_req_hs;
  DskTlsHandshakeMessage *cert_hs;

  // If we are offering a cert in response to a CertificateRequest message.
  DskTlsCertChain *client_certificate;
};

