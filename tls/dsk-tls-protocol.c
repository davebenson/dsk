#include "../dsk.h"
#include <string.h>
#include <stdio.h>

const char *
dsk_tls_record_content_type_name (DskTlsRecordContentType type)
{
  switch (type)
    {
#define WRITE_CASE(shortname) case DSK_TLS_RECORD_CONTENT_TYPE_##shortname: return #shortname
      WRITE_CASE(INVALID);
      WRITE_CASE(CHANGE_CIPHER_SPEC);
      WRITE_CASE(ALERT);
      WRITE_CASE(HANDSHAKE);
      WRITE_CASE(APPLICATION_DATA);
#undef WRITE_CASE
      default: return "*unknown record-content type*";
    }
}

const char *
dsk_tls_extension_type_name(DskTlsExtensionType type)
{
  switch (type)
    {
#define WRITE_CASE(shortname) case DSK_TLS_EXTENSION_TYPE_##shortname: return #shortname
      WRITE_CASE(SERVER_NAME);
      WRITE_CASE(MAX_FRAGMENT_LENGTH);
      WRITE_CASE(STATUS_REQUEST);
      WRITE_CASE(SUPPORTED_GROUPS);
      WRITE_CASE(SIGNATURE_ALGORITHMS);
      WRITE_CASE(USE_SRTP);
      WRITE_CASE(HEARTBEAT);
      WRITE_CASE(APPLICATION_LAYER_PROTOCOL_NEGOTIATION);
      WRITE_CASE(SIGNED_CERTIFICATE_TIMESTAMP);
      WRITE_CASE(CLIENT_CERTIFICATE_TYPE);
      WRITE_CASE(SERVER_CERTIFICATE_TYPE);
      WRITE_CASE(PADDING);
      WRITE_CASE(SESSION_TICKET);
      WRITE_CASE(RECORD_SIZE_LIMIT);
      WRITE_CASE(PRE_SHARED_KEY);
      WRITE_CASE(EARLY_DATA);
      WRITE_CASE(SUPPORTED_VERSIONS);
      WRITE_CASE(COOKIE);
      WRITE_CASE(PSK_KEY_EXCHANGE_MODES);
      WRITE_CASE(CERTIFICATE_AUTHORITIES);
      WRITE_CASE(OID_FILTERS);
      WRITE_CASE(POST_HANDSHAKE_AUTH);
      WRITE_CASE(SIGNATURE_ALGORITHMS_CERT);
      WRITE_CASE(KEY_SHARE);
#undef WRITE_CASE
      default: return "*unknown extension type*";
    }
}

const char *dsk_tls_signature_scheme_name (DskTlsSignatureScheme scheme)
{
  switch (scheme)
    {
#define WRITE_CASE(shortname) case DSK_TLS_SIGNATURE_SCHEME_##shortname: return #shortname
      WRITE_CASE(RSA_PKCS1_SHA256);
      WRITE_CASE(RSA_PKCS1_SHA384);
      WRITE_CASE(RSA_PKCS1_SHA512);
      WRITE_CASE(ECDSA_SECP256R1_SHA256);
      WRITE_CASE(ECDSA_SECP384R1_SHA384);
      WRITE_CASE(ECDSA_SECP521R1_SHA512);
      WRITE_CASE(RSA_PSS_RSAE_SHA256);
      WRITE_CASE(RSA_PSS_RSAE_SHA384);
      WRITE_CASE(RSA_PSS_RSAE_SHA512);
      WRITE_CASE(ED25519);
      WRITE_CASE(ED448);
      WRITE_CASE(RSA_PSS_PSS_SHA256);
      WRITE_CASE(RSA_PSS_PSS_SHA384);
      WRITE_CASE(RSA_PSS_PSS_SHA512);
      WRITE_CASE(RSA_PKCS1_SHA1);
      WRITE_CASE(ECDSA_SHA1);
#undef WRITE_CASE
      default: return "*unknown signature scheme*";
    }
}

