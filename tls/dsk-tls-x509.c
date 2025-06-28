#include "../dsk.h"
#include "../dsk-qsort-macro.h"
#include <string.h>

//
// TODO: add comments from the ASN def of x509
//       just to break up the monotony.
//

#if 0
static void
dump_asn1(const DskASN1Value *v)
{
  DskBuffer buf = DSK_BUFFER_INIT;
  dsk_asn1_value_dump_to_buffer (v, &buf);
  dsk_buffer_writev (&buf, STDOUT_FILENO);
  dsk_buffer_clear (&buf);
}
#endif
static bool
parse_algo_id (DskASN1Value *algorithm_id,
               DskMemPool *pool,
               DskTlsX509SignatureAlgorithm *algo_out,
               DskError  **error)
{
  if (algorithm_id->type != DSK_ASN1_TYPE_SEQUENCE
   || algorithm_id->v_sequence.n_children == 0
   || algorithm_id->v_sequence.n_children > 2
   || algorithm_id->v_sequence.children[0]->type != DSK_ASN1_TYPE_OBJECT_IDENTIFIER)
    {
      *error = dsk_error_new ("AlgorithmID must be a Sequence with OID at first position");
      return false;
    }

  const DskTlsObjectID *oid = algorithm_id->v_sequence.children[0]->v_object_identifier;
  const DskASN1Value *algo_params = algorithm_id->v_sequence.children[1];
  if (!dsk_tls_oid_to_x509_signature_algorithm (oid, pool, algo_params, algo_out, error))
    {
      //*error = dsk_error_new ("signature-scheme not known for OID");
      return false;
    }
  return true;
}

#if 0
static int
compare_p_distiguished_name_by_type (const void *a, const void *b)
{
  const DskTlsX509DistinguishedName *const *p_a_dn = a;
  const DskTlsX509DistinguishedName *const *p_b_dn = b;
  const DskTlsX509DistinguishedName *a_dn = *p_a_dn;
  const DskTlsX509DistinguishedName *b_dn = *p_b_dn;
  return a_dn->type < b_dn->type
       ? -1
       : a_dn->type > b_dn->type
       ? 1
       : 0;
}
#endif

static bool
parse_name (DskASN1Value    *value,
            DskTlsX509Name  *name_out,
            DskError       **error)
{
  if (value->type != DSK_ASN1_TYPE_SEQUENCE)
    {
      *error = dsk_error_new ("name must be a sequence");
      return false;
    }
  unsigned n_names = value->v_sequence.n_children;
  DskASN1Value **names = value->v_sequence.children;
  DskTlsX509DistinguishedName *dn = DSK_NEW_ARRAY (n_names, DskTlsX509DistinguishedName);
  unsigned i;
  for (i = 0; i < n_names; i++)
    {
      DskASN1Value *name = names[i];
      if (name->type != DSK_ASN1_TYPE_SET
       || name->v_set.n_children != 1)
        {
          *error = dsk_error_new ("each distinguished-name must encased in a singleton");
          goto failed;
        }
      name = name->v_set.children[0];
      
      if (name->type != DSK_ASN1_TYPE_SEQUENCE
       || name->v_sequence.n_children != 2
       || name->v_sequence.children[0]->type != DSK_ASN1_TYPE_OBJECT_IDENTIFIER)
        {
          *error = dsk_error_new ("each distinguished-name must a sequence of OID and Name");
          goto failed;
        }
      const DskTlsObjectID *oid = name->v_sequence.children[0]->v_object_identifier;
      DskASN1Value *name_value = name->v_sequence.children[1];
      if (!dsk_tls_oid_to_x509_distinguished_name_type (oid, &dn[i].type))
        {
          char *oid_str = dsk_tls_object_id_to_string (oid);
          *error = dsk_error_new ("distinguished-name type has unrecognized OID: %s", oid_str);
          dsk_free (oid_str);
          goto failed;
        }

      //
      // XXX: type check of dn[i].type versus name_value's type
      //
      //TODO

      if (name_value->is_constructed)
        {
          *error = dsk_error_new ("distinguished-name values must be primitive");
          goto failed;
        }
      dn[i].name = dsk_asn1_primitive_value_to_string (name_value);
    }
  name_out->n_distinguished_names = n_names;
  name_out->distinguished_names = dn;

  //
  // Compute by-type lookup table.
  //
  name_out->names_sorted_by_type = DSK_NEW_ARRAY (n_names, DskTlsX509DistinguishedName *);
  for (unsigned i = 0; i < n_names; i++)
    name_out->names_sorted_by_type[i] = dn + i;
#define COMPARE_DN_BY_TYPE(a,b, rv)   \
    if (a->type < b->type)            \
      rv = -1;                        \
    else if (a->type > b->type)       \
      rv = 1;                         \
    else                              \
      rv = 0;
  DSK_QSORT(name_out->names_sorted_by_type,
            DskTlsX509DistinguishedName *,
            n_names,
            COMPARE_DN_BY_TYPE);
#undef COMPARE_DN_BY_TYPE

  //
  // Verify that all the distinguished-names
  // have different distinguishers (ie types).
  //
  // (This loop works because names_sorted_by_type is sorted.)
  //
  for (unsigned i = 1; i < n_names; i++)
    {
      if (name_out->names_sorted_by_type[i-1]->type
       == name_out->names_sorted_by_type[i]->type)
        {
          *error = dsk_error_new ("distinguished-name entries must have distinct types");
          dsk_free (name_out->names_sorted_by_type);
          goto failed;
        }
    }
  return true;

failed:
  for (unsigned j = 0; j < i; j++)
    dsk_free (dn[j].name);
  dsk_free (dn);
  return false;
}

