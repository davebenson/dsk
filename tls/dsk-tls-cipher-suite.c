#include "../dsk.h"

//
//                      -------------------------------
//                             AES128 GCM SHA-256
//                      -------------------------------
//

//
// RFC 5116, Sections 5.1, 5.2.
#define DSK_CIPHER_SUITE_GCM_IV_LEN        12           // N_MIN==N_MAX==12
#define DSK_CIPHER_SUITE_GCM_AUTH_TAG_LEN  16

typedef struct AES128_GCM_CTX AES128_GCM_CTX;
struct AES128_GCM_CTX
{
  DskAES128 cipher;
  Dsk_AEAD_GCM_Precomputation gcm_precompute;
};

static void
aes128_gcm_init     (void           *instance,
                     const uint8_t  *key)
{
  AES128_GCM_CTX *ctx = instance;
  dsk_aes128_init (&ctx->cipher, key);
  dsk_aead_gcm_precompute ((DskBlockCipherInplaceFunc) dsk_aes128_encrypt_inplace,
                           &ctx->cipher, &ctx->gcm_precompute);
}
static void
aes128_gcm_encrypt  (void           *instance,
                     size_t          plaintext_len,
                     const uint8_t  *plaintext,
                     size_t          associated_data_len,
                     const uint8_t  *associated_data,
                     const uint8_t  *iv,
                     uint8_t        *ciphertext,
                     uint8_t        *authentication_tag)
{
  AES128_GCM_CTX *ctx = instance;
  ctx->gcm_precompute.block_cipher_object = &ctx->cipher;
  dsk_aead_gcm_encrypt (&ctx->gcm_precompute, plaintext_len, plaintext,
                        associated_data_len, associated_data,
                        DSK_CIPHER_SUITE_GCM_IV_LEN, iv,
                        ciphertext,
                        DSK_CIPHER_SUITE_GCM_AUTH_TAG_LEN, authentication_tag);
}
static bool
aes128_gcm_decrypt  (void           *instance,
                     size_t          ciphertext_len,
                     const uint8_t  *ciphertext,
                     size_t          associated_data_len,
                     const uint8_t  *associated_data,
                     const uint8_t  *iv,
                     uint8_t        *plaintext,
                     const uint8_t  *authentication_tag)
{
  AES128_GCM_CTX *ctx = instance;
  ctx->gcm_precompute.block_cipher_object = &ctx->cipher;
  return dsk_aead_gcm_decrypt (&ctx->gcm_precompute, ciphertext_len, ciphertext,
                               associated_data_len, associated_data,
                               DSK_CIPHER_SUITE_GCM_IV_LEN, iv,
                               plaintext,
                               DSK_CIPHER_SUITE_GCM_AUTH_TAG_LEN, authentication_tag);
}

DskTlsCipherSuite dsk_tls_cipher_suite_aes128_gcm_sha256 =
{
  "AES128_GCM_SHA256",
  DSK_TLS_CIPHER_SUITE_TLS_AES_128_GCM_SHA256,
  16,                   // key_size
  DSK_CIPHER_SUITE_GCM_IV_LEN,
  DSK_CIPHER_SUITE_GCM_AUTH_TAG_LEN,
  32,                   // hash_size
  sizeof(AES128_GCM_CTX),
  &dsk_checksum_type_sha256,
  aes128_gcm_init,
  aes128_gcm_encrypt,
  aes128_gcm_decrypt,
};

//
//                      -------------------------------
//                             AES256 GCM SHA-384
//                      -------------------------------
//

typedef struct AES256_GCM_CTX AES256_GCM_CTX;
struct AES256_GCM_CTX
{
  DskAES256 cipher;
  Dsk_AEAD_GCM_Precomputation gcm_precompute;
};

