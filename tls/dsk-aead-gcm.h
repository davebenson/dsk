typedef void (*DskBlockCipherInplaceFunc) (void *cipher_object,
                                           uint8_t *inout);


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
void dsk_aead_gcm_encrypt (DskBlockCipherInplaceFunc inplace_cipher_func,
                           void                     *inplace_cipher_object,
                           size_t                     plaintext_len,
                           const uint8_t             *plaintext,
                           size_t                     associated_data_len,
                           const uint8_t             *associated_data,
                           size_t                     iv_len,
                           const uint8_t             *iv,
                           uint8_t                   *ciphertext,
                           size_t                     authentication_tag_len,
                           uint8_t                   *authentication_tag);

