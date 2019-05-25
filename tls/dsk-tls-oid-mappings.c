#include "../dsk.h"
#include <stdio.h>
//
// See https://www.cryptosys.net/pki/manpki/pki_distnames.html
//
bool
dsk_tls_oid_to_x509_distinguished_name_type (const DskTlsObjectID *oid,
                                             DskTlsX509DistinguishedNameType *out)
{
  printf("dsk_tls_oid_to_x509_distinguished_name_type: oid=%p\n",oid);
  if (oid->n_subids == 4)
    {
      if (oid->subids[0] == 2
       && oid->subids[1] == 5
       && oid->subids[2] == 4)
        {
          switch (oid->subids[3])
            {
            case 3:
              *out = DSK_TLS_X509_DN_COMMON_NAME;               /* CN */
              return true;
            case 4:
              *out = DSK_TLS_X509_DN_SURNAME;                   /* SN */
              return true;
            case 5:
              *out = DSK_TLS_X509_DN_SERIAL_NUMBER;              /* SERIALNUMBER */
              return true;
            case 6:
              *out = DSK_TLS_X509_DN_COUNTRY;              /* C */
              return true;
            case 7:
              *out = DSK_TLS_X509_DN_LOCALITY;             /* L */
              return true;
            case 8:
              *out = DSK_TLS_X509_DN_STATE_OR_PROVINCE;    /* S or ST */
              return true;
            case 9:
              *out = DSK_TLS_X509_DN_STREET_ADDRESS;            /* STREET */
              return true;
            case 10:
              *out = DSK_TLS_X509_DN_ORGANIZATION_NAME;         /* O */
              return true;
            case 11:
              *out = DSK_TLS_X509_DN_ORGANIZATIONAL_UNIT;       /* OU */
              return true;
            case 12:
              *out = DSK_TLS_X509_DN_TITLE;                     /* T or TITLE */
              return true;
            case 42:
              *out = DSK_TLS_X509_DN_GIVEN_NAME;                /* G or GN */
              return true;
            }
        }
    }
  else if (oid->n_subids == 7)
    {
      if (dsk_tls_object_ids_equal (dsk_tls_object_id__dn_email_address, oid))
        {
          *out = DSK_TLS_X509_DN_EMAIL_ADDRESS;
          return true;
        }
      if (dsk_tls_object_ids_equal (dsk_tls_object_id__dn_user_id, oid))
        {
          *out = DSK_TLS_X509_DN_USER_ID;
          return true;
        }
      if (dsk_tls_object_ids_equal (dsk_tls_object_id__dn_domain_component, oid))
        {
          *out = DSK_TLS_X509_DN_DOMAIN_COMPONENT;
          return true;
        }
    }
  return false;
}

//
//  https://www.itu.int/ITU-T/formal-language/itu-t/x/x509/2016/AlgorithmObjectIdentifiers.html
//


bool
dsk_tls_oid_to_checksum_type (const DskTlsObjectID *oid,
                      DskChecksumType *out)
{
  if (oid->n_subids == 6)
    {
      if (dsk_tls_object_ids_equal (oid, dsk_tls_object_id__hash_sha1))
        {
          *out = DSK_CHECKSUM_SHA1;
          return true;
        }
      return false;
    }
  else if (oid->n_subids == 10)
    {
      if (oid->subids[0] == 1
       && oid->subids[1] == 3
       && oid->subids[2] == 6
       && oid->subids[3] == 1
       && oid->subids[4] == 4
       && oid->subids[5] == 1
       && oid->subids[6] == 1722
       && oid->subids[7] == 12
       && oid->subids[8] == 2)
        {
          switch (oid->subids[9])
            {
            case 1:
              *out = DSK_CHECKSUM_SHA256;
              return true;
            case 2:
              *out = DSK_CHECKSUM_SHA384;
              return true;
            case 3:
              *out = DSK_CHECKSUM_SHA512;
              return true;
            case 4:
              *out = DSK_CHECKSUM_SHA224;
              return true;
            case 5:
              *out = DSK_CHECKSUM_SHA512_224;
              return true;
            case 6:
              *out = DSK_CHECKSUM_SHA512_256;
              return true;
            default:
              return false;
            }
        }
    }
  return false;
}

