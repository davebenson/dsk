#include "../dsk.h"
#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>

// RFC 2104, Section 2: Definition of HMAC.
//
//   "We define two fixed and different strings ipad and opad as follows
//    (the 'i' and 'o' are mnemonics for inner and outer):
//    
//                      ipad = the byte 0x36 repeated B times
//                      opad = the byte 0x5C repeated B times.
//    
//       To compute HMAC over the data `text' we perform
//    
//                      H(K XOR opad, H(K XOR ipad, text))
//    
//       Namely,
//    
//        (1) append zeros to the end of K to create a B byte string
//            (e.g., if K is of length 20 bytes and B=64, then K will be
//             appended with 44 zero bytes 0x00)
//        (2) XOR (bitwise exclusive-OR) the B byte string computed in step
//            (1) with ipad
//        (3) append the stream of data 'text' to the B byte string resulting
//            from step (2)
//        (4) apply H to the stream generated in step (3)
//        (5) XOR (bitwise exclusive-OR) the B byte string computed in
//            step (1) with opad
//        (6) append the H result from step (4) to the B byte string
//            resulting from step (5)
//        (7) apply H to the stream generated in step (6) and output
//            the result"
//

// In the above, K is the key, B is the block-length of the hash-function.

// TODO: for these purposes, a block-based hash-function instance would be nice.

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
