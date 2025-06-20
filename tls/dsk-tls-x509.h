//
// These are simplified somewhat from
//
//     https://tools.ietf.org/html/rfc5280
//
// Validation is described in Section 6.
//
// RFC 8017 defines the algorithms for
// encryption/decryption for PKCS 1.5
// or RSA-PSS.
// 

typedef struct DskTlsX509CertificateClass DskTlsX509CertificateClass;
typedef struct DskTlsX509Certificate DskTlsX509Certificate;

typedef enum
{
  // PKCS1 v1.5.  RFC 8017 Section ...
  DSK_TLS_X509_SIGNATURE_ALGORITHM_RSA_PKCS1_SHA1,
  DSK_TLS_X509_SIGNATURE_ALGORITHM_RSA_PKCS1_SHA256,
  DSK_TLS_X509_SIGNATURE_ALGORITHM_RSA_PKCS1_SHA384,
  DSK_TLS_X509_SIGNATURE_ALGORITHM_RSA_PKCS1_SHA512,

  // PKCS1 PSS.  RFC 8017 Section ...
  DSK_TLS_X509_SIGNATURE_ALGORITHM_RSA_PSS_SHA256,
  DSK_TLS_X509_SIGNATURE_ALGORITHM_RSA_PSS_SHA384,
  DSK_TLS_X509_SIGNATURE_ALGORITHM_RSA_PSS_SHA512,

  // hash algo unspecified
  DSK_TLS_X509_SIGNATURE_ALGORITHM_RSA_PSS,
} DskTlsX509SignatureAlgorithm;


//
// Everyone's favorite concept from ASN.1: the DistinguishedName.
// (These suckers are pervasive in ldap as well)
//
typedef enum
{
  DSK_TLS_X509_DN_NAME,
  DSK_TLS_X509_DN_SURNAME,
  DSK_TLS_X509_DN_GIVEN_NAME,
  DSK_TLS_X509_DN_INITIALS,
  DSK_TLS_X509_DN_GENERATION_QUALIFIER,
  DSK_TLS_X509_DN_COMMON_NAME,
  DSK_TLS_X509_DN_LOCALITY,
  DSK_TLS_X509_DN_STATE_OR_PROVINCE,
  DSK_TLS_X509_DN_ORGANIZATION_NAME,
  DSK_TLS_X509_DN_ORGANIZATIONAL_UNIT,
  DSK_TLS_X509_DN_STREET_ADDRESS,
  DSK_TLS_X509_DN_TITLE,
  DSK_TLS_X509_DN_QUALIFIER,
  DSK_TLS_X509_DN_COUNTRY,
  DSK_TLS_X509_DN_SERIAL_NUMBER,
  DSK_TLS_X509_DN_PSEUDONAME,
  DSK_TLS_X509_DN_DOMAIN_COMPONENT,
  DSK_TLS_X509_DN_EMAIL_ADDRESS,
  DSK_TLS_X509_DN_USER_ID,
} DskTlsX509DistinguishedNameType;

typedef struct DskTlsX509DistinguishedName
{
  DskTlsX509DistinguishedNameType type;
  char *name;
} DskTlsX509DistinguishedName;

typedef struct DskTlsX509Name
{
  size_t n_distinguished_names;
  DskTlsX509DistinguishedName *distinguished_names;
  DskTlsX509DistinguishedName **names_sorted_by_type;
} DskTlsX509Name;

const char *dsk_tls_x509_name_get_component (const DskTlsX509Name *name,
                                             DskTlsX509DistinguishedNameType t);

bool     dsk_tls_x509_names_equal(const DskTlsX509Name *a,
                                  const DskTlsX509Name *b);
unsigned dsk_tls_x509_name_hash (const DskTlsX509Name *a);

char * dsk_tls_x509_name_to_string (const DskTlsX509Name *a);

//
// Unix-Timestamps for the range with
// the certificate is valid.
//
typedef struct DskTlsX509Validity
{
  uint64_t not_before;
  uint64_t not_after;
} DskTlsX509Validity;

typedef struct DskTlsX509SubjectPublicKeyInfo
{
  DskTlsX509SignatureAlgorithm algorithm;
  size_t public_key_length;
  uint8_t *public_key;
} DskTlsX509SubjectPublicKeyInfo;

#define DSK_TLS_X509_CERTIFICATE(o) \
  DSK_OBJECT_CAST(DskTlsX509Certificate, o, &dsk_tls_x509_certificate_class)
#define DSK_IS_TLS_X509_CERTIFICATE(o) \
  dsk_object_is_a ((o), &dsk_tls_x509_certificate_class)

extern DskTlsX509CertificateClass dsk_tls_x509_certificate_class;
struct DskTlsX509CertificateClass
{
  DskTlsKeyPairClass base_class;
};


typedef enum
{
  DSK_TLS_X509_KEY_TYPE_RSA_PUBLIC,
  DSK_TLS_X509_KEY_TYPE_RSA_PRIVATE,
} DskTlsX509KeyType;

//
// Handles unsigned ("TBS") and signed Certificates.
//
struct DskTlsX509Certificate
{
  DskTlsKeyPair base_instance;

  // If non-null, then all the other members were allocated
  // out of this slab.
  void *allocations;

  unsigned version;             // 1 is v1, etc
  uint8_t serial_number[20];
  DskTlsX509SignatureAlgorithm signature_algorithm;
  DskTlsX509Name issuer;
  DskTlsX509Validity validity;
  DskTlsX509Name subject;
  DskTlsX509SubjectPublicKeyInfo subject_public_key_info;

  DskTlsX509KeyType key_type;
  void *key;

  bool has_issuer_unique_id;
  size_t issuer_unique_id_len;
  uint32_t *issuer_unique_id;

  bool has_subject_unique_id;
  size_t subject_unique_id_len;
  uint32_t *subject_unique_id;

  unsigned n_extensions;
  // TODO extensions not handled yet

  bool is_signed;
  size_t signature_length;
  uint8_t *signature_data;

  //
  // Serialized version of the cert.
  //
  unsigned cert_data_length;
  uint8_t *cert_data;
};

//
// NOTE: the pool is not used to allocate the final object,
// but instead various ASN1 intermediaries (via expand_tag)
//
DskTlsX509Certificate *
dsk_tls_x509_unsigned_certificate_from_asn1 (DskASN1Value *value,
                                             DskMemPool   *pool,
                                             DskError    **error);
DskTlsX509Certificate *
dsk_tls_x509_certificate_from_asn1 (DskASN1Value *value,
                                    DskMemPool   *pool,
                                    DskError    **error);


bool dsk_tls_x509_certificates_match (const DskTlsX509Certificate *a,
                                      const DskTlsX509Certificate *b);

// This only validates the signature.  Expiration times are ignored.
///bool
///dsk_tls_x509_certificate_verify (DskTlsX509Certificate *cert,
///                                 DskTlsX509Certificate *signing_cert);
