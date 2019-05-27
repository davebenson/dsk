
/* Implementation of TLS 1.3 RFC 8446. */

typedef struct DskTlsCertificate DskTlsCertificate;

typedef struct DskTlsExtensionBase DskTlsExtensionBase;
struct DskTlsExtensionBase
{
  DskTlsExtensionType type;
  size_t extension_data_length;
  const uint8_t *extension_data;
  bool is_generic;        // if true, no pre-parsed data is available.
};


/* RFC 6066. Section 3 */
typedef enum
{
  DSK_TLS_EXTENSION_SERVER_NAME_TYPE_HOSTNAME = 0
} DskTlsExtensionServerNameType;
typedef struct DskTlsExtension_ServerNameList_Entry DskTlsExtension_ServerNameList_Entry;
struct DskTlsExtension_ServerNameList_Entry
{
  DskTlsExtensionServerNameType type;
  unsigned name_length;
  const char *name;
};
typedef struct DskTlsExtension_ServerNameList DskTlsExtension_ServerNameList;
struct DskTlsExtension_ServerNameList
{
  DskTlsExtensionBase base;
  unsigned n_entries;
  DskTlsExtension_ServerNameList_Entry *entries;
};

/* RFC 6066. Section 4 */
typedef struct DskTlsExtension_MaxFragmentLength DskTlsExtension_MaxFragmentLength;
struct DskTlsExtension_MaxFragmentLength
{
  DskTlsExtensionBase base;
  unsigned log2_length;      /* must be 9, 10, 11, 12 */
};


/* Certificate, signed or unsigned depending on the context.
 */
struct DskTlsCertificate
{
  DskTlsCertificateType type;
  union {
    DskTlsX509SubjectPublicKeyInfo raw_public_key;
    DskTlsX509Certificate x509;
  };
};

/* RFC 6066. Section 8 */
//typedef struct
//{
//  unsigned n_responder_ids;
//  uint16_t *responder_ids;
//  uint16_t extensions;   /*  DER encoding of OCSP request extensions */
//} DskTlsExtension_OCSPStatusRequest;
//#define DSK_TLS_EXTENSION_STATUS_REQUEST_STATUS_TYPE_OCSP   1
//struct DskTlsExtension_StatusRequest
//{
//  DskTlsExtensionBase base;
//  uint8_t status_type;
//  union {
//    DskTlsExtension_OCSPStatusRequest ocsp;
//  };
//};

/* RFC 7919.  Named Elliptic Curves "Supported Group Extension" */
typedef struct DskTlsExtension_SupportedGroups DskTlsExtension_SupportedGroups;
struct DskTlsExtension_SupportedGroups
{
  DskTlsExtensionBase base;
  unsigned n_supported_groups;
  DskTlsNamedGroup *supported_groups;
};

const char *dsk_tls_signature_scheme_name (DskTlsSignatureScheme scheme);

typedef struct DskTlsExtension_SignatureAlgorithms DskTlsExtension_SignatureAlgorithms;
struct DskTlsExtension_SignatureAlgorithms
{
  DskTlsExtensionBase base;
  uint16_t n_schemes;
  DskTlsSignatureScheme *schemes;
};

/* --- Heartbeat Mode RFC 6520 --- */
typedef enum
{
  DSK_TLS_EXTENSION_HEARTBEAT_MODE_ALLOWED      = 1,
  DSK_TLS_EXTENSION_HEARTBEAT_MODE_NOT_ALLOWED  = 2,
} DskTlsExtension_HeartbeatMode;
typedef struct DskTlsExtension_Heartbeat DskTlsExtension_Heartbeat;
struct DskTlsExtension_Heartbeat
{
  DskTlsExtensionBase base;
  DskTlsExtension_HeartbeatMode mode;
};


