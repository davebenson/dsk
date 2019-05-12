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

struct DskTlsConnectionHandshakeInfo
{
  DskTlsHandshake *first_handshake;
  DskTlsHandshake *last_handshake;
  DskMemPool mem_pool;
};

typedef struct DskTlsContext DskTlsContext;
typedef struct DskTlsConnectionClass DskTlsConnectionClass;
struct DskTlsConnectionClass
{
  DskStreamClass base_class;
};
typedef struct DskTlsConnection DskTlsConnection;
struct DskTlsConnection
{
  DskStream base_instance;
  DskStream *underlying;
  DskTlsContext *context;
  DskTlsConnectionState state;
  DskBuffer incoming_raw, outgoing_raw;
  DskBuffer incoming_plaintext, outgoing_plaintext;
};

