
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

  DskTlsClientFindPSKsFunc client_find_psks;
  void * client_find_psks_data;

  DskTlsServerSelectPSKFunc server_select_psk;
  void * server_select_psk_data;
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
  DskTlsHandshakeMessage *first_handshake;
  DskTlsHandshakeMessage *last_handshake;
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

  // PSKs shared, if client.
  unsigned n_psks;
  DskTlsPresharedKeyInfo *psks;

  // Was a Pre-Shared Key successfully negotiated?
  // The other fields should only be used of has_psk is true.
  const DskTlsOfferedPresharedKeys *offered_psks;
  bool has_psk;
  DskTlsPSKKeyExchangeMode psk_key_exchange_mode;
  const DskTlsPresharedKeyIdentity *psk_identity;
  int psk_index; // index in PreSharedKey extension of ClientHello

  void *transcript_hash_instance;

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
  DskTlsHandshakeMessage  *certificate;
  DskTlsSignatureScheme certificate_scheme;
};

bool dsk_tls_handshake_serialize (DskTlsHandshakeNegotiation *hs_info,
                                  DskTlsHandshakeMessage     *message,
                                  DskError                  **error);
void dsk_tls_record_layer_send_handshake (DskTlsHandshakeNegotiation *hs_info,
                                          unsigned msg_len, const uint8_t *msg_data);


void dsk_tls_connection_fail (DskTlsConnection *connection,
                              DskError        **error);
