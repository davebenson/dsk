#include "../dsk.h"
#include "dsk-tls-private.h"
#include <stdlib.h>

static DskTlsCipherSuite std_cipher_suite[] = {
  DSK_TLS_CIPHER_SUITE_TLS_AES_128_GCM_SHA256,
  DSK_TLS_CIPHER_SUITE_TLS_AES_128_CCM_SHA256,
};

static int
compare_key_share_by_named_group (const void *a, const void *b)
{
  const DskTlsKeyShareEntry *A = a;
  const DskTlsKeyShareEntry *B = b;
  return A->named_group < B->named_group ? -1
       : A->named_group > B->named_group ? 1
       : 0;
}
static int
compare_named_groups (const void *a, const void *b)
{
  const DskTlsNamedGroup *A = a;
  const DskTlsNamedGroup *B = b;
  return A < B ? -1
       : A > B ? 1
       : 0;
}

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

typedef struct KeyShareNegotiationResult KeyShareNegotiationResult;
typedef enum KeyShareNegotiationResultCode
{
  // Return is a KeyShareEntry
  KEY_SHARE_NEGOTIATION_RESULT_CODE_GOT_KEY_SHARE,
  KEY_SHARE_NEGOTIATION_RESULT_CODE_GOT_NAMED_GROUP,
  KEY_SHARE_NEGOTIATION_RESULT_CODE_NONE,
} KeyShareNegotiationResultCode;
struct KeyShareNegotiationResult
{
  KeyShareNegotiationResultCode code;

  // For GOT_KEY_SHARE and GOT_NAMED_GROUP responses.
  unsigned request_index;
  DskTlsKeyShareEntry key_share;
};

static KeyShareNegotiationResult
server_do_key_share_negotiation (DskTlsConnection *connection,
                                 unsigned n_key_shares,
                                 DskTlsKeyShareEntry *key_shares,
                                 unsigned n_supported_groups,
                                 DskTlsNamedGroup *supported_groups)
{
  KeyShareNegotiationResult rv;
  DskTlsConnectionHandshakeInfo *hs_info = connection->handshake_info;
  DskMemPool *pool = &hs_info->mem_pool;

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
  DskTlsNamedGroup named_group = 0;
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
      rv.code = KEY_SHARE_NEGOTIATION_RESULT_CODE_GOT_KEY_SHARE;
      rv.request_index = best_ks_index;
      named_group = key_shares[best_ks_index].named_group;
    }
  else if (best_sg_index >= 0 && best_ks_quality < 0)
    {
      rv.code = KEY_SHARE_NEGOTIATION_RESULT_CODE_GOT_NAMED_GROUP;
      rv.request_index = best_sg_index;
      named_group = supported_groups[best_sg_index];
    }
  else if (best_sg_index >= 0 && best_ks_quality >= 0)
    {
      // TODO: may not be the best at times?
      rv.code = KEY_SHARE_NEGOTIATION_RESULT_CODE_GOT_KEY_SHARE;
      rv.request_index = best_ks_index;
      named_group = key_shares[best_ks_index].named_group;
    }
  else
    {
      rv.code = KEY_SHARE_NEGOTIATION_RESULT_CODE_NONE;
      return rv;
    }


  DskTlsKeyShareMethod *method;
  method = dsk_tls_key_share_method_by_group (named_group);
  assert (method != NULL);

  //
  // Generate public/private key-pair and set key-share to public key.
  //
  connection->handshake_info->n_key_shares = 1;
  connection->handshake_info->key_shares = dsk_mem_pool_alloc (pool, sizeof (DskTlsPublicPrivateKeyShare));
  connection->handshake_info->key_shares[0].method = method;
  uint8_t *private_key = dsk_mem_pool_alloc (pool, method->private_key_bytes);
  connection->handshake_info->key_shares[0].private_key = private_key;
  uint8_t *public_key = dsk_mem_pool_alloc (pool, method->public_key_bytes);
  connection->handshake_info->key_shares[0].public_key = public_key;
  do
    {
      // generate random data
      dsk_get_cryptorandom_data (method->private_key_bytes, private_key);
    }
  while (!method->make_key_pair(method, private_key, public_key));

  if (rv.code == KEY_SHARE_NEGOTIATION_RESULT_CODE_GOT_KEY_SHARE)
    {
      connection->shared_key_length = method->shared_key_bytes;
      connection->shared_key = dsk_malloc (method->shared_key_bytes);
      method->make_shared_key (method,
                               private_key, 
                               key_shares[best_ks_index].key_exchange_data,
                               connection->shared_key);
    }
  return rv;
}

