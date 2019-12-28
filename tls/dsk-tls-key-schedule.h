//
// An implementation of TLS 1.3's Key Schedule
// specification.
//
// This implements RFC 8446 Section 7.1 Key Schedule
// and the cryptographic part of Section 4.4.4 Finished.
//

typedef struct DskTlsKeySchedule DskTlsKeySchedule;
struct DskTlsKeySchedule
{
  unsigned has_early_secrets : 1;
  unsigned has_handshake_secrets : 1;
  unsigned has_master_secrets : 1;
  unsigned has_server_finish_data : 1;
  unsigned has_client_finish_data : 1;
  unsigned has_resumption_secret : 1;

  DskTlsCipherSuite *cipher;

  //
  // Early Secrets (those derived before Key-Sharing)
  // (Only meaningful if has_early_secrets)
  //
  uint8_t *early_secret;
  uint8_t *binder_key;
  uint8_t *client_early_traffic_secret;
  uint8_t *early_exporter_master_secret;
  uint8_t *early_derived_secret;

  //
  // Key-Share-derived Secrets
  //
  uint8_t *handshake_secret;
  uint8_t *client_handshake_traffic_secret;
  uint8_t *server_handshake_traffic_secret;
  uint8_t *handshake_derived_secret;

  //
  // Finished Secrets
  //
  uint8_t *server_verify_data;  // for the server's Finished
  uint8_t *client_verify_data;  // for the client's Finished

  //
  // Master Secrets
  //
  uint8_t *master_secret;
  uint8_t *client_application_traffic_secret;
  uint8_t *server_application_traffic_secret;
  uint8_t *exporter_master_secret;
  uint8_t *resumption_master_secret;

  //
  // Traffic Keys
  //
  uint8_t *server_handshake_write_key;
  uint8_t *server_handshake_write_iv;
  uint8_t *client_handshake_write_key;
  uint8_t *client_handshake_write_iv;
  uint8_t *server_application_write_key;
  uint8_t *server_application_write_iv;
  uint8_t *client_application_write_key;
  uint8_t *client_application_write_iv;
};


DskTlsKeySchedule *dsk_tls_key_schedule_new (DskTlsCipherSuite *cipher);
void               dsk_tls_key_schedule_free  (DskTlsKeySchedule *schedule);

//
// The hashes below are "Transcript Hashes",
// which in most cases are just the hash of all
// handshake messages up to that point.
//
// The exception is sequences after a HelloRetryRequest.
// Please see RFD 8446 4.4.1 for correct Transcript-Hash
// computation after a HelloRetryRequest.
//
void
  dsk_tls_key_schedule_compute_early_secrets
                      (DskTlsKeySchedule *schedule,
                       bool               externally_shared,
                       const uint8_t     *client_hello_hash,
                       size_t             opt_pre_shared_key_size,
                       const uint8_t     *opt_pre_shared_key);
void
  dsk_tls_key_schedule_compute_handshake_secrets
                      (DskTlsKeySchedule *schedule,
                       const uint8_t     *server_hello_hash,
                       unsigned           key_share_len,
                       const uint8_t     *key_share);
void
  dsk_tls_key_schedule_compute_server_verify_data
                      (DskTlsKeySchedule *schedule,
                       const uint8_t     *s_cert_verify_hash);
void
  dsk_tls_key_schedule_compute_master_secrets 
                      (DskTlsKeySchedule *schedule,
                       const uint8_t     *s_finished_hash);

// c_cert_verify_hash is the transcript hash
// up to the client's CertificateVerify, which only
// exists when using client certs.  The last message
// reflected by c_cert_verify_hash is likely to be 
// the server's Finished message.
//
void
  dsk_tls_key_schedule_compute_client_verify_data
                      (DskTlsKeySchedule *schedule,
                       const uint8_t     *c_cert_verify_hash);

void
  dsk_tls_key_schedule_compute_resumption_secret
                      (DskTlsKeySchedule *schedule,
                       const uint8_t     *c_finished_hash);
