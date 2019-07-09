
////struct DskTlsContext
////{
////  unsigned ref_count;
////
////  unsigned server_allow_pre_shared_keys;
////  //bool server_generate_psk_post_handshake;
////
////  size_t n_alps;
////  const char **alps;
////  bool alpn_required;
////
////  size_t n_certificates;
////  DskTlsKeyPair **certificates;
////
////  DskTlsServerSelectPSKFunc server_select_psk;
////  void * server_select_psk_data;
////
////  bool always_request_client_certificate;
////};


bool dsk_tls_handshake_message_serialize (DskTlsHandshakeMessage     *message,
                                          DskMemPool                 *pool,
                                          DskError                  **error);
//void dsk_tls_record_layer_send_handshake (DskTlsHandshakeNegotiation *hs_info,
                                          //unsigned msg_len, const uint8_t *msg_data);



#define DSK_ERROR_SET_TLS(error, level_short, description_short)           \
  dsk_error_set_tls_alert((error),                                         \
                          DSK_TLS_ALERT_LEVEL_##level_short,               \
                          DSK_TLS_ALERT_DESCRIPTION_##description_short)

int dsk_tls_support_get_group_quality(DskTlsNamedGroup group);