static void
aes256_gcm_init     (void           *instance,
                     const uint8_t  *key)
{
  AES256_GCM_CTX *ctx = instance;
  dsk_aes256_init (&ctx->cipher, key);
  dsk_aead_gcm_precompute ((DskBlockCipherInplaceFunc) dsk_aes256_encrypt_inplace,
                           &ctx->cipher, &ctx->gcm_precompute);
}
static void
aes256_gcm_encrypt  (void           *instance,
                     size_t          plaintext_len,
                     const uint8_t  *plaintext,
                     size_t          associated_data_len,
                     const uint8_t  *associated_data,
                     const uint8_t  *iv,
                     uint8_t        *ciphertext,
                     uint8_t        *authentication_tag)
{
  AES256_GCM_CTX *ctx = instance;
  ctx->gcm_precompute.block_cipher_object = &ctx->cipher;
  dsk_aead_gcm_encrypt (&ctx->gcm_precompute, plaintext_len, plaintext,
                        associated_data_len, associated_data,
                        DSK_CIPHER_SUITE_GCM_IV_LEN, iv,
                        ciphertext,
                        DSK_CIPHER_SUITE_GCM_AUTH_TAG_LEN, authentication_tag);
}
static bool
aes256_gcm_decrypt  (void           *instance,
                     size_t          ciphertext_len,
                     const uint8_t  *ciphertext,
                     size_t          associated_data_len,
                     const uint8_t  *associated_data,
                     const uint8_t  *iv,
                     uint8_t        *plaintext,
                     const uint8_t  *authentication_tag)
{
  AES256_GCM_CTX *ctx = instance;
  ctx->gcm_precompute.block_cipher_object = &ctx->cipher;
  return dsk_aead_gcm_decrypt (&ctx->gcm_precompute, ciphertext_len, ciphertext,
                               associated_data_len, associated_data,
                               DSK_CIPHER_SUITE_GCM_IV_LEN, iv,
                               plaintext,
                               DSK_CIPHER_SUITE_GCM_AUTH_TAG_LEN, authentication_tag);
}

DskTlsCipherSuite dsk_tls_cipher_suite_aes256_gcm_sha384 =
{
  "AES256_GCM_SHA384",
  DSK_TLS_CIPHER_SUITE_TLS_AES_256_GCM_SHA384,
  32,                   // key_size
  DSK_CIPHER_SUITE_GCM_IV_LEN,
  DSK_CIPHER_SUITE_GCM_AUTH_TAG_LEN,
  48,                   // hash_size
  sizeof(AES256_GCM_CTX),
  &dsk_checksum_type_sha384,
  aes256_gcm_init,
  aes256_gcm_encrypt,
  aes256_gcm_decrypt,
};

//
//                      -------------------------------
//                             AES128 CCM SHA-256
//                      -------------------------------
//

//
// RFC 5116, Sections 5.1, 5.2.
#define DSK_CIPHER_SUITE_CCM_IV_LEN        12           // N_MIN==N_MAX==12
#define DSK_CIPHER_SUITE_CCM_AUTH_TAG_LEN  16
#define DSK_CIPHER_SUITE_CCM_8_AUTH_TAG_LEN  8

typedef struct AES128_CCM_CTX AES128_CCM_CTX;
struct AES128_CCM_CTX
{
  DskAES128 cipher;
};