const char *dsk_tls_x509_name_get_component (const DskTlsX509Name *name,
                                             DskTlsX509DistinguishedNameType t)
{
  unsigned start = 0, n = name->n_distinguished_names;
  while (n > 0)
    {
      unsigned mid = start + n / 2;
      DskTlsX509DistinguishedNameType mid_type = name->names_sorted_by_type[mid]->type;
      if (t < mid_type)
        {
          n /= 2;
        }
      else if (t > mid_type)
        {
          n = start + n - (mid + 1);
          start = mid + 1;
        }
      else
        return name->names_sorted_by_type[mid]->name;
    }
  if (n == 1 && name->names_sorted_by_type[start]->type == t)
    {
      return name->names_sorted_by_type[start]->name;
    }
  return NULL;
}

bool     dsk_tls_x509_name_equal(const DskTlsX509Name *a,
                                 const DskTlsX509Name *b)
{
  if (a->n_distinguished_names != b->n_distinguished_names)
    return false;
  unsigned n = a->n_distinguished_names;
  DskTlsX509DistinguishedName **a_names = a->names_sorted_by_type;
  DskTlsX509DistinguishedName **b_names = b->names_sorted_by_type;
  for (unsigned i = 0; i < n; i++)
    {
      if (a_names[i]->type != b_names[i]->type)
        return false;
      if (strcmp(a_names[i]->name, b_names[i]->name) == 0)
        return false;
    }
  return true;
}

uint32_t dsk_tls_x509_name_hash (const DskTlsX509Name *a)
{
  unsigned n = a->n_distinguished_names;
  DskTlsX509DistinguishedName **a_names = a->names_sorted_by_type;
  uint32_t hash = 5381;
  for (unsigned i = 0; i < n; i++)
    {
      hash += a_names[i]->type;
      hash *= 33;
      const char *str = a_names[i]->name;
      while (*str)
        {
          hash += (unsigned) (uint8_t) *str++;
          hash *= 33;
        }
    }
  return hash;
}

static void
dsk_tls_x509_name_clear (DskTlsX509Name *name)
{
  for (unsigned i = 0; i < name->n_distinguished_names; i++)
    dsk_free (name->distinguished_names[i].name);
  dsk_free (name->names_sorted_by_type);
  dsk_free (name->distinguished_names);
}

static bool
parse_validity (DskASN1Value        *value,
                DskTlsX509Validity  *validity_out,
                DskError           **error)
{
  if (value->type != DSK_ASN1_TYPE_SEQUENCE
   || value->v_sequence.n_children != 2
   || value->v_sequence.children[0]->type != DSK_ASN1_TYPE_UTC_TIME
   || value->v_sequence.children[1]->type != DSK_ASN1_TYPE_UTC_TIME)
    {
      *error = dsk_error_new ("Validity portion of x509 cert invalid");
      return false;
    }
  validity_out->not_before = value->v_sequence.children[0]->v_time.unixtime;
  validity_out->not_after = value->v_sequence.children[1]->v_time.unixtime;
  return true;
}
#define dsk_tls_x509_validity_clear(v)