DskTlsRecordHeaderParseResult
dsk_tls_parse_record_header (DskBuffer               *buffer)
{
  DskTlsRecordHeaderParseResult result;
  if (buffer->size < 5)
    {
      result.code = DSK_TLS_PARSE_RESULT_CODE_TOO_SHORT;
      return result;
    }
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
      {
        result.code = DSK_TLS_PARSE_RESULT_CODE_ERROR;
        result.error = dsk_error_new ("TLS record header: bad content type 0x%02x", header[0]);
        return result;
      }
    }
  unsigned length = dsk_uint16be_parse (header + 4);
  if (5 + length > buffer->size)
    {
      result.code = DSK_TLS_PARSE_RESULT_CODE_TOO_SHORT;
      return result;
    }
  if (length > (1<<14))
    {
      result.code = DSK_TLS_PARSE_RESULT_CODE_ERROR;
      result.error = dsk_error_new ("record_overflow error");
      return result;
    }
  result.code = DSK_TLS_PARSE_RESULT_CODE_OK;
  result.ok.content_type = header[0];
  result.ok.header_length = 5;
  result.ok.payload_length = length;
  return result;
}

#if 0
typedef struct HandshakeParseResult
{
  ParseResultCode code;
  union {
    DskTlsHandshake *handshake;      // if code==PARSE_RESULT_CODE_OK
    DskError *error;                 // if code==PARSE_RESULT_CODE_ERROR
  };
} HandshakeParseResult;
#endif

static bool
count_extensions (const uint8_t *at,
                  const uint8_t *end,
                  unsigned *count_out)
{
  *count_out = 0;
  while (at + 4 <= end)
    {
      uint8_t extension_data_len = dsk_uint16be_parse (at + 2);
      if (at + 4 + extension_data_len > end)
        return false;
      at += 4 + extension_data_len;
      *count_out += 1;
    }
  if (at != end)
    return false;
  return true;
}

bool
dsk_tls_handshake_is_server_hello(DskTlsHandshake *shake)
{
  return shake->type == DSK_TLS_HANDSHAKE_TYPE_SERVER_HELLO
      && !shake->server_hello.is_retry_request;
}

bool
dsk_tls_handshake_is_hello_retry_request(DskTlsHandshake *shake)
{
  return shake->type == DSK_TLS_HANDSHAKE_TYPE_SERVER_HELLO
      && shake->server_hello.is_retry_request;
}

