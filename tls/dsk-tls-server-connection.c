#include "../dsk.h"
#include "dsk-tls-private.h"
#include <stdlib.h>
#include <string.h>

typedef struct DskTlsServerContextClass DskTlsServerContextClass;
struct DskTlsServerContextClass
{
  DskObjectClass base_class;
};

struct DskTlsServerContext
{
  DskObject base_instance;

  unsigned server_allow_pre_shared_keys;
  //bool server_generate_psk_post_handshake;

  size_t n_alps;
  const char **alps;
  bool alpn_required;

  size_t n_certificates;
  DskTlsKeyPair **certificates;

  DskTlsServerSelectPSKFunc server_select_psk;
  void * server_select_psk_data;

  bool always_request_client_certificate;

  DskTlsServerValidateCertFunc validate_client_cert_func;
  void *validate_client_cert_data;
};

static void dsk_tls_server_context_finalize (DskTlsServerContext *);

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskTlsServerContext);
static DskTlsServerContextClass dsk_tls_server_context_class =
{
  DSK_OBJECT_CLASS_DEFINE(
    DskTlsServerContext,
    &dsk_object_class,
    NULL,
    dsk_tls_server_context_finalize
  )
};


//
// Adapted from RFC 8446 A.2, p120.
//
//
//           +--------------->  START 
//           |   Recv ClientHello |
// needs new |                    v
// key share |                 RECVD_CH
//  (HRR)    |                    | Version negotiation
//           |                    | Lookup and test offered PSKs
//           |                    |               | (if PSKs present)
//           |                    |               v
//           |                    |           SELECTING_PSK
//           |                    |               | user called selected_psk()
//           |                    |               |  or no_selected_psk()
//           |                    |               v
//           |                    | Determine CipherSuite
//           |                    v
//           +---------------  DETERMINE_KEY_SHARE [1]         
//                                |
//                                v
//                             FIND_CERT [1] Also determine SignatureAlgorithm
//                                | 
//                                v
//                             NEGOTIATED
//                                | Send ServerHello
//                                | K_send = handshake
//                                | Send EncryptedExtensions
//                                | [Send CertificateRequest]
// Can send                       | [Send Certificate + CertificateVerify]
// app data                       | Send Finished
// after   -->                    | K_send = application
// here                  +--------+--------+
//              No 0-RTT |                 | 0-RTT
//                       |                 |
//   K_recv = handshake  |                 | K_recv = early data
// [Skip decrypt errors] |    +------> WAIT_EOED -+
//                       |    |       Recv |      | Recv EndOfEarlyData
//                       |    | early data |      | K_recv = handshake
//                       |    +------------+      |
//                       |                        |
//                       +> WAIT_FLIGHT2 <--------+
//                                |
//                       +--------+--------+
//               No auth |                 | Client auth
//                       |                 |
//                       |                 v
//                       |             WAIT_CERT
//                       |        Recv |       | Recv Certificate
//                       |       empty |       v
//                       | Certificate |    WAIT_CV
//                       |             |       | Recv
//                       |             v       | CertificateVerify
//                       +-> WAIT_FINISHED <---+
//                                | Recv Finished
//                                | K_recv = application
//                                v
//                            CONNECTED
//

//
// The Handshake from the Server's POV.
//
// The client offers: [RFC 8446, 4.1.1, p. 25]
//   * possibly some pre-shared keys [sessions to re-use].
//     If there are any, it must indicate whether it wants
//     to use Key-Sharing in addition to that, or not.
//     (It may let the server choose)
//   * supported cipher-suites
//   * supported signing techniques
//   * supported key-sharing methods
//   * some key-share public keys [possibly 0, usually 1]
//   * which versions of TLS the client supports
//
// The server must first determine:
//   * if it can handle any of the client's supported versions
//   * if there are any usable pre-shared keys
//   * which cipher suite it wants
//   * whether any of the key-share public keys are acceptable to it
//   * which signature algorithm to use and
//     which certificate to send (If no cert is available, it must abort)
//
// The server will abort the connection if:
//   * no version support
//   * no certificate can be found for any of the signature algorithms
//     the client supports
//   * no SupportedGroup (Key-Share algorithm)
//     is in common with the client
//
// The server will send a HelloRetryRequest if:
//   * the client supports a Key-Share algorithm
//     but did not send a KeyShare for that algo.
//

static void
fail_connection             (DskTlsServerConnection *connection,
                             DskError               *error);
static void
record_layer_send_handshake (DskTlsServerConnection *connection,
                             size_t                  length,
                             const uint8_t          *data);

void
update_transcript_hash      (DskTlsServerHandshake *hs_info,
                             DskTlsHandshakeMessage *message);

