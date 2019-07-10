#include "../dsk.h"
#include "dsk-tls-private.h"
#include <stdlib.h>
#include <string.h>

//
// Supported stuff.
//
static DskTlsSignatureScheme global_sig_schemes[] = {
  DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA256,
  DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA384,
  DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA512,
  DSK_TLS_SIGNATURE_SCHEME_ECDSA_SECP256R1_SHA256,
  DSK_TLS_SIGNATURE_SCHEME_ECDSA_SECP384R1_SHA384,
  DSK_TLS_SIGNATURE_SCHEME_ECDSA_SECP521R1_SHA512,
  DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA256,
  DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA384,
  DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA512,
  DSK_TLS_SIGNATURE_SCHEME_ED25519,
  DSK_TLS_SIGNATURE_SCHEME_ED448,
  DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_PSS_SHA256,
  DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_PSS_SHA384,
  DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_PSS_SHA512
};

//
// Allocate memory and objects from the handshke-pool.
//
#define HS_ALLOC(hs_info, type) \
  ((type *) dsk_mem_pool_alloc (&((hs_info)->mem_pool), sizeof(type)))
#define HS_ALLOC0(hs_info, type) \
  ((type *) dsk_mem_pool_alloc (&((hs_info)->mem_pool), sizeof(type)))
#define HS_ALLOC_ARRAY(hs_info, count, type) \
  ((type *) dsk_mem_pool_alloc (&((hs_info)->mem_pool), sizeof(type) * (count)))
#define HS_ALLOC_ARRAY0(hs_info, count, type) \
  ((type *) dsk_mem_pool_alloc (&((hs_info)->mem_pool), sizeof(type) * (count)))

static bool
handle_certificate_verify  (DskTlsConnection *conn,
                            DskTlsHandshakeMessage  *shake,
                            DskError        **error)
{
  if (conn->state != ...)
    {
      ...
    }
  ...
}
static bool
handle_finished            (DskTlsConnection *conn,
                            DskTlsHandshakeMessage  *shake,
                            DskError        **error)
{
  if (conn->state != ...)
    {
      ...
    }
  ...
}


static bool
handle_handshake_message (DskTlsConnection *conn,
                          DskTlsHandshakeMessage  *shake,
                          DskError        **error)
{
  switch (shake->type)
    {
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_CLIENT_HELLO:
      return handle_client_hello (conn, shake, error);
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_SERVER_HELLO:
      if (is_retry_request (shake))
        return handle_hello_retry_request (conn, shake, error);
      else
        return handle_server_hello (conn, shake, error);
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_NEW_SESSION_TICKET:
      return handle_new_session_ticket (conn, shake, error);
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_END_OF_EARLY_DATA:
      return handle_end_of_early_data (conn, shake, error);
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_ENCRYPTED_EXTENSIONS:
      return handle_encrypted_extensions (conn, shake, error);
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_CERTIFICATE:
      return handle_certificate (conn, shake, error);
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_CERTIFICATE_REQUEST:
      return handle_certificate_request (conn, shake, error);
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_CERTIFICATE_VERIFY:
      return handle_certificate_verify (conn, shake, error);
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_FINISHED:
      return handle_finished (conn, shake, error);
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_KEY_UPDATE:
      *error = ...
      return handle_certificate_key_update (conn, shake, error);
    default:
      assert(0);
    }
  return true;
}

