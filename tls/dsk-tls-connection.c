#include "../dsk.h"


static const char *
dsk_tls_connection_state_name (DskTlsConnectionState state)
{
  switch (state)
    {
#define WRITE_CASE(shortname) case DSK_TLS_CONNECTION_##shortname: return #shortname
      WRITE_CASE(CLIENT_START);
      WRITE_CASE(CLIENT_WAIT_SERVER_HELLO);
      WRITE_CASE(CLIENT_WAIT_ENCRYPTED_EXTENSIONS);
      WRITE_CASE(CLIENT_WAIT_CERT_CR);
      WRITE_CASE(CLIENT_WAIT_CERT);
      WRITE_CASE(CLIENT_WAIT_CV);
      WRITE_CASE(CLIENT_WAIT_FINISHED);
      WRITE_CASE(CLIENT_CONNECTED);
      WRITE_CASE(CLIENT_FAILED);
      WRITE_CASE(SERVER_START);
      WRITE_CASE(SERVER_RECEIVED_CLIENT_HELLO);
      WRITE_CASE(SERVER_NEGOTIATED);
      WRITE_CASE(SERVER_WAIT_END_OF_EARLY_DATA);
      WRITE_CASE(SERVER_WAIT_FLIGHT2);
      WRITE_CASE(SERVER_WAIT_CERT);
      WRITE_CASE(SERVER_WAIT_CERT_VERIFY);
      WRITE_CASE(SERVER_WAIT_FINISHED);
      WRITE_CASE(SERVER_CONNECTED);
      WRITE_CASE(SERVER_FAILED);
#undef WRITE_CASE
      default: assert(false); return "*unknown connection state*";
    }
}

//https://github.com/bifurcation/mint/blob/master/negotiation.go

static int
server_do_version_negotiation (DskTlsConnection *conn,
                               size_t n_versions,
                               DskTlsProtocolVersion *versions)
{
  (void) conn;

  for (unsigned i = 0; i < n_versions; i++)
    if (versions[i] != DSK_TLS_PROTOCOL_VERSION_1_3)
      return i;

  return -1;
}

// Often called Dixie-Hellman negotiation
typedef struct KeyShareNegotiationResponse KeyShareNegotiationResponse;

typedef enum
{
  KEY_SHARE_NEGOTIATION_RESPONSE_USE_SHARED_KEY,
  KEY_SHARE_NEGOTIATION_RESPONSE_REQUEST_SUPPORTED_GROUP,
  KEY_SHARE_NEGOTIATION_RESPONSE_FAIL,
} KeyShareNegotiationResponseCode;
struct KeyShareNegotiationResponse
{
  KeyShareNegotiationResponseCode code;
  union {
    unsigned use_shared_key_index;
    unsigned use_supported_group_index;
  };
};

static int
get_supported_group_quality (DskTlsNamedGroup group)
{
  switch (group)
    {
      case DSK_TLS_NAMED_GROUP_X25519:
        return 1;
      default:
        return 0;
    }
}

static KeyShareNegotiationResponse
server_do_key_share_negotiation (DskTlsConnection *connection,
                                 unsigned n_key_shares,
                                 DskTlsKeyShareEntry *key_shares,
                                 unsigned n_supported_groups,
                                 DskTlsNamedGroup *supported_groups)
{
  KeyShareNegotiationResponse rv;

  (void) connection;

  //
  // Find the best key-share
  //
  int best_ks_index = -1;
  int best_ks_quality = 0;
  for (unsigned i = 0; i < n_key_shares; i++)
    {
      int quality = get_supported_group_quality (key_shares[i].named_group);
      if (quality > 0 && (best_ks_index == -1 || best_ks_quality < quality))
        {
          best_ks_index = i;
          best_ks_quality = quality;
        }
    }

  //
  // Find the best supported-group
  //
  int best_sg_index = -1;
  int best_sg_quality = 0;
  for (unsigned i = 0; i < n_supported_groups; i++)
    {
      int quality = get_supported_group_quality (supported_groups[i]);
      if (quality > 0 && (best_sg_index == -1 || best_sg_quality < quality))
        {
          best_sg_index = i;
          best_sg_quality = quality;
        }
    }

  if (best_ks_index >= 0 && best_sg_quality < 0)
    {
      rv.code = KEY_SHARE_NEGOTIATION_RESPONSE_USE_SHARED_KEY;
      rv.use_shared_key_index = best_ks_index;
    }
  else if (best_sg_index >= 0 && best_ks_quality < 0)
    {
      rv.code = KEY_SHARE_NEGOTIATION_RESPONSE_REQUEST_SUPPORTED_GROUP;
      rv.use_supported_group_index = best_sg_index;
    }
  else if (best_sg_index >= 0 && best_ks_quality >= 0)
    {
      // TODO: may not be the best at times?
      rv.code = KEY_SHARE_NEGOTIATION_RESPONSE_USE_SHARED_KEY;
      rv.use_shared_key_index = best_ks_index;
    }
  else
    {
      rv.code = KEY_SHARE_NEGOTIATION_RESPONSE_FAIL;
    }
  return rv;
}