/* --- Application Layer Protocol Negotiation RFC 7301 --- */
typedef struct DskTlsExtension_ProtocolName DskTlsExtension_ProtocolName;
struct DskTlsExtension_ProtocolName
{
  uint8_t length;
  char *name;
};
typedef struct DskTlsExtension_ApplicationLayerProtocolNegotiation DskTlsExtension_ApplicationLayerProtocolNegotiation;
struct DskTlsExtension_ApplicationLayerProtocolNegotiation
{
  DskTlsExtensionBase base;
  uint16_t n_protocols;
  DskTlsExtension_ProtocolName *protocols;
};

/* NOTE: client_certificate_type and server_certificate_type
   and defined in RFC7250 and not implemented. */

/* --- Padding Extension RFC 7685 --- */
typedef struct DskTlsExtension_Padding DskTlsExtension_Padding;
struct DskTlsExtension_Padding
{
  DskTlsExtensionBase base;
};


/* --- Key Share extension (RFC 8446) --- */
typedef struct DskTlsKeyShareEntry DskTlsKeyShareEntry;
struct DskTlsKeyShareEntry
{
  DskTlsNamedGroup named_group;  // DSK_TLS_EXTENSION_NAMED_GROUP_*
  unsigned key_exchange_length;
  const uint8_t *key_exchange_data;
};


typedef struct DskTlsExtension_KeyShare DskTlsExtension_KeyShare;
struct DskTlsExtension_KeyShare
{
  DskTlsExtensionBase base;

  // for ClientHello message
  unsigned n_key_shares;
  DskTlsKeyShareEntry *key_shares;
  unsigned n_supported_groups;
  DskTlsNamedGroup *supported_groups;

  // for HelloRetryRequest message
  DskTlsNamedGroup selected_group;

  // for ServerHello message
  DskTlsKeyShareEntry server_share;
};

/* --- Pre Shared Key Extension (RFC 8446, 4.2.11) --- */
typedef struct DskTlsExtension_PresharedKeyIdentity DskTlsExtension_PresharedKeyIdentity;
struct DskTlsExtension_PresharedKeyIdentity
{
  unsigned identity_length;
  const uint8_t *identity;
  uint32_t obfuscated_ticket_age;
};
typedef struct DskTlsExtension_PresharedKeyBinderEntry DskTlsExtension_PresharedKeyBinderEntry;
struct DskTlsExtension_PresharedKeyBinderEntry
{
  uint8_t length;
  const uint8_t *data;
};
typedef struct DskTlsExtension_OfferedPresharedKeys DskTlsExtension_OfferedPresharedKeys;
struct DskTlsExtension_OfferedPresharedKeys
{
  unsigned n_identities;
  DskTlsExtension_PresharedKeyIdentity *identities;
  unsigned n_binder_entries;
  DskTlsExtension_PresharedKeyBinderEntry *binder_entries;
};
typedef struct DskTlsExtension_PreSharedKey DskTlsExtension_PreSharedKey;
struct DskTlsExtension_PreSharedKey
{
  DskTlsExtensionBase base;

  /* for ClientHello */
  DskTlsExtension_OfferedPresharedKeys offered_psks;

  /* for ServerHello */
  unsigned selected_identity;
};

/* Pre Shared Key Exchange Modes (RFC 8446, 4.???) */
typedef enum {
  DSK_TLS_EXTENSION_PSK_EXCHANGE_MODE_KE = 0,
  DSK_TLS_EXTENSION_PSK_EXCHANGE_MODE_DHE_KE = 1,
} DskTlsExtension_PSKKeyExchangeMode;
typedef struct DskTlsExtension_PSKKeyExchangeModes DskTlsExtension_PSKKeyExchangeModes;
struct DskTlsExtension_PSKKeyExchangeModes
{
  DskTlsExtensionBase base;
  unsigned n_modes;
  DskTlsExtension_PSKKeyExchangeMode *modes;
};

/* --- Early Data Indication (RFC 8446, 4.???) --- */
typedef struct DskTlsExtension_EarlyDataIndication DskTlsExtension_EarlyDataIndication;
struct DskTlsExtension_EarlyDataIndication
{
  DskTlsExtensionBase base;

