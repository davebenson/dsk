
/* Implementation of TLS 1.3 RFC 8446. */

typedef enum
{
  DSK_TLS_HANDSHAKE_TYPE_CLIENT_HELLO = 1,
  DSK_TLS_HANDSHAKE_TYPE_SERVER_HELLO = 2,
  DSK_TLS_HANDSHAKE_TYPE_NEW_SESSION_TICKET = 4,
  DSK_TLS_HANDSHAKE_TYPE_END_OF_EARLY_DATA = 5,
  DSK_TLS_HANDSHAKE_TYPE_ENCRYPTED_EXTENSIONS = 8,
  DSK_TLS_HANDSHAKE_TYPE_CERTIFICATE = 11,
  DSK_TLS_HANDSHAKE_TYPE_CERTIFICATE_REQUEST = 13,
  DSK_TLS_HANDSHAKE_TYPE_CERTIFICATE_VERIFY = 15,
  DSK_TLS_HANDSHAKE_TYPE_FINISHED = 20,
  DSK_TLS_HANDSHAKE_TYPE_KEY_UPDATE = 24,
  DSK_TLS_HANDSHAKE_TYPE_MESSAGE_HASH = 254,
} DskTlsHandshakeType;


typedef enum
{
  DSK_TLS_CIPHER_SUITE_TLS_AES_128_GCM_SHA256         = 0x1301,  /*rfc5116*/
  DSK_TLS_CIPHER_SUITE_TLS_AES_256_GCM_SHA384         = 0x1302,  /*rfc5116*/
  DSK_TLS_CIPHER_SUITE_TLS_CHACHA20_POLY1305_SHA256   = 0x1303,  /*rfc8439*/
  DSK_TLS_CIPHER_SUITE_TLS_AES_128_CCM_SHA256         = 0x1304,  /*rfc5116*/
  DSK_TLS_CIPHER_SUITE_TLS_AES_128_CCM_8_SHA256       = 0x1305,  /*rfc6655*/
} DskTlsCipherSuite;

typedef enum
{
  DSK_TLS_CERTIFICATE_TYPE_X509 = 0,
  DSK_TLS_CERTIFICATE_TYPE_OPENPGP = 1,
  DSK_TLS_CERTIFICATE_TYPE_RAW_PUBLIC_KEY = 2
} DskTlsCertificateType;

typedef enum {
  DSK_TLS_EXTENSION_TYPE_SERVER_NAME                               = 0,
  DSK_TLS_EXTENSION_TYPE_MAX_FRAGMENT_LENGTH                       = 1,
  DSK_TLS_EXTENSION_TYPE_STATUS_REQUEST                            = 5,
  DSK_TLS_EXTENSION_TYPE_SUPPORTED_GROUPS                          = 10,
  DSK_TLS_EXTENSION_TYPE_SIGNATURE_ALGORITHMS                      = 13,
  DSK_TLS_EXTENSION_TYPE_USE_SRTP                                  = 14,
  DSK_TLS_EXTENSION_TYPE_HEARTBEAT                                 = 15,
  DSK_TLS_EXTENSION_TYPE_APPLICATION_LAYER_PROTOCOL_NEGOTIATION    = 16,
  DSK_TLS_EXTENSION_TYPE_SIGNED_CERTIFICATE_TIMESTAMP              = 18,
  DSK_TLS_EXTENSION_TYPE_CLIENT_CERTIFICATE_TYPE                   = 19,
  DSK_TLS_EXTENSION_TYPE_SERVER_CERTIFICATE_TYPE                   = 20,
  DSK_TLS_EXTENSION_TYPE_PADDING                                   = 21,
  DSK_TLS_EXTENSION_TYPE_PRE_SHARED_KEY                            = 41,
  DSK_TLS_EXTENSION_TYPE_EARLY_DATA                                = 42,
  DSK_TLS_EXTENSION_TYPE_SUPPORTED_VERSIONS                        = 43,
  DSK_TLS_EXTENSION_TYPE_COOKIE                                    = 44,
  DSK_TLS_EXTENSION_TYPE_PSK_KEY_EXCHANGE_MODES                    = 45,
  DSK_TLS_EXTENSION_TYPE_CERTIFICATE_AUTHORITIES                   = 47,
  DSK_TLS_EXTENSION_TYPE_OID_FILTERS                               = 48,
  DSK_TLS_EXTENSION_TYPE_POST_HANDSHAKE_AUTH                       = 49,
  DSK_TLS_EXTENSION_TYPE_SIGNATURE_ALGORITHMS_CERT                 = 50,
  DSK_TLS_EXTENSION_TYPE_KEY_SHARE                                 = 51,
} DskTlsExtensionType;