//
// NOTE: pool should only be used for temporaries.
// The members of spki_out must be initialized
// with heap memory (ie dsk_new etc).
//
static bool
parse_subject_public_key_info (DskASN1Value        *value,
                               DskMemPool          *pool,
                               DskTlsX509SubjectPublicKeyInfo  *spki_out,
                               DskError           **error)
{
  if (value->type != DSK_ASN1_TYPE_SEQUENCE
   || value->v_sequence.n_children != 2)
    {
      *error = dsk_error_new ("SubjectPublicKeyInfo must be a sequence of length 2");
      return false;
    }
  if (!parse_algo_id (value->v_sequence.children[0], pool, &spki_out->algorithm, error))
    return false;
  if (value->v_sequence.children[1]->type != DSK_ASN1_TYPE_BIT_STRING
   || value->v_sequence.children[1]->v_bit_string.length % 8 != 0)
    {
      *error = dsk_error_new ("public key must be a bit-string of octets");
      return false;
    }

  unsigned len = value->v_sequence.children[1]->v_bit_string.length / 8;
  spki_out->public_key_length = len;
  spki_out->public_key = dsk_memdup (len, value->v_sequence.children[1]->v_bit_string.bits);
  return true;
}

static void
dsk_tls_x509_subject_public_key_info_clear (DskTlsX509SubjectPublicKeyInfo *spki)
{
  dsk_free (spki->public_key);
}

