
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

#define DSK_TLS_EXTENSION_TYPE_SERVER_NAME                               0
#define DSK_TLS_EXTENSION_TYPE_MAX_FRAGMENT_LENGTH                       1
#define DSK_TLS_EXTENSION_TYPE_STATUS_REQUEST                            5
#define DSK_TLS_EXTENSION_TYPE_SUPPORTED_GROUPS                          10
#define DSK_TLS_EXTENSION_TYPE_SIGNATURE_ALGORITHMS                      13
#define DSK_TLS_EXTENSION_TYPE_USE_SRTP                                  14
#define DSK_TLS_EXTENSION_TYPE_HEARTBEAT                                 15
#define DSK_TLS_EXTENSION_TYPE_APPLICATION_LAYER_PROTOCOL_NEGOTIATION    16
#define DSK_TLS_EXTENSION_TYPE_SIGNED_CERTIFICATE_TIMESTAMP              18
#define DSK_TLS_EXTENSION_TYPE_CLIENT_CERTIFICATE_TYPE                   19
#define DSK_TLS_EXTENSION_TYPE_SERVER_CERTIFICATE_TYPE                   20
#define DSK_TLS_EXTENSION_TYPE_PADDING                                   21
#define DSK_TLS_EXTENSION_TYPE_PRE_SHARED_KEY                            41
#define DSK_TLS_EXTENSION_TYPE_EARLY_DATA                                42
#define DSK_TLS_EXTENSION_TYPE_SUPPORTED_VERSIONS                        43
#define DSK_TLS_EXTENSION_TYPE_COOKIE                                    44
#define DSK_TLS_EXTENSION_TYPE_PSK_KEY_EXCHANGE_MODES                    45
#define DSK_TLS_EXTENSION_TYPE_CERTIFICATE_AUTHORITIES                   47
#define DSK_TLS_EXTENSION_TYPE_OID_FILTERS                               48
#define DSK_TLS_EXTENSION_TYPE_POST_HANDSHAKE_AUTH                       49
#define DSK_TLS_EXTENSION_TYPE_SIGNATURE_ALGORITHMS_CERT                 50
#define DSK_TLS_EXTENSION_TYPE_KEY_SHARE                                 51

struct DskTlsExtensionBase
{
  uint16_t type;
  size_t extension_data_length;
  uint8_t *extension_data;
};


/* RFC 6066. Section 3 */
struct DskTlsExtension_ServerNameList_Entry
{
  uint8_t type;
  unsigned name_length;
  const char *name;
};
#define DSK_TLS_EXTENSION_SERVER_NAME_TYPE_HOSTNAME 0
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
   | status_request [RFC6066]                         |  CH, CR, CT |
   |                                                  |             |
   | supported_groups [RFC7919]                       |      CH, EE |
   |                                                  |             |
   | signature_algorithms (RFC 8446)                  |      CH, CR |
  - |                                                  |             |
   | use_srtp [RFC5764]                               |      CH, EE |
   |                                                  |             |
   | heartbeat [RFC6520]                              |      CH, EE |
   |                                                  |             |
   | application_layer_protocol_negotiation [RFC7301] |      CH, EE |
   |                                                  |             |
   | signed_certificate_timestamp [RFC6962]           |  CH, CR, CT |
   |                                                  |             |
   | client_certificate_type [RFC7250]                |      CH, EE |
   |                                                  |             |
   | server_certificate_type [RFC7250]                |      CH, EE |
   |                                                  |             |
   | padding [RFC7685]                                |          CH |
   |                                                  |             |
   | key_share (RFC 8446)                             | CH, SH, HRR |
   |                                                  |             |
   | pre_shared_key (RFC 8446)                        |      CH, SH |
   |                                                  |             |
   | psk_key_exchange_modes (RFC 8446)                |          CH |
   |                                                  |             |
   | early_data (RFC 8446)                            | CH, EE, NST |
   |                                                  |             |
   | cookie (RFC 8446)                                |     CH, HRR |
   |                                                  |             |
   | supported_versions (RFC 8446)                    | CH, SH, HRR |
   |                                                  |             |
   | certificate_authorities (RFC 8446)               |      CH, CR |
   |                                                  |             |
   | oid_filters (RFC 8446)                           |          CR |
   |                                                  |             |
   | post_handshake_auth (RFC 8446)                   |          CH |
   |                                                  |             |
   | signature_algorithms_cert (RFC 8446)


union {
  uint16_t type;
  DskTlsExtensionBase generic;
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