struct DskTlsExtensionBase
{
  DskTlsExtensionType type;
  size_t extension_data_length;
  uint8_t *extension_data;
};


/* RFC 6066. Section 3 */
typedef enum
{
  DSK_TLS_EXTENSION_SERVER_NAME_TYPE_HOSTNAME = 0
} DskTlsExtensionServerNameType;
struct DskTlsExtension_ServerNameList_Entry
{
  DskTlsExtensionServerNameType type;
  unsigned name_length;
  const char *name;
};
struct DskTlsExtension_ServerNameList
{
  DskTlsExtensionBase base;
  unsigned n_entries;
  DskTlsExtension_ServerNameList_Entry *entries;
};

/* RFC 6066. Section 4 */
struct DskTlsExtension_MaxFragmentLength
{
  DskTlsExtensionBase base;
  unsigned length;      /* must be 2^9, 2^10, 2^11, 2^12 */
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
typedef enum
{
  DSK_TLS_EXTENSION_NAMED_GROUP_FFDHE2048     = 256,
  DSK_TLS_EXTENSION_NAMED_GROUP_FFDHE3072     = 257,
  DSK_TLS_EXTENSION_NAMED_GROUP_FFDHE4096     = 258,
  DSK_TLS_EXTENSION_NAMED_GROUP_FFDHE6144     = 259,
  DSK_TLS_EXTENSION_NAMED_GROUP_FFDHE8192     = 260,
} DskTlsExtension_NamedGroup;

struct DskTlsExtension_SupportedGroups
{
  DskTlsExtensionBase base;
  unsigned n_supported_groups;
  DskTlsExtension_NamedGroup *supported_groups;
};


typedef enum {
  DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA256               = 0x0401,
  DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA384               = 0x0501,
  DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA512               = 0x0601,
  
  /* ECDSA algorithms */
  DSK_TLS_SIGNATURE_SCHEME_ECDSA_SECP256R1_SHA256         = 0x0403,
  DSK_TLS_SIGNATURE_SCHEME_ECDSA_SECP384R1_SHA384         = 0x0503,
  DSK_TLS_SIGNATURE_SCHEME_ECDSA_SECP521R1_SHA512         = 0x0603,
  
  /* RSASSA-PSS algorithms with public key OID rsaEncryption */
  DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA256            = 0x0804,
  DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA384            = 0x0805,
  DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA512            = 0x0806,
  
  /* EdDSA algorithms */
  DSK_TLS_SIGNATURE_SCHEME_ED25519                        = 0x0807,
  DSK_TLS_SIGNATURE_SCHEME_ED448                          = 0x0808,
  
  /* RSASSA-PSS algorithms with public key OID RSASSA-PSS */
  DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_PSS_SHA256             = 0x0809,
  DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_PSS_SHA384             = 0x080A,
  DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_PSS_SHA512             = 0x080B,
  
  /* Legacy algorithms */
  DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA1                 = 0x0201,
  DSK_TLS_SIGNATURE_SCHEME_ECDSA_SHA1                     = 0x0203,
} DskTlsSignatureScheme;

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
struct DskTlsExtension_Heartbeat
{
  DskTlsExtensionBase base;
  DskTlsExtension_Heartbeat mode;
};


/* --- Application Layer Protocol Negotiation RFC 7301 --- */
struct DskTlsExtension_ProtocolName
{
  uint8_t length;
  char *name;
};
struct DskTlsExtension_ApplicationLayerProtocolNegotiation
{
  DskTlsExtensionBase base;
  uint16_t n_protocols;
  DskTlsExtension_ProtocolName *protocols;
};

/* NOTE: client_certificate_type and server_certificate_type
   and defined in RFC7250 and not implemented. */

/* --- Padding Extension RFC 7685 --- */
struct DskTlsExtension_Padding
{
  DskTlsExtensionBase base;
};


/* --- Key Share extension (RFC 8446) --- */
struct DskTlsExtension_KeyShareEntry
{
  DskTlsExtension_NamedGroup named_group;  // DSK_TLS_EXTENSION_NAMED_GROUP_*
  unsigned key_exchange_length;
  uint8_t *key_exchange_data;
};


struct DskTlsExtension_KeyShare
{
  DskTlsExtensionBase base;

  // for ClientHello message
  unsigned n_key_shares;
  DskTlsExtension_KeyShareEntry *key_shared;

  // for HelloRetryRequest message
  uint16_t selected_group;

