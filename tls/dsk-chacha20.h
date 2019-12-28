/*
 * RFC 7539.  Chacha-20 and Poly 1305.
 *
 * Stream cipher with authentication from Daniel Bernstein.
 */

void
dsk_chacha20_block_128 (const uint32_t *key,                  // length 4
                        uint32_t        counter,
                        const uint32_t *nonce,                // length 3
                        uint8_t        *out                   // length 64
                       );
void
dsk_chacha20_block_256 (const uint32_t *key,                  // length 8
                        uint32_t        counter,
                        const uint32_t *nonce,                // length 3
                        uint8_t        *out                   // length 64
                       );


// decrypt and encrypt are the same.
//
// NOTE: in both crypt functions, in==out is allowed.
void
dsk_chacha20_crypt_128 (const uint32_t *key,                  // length 4
                        uint32_t        counter,
                        const uint32_t *nonce,                // length 3
                        size_t          length,
                        const uint8_t  *in,
                        uint8_t        *out);
void
dsk_chacha20_crypt_256 (const uint32_t *key,                  // length 8
                        uint32_t        counter,
                        const uint32_t *nonce,                // length 3
                        size_t          length,
                        const uint8_t  *in,
                        uint8_t        *out);

void
dsk_poly1305_key_gen (const uint32_t *key,                    // length 8
                      const uint32_t *nonce,                  // length 3
                      uint32_t       *poly1305_key_out        // length 32
                     );
void
dsk_poly1305_mac     (const uint32_t *key,                    // length 8
                      size_t          message_length,
                      const uint8_t  *message,
                      uint8_t        *mac_out                 // length 16
                     );


#define DSK_AEAD_CHACHA20_POLY1305_IV_LEN            12
#define DSK_AEAD_CHACHA20_POLY1305_AUTH_TAG_LEN      16
void dsk_aead_chacha20_poly1305_encrypt (const uint32_t            *key,
                                         size_t                     plaintext_len,
                                         const uint8_t             *plaintext,
                                         size_t                     associated_data_len,
                                         const uint8_t             *associated_data,
                                         const uint32_t            *iv,
                                         uint8_t                   *ciphertext,
                                         uint8_t                   *authentication_tag);

bool dsk_aead_chacha20_poly1305_decrypt (const uint32_t            *key,
                                         size_t                     ciphertext_len,
                                         const uint8_t             *ciphertext,
                                         size_t                     associated_data_len,
                                         const uint8_t             *associated_data,
                                         const uint32_t            *iv,
                                         uint8_t                   *plaintext,
                                         const uint8_t             *authentication_tag);

