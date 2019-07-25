#include "../dsk.h"
#include <string.h>
#include "dsk-tls-private.h"

struct DskTlsClientContext
{
  unsigned ref_count;

  size_t n_certificates;
  DskTlsKeyPair **certificates;

  size_t n_application_layer_protocols;
  const char **application_layer_protocols;
  bool application_layer_protocol_negotiation_required;

  // A comma-sep list of key-shares whose
  // public/private keys should be computed
  // in the initial ClientHello.
  const char *offered_key_share_groups;

  DskTlsClientLookupSessionsFunc lookup_sessions_func;
  void *lookup_sessions_data;
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

static const char *
dsk_tls_client_connection_state_name (DskTlsClientConnectionState state)
{
  switch (state)
    {
#define WRITE_CASE(shortname) case DSK_TLS_CLIENT_CONNECTION_##shortname: return #shortname
      WRITE_CASE(START);
      WRITE_CASE(DOING_PSK_LOOKUP);
      WRITE_CASE(WAIT_SERVER_HELLO);
      WRITE_CASE(WAIT_ENCRYPTED_EXTENSIONS);
      WRITE_CASE(WAIT_CERT_CR);
      WRITE_CASE(WAIT_CERT);
      WRITE_CASE(WAIT_CV);
      WRITE_CASE(WAIT_FINISHED);
      WRITE_CASE(CONNECTED);
      WRITE_CASE(FAILED);
#undef WRITE_CASE
      default: assert(false); return "*unknown client connection state*";
    }
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

static DskTlsHandshakeMessage *
create_handshake (DskTlsClientHandshake *hs_info,
                  DskTlsHandshakeMessageType         type,
                  unsigned                    max_extensions)
{
  DskTlsHandshakeMessage *hs = HS_ALLOC0 (hs_info, DskTlsHandshakeMessage);
  hs->type = type;
  hs->is_outgoing = true;

  hs_info->n_extensions = 0;
  hs_info->max_extensions = max_extensions;
  hs_info->extensions = HS_ALLOC_ARRAY (hs_info, max_extensions, DskTlsExtension *);
  hs_info->currently_under_construction = hs;

  return hs;
}

static DskTlsExtension *
find_extension_by_type (DskTlsHandshakeMessage *shake, DskTlsExtensionType type)
{
  unsigned n;
  DskTlsExtension **arr;
  switch (shake->type)
    {
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_CLIENT_HELLO:
      n = shake->client_hello.n_extensions;
      arr = shake->client_hello.extensions;
      break;
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_SERVER_HELLO:
      n = shake->server_hello.n_extensions;
      arr = shake->server_hello.extensions;
      break;
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_ENCRYPTED_EXTENSIONS:
      n = shake->server_hello.n_extensions;
      arr = shake->server_hello.extensions;
      break;
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_CERTIFICATE_REQUEST:
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


#define _FIND_EXT(shake, ext_type_suffix, code_suffix) \
    (DskTlsExtension_##ext_type_suffix *) \
     find_extension_by_type (shake, DSK_TLS_EXTENSION_TYPE_##code_suffix)

#define FIND_EXT_KEY_SHARE(shake) \
        _FIND_EXT(shake, KeyShare, KEY_SHARE)
#define FIND_EXT_SUPPORTED_VERSIONS(shake) \
        _FIND_EXT(shake, SupportedVersions, SUPPORTED_VERSIONS)
#define FIND_EXT_SUPPORTED_GROUPS(shake) \
        _FIND_EXT(shake, SupportedGroups, SUPPORTED_GROUPS)
#define FIND_EXT_SERVER_NAME(shake) \
        _FIND_EXT(shake, ServerNameList, SERVER_NAME)
#define FIND_EXT_PSK(shake) \
        _FIND_EXT(shake, PreSharedKey, PRE_SHARED_KEY)
#define FIND_EXT_PSK_KEY_EXCHANGE_MODE(shake) \
        _FIND_EXT(shake, PSKKeyExchangeModes, PSK_KEY_EXCHANGE_MODES)
#define FIND_EXT_SIGNATURE_ALGORITHMS(shake) \
        _FIND_EXT(shake, SignatureAlgorithms, SIGNATURE_ALGORITHMS)
#define FIND_EXT_EARLY_DATA_INDICATION(shake) \
        _FIND_EXT(shake, EarlyDataIndication, EARLY_DATA)
#define FIND_EXT_ALPN(shake) \
        _FIND_EXT(shake, ALPN, APPLICATION_LAYER_PROTOCOL_NEGOTIATION)

#define MAX_RESPONSE_EXTENSIONS    16

// ==============    Handshake Part 1:   Compute ClientHello    =============
// Compute the ClientHello message.
//
// It addition to standard client-side configuration,
// this also performs a lookup for relevant PSKs.
//
// Responding to it is in the next section.
// ==============    Handshake Part 1:   Compute ClientHello    =============

static bool
client_continue_post_psks (DskTlsClientHandshake *hs_info,
                           DskError **error);

//
// Compute the initial ClientHello message.
//
static bool
send_client_hello_flight (DskTlsClientHandshake *hs_info,
                          DskError **error)
{
  DskTlsHandshakeMessage *ch = create_handshake (hs_info, DSK_TLS_HANDSHAKE_MESSAGE_TYPE_CLIENT_HELLO, 16);
  DskTlsClientConnection *conn = hs_info->conn;
  DskTlsClientContext *ctx = conn->context;
  hs_info->client_hello = ch;

  // Offered client PSKs
  if (ctx->lookup_sessions_func != NULL)
    {
      conn->state = DSK_TLS_CLIENT_CONNECTION_DOING_PSK_LOOKUP;
      dsk_object_ref (conn);
      ctx->lookup_sessions_func (hs_info, ctx->lookup_sessions_data);
      return true;
    }
  else
    return client_continue_post_psks (hs_info, error);
}
static DskTlsExtension_KeyShare *
create_client_key_share_extension (DskTlsClientHandshake *hs_info,
                                   unsigned n_groups,
                                   const DskTlsNamedGroup *groups)
{
  DskTlsExtension_KeyShare *ks = HS_ALLOC (hs_info, DskTlsExtension_KeyShare);
  ks->base.type = DSK_TLS_EXTENSION_TYPE_KEY_SHARE;
  ks->n_key_shares = n_groups;
  ks->key_shares = HS_ALLOC_ARRAY(hs_info, n_groups, DskTlsKeyShareEntry);
  hs_info->n_key_shares = n_groups;
  hs_info->key_shares = HS_ALLOC_ARRAY (hs_info, n_groups, DskTlsPublicPrivateKeyShare);
  for (unsigned i = 0; i < n_groups; i++)
    {
      DskTlsNamedGroup group = groups[i];
      const DskTlsKeyShareMethod *share_method = dsk_tls_key_share_method_by_group (group);
      unsigned priv_key_size = share_method->private_key_bytes;
      unsigned pub_key_size = share_method->public_key_bytes;
      uint8_t *priv_key = dsk_mem_pool_alloc (&hs_info->mem_pool, priv_key_size + pub_key_size);
      uint8_t *pub_key = priv_key + priv_key_size;
      do
        {
          dsk_get_cryptorandom_data (priv_key_size, priv_key);
        }
      while (!share_method->make_key_pair (share_method, priv_key, pub_key));
      hs_info->key_shares[i].method = share_method;
      hs_info->key_shares[i].public_key = pub_key;
      hs_info->key_shares[i].private_key = priv_key;

      // Initialize public key-share entry.
      DskTlsKeyShareEntry *entry = ks->key_shares + i;
      entry->named_group = group;
      entry->key_exchange_length = pub_key_size;
      entry->key_exchange_data = pub_key;
    }
  return ks;
}

static bool
send_second_client_hello_flight (DskTlsClientHandshake *hs_info,
                                 DskTlsCipherSuiteCode cs_code,
                                 bool modify_key_shares,
                                 int use_key_share_index,
                                 DskTlsNamedGroup key_share_generate,
                                 DskError **error)
{
  DskTlsHandshakeMessage *orig_client_hello = hs_info->client_hello;
  hs_info->old_client_hello = orig_client_hello;
  DskTlsHandshakeMessage *ch = create_handshake (hs_info, DSK_TLS_HANDSHAKE_MESSAGE_TYPE_CLIENT_HELLO, 16);
  hs_info->client_hello = ch;
  ch->client_hello.legacy_version = 0x301;
  memcpy (ch->client_hello.random,
          orig_client_hello->client_hello.random,
          32);
  ch->client_hello.legacy_session_id_length = 32;
  memcpy (ch->client_hello.legacy_session_id,
          orig_client_hello->client_hello.legacy_session_id,
          32);
  ch->client_hello.n_cipher_suites = 1;
  ch->client_hello.cipher_suites = HS_ALLOC_ARRAY (hs_info, 1, DskTlsCipherSuiteCode);
  ch->client_hello.cipher_suites[0] = cs_code;
  ch->client_hello.n_compression_methods = 0;
  ch->client_hello.compression_methods = NULL;

  // supported groups
  DskTlsExtension_SupportedGroups *sg = HS_ALLOC (hs_info, DskTlsExtension_SupportedGroups);
  sg->base.type = DSK_TLS_EXTENSION_TYPE_SUPPORTED_GROUPS;
  sg->n_supported_groups = dsk_tls_n_named_groups;
  sg->supported_groups = dsk_tls_named_groups;
  hs_info->extensions[hs_info->n_extensions++] = (DskTlsExtension *) sg;

  // supported versions
  static const DskTlsProtocolVersion supported_versions[] = {
    DSK_TLS_PROTOCOL_VERSION_1_3
  };
  DskTlsExtension_SupportedVersions *sv = HS_ALLOC (hs_info, DskTlsExtension_SupportedVersions);
  sv->base.type = DSK_TLS_EXTENSION_TYPE_SUPPORTED_VERSIONS;
  sv->n_supported_versions = DSK_N_ELEMENTS (supported_versions);
  sv->supported_versions = (DskTlsProtocolVersion *) supported_versions;
  hs_info->extensions[hs_info->n_extensions++] = (DskTlsExtension *) sv;

  //
  // Key-Share
  //
  DskTlsExtension_KeyShare *ks;
  if (modify_key_shares)
    {
      if (use_key_share_index >= 0)
        {
          ks = FIND_EXT_KEY_SHARE (orig_client_hello);
          if (ks->n_key_shares != 1)
            {
              DskTlsExtension_KeyShare *new_ks = HS_ALLOC0(hs_info, DskTlsExtension_KeyShare);
              new_ks->base.type = DSK_TLS_EXTENSION_TYPE_KEY_SHARE;
              new_ks->n_key_shares = 1;
              new_ks->key_shares = ks->key_shares + use_key_share_index;
              ks = new_ks;
            }
          else
            {
              // just use the same key-share.  nothing to do.
            }
        }
      else
        {
          ks = create_client_key_share_extension (hs_info, 1, &key_share_generate);
        }
    }
  else
    {
      ks = FIND_EXT_KEY_SHARE (orig_client_hello);
    }
  hs_info->extensions[hs_info->n_extensions++] = (DskTlsExtension *) ks;

  DskTlsExtension_SignatureAlgorithms *sa = FIND_EXT_SIGNATURE_ALGORITHMS (orig_client_hello);
  assert(sa != NULL);
  hs_info->extensions[hs_info->n_extensions++] = (DskTlsExtension *) sa;

  // finish extensions
  hs_info->client_hello->client_hello.n_extensions = hs_info->n_extensions;
  hs_info->client_hello->client_hello.extensions = hs_info->extensions;

  // serialize
  if (!dsk_tls_handshake_message_serialize (hs_info->client_hello, &hs_info->mem_pool, error))
    return false;

  // send it!
  dsk_tls_record_layer_send_handshake (&hs_info->conn->base_instance, ch->data_length, ch->data);

  hs_info->conn->state = DSK_TLS_CLIENT_CONNECTION_WAIT_SERVER_HELLO;

  return true;
}

void
dsk_tls_handshake_client_error_finding_psks (DskTlsClientHandshake *hs_info,
                                             DskError *error)
{
  DskTlsClientConnection *conn = hs_info->conn;
  assert(conn->state == DSK_TLS_CLIENT_CONNECTION_DOING_PSK_LOOKUP);

  dsk_tls_base_connection_fail (&conn->base_instance, error);
  dsk_object_unref (conn);
}

static void
tls_preshared_key_info_copy_to (DskMemPool *pool,
                                DskTlsClientSessionInfo *dst,
                                const DskTlsClientSessionInfo *src)
{
  *dst = *src;
  dst->identity_data = dsk_mem_pool_memdup (pool, src->identity_length, src->identity_data);
  dst->master_key = dsk_mem_pool_memdup (pool, src->master_key_length, src->master_key);
}

void
dsk_tls_handshake_client_found_psks (DskTlsClientHandshake *hs_info,
                                     size_t                      n_psks,
                                     DskTlsClientSessionInfo     *psks)
{
  DskTlsConnection *conn = hs_info->conn;
  if (n_psks > 0)
    {
      hs_info->n_psks = n_psks;
      hs_info->psks = HS_ALLOC_ARRAY (hs_info, n_psks, DskTlsPresharedKeyInfo);
      for (size_t i = 0; i < n_psks; i++)
        tls_preshared_key_info_copy_to (&hs_info->mem_pool, hs_info->psks + i, psks + i);
    }

  DskError *error = NULL;
  if (!client_continue_post_psks (hs_info, &error))
    {
      dsk_tls_connection_failed (conn, error);
      dsk_object_unref (conn);
      dsk_error_unref (error);
      return;
    }
  dsk_object_unref (conn);
}

static bool
client_continue_post_psks (DskTlsClientHandshake *hs_info,
                           DskError **error)
{
  DskTlsConnection *conn = hs_info->conn;
  conn->state = DSK_TLS_CLIENT_CONNECTION_START;

  DskTlsHandshakeMessage *ch = hs_info->client_hello;
  ch->client_hello.legacy_version = 0x301;
  dsk_get_cryptorandom_data (32, ch->client_hello.random);
  ch->client_hello.legacy_session_id_length = 32;
  dsk_get_cryptorandom_data (32, ch->client_hello.legacy_session_id);
  ch->client_hello.n_cipher_suites = DSK_N_ELEMENTS (client_hello_supported_cipher_suites);
  ch->client_hello.cipher_suites = (DskTlsCipherSuiteCode *) client_hello_supported_cipher_suites;
  ch->client_hello.n_compression_methods = 0;
  ch->client_hello.compression_methods = NULL;

  // supported groups
  DskTlsExtension_SupportedGroups *sg = HS_ALLOC (hs_info, DskTlsExtension_SupportedGroups);
  sg->base.type = DSK_TLS_EXTENSION_TYPE_SUPPORTED_GROUPS;
  sg->n_supported_groups = DSK_N_ELEMENTS (client_hello_supported_groups);
  sg->supported_groups = (DskTlsNamedGroup *) client_hello_supported_groups;
  hs_info->extensions[hs_info->n_extensions++] = (DskTlsExtension *) sg;

  // supported versions
  static const DskTlsProtocolVersion supported_versions[] = {
    DSK_TLS_PROTOCOL_VERSION_1_3
  };
  DskTlsExtension_SupportedVersions *sv = HS_ALLOC (hs_info, DskTlsExtension_SupportedVersions);
  sv->base.type = DSK_TLS_EXTENSION_TYPE_SUPPORTED_VERSIONS;
  sv->n_supported_versions = DSK_N_ELEMENTS (supported_versions);
  sv->supported_versions = (DskTlsProtocolVersion *) supported_versions;
  hs_info->extensions[hs_info->n_extensions++] = (DskTlsExtension *) sv;

  // key-share
  static DskTlsNamedGroup client_key_share_groups[] = {
    DSK_TLS_NAMED_GROUP_X25519
  };


  DskTlsExtension_KeyShare *ks = create_client_key_share_extension (hs_info, DSK_N_ELEMENTS (client_key_share_groups), client_key_share_groups);
  hs_info->extensions[hs_info->n_extensions++] = (DskTlsExtension *) ks;

  static const DskTlsSignatureScheme signature_schemes[] = {
    DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA256,
    DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA384,
    DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA512,
    DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA256,
    DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA384,
    DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA512,
    DSK_TLS_SIGNATURE_SCHEME_ED25519,
    DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_PSS_SHA256,
    DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_PSS_SHA384,
    DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_PSS_SHA512,
  };
  DskTlsExtension_SignatureAlgorithms *sa = HS_ALLOC0 (hs_info, DskTlsExtension_SignatureAlgorithms);
  sa->base.type = DSK_TLS_EXTENSION_TYPE_SIGNATURE_ALGORITHMS;
  sa->n_schemes = DSK_N_ELEMENTS (signature_schemes);
  sa->schemes = (DskTlsSignatureScheme *) signature_schemes;
  hs_info->extensions[hs_info->n_extensions++] = (DskTlsExtension *) sa;

  // finish extensions
  hs_info->client_hello->client_hello.n_extensions = hs_info->n_extensions;
  hs_info->client_hello->client_hello.extensions = hs_info->extensions;

  // serialize
  if (!dsk_tls_handshake_serialize (hs_info, hs_info->client_hello, error))
    return false;

  // send it!
  dsk_tls_record_layer_send_handshake (hs_info, ch->data_length, ch->data);

  conn->state = DSK_TLS_CLIENT_CONNECTION_WAIT_SERVER_HELLO;

  return true;
}

//
// ServerHello should indicate that the server was able to come
// up with an agreeable set of parameters, we are ready to
// validate certs or move on to the application data.
//
static bool
handle_server_hello (DskTlsConnection *conn,
                     DskTlsHandshakeMessage  *shake,
                     DskError        **error)
{
  DskTlsClientHandshake *hs_info = conn->handshake_info;

  if (conn->state != DSK_TLS_CLIENT_CONNECTION_WAIT_SERVER_HELLO)
    {
      *error = dsk_error_new ("ServerHello not allowed in %s state",
                              dsk_tls_connection_state_name (conn->state));
      DSK_ERROR_SET_TLS (*error, FATAL, UNEXPECTED_MESSAGE);
      return false;
    }

  //
  // Compute shared secret, use pre-shared-key, or both, or fail.
  //
  DskTlsExtension_PreSharedKey *psk = FIND_EXT_PSK (shake);
  DskTlsExtension_PSKKeyExchangeModes *pskm = FIND_EXT_PSK_KEY_EXCHANGE_MODE (shake);
  if (pskm != NULL)
    {
      *error = dsk_error_new ("PSK-ExchangeModes extension not allowed from server");
      DSK_ERROR_SET_TLS (*error, FATAL, ILLEGAL_PARAMETER);
      return false;
    }
      
  if (psk != NULL)
    {
      //
      // RFC 8446 Page 56:
      //     Clients MUST verify that the server's selected_identity is 
      //     within the range supplied by the client, that the server
      //     selected a cipher suite indicating a Hash associated
      //     with the PSK, and that a server "key_share" extension
      //     is present if required by the ClientHello
      //     "psk_key_exchange_modes" extension.  If these values are not
      //     consistent, the client MUST abort the handshake with an
      //     "illegal_parameter" alert.
      //
      unsigned selected_identity = psk->selected_identity;
      if (selected_identity >= hs_info->n_psks)
        {
          *error = dsk_error_new ("invalid PSK selection from server");
          DSK_ERROR_SET_TLS (*error, FATAL, ILLEGAL_PARAMETER);
          return false;
        }
      hs_info->has_psk = true;
    }

  //
  // Find which CipherSuite was agreed upon.
  //
  DskTlsCipherSuiteCode cs_code = shake->server_hello.cipher_suite;
  const DskTlsCipherSuite *cs = dsk_tls_cipher_suite_by_code (cs_code);
  if (cs == NULL)
    {
      *error = dsk_error_new ("server selected unavailable cipher-suite (0x%04x)", cs_code);
      return false;
    }
  conn->cipher_suite = cs;

  DskTlsExtension_KeyShare *ks = FIND_EXT_KEY_SHARE (shake);
  if (ks == NULL)
    {
      if (!hs_info->has_psk)
        {
          *error = dsk_error_new ("PSK required if no KeyShare present");
          return false;
        }
    }
  else
    {
      // Compute shared-secret.
      DskTlsKeyShareEntry *entry = &ks->server_share;
      DskTlsNamedGroup g = entry->named_group;
      unsigned ks_index;
      DskTlsPublicPrivateKeyShare *shares = hs_info->key_shares;
      for (ks_index = 0; ks_index < hs_info->n_key_shares; ks_index++)
        if (shares[ks_index].method->named_group == g)
          break;
      if (ks_index == hs_info->n_key_shares)
        {
          *error = dsk_error_new ("invalid key-share");
          return false;
        }
      const DskTlsKeyShareMethod *method = shares[ks_index].method;
      DskTlsPublicPrivateKeyShare *pp_share = shares + ks_index;
      unsigned shared_len = method->shared_key_bytes;
      hs_info->shared_key_length = shared_len;
      hs_info->shared_key = dsk_mem_pool_alloc (&hs_info->mem_pool, shared_len);
      if (!method->make_shared_key (method,
                                    pp_share->private_key,
                                    entry->key_exchange_data,
                                    hs_info->shared_key))
        {
          *error = dsk_error_new ("invalid key-share public data");
          return false;
        }
    }
  
  //
  // Compute read/write instances (which use different keys).
  //
  // note: We won't use the write_instance until WAIT_FINISHED,
  // but it seems weird to have it uninitialized;
  // but it does cause us to differ with state transition diagram
  // on page 119 of RFC 8446 (Appendix A.1).
  //
  DskTlsKeySchedule *schedule = hs_info->key_schedule;
  assert(hs_info->shared_key_length == cs->key_size);
  conn->cipher_suite_write_instance = dsk_malloc (cs->instance_size);
  cs->init (conn->cipher_suite_write_instance,
            schedule->client_application_write_key);
  conn->cipher_suite_read_instance = dsk_malloc (cs->instance_size);
  cs->init (conn->cipher_suite_read_instance,
            schedule->server_application_write_key);

  conn->state = DSK_TLS_CLIENT_CONNECTION_WAIT_ENCRYPTED_EXTENSIONS;
  return true;
}

//
// Deal with a HelloRetryRequest.
// This primarily indicates that the server wants a different KeyShare algo.
//
// The 6 allowed adjustments are enumerated in 4.1.2,
// which we copy here:
//
//   -  If a "key_share" extension was supplied in the HelloRetryRequest,
//      replacing the list of shares with a list containing a single
//      KeyShareEntry from the indicated group.
//
//   -  Removing the "early_data" extension (Section 4.2.10) if one was
//      present.  Early data is not permitted after a HelloRetryRequest.
//
//   -  Including a "cookie" extension if one was provided in the
//      HelloRetryRequest.
//
//   -  Updating the "pre_shared_key" extension if present by recomputing
//      the "obfuscated_ticket_age" and binder values and (optionally)
//      removing any PSKs which are incompatible with the server's
//      indicated cipher suite.
//
//   -  Optionally adding, removing, or changing the length of the
//      "padding" extension [RFC7685].
//
//   -  Other modifications that may be allowed by an extension defined in
//      the future and present in the HelloRetryRequest.
//
// We don't support adding padding or other extensions that
// modify HelloRetryRequest, so only the first 4 of these notes is needed.
//

static bool
handle_hello_retry_request (DskTlsConnection *conn,
                            DskTlsHandshakeMessage  *shake,
                            DskError        **error)
{
  DskTlsClientHandshake *hs_info = conn->handshake_info;
  DskTlsHandshakeMessage *client_hello = hs_info->client_hello;
  bool modify_key_shares = false;
  int use_key_share_index = -1;
  DskTlsNamedGroup key_share_generate;          // if modify_key_shares and use_key_share_index==-1
  if (conn->state != DSK_TLS_CLIENT_CONNECTION_WAIT_SERVER_HELLO)
    {
      *error = dsk_error_new ("bad state for HelloRetryRequest message: %s",
                              dsk_tls_connection_state_name (conn->state));
      DSK_ERROR_SET_TLS (*error, FATAL, UNEXPECTED_MESSAGE);
      return false;
    }
  if (hs_info->received_hello_retry_request)
    {
      // RFC 8446 Section 4.1.4, page 32:
      //    If a client receives a second HelloRetryRequest in
      //    the same connection (i.e., where the ClientHello
      //    was itself in response to a HelloRetryRequest),
      //    it MUST abort the handshake with an "unexpected_message" alert.
      *error = dsk_error_new ("only one HelloRetryRequest allowed");
      DSK_ERROR_SET_TLS (*error, FATAL, UNEXPECTED_MESSAGE);
      return false;
    }
  hs_info->received_hello_retry_request = true;

  //
  // Did the server select a named-group
  // from the supported_groups extension?
  //
  DskTlsExtension_KeyShare *hrr_ks = FIND_EXT_KEY_SHARE(shake);
  if (hrr_ks != NULL)
    {
      DskTlsExtension_KeyShare *ch_ks = FIND_EXT_KEY_SHARE(client_hello);
      DskTlsExtension_SupportedGroups *ch_sg = FIND_EXT_SUPPORTED_GROUPS(client_hello);
      unsigned sel = hrr_ks->selected_group;
      if (ch_sg == NULL || sel >= ch_sg->n_supported_groups)
        {
          *error = dsk_error_new ("selected_group in HelloRetryRequest is out-of-bounds");
          DSK_ERROR_SET_TLS (*error, FATAL, ILLEGAL_PARAMETER);
          return false;
        }
      DskTlsNamedGroup group = ch_sg->supported_groups[sel];

      // Did we already offer this as a KeyShare?
      unsigned orig_ks_index;
      for (orig_ks_index = 0;
           orig_ks_index < ch_ks->n_key_shares;
           orig_ks_index++)
        if (ch_ks->key_shares[orig_ks_index].named_group == group)
          break;
      if (orig_ks_index == ch_ks->n_key_shares)
        {
          // will need to generate new KeyShare.
          modify_key_shares = true;
          key_share_generate = group;
        }
      else
        {
          // only need to emit this one KeyShare.
          modify_key_shares = true;
          use_key_share_index = orig_ks_index;
        }
    }
  else
    {
      //
      // If there's no KeyShare extension, then the
      // client should just emit the same KeyShare it emitted the
      // first time.  (If we didn't provide a KeyShare the first time,
      // then abort.)
      //
      // (This will happen automatically below.)
      //
    }

  //
  // Send second ClientHello.
  //
  return send_second_client_hello_flight (hs_info,
                                          cs_code,
                                          modify_key_shares,
                                          use_key_share_index,
                                          key_share_generate,
                                          error);
}

static bool
handle_new_session_ticket  (DskTlsConnection *conn,
                            DskTlsHandshakeMessage  *shake,
                            DskError        **error)
{
  if (conn->state != DSK_TLS_CLIENT_CONNECTION_CONNECTED)
    {
      *error = dsk_error_new ("got NewSessionTicket before client handshake finished");
      DSK_ERROR_SET_TLS (*error, FATAL, UNEXPECTED_MESSAGE);
      return false;
    }

  if (conn->context->store_session_tickets (...))
    {
      ...
    }
  ...
}
static bool
handle_encrypted_extensions(DskTlsConnection *conn,
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
handle_certificate         (DskTlsConnection *conn,
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
handle_certificate_request (DskTlsConnection *conn,
                            DskTlsHandshakeMessage  *shake,
                            DskError        **error)
{
  if (conn->state != ...)
    {
      ...
    }
  ...
}