DskTlsX509Certificate *
dsk_tls_x509_unsigned_certificate_from_asn1 (DskASN1Value *value,
                                             DskMemPool   *pool,
                                             DskError    **error)
{
  if (value->type != DSK_ASN1_TYPE_SEQUENCE)
    {
      *error = dsk_error_new ("expected Sequence for X509 TBS Certificate");
      return NULL;
    }
  unsigned tbs_at = 0;
  int version_number;
  if (tbs_at < value->v_sequence.n_children
   && value->v_sequence.children[0]->type == DSK_ASN1_TYPE_CONTEXT_SPECIFIC_0)
    {
      DskASN1Value *version_tag = value->v_sequence.children[0];
      if (!dsk_asn1_value_expand_tag (version_tag, pool,
                                      DSK_ASN1_TYPE_INTEGER, true,
                                      error))
        {
          return NULL;
        }
      version_tag = version_tag->v_tagged.subvalue;
      version_number = version_tag->v_integer + 1;
      tbs_at++;
    }
  else
    {
      version_number = 1;
    }
  if (tbs_at + 6 > value->v_sequence.n_children)
    {
      *error = dsk_error_new ("at least 6 members needed in sequence after Version");
      return NULL;
    }
  DskASN1Value *serial_number = value->v_sequence.children[tbs_at++];
  if (serial_number->type != DSK_ASN1_TYPE_INTEGER)
    {
      *error = dsk_error_new ("expecting serial-number to be integer in X509 Certificate");
      return NULL;
    }
  if (serial_number->value_end - serial_number->value_start > 20)
    {
      *error = dsk_error_new ("serial-number too large in X509 Certificate");
      return NULL;
    }
  DskTlsX509Certificate *rv = dsk_object_new (&dsk_tls_x509_certificate_class);
  unsigned sn_len = serial_number->value_end - serial_number->value_start;
  unsigned sn_pad = 20 - sn_len;
  memset (rv->serial_number, 0, sn_pad);
  memcpy (rv->serial_number + sn_pad, serial_number->value_start, sn_len);
  rv->version = version_number;

  DskASN1Value *algorithm_id = value->v_sequence.children[tbs_at++];
  if (!parse_algo_id (algorithm_id, pool, &rv->signature_algorithm, error))
    {
      dsk_object_unref (rv);
      return NULL;
    }
  DskASN1Value *issuer_value = value->v_sequence.children[tbs_at++];
  if (!parse_name (issuer_value, &rv->issuer, error))
    {
      dsk_object_unref (rv);
      return NULL;
    }
  DskASN1Value *validity_value = value->v_sequence.children[tbs_at++];
  if (!parse_validity (validity_value, &rv->validity, error))
    {
      dsk_object_unref (rv);
      return NULL;
    }
  DskASN1Value *subject_value = value->v_sequence.children[tbs_at++];
  if (!parse_name (subject_value, &rv->subject, error))
    {
      dsk_object_unref (rv);
      return NULL;
    }
  DskASN1Value *subpki_value = value->v_sequence.children[tbs_at++];
  if (!parse_subject_public_key_info (subpki_value, pool, &rv->subject_public_key_info, error))
    {
      dsk_object_unref (rv);
      return false;
    }

  DskASN1Value *tmp = tbs_at < value->v_sequence.n_children ? value->v_sequence.children[tbs_at] : NULL;
  if (tmp != NULL && tmp->type == DSK_ASN1_TYPE_CONTEXT_SPECIFIC_1)
    {
      if (!dsk_asn1_value_expand_tag (tmp, pool,
                                      DSK_ASN1_TYPE_BIT_STRING, false,
                                      error))
        {
          dsk_object_unref (rv);
          return false;
        }

      rv->has_issuer_unique_id = true;
      rv->issuer_unique_id_len = (tmp->v_tagged.subvalue->v_bit_string.length + 7) / 8;
      rv->issuer_unique_id = dsk_memdup (rv->issuer_unique_id_len, tmp->v_tagged.subvalue->v_bit_string.bits);

      tbs_at++;
      tmp = tbs_at < value->v_sequence.n_children ? value->v_sequence.children[tbs_at] : NULL;
    }

  if (tmp != NULL && tmp->type == DSK_ASN1_TYPE_CONTEXT_SPECIFIC_2)
    {
      if (!dsk_asn1_value_expand_tag (tmp, pool,
                                      DSK_ASN1_TYPE_BIT_STRING, false,
                                      error))
        {
          dsk_object_unref (rv);
          return false;
        }

      rv->has_subject_unique_id = true;
      rv->subject_unique_id_len = (tmp->v_tagged.subvalue->v_bit_string.length + 7) / 8;
      rv->subject_unique_id = dsk_memdup (rv->issuer_unique_id_len, tmp->v_tagged.subvalue->v_bit_string.bits);

      tbs_at++;
      tmp = tbs_at < value->v_sequence.n_children ? value->v_sequence.children[tbs_at] : NULL;
    }
  if (tmp != NULL && tmp->type == DSK_ASN1_TYPE_CONTEXT_SPECIFIC_3)
    {
      if (!dsk_asn1_value_expand_tag (tmp, pool,
                                      DSK_ASN1_TYPE_SEQUENCE, false,
                                      error))
        {
          dsk_object_unref (rv);
          return false;
        }
      
      rv->n_extensions = tmp->v_tagged.subvalue->v_sequence.n_children;
      for (unsigned i = 0; i < rv->n_extensions; i++)
        {
          DskASN1Value *ext_value = tmp->v_tagged.subvalue->v_sequence.children[i];
          //TODO handle extension
          if (ext_value->type != DSK_ASN1_TYPE_SEQUENCE
           || ext_value->v_sequence.n_children < 2
           || ext_value->v_sequence.n_children > 3)
            {
              *error = dsk_error_new ("bad X509 Certificate extension");
              dsk_object_unref (rv);
              return false;
            }
        }

      tbs_at++;
      tmp = NULL;
    }
  rv->is_signed = false;
  return rv;
}
DskTlsX509Certificate *
dsk_tls_x509_certificate_from_asn1 (DskASN1Value *value,
                                    DskMemPool   *pool,
                                    DskError    **error)
{
  if (value->type != DSK_ASN1_TYPE_SEQUENCE)
    {
      *error = dsk_error_new ("expected Sequence for X509 Certificate");
      return NULL;
    }
  if (value->v_sequence.n_children != 3)
    {
      *error = dsk_error_new ("Sequence for X509 Certificate must have 3 children");
      return NULL;
    }
  DskASN1Value *tbs_cert = value->v_sequence.children[0];
  DskASN1Value *cert_sig_algo = value->v_sequence.children[1];
  DskASN1Value *sig = value->v_sequence.children[2];
  DskTlsX509Certificate *rv = dsk_tls_x509_unsigned_certificate_from_asn1 (tbs_cert, pool, error);
  if (rv == NULL)
    return NULL;
  DskTlsX509SignatureAlgorithm algo;
  if (!parse_algo_id (cert_sig_algo, pool, &algo, error))
    {
      dsk_object_unref (rv);
      return NULL;
    }
  if (algo != rv->signature_algorithm)
    {
      *error = dsk_error_new ("signature algorithms not consisent in certificate");
      dsk_object_unref (rv);
      return NULL;
    }
  if (sig->type != DSK_ASN1_TYPE_BIT_STRING
   || sig->v_bit_string.length % 8 != 0)
    {
      *error = dsk_error_new ("signature must be a number of bytes");
      dsk_object_unref (rv);
      return NULL;
    }
  rv->is_signed = true;
  rv->signature_length = sig->v_bit_string.length / 8;
  rv->signature_data = dsk_memdup(rv->signature_length, sig->v_bit_string.bits);
  return rv;
}


