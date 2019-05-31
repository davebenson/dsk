
struct DskTlsCipherSuite
{
  const char *name;

  unsigned key_length;
  unsigned min_iv_length;
  unsigned max_iv_length;
  unsigned min_auth_tag_length;
  unsigned max_auth_tag_length;
  unsigned tls_iv_length;               // MAX(8, min_iv_length)
  unsigned tls_auth_tag_length;
  size_t instance_size;
  DskChecksumType *hash_type;    // only used for key-schedule computation

  void (*init)      (void           *instance,
                     const uint8_t  *key);
  void (*encrypt)   (void           *instance,
                     size_t          plaintext_len,
                     const uint8_t  *plaintext,
                     size_t          associated_data_len,
                     const uint8_t  *associated_data,
                     size_t          iv_len,
                     const uint8_t  *iv,
                     uint8_t        *ciphertext,
                     size_t          authentication_tag_len,
                     uint8_t        *authentication_tag);
  bool (*decrypt)   (void           *instance,
                     size_t          ciphertext_len,
                     const uint8_t  *ciphertext,
                     size_t          associated_data_len,
                     const uint8_t  *associated_data,
                     size_t          iv_len,
                     const uint8_t  *iv,
                     uint8_t        *plaintext,
                     size_t          authentication_tag_len,
                     const uint8_t  *authentication_tag);
};