static void
aes128_ccm_init     (void           *instance,
                     const uint8_t  *key)
{
  AES128_CCM_CTX *ctx = instance;
  dsk_aes128_init (&ctx->cipher, key);
}
static void
aes128_ccm_encrypt  (void           *instance,
                     size_t          plaintext_len,
                     const uint8_t  *plaintext,
                     size_t          associated_data_len,
                     const uint8_t  *associated_data,
                     const uint8_t  *iv,
                     uint8_t        *ciphertext,
                     uint8_t        *authentication_tag)
{
  AES128_CCM_CTX *ctx = instance;
  dsk_aead_ccm_encrypt ((DskBlockCipherInplaceFunc) dsk_aes128_encrypt_inplace, &ctx->cipher,
                        plaintext_len, plaintext,
                        associated_data_len, associated_data,
                        DSK_CIPHER_SUITE_CCM_IV_LEN, iv,
                        ciphertext,
                        DSK_CIPHER_SUITE_CCM_AUTH_TAG_LEN, authentication_tag);
}
static bool
aes128_ccm_decrypt  (void           *instance,
                     size_t          ciphertext_len,
                     const uint8_t  *ciphertext,
                     size_t          associated_data_len,
                     const uint8_t  *associated_data,
                     const uint8_t  *iv,
                     uint8_t        *plaintext,
                     const uint8_t  *authentication_tag)
{
  AES128_CCM_CTX *ctx = instance;
  return dsk_aead_ccm_decrypt ((DskBlockCipherInplaceFunc) dsk_aes128_encrypt_inplace, &ctx->cipher,
                               ciphertext_len, ciphertext,
                               associated_data_len, associated_data,
                               DSK_CIPHER_SUITE_CCM_IV_LEN, iv,
                               plaintext,
                               DSK_CIPHER_SUITE_CCM_AUTH_TAG_LEN, authentication_tag);
}

DskTlsCipherSuite dsk_tls_cipher_suite_aes128_ccm_sha256 =
{
  "AES128_CCM_SHA256",
  DSK_TLS_CIPHER_SUITE_TLS_AES_128_CCM_SHA256,
  16,
  DSK_CIPHER_SUITE_CCM_IV_LEN,
  DSK_CIPHER_SUITE_CCM_AUTH_TAG_LEN,
  32,
  sizeof(AES128_CCM_CTX),
  &dsk_checksum_type_sha256,
  aes128_ccm_init,
  aes128_ccm_encrypt,
  aes128_ccm_decrypt,
};

static void
aes128_ccm_8_encrypt  (void           *instance,
                       size_t          plaintext_len,
                       const uint8_t  *plaintext,
                       size_t          associated_data_len,
                       const uint8_t  *associated_data,
                       const uint8_t  *iv,
                       uint8_t        *ciphertext,
                       uint8_t        *authentication_tag)
{
  AES128_CCM_CTX *ctx = instance;
  dsk_aead_ccm_encrypt ((DskBlockCipherInplaceFunc) dsk_aes128_encrypt_inplace, &ctx->cipher,
                        plaintext_len, plaintext,
                        associated_data_len, associated_data,
                        DSK_CIPHER_SUITE_CCM_IV_LEN, iv,
                        ciphertext,
                        DSK_CIPHER_SUITE_CCM_8_AUTH_TAG_LEN, authentication_tag);
}
static bool
aes128_ccm_8_decrypt  (void           *instance,
                       size_t          ciphertext_len,
                       const uint8_t  *ciphertext,
                       size_t          associated_data_len,
                       const uint8_t  *associated_data,
                       const uint8_t  *iv,
                       uint8_t        *plaintext,
                       const uint8_t  *authentication_tag)
{
  AES128_CCM_CTX *ctx = instance;
  return dsk_aead_ccm_decrypt ((DskBlockCipherInplaceFunc) dsk_aes128_encrypt_inplace, &ctx->cipher,
                               ciphertext_len, ciphertext,
                               associated_data_len, associated_data,
                               DSK_CIPHER_SUITE_CCM_IV_LEN, iv,
                               plaintext,
                               DSK_CIPHER_SUITE_CCM_8_AUTH_TAG_LEN, authentication_tag);
}

DskTlsCipherSuite dsk_tls_cipher_suite_aes128_ccm_8_sha256 =
{
  "AES128_CCM_8_SHA256",
  DSK_TLS_CIPHER_SUITE_TLS_AES_128_CCM_8_SHA256,
  16,
  DSK_CIPHER_SUITE_CCM_IV_LEN,
  DSK_CIPHER_SUITE_CCM_8_AUTH_TAG_LEN,
  32,
  sizeof(AES128_CCM_CTX),
  &dsk_checksum_type_sha256,
  aes128_ccm_init,
  aes128_ccm_8_encrypt,
  aes128_ccm_8_decrypt,
};

