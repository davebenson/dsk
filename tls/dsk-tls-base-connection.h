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

//
// On read:
//
//   This breaks the raw byte-oriented stream
//   into records.  The records may either contain
//   handshake messages or encrypted application data.
//
//   For handshaking, the handle_handshake_message() method
//   is invoked, which is different for client and server streams.
//   It returns a code indicating what to do:
//     * OK:  continue processing incoming records and messages.
//     * FAILED:  send fatal alert and shutdown connection.
//     * SUSPEND:  used to indicate that we are blocked on a user-callback.
//          Whoever returns this must arrange to call unsuspend() or
//          fail().
// 
//
typedef struct DskTlsBaseConnectionClass DskTlsBaseConnectionClass;
typedef struct DskTlsBaseConnection DskTlsBaseConnection;

typedef enum
{
  DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_OK,
  DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED,
  DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_SUSPEND
} DskTlsBaseHandshakeMessageResponseCode;

typedef struct DskTlsBaseHandshake DskTlsBaseHandshake;
struct DskTlsBaseHandshake
{
  DskMemPool mem_pool;
  DskTlsBaseConnection *conn;
  void *transcript_hash_instance;
  DskTlsHandshakeMessage *first_handshake;
  DskTlsHandshakeMessage *last_handshake;

  // TODO: need generic destruction list?
};

void dsk_tls_base_handshake_free (DskTlsBaseHandshake *hs);

typedef uint64_t (*DskTlsGetCurrentTimeFunc)(void *get_time_data);
uint64_t dsk_tls_get_current_time (void *get_time_data);

typedef enum
{
  DSK_TLS_BASE_HMS_INIT,
  DSK_TLS_BASE_HMS_HEADER,
  DSK_TLS_BASE_HMS_GATHERING_BODY,
  DSK_TLS_BASE_HMS_SUSPENDED,
} DskTlsBaseHandshakeMessageState;

#define DSK_TLS_BASE_CONNECTION(o) DSK_OBJECT_CAST(DskTlsBaseConnection, o, &dsk_tls_base_connection_class)
#define DSK_TLS_BASE_CONNECTION_GET_CLASS(o) DSK_OBJECT_CAST_GET_CLASS(DskTlsBaseConnection, o, &dsk_tls_base_connection_class)
struct DskTlsBaseConnectionClass
{
  DskStreamClass base_class;

  DskTlsBaseHandshakeMessageResponseCode (*handle_handshake_message)
       (DskTlsBaseConnection *connection,
        DskTlsHandshakeMessage *message,
        DskError **error);
  DskTlsBaseHandshakeMessageResponseCode (*handle_unsuspend)
       (DskTlsBaseConnection *connection,
        DskError **error);
  bool (*handle_application_data)
       (DskTlsBaseConnection *connection,
        size_t length,
        const uint8_t *data,
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

  unsigned expecting_eof : 1;

  void *handshake;

  //
  // Incoming/Outgoing raw data, and 
  //
  DskBuffer incoming_raw, outgoing_raw;
  DskBuffer incoming_plaintext, outgoing_plaintext;

  //
  // This buffer is used for the record fragment, and the decrypted.
  //
  size_t record_buffer_length;
  uint8_t *record_buffer;
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
  uint8_t *client_application_traffic_secret;   // client write key; server read key
  uint8_t *server_application_traffic_secret;   // server write key; client read key

  uint8_t *read_iv, *write_iv;
  uint64_t read_seqno, write_seqno;
};

void dsk_tls_base_connection_unsuspend   (DskTlsBaseConnection *connection);

bool dsk_tls_base_connection_init_underlying (DskTlsBaseConnection *,
                                              DskStream *underlying,
                                              DskError **error);

// Suspend in constructor.
void dsk_tls_base_connection_init_suspend (DskTlsBaseConnection *connection);

void dsk_tls_base_connection_set_max_fragment_length (DskTlsBaseConnection *conn,
                                                      unsigned              max_fragment_length);

void dsk_tls_base_connection_send_key_update (DskTlsBaseConnection *base,
                                              bool update_requested);

void dsk_tls_base_connection_send_handshake_msgdata (DskTlsBaseConnection *conn,
                                                     unsigned msg_length,
                                                     const uint8_t *msg_data);
bool dsk_tls_base_connection_send_handshake_msg     (DskTlsBaseConnection *conn,
                                                     DskTlsHandshakeMessage *msg,
                                                     DskError **error);

void
dsk_tls_base_connection_get_transcript_hash (DskTlsBaseConnection *base,
                                  uint8_t *hash_out);
// protected
//
// Send a fatal alert, if we are able to.
// And shutdown the connection (after the alert is sent).
// 
// If the connection is suspended (awaiting user callback response),
// you must still call unsuspend() [usually via a special helper method]
// to avoid a memory leak, but processing will not actually resume.
// The connection will remain in the same state (NOT the FAILED state),
// but will have the failed_while_suspended flag set.
// dsk_tls_base_connection_unsuspend() will clear the flag
// and move to the FAILED state.
// 
void dsk_tls_base_connection_fail (DskTlsBaseConnection *conn,
                                   DskError *error);


DskTlsHandshakeMessage *
dsk_tls_base_handshake_create_outgoing_handshake
                                           (DskTlsBaseHandshake       *hs_info,
                                            DskTlsHandshakeMessageType   type,
                                            unsigned                     max_extensions);

