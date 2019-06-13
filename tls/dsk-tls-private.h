
struct DskTlsContext
{
  unsigned ref_count;

  unsigned server_allow_pre_shared_keys;
  //bool server_generate_psk_post_handshake;

  size_t n_alps;
  const char **alps;
  bool alpn_required;

  size_t n_certificates;
  DskTlsCertificate **certificates;
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
typedef struct DskTlsPublicPrivateKeyShare DskTlsPublicPrivateKeyShare;
struct DskTlsPublicPrivateKeyShare
{
  DskTlsKeyShareMethod *method;
  uint8_t *private_key;
  uint8_t *public_key;
};
struct DskTlsHandshakeNegotiation
{
  DskTlsHandshake *first_handshake;
  DskTlsHandshake *last_handshake;
  DskMemPool mem_pool;
  DskTlsConnection *conn;

  const char *server_hostname;

  unsigned sent_hello_retry_request : 1;
  unsigned allow_early_data : 1;

  uint8_t ks_result;    // key-share negotiation result, or 0

  //
  // Only one key-share will be used in server mode;
  // client may offer multiple key-shares,
  // but that is probably rare.
  //
  unsigned n_key_shares;
  DskTlsPublicPrivateKeyShare *key_shares;
  int selected_key_share;

  // Set if ks_result==KS_RESULT_SUCCESS
  DskTlsKeyShareEntry *remote_key_share;

  // Set if ks_result==KS_RESULT_MUST_MAKE_RETRY_REQUEST
  DskTlsNamedGroup best_supported_group;

  DskTlsCipherSuite *cipher_suite;
  DskTlsKeySchedule *key_schedule;
  uint8_t *shared_key;

  // Was a Pre-Shared Key successfully negotiated?
  // The other fields should only be used of has_psk is true.
  bool has_psk;
  DskTlsExtension_PSKKeyExchangeMode psk_key_exchange_mode;
  const DskTlsPresharedKeyIdentity *psk_identity;
  int psk_index; // index in PreSharedKey extension of ClientHello

  void *transcript_hash_instance;

  //
  // Used transiently to handle handshakes.
  //
  DskTlsHandshake  *received_handshake;

  // Handshake currently begin constructed.
  DskTlsHandshake  *currently_under_construction;
  unsigned          n_extensions;
  DskTlsExtension **extensions;
  unsigned          max_extensions;

  DskTlsExtension_KeyShare *keyshare_response;

  // Handshakes as they are available.
  DskTlsHandshake  *client_hello;
  DskTlsHandshake  *server_hello;
  DskTlsHandshake  *certificate;
  DskTlsSignatureScheme certificate_scheme;
};


void dsk_tls_connection_fail (DskTlsConnection *connection,
                              DskError        **error);