static DskTlsExtension *
find_extension_by_type (DskTlsHandshake *shake, DskTlsExtensionType type)
{
  unsigned n;
  DskTlsExtension **arr;
  switch (shake->type)
    {
    case DSK_TLS_HANDSHAKE_TYPE_CLIENT_HELLO:
      n = shake->client_hello.n_extensions;
      arr = shake->client_hello.extensions;
      break;
    case DSK_TLS_HANDSHAKE_TYPE_SERVER_HELLO:
      n = shake->server_hello.n_extensions;
      arr = shake->server_hello.extensions;
      break;
    case DSK_TLS_HANDSHAKE_TYPE_ENCRYPTED_EXTENSIONS:
      n = shake->server_hello.n_extensions;
      arr = shake->server_hello.extensions;
      break;
    case DSK_TLS_HANDSHAKE_TYPE_CERTIFICATE_REQUEST:
      n = shake->certificate_request.n_extensions;
      arr = shake->certificate_request.extensions;
      break;
    default:
      return NULL;
    }
  for (unsigned i = 0; i < n; i++)
    if (arr[i]->type == type)
      return arr[i];
  return NULL;
}

static bool
handle_client_hello (DskTlsConnection *conn,
                     DskTlsHandshake  *shake,
                     DskError        **error)
{
  if (conn->state != DSK_TLS_CONNECTION_SERVER_START)
    {
      *error = dsk_error_new ("ClientHello received in %s state: not allowed",
                              dsk_tls_connection_state_name (conn->state));
      return false;
    }

  DskTlsExtension_ServerNameList *server_name = (DskTlsExtension_ServerNameList *)
    find_extension_by_type (shake, DSK_TLS_EXTENSION_TYPE_SERVER_NAME);

  DskTlsExtension_SupportedVersions *supp_ver = (DskTlsExtension_SupportedVersions *)
    find_extension_by_type (shake, DSK_TLS_EXTENSION_TYPE_SUPPORTED_VERSIONS);
  if (supp_ver != NULL)
    {
      int index = server_do_version_negotiation (conn,
                                                 supp_ver->n_supported_versions,
                                                 supp_ver->supported_versions);
      if (index < 0)
        {
          ... failed negotiation
        }
      else
        {
          conn->version = supp_ver->supported_versions[index];
        }
    }

  //
  // Handle KeyShare extension.
  //
  DskTlsExtension_KeyShareEntry *key_share = (DskTlsExtension_KeyShareEntry *)
    find_extension_by_type (shake, DSK_TLS_EXTENSION_TYPE_KEY_SHARE);
  if (key_share != NULL)
    {
      int index = server_do_key_share_negotiation (conn,
                                                   key_share->n_key_shares,
                                                   key_share->key_shares);
      if (index < 0)
        {
          .... failed negotiation
        }
      else
        {
          ...
        }
    }

  //
  // Handle PreSharedKeys
  //
  DskTlsExtension_PreSharedKey *psk = (DskTlsExtension_PreSharedKey *)
    find_extension_by_type (shake, DSK_TLS_EXTENSION_TYPE_PRE_SHARED_KEY);
  if (psk != NULL)
    {
      ...
    }

  //
  // Handle PreSharedKey Mode Negotiation
  //
  DskTlsExtension_PSKKeyExchangeModes *pskm = (DskTlsExtension_PSKKeyExchangeModes *)
    find_extension_by_type (shake, DSK_TLS_EXTENSION_TYPE_PSK_KEY_EXCHANGE_MODES);
  if (pskm != NULL)
    {
      ...
    }

  //
  // Perform Certificate Selection
  //

  //
  // Perform Early Data Negotiation
  //

  //
  // Cipher Suite Negotiation
  //

  //
  // Application-Level Protocol Negotiation
  //
}

