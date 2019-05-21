


//
// Object-ID to SignatureScheme.
//
bool dsk_tls_oid_to_signature_scheme (const DskOID *oid, DskTlsSignatureScheme *out);
const DskOID * dsk_tls_signature_scheme_get_oid (DskTlsSignatureScheme scheme);