static DskTlsExtension *
parse_extension (DskTlsHandshake    *under_construction,
                 DskTlsExtensionType type,
                 unsigned            ext_payload_len,
                 const uint8_t      *ext_payload,
                 DskMemPool         *pool,
                 DskError          **error)
{
  DskTlsExtension *ext = dsk_mem_pool_alloc (pool, sizeof (DskTlsExtension));
  ext->generic.type = type;
  ext->generic.extension_data_length = ext_payload_len;
  ext->generic.extension_data = ext_payload;
  ext->generic.is_generic = false;
  const uint8_t *at = ext_payload;
  const uint8_t *end = ext_payload + ext_payload_len;
  switch (type)
    {
    case DSK_TLS_EXTENSION_TYPE_SERVER_NAME:
      {
        //
        // From RFC 6066, Section 3:
        //    struct {
        //        NameType name_type;
        //        select (name_type) {
        //            case host_name: HostName;
        //        } name;
        //    } ServerName;
        //
        //    enum {
        //        host_name(0), (255)
        //    } NameType;
        //
        //    opaque HostName<1..2^16-1>;
        //
        //    struct {
        //        ServerName server_name_list<1..2^16-1>
        //    } ServerNameList;
        //
        // extension_data is a ServerNameList.
        if (at + 2 > end)
          {
            *error = dsk_error_new ("too short in ServerName extension");
            return false;
          }
        unsigned names_size = dsk_uint16be_parse (at);
        at += 2;
        if (at + names_size > end)
          {
            *error = dsk_error_new ("names overrune in ServerName extension");
            return false;
          }
        const uint8_t *names_start = at;
        unsigned n_names = 0;
        while (at < end)
          {
            if (*at != 0)
              {
                *error = dsk_error_new ("unknown NameType in ServerName extension [0x%02x]", *at);
                return false;
              }
            if (at + 3 > end)
              {
                *error = dsk_error_new ("too short in ServerName extension");
                return false;
              }
            unsigned name_len = dsk_uint16be_parse (at + 1);
            at += 3;
            if (name_len + at > end)
              {
                *error = dsk_error_new ("name overruns ServerName extension");
                return false;
              }
            at += name_len;
            n_names++;
          }
        ext->server_name.n_entries = n_names;
        DskTlsExtension_ServerNameList_Entry *entries;
        entries = dsk_mem_pool_alloc (pool,
                n_names * sizeof (DskTlsExtension_ServerNameList_Entry));
        ext->server_name.entries = entries;
        at = names_start;
        for (unsigned i = 0; i < n_names; i++)
          {
            entries[i].type = *at;
            unsigned nlen = dsk_uint16be_parse (at + 1);
            entries[i].name_length = nlen;
            char *name = dsk_mem_pool_alloc_unaligned (pool, nlen + 1);
            entries[i].name = name;
            memcpy (name, at + 3, nlen);
            name[nlen] = 0;
            at += nlen + 3;
          }
        return ext;
      }
    case DSK_TLS_EXTENSION_TYPE_MAX_FRAGMENT_LENGTH:
      // RFC 6606, Section 4.
      //
      // The "extension_data" field of this extension SHALL
      // contain:
      //
      //    enum{
      //        2^9(1), 2^10(2), 2^11(3), 2^12(4), (255)
      //    } MaxFragmentLength;
      //
      if (ext_payload_len != 1)
        {
          *error = dsk_error_new ("payload for MaxFragmentLength extension must be exactly one byte");
          return NULL;
        }
      if (1 <= *ext_payload && *ext_payload <= 4)
        {
          ext->max_fragment_length.log2_length = *ext_payload + 8;
          return ext;
        }
      *error = dsk_error_new ("bad length code MaxFragmentLength extension: illegal parameter");
      return NULL;

    //case DSK_TLS_EXTENSION_TYPE_STATUS_REQUEST:
    case DSK_TLS_EXTENSION_TYPE_SUPPORTED_GROUPS:
      if (ext_payload_len < 2)
        {
          *error = dsk_error_new ("too short in SupportedGroups extension");
          return NULL;
        }
      unsigned group_length = dsk_uint16be_parse (ext_payload);
      if (ext_payload_len != group_length + 2)
        {
          *error = dsk_error_new ("bad format in SupportedGroups extension");
          return NULL;
        }
      if (group_length % 2 != 0)
        {
          *error = dsk_error_new ("bad length of array in SupportedGroups extension");
          return NULL;
        }
      ext->supported_groups.n_supported_groups = group_length / 2;
      ext->supported_groups.supported_groups = dsk_mem_pool_alloc (pool, group_length);
      for (unsigned i = 0; i < ext->supported_groups.n_supported_groups; i++)
        ext->supported_groups.supported_groups[i] = dsk_uint16be_parse (ext_payload + 2 * i + 2);
      for (unsigned i = 0; i < ext->supported_groups.n_supported_groups; i++)
        fprintf(stderr, "supported-group[%u] = %04x", i, ext->supported_groups.supported_groups[i]);
      return ext;

    case DSK_TLS_EXTENSION_TYPE_SIGNATURE_ALGORITHMS:
      if (under_construction->type == DSK_TLS_HANDSHAKE_TYPE_CLIENT_HELLO)
        {
	  if (ext_payload_len < 2)
	    {
	      *error = dsk_error_new ("too short in SignatureAlgorithms extension");
	      return NULL;
	    }
	  unsigned algo_size = dsk_uint16be_parse (ext_payload);
	  if (algo_size + 2 != ext_payload_len || algo_size % 2 != 0)
	    {
	      *error = dsk_error_new ("malformed SignatureAlgorithms extension");
	      return NULL;
	    }
	  unsigned n = algo_size / 2;
	  ext->signature_algorithms.n_schemes = n;
	  ext->signature_algorithms.schemes = dsk_mem_pool_alloc (pool, n * sizeof(DskTlsSignatureScheme));
	  for (unsigned i = 0; i < n; i++)
	    ext->signature_algorithms.schemes[i] = dsk_uint16be_parse (ext_payload + 2 + i * 2);
        }
      else if (dsk_tls_handshake_is_server_hello (under_construction))
        {
          //...
        }
    break;

    //TODO:case DSK_TLS_EXTENSION_TYPE_USE_SRTP:
    case DSK_TLS_EXTENSION_TYPE_HEARTBEAT:
      {
        if (ext_payload_len != 1)
          {
            *error = dsk_error_new ("missized Heartbeat extension");
            return NULL;
          }
        if (ext_payload[0] != 1 && ext_payload[0] != 2)
          {
            *error = dsk_error_new ("malformed Heartbeat extension");
            return NULL;
          }
        ext->heartbeat.mode = ext_payload[0];
        return ext;
      }
    case DSK_TLS_EXTENSION_TYPE_APPLICATION_LAYER_PROTOCOL_NEGOTIATION:
      {
        // RFC 7301, Section 3.1.
        //
        // The "extension_data" field of the
        // ("application_layer_protocol_negotiation(16)") extension SHALL
        // contain a "ProtocolNameList" value.
        //
        //        opaque ProtocolName<1..2^8-1>;
        //
        //        struct {
        //            ProtocolName protocol_name_list<2..2^16-1>
        //        } ProtocolNameList;

        // NOTE: this extension-type was invented for HTTP 2.
        if (ext_payload_len < 4)
          {
            *error = dsk_error_new ("too short ALPN extension");
            return NULL;
          }
        unsigned pnl_size = dsk_uint16be_parse (ext_payload);
        const uint8_t *at = ext_payload + 2;
        const uint8_t *end = at + pnl_size;
        unsigned n = 0;
        while (at < end)
          {
            if (at + (unsigned)*at + 1 > end)
              {
                *error = dsk_error_new ("malformed ALPN extension");
                return NULL;
              }
            at += (unsigned)*at + 1;
            n++;
          }
        at = ext_payload + 2;
        DskTlsExtension_ApplicationLayerProtocolNegotiation *alpn = &ext->alpn;
        alpn->n_protocols = n;
        alpn->protocols = dsk_mem_pool_alloc (pool, n * sizeof (DskTlsExtension_ProtocolName));
        for (unsigned i = 0; i < n; i++)
          {
            unsigned name_len = *at++;
            alpn->protocols[i].length = name_len;
            char *name = dsk_mem_pool_alloc_unaligned (pool, name_len + 1);
            memcpy (name, at, name_len);
            name[name_len] = 0;
            alpn->protocols[i].name = name;
          }
        break;
      }
    //TODO:case DSK_TLS_EXTENSION_TYPE_SIGNED_CERTIFICATE_TIMESTAMP:
    //TODO:case DSK_TLS_EXTENSION_TYPE_CLIENT_CERTIFICATE_TYPE:
    //TODO:case DSK_TLS_EXTENSION_TYPE_SERVER_CERTIFICATE_TYPE:
    //TODO:case DSK_TLS_EXTENSION_TYPE_PADDING:


     // From RFC 8446 4.2.11:
     //
     //  struct {
     //      opaque identity<1..2^16-1>;
     //      uint32 obfuscated_ticket_age;
     //  } PskIdentity;
     //
     //  opaque PskBinderEntry<32..255>;
     //
     //  struct {
     //      PskIdentity identities<7..2^16-1>;
     //      PskBinderEntry binders<33..2^16-1>;
     //  } OfferedPsks;
     //
     //  struct {
     //      select (Handshake.msg_type) {
     //          case client_hello: OfferedPsks;
     //          case server_hello: uint16 selected_identity;
     //      };
     //  } PreSharedKeyExtension;

    case DSK_TLS_EXTENSION_TYPE_PRE_SHARED_KEY:
      if (under_construction->type == DSK_TLS_HANDSHAKE_TYPE_CLIENT_HELLO)
        {
          unsigned n_identities = 0, n_binders = 0;
          const uint8_t *at = ext_payload;
          const uint8_t *end = at + ext_payload_len;
          const uint8_t *tmp_end = at + dsk_uint16be_parse (at) + 2;
          if (tmp_end > end - 2)
            {
              *error = dsk_error_new ("malformed PreSharedKey extension");
              return NULL;
            }
          // Scan identities.
          while (at < tmp_end)
            {
              if (at + 2 > tmp_end)
                {
                  *error = dsk_error_new ("malformed PreSharedKey extension");
                  return NULL;
                }
              unsigned identity_len = dsk_uint16be_parse (at);
              if (at + 2 + identity_len + 4 > tmp_end)
                {
                  *error = dsk_error_new ("malformed PreSharedKey extension");
                  return NULL;
                }
              at += 2 + identity_len + 4;
            }
          assert(at == tmp_end);
          assert(at + 2 <= end);
          tmp_end = at + dsk_uint16be_parse (at) + 2;
          at += 2;

          // scan bindings
          while (at < tmp_end)
            {
              if (at + *at + 1 > tmp_end)
                {
                  *error = dsk_error_new ("malformed PreSharedKey extension");
                  return NULL;
                }
              at += *at + 1;
              n_binders++;
            }

          // Allocate and parse identities.
          DskTlsExtension_OfferedPresharedKeys *offered = &ext->pre_shared_key.offered_psks;
          offered->n_identities = n_identities;
          at = ext_payload;
          tmp_end = at + dsk_uint16be_parse (at) + 2;
          at += 2;
          offered->identities = dsk_mem_pool_alloc(pool, n_identities*sizeof(DskTlsExtension_PresharedKeyIdentity));
          for (unsigned i = 0; i < n_identities; i++)
            {
              unsigned ilen = dsk_uint16be_parse (at);
              at += 2;
              offered->identities[i].identity_length = ilen;
              offered->identities[i].identity = at;
              at += ilen;
              offered->identities[i].obfuscated_ticket_age = dsk_uint32be_parse (at);
              at += 4;
            }
          offered->n_binder_entries = n_binders;
          offered->binder_entries = dsk_mem_pool_alloc (pool, n_binders * sizeof(DskTlsExtension_PresharedKeyBinderEntry));
          at += 2;
          for (unsigned i = 0; i < n_binders; i++)
            {
              unsigned blen = *at++;
              offered->binder_entries[i].length = blen;
              offered->binder_entries[i].data = at;
              at += blen;
            }
        }
      else if (under_construction->type == DSK_TLS_HANDSHAKE_TYPE_SERVER_HELLO)
        {
          if (ext_payload_len != 2)
            {
              *error = dsk_error_new ("PreSharedKey extension for server-hello should be a single uint16");
              return NULL;
            }
          ext->pre_shared_key.selected_identity = dsk_uint16be_parse (ext_payload);
        }
      else
        {
          *error = dsk_error_new ("PreSharedKey extension found with unsupported handshake");
          return NULL;
        }

    TODO:case DSK_TLS_EXTENSION_TYPE_EARLY_DATA:
    case DSK_TLS_EXTENSION_TYPE_SUPPORTED_VERSIONS:
      if (ext_payload_len < 3)
        {
          *error = dsk_error_new ("too short in SupportedVersions extension");
          return NULL;
        }
      if (ext_payload_len != 1 + ext_payload[0])
        {
          *error = dsk_error_new ("malformed SupportedVersions extension");
          return NULL;
        }
      assert(ext_payload_len == 1 + ext_payload[0]);
      unsigned n_versions = (ext_payload_len - 1) / 2;
      ext->supported_versions.n_supported_versions = n_versions;
      ext->supported_versions.supported_versions = dsk_mem_pool_alloc (pool, sizeof(DskTlsProtocolVersion) * n_versions);
      for (unsigned i = 0; i < n_versions; i++)
	ext->supported_versions.supported_versions[i] = dsk_uint16be_parse (ext_payload + 1 + 2 * i);
      break;

    //TODO:case DSK_TLS_EXTENSION_TYPE_COOKIE:
    //TODO:case DSK_TLS_EXTENSION_TYPE_PSK_KEY_EXCHANGE_MODES:
    //TODO:case DSK_TLS_EXTENSION_TYPE_CERTIFICATE_AUTHORITIES:
    //TODO:case DSK_TLS_EXTENSION_TYPE_OID_FILTERS:
    //TODO:case DSK_TLS_EXTENSION_TYPE_POST_HANDSHAKE_AUTH:
    //TODO:case DSK_TLS_EXTENSION_TYPE_SIGNATURE_ALGORITHMS_CERT:
    case DSK_TLS_EXTENSION_TYPE_KEY_SHARE:
      if (under_construction->type == DSK_TLS_HANDSHAKE_TYPE_CLIENT_HELLO)
        {
          if (ext_payload_len < 4)
            {
              *error = dsk_error_new ("too short in KeyShare extension");
              return NULL;
            }
          unsigned client_shares_size = dsk_uint16be_parse (ext_payload);
          unsigned n_client_shares = 0;
          const uint8_t *at = ext_payload + 2;
          const uint8_t *end = at + client_shares_size;
          while (at < end)
            {
              if (at + 4 > end)
                {
                  *error = dsk_error_new ("too short in KeyShare extension, key[%u]", n_client_shares);
                  return NULL;
                }
              unsigned key_exchange_size = dsk_uint16be_parse (at + 2);
              at += 4;
              if (at + key_exchange_size > end)
                {
                  *error = dsk_error_new ("too short in KeyShare extension, key[%u] data", n_client_shares);
                  return NULL;
                }
              at += key_exchange_size;
              n_client_shares++;
            }
          ext->key_share.n_key_shares = n_client_shares;
          unsigned sizeof_shares = sizeof(DskTlsKeyShareEntry) * n_client_shares;
          DskTlsKeyShareEntry *shares = dsk_mem_pool_alloc (pool, sizeof_shares);
          ext->key_share.key_shares = shares;
	  at = ext_payload + 2;			// reset at
          for (unsigned i = 0; i < n_client_shares; i++)
            {
              shares[i].named_group = dsk_uint16be_parse (at);
              unsigned len = dsk_uint16be_parse (at + 2);
              shares[i].key_exchange_length = len;
              shares[i].key_exchange_data = at + 4;
              at += 4 + len;
            }
        }
      else if (dsk_tls_handshake_is_hello_retry_request (under_construction))
        {
          if (ext_payload_len != 2)
            {
              *error = dsk_error_new ("bad length in KeyShare extension, HelloRetryRequest");
              return NULL;
            }
          ext->key_share.selected_group = dsk_uint16be_parse (ext_payload);
        }
      else if (under_construction->type == DSK_TLS_HANDSHAKE_TYPE_SERVER_HELLO)
        {
          if (ext_payload_len < 4)
            {
              *error = dsk_error_new ("too short in KeyShare extension, ServerHello");
              return NULL;
            }
          ext->key_share.server_share.named_group = dsk_uint16be_parse (ext_payload);
          ext->key_share.server_share.key_exchange_length = dsk_uint16be_parse (ext_payload + 2);
          ext->key_share.server_share.key_exchange_data = ext_payload + 4;
          if (ext_payload_len < 4 + ext->key_share.server_share.key_exchange_length)
            {
              *error = dsk_error_new ("too short in KeyShare extension data, ServerHello");
              return NULL;
            }
        }
      else
        assert(false);
      break;
    default:
      ext->generic.is_generic = true;
      break;
    }
  return ext;
}

