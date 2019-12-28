
typedef struct DskTlsCipherSuite DskTlsCipherSuite;
struct DskTlsCipherSuite
{
  const char *name;
  DskTlsCipherSuiteCode code;

  unsigned key_size;
  unsigned iv_size;
  unsigned auth_tag_size;
  unsigned hash_size;
  size_t instance_size;
  DskChecksumType *hash_type;    // only used for key-schedule computation

  void (*init)      (void           *instance,
                     bool            for_encryption,
                     const uint8_t  *key);
  void (*encrypt)   (void           *instance,
                     size_t          plaintext_len,
                     const uint8_t  *plaintext,
                     size_t          associated_data_len,
                     const uint8_t  *associated_data,
                     const uint8_t  *iv,
                     uint8_t        *ciphertext,
                     uint8_t        *authentication_tag);
  bool (*decrypt)   (void           *instance,
                     size_t          ciphertext_len,
                     const uint8_t  *ciphertext,
                     size_t          associated_data_len,
                     const uint8_t  *associated_data,
                     const uint8_t  *iv,
                     uint8_t        *plaintext,
                     const uint8_t  *authentication_tag);
};

extern DskTlsCipherSuite dsk_tls_cipher_suite_aes128_gcm_sha256;
extern DskTlsCipherSuite dsk_tls_cipher_suite_aes256_gcm_sha384;
extern DskTlsCipherSuite dsk_tls_cipher_suite_aes128_ccm_sha256;
extern DskTlsCipherSuite dsk_tls_cipher_suite_aes128_ccm_8_sha256;
extern DskTlsCipherSuite dsk_tls_cipher_suite_chacha20_poly1305_sha256;

DskTlsCipherSuite *dsk_tls_cipher_suite_by_code (uint16_t code);
