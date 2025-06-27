

typedef enum
{
  DSK_TLS13_CLIENT_STATE_START,
  DSK_TLS13_CLIENT_STATE_FINDING_PSKS,
  DSK_TLS13_CLIENT_STATE_DONE_FINDING_PSKS,
  DSK_TLS13_CLIENT_STATE_WAIT_SERVER_HELLO,
  DSK_TLS13_CLIENT_STATE_WAIT_ENCRYPTED_EXTENSIONS,
  DSK_TLS13_CLIENT_STATE_WAIT_CERT_CR,
  DSK_TLS13_CLIENT_STATE_FINDING_CLIENT_CERT,
  DSK_TLS13_CLIENT_STATE_DONE_FINDING_CLIENT_CERT,
  DSK_TLS13_CLIENT_STATE_WAIT_CERT,
  DSK_TLS13_CLIENT_STATE_WAIT_CERT_VALIDATE,
  DSK_TLS13_CLIENT_STATE_CERT_VALIDATE_SUCCEEDED,
  DSK_TLS13_CLIENT_STATE_CERT_VALIDATE_FAILED,
  DSK_TLS13_CLIENT_STATE_WAIT_CV,
  DSK_TLS13_CLIENT_STATE_WAIT_FINISHED,
  DSK_TLS13_CLIENT_STATE_CONNECTED,
  DSK_TLS13_CLIENT_STATE_FAILED,
} DskTls13ClientState;

struct DskTls13ClientHandshake
{
  DskMemPool mem_pool;
  DskTlsKeySchedule key_schedule;
  uint8_t is_client : 1;
  uint8_t is_suspended : 1;
  uint8_t state;                        /* DskTls13ClientState */
  DskTlsClientContext *context;

  DskTlsHandshakeMessage *first_message, *last_message;
};

bool dsk_tls13_client_handshake_parse_message (DskTls13ClientHandshake *hs_info,
                                               size_t message_len,
                                               const uint8_t *message,
                                               DskError **error);