// Just frees conn->handshake and sets it to NULL;
// doesn't perform any other state updates.
void
connection_clear_handshake (DskTlsServerConnection *conn);

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
dsk_tls_server_connection_state_name (DskTlsServerConnectionState state)
{
  switch (state)
    {
#define WRITE_CASE(shortname) case DSK_TLS_SERVER_CONNECTION_##shortname: return #shortname
      WRITE_CASE(START);
      WRITE_CASE(RECEIVED_CLIENT_HELLO);
      WRITE_CASE(NEGOTIATED);
      WRITE_CASE(WAIT_END_OF_EARLY_DATA);
      WRITE_CASE(WAIT_FLIGHT2);
      WRITE_CASE(WAIT_CERT);
      WRITE_CASE(WAIT_CERT_VERIFY);
      WRITE_CASE(WAIT_FINISHED);
      WRITE_CASE(CONNECTED);
      WRITE_CASE(FAILED);
#undef WRITE_CASE
      default: assert(false); return "*unknown connection state*";
    }
}

//
// Keyshare Negotiation Result.
//
typedef enum {
  // must be 0 == keyshare negotiation not done yet.
  KS_RESULT_NONE,
                                  
  // no supported KeyShare extension found,
  // but a match in SupportedGroups was found;
  // best_supported_group is set.
  //
  // Will need to either use PSK or do a HelloRetryRequest.
  KS_RESULT_NO_SHARE,

  // remote_key_share is set
  KS_RESULT_SUCCESS,

  // must make HRR: no allowed KeyShare or PSK.
  // no PSK available.
  KS_RESULT_MUST_MAKE_RETRY_REQUEST,

  // KeyShare not possible due to lack of SupportedGroups;
  // no PSK available.
  KS_RESULT_FAIL,
} KSResult;


