

void dsk_tls_ecdsa_sign   (const DskTls_ECPrime_Group *group,
                           size_t                      content_hash_number_length,
                           const uint32_t             *content_hash_number_data,
                           uint32_t                   *signature_out);
bool dsk_tls_ecdsa_verify (const DskTls_ECPrime_Group *group,
                           size_t                      content_hash_number_length,
                           const uint32_t             *content_hash_number_data
                           const uint32_t             *signature_out);