// ---------------------------------------------------------------------
//                Certificate Negotiation
// ---------------------------------------------------------------------
typedef enum
{
  CERTIFICATE_NEGOTIATION_SUCCESS,
  CERTIFICATE_NEGOTIATION_FAILED,
} CertificateNegotiationResultCode;

typedef struct {
  CertificateNegotiationResultCode code;
  union {
    struct {
      DskTlsCertificate *certificate;
      DskTlsSignatureScheme scheme;
    } success;
    DskError *error;
  };
} CertificateNegotiationResult;
static CertificateNegotiationResult
do_certificate_negotiation (DskTlsConnection *conn,
                            const char       *server_name,
                            size_t            n_sig_schemes,
                            DskTlsSignatureScheme *sig_schemes,
                            size_t            n_certs,
                            DskTlsCertificate **certs)
{
  for (size_t i = 0; i < n_certs; i++)
    {
      //
      // Filter out certificates whose
      // name doesn't match the server_name extension value.
      //
      if (server_name != NULL)
        {
          unsigned n_dns_names = certs[i].chain[0].n_dns_names;
          unsigned dns_names = certs[i].chain[0].dns_names;
          bool matched = false;
          for (unsigned j = 0; j < n_dns_names; j++)
            if (strcmp (server_name, dns_names[j]) == 0)
              {
                matched = true;
                break;
              }
          if (!matched)
            continue;
        }
      
      //
      // Look for a supported scheme that matches this cert.
      //
      for (size_t j = 0; j < n_sig_schemes; j++)
        if (scheme_valid_for_key (sig_schemes[j], certs[i].private_key))
          {
            res.success.certificate = certs[i];
            res.success.scheme = sig_schemes[j];
            res.code = CERTIFICATE_NEGOTIATION_SUCCESS;
            return res;
          }
    }

  res.code = CERTIFICATE_NEGOTIATION_FAILED;
  res.error = dsk_error_new ("no certificates compatible with signature schemes");
  return res;
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

#define MAX_RESPONSE_EXTENSIONS    10
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

  unsigned n_response_exts = 0;
  DskTlsExtension *response_exts[MAX_RESPONSE_EXTENSIONS];
  bool need_client_key_share = false;
  DskMemPool *pool = &conn->handshake_info->mem_pool;

  //
  // Find out if the protocol provided a server-name,
  // which we may use in subsequent negotations.
  //
  const char *server_hostname = NULL;
  DskTlsExtension_ServerNameList *server_name_ext = (DskTlsExtension_ServerNameList *)
    find_extension_by_type (shake, DSK_TLS_EXTENSION_TYPE_SERVER_NAME);
  if (server_name_ext != NULL)
    {
      for (unsigned i = 0; i < server_name_ext->n_entries; i++)
        if (server_name_ext->entries[i].type == DSK_TLS_EXTENSION_SERVER_NAME_TYPE_HOSTNAME)
          {
            server_hostname = server_name_ext->entries[i].name;
            break;
          }
    }

  DskTlsExtension_SupportedVersions *supp_ver = (DskTlsExtension_SupportedVersions *)
    find_extension_by_type (shake, DSK_TLS_EXTENSION_TYPE_SUPPORTED_VERSIONS);
  if (supp_ver != NULL)
    {
      int index = server_do_version_negotiation (conn,
                                                 supp_ver->n_supported_versions,
                                                 supp_ver->supported_versions);
      if (index < 0)
        {
          *error = dsk_error_new ("version negotiation failed");
          return false;
        }
      else
        {
          conn->version = supp_ver->supported_versions[index];
        }
    }

  //
  // Handle KeyShare extension.  Section 4.2.8.
  //
  DskTlsExtension_KeyShare *key_share = (DskTlsExtension_KeyShare *)
    find_extension_by_type (shake, DSK_TLS_EXTENSION_TYPE_KEY_SHARE);
  DskTlsExtension_SupportedGroups *supp_groups = (DskTlsExtension_SupportedGroups *)
    find_extension_by_type (shake, DSK_TLS_EXTENSION_TYPE_KEY_SHARE);
  DskTlsKeyShareEntry response_key_share;
  bool did_key_share_negotiations = false;
  if (key_share != NULL
   || supp_groups != NULL)
    {
      KeyShareNegotiationResult res;
      unsigned n_key_shares = 0;
      DskTlsKeyShareEntry *key_shares = NULL;
      unsigned n_supported_groups = 0;
      DskTlsNamedGroup *supported_groups = NULL;
      if (key_share != NULL)
        {
          n_key_shares = key_share->n_key_shares;
          key_shares = key_share->key_shares;
          qsort (key_shares, n_key_shares, sizeof(DskTlsKeyShareEntry), compare_key_share_by_named_group);
        }
      if (supp_groups != NULL)
        {
          n_supported_groups = supp_groups->n_supported_groups;
          supported_groups = supp_groups->supported_groups;
          qsort (supported_groups, n_supported_groups, sizeof(DskTlsNamedGroup), compare_named_groups);
        }

      // Page 48: (4.2.8):
      //    "Clients MUST NOT offer multiple KeyShareEntry values
      //     for the same group.  Clients MUST NOT offer any KeyShareEntry values
      //     for groups not listed in the client's "supported_groups" extension.
      //     Servers MAY check for violations of these rules and abort the
      //     handshake with an "illegal_parameter" alert if one is violated."
      for (unsigned ks = 0, sg = 0; ks < n_key_shares; ks++)
        {
          while (sg < n_supported_groups
              && supported_groups[sg] != key_shares[ks].named_group)
            sg++;
          if (sg == n_supported_groups)
            {
              *error = dsk_error_new ("key-share offered without corresponding supported-group");
              return false;
            }
          if (ks > 0 && key_shares[ks-1].named_group == key_shares[ks].named_group)
            {
              *error = dsk_error_new ("two key-shares offered for the same group");
              return false;
            }
        }

      res = server_do_key_share_negotiation (conn,
                                             n_key_shares,
                                             key_shares,
                                             n_supported_groups,
                                             supported_groups);
      if (res.code == KEY_SHARE_NEGOTIATION_RESULT_CODE_NONE)
        {
          *error = dsk_error_new ("key-share negotiation failed");
          return false;
        }
      else
        {
          DskTlsExtension_KeyShare *e = dsk_mem_pool_alloc (pool, sizeof (DskTlsExtension_KeyShare));
          e->server_share = response_key_share;
          e->base.is_generic = false;
          e->base.type = DSK_TLS_EXTENSION_TYPE_KEY_SHARE;
          e->base.extension_data_length = 0;
          e->base.extension_data = NULL;
          assert(n_response_exts < MAX_RESPONSE_EXTENSIONS);
          response_exts[n_response_exts++] = (DskTlsExtension *) e;

          // if true, we'll send a retry-request to get a key-share.
          need_client_key_share = res.code == KEY_SHARE_NEGOTIATION_RESULT_CODE_GOT_NAMED_GROUP;
          did_key_share_negotiations = true;
        }
    }

  //
  // Cipher Suite Negotiation
  //


  //
  // Handle PreSharedKey Mode Negotiation
  //
  DskTlsExtension_PSKKeyExchangeModes *pskm = (DskTlsExtension_PSKKeyExchangeModes *)
    find_extension_by_type (shake, DSK_TLS_EXTENSION_TYPE_PSK_KEY_EXCHANGE_MODES);
  bool has_key_exchange_mode = false;
  DskTlsExtension_PSKKeyExchangeMode key_exchange_mode;
  if (pskm != NULL)
    {
      bool got_psk_only_key_exchange_mode = false;
      bool got_psk_with_dhe_key_exchange_mode = false;
      for (unsigned i = 0; i < pskm->n_modes; i++)
        switch (pskm->modes[i])
          {
          case DSK_TLS_EXTENSION_PSK_EXCHANGE_MODE_KE:
            got_psk_only_key_exchange_mode = true;
            break;
          case DSK_TLS_EXTENSION_PSK_EXCHANGE_MODE_DHE_KE:
            got_psk_with_dhe_key_exchange_mode = true;
            break;
          }
      bool use_psk_only = key_share == NULL
                       && got_psk_only_key_exchange_mode;
      bool use_psk_dhe = did_key_share_negotiations
                       && got_psk_with_dhe_key_exchange_mode;
      if (use_psk_dhe)
        {
          has_key_exchange_mode = true;
          key_exchange_mode = DSK_TLS_EXTENSION_PSK_EXCHANGE_MODE_DHE_KE;
        }
      else if (use_psk_only)
        {
          has_key_exchange_mode = true;
          key_exchange_mode = DSK_TLS_EXTENSION_PSK_EXCHANGE_MODE_KE;
        }
    }

  //
  // Handle PreSharedKeys
  //
  DskTlsExtension_PreSharedKey *psk = (DskTlsExtension_PreSharedKey *)
    find_extension_by_type (shake, DSK_TLS_EXTENSION_TYPE_PRE_SHARED_KEY);
  if (psk != NULL)
    {
       if (pskm == NULL)
         {
           // 4.2.9: "If clients offer "pre_shared_key"
           //         without a "psk_key_exchange_modes" extension,
           //         servers MUST abort the handshake."
           *error = dsk_error_new ("client offered Pre-Shared Key without Pre-Shared Key Exchange Modes");
           return false;
         }

      // PSK Exchange-Mode negotiation must succeed to do
      // anything with PSKs.
      if (has_key_exchange_mode
       && conn->context->server_allow_pre_shared_keys)
        {
          //...
        }
    }

  //
  // Perform Certificate Selection
  //
  if (server_hostname != NULL)
    {
      CertificateNegotiationResult res = do_certificate_negotiation (conn, ...);
      ...
    }
      

  //
  // Perform Early Data Negotiation
  //

  //
  // Application-Level Protocol Negotiation
  //


  if (need_client_key_share)
    {
      ... must send hello-retry-request
    }
  else
    {
      ... send server-hello
      ... set decryptor, encryptor
    }
}