  // for ServerHello message
  DskTlsExtension_KeyShareEntry server_share;
};

/* --- Pre Shared Key Extension (RFC 8446, 4.2.11) --- */
struct DskTlsExtension_PresharedKeyIdentity
{
  unsigned identity_length;
  uint8_t *identity;
  uint32_t obfuscated_ticket_age;
};
struct DskTlsExtension_PresharedKeyBinderEntry
{
  uint8_t length;
  uint8_t *data;
};
struct DskTlsExtension_OfferedPresharedKeys
{
  unsigned n_identities;
  DskTlsExtension_PresharedKeyIdentity *identities;
  unsigned n_binder_entries;
  DskTlsExtension_PresharedKeyBinderEntry *binder_entries;
};
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
struct DskTlsExtension_PSKKeyExchangeModes
{
  DskTlsExtensionBase base;
  unsigned n_modes;
  DskTlsExtension_PSKKeyExchangeMode *modes;
};

/* --- Early Data Indication (RFC 8446, 4.???) --- */
struct DskTlsExtension_EarlyDataIndication
{
  DskTlsExtensionBase base;

  // Only valid for new_session_ticket handshake-message-type;
  // 0 otherwise, just an indicator that early-data is wanted.
  uint32_t max_early_data_size;
};

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
} DskTlsProtocolVersion

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
struct DskTlsExtension_CertificateAuthorityDistinguishedName
{
  unsigned length;
  char *name;
};
struct DskTlsExtension_CertificateAuthorities
{
  DskTlsExtensionBase base;
  unsigned n_names;
  DskTlsExtension_CertificateAuthorityDistinguishedName *names;
};

/* --- OID Filters Extension RFC 8446 4.2.5 --- */
struct DskTlsOIDFilter
{
  unsigned oid_length;
  uint8_t *oid;
  unsigned value_length;
  uint8_t *value;
};
struct DskTlsExtension_OIDFilters
{
  DskTlsExtensionBase base;
  unsigned n_filters;
  DskTlsOIDFilter *filters;
};

   | post_handshake_auth (RFC 8446)                   |          CH |
   | signature_algorithms_cert (RFC 8446)


union {
  uint16_t type;
  DskTlsExtensionBase generic;
  DskTlsExtension_ServerNameList server_name;
  DskTlsExtension_MaxFragmentLength max_fragment_length;
  DskTlsExtension_StatusRequest status_request;
  DskTlsExtension_ECSupportedGroups supported_groups;
  DskTlsExtension_SignatureAlgorithms signature_algorithms;
  // TODO: client_certificate_type server_certificate_type
  DskTlsExtension_Padding padding;
  DskTlsExtension_KeyShare key_share;
  DskTlsExtension_EarlyDataIndication early_data_indication;
} DskTlsExtension;


struct DskTlsHandshake
{
  DskTlsHandshakeType type;
  union {
    struct {
      uint16_t legacy_version;
      uint8_t random[32];
      unsigned legacy_session_id_length;
      uint8_t *legacy_session_id;
      unsigned n_cipher_suites;
      DskTlsCipherSuite *cipher_suites;
      unsigned n_compression_methods;   /* legacy */
      uint8_t *compression_methods;     /* legacy */
      unsigned n_extensions;
      DskTlsExtension *extensions;
    } client_hello;
    struct {
      uint16_t legacy_version;
      uint8_t random[32];
      unsigned legacy_session_id_echo_length;
      uint8_t *legacy_session_id_echo;
      DskTlsCipherSuite cipher_suite;
      uint8_t compression_method;       /* must be 0 for tls 1.3 */
      unsigned n_extensions;
      DskTlsExtension *extensions;
    } server_hello, hello_retry_request;
    struct {
      unsigned n_extensions;
      DskTlsExtension *extensions;
    } encrypted_extensions;
    struct {
      unsigned certificate_request_context_length;
      uint8_t *certificate_request_context;
      unsigned n_extensions;
      DskTlsExtension **extensions;
    } certificate_request;
  };
  DskMemPool allocator;
};


typedef enum
{
  DSK_TLS_CONTENT_TYPE_CONTENT_INVALID = 0,
  DSK_TLS_CONTENT_TYPE_CHANGE_CIPHER_SPEC = 20,
  DSK_TLS_CONTENT_TYPE_ALERT = 21,
  DSK_TLS_CONTENT_TYPE_HANDSHAKE = 22,
  DSK_TLS_CONTENT_TYPE_APPLICATION_DATA = 23
} DskTlsContentType;

struct DskTlsContent
{
  DskTlsContentType type;
  ...
};

