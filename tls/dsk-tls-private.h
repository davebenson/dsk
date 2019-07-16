
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

extern size_t dsk_tls_n_named_groups;
extern DskTlsNamedGroup dsk_tls_named_groups[];

extern size_t dsk_tls_n_signature_schemes;
extern DskTlsSignatureScheme dsk_tls_signature_schemes[];

extern size_t dsk_tls_n_cipher_suite_codes;
extern DskTlsCipherSuiteCode dsk_tls_cipher_suite_codes[];

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


DskIOResult dsk_tls_base_connection_read        (DskStream      *stream,
                                                 unsigned        max_len,
                                                 void           *data_out,
                                                 unsigned       *bytes_read_out,
                                                 DskError      **error);
DskIOResult dsk_tls_base_connection_read_buffer (DskStream      *stream,
                                                 DskBuffer      *read_buffer,
                                                 DskError      **error);

/* not always implemented */
void        dsk_tls_base_connection_shutdown_read(DskStream      *stream);
DskIOResult dsk_tls_base_connection_write        (DskStream      *stream,
                                                  unsigned        max_len,
                                                  const void     *data_out,
                                                  unsigned       *n_written_out,
                                                  DskError      **error);
DskIOResult dsk_tls_base_connection_write_buffer (DskStream      *stream,
                                                  DskBuffer      *write_buffer,
                                                  DskError      **error);

/* not always implemented */
void        dsk_tls_base_connection_shutdown_write(DskStream   *stream);

bool dsk_tls_base_connection_init_underlying (DskTlsBaseConnection *conn,
                                              DskStream *underlying,
                                              DskError **error);
#define DSK_TLS_BASE_CONNECTION_DEFINE_CLASS(clss, lc_clss)   \
  {                                                           \
    {                                                         \
      DSK_OBJECT_CLASS_DEFINE(                                \
        clss,                                                 \
        &dsk_tls_base_connection_class,                       \
        NULL,                                                 \
        lc_clss##_finalize                                    \
      ),                                                      \
      dsk_tls_base_connection_read,                           \
      dsk_tls_base_connection_read_buffer,                    \
      dsk_tls_base_connection_shutdown_read,                  \
      dsk_tls_base_connection_write,                          \
      dsk_tls_base_connection_write_buffer,                   \
      dsk_tls_base_connection_shutdown_write,                 \
    },                                                        \
    lc_clss##_handle_handshake_message,                       \
    lc_clss##_handle_application_data,                        \
    lc_clss##_fatal_alert_received,                           \
    lc_clss##_warning_alert_received                          \
  }
