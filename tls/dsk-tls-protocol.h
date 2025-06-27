
/* Implementation of TLS 1.3 RFC 8446. */

//typedef struct DskTlsCertificate DskTlsCertificate;

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
  DSK_TLS_SERVER_NAME_TYPE_HOSTNAME = 0
} DskTlsServerNameType;
typedef struct DskTlsServerNameListEntry DskTlsServerNameListEntry;
struct DskTlsServerNameListEntry
{
  DskTlsServerNameType type;
  unsigned name_length;
  const char *name;             /* NUL terminated. */
};
typedef struct DskTlsExtension_ServerNameList DskTlsExtension_ServerNameList;
struct DskTlsExtension_ServerNameList
{
  DskTlsExtensionBase base;

  // This will always be 0 for ServerHello messages.
  // In that case, it merely means that the server used the ServerName extension's value.
  unsigned n_entries;
  DskTlsServerNameListEntry *entries;
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
#if 0
struct DskTlsCertificate
{
  DskTlsCertificateType type;
  union {
    DskTlsX509SubjectPublicKeyInfo raw_public_key;
    struct {
      size_t chain_length;
      DskTlsX509Certificate **chain;
    } x509;
  };

  size_t cert_data_length;
  const uint8_t *cert_data;
};
#endif

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
  const DskTlsSignatureScheme *schemes;
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
typedef struct DskTlsExtension_ALPN DskTlsExtension_ALPN;
struct DskTlsExtension_ALPN
{
  DskTlsExtensionBase base;
  uint16_t n_protocols;
  const char **protocols;
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

  // for HelloRetryRequest message
  DskTlsNamedGroup selected_group;

  // for ServerHello message
  DskTlsKeyShareEntry server_share;
};

/* --- Pre Shared Key Extension (RFC 8446, 4.2.11) --- */
typedef struct DskTlsPresharedKeyIdentity DskTlsPresharedKeyIdentity;
struct DskTlsPresharedKeyIdentity
{
  unsigned identity_length;
  const uint8_t *identity;
  uint32_t obfuscated_ticket_age;
  uint8_t binder_length;
  const uint8_t *binder_data;
};
typedef struct DskTlsOfferedPresharedKeys DskTlsOfferedPresharedKeys;
struct DskTlsOfferedPresharedKeys
{
  unsigned n_identities;
  DskTlsPresharedKeyIdentity *identities;
};
typedef struct DskTlsExtension_PreSharedKey DskTlsExtension_PreSharedKey;
struct DskTlsExtension_PreSharedKey
{
  DskTlsExtensionBase base;

  /* for ClientHello */
  DskTlsOfferedPresharedKeys offered_psks;

  /* for ServerHello */
  unsigned selected_identity;
};

/* Pre-Shared-Key Exchange Modes (RFC 8446, 4.???) */
typedef enum {
  // Use the PSK without KeyShare (less roundtrips)
  DSK_TLS_PSK_EXCHANGE_MODE_KE = 0,

  // Use the PSK with KeyShare (more secure)
  DSK_TLS_PSK_EXCHANGE_MODE_DHE_KE = 1,
} DskTlsPSKKeyExchangeMode;

typedef struct DskTlsExtension_PSKKeyExchangeModes DskTlsExtension_PSKKeyExchangeModes;
struct DskTlsExtension_PSKKeyExchangeModes
{
  DskTlsExtensionBase base;
  unsigned n_modes;
  DskTlsPSKKeyExchangeMode *modes;
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
typedef struct DskTlsExtension_CertificateAuthorities DskTlsExtension_CertificateAuthorities;
struct DskTlsExtension_CertificateAuthorities
{
  DskTlsExtensionBase base;
  unsigned n_CAs;
  DskTlsX509DistinguishedName *CAs;
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

/* --- SignatureAlgorithmCert --- */
typedef struct DskTlsExtension_SignatureAlgorithmsCert DskTlsExtension_SignatureAlgorithmsCert;
struct DskTlsExtension_SignatureAlgorithmsCert
{
  DskTlsExtensionBase base;
  uint16_t n_schemes;
  DskTlsSignatureScheme *schemes;
};

typedef struct DskTlsExtension_CertificateType DskTlsExtension_CertificateType;
struct DskTlsExtension_CertificateType
{
  DskTlsExtensionBase base;

  uint16_t n_cert_types;
  DskTlsCertificateType *cert_types;
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
  DskTlsExtension_ALPN alpn;
  DskTlsExtension_PreSharedKey pre_shared_key;
  DskTlsExtension_CertificateType client_certificate_type;
  DskTlsExtension_CertificateType server_certificate_type;
  DskTlsExtension_Padding padding;
  DskTlsExtension_KeyShare key_share;
  DskTlsExtension_EarlyDataIndication early_data_indication;
  DskTlsExtension_Cookie cookie;
  DskTlsExtension_SignatureAlgorithmsCert signature_algorithms_cert;
} DskTlsExtension;

typedef struct {
  unsigned cert_data_length;
  const uint8_t *cert_data;
  DskTlsX509Certificate *cert;
  unsigned n_extensions;
  DskTlsExtension **extensions;
} DskTlsCertificateEntry;


//
// This structure is designed to map to
// the data format RFC 8446 calls "Handshake".
//
// But we use DskTlsHandshake to refer to the
// whole sequence of message-flights,
// and call the RFC8446/Handshake HandshakeMessage,
// which seems more accurate.
// 
typedef struct DskTlsHandshakeMessage DskTlsHandshakeMessage;
struct DskTlsHandshakeMessage
{
  DskTlsHandshakeMessageType type;
  unsigned data_length;
  const uint8_t *data;
  DskTlsHandshakeMessage *next;
  bool is_outgoing;   /* are we the sender or recipient of this message? */

