#include "../dsk.h"
#include <string.h>
#include <alloca.h>
#include <stdio.h>
#include "dsk-tls-private.h"
#include "../dsk-list-macros.h"

#define MAX_TICKET_LIFETIME  (24 * 60 * 60 * 7)

//                            START <----+
//                               |        |
//                      v--------+        |
//                  FINDING_PSKS |        |
//                      |        |        |
//                      v        |        |
//             DONE_FINDING_PSKS |        |
//                      |        |        |
//                      +------->+        |
//                               |        |
//              Send ClientHello |        | Recv HelloRetryRequest
//         [K_send = early data] v        |
//                               v        |
//          /                 WAIT_SH ----+
//          |                    | Recv ServerHello
//          |                    | K_recv = handshake
//      Can |                    V
//     send |                 WAIT_EE
//    early |                    | Recv EncryptedExtensions
//     data |           +--------+--------+
//          |     Using |                 | Using certificate
//          |       PSK |                 v
//          |           |            WAIT_CERT_CR
//          |           |        Recv |       | Recv CertificateRequest
//          |           | Certificate |       v
//          |           |             |    FINDING_CLIENT_CERT
//          |           |             |       |
//          |           |             |       v
//          |           |             |  DONE_FINDING_CLIENT_CERT
//          |           |             |       |
//          |           |             |       v
//          |           |             |    WAIT_CERT
//          |           |             |       | Recv Certificate
//          |           |             v       v
//          |           |              WAIT_CV
//          |           |                 | 
//          |           |                 | Recv CertificateVerify
//          |           |                 | 
//          |           |              WAIT_CERT_VALIDATE
//          |           |                 |  (user callback being
//          |           |                 |   invoked)
//          |           |              CERT_VALIDATE_SUCCEEDED/FAILED
//          |           |                 |
//          |           +> WAIT_FINISHED <+
//          |                  | Recv Finished
//          \                  | [Send EndOfEarlyData]
//                             | K_send = handshake
//                             | [Send Certificate
//                             |     [+ CertificateVerify]]
//   Can send                  | Send Finished
//   app data   -->            | K_send = K_recv = application
//   after here                v
//                         CONNECTED
//

#define DSK_TLS_CLIENT_CONNECTION(conn) \
  DSK_OBJECT_CAST(DskTlsClientConnection, conn, &dsk_tls_client_connection_class)

struct DskTlsClientContext
{
  unsigned ref_count;

  //size_t n_certificates;
  //DskTlsKeyPair **certificates;

  // if we only support one cert type, we can advertise it
  // in the ClientHello message.
  //
  // Currently, we only support X509, so this can usually be done.
  bool has_client_cert_type;
  DskTlsCertificateType client_cert_type;

  size_t n_application_layer_protocols;
  const char **application_layer_protocols;
  bool application_layer_protocol_negotiation_required;

  bool support_early_data;

  // A comma-sep list of key-shares whose
  // public/private keys should be computed
  // in the initial ClientHello.
  const char *offered_key_share_groups;

  DskTlsClientLookupSessionsFunc lookup_sessions_func;
  void *lookup_sessions_data;

  DskTlsClientStoreSessionFunc store_session_func;
  void *store_session_data;

  DskTlsCertDatabase *cert_database;
  DskTlsClientVerifyServerCertFunc cert_verify_func;
  void *cert_verify_data;

  unsigned n_certificate_authorities;
  DskTlsX509DistinguishedName *certificate_authorities;
    
  //TODO: initialize these!!!!
  size_t n_supported_groups;
  DskTlsNamedGroup *supported_groups;

  DskTlsClientCertificateLookupFunc cert_lookup_func;
  void *cert_lookup_data;

  DskTlsGetCurrentTimeFunc get_time;
  void *get_time_data;

  char *server_name;

  DskRand *rng;

  bool heartbeat_allowed_from_server;

  bool ignore_certificate_expiration;
  bool allow_self_signed;
  DskTlsX509Certificate *pinned_cert;

  DskTlsClientWarnHandler warn_handler;
  void *warn_data;
  DskDestroyNotify warn_destroy;

  DskTlsClientFatalHandler fatal_handler;
  void *fatal_handler_data;
  DskDestroyNotify fatal_handler_destroy;
};
//
// Allocate memory and objects from the handshake-pool.
//
#define HS_ALLOC_DATA(hs_info, len) \
  (         dsk_mem_pool_alloc (&((hs_info)->base.mem_pool), (len)))
#define HS_ALLOC(hs_info, type) \
  ((type *) dsk_mem_pool_alloc (&((hs_info)->base.mem_pool), sizeof(type)))
#define HS_ALLOC0(hs_info, type) \
  ((type *) dsk_mem_pool_alloc (&((hs_info)->base.mem_pool), sizeof(type)))
#define HS_ALLOC_ARRAY(hs_info, count, type) \
  ((type *) dsk_mem_pool_alloc (&((hs_info)->base.mem_pool), sizeof(type) * (count)))
#define HS_ALLOC_ARRAY0(hs_info, count, type) \
  ((type *) dsk_mem_pool_alloc (&((hs_info)->base.mem_pool), sizeof(type) * (count)))

#define GET_HS_MESSAGE_LIST(hs_info) \
  DskTlsHandshakeMessage *, \
  (hs_info)->first_handshake, \
  (hs_info)->last_handshake, \
  next

