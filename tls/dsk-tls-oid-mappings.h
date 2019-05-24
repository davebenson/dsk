

bool dsk_tls_oid_to_checksum_type (const DskTlsObjectID *oid, DskChecksumType *out);
const DskTlsObjectID *dsk_tls_checksum_to_oid (DskChecksumType type);

//
// Object-ID to SignatureScheme.
//
bool dsk_tls_oid_to_signature_scheme (const DskTlsObjectID *oid, 
                                      DskMemPool *tmp_pool,
                                      const DskASN1Value   *algo_params,
                                      DskTlsSignatureScheme *out,
                                      DskError **error);
const DskTlsObjectID *
     dsk_tls_signature_scheme_get_oid (DskTlsSignatureScheme scheme,
                                       size_t *params_length_out,
                                       const uint8_t **params_data_out);


//
// Object-ID to DskTlsX509DistinguishedNameType
bool dsk_tls_oid_to_x509_distinguished_name_type (const DskTlsObjectID *oid,
                                                  DskTlsX509DistinguishedNameType *name_type_out);