static void
dsk_tls_x509_certificate_finalize (DskTlsX509Certificate *cert)
{
  dsk_tls_x509_name_clear (&cert->issuer);
  dsk_tls_x509_validity_clear (&cert->validity);
  dsk_tls_x509_name_clear (&cert->subject);
  dsk_tls_x509_subject_public_key_info_clear (&cert->subject_public_key_info);
  dsk_free (cert->issuer_unique_id);
  dsk_free (cert->subject_unique_id);
  dsk_free (cert->signature_data);
}

static size_t
dsk_tls_x509_certificate_get_signature_length (DskTlsKeyPair     *kp,
                                               DskTlsSignatureScheme scheme)
{
  DskTlsX509Certificate *cert = (DskTlsX509Certificate *) kp;

  (void) scheme;

  switch (cert->key_type)
    {
    case DSK_TLS_X509_KEY_TYPE_RSA_PUBLIC:
      {
        DskRSAPublicKey *k = cert->key;
        return k->modulus_length_bytes;
      }
    case DSK_TLS_X509_KEY_TYPE_RSA_PRIVATE:
      {
        DskRSAPrivateKey *k = cert->key;
        return k->modulus_length_bytes;
      }
    default:
      assert(false);
      return 0;
    }
}

static bool
dsk_tls_x509_certificate_has_private_key      (DskTlsKeyPair     *kp)
{
  DskTlsX509Certificate *cert = (DskTlsX509Certificate *) kp;
  switch (cert->key_type)
    {
    case DSK_TLS_X509_KEY_TYPE_RSA_PUBLIC:
      return false;
    case DSK_TLS_X509_KEY_TYPE_RSA_PRIVATE:
      return true;
    default:
      assert(false);
      return 0;
    }
}

static bool
dsk_tls_x509_certificate_supports_scheme      (DskTlsKeyPair     *kp,
                                               DskTlsSignatureScheme algorithm)
{
  DskTlsX509Certificate *cert = (DskTlsX509Certificate *) kp;
  (void) cert;

  switch (algorithm)
    {
    case DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA256:
    case DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA384:
    case DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA512:
    case DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA256:
    case DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA384:
    case DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA512:
    case DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_PSS_SHA256:
    case DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_PSS_SHA384:
    case DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_PSS_SHA512:
      return true;
    default:
      return false;
  }
}

static void
dsk_tls_x509_certificate_sign    (DskTlsKeyPair     *kp,
                                  DskTlsSignatureScheme algorithm,
                                  size_t             content_len,
                                  const uint8_t     *content_data,
                                  uint8_t           *signature_out)
{
  DskTlsX509Certificate *cert = (DskTlsX509Certificate *) kp;
  switch (algorithm)
    {
    case DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA256:
      assert (cert->key_type == DSK_TLS_X509_KEY_TYPE_RSA_PRIVATE);
      dsk_rsassa_pkcs1_5_sign (cert->key,
                               &dsk_checksum_type_sha256,
                               content_len, content_data,
                               signature_out);
      break;
    case DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA384:
      assert (cert->key_type == DSK_TLS_X509_KEY_TYPE_RSA_PRIVATE);
      dsk_rsassa_pkcs1_5_sign (cert->key,
                               &dsk_checksum_type_sha384,
                               content_len, content_data,
                               signature_out);
      break;
    case DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA512:
      assert (cert->key_type == DSK_TLS_X509_KEY_TYPE_RSA_PRIVATE);
      dsk_rsassa_pkcs1_5_sign (cert->key,
                               &dsk_checksum_type_sha512,
                               content_len, content_data,
                               signature_out);
      break;
    case DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA256:
    case DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_PSS_SHA256:
      assert (cert->key_type == DSK_TLS_X509_KEY_TYPE_RSA_PRIVATE);
      dsk_rsassa_pss_sign (cert->key,
                           &dsk_checksum_type_sha256,
                           256/8,          // length of salt is hash-length
                           NULL,           // default RNG
                           content_len, content_data,
                           signature_out);
      break;

    case DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA384:
    case DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_PSS_SHA384:
      assert (cert->key_type == DSK_TLS_X509_KEY_TYPE_RSA_PRIVATE);
      dsk_rsassa_pss_sign (cert->key,
                           &dsk_checksum_type_sha384,
                           384/8,          // length of salt is hash-length
                           NULL,           // default RNG
                           content_len, content_data,
                           signature_out);
      break;
    case DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA512:
    case DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_PSS_SHA512:
      assert (cert->key_type == DSK_TLS_X509_KEY_TYPE_RSA_PRIVATE);
      dsk_rsassa_pss_sign (cert->key,
                           &dsk_checksum_type_sha512,
                           512/8,          // length of salt is hash-length
                           NULL,           // default RNG
                           content_len, content_data,
                           signature_out);
      break;
    default:
      dsk_die ("algorithm not supported by dsk_tls_x509_certificate_sign()");
    }
    
}

