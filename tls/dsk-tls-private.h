
struct DskTlsContext
{

  unsigned server_allow_pre_shared_keys;
  //bool server_generate_psk_post_handshake;
};

void dsk_tls_connection_fail (DskTlsConnection *connection,
                              DskError        **error);
