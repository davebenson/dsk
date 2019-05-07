#include "dsk-tls-connection.h"

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
  shake.client_hello.n_cipher_suites = DSK_N_ELEMENTS (std_cipher_suite);
  shake.client_hello.cipher_suites = std_cipher_suite;
  shake.client_hello.n_compression_methods = ...
  shake.client_hello.compression_methods = ...
  shake.client_hello.n_extensions = ...
  shake.client_hello.extensions = ...
  dsk_tls_handshake_serialize_to_buffer (&shake, out);
}

typedef enum
{
  FRAME_HEADER_OK,
  FRAME_HEADER_ERROR,
  FRAME_HEADER_TOO_SHORT
} FrameHeaderStatus;


static FrameHeaderStatus
check_frame_header (DskBuffer *buffer,
                    DskTlsRecordContentType content_type,
                    unsigned               *header_len_out,
                    unsigned               *fragment_len_out)
{
  if (buffer->size < 5)
    return FRAME_HEADER_TOO_SHORT;
  uint8_t header[5];
  dsk_buffer_peek (buffer, 5, header);
  
  switch (header[0])
    {
    case DSK_TLS_RECORD_CONTENT_TYPE_CHANGE_CIPHER_SPEC:
    case DSK_TLS_RECORD_CONTENT_TYPE_HANDSHAKE:
    case DSK_TLS_RECORD_CONTENT_TYPE_APPLICATION_DATA:
    case DSK_TLS_RECORD_CONTENT_TYPE_ALERT:
      break;
    case DSK_TLS_RECORD_CONTENT_TYPE_INVALID:
    default:
      return FRAME_HEADER_ERROR;
    }
  unsigned length = ((unsigned) buffer[3] << 8) | ((unsigned) buffer[4]);
  if (5 + length > buffer->size)
    return FRAME_HEADER_TOO_SHORT;
  if (length > (1<<14))
    return FRAME_HEADER_ERROR;                  // "must terminate with record_overflow alert" (5.1)
  *header_len_out = 5;
  *fragment_len_out = length;
}

static bool
handle_underlying_readable (DskStream *underlying,
                            DskTlsConnection *conn)
{
  assert (conn->underlying == underlying);

  dsk_stream_read_buffer (underlying, &conn->incoming_raw);
  switch (check_frame_header (&conn->incoming_raw, &content_type, &packet_length))
    {
    case FRAME_HEADER_OK:
      break;
    case FRAME_HEADER_ERROR:
      {
        dsk_tls_connection_fail (conn, "error parsing frame header");
        return;
      }
    case FRAME_HEADER_TOO_SHORT:
      return true;
    }

  switch (content_type)
    


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