  unsigned n_extensions;
  DskTlsExtension **extensions;
  unsigned max_extensions;

  union {
    struct {
      uint16_t legacy_version;
      uint8_t random[32];
      unsigned legacy_session_id_length;
      uint8_t legacy_session_id[32];
      unsigned n_cipher_suites;
      DskTlsCipherSuiteCode *cipher_suites;
      unsigned n_compression_methods;   /* legacy */
      uint8_t *compression_methods;     /* legacy */
    } client_hello;
    struct {
      uint16_t legacy_version;
      uint8_t random[32];
      unsigned legacy_session_id_echo_length;
      uint8_t *legacy_session_id_echo;
      DskTlsCipherSuiteCode cipher_suite;
      uint8_t compression_method;       /* must be 0 for tls 1.3 */
      bool is_retry_request;
    } server_hello, hello_retry_request;
    struct {
      unsigned n_extensions;
      DskTlsExtension **extensions;
    } encrypted_extensions;
    struct {
      uint32_t ticket_lifetime;
      uint32_t ticket_age_add;
      uint32_t ticket_nonce_length;
      const uint8_t *ticket_nonce;
      unsigned ticket_length;
      const uint8_t *ticket_data;
    } new_session_ticket;
    struct {
      unsigned certificate_request_context_length;
      const uint8_t *certificate_request_context;
    } certificate_request;
    struct {
      // Should be empty in handshake; only for post-handshake auth.
      unsigned certificate_request_context_len;
      const uint8_t *certificate_request_context;

      unsigned n_entries;
      DskTlsCertificateEntry *entries;
    } certificate;
    struct {
      DskTlsSignatureScheme algorithm;
      unsigned signature_length;
      const uint8_t *signature_data;
    } certificate_verify;
    struct {
      unsigned verify_data_length;
      const uint8_t *verify_data;
    } finished;
    struct {
      bool update_requested;
    } key_update;
  };
};

// type==SERVER_HELLO && !is_retry_request
DSK_INLINE bool dsk_tls_handshake_is_server_hello (DskTlsHandshakeMessage *handshake);

// type==SERVER_HELLO && is_retry_request
//
// However, in the future,
// this may return true if a new HandshakeType==HELLO_RETRY_REQUEST
// (which is reserved) is created.
DSK_INLINE bool dsk_tls_handshake_is_hello_retry_request (DskTlsHandshakeMessage *handshake);


/* --- Parsing --- */

// NOTE: may substructures returned by this function point directly
// into 'data'.  You can track that separately, or you can allocate 'data'
// from 'pool', which is what we do.
//
// In order to do this you'll have to pre-frame the handshake,
// which is why we take these inputs.
DskTlsHandshakeMessage *dsk_tls_handshake_message_parse  (DskTlsHandshakeMessageType type,
                                           unsigned            len,
                                           const uint8_t      *data,
                                           DskMemPool         *pool,
                                           DskError          **error);

void dsk_tls_handshake_message_add_extension(DskTlsHandshakeMessage *msg, DskTlsExtension *ext);
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
void dsk_tls_handshake_to_buffer (const DskTlsHandshakeMessage *handshake,
                                  DskBuffer *out);


DSK_INLINE bool dsk_tls_handshake_is_server_hello (DskTlsHandshakeMessage *handshake)
{
  return handshake->type == DSK_TLS_HANDSHAKE_MESSAGE_TYPE_SERVER_HELLO
      && !handshake->server_hello.is_retry_request;
}

DSK_INLINE bool dsk_tls_handshake_is_hello_retry_request (DskTlsHandshakeMessage *handshake)
{
  return handshake->type == DSK_TLS_HANDSHAKE_MESSAGE_TYPE_SERVER_HELLO
      && handshake->server_hello.is_retry_request;
}

// RFC 8446, 7.1 Key Schedule gives this function as Derive-Secret.
void
dsk_tls_derive_secret (DskChecksumType*type,
                       const uint8_t  *secret,
                       size_t          label_len,
                       const uint8_t  *label,                   // an ascii string
                       const uint8_t  *transcript_hash,
                       uint8_t        *out);

// RFC 8446, 7.1 Key Schedule gives this function as HKDF-Expand-Label.
//
// NOTE: in most cases, context/context_len is a transcript_hash,
//       but in a few cases in TLS context is the empty string.
//
//       So, we need to give the length even though it is usually
//       determined by the hash-type.

void
dsk_tls_hkdf_expand_label(DskChecksumType*type,
                          const uint8_t  *secret,
                          size_t          label_length,
                          const uint8_t  *label,                // an ascii string
                          size_t          context_length,
                          const uint8_t  *context,
                          size_t          output_length,
                          uint8_t        *out);

// Update the application-secret,
// in response to a KeyUpdate message.
void
dsk_tls_update_key_inplace (DskChecksumType *checksum_type,
                            uint8_t         *secret_inout);

// Compute verify_data for the Finished message.
//
// Compute verify_data using BaseKey and a TranscriptHash.
// This uses hkdf_expand_label and hmac.
//
// This function implements most of the cryptographic data
// in RFC 8446, Section 4.4.4,
// but it is also reusable to compute binder_key as per Section 4.2.11.2.
void
dsk_tls_finished_get_verify_data (const uint8_t *base_key,
                                  const uint8_t *transcript_hash,
                                  uint8_t       *verify_data_out);


void
dsk_tls_checksum_binder_truncated_handshake_message
                                 (const DskChecksumType *csum_type,
                                  void *csum_instance,
                                  DskTlsHandshakeMessage *client_hello);