static const char *
dsk_tls_client_connection_state_name (DskTlsClientConnectionState state)
{
  switch (state)
    {
#define WRITE_CASE(shortname) case DSK_TLS_CLIENT_CONNECTION_##shortname: return #shortname
      WRITE_CASE(START);
      WRITE_CASE(FINDING_PSKS);
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
#define STATE_NAME(evalue) dsk_tls_client_connection_state_name(evalue)

static DskTlsExtension *
find_extension_by_type (DskTlsHandshakeMessage *shake, DskTlsExtensionType type)
{
  unsigned n = shake->n_extensions;
  DskTlsExtension **arr = shake->extensions;
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
#define FIND_EXT_CERTIFICATE_AUTHORITIES(shake) \
        _FIND_EXT(shake, CertificateAuthorities, CERTIFICATE_AUTHORITIES)
#define FIND_EXT_EARLY_DATA_INDICATION(shake) \
        _FIND_EXT(shake, EarlyDataIndication, EARLY_DATA)
#define FIND_EXT_ALPN(shake) \
        _FIND_EXT(shake, ALPN, APPLICATION_LAYER_PROTOCOL_NEGOTIATION)
#define FIND_EXT_OID_FILTERS(shake) \
        _FIND_EXT(shake, OIDFilters, OID_FILTERS)
#define FIND_EXT_SIGNATURE_ALGORITHMS_CERT(shake) \
        _FIND_EXT(shake, SignatureAlgorithmsCert, SIGNATURE_ALGORITHMS_CERT)

#define MAX_RESPONSE_EXTENSIONS    16

static inline DskTlsHandshakeMessage *
create_handshake (DskTlsClientHandshake *hs_info,
                  DskTlsHandshakeMessageType type,
                  size_t max_extensions)
{
  return dsk_tls_base_handshake_create_outgoing_handshake (
    &hs_info->base,
    type,
    max_extensions
  );
}
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

static inline DskTlsClientConnection *
HS_INFO_GET_CONN(DskTlsClientHandshake *h)
{
  return DSK_TLS_CLIENT_CONNECTION (h->base.conn);
}

//
// Compute the initial ClientHello message.
//
#if 0
static bool
send_client_hello_flight (DskTlsClientHandshake *hs_info,
                          DskError **error)
{
  DskTlsHandshakeMessage *ch = dsk_tls_base_handshake_create_outgoing_handshake (
    &hs_info->base,
    DSK_TLS_HANDSHAKE_MESSAGE_TYPE_CLIENT_HELLO,
    16
  );
  DskTlsClientConnection *conn = HS_INFO_GET_CONN (hs_info);
  DskTlsClientContext *ctx = conn->context;
  hs_info->client_hello = ch;

  // Offered client PSKs
  if (ctx->lookup_sessions_func != NULL)
    {
      conn->state = DSK_TLS_CLIENT_CONNECTION_DOING_PSK_LOOKUP;
      ctx->lookup_sessions_func (conn, ctx->lookup_sessions_data);
      return true;
    }
  else
    return client_continue_post_psks (hs_info, error);
}
#endif
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
      uint8_t *priv_key = dsk_mem_pool_alloc (&hs_info->base.mem_pool, priv_key_size + pub_key_size);
      uint8_t *pub_key = priv_key + priv_key_size;
      do
        {
          dsk_rand_get_bytes (hs_info->rng, priv_key_size, priv_key);
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

// TODO: add Cookie extension???
static DskTlsBaseHandshakeMessageResponseCode
send_second_client_hello_flight (DskTlsClientHandshake *hs_info,
                                 DskTlsCipherSuiteCode cs_code,
                                 bool modify_key_shares,
                                 int use_key_share_index,
                                 DskTlsNamedGroup key_share_generate,
                                 DskError **error)
{
  DskTlsClientConnection *conn = HS_INFO_GET_CONN (hs_info);
  DskTlsClientContext *ctx = conn->context;
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
  sg->n_supported_groups = ctx->n_supported_groups;
  sg->supported_groups = ctx->supported_groups;
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
  hs_info->client_hello->n_extensions = hs_info->n_extensions;
  hs_info->client_hello->extensions = hs_info->extensions;

  // serialize
  if (!dsk_tls_handshake_message_serialize (hs_info->client_hello, &hs_info->base.mem_pool, error))
    return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;

  // send it!
  dsk_tls_record_layer_send_handshake (hs_info->base.conn, ch->data_length, ch->data);

  conn->state = DSK_TLS_CLIENT_CONNECTION_WAIT_SERVER_HELLO;

  return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_SUSPEND;
}

void
dsk_tls_handshake_client_error_finding_psks (DskTlsClientHandshake *hs_info,
                                             DskError *error)
{
  DskTlsClientConnection *conn = HS_INFO_GET_CONN (hs_info);
  assert(conn->state == DSK_TLS_CLIENT_CONNECTION_FINDING_PSKS);

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
  dst->binder_data = dsk_mem_pool_memdup (pool, src->binder_length, src->binder_data);
  dst->state_data = dsk_mem_pool_memdup (pool, src->state_length, src->state_data);
}

void
dsk_tls_handshake_client_found_psks (DskTlsClientHandshake *hs_info,
                                     size_t                      n_psks,
                                     DskTlsClientSessionInfo     *psks)
{
  DskTlsClientConnection *conn = HS_INFO_GET_CONN (hs_info);
  if (n_psks > 0)
    {
      hs_info->n_psks = n_psks;
      hs_info->psks = HS_ALLOC_ARRAY (hs_info, n_psks, DskTlsClientSessionInfo);
      for (size_t i = 0; i < n_psks; i++)
        tls_preshared_key_info_copy_to (&hs_info->base.mem_pool, hs_info->psks + i, psks + i);
    }

  DskError *error = NULL;
  if (!client_continue_post_psks (hs_info, &error))
    {
      dsk_tls_base_connection_fail (&conn->base_instance, error);
      dsk_object_unref (conn);
      dsk_error_unref (error);
      return;
    }
  dsk_object_unref (conn);
}

#if 0
static bool
client_continue_post_psks (DskTlsClientHandshake *hs_info,
                           DskError **error)
{
  DskTlsClientConnection *conn = HS_INFO_GET_CONN (hs_info);
  conn->state = DSK_TLS_CLIENT_CONNECTION_START;

  DskTlsHandshakeMessage *ch = hs_info->client_hello;
  ch->client_hello.legacy_version = 0x301;
  dsk_rand_get_bytes (hs_info->rng, 32, ch->client_hello.random);
  ch->client_hello.legacy_session_id_length = 32;
  dsk_rand_get_bytes (hs_info->rng, 32, ch->client_hello.legacy_session_id);
  ch->client_hello.n_cipher_suites = dsk_tls_n_cipher_suite_codes;
  ch->client_hello.cipher_suites = (DskTlsCipherSuiteCode *) dsk_tls_cipher_suite_codes;
  ch->client_hello.n_compression_methods = 0;
  ch->client_hello.compression_methods = NULL;

  // supported groups
  static const DskTlsNamedGroup client_supported_groups[] = {
    DSK_TLS_NAMED_GROUP_X25519,
    DSK_TLS_NAMED_GROUP_X448,
    DSK_TLS_NAMED_GROUP_SECP256R1,
    DSK_TLS_NAMED_GROUP_SECP384R1,
    DSK_TLS_NAMED_GROUP_SECP521R1,
    DSK_TLS_NAMED_GROUP_FFDHE2048,
    DSK_TLS_NAMED_GROUP_FFDHE3072,
    DSK_TLS_NAMED_GROUP_FFDHE4096,
    DSK_TLS_NAMED_GROUP_FFDHE6144,
    DSK_TLS_NAMED_GROUP_FFDHE8192,
  };
  DskTlsExtension_SupportedGroups *sg = HS_ALLOC (hs_info, DskTlsExtension_SupportedGroups);
  sg->base.type = DSK_TLS_EXTENSION_TYPE_SUPPORTED_GROUPS;
  sg->n_supported_groups = DSK_N_ELEMENTS (client_supported_groups);
  sg->supported_groups = (DskTlsNamedGroup *) client_supported_groups;
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

  DskTlsExtension_SignatureAlgorithms *sa = HS_ALLOC0 (hs_info, DskTlsExtension_SignatureAlgorithms);
  sa->base.type = DSK_TLS_EXTENSION_TYPE_SIGNATURE_ALGORITHMS;
  sa->n_schemes = DSK_N_ELEMENTS (signature_schemes);
  sa->schemes = (DskTlsSignatureScheme *) signature_schemes;
  hs_info->extensions[hs_info->n_extensions++] = (DskTlsExtension *) sa;

  // finish extensions
  hs_info->client_hello->n_extensions = hs_info->n_extensions;
  hs_info->client_hello->extensions = hs_info->extensions;

  // serialize
  if (!dsk_tls_handshake_message_serialize (hs_info->client_hello,
                                            &hs_info->base.mem_pool,
                                            error))
    return false;

  // send it!
  dsk_tls_record_layer_send_handshake (hs_info->base.conn,
                                       ch->data_length,
                                       ch->data);

  conn->state = DSK_TLS_CLIENT_CONNECTION_WAIT_SERVER_HELLO;

  return true;
}
#endif

//
// ServerHello should indicate that the server was able to come
// up with an agreeable set of parameters, we are ready to
// validate certs or move on to the application data.
//
static DskTlsBaseHandshakeMessageResponseCode
handle_server_hello (DskTlsClientConnection *conn,
                     DskTlsHandshakeMessage  *shake,
                     DskError        **error)
{
  DskTlsClientHandshake *hs_info = conn->base_instance.handshake;

  if (conn->state != DSK_TLS_CLIENT_CONNECTION_WAIT_SERVER_HELLO)
    {
      *error = dsk_error_new ("ServerHello not allowed in %s state",
                              STATE_NAME (conn->state));
      DSK_ERROR_SET_TLS (*error, FATAL, UNEXPECTED_MESSAGE);
      return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
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
      return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
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
          return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
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
      return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
    }
  conn->base_instance.cipher_suite = cs;

  DskTlsExtension_KeyShare *ks = FIND_EXT_KEY_SHARE (shake);
  if (ks == NULL)
    {
      if (!hs_info->has_psk)
        {
          *error = dsk_error_new ("PSK required if no KeyShare present");
          return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
        }
    }
  else
    {
      //
      // Compute shared-secret.
      //
      // Locate our key-share that corresponds 
      // to our client key-share.
      //
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
          return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
        }

      //
      // Invoke make_shared_key.
      //
      const DskTlsKeyShareMethod *method = shares[ks_index].method;
      DskTlsPublicPrivateKeyShare *pp_share = shares + ks_index;
      unsigned shared_len = method->shared_key_bytes;
      hs_info->shared_key_length = shared_len;
      hs_info->shared_key = dsk_mem_pool_alloc (&hs_info->base.mem_pool, shared_len);
      if (!method->make_shared_key (method,
                                    pp_share->private_key,
                                    entry->key_exchange_data,
                                    hs_info->shared_key))
        {
          *error = dsk_error_new ("invalid key-share public data");
          return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
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
  DskTlsBaseConnection *base = &conn->base_instance;
  base->cipher_suite_write_instance = dsk_malloc (cs->instance_size);
  cs->init (base->cipher_suite_write_instance,
            true,
            schedule->client_application_write_key);
  base->cipher_suite_read_instance = dsk_malloc (cs->instance_size);
  cs->init (base->cipher_suite_read_instance,
            false,
            schedule->server_application_write_key);

  conn->state = DSK_TLS_CLIENT_CONNECTION_WAIT_ENCRYPTED_EXTENSIONS;
  return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_OK;
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

static DskTlsBaseHandshakeMessageResponseCode
handle_hello_retry_request (DskTlsClientConnection *conn,
                            DskTlsHandshakeMessage  *shake,
                            DskError        **error)
{
  DskTlsClientHandshake *hs_info = conn->base_instance.handshake;
  DskTlsHandshakeMessage *client_hello = hs_info->client_hello;
  bool modify_key_shares = false;
  int use_key_share_index = -1;
  DskTlsNamedGroup key_share_generate=0;          // if modify_key_shares and use_key_share_index==-1
  if (conn->state != DSK_TLS_CLIENT_CONNECTION_WAIT_SERVER_HELLO)
    {
      *error = dsk_error_new ("bad state for HelloRetryRequest message: %s",
                              STATE_NAME (conn->state));
      DSK_ERROR_SET_TLS (*error, FATAL, UNEXPECTED_MESSAGE);
      return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
    }
  if (hs_info->received_hello_retry_request)
    {
      //
      // RFC 8446 Section 4.1.4, page 32:
      //
      //    If a client receives a second HelloRetryRequest in
      //    the same connection (i.e., where the ClientHello
      //    was itself in response to a HelloRetryRequest),
      //    it MUST abort the handshake with an "unexpected_message" alert.
      //
      *error = dsk_error_new ("only one HelloRetryRequest allowed");
      DSK_ERROR_SET_TLS (*error, FATAL, UNEXPECTED_MESSAGE);
      return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
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
          return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
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
      modify_key_shares = false;
    }

  //
  // Send second ClientHello.
  //
  return send_second_client_hello_flight (hs_info,
                                          shake->hello_retry_request.cipher_suite,
                                          modify_key_shares,
                                          use_key_share_index,
                                          key_share_generate,
                                          error);
}


//
// ClientState that must be stored for PSK use.
//
//
// Currently, the only data we save is the resumption_master_secret,
// because that is used as the PSK in the key-schedule.
//
// state format:
//
//     opaque<1..65535>         resumption_master_secret
//     uint32                   ticket_add_age    
//
static void
construct_client_state_for_identity (DskTlsClientConnection *conn,
                                     DskTlsClientHandshake  *hs_info,
                                     DskTlsHandshakeMessage *new_session_ticket,
                                     size_t                 *state_len_out,
                                     const uint8_t         **state_data_out  //allocated from hs_info->mem_pool
                                    )
{
  assert (conn->base_instance.handshake == hs_info);
  (void) new_session_ticket;

  unsigned hash_size = hs_info->key_schedule->cipher->hash_size;
  unsigned state_len = 2 + hash_size + 4;
  uint8_t *state = dsk_mem_pool_alloc (&hs_info->base.mem_pool, state_len);
  uint8_t *state_at = state;
  dsk_uint16le_pack (hash_size, state);
  state_at += 2;
  memcpy (state + 2, hs_info->key_schedule->resumption_master_secret, hash_size);
  state_at += hash_size;
  dsk_uint32le_pack (new_session_ticket->new_session_ticket.ticket_age_add, state_at);
  state_at += 4;
  assert (state + state_len == state_at);

  *state_len_out = state_len;
  *state_data_out = state;
}

//
// RFC 8446 4.2.11.2:
//
//     The PSK binder value forms a binding between a PSK and the current
//     handshake, as well as a binding between the handshake in which the
//     PSK was generated (if via a NewSessionTicket message) and the current
//     handshake.  Each entry in the binders list is computed as an HMAC
//     over a transcript hash (see Section 4.4.1) containing a partial
//     ClientHello up to and including the PreSharedKeyExtension.identities
//     field.  That is, it includes all of the ClientHello but not the
//     binders list itself.  The length fields for the message (including
//     the overall length, the length of the extensions block, and the
//     length of the "pre_shared_key" extension) are all set as if binders
//     of the correct lengths were present.
//  
//     The PskBinderEntry is computed in the same way as the Finished
//     message (Section 4.4.4) but with the BaseKey being the binder_key
//     derived via the key schedule from the corresponding PSK which is
//     being offered (see Section 7.1).
//  
//     If the handshake includes a HelloRetryRequest, the initial
//     ClientHello and HelloRetryRequest are included in the transcript
//     along with the new ClientHello.  For instance, if the client sends
//     ClientHello1, its binder will be computed over:
//  
//        Transcript-Hash(Truncate(ClientHello1))
//  
//     Where Truncate() removes the binders list from the ClientHello.
//  
//     If the server responds with a HelloRetryRequest and the client then
//     sends ClientHello2, its binder will be computed over:
//  
//        Transcript-Hash(ClientHello1,
//                        HelloRetryRequest,
//                        Truncate(ClientHello2))
//  
//     The full ClientHello1/ClientHello2 is included in all other handshake
//     hash computations.  Note that in the first flight,
//     Truncate(ClientHello1) is hashed directly, but in the second flight,
//     ClientHello1 is hashed and then reinjected as a "message_hash"
//     message, as described in Section 4.4.1.
//

static void
init_zero_binders (DskTlsClientHandshake *hs_info,
                   size_t *binder_length,
                   uint8_t **binder_data)
{
  const DskTlsCipherSuite *csuite = hs_info->base.conn->cipher_suite;
  *binder_length = csuite->hash_size;
  *binder_data = dsk_mem_pool_alloc0 (&hs_info->base.mem_pool, *binder_length);
}


static void
compute_binder (DskTlsClientHandshake *hs_info,
                uint8_t *binder_data_out)
{
  const DskTlsCipherSuite *csuite = hs_info->base.conn->cipher_suite;
  DskChecksumType *csum_type = csuite->hash_type;
  void *csum_instance = alloca (csum_type->instance_size);
  if (hs_info->received_hello_retry_request)
    {
      // Compute:
      //        Transcript-Hash(ClientHello1,
      //                        HelloRetryRequest,
      //                        Truncate(ClientHello2))
      // hash old_client_hello and server_hello: just copy the extant client-hello.

      // the current transcript-hash should contain ClientHello1 and HRR.
      memcpy (csum_instance,
              hs_info->base.transcript_hash_instance,
              csum_type->instance_size);
    }
  else
    {
      csum_type->init (csum_instance);
    }

  // Append Truncate(ClientHello) where we use the latest ClientHello
  // if more than one exists (in HelloRetryRequest case).
  dsk_tls_checksum_binder_truncated_handshake_message (csum_type, csum_instance,
                                               hs_info->client_hello);

  // Compute the hash.
  size_t hash_size = csum_type->hash_size;
  uint8_t *transcript_hash = alloca (hash_size);
  csum_type->end (csum_instance, transcript_hash);

  //
  // binder_key = HMAC(binder_key, transcript_hash).
  //
  DskTlsKeySchedule *schedule = hs_info->key_schedule;
  dsk_hmac_digest (csum_type,
                   hash_size, schedule->binder_key,
                   hash_size, transcript_hash,
                   binder_data_out);
}

static DskTlsBaseHandshakeMessageResponseCode
handle_new_session_ticket  (DskTlsClientConnection *conn,
                            DskTlsHandshakeMessage  *shake,
                            DskError        **error)
{
  DskTlsClientHandshake *hs_info = conn->base_instance.handshake;
  if (conn->state != DSK_TLS_CLIENT_CONNECTION_CONNECTED)
    {
      *error = dsk_error_new ("got NewSessionTicket before client handshake finished");
      DSK_ERROR_SET_TLS (*error, FATAL, UNEXPECTED_MESSAGE);
      return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
    }

  DskTlsClientSessionInfo session_info;
///    struct {
///      uint32_t ticket_lifetime;
///      uint32_t ticket_age_add;
///      uint32_t ticket_nonce_length;
///      const uint8_t *ticket_nonce;
///      unsigned ticket_length;
///      const uint8_t *ticket_data;
///      unsigned n_extensions;
///      DskTlsExtension **extensions;
///    } shake->new_session_ticket;

  unsigned lifetime = shake->new_session_ticket.ticket_lifetime;
  if (lifetime > MAX_TICKET_LIFETIME)
    lifetime = MAX_TICKET_LIFETIME;

  session_info.expiration = conn->context->get_time (conn->context->get_time_data) + lifetime;
  session_info.identity_length = shake->new_session_ticket.ticket_length;
  session_info.identity_data = shake->new_session_ticket.ticket_data;

  init_zero_binders (hs_info,
                     &session_info.binder_length,
                     (uint8_t **) &session_info.binder_data);

  construct_client_state_for_identity (conn, hs_info, shake,
                                       &session_info.state_length,
                                       &session_info.state_data);
  conn->context->store_session_func (hs_info,
                                     &session_info,
                                     conn->context->store_session_data);
  return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_OK;
}

static DskTlsBaseHandshakeMessageResponseCode
handle_encrypted_extensions(DskTlsClientConnection *conn,
                            DskTlsHandshakeMessage  *shake,
                            DskError        **error)
{
  if (conn->state != DSK_TLS_CLIENT_CONNECTION_WAIT_ENCRYPTED_EXTENSIONS)
    {
      *error = dsk_error_new ("got EncryptedExtensions before client handshake finished");
      DSK_ERROR_SET_TLS (*error, FATAL, UNEXPECTED_MESSAGE);
      return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
    }
  for (unsigned i = 0; i < shake->encrypted_extensions.n_extensions; i++)
    {
      DskTlsExtension *ext = shake->encrypted_extensions.extensions[i];
      switch (ext->type)
        {
        default:
          fprintf(stderr,
                  "extension 0x%x not handled in EncryptedExtensions message\n",
                  ext->type);
          break;
        }
    }
  return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_OK;
}

// Called after obtaining the Cert.
static DskTlsBaseHandshakeMessageResponseCode
done_getting_client_cert (DskTlsClientConnection *conn,
                          DskError **error);

static DskTlsBaseHandshakeMessageResponseCode 
handle_certificate_request (DskTlsClientConnection *conn,
                            DskTlsHandshakeMessage  *shake,
                            DskError        **error)
{
  if (conn->state != DSK_TLS_CLIENT_CONNECTION_WAIT_CERT_CR)
    {
      *error = dsk_error_new ("bad state for CertificateRequest message: %s",
                              STATE_NAME (conn->state));
      DSK_ERROR_SET_TLS (*error, FATAL, UNEXPECTED_MESSAGE);
      return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
    }
  DskTlsClientHandshake *hs_info = conn->base_instance.handshake;
  //unsigned req_context_length = shake->certificate_request.certificate_request_context_length;
  //const uint8_t *req_context = shake->certificate_request.certificate_request_context;
  //DskTlsExtension_StatusRequest *sr = FIND_EXT_STATUS_REQUEST (shake);
  DskTlsExtension_SignatureAlgorithms *sa = FIND_EXT_SIGNATURE_ALGORITHMS (shake);
  //DskTlsExtension_SignedCertificateTimestamp *sct = FIND_EXT_SIGNATURE_ALGORITHMS (shake);
  DskTlsExtension_CertificateAuthorities *ca = FIND_EXT_CERTIFICATE_AUTHORITIES (shake);
  DskTlsExtension_OIDFilters *oidf = FIND_EXT_OID_FILTERS (shake);
  DskTlsExtension_SignatureAlgorithmsCert *sac = FIND_EXT_SIGNATURE_ALGORITHMS_CERT (shake);

  if (sa == NULL && sac == NULL)
    {
      *error = dsk_error_new ("CertificateRequest must have SignatureAlgorithms or SignatureAlgorithmsCert extension");
      DSK_ERROR_SET_TLS (*error, FATAL, MISSING_EXTENSION);
      return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
    }

  DskTlsClientCertificateLookup lookup = {
    conn,
    hs_info,
    oidf ? oidf->n_filters : 0,
    oidf ? oidf->filters : NULL,
    ca ? ca->n_CAs : 0,
    ca ? ca->CAs : NULL,
    sac ? sac->n_schemes : sa->n_schemes,
    sac ? sac->schemes : sa->schemes,
  };
  conn->state = DSK_TLS_CLIENT_CONNECTION_FINDING_CLIENT_CERT;
  hs_info->doing_client_cert_lookup = true;
  assert (hs_info->invoking_user_handler);
  hs_info->invoking_user_handler = true;
  conn->context->cert_lookup_func (&lookup, conn->context->cert_lookup_data);

  hs_info->invoking_user_handler = false;
  if (conn->state == DSK_TLS_CLIENT_CONNECTION_FINDING_CLIENT_CERT)
    return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_SUSPEND;
  return done_getting_client_cert (conn, error);
}

// Second half of CertificateRequest handler.
static DskTlsBaseHandshakeMessageResponseCode
done_finding_client_cert (DskTlsClientConnection *conn,
                          DskError **error)
{
  // if the handler terminated without blocking, process the results.
  DskTlsClientHandshake *hs_info = (DskTlsClientHandshake*) conn->base_instance.handshake;
  assert (conn->state == DSK_TLS_CLIENT_CONNECTION_DONE_FINDING_CLIENT_CERT);
  if (hs_info->client_certificate == NULL)
    {
      *error = dsk_error_new ("no client-side certificate found to satisfy server request");
      DSK_ERROR_SET_TLS (*error, FATAL, CERTIFICATE_REQUIRED);
      return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
    }
  conn->state = DSK_TLS_CLIENT_CONNECTION_WAIT_CERT;
  return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_OK;
}

void
dsk_tls_client_connection_certificate_found (DskTlsClientConnection *conn,
                                             DskTlsCertChain *chain)
{
  DskTlsClientHandshake *hs_info = (DskTlsClientHandshake*) conn->base_instance.handshake;
  assert (conn->state == DSK_TLS_CLIENT_CONNECTION_FINDING_CLIENT_CERT);
  assert (hs_info->client_certificate == NULL);
  hs_info->client_certificate = dsk_tls_cert_chain_ref (chain);
  conn->state = DSK_TLS_CLIENT_CONNECTION_DONE_FINDING_CLIENT_CERT;

  if (!hs_info->invoking_user_handler)
    dsk_tls_base_connection_unsuspend (&conn->base_instance);
}
void dsk_tls_client_connection_certificate_not_found (DskTlsClientConnection *conn)
{
  DskTlsClientHandshake *hs_info = (DskTlsClientHandshake*) conn->base_instance.handshake;
  assert (conn->state == DSK_TLS_CLIENT_CONNECTION_FINDING_CLIENT_CERT);
  assert (hs_info->client_certificate == NULL);
  conn->state = DSK_TLS_CLIENT_CONNECTION_DONE_FINDING_CLIENT_CERT;

  if (!hs_info->invoking_user_handler)
    dsk_tls_base_connection_unsuspend (&conn->base_instance);
}

static DskTlsBaseHandshakeMessageResponseCode 
handle_certificate         (DskTlsClientConnection *conn,
                            DskTlsHandshakeMessage  *shake,
                            DskError        **error)
{
  if (conn->state != DSK_TLS_CLIENT_CONNECTION_WAIT_CERT_CR
   && conn->state != DSK_TLS_CLIENT_CONNECTION_WAIT_CERT)
    {
      *error = dsk_error_new ("bad state for Certificate message: %s",
                              STATE_NAME (conn->state));
      DSK_ERROR_SET_TLS (*error, FATAL, UNEXPECTED_MESSAGE);
      return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
    }
  DskTlsClientHandshake *hs_info = conn->base_instance.handshake;

  hs_info->certificate_hs = shake;
  conn->state = DSK_TLS_CLIENT_CONNECTION_WAIT_CV;
  return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_OK;
}

static void
default_cert_verify (DskTlsClientVerifyServerCertInfo *info,
                     void *cert_verify_data)
{
  DskError *err = NULL;
  DskTlsClientConnection *conn = info->connection;
  (void) cert_verify_data;
  if (!info->in_time_range
   && !conn->context->ignore_certificate_expiration)
    {
      err = dsk_error_new ("certificate expired");
      DSK_ERROR_SET_TLS (err, FATAL, CERTIFICATE_EXPIRED);
    }
  if (err == NULL
   && info->self_signed
   && !conn->context->allow_self_signed)
    {
      err = dsk_error_new ("self-signed certificate not allowed");
      DSK_ERROR_SET_TLS (err, FATAL, UNSUPPORTED_CERTIFICATE);
    }

  if (err == NULL
   && conn->context->pinned_cert != NULL
   && !dsk_tls_x509_certificates_match (conn->context->pinned_cert, info->certs[0].cert))
    {
      err = dsk_error_new ("certificate does not match pinned cert");
      DSK_ERROR_SET_TLS (err, FATAL, UNSUPPORTED_CERTIFICATE);
    }

  if (err == NULL)
    return dsk_tls_client_connection_server_cert_notify_ok(conn);

  dsk_tls_client_connection_server_cert_notify_error (info->connection, err);
  dsk_error_unref (err);
}

static DskTlsBaseHandshakeMessageResponseCode
done_wait_cert_validate    (DskTlsClientConnection  *conn,
                            DskError               **error);

static DskTlsBaseHandshakeMessageResponseCode 
handle_certificate_verify  (DskTlsClientConnection *conn,
                            DskTlsHandshakeMessage  *shake,
                            DskError        **error)
{
  DskTlsBaseConnection *base = &conn->base_instance;
  DskTlsClientHandshake *hs_info = base->handshake;
  DskTlsKeySchedule *schedule = hs_info->key_schedule;
  const DskTlsCipherSuite *csuite = conn->base_instance.cipher_suite;
  DskChecksumType *csum_type = csuite->hash_type;
  uint8_t *transcript_hash = alloca (csum_type->hash_size);
  dsk_tls_base_connection_get_transcript_hash (base, transcript_hash);
  dsk_tls_key_schedule_compute_server_verify_data (schedule, transcript_hash);

  unsigned n_entries = shake->certificate.n_entries;
  DskTlsCertificateEntry *entries = shake->certificate.entries;
  if (conn->state != DSK_TLS_CLIENT_CONNECTION_WAIT_CV)
    {
      *error = dsk_error_new ("bad state for CertificateVerify message: %s",
                              STATE_NAME (conn->state));
      DSK_ERROR_SET_TLS (*error, FATAL, UNEXPECTED_MESSAGE);
      return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
    }
 
  uint64_t cur_time = conn->context->get_time (conn->context->get_time_data);
  for (unsigned i = 1; i < n_entries; i++)
    {
      DskTlsX509Certificate *cert0 = entries[i-1].cert;
      DskTlsX509Certificate *cert1 = entries[i].cert;
      if (!dsk_tls_x509_certificate_verify (cert0, cert1))
        {
          *error = dsk_error_new ("bad state for CertificateVerify message: %s",
                                  STATE_NAME (conn->state));
          DSK_ERROR_SET_TLS (*error, FATAL, BAD_CERTIFICATE);
          return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
        }
    }

  bool in_time_range = true;
  for (unsigned i = 0; i < n_entries; i++)
    {
      DskTlsX509Certificate *cert = entries[i].cert;
      DskTlsX509Validity *validity = &cert->validity;
      if (cur_time < validity->not_before)
        {
          in_time_range = false;
        }
      if (cur_time > validity->not_after)
        {
          in_time_range = false;
        }
    }

  //
  // Validate signature and
  // compute DskTlsClientVerifyServerCertInfo.
  //
  bool chain_found = false;
  DskTlsX509Certificate *last_cert = entries[n_entries-1].cert;
  size_t n_db_certs = 0;
  DskTlsX509Certificate **db_certs = NULL;
  if (conn->context->cert_database != NULL)
    {
      switch (dsk_tls_cert_database_validate_cert (
                           conn->context->cert_database,
                           last_cert,
                           &n_db_certs,
                           &db_certs))
       {
       case DSK_TLS_CERT_DATABASE_VALIDATE_OK:
         chain_found = true;
         break;
       case DSK_TLS_CERT_DATABASE_VALIDATE_BAD_SIGNATURE:
         *error = dsk_error_new ("bad signature");
         DSK_ERROR_SET_TLS (*error, FATAL, BAD_CERTIFICATE);
         return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
       case DSK_TLS_CERT_DATABASE_VALIDATE_NO_CERT_FOUND:
         break;
       case DSK_TLS_CERT_DATABASE_VALIDATE_CERT_DB_ERROR:
         *error = dsk_error_new ("bad certificate database");
         DSK_ERROR_SET_TLS (*error, FATAL, BAD_CERTIFICATE);
         return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
       }
    }
  else
    {
      // no chain validation
    }

  bool self_signed = last_cert != NULL
                  && last_cert->is_signed
                  && last_cert->has_issuer_unique_id 
                  && last_cert->has_subject_unique_id 
                  && last_cert->issuer_unique_id_len
                     == last_cert->subject_unique_id_len
                  && memcmp (last_cert->issuer_unique_id,
                             last_cert->subject_unique_id,
                             last_cert->subject_unique_id_len * 4) == 0;

  //
  // Test if hostname matches ServerName extension if any.
  //
  if (conn->context->server_name != NULL)
    {
      DskTlsX509Certificate *cert = entries[0].cert;
      DskTlsX509DistinguishedName *name = &cert->subject;
      const char *cn = dsk_tls_x509_distinguished_name_get_component (name, DSK_TLS_X509_RDN_COMMON_NAME);
      if (cn == NULL)
        {
          *error = dsk_error_new ("bad certificate: no server_name");
          DSK_ERROR_SET_TLS (*error, FATAL, BAD_CERTIFICATE);
          return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
        }
      if (strcasecmp (cn, conn->context->server_name) != 0)
        {
          *error = dsk_error_new ("bad certificate: wrong server name: got %s, expected %s",
                                  cn, conn->context->server_name);
          DSK_ERROR_SET_TLS (*error, FATAL, BAD_CERTIFICATE);
          return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
        }
    }

  DskTlsClientVerifyServerCertInfo verify_info;
  memset(&verify_info, 0, sizeof(verify_info));
  verify_info.ok = chain_found && in_time_range;
  verify_info.cert_chain_found = chain_found;
  verify_info.in_time_range = in_time_range;
  verify_info.self_signed = self_signed;
  verify_info.n_certs = n_entries;
  verify_info.certs = entries;
  verify_info.n_db_certs = n_db_certs;
  verify_info.db_certs = db_certs;
  verify_info.cert = entries[0].cert;
  verify_info.connection = conn;
  DskTlsClientVerifyServerCertFunc cert_verify_func =
       conn->context->cert_verify_func != NULL
       ? conn->context->cert_verify_func
       : default_cert_verify;
  //
  // Invoke callback to see if certificate is issued
  // by acceptable authority.
  //
  conn->state = DSK_TLS_CLIENT_CONNECTION_WAIT_CERT_VALIDATE;
  hs_info->invoking_user_handler = true;
  cert_verify_func (&verify_info,
                    conn->context->cert_verify_data);
  hs_info->invoking_user_handler = false;
  if (conn->state == DSK_TLS_CLIENT_CONNECTION_WAIT_CERT_VALIDATE)
    return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_SUSPEND;

  return done_wait_cert_validate (conn, error);
}

/* Part 2 of the CertificateVerify handler. */
static DskTlsBaseHandshakeMessageResponseCode
done_wait_cert_validate    (DskTlsClientConnection  *conn,
                            DskError               **error)
{
  DskTlsClientHandshake *hs_info = conn->base_instance.handshake;
  switch (conn->state)
    {
    case DSK_TLS_CLIENT_CONNECTION_CERT_VALIDATE_SUCCEEDED:
      conn->state = DSK_TLS_CLIENT_CONNECTION_WAIT_FINISHED;
      return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_OK;

    case DSK_TLS_CLIENT_CONNECTION_CERT_VALIDATE_FAILED:
      assert(hs_info->tmp_error != NULL);
      *error = hs_info->tmp_error;
      hs_info->tmp_error = NULL;
      return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;

    default:
      assert(false);
      return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
    }

}

static DskTlsBaseHandshakeMessageResponseCode 
handle_key_update          (DskTlsClientConnection *conn,
                            DskTlsHandshakeMessage  *shake,
                            DskError        **error)
{
  //
  // RFC 8446, Section 4.6.3:
  //
  //     This message can be sent by either peer after it has sent
  //     a Finished message.  Implementations that receive a
  //     KeyUpdate message prior to receiving a Finished message
  //     MUST terminate the connection with an "unexpected_message"
  //     alert.
  //
  if (conn->state != DSK_TLS_CLIENT_CONNECTION_CONNECTED)
    {
      *error = dsk_error_new ("bad state for KeyUpdate message: %s",
                              STATE_NAME (conn->state));
      DSK_ERROR_SET_TLS (*error, FATAL, UNEXPECTED_MESSAGE);
      return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
    }

  //
  // Update key.
  //
  DskTlsBaseConnection *base = &conn->base_instance;
  dsk_tls_update_key_inplace (base->cipher_suite->hash_type,
                              base->server_application_traffic_secret);
  base->cipher_suite->init (base->cipher_suite_read_instance,
                            false,               // for decryption
                            base->server_application_traffic_secret);

  if (shake->key_update.update_requested)
    dsk_tls_base_connection_send_key_update (&conn->base_instance, false);
  return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_OK;
}

static void
handshake_get_transcript_hash (DskTlsClientHandshake *hs_info,
                               uint8_t *hash_out)
{
  dsk_tls_base_connection_get_transcript_hash (
    hs_info->base.conn,
    hash_out
  );
}

//
// RFC 8446, Section 4.4.4 Finished.
//
static DskTlsBaseHandshakeMessageResponseCode
handle_finished (DskTlsClientConnection *conn,
                 DskTlsHandshakeMessage *message,
                 DskError              **error)
{
  DskTlsBaseConnection *base = &conn->base_instance;
  DskTlsClientHandshake *hs_info = base->handshake;
  const DskChecksumType *csum_type = base->cipher_suite->hash_type;
  DskTlsKeySchedule *schedule = hs_info->key_schedule;
  //DskTlsClientContext *context = conn->context;

  //
  // Update read cipher according to key-schedule.
  //
  uint8_t *transcript_hash = alloca (csum_type->hash_size);
  dsk_tls_base_connection_get_transcript_hash (base, transcript_hash);
  dsk_tls_key_schedule_compute_master_secrets (schedule, transcript_hash);
  base->cipher_suite->init(base->cipher_suite_read_instance,
                           false,               // for decryption
                           schedule->master_secret);

  //
  // Verify verify_data.
  //
  //     "Recipients of Finished messages MUST verify that
  //      the contents are correct and if incorrect MUST terminate
  //      the connection with a "decrypt_error" alert."
  //
  if (message->finished.verify_data_length != csum_type->hash_size)
    {
      *error = dsk_error_new ("bad verification data length");
      DSK_ERROR_SET_TLS (*error, FATAL, DECRYPT_ERROR);
      return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
    }
  if (!schedule->has_server_finish_data)
    {
      *error = dsk_error_new ("missing CertificateVerify");
      DSK_ERROR_SET_TLS (*error, FATAL, UNEXPECTED_MESSAGE);
      return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
    }
  if (memcmp (schedule->server_verify_data,
              message->finished.verify_data,
              csum_type->hash_size) != 0)
    {
      *error = dsk_error_new ("bad incoming verify data");
      DSK_ERROR_SET_TLS (*error, FATAL, DECRYPT_ERROR);
      return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
    }

  // All the messages between the server Finished and the client Finished
  // are optional.  Use this flag to determine if any optional messages are sent.
  //
  // If so, the transcript_hash must be recomputed.
  bool sent_message_after_server_finished = false;

  // Maybe send end-of-early-data
  if (hs_info->allow_early_data)
    {
      sent_message_after_server_finished = true;
      DskTlsHandshakeMessage *eoed_message = HS_ALLOC0 (hs_info, DskTlsHandshakeMessage);
      eoed_message->type = DSK_TLS_HANDSHAKE_MESSAGE_TYPE_END_OF_EARLY_DATA;
      eoed_message->is_outgoing = true;
      if (!dsk_tls_base_connection_send_handshake_msg (base, eoed_message, error))
        return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
    }

  //
  // Client Certificate and CertificateVerify.
  //
  // [These are for the relatively unusual
  //  client-side certificates.]
  //
  if (hs_info->client_certificate != NULL)
    {
      sent_message_after_server_finished = true;

      DskTlsHandshakeMessage *cert_message = HS_ALLOC0 (hs_info, DskTlsHandshakeMessage);
      cert_message->type = DSK_TLS_HANDSHAKE_MESSAGE_TYPE_CERTIFICATE;
      cert_message->is_outgoing = true;
      DskTlsCertChain *ccert = hs_info->client_certificate;
      cert_message->certificate.n_entries = ccert->length;
      cert_message->certificate.entries = ccert->chain;

      if (!dsk_tls_base_connection_send_handshake_msg (base, cert_message, error))
        return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;

      DskTlsHandshakeMessage *cv_message = HS_ALLOC0 (hs_info, DskTlsHandshakeMessage);
      cv_message->type = DSK_TLS_HANDSHAKE_MESSAGE_TYPE_CERTIFICATE_VERIFY;
      cv_message->is_outgoing = true;

      // Find signing algorithm for
      // the client's certificate 'ccert'.
      DskTlsKeyPair *ccert_kp = ccert->key_pair;
      size_t n_ccert_schemes = ccert_kp->n_supported_schemes;
      const DskTlsSignatureScheme *ccert_schemes_at = ccert_kp->supported_schemes;
      size_t ccert_scheme_index;
      for (ccert_scheme_index = 0;
           ccert_scheme_index < n_ccert_schemes;
           ccert_scheme_index++, ccert_schemes_at++)
        if (dsk_tls_key_pair_supports_scheme (ccert_kp, *ccert_schemes_at))
          break;
      if (ccert_scheme_index == n_ccert_schemes)
        {
          *error = dsk_error_new ("no matching client sig scheme found");
          DSK_ERROR_SET_TLS (*error, FATAL, CERTIFICATE_REQUIRED);
          return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
        }
      DskTlsSignatureScheme algorithm = *ccert_schemes_at;
      cv_message->certificate_verify.algorithm = algorithm;

      //
      // The digital signature is then computed over the concatenation of:
      //
      //  -  A string that consists of octet 32 (0x20) repeated 64 times
      //  -  The context string ("TLS 1.3, client CertificateVerify")
      //  -  A single 0 byte which serves as the separator
      //  -  The content to be signed
      // 
      dsk_tls_base_connection_get_transcript_hash (base, transcript_hash);
      const char context[] = "TLS 1.3, client CertificateVerify";
      size_t context_and_nul_len = sizeof(context);
      size_t to_sign_length = 64
                            + context_and_nul_len
                            + csum_type->hash_size;
      uint8_t *to_sign = alloca (to_sign_length);
      memset (to_sign, 0x20, 64);
      memcpy (to_sign + 64, context, context_and_nul_len);
      memcpy (to_sign + 64 + context_and_nul_len, transcript_hash,
              csum_type->hash_size);
      size_t sig_length = dsk_tls_key_pair_get_signature_length (ccert_kp, algorithm);

      uint8_t *sig_data = HS_ALLOC_DATA (hs_info, sig_length);
      cv_message->certificate_verify.signature_data = sig_data;
      cv_message->certificate_verify.signature_length = sig_length;
      dsk_tls_key_pair_sign (ccert_kp, algorithm,
                             to_sign_length, to_sign,
                             sig_data);
      if (!dsk_tls_base_connection_send_handshake_msg (base, cv_message, error))
        return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
    }

  // Send Finished
  if (sent_message_after_server_finished)
    handshake_get_transcript_hash (hs_info, transcript_hash);
  dsk_tls_key_schedule_compute_client_verify_data (schedule, transcript_hash);
  DskTlsHandshakeMessage *c_finished = HS_ALLOC0 (hs_info, DskTlsHandshakeMessage);
  c_finished->type = DSK_TLS_HANDSHAKE_MESSAGE_TYPE_FINISHED;
  c_finished->is_outgoing = true;
  c_finished->finished.verify_data_length = csum_type->hash_size;
  c_finished->finished.verify_data = schedule->client_verify_data;
  if (!dsk_tls_base_connection_send_handshake_msg (base, c_finished, error))
    return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;

  // Compute resumption secret.
  handshake_get_transcript_hash (hs_info, transcript_hash);
  dsk_tls_key_schedule_compute_resumption_secret (schedule, transcript_hash);

  conn->state = DSK_TLS_CLIENT_CONNECTION_CONNECTED;
  conn->base_instance.handshake = NULL;
  dsk_tls_base_handshake_free (&hs_info->base);
  return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_OK;
}


static DskTlsBaseHandshakeMessageResponseCode
dsk_tls_client_connection_handle_handshake_message
                         (DskTlsBaseConnection   *connection,
                          DskTlsHandshakeMessage *message,
                          DskError              **error)
{
  DskTlsClientConnection *cconn = DSK_TLS_CLIENT_CONNECTION (connection);
  switch (message->type)
    {
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_SERVER_HELLO:
      if (message->server_hello.is_retry_request)
        return handle_hello_retry_request (cconn, message, error);
      else
        return handle_server_hello (cconn, message, error);
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_NEW_SESSION_TICKET:
      return handle_new_session_ticket (cconn, message, error);
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_ENCRYPTED_EXTENSIONS:
      return handle_encrypted_extensions (cconn, message, error);
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_CERTIFICATE_REQUEST:
      return handle_certificate_request (cconn, message, error);
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_CERTIFICATE:
      return handle_certificate (cconn, message, error);
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_CERTIFICATE_VERIFY:
      return handle_certificate_verify (cconn, message, error);
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_FINISHED:
      return handle_finished (cconn, message, error);
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_KEY_UPDATE:
      return handle_key_update (cconn, message, error);

    // All unhandled message types are invalid because they are
    // unexpected.
    default: break;
    }
  *error = dsk_error_new ("invalid handshake message received by client: %02x", message->type);
  return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
}

static DskTlsBaseHandshakeMessageResponseCode
done_maybe_finding_psks    (DskTlsClientConnection  *conn,
                            DskError               **error);

static DskTlsBaseHandshakeMessageResponseCode
dsk_tls_client_connection_handle_unsuspend (DskTlsBaseConnection *conn,
                                            DskError              **error)
{
  DskTlsClientConnection *cconn = DSK_TLS_CLIENT_CONNECTION (conn);
  switch (cconn->state)
    {
    case DSK_TLS_CLIENT_CONNECTION_DONE_FINDING_PSKS:
      return done_maybe_finding_psks (cconn, error);
    case DSK_TLS_CLIENT_CONNECTION_WAIT_CERT_VALIDATE:
      return done_wait_cert_validate (cconn, error);
    case DSK_TLS_CLIENT_CONNECTION_DONE_FINDING_CLIENT_CERT:
      return done_finding_client_cert (cconn, error);
    default:
      *error = dsk_error_new ("bad state for handle-unsuspend");
      return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
    }
}

static bool
dsk_tls_client_connection_handle_application_data
                        (DskTlsBaseConnection   *connection,
                         size_t                  length,
                         const uint8_t          *data,
                         DskError              **error)
{
  DskTlsClientConnection *conn = DSK_TLS_CLIENT_CONNECTION (connection);
  if (conn->state != DSK_TLS_CLIENT_CONNECTION_CONNECTED)
    {
      *error = dsk_error_new ("bad state for application-data: %s",
                              STATE_NAME (conn->state));
      DSK_ERROR_SET_TLS (*error, FATAL, UNEXPECTED_MESSAGE);
      return false;
    }

  (void) error;

  dsk_buffer_append (&connection->incoming_plaintext, length, data);
  return true;
}

static void
dsk_tls_client_connection_finalize (DskTlsClientConnection *conn)
{
  DskTlsClientHandshake *hs_info = conn->base_instance.handshake;
  if (hs_info != NULL)
    {
      dsk_tls_base_handshake_free (&hs_info->base);
      conn->base_instance.handshake = NULL;
    }
}

static void 
dsk_tls_client_connection_fatal_alert_received
                     (DskTlsBaseConnection      *connection,
                      DskTlsAlertDescription     description)
{
  DskTlsClientConnection *conn = DSK_TLS_CLIENT_CONNECTION (connection);
  if (conn->context->fatal_handler != NULL)
    conn->context->fatal_handler (conn,
                                  description,
                                  conn->context->fatal_handler_data);
}

static bool
dsk_tls_client_connection_warning_alert_received
                       (DskTlsBaseConnection    *connection,
                        DskTlsAlertDescription   description,
                        DskError               **error)
{
  DskTlsClientConnection *conn = DSK_TLS_CLIENT_CONNECTION (connection);
  if (conn->context->warn_handler != NULL)
    return conn->context->warn_handler (conn, description, conn->context->warn_data, error);
  else
    return true;
}

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskTlsClientConnection);
DskTlsClientConnectionClass dsk_tls_client_connection_class = {
  DSK_TLS_BASE_CONNECTION_DEFINE_CLASS(
    DskTlsClientConnection,
    dsk_tls_client_connection
  )
};

#define ADD_EXT(m, e) dsk_tls_handshake_message_add_extension(m, e, &hs_info->base.mem_pool)

DskTlsClientConnection *
dsk_tls_client_connection_new (DskStream     *underlying,
                               DskTlsClientContext *context,
                               DskError     **error)
{
  DskTlsClientConnection *rv = dsk_object_new (&dsk_tls_client_connection_class);
  DskTlsBaseConnection *rv_base = (DskTlsBaseConnection *) rv;
  if (!dsk_tls_base_connection_init_underlying (&rv->base_instance, underlying, error))
    {
      dsk_object_unref (rv);
      return NULL;
    }

  //
  // Create handshake-info.
  //
  DskTlsClientHandshake *hs_info = DSK_NEW0 (DskTlsClientHandshake);
  hs_info->base.conn = rv_base;
  hs_info->rng = context->rng ? context->rng : dsk_rand_get_global();
  rv->base_instance.handshake = &hs_info->base;
  //...

  //
  // May need to compute PSK binders.
  // This could suspend.
  //
  if (context->lookup_sessions_func != NULL)
    {
      rv->state = DSK_TLS_CLIENT_CONNECTION_FINDING_PSKS;
      hs_info->invoking_user_handler = true;
      context->lookup_sessions_func (rv, context->lookup_sessions_data);
      hs_info->invoking_user_handler = false;
      if (rv->state == DSK_TLS_CLIENT_CONNECTION_FINDING_PSKS)
        {
          // suspend!
          dsk_tls_base_connection_init_suspend (&rv->base_instance);
          return rv;
        }
      if (rv->state != DSK_TLS_CLIENT_CONNECTION_DONE_FINDING_PSKS)
        {
          dsk_object_unref (rv);
          *error = dsk_error_new ("invalid state when finding pre-shared-keys");
          return NULL;
        }
    }

  switch (done_maybe_finding_psks (rv, error))
    {
    case DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_SUSPEND:
      assert(false);
    case DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_OK:
      break;
    case DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED:
      dsk_object_unref (rv);
      return NULL;
    }

  return rv;
}

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

static const DskTlsCertificateType supported_cert_types[] = {
  DSK_TLS_CERTIFICATE_TYPE_X509
};

static long long now_in_millis (void)
{
  DskDispatch *d = dsk_dispatch_default ();
  return (unsigned long long) d->last_dispatch_secs * 1000
       + (unsigned long long) d->last_dispatch_usecs / 1000;
} 

// Compute and transmit the ClientHello message.
static DskTlsBaseHandshakeMessageResponseCode
done_maybe_finding_psks (DskTlsClientConnection *conn,
                         DskError **error)
{
  DskTlsClientHandshake *hs_info = conn->base_instance.handshake;
  DskTlsClientContext *context = conn->context;

  //
  // Create ClientHello message.
  //
  DskTlsHandshakeMessage *c_hello
  = dsk_tls_base_handshake_create_outgoing_handshake (
      conn->base_instance.handshake,
      DSK_TLS_HANDSHAKE_MESSAGE_TYPE_CLIENT_HELLO,
      8
    );

  //
  // Populate members.
  // 
  c_hello->client_hello.legacy_version = 0x0303;
  dsk_rand_get_bytes (hs_info->rng, 32, c_hello->client_hello.random);
  c_hello->client_hello.legacy_session_id_length = 0;
  c_hello->client_hello.n_cipher_suites = dsk_tls_n_cipher_suite_codes;
  c_hello->client_hello.cipher_suites = (DskTlsCipherSuiteCode *) dsk_tls_cipher_suite_codes;
  c_hello->client_hello.n_compression_methods = 0;
  c_hello->client_hello.compression_methods = NULL;


  // extension: server_name
  if (context->server_name != NULL)
    {
      DskTlsExtension_ServerNameList *sn = HS_ALLOC0(hs_info, DskTlsExtension_ServerNameList);
      sn->base.type = DSK_TLS_EXTENSION_TYPE_SERVER_NAME;
      sn->n_entries = 1;
      sn->entries = HS_ALLOC_ARRAY(hs_info, sn->n_entries, DskTlsServerNameListEntry);
      sn->entries[0].type = DSK_TLS_SERVER_NAME_TYPE_HOSTNAME;
      sn->entries[0].name_length = strlen (context->server_name);
      sn->entries[0].name = (char*) (context->server_name);
     ADD_EXT (c_hello, (DskTlsExtension *) sn);
    }

  // NOT SUPPORTED: extension: max_fragment_length

  // extension: heartbeat
  if (context->heartbeat_allowed_from_server)
    {
      DskTlsExtension_Heartbeat *hb = HS_ALLOC0 (hs_info, DskTlsExtension_Heartbeat);
      hb->base.type = DSK_TLS_EXTENSION_TYPE_HEARTBEAT;
      hb->mode = DSK_TLS_EXTENSION_HEARTBEAT_MODE_ALLOWED;
      ADD_EXT (c_hello, (DskTlsExtension *) hb);
    }
  // NOT SUPPORTED: extension: status_request

  // extension: supported_groups
  DskTlsExtension_SupportedGroups *sg = HS_ALLOC0 (hs_info, DskTlsExtension_SupportedGroups);
  sg->base.type = DSK_TLS_EXTENSION_TYPE_SUPPORTED_GROUPS;
  sg->n_supported_groups = context->n_supported_groups;
  sg->supported_groups = context->supported_groups;
  ADD_EXT (c_hello, (DskTlsExtension *) sg);

  // extension: signature_algorithm
  DskTlsExtension_SignatureAlgorithms *salgos = HS_ALLOC (hs_info, DskTlsExtension_SignatureAlgorithms);
  salgos->base.type = DSK_TLS_EXTENSION_TYPE_SIGNATURE_ALGORITHMS;
  salgos->n_schemes = DSK_N_ELEMENTS (signature_schemes);
  salgos->schemes = signature_schemes;
  ADD_EXT (c_hello, (DskTlsExtension *) salgos);

  // NOT SUPPORTED: extension: use_srtp

  // extension: alpn
  if (context->application_layer_protocol_negotiation_required)
    {
      DskTlsExtension_ALPN *alpn = HS_ALLOC0 (hs_info, DskTlsExtension_ALPN);
      alpn->base.type = DSK_TLS_EXTENSION_TYPE_APPLICATION_LAYER_PROTOCOL_NEGOTIATION;
      alpn->n_protocols = context->n_application_layer_protocols;
      alpn->protocols = context->application_layer_protocols;
      ADD_EXT (c_hello, (DskTlsExtension *) alpn);
    }

  // NOT SUPPORTED: extension: signed_certificate_timestamp

  // Normally, we just don't specify - but if we have a fixed cert,
  // let's use it.
  if (context->has_client_cert_type)
    {
      DskTlsExtension_CertificateType *cct = HS_ALLOC0 (hs_info, DskTlsExtension_CertificateType);
      cct->base.type = DSK_TLS_EXTENSION_TYPE_CLIENT_CERTIFICATE_TYPE;
      cct->n_cert_types = 1;
      DskTlsCertificateType *ct_arr = HS_ALLOC_ARRAY (hs_info, 1, DskTlsCertificateType);
      cct->cert_types = ct_arr;
      ct_arr[0] = context->client_cert_type;
      ADD_EXT (c_hello, (DskTlsExtension *) cct);
    }

  // extension: server_certificate_type
  // Specify all supported cert types.
  {
    DskTlsExtension_CertificateType *sct = HS_ALLOC0 (hs_info, DskTlsExtension_CertificateType);
    sct->base.type = DSK_TLS_EXTENSION_TYPE_SERVER_CERTIFICATE_TYPE;
    sct->n_cert_types = DSK_N_ELEMENTS (supported_cert_types);
    sct->cert_types = (DskTlsCertificateType *) supported_cert_types;
    ADD_EXT (c_hello, (DskTlsExtension *) sct);
  }

  if (hs_info->n_psks > 0)
    {
      DskTlsExtension_PreSharedKey *psk = HS_ALLOC0 (hs_info, DskTlsExtension_PreSharedKey);
      psk->base.type = DSK_TLS_EXTENSION_TYPE_PRE_SHARED_KEY;
      psk->offered_psks.n_identities = hs_info->n_psks;
      psk->offered_psks.identities = HS_ALLOC_ARRAY (hs_info, hs_info->n_psks, DskTlsPresharedKeyIdentity);
      for (unsigned i = 0; i < hs_info->n_psks; i++)
        {
          const DskTlsClientSessionInfo *in = hs_info->psks + i;
          DskTlsPresharedKeyIdentity *out = psk->offered_psks.identities + i;
          out->identity_length = in->identity_length;
          out->identity = (uint8_t *) in->identity_data;
          out->obfuscated_ticket_age = (uint32_t) now_in_millis()
                                     - (uint32_t) in->session_create_time
                                     + (uint32_t) in->ticket_age_add;
                                        
                           
          out->binder_length = in->binder_length;
          out->binder_data = (uint8_t *) in->binder_data;
        }
      ADD_EXT (c_hello, (DskTlsExtension *) psk);

      // extension: pkem:  only support non-DH modes,
      // because roundtrip avoidance is the primary use case
      // for PSKs.
      DskTlsExtension_PSKKeyExchangeModes *pkem = HS_ALLOC0 (hs_info, DskTlsExtension_PSKKeyExchangeModes);
      pkem->base.type = DSK_TLS_EXTENSION_TYPE_PSK_KEY_EXCHANGE_MODES;
      pkem->n_modes = 1;
      pkem->modes = HS_ALLOC_ARRAY (hs_info, 1, DskTlsPSKKeyExchangeMode);
      pkem->modes[0] = DSK_TLS_PSK_EXCHANGE_MODE_KE;
      ADD_EXT (c_hello, (DskTlsExtension *) pkem);
    }

  // Technically this also requires that we are using the KE exchange-mode,
  // but that's the only mode we support.
  if (context->support_early_data
   && hs_info->n_psks > 0)
    {
      DskTlsExtension_EarlyDataIndication *edi = HS_ALLOC0 (hs_info, DskTlsExtension_EarlyDataIndication);
      edi->base.type = DSK_TLS_EXTENSION_TYPE_EARLY_DATA;
      ADD_EXT (c_hello, (DskTlsExtension *) edi);
    }

  // Initial Client-Hello should not have cookies.

  // SupportedVersions extension
  DskTlsExtension_SupportedVersions *sv = HS_ALLOC0 (hs_info, DskTlsExtension_SupportedVersions);
  static DskTlsProtocolVersion tls13 = DSK_TLS_PROTOCOL_VERSION_1_3;
  sv->base.type = DSK_TLS_EXTENSION_TYPE_SUPPORTED_VERSIONS;
  sv->n_supported_versions = 1;
  sv->supported_versions = &tls13;
  ADD_EXT (c_hello, (DskTlsExtension *) sv);

  // extension: certificate_authorities
  if (context->n_certificate_authorities > 0)
    {
      DskTlsExtension_CertificateAuthorities *ca = HS_ALLOC0 (hs_info, DskTlsExtension_CertificateAuthorities);
      ca->base.type = DSK_TLS_EXTENSION_TYPE_CERTIFICATE_AUTHORITIES;
      ca->n_CAs = context->n_certificate_authorities;
      ca->CAs = context->certificate_authorities;
      ADD_EXT (c_hello, (DskTlsExtension *) ca);
    }

  // TODO: extension: post_handshake_auth
  // TODO: extension: signature_algorithm_cert

  // TODO: generating padding is unsupported

  //
  // Try to write ClientHello to record-layer.
  //
  if (!dsk_tls_base_connection_send_handshake_msg
         (&conn->base_instance,
          c_hello,
          error))
    {
      return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
    }

  return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_OK;
}

static DskTlsClientSessionInfo *
copy_sessions (size_t n,
               const DskTlsClientSessionInfo *sessions,
               DskMemPool *mem_pool)
{
  size_t siz = n * sizeof (DskTlsClientSessionInfo);
  DskTlsClientSessionInfo *rv = dsk_mem_pool_alloc (mem_pool, siz);
  memcpy (rv, sessions, siz);
  for (size_t i = 0; i < n; i++)
    {
#define COPY_DATA(len_member, member)                         \
  rv[i].member = dsk_mem_pool_memdup (mem_pool,               \
                                      rv[i].len_member,       \
                                      rv[i].member)
      COPY_DATA(binder_length, binder_data);
      COPY_DATA(identity_length, identity_data);
      COPY_DATA(state_length, state_data);
#undef COPY_DATA
    }
  return rv;
}

void dsk_tls_client_connection_found_sessions (DskTlsClientConnection *conn,
                                              size_t n_sessions_found,
                                              const DskTlsClientSessionInfo *sessions,
                                              const uint8_t *state_length)
{
  DskTlsClientHandshake *hs_info = conn->base_instance.handshake;
  assert(conn->state == DSK_TLS_CLIENT_CONNECTION_FINDING_PSKS);

  //
  // Append PSK extensions.
  //

  // Store Pre-Shared-Keys.
  hs_info->n_psks = n_sessions_found;
  hs_info->psks = copy_sessions (n_sessions_found, sessions, &hs_info->base.mem_pool);

  // Setup allowed exchange-modes (PKEMs)
  hs_info->psk_allow_wo_dh = true;
  hs_info->psk_allow_w_dh = false;

  conn->state = DSK_TLS_CLIENT_CONNECTION_DONE_FINDING_PSKS;
  if (!hs_info->invoking_user_handler)
    dsk_tls_base_connection_unsuspend (&conn->base_instance);
}

void dsk_tls_client_connection_session_not_found  (DskTlsClientConnection *conn)
{
  DskTlsClientHandshake *hs_info = conn->base_instance.handshake;
  assert(conn->state == DSK_TLS_CLIENT_CONNECTION_FINDING_PSKS);
  conn->state = DSK_TLS_CLIENT_CONNECTION_DONE_FINDING_PSKS;
  if (!hs_info->invoking_user_handler)
    dsk_tls_base_connection_unsuspend (&conn->base_instance);
}
