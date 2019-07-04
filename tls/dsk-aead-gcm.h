/* AEAD = Authenticated Encryption with Associated Data.  RFC 5116.
 *
 * A pair of functions that encrypt and sign data;
 * or decrypt and verify the signature.  In addition to a plaintext,
 * which will be fully encrypted, there's also associated data that will
 * be signed but not encrypted.
 *
 * This functions are for the Galois Counter Mode (GCM) implementation of AEAD.
 */

/* Although the algorithm doesn't restrict IV-length,
 * RFC 5116 forces it to 12 bytes.  (Section 5.1)
 */
#define DSK_AEAD_GCM_MIN_IV_LENGTH              12
#define DSK_AEAD_GCM_MAX_IV_LENGTH              12

// key-dependent pre-computations.
typedef struct {
  void *block_cipher_object;
  DskBlockCipherInplaceFunc block_cipher;

  uint32_t h_v_table[128 * 4];
} Dsk_AEAD_GCM_Precomputation;
/*
 * inplace_cipher_func: a cipher with block-size of 128-bits==16 bytes,
 *   this is the encrypt function.
 * inplace_cipher_object: first param to inplace_cipher_func.
 */
void dsk_aead_gcm_precompute (DskBlockCipherInplaceFunc block_cipher_func,
                              void *block_cipher_object,
                              Dsk_AEAD_GCM_Precomputation *out);
/* cipher_precomputation: ...
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

