
// These are simplified somewhat from
//     https://tools.ietf.org/html/rfc5280

typedef struct DskTlsX509Certificate DskTlsX509Certificate;


//
// Everyone's favorite concept from LDAP, the DistinguishedName.
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
} DskTlsX509Name;

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
  DskTlsSignatureScheme algorithm;
  size_t public_key_length;
  uint8_t *public_key;
} DskTlsX509SubjectPublicKeyInfo;


//
// Handles unsigned ("TBS") and signed Certificates.
//
struct DskTlsX509Certificate
{
  unsigned version;             // 1 is v1, etc
  uint8_t serial_number[20];
  DskTlsSignatureScheme signature_scheme;
  DskTlsX509Name issuer;
  DskTlsX509Validity validity;
  DskTlsX509Name subject;
  DskTlsX509SubjectPublicKeyInfo subject_public_key_info;

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
};
DskTlsX509Certificate *
dsk_tls_x509_unsigned_certificate_from_asn1 (DskASN1Value *value,
                                             DskMemPool   *tmp_pool,
                                             DskError    **error);
DskTlsX509Certificate *
dsk_tls_x509_certificate_from_asn1 (DskASN1Value *value,
                                    DskMemPool   *tmp_pool,
                                    DskError    **error);

void dsk_tls_x509_certificate_free (DskTlsX509Certificate *cert);

