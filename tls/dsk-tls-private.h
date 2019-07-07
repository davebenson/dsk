
struct DskTlsContext
{
  unsigned ref_count;

  unsigned server_allow_pre_shared_keys;
  //bool server_generate_psk_post_handshake;

  size_t n_alps;
  const char **alps;
  bool alpn_required;

  size_t n_certificates;
  DskTlsKeyPair **certificates;

  DskTlsClientFindPSKsFunc client_find_psks;
  void * client_find_psks_data;

  DskTlsServerSelectPSKFunc server_select_psk;
  void * server_select_psk_data;

  bool always_request_client_certificate;

  DskTlsClientStoreSessionFunc client_store_session;
  void *client_store_session_data;
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
  const DskTlsKeyShareMethod *method;
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

  unsigned must_send_hello_retry_request : 1;
  unsigned sent_hello_retry_request : 1;
  unsigned allow_early_data : 1;
  unsigned request_client_certificate : 1;
  unsigned received_hello_retry_request : 1;

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

  const DskTlsCipherSuite *cipher_suite;
  DskTlsKeySchedule *key_schedule;
  size_t shared_key_length;
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

bool dsk_tls_handshake_serialize (DskTlsHandshakeNegotiation *hs_info,
                                  DskTlsHandshakeMessage     *message,
                                  DskError                  **error);
void dsk_tls_record_layer_send_handshake (DskTlsHandshakeNegotiation *hs_info,
                                          unsigned msg_len, const uint8_t *msg_data);


void dsk_tls_connection_fail (DskTlsConnection *connection,
                              DskError        **error);

#define DSK_ERROR_SET_TLS(error, level_short, description_short)           \
  dsk_error_set_tls_alert((error),                                         \
                          DSK_TLS_ALERT_LEVEL_##level_short,               \
                          DSK_TLS_ALERT_DESCRIPTION_##description_short)