  // Only valid for new_session_ticket handshake-message-type;
  // 0 otherwise, just an indicator that early-data is wanted.
  uint32_t max_early_data_size;
};

typedef struct DskTlsExtension_Cookie DskTlsExtension_Cookie;
struct DskTlsExtension_Cookie
{
  DskTlsExtensionBase base;

  // The same as base.extension_data[_length].
  // (But easier to read)
  unsigned cookie_length;
  uint8_t *cookie;
};

/* --- Supported Versions Extension RFC 8446 4.2.1 --- */
typedef enum
{
  DSK_TLS_PROTOCOL_VERSION_1_2 = 0x303,
  DSK_TLS_PROTOCOL_VERSION_1_3 = 0x304,
} DskTlsProtocolVersion;

typedef struct DskTlsExtension_SupportedVersions DskTlsExtension_SupportedVersions;
struct DskTlsExtension_SupportedVersions
{
  DskTlsExtensionBase base;
  // for ClientHello
  unsigned n_supported_versions;
  DskTlsProtocolVersion *supported_versions;

  // for ServerHello
  DskTlsProtocolVersion selected_version;
};

/* --- Certificate Authorities Extension RFC 8446 4.2.4 --- */
typedef struct DskTlsExtension_CertificateAuthorityDistinguishedName DskTlsExtension_CertificateAuthorityDistinguishedName;
struct DskTlsExtension_CertificateAuthorityDistinguishedName
{
  unsigned length;
  char *name;
};
typedef struct DskTlsExtension_CertificateAuthorities DskTlsExtension_CertificateAuthorities;
struct DskTlsExtension_CertificateAuthorities
{
  DskTlsExtensionBase base;
  unsigned n_names;
  DskTlsExtension_CertificateAuthorityDistinguishedName *names;
};

/* --- OID Filters Extension RFC 8446 4.2.5 --- */
typedef struct DskTlsOIDFilter DskTlsOIDFilter;
struct DskTlsOIDFilter
{
  unsigned oid_length;
  uint8_t *oid;
  unsigned value_length;
  uint8_t *value;
};
typedef struct DskTlsExtension_OIDFilters DskTlsExtension_OIDFilters;
struct DskTlsExtension_OIDFilters
{
  DskTlsExtensionBase base;
  unsigned n_filters;
  DskTlsOIDFilter *filters;
};



typedef union {
  uint16_t type;
  DskTlsExtensionBase generic;
  DskTlsExtension_ServerNameList server_name;
  DskTlsExtension_MaxFragmentLength max_fragment_length;
  //DskTlsExtension_StatusRequest status_request;
  DskTlsExtension_SupportedGroups supported_groups;
  DskTlsExtension_SupportedVersions supported_versions;
  DskTlsExtension_SignatureAlgorithms signature_algorithms;
  DskTlsExtension_Heartbeat heartbeat;
  DskTlsExtension_ApplicationLayerProtocolNegotiation alpn;
  DskTlsExtension_PreSharedKey pre_shared_key;
  // TODO: client_certificate_type server_certificate_type
  DskTlsExtension_Padding padding;
  DskTlsExtension_KeyShare key_share;
  DskTlsExtension_EarlyDataIndication early_data_indication;
  DskTlsExtension_Cookie cookie;
} DskTlsExtension;