/*
  RSASSA-PSS-params  ::=  SEQUENCE  {
    hashAlgorithm     [0] HashAlgorithm DEFAULT sha1Identifier,
    maskGenAlgorithm  [1] MaskGenAlgorithm DEFAULT mgf1SHA1Identifier,
    saltLength        [2] INTEGER DEFAULT 20,
    trailerField      [3] INTEGER DEFAULT 1 
  }

  The MaskGenAlgorithm has format same as algorithm identifier,
  but algorithm is always 1.2.840.113549.1.1.8 (MaskGenFunction1)
  and the parameters field is an object ID for a hash-function:
       1.3.14.3.2.26           == SHA1
       1.3.6.1.4.1.1722.12.2.1 == SHA256
       1.3.6.1.4.1.1722.12.2.2 == SHA384
       1.3.6.1.4.1.1722.12.2.3 == SHA512
       1.3.6.1.4.1.1722.12.2.4 == SHA224
       1.3.6.1.4.1.1722.12.2.5 == SHA512-224
       1.3.6.1.4.1.1722.12.2.6 == SHA512-256

  The saltLength for the TLS 1.3 formats (see RFC 8446)
  must be equal to the digest-length.

    https://crypto.stackexchange.com/questions/58680/whats-the-difference-between-rsa-pss-pss-and-rsa-pss-rsae-schemes
 */
static bool
parse_rsapss_parameters (const DskASN1Value *algo_params,
                         DskMemPool *tmp_pool,
                         DskTlsSignatureScheme *out,
                         DskError **error)
{
  DskChecksumType hash = DSK_CHECKSUM_SHA1;
  unsigned n = algo_params->v_sequence.n_children;
  unsigned salt_length = 20, trailer_field = 1;
  unsigned i = 0;
  DskASN1Value **children = algo_params->v_sequence.children;
  if (i < n && children[i]->type == DSK_ASN1_TYPE_CONTEXT_SPECIFIC_0)
    {
      // hashAlgorithm. sequence(tls_object_id, params)
      if (!dsk_asn1_value_expand_tag (children[i], tmp_pool,
                                      DSK_ASN1_TYPE_SEQUENCE,
                                      true, error))
        {
          return false;
        }
      DskASN1Value *hash_algo = children[i]->v_tagged.subvalue;
      if (hash_algo->v_sequence.n_children < 1
       || hash_algo->v_sequence.n_children > 2
       || hash_algo->v_sequence.children[0]->type != DSK_ASN1_TYPE_OBJECT_IDENTIFIER)
        {
          *error = dsk_error_new ("expected OID for hash-algo");
          return false;
        }
      if (!dsk_tls_oid_to_checksum_type (hash_algo->v_sequence.children[0]->v_object_identifier, &hash))
        {
          *error = dsk_error_new ("unknown checksum type");
          return false;
        }
      i++;
    }
  if (i < n && children[i]->type == DSK_ASN1_TYPE_CONTEXT_SPECIFIC_1)
    {
      // maskGenAlgorithm. sequence(tls_object_id, params)
      DskASN1Value *mga_value = children[i];
      if (!dsk_asn1_value_expand_tag (mga_value, tmp_pool,
                                      DSK_ASN1_TYPE_SEQUENCE, true,
                                      error))
        {
          return false;
        }
      mga_value = mga_value->v_tagged.subvalue;
      if (mga_value->type != DSK_ASN1_TYPE_SEQUENCE
       || mga_value->v_sequence.n_children == 0
       || mga_value->v_sequence.n_children > 0
       || mga_value->v_sequence.children[0]->type != DSK_ASN1_TYPE_OBJECT_IDENTIFIER
       || !dsk_tls_object_ids_equal (mga_value->v_sequence.children[0]->v_object_identifier,
                                     dsk_tls_object_id__maskgenerationfunction_1))
        {
          return false;
        }

      //
      // Must match maskGenAlgorithm_HASH where HASH matches
      // DskChecksumType.
      //
      //TODO
      i++;
    }
  if (i < n && children[i]->type == DSK_ASN1_TYPE_CONTEXT_SPECIFIC_2)
    {
      // salt_length. (integer)
      if (!dsk_asn1_value_expand_tag (children[i], tmp_pool,
                                      DSK_ASN1_TYPE_INTEGER, true,
                                      error))
        return false;
      salt_length = children[i]->v_integer;
      i++;
    }
  if (i < n && children[i]->type == DSK_ASN1_TYPE_CONTEXT_SPECIFIC_3)
    {
      // trailer_field. (integer)   must be 1.
      // See RFC 4055 3.1 "trailerField".
      if (!dsk_asn1_value_expand_tag (children[i], tmp_pool,
                                      DSK_ASN1_TYPE_INTEGER, true,
                                      error))
        return false;
      trailer_field = children[i]->v_integer;
      i++;
    }

  if (i < n)
    {
      *error = dsk_error_new ("unexpected field at end of RSAPSS parameters");
      return false;
    }

  if (dsk_checksum_type_get_size (hash) != salt_length)
    {
      *error = dsk_error_new ("checksum size does not match salt length");
      return false;
    }
  if (trailer_field != 1)
    {
      *error = dsk_error_new ("trailerField is not 1");
      return false;
    }

  switch (hash)
    {
    case DSK_CHECKSUM_SHA256:
      *out = DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_PSS_SHA256;
      return true;
    case DSK_CHECKSUM_SHA384:
      *out = DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_PSS_SHA384;
      return true;
    case DSK_CHECKSUM_SHA512:
      *out = DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_PSS_SHA512;
      return true;
    default:
      *error = dsk_error_new ("checksum type not allowed for RSASSA_PSS");
      return false;
    }
}