static bool
parse_length_prefixed_extensions (DskTlsHandshake *under_construction,
                                  const uint8_t **at_inout,
                                  const uint8_t *end,
                                  unsigned *n_extensions_out,
                                  DskTlsExtension ***extensions_out,
                                  DskMemPool *pool,
                                  DskError **error)
{
  if (*at_inout + 2 > end)
    {
      *error = dsk_error_new ("extension size truncated");
      return false;
    }
  unsigned extensions_size = dsk_uint16be_parse (*at_inout);
  if (*at_inout + extensions_size > end)
    {
      *error = dsk_error_new ("Extensions overran message");
      return false;
    }
  *at_inout += 2;
  unsigned n_extensions;
  if (!count_extensions (*at_inout,
                         *at_inout + extensions_size,
                         &n_extensions))
    {
      *error = dsk_error_new ("error scanning extensions in ");
      return false;
    }
  *n_extensions_out = n_extensions;
  *extensions_out = dsk_mem_pool_alloc (pool, sizeof (DskTlsExtension*) * n_extensions);
  fprintf(stderr, "n_extensions=%u; atinout=%p end=%p\n",n_extensions,*at_inout,end);
  for (unsigned i = 0; i < n_extensions; i++)
    {
      fprintf(stderr, "ext[%u]\n",i);
      assert (*at_inout + 4 <= end);
      unsigned ext_type = dsk_uint16be_parse (*at_inout);
      unsigned ext_payload_len = dsk_uint16be_parse (*at_inout + 2);
      fprintf(stderr, "extension[%i]: type=%x [%s]: payload_len=%u\n", i, ext_type, dsk_tls_extension_type_name(ext_type), ext_payload_len);
      *at_inout += 4;
      assert (*at_inout + ext_payload_len <= end);
      DskTlsExtension *ext = parse_extension (under_construction, ext_type, ext_payload_len, *at_inout, pool, error);
      if (ext == NULL)
        {
          fprintf(stderr, "extension parsing failed: %s\n", (*error)->message);
          return false;
        }
      (*extensions_out)[i] = ext;
      *at_inout += ext_payload_len;
    }
  return true;
}