// ServerHello should indicate that the
// server was able to come up with an agreeable
// set of parameters, we are ready to
// validate certs or move on to the application data.
static bool
handle_server_hello (DskTlsConnection *conn,
                     DskTlsHandshake  *shake,
                     DskError        **error)
{
  ...
}

static bool
handle_hello_retry_request (DskTlsConnection *conn,
                            DskTlsHandshake  *shake,
                            DskError        **error)
{
  ...
}

static bool
handle_new_session_ticket  (DskTlsConnection *conn,
                            DskTlsHandshake  *shake,
                            DskError        **error)
{
  if (conn->context->support_pre_shared_keys
  ...
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
      if (is_retry_request (shake))
        return handle_hello_retry_request (conn, shake, error);
      else
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
  switch (header_result.content_type)
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
            DskTlsHandshake *handshake = dsk_tls_handshake_parse (*parse_at, len, msgbuf, &pool, &error);
            if (handshake == NULL)
              {
                dsk_tls_connection_fail (conn, error);
                dsk_error_unref (error);
                return false;
              }
            else if (!handle_handshake (conn, handshake, &error))
              {
                dsk_tls_connection_fail (conn, err);
                dsk_error_unref (err);
                dsk_tls_handshake_unref (shake);
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
        //...
      }
      goto try_parse_record_header;

    case DSK_TLS_RECORD_CONTENT_TYPE_HEARTBEAT:
      {
        ...
      }
      goto try_parse_record_header;

    default:
      error = dsk_error_new ("unknown ContentType 0x%02x", 
                             header_result.content_type);
      dsk_tls_connection_fail (conn, err);
      dsk_error_unref (err);
      return false;
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
