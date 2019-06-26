#include "../dsk.h"
#include "dsk-tls-private.h"
#include <stdlib.h>
#include <string.h>

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

//
// Keyshare Negotiation Result.
//
typedef enum {
  KS_RESULT_NONE,  // must be 0 == keyshare negotiation not done yet
  KS_RESULT_NO_SHARE,
  KS_RESULT_SUCCESS,                  // remote_key_share is set
  KS_RESULT_MUST_MAKE_RETRY_REQUEST,  // best_supported_group is set
} KSResult;

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

static int
compare_ptr_cipher_suites_by_code (const void *a, const void *b)
{
  const DskTlsCipherSuite *A = * (const DskTlsCipherSuite * const *) a;
  const DskTlsCipherSuite *B = * (const DskTlsCipherSuite * const *) b;
  return A->code < B->code ? -1
       : A->code > B->code ? 1
       : 0;
}

static const char *
dsk_tls_connection_state_name (DskTlsConnectionState state)
{
  switch (state)
    {
#define WRITE_CASE(shortname) case DSK_TLS_CONNECTION_##shortname: return #shortname
      WRITE_CASE(CLIENT_START);
      WRITE_CASE(CLIENT_DOING_PSK_LOOKUP);
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
create_handshake (DskTlsHandshakeNegotiation *hs_info,
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

void
dsk_tls_connection_failed (DskTlsConnection *conn,
                           DskError         *error)
{
  if (conn->failed_error != NULL)
    {
      dsk_warning("failed after: %s\nprevious error:\n%s",
                  conn->failed_error->message,
                  error->message);
      return;
    }
  conn->failed_error = dsk_error_ref (error);
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
client_continue_post_psks (DskTlsHandshakeNegotiation *hs_info,
                           DskError **error);

//
// Compute the initial ClientHello message.
//
static bool
send_client_hello_flight (DskTlsHandshakeNegotiation *hs_info,
                          DskError **error)
{
  DskTlsHandshakeMessage *ch = create_handshake (hs_info, DSK_TLS_HANDSHAKE_MESSAGE_TYPE_CLIENT_HELLO, 16);
  DskTlsConnection *conn = hs_info->conn;
  DskTlsContext *ctx = conn->context;
  hs_info->client_hello = ch;

  // Offered client PSKs
  if (ctx->client_find_psks != NULL)
    {
      conn->state = DSK_TLS_CONNECTION_CLIENT_DOING_PSK_LOOKUP;
      dsk_object_ref (conn);
      ctx->client_find_psks (hs_info, ctx->client_find_psks_data);
      return true;
    }
  else
    return client_continue_post_psks (hs_info, error);
}

void
dsk_tls_handshake_client_error_finding_psks (DskTlsHandshakeNegotiation *hs_info,
                                             DskError *error)
{
  DskTlsConnection *conn = hs_info->conn;
  assert(conn->state == DSK_TLS_CONNECTION_CLIENT_DOING_PSK_LOOKUP);

  if (conn->failed_error == NULL)
    dsk_tls_connection_failed (conn, error);
  dsk_object_unref (conn);
}

static void
tls_preshared_key_info_copy_to (DskMemPool *pool,
                                DskTlsPresharedKeyInfo *dst,
                                const DskTlsPresharedKeyInfo *src)
{
  *dst = *src;
  dst->identity = dsk_mem_pool_memdup (pool, src->identity_length, src->identity);
  dst->master_key = dsk_mem_pool_memdup (pool, src->master_key_length, src->master_key);
}

void
dsk_tls_handshake_client_found_psks (DskTlsHandshakeNegotiation *hs_info,
                                     size_t                      n_psks,
                                     DskTlsPresharedKeyInfo     *psks)
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
client_continue_post_psks (DskTlsHandshakeNegotiation *hs_info,
                           DskError **error)
{
  DskTlsConnection *conn = hs_info->conn;
  conn->state = DSK_TLS_CONNECTION_CLIENT_START;

  DskTlsHandshakeMessage *ch = hs_info->client_hello;
  ch->client_hello.legacy_version = 0x301;
  dsk_get_cryptorandom_data (32, ch->client_hello.random);
  ch->client_hello.legacy_session_id_length = 32;
  dsk_get_cryptorandom_data (32, ch->client_hello.legacy_session_id);
  static const DskTlsCipherSuiteCode client_hello_supported_cipher_suites[] = {
    DSK_TLS_CIPHER_SUITE_TLS_AES_128_GCM_SHA256,
    DSK_TLS_CIPHER_SUITE_TLS_AES_256_GCM_SHA384,
    DSK_TLS_CIPHER_SUITE_TLS_CHACHA20_POLY1305_SHA256,
    DSK_TLS_CIPHER_SUITE_TLS_AES_128_CCM_SHA256,
    DSK_TLS_CIPHER_SUITE_TLS_AES_128_CCM_8_SHA256,
  };
  ch->client_hello.n_cipher_suites = DSK_N_ELEMENTS (client_hello_supported_cipher_suites);
  ch->client_hello.cipher_suites = (DskTlsCipherSuiteCode *) client_hello_supported_cipher_suites;
  ch->client_hello.n_compression_methods = 0;
  ch->client_hello.compression_methods = NULL;

  // supported groups
  static const DskTlsNamedGroup client_hello_supported_groups[] = {
    DSK_TLS_NAMED_GROUP_X25519,
    DSK_TLS_NAMED_GROUP_FFDHE2048,
    DSK_TLS_NAMED_GROUP_FFDHE3072,
    DSK_TLS_NAMED_GROUP_FFDHE4096,
    DSK_TLS_NAMED_GROUP_FFDHE6144,
    DSK_TLS_NAMED_GROUP_FFDHE8192,
  };
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
  DskTlsExtension_KeyShare *ks = HS_ALLOC (hs_info, DskTlsExtension_KeyShare);
  ks->base.type = DSK_TLS_EXTENSION_TYPE_KEY_SHARE;
  unsigned n_key_shares = DSK_N_ELEMENTS (client_key_share_groups);
  ks->n_key_shares = n_key_shares;
  ks->key_shares = HS_ALLOC_ARRAY(hs_info, n_key_shares, DskTlsKeyShareEntry);
  hs_info->n_key_shares = n_key_shares;
  hs_info->key_shares = HS_ALLOC_ARRAY (hs_info, n_key_shares, DskTlsPublicPrivateKeyShare);
  for (unsigned i = 0; i < n_key_shares; i++)
    {
    DskTlsNamedGroup group = client_key_share_groups[i];
      const DskTlsKeyShareMethod *share_method = dsk_tls_key_share_method_by_group (group);
      DskTlsKeyShareEntry *entry = ks->key_shares + i;
      entry->named_group = client_key_share_groups[i];
      unsigned priv_key_size = share_method->private_key_bytes;
      unsigned pub_key_size = share_method->public_key_bytes;
      uint8_t *priv_key = dsk_mem_pool_alloc (&hs_info->mem_pool, priv_key_size + pub_key_size);
      uint8_t *pub_key = priv_key + priv_key_size;
      entry->key_exchange_length = pub_key_size;
      entry->key_exchange_data = pub_key;
      do
        {
          dsk_get_cryptorandom_data (priv_key_size, priv_key);
        }
      while (!share_method->make_key_pair (share_method, priv_key, pub_key));
      hs_info->key_shares[i].method = share_method;
      hs_info->key_shares[i].public_key = pub_key;
      hs_info->key_shares[i].private_key = priv_key;
    }
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

  conn->state = DSK_TLS_CONNECTION_CLIENT_WAIT_SERVER_HELLO;

  return true;
}

// =======================================================================
//
// ===    Handshake Part 2:   Compute ServerHello/HelloRetryRequest    ===
//
//
// This starts with the ClientHello message received.
//
// Most of the "Negotiations" are fairly fixed by the configuration.
// 
// This is where the vast majority of negotiations happen.
//
// ===    Handshake Part 2:   Compute ServerHello/HelloRetryRequest    ===
//
// =======================================================================
                                                                     
 
//
// Compute the optional requested server hostname
// from the ServerName extension.
//
// This cannot fail, since the extension is optional.
//
static bool
find_server_name (DskTlsHandshakeNegotiation *hs_info,
                  DskError **error)
{
  DskTlsExtension_ServerNameList *server_name_ext = FIND_EXT_SERVER_NAME (hs_info->received_handshake);
  if (server_name_ext != NULL)
    {
      for (unsigned i = 0; i < server_name_ext->n_entries; i++)
        if (server_name_ext->entries[i].type == DSK_TLS_SERVER_NAME_TYPE_HOSTNAME)
          {
            hs_info->server_hostname = server_name_ext->entries[i].name;
            return true;
          }
    }
  (void) error;
  return true;
}

// ---------------------------------------------------------------------
// Negotiate TLS Version.
//
// We only support TLS 1.3 so that's all we negotiate at this point.
//
static bool
do_tls_version_negotiation (DskTlsHandshakeNegotiation *hs_info,
                            DskError **error)
{
  DskTlsExtension_SupportedVersions *supp_ver = (DskTlsExtension_SupportedVersions *)
    find_extension_by_type (hs_info->received_handshake,
                            DSK_TLS_EXTENSION_TYPE_SUPPORTED_VERSIONS);
  if (supp_ver == NULL)
    {
      // This is a TLS 1.2 client, but we don't support that (yet?).
      *error = dsk_error_new ("no SupportedVersion implies TLS 1.2, which is unsupported");
      return false;
    }

  if (supp_ver != NULL)
    {
      unsigned i;
      for (i = 0; i < supp_ver->n_supported_versions; i++)
        if (supp_ver->supported_versions[i] == DSK_TLS_PROTOCOL_VERSION_1_3)
          break;
      if (i == supp_ver->n_supported_versions)
        {
          *error = dsk_error_new ("version negotiation failed");
          return false;
        }
      else
        {
          hs_info->conn->version = supp_ver->supported_versions[i];
        }
    }
  return true;
}

// ---------------------------------------------------------------------
//
// Handle KeyShare extension.  Section 4.2.8.
//
// There are several possible results:
//    * key-share sent by client is accepted, respond with ServerHello
//      (unless there are other negotiation problems).  returns true.
//    * a suitable NamedGroup was found, but that's not the key-share sent.
//      A HelloRetryRequest will be sent.  return true.
//    * format or other protocol error: *error is set, return false.
//
static bool
do_keyshare_negotiation (DskTlsHandshakeNegotiation *hs_info,
                         DskError **error)
{
  DskTlsHandshakeMessage *shake = hs_info->received_handshake;
  DskTlsExtension_KeyShare *key_share = FIND_EXT_KEY_SHARE(shake);
  DskTlsExtension_SupportedGroups *supp_groups = FIND_EXT_SUPPORTED_GROUPS(shake);
  if (key_share == NULL && supp_groups == NULL)
    {
      // relying on PSK with exchange-mode KE-only
      hs_info->ks_result = KS_RESULT_NO_SHARE;
      return true;
    }
  // Section 9.2:
  //   "If containing a "supported_groups" extension, it MUST also contain
  //    a "key_share" extension, and vice versa.  An empty
  //    KeyShare.client_shares vector is permitted."
  if (key_share == NULL)
    {
      *error = dsk_error_new ("supported-groups implies key-share");
      return false;
    }
  if (supp_groups == NULL)
    {
      *error = dsk_error_new ("key-share implies supported-groups");
      return false;
    }

  unsigned n_key_shares = key_share->n_key_shares;
  DskTlsKeyShareEntry *key_shares = key_share->key_shares;
  qsort (key_shares,
         n_key_shares,
         sizeof(DskTlsKeyShareEntry),
         compare_key_share_by_named_group);

  unsigned n_supported_groups = supp_groups->n_supported_groups;
  DskTlsNamedGroup *supported_groups = supp_groups->supported_groups;
  qsort (supported_groups,
         n_supported_groups,
         sizeof(DskTlsNamedGroup),
         compare_named_groups);

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

  DskTlsExtension_KeyShare *keyshare_response = NULL;
  if (n_key_shares == 0 && n_supported_groups == 0)
    {
      // Only Pre-Shared Keys with exchange-mode==PSK_EXCHANGE_MODE_KE.
      hs_info->ks_result = KS_RESULT_NO_SHARE;
    }
  else
    {
      // client is requesting server to choose.
      int best_sg_quality = 0;
      int best_sg_index = -1;
      for (unsigned g = 0; g < n_supported_groups; g++)
        {
          int quality = get_supported_group_quality (supported_groups[g]);
          if (quality > 0 && quality > best_sg_quality)
            {
              best_sg_quality = quality;
              best_sg_index = supported_groups[g];
            }
        }
      if (best_sg_quality < 0)
        {
          *error = dsk_error_new ("key-share negotiation failed (client requested server choose)");
          return false;
        }

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

      if (best_ks_index >= 0)
        {
          keyshare_response = HS_ALLOC (hs_info, DskTlsExtension_KeyShare);
          assert(hs_info->n_key_shares == 1);
          keyshare_response->base.is_generic = false;
          keyshare_response->base.type = DSK_TLS_EXTENSION_TYPE_KEY_SHARE;
          keyshare_response->base.extension_data_length = 0;
          keyshare_response->base.extension_data = NULL;
          hs_info->ks_result = KS_RESULT_SUCCESS;
        }
      else if (best_sg_index >= 0)
        {
          keyshare_response = HS_ALLOC (hs_info, DskTlsExtension_KeyShare);
          keyshare_response->base.is_generic = false;
          keyshare_response->base.type = DSK_TLS_EXTENSION_TYPE_KEY_SHARE;
          keyshare_response->base.extension_data_length = 0;
          keyshare_response->base.extension_data = NULL;
          hs_info->ks_result = KS_RESULT_MUST_MAKE_RETRY_REQUEST;
          hs_info->best_supported_group = best_sg_index;
        }
      else
        {
          *error = dsk_error_new ("key-share negotiation failed");
          return false;
        }
    }

  if (keyshare_response != NULL)
    hs_info->keyshare_response = keyshare_response;
  return true;
}

//
static bool
do_psk_negotiation (DskTlsHandshakeNegotiation *hs_info,
                    DskError **error)
{
  DskTlsHandshakeMessage *shake = hs_info->received_handshake;
  DskTlsExtension_PreSharedKey *psk = FIND_EXT_PSK (shake);
  DskTlsExtension_PSKKeyExchangeModes *pskm = FIND_EXT_PSK_KEY_EXCHANGE_MODE(shake);

  assert (*error == NULL);

  if (psk == NULL)
    {
      return true;
    }
  if (pskm == NULL)
    {
      // 4.2.9: "If clients offer "pre_shared_key"
      //         without a "psk_key_exchange_modes" extension,
      //         servers MUST abort the handshake."
      *error = dsk_error_new ("client offered Pre-Shared Key without Pre-Shared Key Exchange Modes");
      return false;
    }


  //
  // Handle PreSharedKey Mode Negotiation
  //
  bool got_psk_only_key_exchange_mode = false;
  bool got_psk_with_dhe_key_exchange_mode = false;
  for (unsigned i = 0; i < pskm->n_modes; i++)
    switch (pskm->modes[i])
      {
      case DSK_TLS_PSK_EXCHANGE_MODE_KE:
        got_psk_only_key_exchange_mode = true;
        break;
      case DSK_TLS_PSK_EXCHANGE_MODE_DHE_KE:
        got_psk_with_dhe_key_exchange_mode = true;
        break;
      }
  if (!got_psk_with_dhe_key_exchange_mode
   && !got_psk_only_key_exchange_mode)
    {
      // PSK negotiation failed, since there's no valid exchange mode.
      return true;
    }

  DskTlsPSKKeyExchangeMode kem;
  if (got_psk_only_key_exchange_mode
   && got_psk_with_dhe_key_exchange_mode)
    {
      // Server must choose which exchange-mode to use.
      // In practice, we'll have to use PSK_ONLY if 0-RTT is
      // going to work, so we just do that.
      kem = DSK_TLS_PSK_EXCHANGE_MODE_KE;
    }
  else if (got_psk_with_dhe_key_exchange_mode)
    {
      kem = DSK_TLS_PSK_EXCHANGE_MODE_DHE_KE;
    }
  else if (got_psk_only_key_exchange_mode)
    {
      kem = DSK_TLS_PSK_EXCHANGE_MODE_KE;
    }
  else
    {
      // PSK Exchange-Mode negotiation must succeed to do
      // anything with PSKs.
      return true;
    }

  //
  // Compute the set of cipher-suites that are available,
  // by computing the intersection of 
  //
  unsigned client_n_cs = hs_info->received_handshake->client_hello.n_cipher_suites;
  const DskTlsCipherSuiteCode *client_cs = hs_info->received_handshake->client_hello.cipher_suites;
  DskTlsCipherSuite **cipher_suites_sorted = alloca (sizeof (DskTlsCipherSuite *) * client_n_cs);
  unsigned n_cipher_suites = 0;
  for (unsigned i = 0; i < client_n_cs; i++)
    {
      DskTlsCipherSuite *cs = dsk_tls_cipher_suite_by_code (client_cs[i]);
      if (cs != NULL)
        cipher_suites_sorted[n_cipher_suites++] = cs;
    }
  qsort (cipher_suites_sorted, n_cipher_suites, sizeof(DskTlsCipherSuite*),
         compare_ptr_cipher_suites_by_code);

  //
  // Handle PreSharedKeys
  //
  // Can we actually use any of the identies with the binders?
  //
  // Look at has_psk to determine if a usable PSK was found.
  //
  const DskTlsOfferedPresharedKeys *offered = &psk->offered_psks;
  hs_info->offered_psks = offered;
  DskTlsConnection *conn = hs_info->conn;
  conn->state = DSK_TLS_CONNECTION_SERVER_SELECTING_PSK;
  conn->context->server_select_psk (hs_info,
                                    offered,
                                    conn->context->server_select_psk_data);
  return true;
}


static void server_continue_post_psk_negotiation (DskTlsHandshakeNegotiation *hs_info);
void
dsk_tls_handshake_server_chose_psk (DskTlsHandshakeNegotiation *hs_info,
                                    DskTlsPSKKeyExchangeMode exchange_mode,
                                    unsigned                    which_psk)
{
  // Simply return true without setting anything up
  // if no PSK was valid.
  assert (which_psk < hs_info->offered_psks->n_identities);

  hs_info->has_psk = true;
  hs_info->psk_key_exchange_mode = exchange_mode;
  hs_info->psk_index = which_psk;

  server_continue_post_psk_negotiation (hs_info);
}
void
dsk_tls_handshake_server_chose_no_psk (DskTlsHandshakeNegotiation *hs_info)
{
  server_continue_post_psk_negotiation (hs_info);
}

void
dsk_tls_handshake_server_error_choosing_psk (DskTlsHandshakeNegotiation *hs_info,
                                             DskError                    *error)
{
  dsk_tls_connection_failed (hs_info->conn, error);
}

// ---------------------------------------------------------------------
//
// Combine the results of Pre-Shared-Key and Key-Share
// negotiations.
//
static bool
combine_psk_and_keyshare (DskTlsHandshakeNegotiation *hs_info,
                          DskError **error)
{
  if (hs_info->has_psk)
    {
      switch (hs_info->psk_key_exchange_mode)
        {
        case DSK_TLS_PSK_EXCHANGE_MODE_KE:
          hs_info->keyshare_response = NULL;
          hs_info->ks_result = KS_RESULT_NO_SHARE;
          break;
        case DSK_TLS_PSK_EXCHANGE_MODE_DHE_KE:
          if (hs_info->keyshare_response == NULL
           || hs_info->ks_result == KS_RESULT_NO_SHARE)
            {
              *error = dsk_error_new ("KeyShare extension required for PSK with DHE");
              return false;
            }
          break;
        }
    }
  else
    {
      if (hs_info->ks_result == KS_RESULT_NO_SHARE)
        {
          *error = dsk_error_new ("KeyShare extension required");
          return false;
        }
    }
  return true;
}

// ---------------------------------------------------------------------
//                 Cipher Suite negotiation
// ---------------------------------------------------------------------
static bool
do_cipher_suite_negotiation (DskTlsHandshakeNegotiation *hs_info,
                             DskError **error)
{
  //
  // CipherSuite may be chosen as part of PSK negotiations.
  //
  if (hs_info->cipher_suite != NULL)
    return true;
    
  DskTlsHandshakeMessage *shake = hs_info->client_hello;
  unsigned n_cipher_suites = shake->client_hello.n_cipher_suites;
  DskTlsCipherSuiteCode *cipher_suites = shake->client_hello.cipher_suites;
  for (unsigned i = 0; i < n_cipher_suites; i++)
    {
      DskTlsCipherSuite *cs = dsk_tls_cipher_suite_by_code (cipher_suites[i]);
      if (cs != NULL)
        {
          hs_info->cipher_suite = cs;
          return true;
        }
    }
  *error = dsk_error_new ("CipherSuite negotiation failed");
  return false;
}

// ---------------------------------------------------------------------
//                   Diffie-Hellman-style Key Share
// ---------------------------------------------------------------------
static void
generate_key_pair (DskTlsHandshakeNegotiation *hs_info,
                   const DskTlsKeyShareMethod *method)
{
  DskMemPool *pool = &hs_info->mem_pool;
  uint8_t *priv_key = dsk_mem_pool_alloc (pool, method->private_key_bytes);
  uint8_t *pub_key = dsk_mem_pool_alloc (pool, method->public_key_bytes);
  do
    dsk_get_cryptorandom_data (method->private_key_bytes, priv_key);
  while (!method->make_key_pair(method, priv_key, pub_key));

  hs_info->n_key_shares = 1;
  hs_info->key_shares = HS_ALLOC_ARRAY (hs_info, 1, DskTlsPublicPrivateKeyShare);
  hs_info->key_shares[0].public_key = pub_key;
  hs_info->key_shares[0].private_key = priv_key;
  hs_info->key_shares[0].method = method;
  hs_info->selected_key_share = 0;
}

static bool
maybe_calculate_key_share (DskTlsHandshakeNegotiation *hs_info,
                             DskError **error)
{
  switch (hs_info->ks_result)
    {
    case KS_RESULT_NONE:
      assert(false);
      return false;

    case KS_RESULT_NO_SHARE:
      assert (hs_info->has_psk);
      assert (hs_info->psk_key_exchange_mode == DSK_TLS_PSK_EXCHANGE_MODE_KE);
      return true;

    case KS_RESULT_MUST_MAKE_RETRY_REQUEST:
      {
        DskTlsExtension_KeyShare *ks = HS_ALLOC0 (hs_info, DskTlsExtension_KeyShare);
        ks->base.type = DSK_TLS_EXTENSION_TYPE_KEY_SHARE;

        /// XXX: should make sure this is part of SupportedGroups
        ks->selected_group = DSK_TLS_NAMED_GROUP_X25519;
        assert(hs_info->n_extensions < hs_info->max_extensions);
        hs_info->extensions[hs_info->n_extensions++]
          = (DskTlsExtension *) ks;
      }
      return true;

    case KS_RESULT_SUCCESS:
      {
        DskTlsExtension_KeyShare *keyshare_response = hs_info->keyshare_response;
        assert (keyshare_response != NULL);
        DskTlsKeyShareEntry *remote_share = hs_info->remote_key_share;
        DskTlsKeyShareEntry *server_share = &keyshare_response->server_share;
        DskTlsNamedGroup group = server_share->named_group;
        const DskTlsKeyShareMethod *method
                  = dsk_tls_key_share_method_by_group (group);
        assert(method != NULL);
        if (remote_share->key_exchange_length != method->public_key_bytes)
          {
            *error = dsk_error_new ("KeyShare method does not match public key length");
            return false;
          }

        // generate key pair
        generate_key_pair (hs_info, method);

        // compute shared secret.
        hs_info->shared_key_length = method->shared_key_bytes;
        hs_info->shared_key = dsk_mem_pool_alloc (&hs_info->mem_pool,
                                                  method->shared_key_bytes);
        method->make_shared_key (method,
                                 hs_info->key_shares[0].private_key,
                                 remote_share->key_exchange_data,
                                 hs_info->shared_key);

        // Add key-share extensions to array.
        assert(hs_info->n_extensions < hs_info->max_extensions);
        hs_info->extensions[hs_info->n_extensions++]
          = (DskTlsExtension *) keyshare_response;
      }
    }
  return true;
}

// ---------------------------------------------------------------------
//                   Server-Side Certificate negotiation
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


static bool
x509_signature_algorithm_matches_tls_scheme
                               (DskTlsX509SignatureAlgorithm algo,
                                DskTlsSignatureScheme        scheme)
{
  switch (algo)
    {
      case DSK_TLS_X509_SIGNATURE_ALGORITHM_RSA_PKCS1_SHA1:
        return false;
      case DSK_TLS_X509_SIGNATURE_ALGORITHM_RSA_PKCS1_SHA256:
        return scheme == DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA256;
      case DSK_TLS_X509_SIGNATURE_ALGORITHM_RSA_PKCS1_SHA384:
        return scheme == DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA384;
      case DSK_TLS_X509_SIGNATURE_ALGORITHM_RSA_PKCS1_SHA512:
        return scheme == DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA512;
      case DSK_TLS_X509_SIGNATURE_ALGORITHM_RSA_PSS_SHA256:
        return scheme == DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA256
            || scheme == DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_PSS_SHA256;
      case DSK_TLS_X509_SIGNATURE_ALGORITHM_RSA_PSS_SHA384:
        return scheme == DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA384
            || scheme == DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_PSS_SHA384;
      case DSK_TLS_X509_SIGNATURE_ALGORITHM_RSA_PSS_SHA512:
        return scheme == DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA512
            || scheme == DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_PSS_SHA512;
      case DSK_TLS_X509_SIGNATURE_ALGORITHM_RSA_PSS:
        return scheme == DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA256
            || scheme == DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_PSS_SHA256
            || scheme == DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA384
            || scheme == DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_PSS_SHA384
            || scheme == DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA512
            || scheme == DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_PSS_SHA512;
      default:
        return false;
    }
}

static bool
scheme_valid_for_certificate (DskTlsSignatureScheme scheme,
                              DskTlsCertificate    *cert)
{
  switch (cert->type)
    {
      case DSK_TLS_CERTIFICATE_TYPE_X509:
        {
          DskTlsX509SignatureAlgorithm algo = cert->x509.chain[0]->signature_algorithm;
          return x509_signature_algorithm_matches_tls_scheme (algo, scheme);
        }
      case DSK_TLS_CERTIFICATE_TYPE_RAW_PUBLIC_KEY:
        {
          //TODO
          return false;
        }
      default:
        return false;
    }
}

static bool
do_certificate_selection (DskTlsHandshakeNegotiation *hs_info,
                          DskError **error)
{
  //
  // Perform Certificate Selection
  //
  DskTlsExtension_SignatureAlgorithms *sig_algos = FIND_EXT_SIGNATURE_ALGORITHMS(hs_info->received_handshake);
  if (sig_algos == NULL)
    {
      *error = dsk_error_new ("client didn't specify SignatureAlgorithms");
      return false;
    }

  DskTlsConnection *conn = hs_info->conn;
  DskTlsContext *context = conn->context;
  size_t n_certs = context->n_certificates;
  DskTlsCertificate **certs = context->certificates;
  for (size_t i = 0; i < n_certs; i++)
    {
      // TODO: handle RAW_PUBLIC_KEY
      if (certs[i]->type != DSK_TLS_CERTIFICATE_TYPE_X509)
        continue;

      //
      // Filter out certificates whose
      // name doesn't match the server_name extension value.
      //
      if (hs_info->server_hostname != NULL)
        {
          DskTlsX509Name *subject_name = &certs[i]->x509.chain[0]->subject;
          const char *domain_name = dsk_tls_x509_name_get_component (subject_name,
                                      DSK_TLS_X509_DN_DOMAIN_COMPONENT);
          if (domain_name == NULL
           || strcmp (hs_info->server_hostname, domain_name) == 0)
            continue;
        }

      
      //
      // Look for a supported scheme that matches this cert.
      //
      unsigned n_sig_schemes = sig_algos->n_schemes;
      const DskTlsSignatureScheme *sig_schemes = sig_algos->schemes;
      for (size_t j = 0; j < n_sig_schemes; j++)
        if (scheme_valid_for_certificate (sig_schemes[j], certs[i]))
          {
            DskTlsCertificate *cert = certs[i];
            hs_info->certificate_scheme = sig_schemes[j];

            DskTlsHandshakeMessage *cert_hs = create_handshake (hs_info, DSK_TLS_HANDSHAKE_MESSAGE_TYPE_CERTIFICATE, 0);
            cert_hs->certificate.n_entries = 1;
            DskTlsCertificateEntry *entry = HS_ALLOC0 (hs_info, DskTlsCertificateEntry);
            cert_hs->certificate.entries = entry;
            entry[0].cert_data_length = cert->cert_data_length;
            entry[0].cert_data = cert->cert_data;
            entry[0].n_extensions = 0;
            entry[0].extensions = NULL;
            assert(hs_info->n_extensions < hs_info->max_extensions);
            hs_info->certificate = cert_hs;
            return true;
          }
    }

  *error = dsk_error_new ("no certificates compatible with signature schemes");
  return false;
}

//
// Perform Early Data Negotiation
//
static bool
do_early_data_negotiation (DskTlsHandshakeNegotiation *hs_info,
                           DskError **error)
{
  DskTlsHandshakeMessage *shake = hs_info->received_handshake;
  DskTlsExtension_EarlyDataIndication *edi = FIND_EXT_EARLY_DATA_INDICATION (shake);
  if (edi != NULL
   && hs_info->has_psk
   && hs_info->psk_key_exchange_mode == DSK_TLS_PSK_EXCHANGE_MODE_KE
   && hs_info->psk_index == 0)
    {
      hs_info->allow_early_data = true;
    }
  if (edi != NULL)
    {
      if (FIND_EXT_PSK (shake) == NULL)
        {
          *error = dsk_error_new ("PreSharedKey required for EarlyData");
          return false;
        }
    }
  return true;
}

//
// Application-Level Protocol Negotiation
//
static bool
do_application_level_protocol_negotiation (DskTlsHandshakeNegotiation *hs_info,
                           DskError **error)
{
  DskTlsConnection *conn = hs_info->conn;
  DskTlsHandshakeMessage *shake = hs_info->received_handshake;
  DskMemPool *pool = &hs_info->mem_pool;
  DskTlsContext *context = conn->context;
  if (context->n_alps > 0)
    {
      DskTlsExtension_ALPN *alpn = FIND_EXT_ALPN(shake);
      unsigned n_protocols = alpn->n_protocols;
      const char **protocols = alpn->protocols;
      unsigned n_supp_protocols = context->n_alps;
      const char **supp_protocols = context->alps;
      const char *matched_protocol = NULL;
      for (unsigned i = 0; i < n_protocols && matched_protocol == NULL; i++)
        for (unsigned j = 0; j < n_supp_protocols; j++)
          if (strcmp (protocols[i], supp_protocols[j]) == 0)
            {
              matched_protocol = supp_protocols[j];
              break;
            }
      if (matched_protocol != NULL)
        {
          DskTlsExtension_ALPN *resp_alpn
            = dsk_mem_pool_alloc0 (pool, sizeof (DskTlsExtension_ALPN));
          resp_alpn->base.type = DSK_TLS_EXTENSION_TYPE_APPLICATION_LAYER_PROTOCOL_NEGOTIATION;
          resp_alpn->n_protocols = 1;
          resp_alpn->protocols = dsk_mem_pool_alloc (pool, sizeof(const char*));
          resp_alpn->protocols[0] = matched_protocol;
        }
      else if (context->alpn_required)
        {
          *error = dsk_error_new ("Application-Level Protocol Negotiation failed, but is required");
          return false;
        }
    }
  else
    {
      assert(!context->alpn_required);
    }
  return true;
}

#define MAX_HRR_EXTENSIONS       16

static bool
send_hello_retry_request (DskTlsHandshakeNegotiation *hs_info,
                          DskError **error)
{
  // In order to avoid loops, do not
  // offer a second HelloRetryRequest.
  if (hs_info->sent_hello_retry_request)
    {
      // 2nd retry not allowed.  TODO: spec cite?
      *error = dsk_error_new ("second Hello-Retry-Request denied");
      return false;
    }

  assert(hs_info->cipher_suite != NULL);

  // must send hello-retry-request
  hs_info->sent_hello_retry_request = true;

  // Compose HelloRetryRequest HandshakeMessage object.
  DskTlsHandshakeMessage *sh = create_handshake(hs_info, DSK_TLS_HANDSHAKE_MESSAGE_TYPE_SERVER_HELLO, 16);
  sh->server_hello.is_retry_request = true;
  sh->server_hello.n_extensions = 0;
  sh->server_hello.extensions = HS_ALLOC_ARRAY (hs_info, MAX_HRR_EXTENSIONS, DskTlsExtension *);

  if (hs_info->keyshare_response != NULL)
    {
      sh->server_hello.extensions[sh->server_hello.n_extensions++] = (DskTlsExtension *) hs_info->keyshare_response;
    }
  // TODO:  supported-groups, cookie?
  if (!dsk_tls_handshake_serialize (hs_info, sh, error))
    {
      return false;
    }

  //
  // Deal with Transcript-Hash.
  //
  // Section 4.4.1: when the server responds to a
  // ClientHello with a HelloRetryRequest, the value of ClientHello1 is
  // replaced with a special synthetic handshake message of handshake type
  // "message_hash" containing Hash(ClientHello1).  I.e.,
  //
  //  Transcript-Hash(ClientHello1, HelloRetryRequest, ... Mn) =
  //      Hash(message_hash       ||  /* Handshake type */
  //           00 00 Hash.length  ||  /* Handshake message length (bytes) */
  //           Hash(ClientHello1) ||  /* Hash of ClientHello1 */
  //           HelloRetryRequest  || ... || Mn)
  //
  // The reason for this construction is to allow the server to do a
  // stateless HelloRetryRequest by storing just the hash of ClientHello1
  // in the cookie, rather than requiring it to export the entire
  // intermediate hash state (see Section 4.2.2).
  //
  DskChecksumType *hash_type = hs_info->cipher_suite->hash_type;
  DskTlsHandshakeMessage *ch = hs_info->client_hello;

  // First, compute Hash(ClientHello1) as cli_hello_hash.
  uint8_t *cli_hello_hash = alloca(hash_type->hash_size);
  hash_type->init(hs_info->transcript_hash_instance);
  hash_type->feed(hs_info->transcript_hash_instance,
                  ch->data_length, ch->data);
  hash_type->end(hs_info->transcript_hash_instance, cli_hello_hash);
  hash_type->init(hs_info->transcript_hash_instance);

  // Build header to be equal to the 6 bytes:
  //       message_hash || 00 00 Hash.length
  // Per spec, message_hash==254.
  uint8_t header[6] = {
    0,
    254,
    0,
    0,
    hash_type->hash_size >> 8,
    hash_type->hash_size & 0xff
  };
  hash_type->feed(hs_info->transcript_hash_instance, 6, header);
  hash_type->feed(hs_info->transcript_hash_instance,
                  hash_type->hash_size, cli_hello_hash);

  // Write message to record layer.
  dsk_tls_record_layer_send_handshake (hs_info, sh->data_length, sh->data);

  return true;
}

static bool
send_server_hello_flight (DskTlsHandshakeNegotiation *hs_info,
                          DskError **error)
{
  DskTlsConnection *conn = hs_info->conn;
  DskMemPool *pool = &hs_info->mem_pool;
  DskChecksumType *hash_type = conn->cipher_suite->hash_type;
  DskTlsKeySchedule *key_schedule = hs_info->key_schedule;
  void *th_instance;
  if (hs_info->sent_hello_retry_request)
    {
      th_instance = hs_info->transcript_hash_instance;
      assert (th_instance != NULL);
    }
  else
    {
      th_instance = dsk_mem_pool_alloc (pool, hash_type->instance_size);
      hs_info->transcript_hash_instance = th_instance;
      hash_type->init (th_instance);
      hash_type->feed (th_instance, hs_info->client_hello->data_length, hs_info->client_hello->data);
    }

  //
  // Make a temporary instance that we will use to
  // finish the hash non-destructively.
  //
  void *instance_copy;
  instance_copy = alloca (hash_type->instance_size);

  //
  // Compute the ClientHello Transcript-Hash.
  //
  memcpy(instance_copy, th_instance, hash_type->instance_size);
  uint8_t *client_hello_transcript_hash = alloca (hash_type->hash_size);
  hash_type->end (instance_copy, client_hello_transcript_hash);

  size_t psk_size = 0;
  const uint8_t *psk = NULL;
  if (hs_info->has_psk)
    {
      psk_size = hs_info->psk_identity->identity_length;
      psk = hs_info->psk_identity->identity;
    }
  dsk_tls_key_schedule_compute_early_secrets (key_schedule,
                                              false,    // externally shared
                                              client_hello_transcript_hash,
                                              psk_size, psk);

  DskTlsHandshakeMessage *server_hello;
  server_hello = create_handshake (hs_info, DSK_TLS_HANDSHAKE_MESSAGE_TYPE_SERVER_HELLO, 16);
  hs_info->server_hello = server_hello;
  //... send server-hello
  server_hello->server_hello.legacy_version = hs_info->client_hello->client_hello.legacy_version;
  memcpy (server_hello->server_hello.random,
          hs_info->client_hello->client_hello.random,
          32);
  server_hello->server_hello.legacy_session_id_echo_length
    = hs_info->client_hello->client_hello.legacy_session_id_length;
  server_hello->server_hello.legacy_session_id_echo
    = hs_info->client_hello->client_hello.legacy_session_id;
  server_hello->server_hello.cipher_suite
    = conn->cipher_suite->code;
  server_hello->server_hello.compression_method = 0;
  server_hello->server_hello.is_retry_request = false;
  if (!dsk_tls_handshake_serialize (hs_info, server_hello, error))
    return false;
  dsk_tls_record_layer_send_handshake (hs_info, server_hello->data_length, server_hello->data);

  hash_type->feed (hs_info->transcript_hash_instance,
                   server_hello->data_length,
                   server_hello->data);
  uint8_t *server_hello_transcript_hash = alloca (hash_type->hash_size);
  memcpy(instance_copy,
         hs_info->transcript_hash_instance,
         hash_type->instance_size);
  hash_type->end (instance_copy, server_hello_transcript_hash);

  dsk_tls_key_schedule_compute_handshake_secrets (key_schedule,
                                                  server_hello_transcript_hash,
                                                  hs_info->shared_key_length,
                                                  hs_info->shared_key);


  // set decryptor, encryptor
  conn->cipher_suite->init (conn->cipher_suite_read_instance,
                            key_schedule->client_handshake_write_key);
  conn->cipher_suite->init (conn->cipher_suite_write_instance,
                            key_schedule->server_handshake_write_key);

  // send_handshake encrypted_extensions
  DskTlsHandshakeMessage *ee = create_handshake (hs_info, DSK_TLS_HANDSHAKE_MESSAGE_TYPE_ENCRYPTED_EXTENSIONS, 0);
  ee->encrypted_extensions.n_extensions = 0;
  ee->encrypted_extensions.extensions = NULL;
  if (!dsk_tls_handshake_serialize (hs_info, ee, error))
    return false;
  dsk_tls_record_layer_send_handshake (hs_info, ee->data_length, ee->data);

  // Optional:  send_handshake CertificateRequest.
  if (...)
    {
      ...
    }

  ... send_handshake (conn, cert_hs);
  ... send_handshake (conn, cert_verify_hs);
}

typedef struct NegotiationStep NegotiationStep;
struct NegotiationStep
{
  const char *name;
  bool (*func)(DskTlsHandshakeNegotiation *hs_info, DskError **error);
};

//
// This is where the server handles the ClientHello message.
//
//
// This the first message of the handshake 
// (and thus of the whole connection),
// unless it is a response to a HelloRetryRequest.
// 
//
// The client offers its preferences and requirements,
// and an initial key-share so that the server can
// start encrypting as soon as it
// computes its end of the key-share.
//
// This function will either:
//    * send a ServerHello if the client's request is fulfillable,
//      return true.
//    * send a HelloRetryRequest for various correctable
//      problems, return true.
//    * return false and set error if there were fatal problems.
//
// This is where most of the negotiation happens.
// 
static bool
handle_client_hello (DskTlsConnection *conn,
                     DskTlsHandshakeMessage  *shake,
                     DskError        **error)
{
  if (conn->state != DSK_TLS_CONNECTION_SERVER_START)
    {
      *error = dsk_error_new ("ClientHello received in %s state: not allowed",
                              dsk_tls_connection_state_name (conn->state));
      return false;
    }


  DskTlsExtension *response_exts[MAX_RESPONSE_EXTENSIONS];
  bool need_client_key_share = false;
  DskTlsHandshakeNegotiation *hs_info = conn->handshake_info;
  DskMemPool *pool = &hs_info->mem_pool;
  hs_info->received_handshake = shake;
  hs_info->client_hello = shake;
  hs_info->n_extensions = 0;
  hs_info->extensions = response_exts;
  hs_info->max_extensions = MAX_RESPONSE_EXTENSIONS;


  static const NegotiationStep ch_negotation_steps[] = {
    NEGOTIATION_STEP(find_server_name),
    NEGOTIATION_STEP(do_tls_version_negotiation),
    NEGOTIATION_STEP(do_keyshare_negotiation),
    NEGOTIATION_STEP(do_psk_negotiation),
  };
  for (unsigned step = 0;
       step < DSK_N_ELEMENTS(ch_negotation_steps);
       step++)
    {
      if (!ch_negotation_steps[step].func (handshake_info, error))
        {
          dsk_add_error_prefix (&error, ch_negotation_steps[step].name);
          return false;
        }
    }


  return true;
}

static 
server_continue_post_psk_negotiation (DskTlsHandshakeNegotiation *hs_info)
{
  static const NegotiationStep server_post_psk_steps[] = {
    NEGOTIATION_STEP(combine_psk_and_keyshare),
    NEGOTIATION_STEP(do_cipher_suite_negotiation),
    NEGOTIATION_STEP(maybe_calculate_key_share),
    NEGOTIATION_STEP(do_certificate_selection),
    NEGOTIATION_STEP(do_early_data_negotiation),
    NEGOTIATION_STEP(do_application_level_protocol_negotiation),
  };

  //
  ...

  if (hs_info->need_hello_retry_request)
    {
      if (!send_hello_retry_request (hs_info, error))
        return false;
    }
  else
    {
      if (!send_server_hello_flight (hs_info, error))
        return false;
    }

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
  DskTlsHandshakeNegotiation *handshake_info = conn->handshake_info;

  //
  // Compute shared secret, use pre-shared-key, or fail.
  //
  ...

  //
  // Find which CipherSuite was agreed upon.
  //
  DskTlsCipherSuiteCode cs_code = shake->server_hello.cipher_suite;
  DskTlsCipherSuite *cs = dsk_tls_cipher_suite_by_code (cs_code);
  if (cs == NULL)
    {
      *error = dsk_error_new ("server selected unavailable cipher-suite (0x%04x)", cs_code);
      return false;
    }
  conn->cipher_suite = cs;

  // optional: [Send EndOfEarlyData]
  ...

  // optional: send Certificate [opt CertificateVerify]
  ..

  // send Finished
  ...
  dsk_tls_key_schedule_compute_finished_data (...);
  ...
  
  //
  // Compute read/write instances (which use different keys).
  //
  DskTlsKeySchedule *schedule = handshake_info->key_schedule;
  assert(conn->shared_key_length == cs->key_size);
  // write instance
  conn->cipher_suite_write_instance = dsk_malloc (cs->instance_size);
  cs->init (conn->cipher_suite_write_instance,
            key_schedule->client_application_write_key);
  // read instance
  conn->cipher_suite_read_instance = dsk_malloc (cs->instance_size);
  cs->init (conn->cipher_suite_read_instance,
            key_schedule->server_application_write_key);
}

//
// We need to either resolve the constraint failures,
// or close the connection.
//
static bool
handle_hello_retry_request (DskTlsConnection *conn,
                            DskTlsHandshakeMessage  *shake,
                            DskError        **error)
{
  switch (conn->state)
    {
    ...
    }
  ...
}

static bool
handle_new_session_ticket  (DskTlsConnection *conn,
                            DskTlsHandshakeMessage  *shake,
                            DskError        **error)
{
  if (conn->context->store_session_tickets (...))
    {
      ...
    }
  ...
}
static bool
handle_end_of_early_data   (DskTlsConnection *conn,
                            DskTlsHandshakeMessage  *shake,
                            DskError        **error)
{
  ...
}
static bool
handle_encrypted_extensions(DskTlsConnection *conn,
                            DskTlsHandshakeMessage  *shake,
                            DskError        **error)
{
  ...
}
static bool
handle_certificate         (DskTlsConnection *conn,
                            DskTlsHandshakeMessage  *shake,
                            DskError        **error)
{
  ...
}
static bool
handle_certificate_request (DskTlsConnection *conn,
                            DskTlsHandshakeMessage  *shake,
                            DskError        **error)
{
  ...
}
static bool
handle_certificate_verify  (DskTlsConnection *conn,
                            DskTlsHandshakeMessage  *shake,
                            DskError        **error)
{
  ...
}
static bool
handle_finished            (DskTlsConnection *conn,
                            DskTlsHandshakeMessage  *shake,
                            DskError        **error)
{
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

static bool handle_underlying_readable (DskStream *underlying,
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
        goto try_parse_record_header;
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
