#include "../dsk.h"
#include <string.h>

static bool
parse_algo_id (DskASN1Value *algorithm_id,
               DskTlsSignatureScheme *scheme_out,
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
  if (!dsk_tls_oid_to_signature_scheme (oid, algo_params, scheme_out))
    {
      *error = dsk_error_new ("signature-scheme not known for OID");
      return false;
    }
  return true;
}

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
      if (names[i]->type != DSK_ASN1_TYPE_SEQUENCE
       || names[i]->v_sequence.n_children != 2
       || names[i]->v_sequence.children[0]->type != DSK_ASN1_TYPE_OBJECT_IDENTIFIER)
        {
          *error = dsk_error_new ("each distinguished-name must a sequence of OID and Name");
          goto failed;
        }
      const DskTlsObjectID *oid = names[i]->v_sequence.children[0]->v_object_identifier;
      DskASN1Value *name_value = names[i]->v_sequence.children[1];
      if (!dsk_tls_oid_to_x509_distinguished_name_type (oid, &dn[i].type))
        {
          *error = dsk_error_new ("distinguished-name type has unrecognied OID");
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
  return true;

failed:
  for (unsigned j = 0; j < i; j++)
    dsk_free (dn[j].name);
  dsk_free (dn);
  return false;
}

static void
dsk_tls_x509_name_clear (DskTlsX509Name *name)
{
  for (unsigned i = 0; i < name->n_distinguished_names; i++)
    dsk_free (name->distinguished_names[i].name);
  dsk_free (name->distinguished_names);
}

static bool
parse_validity (DskASN1Value        *value,
                DskTlsX509Validity  *validity_out,
                DskError           **error)
{
  if (value->type != DSK_ASN1_TYPE_SEQUENCE
   || value->v_sequence.n_children != 0
   || value->v_sequence.children[0]->type != DSK_ASN1_TYPE_INTEGER
   || value->v_sequence.children[1]->type != DSK_ASN1_TYPE_INTEGER)
    {
      *error = dsk_error_new ("Validity portion of x509 cert invalid");
      return false;
    }
  validity_out->not_before = value->v_sequence.children[0]->v_integer;
  validity_out->not_after = value->v_sequence.children[1]->v_integer;
  return true;
}
#define dsk_tls_x509_validity_clear(v)


static bool
parse_subject_public_key_info (DskASN1Value        *value,
                               DskTlsX509SubjectPublicKeyInfo  *spki_out,
                               DskError           **error)
{
  if (value->type != DSK_ASN1_TYPE_SEQUENCE
   || value->v_sequence.n_children != 2)
    {
      *error = dsk_error_new ("SubjectPublicKeyInfo must be a sequence of length 2");
      return false;
    }
  if (!parse_algo_id (value->v_sequence.children[0], &spki_out->algorithm, error))
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
                                             DskMemPool   *tmp_pool,
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
      if (!dsk_asn1_value_expand_tag (version_tag, tmp_pool,
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
  DskTlsX509Certificate *rv = DSK_NEW0 (DskTlsX509Certificate);
  unsigned sn_len = serial_number->value_end - serial_number->value_start;
  unsigned sn_pad = 20 - sn_len;
  memset (rv->serial_number, 0, sn_pad);
  memcpy (rv->serial_number + sn_pad, serial_number->value_start, sn_len);

  DskASN1Value *algorithm_id = value->v_sequence.children[tbs_at++];
  if (!parse_algo_id (algorithm_id, &rv->signature_scheme, error))
    {
      dsk_tls_x509_certificate_free (rv);
      return NULL;
    }
  DskASN1Value *issuer_value = value->v_sequence.children[tbs_at++];
  if (!parse_name (issuer_value, &rv->issuer, error))
    {
      dsk_tls_x509_certificate_free (rv);
      return NULL;
    }
  DskASN1Value *validity_value = value->v_sequence.children[tbs_at++];
  if (!parse_validity (validity_value, &rv->validity, error))
    {
      dsk_tls_x509_certificate_free (rv);
      return NULL;
    }
  DskASN1Value *subject_value = value->v_sequence.children[tbs_at++];
  if (!parse_name (subject_value, &rv->subject, error))
    {
      dsk_tls_x509_certificate_free (rv);
      return NULL;
    }
  DskASN1Value *subpki_value = value->v_sequence.children[tbs_at++];
  if (!parse_subject_public_key_info (subpki_value, &rv->subject_public_key_info, error))
    {
      dsk_tls_x509_certificate_free (rv);
      return false;
    }

  DskASN1Value *tmp = tbs_at < value->v_sequence.n_children ? value->v_sequence.children[tbs_at] : NULL;
  if (tmp != NULL && tmp->type == DSK_ASN1_TYPE_CONTEXT_SPECIFIC_1)
    {
      if (!dsk_asn1_value_expand_tag (tmp, tmp_pool,
                                      DSK_ASN1_TYPE_BIT_STRING, false,
                                      error))
        {
          dsk_tls_x509_certificate_free (rv);
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
      if (!dsk_asn1_value_expand_tag (tmp, tmp_pool,
                                      DSK_ASN1_TYPE_BIT_STRING, false,
                                      error))
        {
          dsk_tls_x509_certificate_free (rv);
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
      if (!dsk_asn1_value_expand_tag (tmp, tmp_pool,
                                      DSK_ASN1_TYPE_SEQUENCE, false,
                                      error))
        {
          dsk_tls_x509_certificate_free (rv);
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
              dsk_tls_x509_certificate_free (rv);
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
                                    DskMemPool   *tmp_pool,
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
  DskTlsX509Certificate *rv = dsk_tls_x509_unsigned_certificate_from_asn1 (tbs_cert, tmp_pool, error);
  if (rv == NULL)
    return NULL;
  DskTlsSignatureScheme scheme;
  if (!parse_algo_id (cert_sig_algo, &scheme, error))
    {
      dsk_tls_x509_certificate_free (rv);
      return NULL;
    }
  if (scheme != rv->signature_scheme)
    {
      *error = dsk_error_new ("signature algorithms not consisent in certificate");
      dsk_tls_x509_certificate_free (rv);
      return NULL;
    }
  if (sig->type != DSK_ASN1_TYPE_BIT_STRING
   || sig->v_bit_string.length % 8 != 0)
    {
      *error = dsk_error_new ("signature must be a number of bytes");
      dsk_tls_x509_certificate_free (rv);
      return NULL;
    }
  rv->is_signed = true;
  rv->signature_length = sig->v_bit_string.length / 8;
  rv->signature_data = dsk_memdup(rv->signature_length, sig->v_bit_string.bits);
  return rv;
}


void
dsk_tls_x509_certificate_free (DskTlsX509Certificate *cert)
{
  dsk_tls_x509_name_clear (&cert->issuer);
  dsk_tls_x509_validity_clear (&cert->validity);
  dsk_tls_x509_name_clear (&cert->subject);
  dsk_tls_x509_subject_public_key_info_clear (&cert->subject_public_key_info);
  dsk_free (cert->issuer_unique_id);
  dsk_free (cert->subject_unique_id);
  dsk_free (cert->signature_data);
  dsk_free (cert);
}
