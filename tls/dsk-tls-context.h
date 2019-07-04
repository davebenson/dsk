
typedef struct DskTlsClientContextOptions DskTlsClientContextOptions;
typedef struct DskTlsServerContextOptions DskTlsServerContextOptions;
typedef struct DskTlsContextOptions DskTlsContextOptions;


typedef struct {
  size_t identity_length;
  const uint8_t *identity;

  size_t master_key_length;
  const uint8_t *master_key;
} DskTlsPresharedKeyInfo;

//
// Called on the client-side to lookup which Pre-Shared-Keys are available.
//
// This function must eventually call either dsk_tls_handshake_client_found_psks()
// or dsk_tls_handshake_client_error_finding_psks().
//
void dsk_tls_handshake_client_found_psks         (DskTlsHandshakeNegotiation *hs_info,
                                                  size_t                      n_psks,
                                                  DskTlsPresharedKeyInfo     *psks);
void dsk_tls_handshake_client_error_finding_psks (DskTlsHandshakeNegotiation *hs_info,
                                                  DskError                   *error);
typedef void (*DskTlsClientFindPSKsFunc) (DskTlsHandshakeNegotiation *handshake,
                                          void *client_find_psks_data);

//
// Called on the server-side to choose a client Pre-Shared-Key if any.
//
typedef void (*DskTlsServerSelectPSKFunc) (DskTlsHandshakeNegotiation *handshake,
                                           const DskTlsOfferedPresharedKeys *offered_keys,
                                           void *server_find_psk_data);
void dsk_tls_handshake_server_chose_psk          (DskTlsHandshakeNegotiation *hs_info,
                                                  DskTlsPSKKeyExchangeMode exchange_mode,
                                                  unsigned                    which_psk,
                                                  const DskTlsCipherSuite    *cipher);
void dsk_tls_handshake_server_chose_no_psk       (DskTlsHandshakeNegotiation *hs_info);
void dsk_tls_handshake_server_error_choosing_psk (DskTlsHandshakeNegotiation *hs_info,
                                                  DskError                    *error);

typedef void (*DskTlsServerFindCertificateFunc) (DskTlsHandshakeNegotiation *handshake,
                                                 void *server_find_certificate_data);
void dsk_tls_handshake_server_request_client_cert(DskTlsHandshakeNegotiation *handshake);
void dsk_tls_handshake_server_chose_certs        (DskTlsHandshakeNegotiation *hs_info,
                                                  ...);
void dsk_tls_handshake_server_error_choosing_certs(DskTlsHandshakeNegotiation *hs_info,
                                                  ...);

struct DskTlsContextOptions
{
  // A comma-sep list of groups
  // to NOT support.  The actual
  // list of groups will be found
  // by computing the difference
  // from the set of groups we literally support
  // (as in, have the code for) and
  // of course the peer must support the same group.
  const char *unsupported_groups;

  size_t n_certificates;
  DskTlsKeyPair **certificates;

  size_t n_application_layer_protocols;
  const char **application_layer_protocols;
  bool application_layer_protocol_negotiation_required;

  // Setup for server/client specific options.
  DskTlsServerContextOptions *server_options;
  DskTlsClientContextOptions *client_options;
};

struct DskTlsClientContextOptions
{
  // A comma-sep list of key-shares whose
  // public/private keys should be computed
  // before the initial handshake.
  const char *offered_key_share_groups;

  DskTlsClientFindPSKsFunc find_psks_func;
  void *find_psks_data;

};
struct DskTlsServerContextOptions
{
  bool allow_pre_shared_keys;

  DskTlsServerSelectPSKFunc select_psk;
  void * select_psk_data;

  DskTlsServerFindCertificateFunc find_certificate;
  void * find_certificate_data;

  bool always_request_client_certificate;
};




DskTlsContext    *dsk_tls_context_new   (DskTlsContextOptions     *options,
                                         DskError                **error);
DskTlsContext    *dsk_tls_context_ref   (DskTlsContext            *context);
void              dsk_tls_context_unref (DskTlsContext            *context);