static DskTlsHandshakeMessage *
create_handshake_message (DskTlsServerHandshake *hs_info,
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


//#define _FIND_EXT(shake, ext_type_suffix, code_suffix) \
//    (DskTlsExtension_##ext_type_suffix *) \
//     find_extension_by_type (shake, DSK_TLS_EXTENSION_TYPE_##code_suffix)
//
//#define FIND_EXT_KEY_SHARE(shake) \
//        _FIND_EXT(shake, KeyShare, KEY_SHARE)
//#define FIND_EXT_SUPPORTED_VERSIONS(shake) \
//        _FIND_EXT(shake, SupportedVersions, SUPPORTED_VERSIONS)
//#define FIND_EXT_SUPPORTED_GROUPS(shake) \
//        _FIND_EXT(shake, SupportedGroups, SUPPORTED_GROUPS)
//#define FIND_EXT_SERVER_NAME(shake) \
//        _FIND_EXT(shake, ServerNameList, SERVER_NAME)
//#define FIND_EXT_PSK(shake) \
//        _FIND_EXT(shake, PreSharedKey, PRE_SHARED_KEY)
//#define FIND_EXT_PSK_KEY_EXCHANGE_MODE(shake) \
//        _FIND_EXT(shake, PSKKeyExchangeModes, PSK_KEY_EXCHANGE_MODES)
//#define FIND_EXT_SIGNATURE_ALGORITHMS(shake) \
//        _FIND_EXT(shake, SignatureAlgorithms, SIGNATURE_ALGORITHMS)
//#define FIND_EXT_EARLY_DATA_INDICATION(shake) \
//        _FIND_EXT(shake, EarlyDataIndication, EARLY_DATA)
//#define FIND_EXT_ALPN(shake) \
//        _FIND_EXT(shake, ALPN, APPLICATION_LAYER_PROTOCOL_NEGOTIATION)

#define MAX_RESPONSE_EXTENSIONS    16

//https://github.com/bifurcation/mint/blob/master/negotiation.go

void
dsk_tls_server_connection_failed (DskTlsServerConnection *conn,
                                  DskError               *error)
{
  if (conn->base_instance.base_instance.latest_error != NULL)
    {
      dsk_warning("failed after: %s\nprevious error:\n%s",
                  conn->base_instance.base_instance.latest_error->message,
                  error->message);
      return;
    }
  dsk_stream_set_error (DSK_STREAM (conn), error);
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
find_server_name (DskTlsServerHandshake *hs_info,
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
do_tls_version_negotiation (DskTlsServerHandshake *hs_info,
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
compare_key_share_by_named_group (const void *a, const void *b)
{
  const DskTlsKeyShareEntry *A = a;
  const DskTlsKeyShareEntry *B = b;
  return A->named_group < B->named_group ? -1
       : A->named_group > B->named_group ? 1
       : 0;
}

static bool
do_keyshare_negotiation (DskTlsServerHandshake *hs_info,
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

  // Select best supported group.
  int best_sg_quality = 0;
  int best_sg_index = -1;
  for (unsigned g = 0; g < n_supported_groups; g++)
    {
      int quality = dsk_tls_support_get_group_quality(supported_groups[g]);
      if (quality > 0 && quality > best_sg_quality)
        {
          best_sg_quality = quality;
          best_sg_index = supported_groups[g];
        }
    }
  if (best_sg_index >= 0)
    hs_info->best_supported_group = supported_groups[best_sg_index];

  DskTlsExtension_KeyShare *keyshare_response = NULL;
  if (n_key_shares == 0 && n_supported_groups == 0)
    {
      // Only Pre-Shared Keys with exchange-mode==PSK_EXCHANGE_MODE_KE.
      hs_info->ks_result = KS_RESULT_NO_SHARE;
    }
  else if (n_key_shares == 0)
    {
      hs_info->ks_result = KS_RESULT_NO_SHARE;
    }
  else  // n_key_shares > 0
    {
      //
      // Find the best key-share
      //
      int best_ks_index = -1;
      int best_ks_quality = 0;
      for (unsigned i = 0; i < n_key_shares; i++)
        {
          int quality = dsk_tls_support_get_group_quality (key_shares[i].named_group);
          if (quality > 0 && (best_ks_index == -1 || best_ks_quality < quality))
            {
              best_ks_index = i;
              best_ks_quality = quality;
            }
        }

      if (best_ks_index >= 0)
        {
          hs_info->found_keyshare = key_shares + best_ks_index;
          hs_info->ks_result = KS_RESULT_SUCCESS;
        }
      else if (best_sg_index >= 0)
        {
          hs_info->ks_result = KS_RESULT_MUST_MAKE_RETRY_REQUEST;
        }
      else
        {
          hs_info->ks_result = KS_RESULT_NO_SHARE;
        }
    }

  if (best_sg_quality < 0)
    {
      *error = dsk_error_new ("key-share negotiation failed (client requested server choose)");
      return false;
    }

  if (keyshare_response != NULL)
    hs_info->keyshare_response = keyshare_response;
  return true;
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

//
static bool
do_psk_negotiation (DskTlsServerHandshake *hs_info,
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
  DskTlsServerConnection *conn = hs_info->conn;
  conn->state = DSK_TLS_SERVER_CONNECTION_SELECTING_PSK;
  conn->context->server_select_psk (hs_info,
                                    offered,
                                    conn->context->server_select_psk_data);
  return true;
}


static void server_continue_post_psk_negotiation (DskTlsServerHandshake *hs_info);
void
dsk_tls_server_handshake_chose_psk (DskTlsServerHandshake *hs_info,
                                    DskTlsPSKKeyExchangeMode exchange_mode,
                                    unsigned                    which_psk,
                                    const DskTlsCipherSuite    *cipher
                                    )
{
  // Simply return true without setting anything up
  // if no PSK was valid.
  assert (which_psk < hs_info->offered_psks->n_identities);

  hs_info->has_psk = true;
  hs_info->psk_key_exchange_mode = exchange_mode;
  hs_info->psk_index = which_psk;
  hs_info->cipher_suite = cipher;

  server_continue_post_psk_negotiation (hs_info);
}
void
dsk_tls_server_handshake_chose_no_psk (DskTlsServerHandshake *hs_info)
{
  server_continue_post_psk_negotiation (hs_info);
}

void
dsk_tls_server_handshake_error_choosing_psk (DskTlsServerHandshake *hs_info,
                                             DskError                    *error)
{
  fail_connection (hs_info->conn, error);
}

// ---------------------------------------------------------------------
//
// Combine the results of Pre-Shared-Key and Key-Share
// negotiations.
//
static bool
combine_psk_and_keyshare (DskTlsServerHandshake *hs_info,
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
  else if (hs_info->ks_result == KS_RESULT_NO_SHARE)
    {
      if (hs_info->has_best_supported_group) 
        {
          hs_info->ks_result = KS_RESULT_MUST_MAKE_RETRY_REQUEST;
        }
      else
        {
          hs_info->ks_result = KS_RESULT_FAIL;
        }
    }
  else
    {
      assert (hs_info->ks_result == KS_RESULT_SUCCESS);
    }
  return true;
}

// ---------------------------------------------------------------------
//                 Cipher Suite negotiation
// ---------------------------------------------------------------------
static bool
do_cipher_suite_negotiation (DskTlsServerHandshake *hs_info,
                             DskError **error)
{
  //
  // CipherSuite may be chosen as part of PSK negotiations.
  //
  if (hs_info->cipher_suite != NULL)
    return true;
  if (hs_info->has_psk)
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
generate_key_pair (DskTlsServerHandshake *hs_info,
                   const DskTlsKeyShareMethod *method)
{
  DskMemPool *pool = &hs_info->mem_pool;
  uint8_t *priv_key = dsk_mem_pool_alloc (pool, method->private_key_bytes);
  uint8_t *pub_key = dsk_mem_pool_alloc (pool, method->public_key_bytes);
  do
    dsk_get_cryptorandom_data (method->private_key_bytes, priv_key);
  while (!method->make_key_pair(method, priv_key, pub_key));

  hs_info->key_share_method = method;
  hs_info->public_key = pub_key;
  hs_info->private_key = priv_key;
}

static bool
maybe_calculate_key_share (DskTlsServerHandshake *hs_info,
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
        DskTlsKeyShareEntry *remote_share = hs_info->found_keyshare;
        const DskTlsKeyShareMethod *method = hs_info->key_share_method;
        assert(method != NULL);
        if (remote_share->key_exchange_length != method->public_key_bytes)
          {
            *error = dsk_error_new ("KeyShare method does not match public key length");
            return false;
          }

        // generate key pair
        generate_key_pair (hs_info, method);

        // compute shared secret.
        hs_info->shared_key = dsk_mem_pool_alloc (&hs_info->mem_pool,
                                                  method->shared_key_bytes);
        method->make_shared_key (method,
                                 hs_info->private_key,
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
      DskTlsKeyPair *certificate;
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
do_certificate_selection (DskTlsServerHandshake *hs_info,
                          DskError **error)
{
  if (hs_info->has_psk)
    return true;

  //
  // Perform Certificate Selection
  //
  DskTlsExtension_SignatureAlgorithms *sig_algos = FIND_EXT_SIGNATURE_ALGORITHMS(hs_info->received_handshake);
  if (sig_algos == NULL)
    {
      *error = dsk_error_new ("client didn't specify SignatureAlgorithms");
      return false;
    }

  DskTlsServerConnection *conn = hs_info->conn;
  DskTlsServerContext *context = conn->context;
  size_t n_certs = context->n_certificates;
  DskTlsKeyPair **certs = context->certificates;
  for (size_t i = 0; i < n_certs; i++)
    {
      // TODO: handle RAW_PUBLIC_KEY
      if (!DSK_IS_TLS_X509_CERTIFICATE (certs[i]))
        continue;
      DskTlsX509Certificate *cert = DSK_TLS_X509_CERTIFICATE (certs[i]);
      //
      // Filter out certificates whose
      // name doesn't match the server_name extension value.
      //
      if (hs_info->server_hostname != NULL)
        {
          DskTlsX509Name *subject_name = &cert->subject;
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
        if (dsk_tls_key_pair_supports_scheme (certs[i], sig_schemes[j]))
          {
            hs_info->certificate_scheme = sig_schemes[j];

            //
            // RFC 8446 4.2.2: "Valid extensions for server certificates at
            // present include the OCSP Status extension [RFC6066]
            // and the SignedCertificateTimestamp extension [RFC6962];
            // future extensions may be defined for this message as well.
            // Extensions in the Certificate message from the server MUST
            // correspond to ones from the ClientHello message."
            //
            DskTlsHandshakeMessage *cert_hs = create_handshake_message (hs_info, DSK_TLS_HANDSHAKE_MESSAGE_TYPE_CERTIFICATE, 0);
            cert_hs->certificate.n_entries = 1;
            DskTlsCertificateEntry *entry = HS_ALLOC0 (hs_info, DskTlsCertificateEntry);
            cert_hs->certificate.entries = entry;
            entry[0].cert_data_length = cert->cert_data_length;
            entry[0].cert_data = cert->cert_data;
            entry[0].n_extensions = 0;
            entry[0].extensions = NULL;
            assert(hs_info->n_extensions < hs_info->max_extensions);
            hs_info->certificate_hs = cert_hs;
            hs_info->certificate = cert;
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
do_early_data_negotiation (DskTlsServerHandshake *hs_info,
                           DskError **error)
{
  DskTlsHandshakeMessage *shake = hs_info->received_handshake;
  DskTlsExtension_EarlyDataIndication *edi = FIND_EXT_EARLY_DATA_INDICATION (shake);
  if (edi != NULL
   && hs_info->has_psk
   && hs_info->psk_key_exchange_mode == DSK_TLS_PSK_EXCHANGE_MODE_KE
   && hs_info->psk_index == 0)
    {
      // note: We must send an EarlyData extension in EncryptedExtensions.
      hs_info->receiving_early_data = true;
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
do_application_level_protocol_negotiation (DskTlsServerHandshake *hs_info,
                           DskError **error)
{
  DskTlsServerConnection *conn = hs_info->conn;
  DskTlsHandshakeMessage *shake = hs_info->received_handshake;
  DskMemPool *pool = &hs_info->mem_pool;
  DskTlsServerContext *context = conn->context;
  if (hs_info->has_psk)
    return true;
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
send_hello_retry_request (DskTlsServerHandshake *hs_info,
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
  DskTlsHandshakeMessage *sh = create_handshake_message(hs_info, DSK_TLS_HANDSHAKE_MESSAGE_TYPE_SERVER_HELLO, 16);
  sh->server_hello.is_retry_request = true;
  sh->server_hello.n_extensions = 0;
  sh->server_hello.extensions = HS_ALLOC_ARRAY (hs_info, MAX_HRR_EXTENSIONS, DskTlsExtension *);

  if (hs_info->keyshare_response != NULL)
    {
      sh->server_hello.extensions[sh->server_hello.n_extensions++] = (DskTlsExtension *) hs_info->keyshare_response;
    }
  // TODO:  supported-groups, cookie?
  if (!dsk_tls_handshake_message_serialize (sh, &hs_info->mem_pool, error))
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
  record_layer_send_handshake (hs_info->conn, sh->data_length, sh->data);

  return true;
}

//
// Compute the CertificateVerify handshake-message.
//
static DskTlsHandshakeMessage *
compute_certificate_verify (DskTlsServerHandshake *hs_info)
{
  DskTlsHandshakeMessage *cert_ver = HS_ALLOC0 (hs_info, DskTlsHandshakeMessage);
  DskTlsX509Certificate *cert = hs_info->certificate;
  cert_ver->type = DSK_TLS_HANDSHAKE_MESSAGE_TYPE_CERTIFICATE_VERIFY;
  cert_ver->certificate_verify.algorithm = hs_info->certificate_scheme;
  unsigned sig_length = dsk_tls_key_pair_get_signature_length (&cert->base_instance, hs_info->certificate_scheme);
  cert_ver->certificate_verify.signature_length = sig_length;
  uint8_t *sig_data = dsk_mem_pool_alloc (&hs_info->mem_pool, sig_length);
  cert_ver->certificate_verify.signature_data = sig_data;

  // RFC 8446, Section 4.4.3:
  //
  //      The digital signature is then computed over the concatenation of:
  //        -  A string that consists of octet 32 (0x20) repeated 64 times
  //        -  The context string
  //           - for servers: "TLS 1.3, server CertificateVerify"
  //           - for clients: "TLS 1.3, client CertificateVerify"
  //        -  A single 0 byte which serves as the separator
  //        -  The content to be signed == Transcript-Hash(Handshake-Context, Certificate)
  //
  // The length of each of these components is 64, 33, 1, hashlen.
  const DskChecksumType *hash_type = hs_info->cipher_suite->hash_type;
  unsigned hash_size = hash_type->hash_size;
  unsigned content_to_sign_len = 98 + hash_size;
  uint8_t *content_to_sign = alloca (content_to_sign_len);
  memset(content_to_sign, 32, 64);
  memcpy (content_to_sign + 64, 
                 // 012345678901234567890123456789012 (33 bytes w/o nul; 34 bytes w/ nul)
          "TLS 1.3, server CertificateVerify",
          34 // includes NUL
          );
  assert(hs_info->transcript_hash_last_message_type == DSK_TLS_HANDSHAKE_MESSAGE_TYPE_CERTIFICATE);
  uint8_t *transcript_hash = alloca (hash_size);
  void *instance_copy = alloca(hash_type->instance_size);
  memcpy (instance_copy, hs_info->transcript_hash_instance, hash_type->instance_size);
  hash_type->end(instance_copy, transcript_hash);
  memcpy (content_to_sign + 98,
          transcript_hash,
          hash_size);

  dsk_tls_key_pair_sign (&cert->base_instance,
                         hs_info->certificate_scheme,
                         content_to_sign_len,
                         content_to_sign,
                         sig_data);
  return cert_ver;
}

static bool
send_server_hello_flight (DskTlsServerHandshake *hs_info,
                          DskError **error)
{
  DskTlsServerConnection *conn = hs_info->conn;
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
      const DskTlsPresharedKeyIdentity *pi =
        hs_info->offered_psks->identities + hs_info->psk_index;
      psk_size = pi->identity_length;
      psk = pi->identity;
    }
  dsk_tls_key_schedule_compute_early_secrets (key_schedule,
                                              false,    // externally shared
                                              client_hello_transcript_hash,
                                              psk_size, psk);

  DskTlsHandshakeMessage *server_hello;
  server_hello = create_handshake_message (hs_info, DSK_TLS_HANDSHAKE_MESSAGE_TYPE_SERVER_HELLO, 16);
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
  if (!dsk_tls_handshake_message_serialize (server_hello, &hs_info->mem_pool, error))
    return false;
  record_layer_send_handshake (hs_info->conn, server_hello->data_length, server_hello->data);

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
                                                  hs_info->key_share_method->shared_key_bytes,
                                                  hs_info->shared_key);


  // set decryptor, encryptor
//  conn->cipher_suite->init (conn->cipher_suite_read_instance,
//                            key_schedule->client_handshake_write_key);
  conn->cipher_suite->init (conn->cipher_suite_write_instance,
                            key_schedule->server_handshake_write_key);

  // send_handshake encrypted_extensions
  DskTlsHandshakeMessage *ee = create_handshake_message (hs_info, DSK_TLS_HANDSHAKE_MESSAGE_TYPE_ENCRYPTED_EXTENSIONS, 0);
  ee->encrypted_extensions.n_extensions = 0;
  ee->encrypted_extensions.extensions = NULL;
  if (!dsk_tls_handshake_message_serialize (ee, &hs_info->mem_pool, error))
    return false;
  record_layer_send_handshake (hs_info->conn, ee->data_length, ee->data);

  // Optional:  send_handshake CertificateRequest.
  //
  // RFC 8446 4.3.2:
  //    Servers which are authenticating with a PSK MUST NOT send the
  //    CertificateRequest message in the main handshake, though they MAY
  //    send it in post-handshake authentication (see Section 4.6.2) provided
  //    that the client has sent the "post_handshake_auth" extension (see
  //    Section 4.2.6).
  //
  if (hs_info->request_client_certificate
   && !hs_info->has_psk)
    {
      //
      // RFC 8446 4.3.2.  The "signature_algorithms" extension
      // MUST be specified, and other extensions may optionally be included
      // if defined for this message.  Clients MUST ignore unrecognized
      // extensions.
      //
      // In prior versions of TLS, the CertificateRequest message carried a
      // list of signature algorithms and certificate authorities which the
      // server would accept.  In TLS 1.3, the former is expressed by sending
      // the "signature_algorithms" and optionally "signature_algorithms_cert"
      // extensions.  The latter is expressed by sending the
      // "certificate_authorities" extension (see Section 4.2.4).
      //
      hs_info->cert_req_hs = HS_ALLOC0 (hs_info, DskTlsHandshakeMessage);
      hs_info->cert_req_hs->type = DSK_TLS_HANDSHAKE_MESSAGE_TYPE_CERTIFICATE_REQUEST;

      DskTlsExtension *exts[6];
      unsigned n_exts = 0;
      DskTlsExtension_SignatureAlgorithms *ext_sa = HS_ALLOC0 (hs_info, DskTlsExtension_SignatureAlgorithms);
      ext_sa->n_schemes = dsk_tls_n_signature_schemes;
      ext_sa->schemes = dsk_tls_signature_schemes;
      ext_sa->base.type = DSK_TLS_EXTENSION_TYPE_SIGNATURE_ALGORITHMS;
      exts[n_exts++] = (DskTlsExtension *) ext_sa;

      // TODO: CA extension

      assert(n_exts <= DSK_N_ELEMENTS(exts));
      hs_info->cert_req_hs->certificate_request.n_extensions = n_exts;
      hs_info->cert_req_hs->certificate_request.extensions = HS_ALLOC_ARRAY (hs_info, n_exts, DskTlsExtension *);
      memcpy (hs_info->cert_req_hs->certificate_request.extensions, exts, n_exts * sizeof(DskTlsExtension*));

      if (!dsk_tls_handshake_message_serialize (hs_info->cert_req_hs, &hs_info->mem_pool, error))
        return false;
      record_layer_send_handshake (hs_info->conn,
                                   hs_info->cert_req_hs->data_length,
                                   hs_info->cert_req_hs->data);
    }

  //
  // Send Certificate handshake. (obtained earlier from do_certificate_selection())
  //
  if (!dsk_tls_handshake_message_serialize (hs_info->cert_hs, &hs_info->mem_pool, error))
    return false;
  record_layer_send_handshake (hs_info->conn,
                               hs_info->cert_hs->data_length,
                               hs_info->cert_hs->data);

  //
  // Compute Certificate Verify.
  //
  DskTlsHandshakeMessage *cert_ver = compute_certificate_verify (hs_info);
  if (!dsk_tls_handshake_message_serialize (cert_ver, &hs_info->mem_pool, error))
    return false;
  record_layer_send_handshake (hs_info->conn,
                               cert_ver->data_length,
                               cert_ver->data);
  return true;
}

typedef struct NegotiationStep NegotiationStep;
struct NegotiationStep
{
  const char *name;
  bool (*func)(DskTlsServerHandshake *hs_info, DskError **error);
};
#define NEGOTIATION_STEP(f) { #f, f }

//
// This is where the server handles the ClientHello message.
//
// This the first message of the handshake 
// (and thus of the whole connection),
// unless it is a response to a HelloRetryRequest.
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
static DskTlsBaseHandshakeMessageResponseCode
handle_client_hello (DskTlsServerConnection *conn,
                     DskTlsHandshakeMessage  *shake,
                     DskError        **error)
{
  if (conn->state != DSK_TLS_SERVER_CONNECTION_START)
    {
      *error = dsk_error_new ("ClientHello received in %s state: not allowed",
                              dsk_tls_server_connection_state_name (conn->state));
      return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
    }


  DskTlsExtension *response_exts[MAX_RESPONSE_EXTENSIONS];
  DskTlsServerHandshake *hs_info = conn->handshake;
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
      if (!ch_negotation_steps[step].func (hs_info, error))
        {
          dsk_add_error_prefix (error, "%s (%s:%u)",
                                ch_negotation_steps[step].name,
                                __FILE__, __LINE__);
          return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
        }
    }

  return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_OK;
}

static void
server_continue_post_psk_negotiation (DskTlsServerHandshake *hs_info)
{
  DskError *error = NULL;

  static const NegotiationStep server_post_psk_steps[] = {
    NEGOTIATION_STEP(combine_psk_and_keyshare),
    NEGOTIATION_STEP(do_cipher_suite_negotiation),
    NEGOTIATION_STEP(maybe_calculate_key_share),
    NEGOTIATION_STEP(do_certificate_selection),
    NEGOTIATION_STEP(do_early_data_negotiation),
    NEGOTIATION_STEP(do_application_level_protocol_negotiation),
  };

  for (unsigned i = 0; i < DSK_N_ELEMENTS (server_post_psk_steps); i++)
    {
      if (!(server_post_psk_steps[i].func (hs_info, &error)))
        {
          fail_connection (hs_info->conn, error);
          dsk_error_unref (error);
          return;
        }
    }

  if (hs_info->must_send_hello_retry_request)
    {
      if (!send_hello_retry_request (hs_info, &error))
        {
          fail_connection (hs_info->conn, error);
          dsk_error_unref (error);
          return;
        }
    }
  else
    {
      if (!send_server_hello_flight (hs_info, &error))
        {
          fail_connection (hs_info->conn, error);
          dsk_error_unref (error);
          return;
        }
    }

}
static DskTlsBaseHandshakeMessageResponseCode
handle_end_of_early_data   (DskTlsServerConnection *conn,
                            DskTlsHandshakeMessage  *shake,
                            DskError        **error)
{
  DskTlsServerHandshake *hs_info = conn->handshake;
  (void) shake;
  if (!hs_info->receiving_early_data)
    {
      *error = dsk_error_new ("End-of-Early-Data not expected");
      DSK_ERROR_SET_TLS (*error, FATAL, UNEXPECTED_MESSAGE);
      return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
    }
  hs_info->receiving_early_data = false;
  return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_OK;
}

static DskTlsBaseHandshakeMessageResponseCode
handle_certificate         (DskTlsServerConnection *conn,
                            DskTlsHandshakeMessage  *shake,
                            DskError        **error)
{
  if (conn->state != DSK_TLS_SERVER_CONNECTION_WAIT_CERT)
    {
      *error = dsk_error_new ("received client Certificate in invalid state");
      DSK_ERROR_SET_TLS (*error, FATAL, UNEXPECTED_MESSAGE);
      return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
    }
  DskTlsServerHandshake *hs_info = conn->handshake;

  hs_info->certificate_hs = shake;
  DskTlsServerContext *ctx = conn->context;
  if (shake->certificate.n_entries == 0
   && !ctx->validate_client_cert_func (hs_info, 
                                       0, NULL,
                                       ctx->validate_client_cert_data,
                                       error))
    {
      return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
    }

  conn->state = (shake->certificate.n_entries == 0)
              ? DSK_TLS_SERVER_CONNECTION_WAIT_FINISHED
              : DSK_TLS_SERVER_CONNECTION_WAIT_CERT_VERIFY;
  update_transcript_hash (hs_info, shake);
  return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_OK;
}
static DskTlsBaseHandshakeMessageResponseCode
handle_certificate_verify  (DskTlsServerConnection *conn,
                            DskTlsHandshakeMessage  *shake,
                            DskError        **error)
{
  DskTlsServerHandshake *hs_info = conn->handshake;
  (void) shake;

  if (conn->state != DSK_TLS_SERVER_CONNECTION_WAIT_CERT_VERIFY)
    {
      *error = dsk_error_new ("received client CertificateVerify in invalid state");
      DSK_ERROR_SET_TLS (*error, FATAL, UNEXPECTED_MESSAGE);
      return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
    }

  //
  // Verify the signature on the certs, before calling the user certificate
  // validator.
  //
  const DskChecksumType *sig_hash = conn->cipher_suite->hash_type;
  //                         0         1         2         3
  //                         012345678901234567890123456789012
  const char *sig_context = "TLS 1.3, client CertificateVerify"; //len=33
  unsigned sig_context_len = 33;                // strlen(sig_context)
  unsigned sig_subject_len = 64 + 33 + 1 + sig_hash->hash_size;
  uint8_t *sig_subject = alloca (sig_subject_len);
  memset (sig_subject, ' ', 64);
  memcpy (sig_subject + 64, sig_context, sig_context_len + 1); // include NUL
  void *transcript_hash_instance_copy = alloca (sig_hash->instance_size);
  memcpy (transcript_hash_instance_copy,
          hs_info->transcript_hash_instance,
          sig_hash->instance_size);
  sig_hash->end (transcript_hash_instance_copy,
                 sig_subject + 64 + sig_context_len + 1);
  DskTlsKeyPair *sig_scheme = hs_info->certificate_key_pair;
  DskTlsKeyPairClass *sig_scheme_class = DSK_TLS_KEY_PAIR_GET_CLASS (sig_scheme);
  DskTlsSignatureScheme sig_scheme_code = hs_info->certificate_scheme;
  if (shake->certificate_verify.signature_length
      != sig_scheme_class->get_signature_length (sig_scheme, sig_scheme_code))
                                                 
    {
      *error = dsk_error_new ("invalid signature- wrong length for scheme");
      return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
    }
  if (!sig_scheme_class->verify (sig_scheme, sig_scheme_code,
                                 sig_subject_len, sig_subject,
                                 shake->certificate_verify.signature_data))
    {
      *error = dsk_error_new ("certificate signature validation failed");
      return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
    }

  //
  // Certificates acceptable to user?
  //
  DskTlsServerContext *ctx = conn->context;
  if (!ctx->validate_client_cert_func 
             (hs_info, 
              hs_info->certificate_hs->certificate.n_entries,
              hs_info->certificate_hs->certificate.entries,
              ctx->validate_client_cert_data,
              error))
    {
      // Assert: client must set *error if validation failed.
      assert(*error != NULL);
      return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
    }

  conn->state = DSK_TLS_SERVER_CONNECTION_WAIT_FINISHED;
  return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_OK;
}

static DskTlsBaseHandshakeMessageResponseCode
handle_finished    (DskTlsServerConnection  *conn,
                    DskTlsHandshakeMessage  *shake,
                    DskError               **error)
{
  (void) shake;

  if (conn->state != DSK_TLS_SERVER_CONNECTION_WAIT_FINISHED)
    {
      *error = dsk_error_new ("received client Finished in invalid state");
      DSK_ERROR_SET_TLS (*error, FATAL, UNEXPECTED_MESSAGE);
      return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_FAILED;
    }
  conn->state = DSK_TLS_SERVER_CONNECTION_CONNECTED;
  // XXX: switch to application master key.
  return DSK_TLS_BASE_HANDSHAKE_MESSAGE_RESPONSE_OK;
}

static DskTlsBaseHandshakeMessageResponseCode
dsk_tls_server_connection_handle_handshake_message
                               (DskTlsBaseConnection    *connection,
                                DskTlsHandshakeMessage  *shake,
                                DskError               **error)
{
  DskTlsServerConnection *conn = DSK_TLS_SERVER_CONNECTION (connection);
  switch (shake->type)
    {
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_CLIENT_HELLO:
      return handle_client_hello (conn, shake, error);
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_END_OF_EARLY_DATA:
      return handle_end_of_early_data (conn, shake, error);
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_CERTIFICATE:
      return handle_certificate (conn, shake, error);
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_CERTIFICATE_VERIFY:
      return handle_certificate_verify (conn, shake, error);
    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_FINISHED:
      return handle_finished (conn, shake, error);
//    case DSK_TLS_HANDSHAKE_MESSAGE_TYPE_KEY_UPDATE:
//      return handle_key_update (conn, shake, error);
    default:
      *error = dsk_error_new ("unexpected message received");
      DSK_ERROR_SET_TLS (*error, FATAL, UNEXPECTED_MESSAGE);
      return false;
    }
}

static void
dsk_tls_server_connection_handle_application_data
       (DskTlsBaseConnection *connection,
        DskTlsHandshakeMessage *message,
        DskError **error);
static void 
dsk_tls_server_connection_fatal_alert_received
       (DskTlsBaseConnection *connection,
        DskTlsAlertDescription description);
static bool 
dsk_tls_server_connection_warning_alert_received
       (DskTlsBaseConnection *connection,
        DskTlsAlertDescription description,
        DskError **error);

static void
dsk_tls_server_connection_finalize (DskTlsServerConnection *conn)
{
  if (conn->base_instance.handshake != NULL)
    {
      DskTlsServerHandshake *hs_info = conn->base_instance.handshake;
      dsk_mem_pool_clear (&hs_info->mem_pool);
      dsk_free (hs_info);
    }
}

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskTlsServerConnection);
DskTlsServerConnectionClass dsk_tls_server_connection_class = {
  DSK_TLS_BASE_CONNECTION_DEFINE_CLASS(
    DskTlsServerConnection,
    dsk_tls_server_connection
  )
};

//
// A Single Connection from a server.
//
DskTlsServerConnection *
dsk_tls_server_connection_new (DskStream           *underlying,
                               DskTlsServerContext *context,
                               DskError           **error)
{
  DskTlsServerConnection *rv;
  rv = dsk_object_new (&dsk_tls_server_connection_class);
  rv->handshake = DSK_NEW0 (DskTlsServerHandshake);
  rv->handshake->conn = rv;
  rv->context = dsk_object_ref (context);
  if (!dsk_tls_base_connection_init_underlying (&rv->base_instance, underlying, error))
    {
      dsk_object_unref (rv);
      return NULL;
    }
  return rv;
}
