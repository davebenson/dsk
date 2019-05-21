
bool dsk_tls_oid_to_signature_scheme (const DskOID *oid, DskTlsSignatureScheme *out)
{

}
const DskOID * dsk_tls_signature_scheme_get_oid (DskTlsSignatureScheme scheme)
{
  switch (scheme)
    {
    case DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA1:
      return dsk_oid__signature_sha1_rsa;
    case DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA256:
      return dsk_oid__signature_sha256_rsa;
    case DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA384:
      return dsk_oid__signature_sha384_rsa;
    case DSK_TLS_SIGNATURE_SCHEME_RSA_PKCS1_SHA512:
      return dsk_oid__signature_sha512_rsa;

    /* ECDSA algorithms */
    case DSK_TLS_SIGNATURE_SCHEME_ECDSA_SHA1:
      return dsk_oid__signature_ecdsa_with_sha1;
    case DSK_TLS_SIGNATURE_SCHEME_ECDSA_SECP256R1_SHA256:
      return dsk_oid__signature_ecdsa_with_sha256;
    case DSK_TLS_SIGNATURE_SCHEME_ECDSA_SECP384R1_SHA384:
      return dsk_oid__signature_ecdsa_with_sha384;
    case DSK_TLS_SIGNATURE_SCHEME_ECDSA_SECP521R1_SHA512:
      return dsk_oid__signature_ecdsa_with_sha512;
  
    /* RSASSA-PSS algorithms with public key OID rsaEncryption */
    case DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA256:
      ...
    case DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA384:
      ...
    case DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA512:
      ...
  
    /* EdDSA algorithms */
    case DSK_TLS_SIGNATURE_SCHEME_ED25519:
      return dsk_oid__signature_ed25519;
    case DSK_TLS_SIGNATURE_SCHEME_ED448:
      ...
  
    /* RSASSA-PSS algorithms with public key OID RSASSA-PSS */
    case DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_SHA256:
      ...
    case DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_SHA384:
      ...
    case DSK_TLS_SIGNATURE_SCHEME_RSASSA_PSS_SHA512:
      ...

  default:
    return NULL;

}
