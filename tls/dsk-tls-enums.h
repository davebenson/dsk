
/* The outermost layer of TLS is the Record Layer.
 * This 5-byte header is sent unencrypted.
 */
typedef enum
{
  DSK_TLS_RECORD_CONTENT_TYPE_INVALID = 0,
  DSK_TLS_RECORD_CONTENT_TYPE_CHANGE_CIPHER_SPEC = 20,
  DSK_TLS_RECORD_CONTENT_TYPE_ALERT = 21,
  DSK_TLS_RECORD_CONTENT_TYPE_HANDSHAKE = 22,
  DSK_TLS_RECORD_CONTENT_TYPE_APPLICATION_DATA = 23,
} DskTlsRecordContentType;
const char *dsk_tls_record_content_type_name (DskTlsRecordContentType type);

/* If the record's ContentType is HANDSHAKE==22,
 * then the records contains whole or partial 
 * handshake messages.
 */
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
  DSK_TLS_HANDSHAKE_TYPE_MESSAGE_HASH = 254,    // not a real message type; see 4.4.1
} DskTlsHandshakeType;
const char *dsk_tls_handshake_type_name (DskTlsHandshakeType type);

typedef enum
{
  DSK_TLS_CIPHER_SUITE_TLS_AES_128_GCM_SHA256         = 0x1301,  /*rfc5116*/
  DSK_TLS_CIPHER_SUITE_TLS_AES_256_GCM_SHA384         = 0x1302,  /*rfc5116*/
  DSK_TLS_CIPHER_SUITE_TLS_CHACHA20_POLY1305_SHA256   = 0x1303,  /*rfc8439*/
  DSK_TLS_CIPHER_SUITE_TLS_AES_128_CCM_SHA256         = 0x1304,  /*rfc5116*/
  DSK_TLS_CIPHER_SUITE_TLS_AES_128_CCM_8_SHA256       = 0x1305,  /*rfc6655*/
} DskTlsCipherSuite;
const char *dsk_tls_cipher_suite_name (DskTlsCipherSuite cipher);

typedef enum
{
  DSK_TLS_CERTIFICATE_TYPE_X509 = 0,
  DSK_TLS_CERTIFICATE_TYPE_OPENPGP = 1,
  DSK_TLS_CERTIFICATE_TYPE_RAW_PUBLIC_KEY = 2
} DskTlsCertificateType;
const char *dsk_tls_certificate_type_name (DskTlsCertificateType type);

// Except as noted, these "extensions" originate from the TLS 1.3 specification, RFC 8446.
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
  DSK_TLS_EXTENSION_TYPE_RECORD_SIZE_LIMIT                         = 28,   // RFC 8449
  DSK_TLS_EXTENSION_TYPE_SESSION_TICKET                            = 35,   // RFC 5077
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
const char * dsk_tls_extension_type_name(DskTlsExtensionType type);

/* RFC 7919.  Named Elliptic Curves "Supported Group Extension" */
typedef enum
{
  DSK_TLS_NAMED_GROUP_SECP256R1     = 0x17,
  DSK_TLS_NAMED_GROUP_SECP384R1     = 0x18,
  DSK_TLS_NAMED_GROUP_SECP521R1     = 0x19,
  DSK_TLS_NAMED_GROUP_X25519        = 0x1d,
  DSK_TLS_NAMED_GROUP_X448          = 0x1e,
  
  // See RFC 7919 for the values
  DSK_TLS_NAMED_GROUP_FFDHE2048     = 0x100,
  DSK_TLS_NAMED_GROUP_FFDHE3072     = 0x101,
  DSK_TLS_NAMED_GROUP_FFDHE4096     = 0x102,
  DSK_TLS_NAMED_GROUP_FFDHE6144     = 0x103,
  DSK_TLS_NAMED_GROUP_FFDHE8192     = 0x104,
} DskTlsNamedGroup;
const char *dsk_tls_named_group_name (DskTlsNamedGroup g);


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
  DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_PSS_SHA256          = 0x0809,
  DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_PSS_SHA384          = 0x080A,
  DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_PSS_SHA512          = 0x080B,
  
  /* Legacy algorithms */
  DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA1                 = 0x0201,
  DSK_TLS_SIGNATURE_SCHEME_ECDSA_SHA1                     = 0x0203,
} DskTlsSignatureScheme;

