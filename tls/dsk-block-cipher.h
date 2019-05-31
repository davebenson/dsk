
typedef void (*DskBlockCipherInplaceFunc)(void *cipher_object,
                                          uint8_t *data_inout);

#define DskBlockCipher128InplaceFunc DskBlockCipherInplaceFunc


typedef struct DskBlockCipher DskBlockCipher;
struct DskBlockCipher
{
  const char *name;
  unsigned key_length;
  unsigned block_size;                  // always 16 for now
  size_t instance_size;
  void (*init)(void *instance, const uint8_t *key);
  void (*encrypt_inplace) (void *instance, uint8_t *data_inout);
  void (*decrypt_inplace) (void *instance, uint8_t *data_inout);
};

extern DskBlockCipher dsk_block_cipher_aes128;
extern DskBlockCipher dsk_block_cipher_aes192;
extern DskBlockCipher dsk_block_cipher_aes256;