static bool
handle_underlying_readable (DskStream *underlying,
                            DskTlsConnection *conn)
{
  RecordHeaderParseResult header_result;

  assert (conn->underlying == underlying);

  // Perform raw read.
  dsk_stream_read_buffer (underlying, &conn->incoming_raw);

try_parse_record_header:
  header_result = dsk_tls_parse_record_header (&conn->incoming_raw);
  switch (header_result.code)
    {
    case PARSE_RESULT_CODE_OK:
      break;
    case PARSE_RESULT_CODE_ERROR:
      {
        dsk_tls_connection_fail (conn, header_result.error);
        dsk_error_unref (header_result.error);
        return;
      }
    case PARSE_RESULT_CODE_TOO_SHORT:
      return true;
    }

  dsk_buffer_discard (&conn->incoming_raw, header_result.header_length);

  ensure_record_data_capacity (conn, header_result.payload_length);
  uint8_t *record = conn->records_plaintext + conn->records_plaintext_lenth;
  dsk_buffer_read (&conn->incoming_raw, header_result.payload_length, record);

  if (conn->cipher_suite_read_instance != NULL)
    {
      ...
    }
  else
    {
      conn->records_plaintext_lenth += header_result.payload_length;
    }

  const uint8_t *parse_at = conn->records_plaintext;
  const uint8_t *end_parse = conn->records_plaintext + conn->records_plaintext_lenth;
  switch (header_result.content_type)
    {
    case DSK_TLS_RECORD_CONTENT_TYPE_ALERT:
      {
        ...
        break;
      }
    case DSK_TLS_RECORD_CONTENT_TYPE_HANDSHAKE:
      {
        while (parse_at + 4 <= end_parse)
          {
            uint32_t len = dsk_uint24be_parse (parse_at + 1);
            if (parse_at + 4 + len > 0)
              {
                ... need more data
              }
            uint8_t *msgbuf = dsk_mem_pool_alloc (pool, len);
            memcpy (msgbuf, parse_at + 4, len);
            DskTlsHandshakeMessage *handshake = dsk_tls_handshake_parse (*parse_at, len, msgbuf, &pool, &error);
            if (handshake == NULL)
              {
                dsk_tls_connection_fail (conn, error);
                dsk_error_unref (error);
                return false;
              }
            else if (!handle_handshake_message (conn, handshake, &error))
              {
                dsk_tls_connection_fail (conn, err);
                dsk_error_unref (err);
                dsk_tls_handshake_unref (shake);
                return false;
              }
          }
        break;
      }

    case DSK_TLS_RECORD_CONTENT_TYPE_APPLICATION_DATA:
      {
        // unencrypt and validate data
        ...

        // 
        ...
      }

    case DSK_TLS_RECORD_CONTENT_TYPE_CHANGE_CIPHER_SPEC:
      {
        //...
      }
      break;

    case DSK_TLS_RECORD_CONTENT_TYPE_HEARTBEAT:
      {
        ...
      }
      break;

    default:
      error = dsk_error_new ("unknown ContentType 0x%02x", 
                             header_result.content_type);
      dsk_tls_connection_fail (conn, err);
      dsk_error_unref (err);
      return false;
    }

  goto try_parse_record_header;
}

DskTlsConnection *
dsk_tls_connection_client_new (DskTlsClientContext *context,
                               DskStream           *underlying,
                               DskError           **error)
{
  DskTlsConnection *conn = dsk_object_new (&dsk_tls_connection_class);
  conn->underlying = dsk_object_ref (underlying);
  conn->context = dsk_object_ref (context);
  conn->handshake_info = dsk_new0 (DskTlsHandshakeNegotiation);

  conn->state = DSK_TLS_CONNECTION_CLIENT_WAIT_SERVER_HELLO;
  if (!send_client_hello_flight (conn->handshake_info,
                                 error))
    {
      ...
    }

  dsk_hook_trap (...);  read
  dsk_hook_trap (..);   write
  return conn;
}

DskTlsConnection *
dsk_tls_connection_server_new (DskTlsServerContext *context,
                               DskStream           *underlying,
                               DskError           **error)
{
  DskTlsConnection *conn = dsk_object_new (&dsk_tls_connection_class);
  conn->underlying = dsk_object_ref (underlying);
  conn->context = dsk_object_ref (context);
  conn->handshake_info = dsk_new0 (DskTlsHandshakeNegotiation);
  conn->state = DSK_TLS_CONNECTION_SERVER_START;
  dsk_hook_trap (...);  read
  return conn;
}