bool
dsk_tls_oid_to_signature_scheme (const DskTlsObjectID *oid, 
                                 DskMemPool *tmp_pool,
                                 const DskASN1Value   *algo_params,
                                 DskTlsSignatureScheme *out,
                                 DskError **error)
{
  if (oid->n_subids == 7)
    {
      if (oid->subids[0] == 1
       && oid->subids[1] == 2
       && oid->subids[2] == 840)
        {
          if (oid->subids[3] == 113549
           && oid->subids[4] == 1
           && oid->subids[5] == 1)
            {
              switch (oid->subids[6])
                {
                //case 4:
                case 5:
                  *out = DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA1;
                  return true;
                case 11:
                  *out = DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA256;
                  return true;
                case 12:
                  *out = DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA384;
                  return true;
                case 13:
                  *out = DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA512;
                  return true;
                case 10:
                  /* RSAPSS depends on the Parameters field */
                  return parse_rsapss_parameters (algo_params, tmp_pool, out, error);
                }
            }
        }
    }
       
//  [ "signature_rsapss",            "1.2.840.113549.1.1.10"  ],
//  [ "signature_dsa_with_sha1",     "1.2.840.10040.4.3"      ],
//  [ "signature_dsa_with_sha256",   "2.16.840.1.101.3.4.3.2" ],
//  [ "signature_ecdsa_with_sha1",   "1.2.840.10045.4.1"      ],
//  [ "signature_ecdsa_with_sha256", "1.2.840.10045.4.3.2"    ],
//  [ "signature_ecdsa_with_sha384", "1.2.840.10045.4.3.3"    ],
//  [ "signature_ecdsa_with_sha512", "1.2.840.10045.4.3.4"    ],
//  [ "signature_ed25519",           "1.3.101.112"            ],
  char *oid_str = dsk_tls_object_id_to_string(oid);
  *error = dsk_error_new ("unknown signature ObjectID: %s", oid_str);
  dsk_free (oid_str);
  return false;
}
const DskTlsObjectID *
dsk_tls_signature_scheme_get_tls_object_id (DskTlsSignatureScheme scheme,
                                  size_t *params_length_out,
                                  const uint8_t **params_data_out)
{
  *params_length_out = 1;
  *params_data_out = (uint8_t *) "\5";              // encoding for NULL
  switch (scheme)
    {
    case DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA1:
      return dsk_tls_object_id__signature_sha1_rsa;
    case DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA256:
      return dsk_tls_object_id__signature_sha256_rsa;
    case DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA384:
      return dsk_tls_object_id__signature_sha384_rsa;
    case DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA512:
      return dsk_tls_object_id__signature_sha512_rsa;

    /* ECDSA algorithms */
    case DSK_TLS_SIGNATURE_SCHEME_ECDSA_SHA1:
      return dsk_tls_object_id__signature_ecdsa_with_sha1;
    case DSK_TLS_SIGNATURE_SCHEME_ECDSA_SECP256R1_SHA256:
      return dsk_tls_object_id__signature_ecdsa_with_sha256;
    case DSK_TLS_SIGNATURE_SCHEME_ECDSA_SECP384R1_SHA384:
      return dsk_tls_object_id__signature_ecdsa_with_sha384;
    case DSK_TLS_SIGNATURE_SCHEME_ECDSA_SECP521R1_SHA512:
      return dsk_tls_object_id__signature_ecdsa_with_sha512;
  
    /* RSASSA-PSS algorithms with public key OID rsaEncryption */
//    case DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA256:
//      ...
//    case DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA384:
//      ...
//    case DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA512:
//      ...
  
    /* EdDSA algorithms */
    case DSK_TLS_SIGNATURE_SCHEME_ED25519:
      return dsk_tls_object_id__signature_ed25519;
//    case DSK_TLS_SIGNATURE_SCHEME_ED448:
//      ...
  
    /* RSASSA-PSS algorithms with public key OID RSASSA-PSS */
//    case DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_SHA256:
//      *params_length_out = ...;
//      *params_data_out = ...;
//      return dsk_tls_object_id__signature_rsapss;
//    case DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_SHA384:
//      *params_length_out = ...;
//      *params_data_out = ...;
//      return dsk_tls_object_id__signature_rsapss;
//    case DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_SHA512:
//      *params_length_out = ...;
//      *params_data_out = ...;
//      return dsk_tls_object_id__signature_rsapss;

    default:
      return NULL;
    }

}
