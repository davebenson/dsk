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
  DSK_TLS_BASE_RECORD_RESPONSE_OK,
  DSK_TLS_BASE_RECORD_RESPONSE_FAILED,
  DSK_TLS_BASE_RECORD_RESPONSE_SUSPEND
} DskTlsBaseRecordResponseCode;

struct DskTlsBaseConnectionClass
{
  DskStreamClass base_class;

  DskMemPool *(*peek_handshake_mempool)
       (DskTlsBaseConnection *connection);
  DskTlsBaseHandshakeMessageResponseCode (*handle_handshake_message)
       (DskTlsBaseConnection *connection,
        DskTlsHandshakeMessage *message,
        DskError **error);
  void (*handle_application_data)
       (DskTlsBaseConnection *connection,
        DskTlsHandshakeMessage *message,
        DskError **error);
  .... fatal error pre-handlign
  .... warning handling
};
struct DskTlsBaseConnection
{
  DskStream base_instance;
  DskStream *underlying;
  DskHookTrap *write_trap;
  DskHookTrap *read_trap;

  unsigned is_suspended : 1;

  //
  // Incoming/Outgoing raw data, and 
  //
  DskBuffer incoming_raw, outgoing_raw;
  DskFlatBuffer incoming_plaintext, outgoing_plaintext;

  // If cipher_suite != NULL, then we send
  // records encrypted.
  const DskTlsCipherSuite *cipher_suite;
  void *cipher_suite_read_instance;
  void *cipher_suite_write_instance;
};

void dsk_tls_base_connection_resume      (DskTlsBaseConnection *connection);
void dsk_tls_base_connection_resume_fail (DskTlsBaseConnection *connection,
                                          DskError            **error);
