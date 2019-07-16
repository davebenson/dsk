//
// A Base-Class for DskTlsServerConnection
// and DskTlsClientConnection that handles the record layer.
//
// Users should never use any of this directly- this
// is just an implementation shortcut.
//
// DskTlsServerConnection and DskTlsClientConnection
// should provide all the public methods for users.
// 
typedef struct DskTlsBaseConnectionClass DskTlsBaseConnectionClass;
typedef struct DskTlsBaseConnection DskTlsBaseConnection;

typedef enum
{
  DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_OK,
  DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED,
  DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_SUSPEND
} DskTlsBaseHandshakeMessageResponseCode;

#define DSK_TLS_BASE_HANDSHAKE_MEMBERS(connection_type) \
  DskMemPool mem_pool; \
  connection_type *conn
  

typedef enum
{
  DSK_TLS_BASE_HMS_INIT,
  DSK_TLS_BASE_HMS_HEADER,
  DSK_TLS_BASE_HMS_GATHERING_BODY,
  DSK_TLS_BASE_HMS_SUSPENDED,
} DskTlsBaseHandshakeMessageState;

struct DskTlsBaseConnectionClass
{
  DskStreamClass base_class;

  DskTlsBaseHandshakeMessageResponseCode (*handle_handshake_message)
       (DskTlsBaseConnection *connection,
        DskTlsHandshakeMessage *message,
        DskError **error);
  void (*handle_application_data)
       (DskTlsBaseConnection *connection,
        DskTlsHandshakeMessage *message,
        DskError **error);
  void (*fatal_alert_received)
       (DskTlsBaseConnection *connection,
        DskTlsAlertDescription description);
  bool (*warning_alert_received)
       (DskTlsBaseConnection *connection,
        DskTlsAlertDescription description,
        DskError **error);
};
extern DskTlsBaseConnectionClass dsk_tls_base_connection_class;
struct DskTlsBaseConnection
{
  DskStream base_instance;
  DskStream *underlying;
  DskHookTrap *write_trap;
  DskHookTrap *read_trap;

  unsigned is_suspended : 1;

  void *handshake;

  //
  // Incoming/Outgoing raw data, and 
  //
  DskBuffer incoming_raw, outgoing_raw;
  DskFlatBuffer incoming_plaintext, outgoing_plaintext;

  //
  // Decoding the stream of Handshake messages,
  // which may be fragmented or combined.
  //
  DskTlsBaseHandshakeMessageState handshake_message_state;
  uint8_t hm_header[4];
  uint8_t hm_header_length;
  unsigned hm_length;
  unsigned cur_hm_length;
  uint8_t *hm_data;

  // If cipher_suite != NULL, then we send
  // records encrypted.
  const DskTlsCipherSuite *cipher_suite;
  void *cipher_suite_read_instance;
  void *cipher_suite_write_instance;
};

void dsk_tls_base_connection_resume      (DskTlsBaseConnection *connection);
void dsk_tls_base_connection_resume_fail (DskTlsBaseConnection *connection,
                                          DskError            **error);

