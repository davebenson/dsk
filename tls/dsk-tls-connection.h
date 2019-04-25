
typedef enum DskTlsConnectionState
{
  DSK_TLS_CONNECTION_CLIENT_START = 0x100,
  DSK_TLS_CONNECTION_CLIENT_WAIT_SERVER_HELLO,
  DSK_TLS_CONNECTION_CLIENT_WAIT_ENCRYPTED_EXTENSIONS,
  DSK_TLS_CONNECTION_CLIENT_WAIT_CERT_CR,
  DSK_TLS_CONNECTION_CLIENT_WAIT_CERT,
  DSK_TLS_CONNECTION_CLIENT_WAIT_CV,
  DSK_TLS_CONNECTION_CLIENT_WAIT_FINISHED,
  DSK_TLS_CONNECTION_CLIENT_CONNECTED,
  DSK_TLS_CONNECTION_CLIENT_FAILED,

  DSK_TLS_CONNECTION_SERVER_START = 0x200,
  DSK_TLS_CONNECTION_SERVER_RECEIVED_CLIENT_HELLO,
  DSK_TLS_CONNECTION_SERVER_NEGOTIATED,
  DSK_TLS_CONNECTION_SERVER_WAIT_END_OF_EARLY_DATA,
  DSK_TLS_CONNECTION_SERVER_WAIT_FLIGHT2,
  DSK_TLS_CONNECTION_SERVER_WAIT_CERT,
  DSK_TLS_CONNECTION_SERVER_WAIT_CERT_VERIFY,
  DSK_TLS_CONNECTION_SERVER_WAIT_FINISHED,
  DSK_TLS_CONNECTION_SERVER_CONNECTED,
  DSK_TLS_CONNECTION_SERVER_FAILED,
} DskTlsConnectionState;

#define DSK_TLS_CONNECTION_STATE_IS_CLIENT(st)   (((st) >> 8) == 1)
#define DSK_TLS_CONNECTION_STATE_IS_SERVER(st)   (((st) >> 8) == 2)

struct DskTlsConnectionClass
{
  DskStreamClass base_class;
};
struct DskTlsConnection
{
  DskStream base_instance;
  DskStream *underlying;
  DskTlsContext *context;
  DskTlsConnectionState state;
  DskBuffer incoming_raw, outgoing_raw;
  DskBuffer incoming_plaintext, outgoing_plaintext;
};

static void
client_hello_to_buffer (DskTlsContext *context,
                        DskBuffer     *out)
{
  DskTlsHandshake shake;
  shake.type = DSK_TLS_HANDSHAKE_TYPE_CLIENT_HELLO;
  shake.client_hello.legacy_version = DSK_TLS_HANDSHAKE_CLIENT_HELLO_LEGACY_VERSION;
  gen_random (32, shake.client_hello.random);
  shake.client_hello.legacy_session_id_length = 0;
  shake.client_hello.legacy_session_id = NULL;
  shake.client_hello.n_cipher_suites = ...
  shake.client_hello.cipher_suites = ...
  shake.client_hello.n_compression_methods = ...
  shake.client_hello.compression_methods = ...


    struct {
      uint16_t legacy_version;
      uint8_t random[32];
      unsigned legacy_session_id_length;
      uint8_t *legacy_session_id;
      unsigned n_cipher_suites;
      DskTlsCipherSuite *cipher_suites;
      unsigned n_compression_methods;   /* legacy */
      uint8_t *compression_methods;     /* legacy */
      unsigned n_extensions;
      DskTlsExtension **extensions;
    } client_hello;

DskTlsConnection *
dsk_tls_connection_new (DskTlsContext *context,
                        boolean        is_server,
                        DskStream     *underlying)
{
  DskTlsConnection *conn = dsk_object_new (&dsk_tls_connection_class);
  conn->underlying = dsk_object_ref (underlying);
  conn->context = dsk_object_ref (context);
  conn->state = is_server ? DSK_TLS_CONNECTION_SERVER_START
                          : DSK_TLS_CONNECTION_CLIENT_START;
  if (!is_server)
    {
      client_hello_to_buffer (context, &conn->outgoing_raw);
      ensure_has_write_hook (conn);
      conn->state = DSK_TLS_CONNECTION_CLIENT_WAIT_SERVER_HELLO;
    }
  return conn;
}
