#include "../dsk.h"
#include <stdio.h>

void dsk_hmac_digest (DskChecksumType type,
                      size_t          key_len,
                      const uint8_t  *key,
                      size_t          text_len,
                      const uint8_t  *text,
                      uint8_t        *digest_out)
{
  unsigned B = dsk_checksum_type_get_block_size (type);
  unsigned L = dsk_checksum_type_get_size (type);
  void *H = alloca (dsk_checksum_sizeof_instance (type));

  if (key_len > B)
    {
      uint8_t *long_key = alloca (L);
      dsk_checksum_init (H, type);
      dsk_checksum_feed (H, key_len, key);
      dsk_checksum_done (H);
      dsk_checksum_get (H, long_key);
      key = long_key;
      key_len = L;
    }

  uint8_t *block = alloca (B);
  for (unsigned i = 0; i < key_len; i++)
    block[i] = 0x36 ^ key[i];
  for (unsigned i = key_len; i < B; i++)
    block[i] = 0x36;
  
  // compute digest_out as inner_hash
  dsk_checksum_init (H, type);
  dsk_checksum_feed (H, B, block);
  dsk_checksum_feed (H, text_len, text);
  dsk_checksum_done (H);
  dsk_checksum_get (H, digest_out);

 
  // block = K XOR opad
  for (unsigned i = 0; i < B; i++)
    block[i] ^= 0x36 ^ 0x5c;
  dsk_checksum_init (H, type);
  dsk_checksum_feed (H, B, block);
  dsk_checksum_feed (H, L, digest_out);
  dsk_checksum_done (H);
  dsk_checksum_get (H, digest_out);
}