static bool
handle_handshake (DskTlsConnection *conn,
                  DskTlsHandshake  *shake,
                  DskError        **error)
{
  switch (shake->type)
    {
    case DSK_TLS_HANDSHAKE_TYPE_CLIENT_HELLO:
      return handle_client_hello (conn, shake, error);
    case DSK_TLS_HANDSHAKE_TYPE_SERVER_HELLO:
      return handle_server_hello (conn, shake, error);
    case DSK_TLS_HANDSHAKE_TYPE_NEW_SESSION_TICKET:
      return handle_new_session_ticket (conn, shake, error);
    case DSK_TLS_HANDSHAKE_TYPE_END_OF_EARLY_DATA:
      return handle_end_of_early_data (conn, shake, error);
    case DSK_TLS_HANDSHAKE_TYPE_ENCRYPTED_EXTENSIONS:
      return handle_encrypted_extensions (conn, shake, error);
    case DSK_TLS_HANDSHAKE_TYPE_CERTIFICATE:
      return handle_certificate (conn, shake, error);
    case DSK_TLS_HANDSHAKE_TYPE_CERTIFICATE_REQUEST:
      return handle_certificate_request (conn, shake, error);
    case DSK_TLS_HANDSHAKE_TYPE_CERTIFICATE_VERIFY:
      return handle_certificate_verify (conn, shake, error);
    case DSK_TLS_HANDSHAKE_TYPE_FINISHED:
      return handle_certificate_finished (conn, shake, error);
    case DSK_TLS_HANDSHAKE_TYPE_KEY_UPDATE:
      return handle_certificate_key_update (conn, shake, error);
    case DSK_TLS_HANDSHAKE_TYPE_MESSAGE_HASH:
      return handle_certificate_message_hash (conn, shake, error);
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

  // throw away header
  dsk_buffer_discard (&conn->incoming_raw, header_result.header_length);

  ensure_record_data_capacity (conn, header_result.payload_length);
  uint8_t *record = conn->records_plaintext + conn->records_plaintext_lenth;
  dsk_buffer_read (&conn->incoming_raw, header_result.payload_length, record);

  if (conn->decryptor != NULL)
    {
      ...
    }
  else
    {
      conn->records_plaintext_lenth += header_result.payload_length;
    }

  const uint8_t *parse_at = conn->records_plaintext;
  const uint8_t *end_parse = conn->records_plaintext + conn->records_plaintext_lenth;
  switch (content_type)
    {
    case DSK_TLS_RECORD_CONTENT_TYPE_ALERT:
      {
        ...
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
            DskTlsHandshakeParseResult result = dsk_tls_parse_handshake (*parse_at, len, msgbuf);
            switch (result.code)
              {
              case PARSE_RESULT_CODE_OK:
                {
                  DskTlsHandshake *shake = result.handshake;
                  if (!handle_handshake (conn, shake, &err))
                    {
                      dsk_tls_connection_fail (conn, err);
                      dsk_error_unref (err);
                      dsk_tls_handshake_unref (shake);
                      return false;
                    }
                  dsk_tls_handshake_unref (shake);
                  break;
                }
              case PARSE_RESULT_CODE_ERROR:
                {
                  dsk_tls_connection_fail (conn, result.error);
                  dsk_error_unref (result.error);
                  return false;
                }
              case PARSE_RESULT_CODE_TOO_SHORT:
                assert(false);
                return false;
              }
          }
        goto try_parse_record_header;
      }

    case DSK_TLS_RECORD_CONTENT_TYPE_APPLICATION_DATA:
      {
        ...
      }

    case DSK_TLS_RECORD_CONTENT_TYPE_CHANGE_CIPHER_SPEC:
      {
        ...
      }
    }
}

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
