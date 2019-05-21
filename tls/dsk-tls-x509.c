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

  const DskOID *oid = algorithm_id->v_sequence.children[0]->v_object_identifier;
  if (!dsk_tls_oid_to_signature_scheme (oid, scheme_out))
    {
      *error = dsk_error_new ("signature-scheme not known for OID");
      return false;
    }
  return true;
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
  if (!parse_subject_public_key_info (subpki_value, &subject_public_key_info, error))
    {
      dsk_tls_x509_name_clear (&issuer);
      dsk_tls_x509_name_clear (&subject);
      return false;
    }

  DskASN1Value *tmp = tbs_at < value->v_sequence.n_children ? value->v_sequence.children[tbs_at] : NULL;
  if (tmp != NULL && tmp->type == DSK_ASN1_TYPE_CONTEXT_SPECIFIC_1)
    {
      ... handle optional issuer unique id

      tbs_at++;
      tmp = tbs_at < value->v_sequence.n_children ? value->v_sequence.children[tbs_at] : NULL;
    }

  if (tmp != NULL && tmp->type == DSK_ASN1_TYPE_CONTEXT_SPECIFIC_2)
    {
      ... handle optional subject unique id

      tbs_at++;
      tmp = tbs_at < value->v_sequence.n_children ? value->v_sequence.children[tbs_at] : NULL;
    }
  if (tmp != NULL && tmp->type == DSK_ASN1_TYPE_CONTEXT_SPECIFIC_3)
    {
      ... handle extensions

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
  DskTlsX509Certificate *rv = dsk_tls_x509_unsigned_certificate_from_asn1 (tbs_sert, tmp_pool, error);
  if (rv == NULL)
    return NULL;
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
  rv->signature_data = dsk_memdup(rv->signature_length, sig->v_bit_string.data);
  return rv;
}