DskTlsHandshake *dsk_tls_handshake_parse  (DskTlsHandshakeType type,
                                           unsigned            len,
                                           const uint8_t      *data,
                                           DskMemPool         *pool,
                                           DskError          **error)
{
  DskTlsHandshake *rv;
  if (len < 1)
    {
      *error = dsk_error_new ("too short- 0 length handshake");
      return NULL;
    }
  rv = dsk_mem_pool_alloc (pool, sizeof (DskTlsHandshake));
  rv->type = type;
  rv->data_length = len;
  rv->data = data;

  const uint8_t *at = data;
  const uint8_t *end = data + len;

  switch (rv->type)
  {
  case DSK_TLS_HANDSHAKE_TYPE_CLIENT_HELLO:
    {
      if (at + 35 > end)
        {
          *error = dsk_error_new ("too little data for ClientHello message");
          return NULL;
        }

      //
      // Read legacy version.  This field is set to 0x03 0x03, but
      // the supported_versions extension should be used instead of version negotiation in
      // TLS 1.3.
      //
      rv->client_hello.legacy_version = dsk_uint16be_parse (at);
      at += 2;

      //
      // Read random data.
      //
      memcpy (rv->client_hello.random, at, 32);
      at += 32;

      //
      // Legacy session id.  Replaced by pre-shared-keys.
      //
      unsigned legacy_session_id_length = *at++;
      rv->client_hello.legacy_session_id_length = legacy_session_id_length;
      if (legacy_session_id_length > 32 || at + legacy_session_id_length > end)
        {
          *error = dsk_error_new ("too little data for ClientHello message (truncated legacy-session-id)");
          return NULL;
        }
      memcpy (rv->client_hello.legacy_session_id, at,
              legacy_session_id_length);
      at += legacy_session_id_length;

      //
      // Cipher Suites
      //
      if (at + 2 > end)
        {
          *error = dsk_error_new ("too little data for ClientHello message (truncated cipher-suite length)");
          return NULL;
        }
      unsigned cipher_suites_size = dsk_uint16be_parse (at);
      at += 2;
      if (cipher_suites_size % 2 != 0)
        {
          *error = dsk_error_new ("cipher-suite array length was odd");
          return NULL;
        }
      if (at + cipher_suites_size > end)
        {
          *error = dsk_error_new ("too little data for ClientHello message (truncated cipher-suite array)");
          return NULL;
        }
      unsigned n_cipher_suites = cipher_suites_size / 2;
      rv->client_hello.n_cipher_suites = n_cipher_suites;
      rv->client_hello.cipher_suites = dsk_mem_pool_alloc (pool, n_cipher_suites * sizeof(DskTlsCipherSuite));
      for (unsigned i = 0; i < n_cipher_suites; i++)
        {
          rv->client_hello.cipher_suites[i] = dsk_uint16be_parse (at);
          at += 2;
        }

      //
      // Legacy compression methods.  Client must offer "null" (0) to mean
      // no compression and no compression must be used.
      //
      uint8_t n_compression_methods;
      if (at == end)
        {
          *error = dsk_error_new ("too little data for ClientHello message (truncated n-compression-methods)");
          return false;
        }
      n_compression_methods = *at++;
      if (at + n_compression_methods + 2 > end)
        {
          *error = dsk_error_new ("too little data for ClientHello message (truncated compression-methods)");
          return false;
        }
      rv->client_hello.n_compression_methods = n_compression_methods;
      rv->client_hello.compression_methods = dsk_mem_pool_alloc_unaligned (pool, n_compression_methods);
      memcpy (rv->client_hello.compression_methods, at, n_compression_methods);
      at += n_compression_methods;

      //
      // Parse Extensions.
      // 
      if (!parse_length_prefixed_extensions (rv,
                                             &at, end,
                                             &rv->client_hello.n_extensions,
                                             &rv->client_hello.extensions,
                                             pool,
                                             error))
        return false;
      break;
    }
  case DSK_TLS_HANDSHAKE_TYPE_SERVER_HELLO:
    {
      if (at + 35 > end)
        {
          *error = dsk_error_new ("too little data for ServerHello message");
          return false;
        }

      //
      // Read legacy version.  This field is set to 0x03 0x03, but
      // the supported_versions extension should be used instead of version negotiation in
      // TLS 1.3.
      //
      rv->server_hello.legacy_version = dsk_uint16be_parse (at);
      at += 2;

      //
      // Read random data.
      //
      memcpy (rv->server_hello.random, at, 32);
      at += 32;

      //
      // Magic value for retry-request. (See RFC 8446 Section 4.1.3)
      //
      rv->server_hello.is_retry_request = memcmp (rv->server_hello.random,
                                            "\xCF\x21\xAD\x74\xE5\x9A\x61\x11"
                                            "\xBE\x1D\x8C\x02\x1E\x65\xB8\x91"
                                            "\xC2\xA2\x11\x16\x7A\xBB\x8C\x5E"
                                            "\x07\x9E\x09\xE2\xC8\xA8\x33\x9C",
                                                  32) == 0;

      //
      // Legacy session id.  Replaced by pre-shared-keys.
      //
      unsigned legacy_session_id_length = *at++;
      rv->server_hello.legacy_session_id_echo_length = legacy_session_id_length;
      if (legacy_session_id_length > 32 || at + legacy_session_id_length > end)
        {
          *error = dsk_error_new ("too little data for ServerHello message (truncated legacy-session-id)");
          return false;
        }
      memcpy (rv->server_hello.legacy_session_id_echo, at,
              legacy_session_id_length);
      at += legacy_session_id_length;

      //
      // Cipher Suite and Compression Method
      //
      if (at + 3 > end)
        {
          *error = dsk_error_new ("too little data for ServerHello message (truncated cipher-suite length)");
          return false;
        }
      rv->server_hello.cipher_suite = dsk_uint16be_parse (at);
      rv->server_hello.compression_method = dsk_uint16be_parse (at);

      //
      // Parse Extensions.
      // 
      if (!parse_length_prefixed_extensions (rv,
                                             &at, end,
                                             &rv->server_hello.n_extensions,
                                             &rv->server_hello.extensions,
                                             pool,
                                             error))
        return false;
      break;
    }
  case DSK_TLS_HANDSHAKE_TYPE_NEW_SESSION_TICKET:
  case DSK_TLS_HANDSHAKE_TYPE_END_OF_EARLY_DATA:
  case DSK_TLS_HANDSHAKE_TYPE_ENCRYPTED_EXTENSIONS:
  case DSK_TLS_HANDSHAKE_TYPE_CERTIFICATE:
  case DSK_TLS_HANDSHAKE_TYPE_CERTIFICATE_REQUEST:
  case DSK_TLS_HANDSHAKE_TYPE_CERTIFICATE_VERIFY:
  case DSK_TLS_HANDSHAKE_TYPE_FINISHED:
  case DSK_TLS_HANDSHAKE_TYPE_KEY_UPDATE:
  case DSK_TLS_HANDSHAKE_TYPE_MESSAGE_HASH:
  default:
    *error = dsk_error_new ("bad handshake packet: 0x%02x", rv->type);
    return false;
  }
  return rv;
}

