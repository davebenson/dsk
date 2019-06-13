//
// Currently, TLS is pretty strongly built on
// AES ciphers with a 16-byte block size (128 bits).
// Ideally, use DskBlockCipher128InplaceFunc
// when that is what you intend.
//
// Both the GCM and CCM modes of operation
// assume 16-byte blocks.
//
// (The chacha-20 situation is different - it
// uses 512-bit blocks (== 64 bytes), but
// it uses it's own AEAD using poly1305.)
//

typedef void (*DskBlockCipherInplaceFunc)(void *cipher_object,
                                          uint8_t *data_inout);

#define DskBlockCipher128InplaceFunc DskBlockCipherInplaceFunc


typedef struct DskBlockCipher DskBlockCipher;
struct DskBlockCipher
{
  const char *name;
  unsigned key_length;
  unsigned block_size;                  // always 16 for now
  size_t cipher_object_size;
  void (*init)(void *cipher_object, const uint8_t *key);
  void (*encrypt_inplace) (void *cipher_object, uint8_t *data_inout);
  void (*decrypt_inplace) (void *cipher_object, uint8_t *data_inout);
};

extern DskBlockCipher dsk_block_cipher_aes128;
extern DskBlockCipher dsk_block_cipher_aes192;
extern DskBlockCipher dsk_block_cipher_aes256;