static bool
dsk_tls_x509_certificate_verify  (DskTlsKeyPair     *kp,
                                  DskTlsSignatureScheme algorithm,
                                  size_t             content_len,
                                  const uint8_t     *content_data,
                                  const uint8_t     *sig)
{
  DskTlsX509Certificate *cert = (DskTlsX509Certificate *) kp;
  switch (algorithm)
    {
    case DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA256:
      assert (cert->key_type == DSK_TLS_X509_KEY_TYPE_RSA_PRIVATE);
      return dsk_rsassa_pkcs1_5_verify (cert->key,
                                        &dsk_checksum_type_sha256,
                                        content_len, content_data,
                                        sig);
    case DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA384:
      assert (cert->key_type == DSK_TLS_X509_KEY_TYPE_RSA_PRIVATE);
      return dsk_rsassa_pkcs1_5_verify (cert->key,
                                        &dsk_checksum_type_sha384,
                                        content_len, content_data,
                                        sig);
    case DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA512:
      assert (cert->key_type == DSK_TLS_X509_KEY_TYPE_RSA_PRIVATE);
      return dsk_rsassa_pkcs1_5_verify (cert->key,
                                        &dsk_checksum_type_sha512,
                                        content_len, content_data,
                                        sig);
    case DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA256:
    case DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_PSS_SHA256:
      assert (cert->key_type == DSK_TLS_X509_KEY_TYPE_RSA_PRIVATE);
      return dsk_rsassa_pss_verify (cert->key,
                                    &dsk_checksum_type_sha256,
                                    256/8,
                                    content_len, content_data,
                                    sig);

    case DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA384:
    case DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_PSS_SHA384:
      assert (cert->key_type == DSK_TLS_X509_KEY_TYPE_RSA_PRIVATE);
      return dsk_rsassa_pss_verify (cert->key,
                                    &dsk_checksum_type_sha384,
                                    384/8,
                                    content_len, content_data,
                                    sig);

    case DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA512:
    case DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_PSS_SHA512:
      assert (cert->key_type == DSK_TLS_X509_KEY_TYPE_RSA_PRIVATE);
      return dsk_rsassa_pss_verify (cert->key,
                                    &dsk_checksum_type_sha512,
                                    512/8,
                                    content_len, content_data,
                                    sig);

    default:
      dsk_die ("algorithm not supported by dsk_tls_x509_certificate_sign()");
      return false;
    }
    
}

#define dsk_tls_x509_certificate_init NULL

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskTlsX509Certificate);
DskTlsX509CertificateClass dsk_tls_x509_certificate_class =
{
  DSK_TLS_KEY_PAIR_DEFINE_CLASS(
    DskTlsX509Certificate,
    dsk_tls_x509_certificate
  )
};


// 'a' is the pinned client certificate.
// 'b' is the first in the server's certificate chain
//
// This called when the client gets a CertificateVerify message.`
bool dsk_tls_x509_certificates_match (const DskTlsX509Certificate *a,
                                      const DskTlsX509Certificate *b)
{
  const DskTlsX509SubjectPublicKeyInfo *a_pki = &a->subject_public_key_info;
  const DskTlsX509SubjectPublicKeyInfo *b_pki = &b->subject_public_key_info;
  if (a_pki->algorithm != b_pki->algorithm
   || a_pki->public_key_length == b_pki->public_key_length
   || memcmp (a_pki->public_key,
	      b_pki->public_key,
	      a_pki->public_key_length) != 0)
    {
      return false;
    }
  return true;
}
