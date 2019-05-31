#include "../dsk.h"
#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>

void dsk_hmac_digest (DskChecksumType*type,
                      size_t          key_len,
                      const uint8_t  *key,
                      size_t          text_len,
                      const uint8_t  *text,
                      uint8_t        *digest_out)
{
  unsigned B = type->block_size_in_bits;
  unsigned L = type->hash_size;
  void *H = alloca (type->instance_size);

  if (key_len > B)
    {
      uint8_t *long_key = alloca (L);
      type->init (H);
      type->feed (H, key_len, key);
      type->end (H, long_key);
      key = long_key;
      key_len = L;
    }

  uint8_t *block = alloca (B);
  for (unsigned i = 0; i < key_len; i++)
    block[i] = 0x36 ^ key[i];
  for (unsigned i = key_len; i < B; i++)
    block[i] = 0x36;
  
  // compute digest_out as inner_hash
  type->init (H);
  type->feed (H, B, block);
  type->feed (H, text_len, text);
  type->end (H, digest_out);

 
  // block = K XOR opad
  for (unsigned i = 0; i < B; i++)
    block[i] ^= 0x36 ^ 0x5c;
  type->init (H);
  type->feed (H, B, block);
  type->feed (H, L, digest_out);
  type->end (H, digest_out);
}