typedef struct DskTlsHandshake DskTlsHandshake;
struct DskTlsHandshake
{
  DskTlsHandshakeType type;
  unsigned data_length;
  const uint8_t *data;
  DskTlsHandshake *next;
  union {
    struct {
      uint16_t legacy_version;
      uint8_t random[32];
      unsigned legacy_session_id_length;
      uint8_t legacy_session_id[32];
      unsigned n_cipher_suites;
      DskTlsCipherSuite *cipher_suites;
      unsigned n_compression_methods;   /* legacy */
      uint8_t *compression_methods;     /* legacy */
      unsigned n_extensions;
      DskTlsExtension **extensions;
    } client_hello;
    struct {
      uint16_t legacy_version;
      uint8_t random[32];
      unsigned legacy_session_id_echo_length;
      uint8_t *legacy_session_id_echo;
      DskTlsCipherSuite cipher_suite;
      uint8_t compression_method;       /* must be 0 for tls 1.3 */
      unsigned n_extensions;
      DskTlsExtension **extensions;
      bool is_retry_request;
    } server_hello, hello_retry_request;
    struct {
      unsigned n_extensions;
      DskTlsExtension **extensions;
    } encrypted_extensions;
    struct {
      uint32_t ticket_lifetime;
      uint32_t ticket_age_add;
      uint32_t ticket_nonce;
      unsigned ticket_length;
      const uint8_t *ticket_data;
      unsigned n_extensions;
      DskTlsExtension **extensions;
    } new_session;
    struct {
      unsigned certificate_request_context_length;
      uint8_t *certificate_request_context;
      unsigned n_extensions;
      DskTlsExtension **extensions;
    } certificate_request;
    struct {
      // Should be empty in handshake; only for post-handshake auth.
      unsigned certificate_request_context_len;
      uint8_t *certificate_request_context;

      // Chain of certificates, each supporting the one before it.
      unsigned chain_length;
      DskTlsCertificate **certificate_chain;
    } certificate;
  };
};

// type==SERVER_HELLO && !is_retry_request
DSK_INLINE_FUNC bool dsk_tls_handshake_is_server_hello (DskTlsHandshake *handshake);

// type==SERVER_HELLO && is_retry_request
//
// However, in the future,
// this may return true if a new HandshakeType==HELLO_RETRY_REQUEST
// (which is reserved) is created.
DSK_INLINE_FUNC bool dsk_tls_handshake_is_hello_retry_request (DskTlsHandshake *handshake);


/* --- Parsing --- */

// NOTE: may substructures returned by this function point directly
// into 'data'.  You can track that separately, or you can allocate 'data'
// from 'pool', which is what we do.
//
// In order to do this you'll have to pre-frame the handshake,
// which is why we take these inputs.
DskTlsHandshake *dsk_tls_handshake_parse  (DskTlsHandshakeType type,
                                           unsigned            len,
                                           const uint8_t      *data,
                                           DskMemPool         *pool,
                                           DskError          **error);

typedef enum
{
  DSK_TLS_PARSE_RESULT_CODE_OK,
  DSK_TLS_PARSE_RESULT_CODE_ERROR,
  DSK_TLS_PARSE_RESULT_CODE_TOO_SHORT
} DskTlsParseResultCode;

typedef struct DskTlsRecordHeaderParseResult
{
  DskTlsParseResultCode code;
  union {
    struct {
      DskTlsRecordContentType content_type;
      unsigned header_length;           // always 5
      unsigned payload_length;
    } ok;
    DskError *error;
  };
} DskTlsRecordHeaderParseResult;
DskTlsRecordHeaderParseResult dsk_tls_parse_record_header (DskBuffer *buffer);


/* --- Serializing --- */
void dsk_tls_handshake_to_buffer (const DskTlsHandshake *handshake,
                                  DskBuffer *out);


DSK_INLINE_FUNC bool dsk_tls_handshake_is_server_hello (DskTlsHandshake *handshake)
{
  return handshake->type == DSK_TLS_HANDSHAKE_TYPE_SERVER_HELLO
      && !handshake->server_hello.is_retry_request;
}

DSK_INLINE_FUNC bool dsk_tls_handshake_is_hello_retry_request (DskTlsHandshake *handshake)
{
  return handshake->type == DSK_TLS_HANDSHAKE_TYPE_SERVER_HELLO
      && handshake->server_hello.is_retry_request;
}

// RFC 8446, 7.1 Key Schedule gives this function as Derive-Secret.
void
dsk_tls_derive_secret (DskChecksumType type,
                       const uint8_t  *secret,
                       size_t          label_len,
                       const uint8_t  *label,
                       const uint8_t  *transcript_hash,
                       uint8_t        *out);
