/* AEAD = Authenticated Encryption with Associated Data.  RFC 5116.
 *
 * A pair of functions that encrypt and sign data;
 * or decrypt and verify the signature.  In addition to a plaintext,
 * which will be fully encrypted, there's also associated data that will
 * be signed but not encrypted.
 */

// key-dependent pre-computations.
typedef struct {
  void *block_cipher_object;
  DskBlockCipherInplaceFunc block_cipher;

  uint32_t h_v_table[128 * 4];
} Dsk_AEAD_GCM_Precomputation;
void dsk_aead_gcm_precompute (DskBlockCipherInplaceFunc cipher_func,
                              void *cipher_object,
                              Dsk_AEAD_GCM_Precomputation *out);

/*
 * inplace_cipher_func: a cipher with block-size of 128-bits==16 bytes.
 * inplace_cipher_object: first param to inplace_cipher_func.
 * plaintext_len: length of plaintext in bytes
 * plaintext: unencrypted plaintext
 * associated_data_len:
 * associated_data:
 * iv_len: initial-value data length in bytes
 * iv_data: initial-value data
 * ciphertext: output, same length as plaintext.
 * authentication_tag_len:
 * authentication_tag:
 */
void dsk_aead_gcm_encrypt (Dsk_AEAD_GCM_Precomputation *cipher_precomputation,
                           size_t                     plaintext_len,
                           const uint8_t             *plaintext,
                           size_t                     associated_data_len,
                           const uint8_t             *associated_data,
                           size_t                     iv_len,
                           const uint8_t             *iv,
                           uint8_t                   *ciphertext,
                           size_t                     authentication_tag_len,
                           uint8_t                   *authentication_tag);

bool dsk_aead_gcm_decrypt (Dsk_AEAD_GCM_Precomputation *precompute,
                           size_t                     ciphertext_len,
                           const uint8_t             *ciphertext,
                           size_t                     associated_data_len,
                           const uint8_t             *associated_data,
                           size_t                     iv_len,
                           const uint8_t             *iv,
                           uint8_t                   *plaintext,
                           size_t                     authentication_tag_len,
                           const uint8_t             *authentication_tag);

