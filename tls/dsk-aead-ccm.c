#include "../dsk.h"
#include <string.h>
#include <stdio.h>

//
// See https://nvlpubs.nist.gov/nistpubs/Legacy/SP/nistspecialpublication800-38c.pdf.
//

static void
compute_b0 (size_t plaintext_len,
            size_t associated_data_len,
            size_t iv_len,
            const uint8_t *iv,
            size_t authentication_tag_len,
            uint8_t *block0_out)
{
  assert (4 <= authentication_tag_len && authentication_tag_len <= 16);
  assert (authentication_tag_len % 2 == 0);
  assert (7 <= iv_len && iv_len <= 13);

  unsigned q = 15 - iv_len;

  block0_out[0] = (q - 1)
                | (((authentication_tag_len - 2) / 2) << 3)
                | (associated_data_len > 0 ? 64 : 0);

  memcpy (block0_out + 1, iv, iv_len);

  if (q < 8)
    assert (plaintext_len < (1ULL << (8*q)));

  for (unsigned i = 0; i < q; i++)
    block0_out[1 + iv_len + i] = plaintext_len >> ((q-i-1) * 8);
}

static unsigned
write_associated_data_length (size_t associated_data_len, uint8_t *block)
{
  if (associated_data_len < (1<<16) - (1<<8))
    {
      block[0] = (associated_data_len >> 8) & 0xff;
      block[1] = associated_data_len & 0xff;
      return 2;
    }
  else if ((uint64_t) associated_data_len < (1ULL<<32))
    {
      block[0] = 0xff;
      block[1] = 0xfe;
      block[2] = (associated_data_len >> 24) & 0xff;
      block[3] = (associated_data_len >> 16) & 0xff;
      block[4] = (associated_data_len >> 8) & 0xff;
      block[5] = (associated_data_len >> 0) & 0xff;
      return 6;
    }
  else
    {
      block[0] = 0xff;
      block[1] = 0xff;
      block[2] = (associated_data_len >> 56) & 0xff;
      block[3] = (associated_data_len >> 48) & 0xff;
      block[4] = (associated_data_len >> 40) & 0xff;
      block[5] = (associated_data_len >> 32) & 0xff;
      block[6] = (associated_data_len >> 24) & 0xff;
      block[7] = (associated_data_len >> 16) & 0xff;
      block[8] = (associated_data_len >> 8) & 0xff;
      block[9] = (associated_data_len >> 0) & 0xff;
      return 10;
    }
}

static inline void
xor_assign_aligned (uint8_t *inout, const uint8_t *b)
{
  uint32_t *A = (uint32_t *) inout;
  const uint32_t *B = (const uint32_t *) b;
  A[0] ^= B[0];
  A[1] ^= B[1];
  A[2] ^= B[2];
  A[3] ^= B[3];
}

// Implements steps 1-4 of Generation-Encryption Process (Section 6.1),
// or steps 7-9 + part of 10 for Decryption-Verification Process (Section 6.2).
static void
compute_T                 (DskBlockCipher128InplaceFunc func,
                           void                        *cipher_object,
                           size_t                     plaintext_len,
                           const uint8_t             *plaintext,
                           size_t                     associated_data_len,
                           const uint8_t             *associated_data,
                           size_t                     iv_len,
                           const uint8_t             *iv,
                           size_t                     authentication_tag_len,
                           uint8_t                   *T)
{
  uint32_t aligned[8];
  uint8_t *rv = (uint8_t *) (aligned + 0);
  uint8_t *block = (uint8_t *) (aligned + 4);
  compute_b0 (plaintext_len, associated_data_len, iv_len, iv, authentication_tag_len, rv);
  func (cipher_object, rv);                     // rv==Y_0
  
  if (associated_data_len > 0)
    {
      size_t used = write_associated_data_length (associated_data_len, block);
      size_t ass_at;
      if (16 - used > associated_data_len)
        {
          memcpy (block + used, associated_data, associated_data_len);
          memset (block + used + associated_data_len, 0, 16 - used - associated_data_len);
          ass_at = associated_data_len;
        }
      else
        {
          memcpy (block + used, associated_data, 16 - used);
          ass_at = 16 - used;
        }

      // do round
      xor_assign_aligned (rv, block);
      func (cipher_object, rv);

      while (ass_at + 16 <= associated_data_len)
        {
          // non-zero-padded rounds
          memcpy (block, associated_data + ass_at, 16);
          xor_assign_aligned (rv, block);
          func (cipher_object, rv);
          ass_at += 16;
        }
      if (ass_at < associated_data_len)
        {
          // padded round
          unsigned rem = associated_data_len - ass_at;
          memcpy (block, associated_data + ass_at, rem);
          memset (block + rem, 0, 16 - rem);
          xor_assign_aligned (rv, block);
          func (cipher_object, rv);
        }
    }

  size_t p_at = 0;
  while (p_at + 16 <= plaintext_len)
    {
      // non-zero-padding rounds
      memcpy (block, plaintext + p_at, 16);
      xor_assign_aligned (rv, block);
      func (cipher_object, rv);
      p_at += 16;
    }
  if (p_at < plaintext_len)
    {
      unsigned rem = plaintext_len - p_at;
      memcpy (block, plaintext + p_at, rem);
      memset (block + rem, 0, 16 - rem);
      xor_assign_aligned (rv, block);
      func (cipher_object, rv);
    }
  memcpy (T, rv, authentication_tag_len);
}

static void
increment_ctr (uint8_t *ctr, size_t ctr_bytes)
{
  uint8_t *at = ctr + 15;
  while (ctr_bytes-- > 0)
    {
      *at += 1;
      if (*at != 0)
        return;
    }
}

//
// Generation-Encryption Process, given in Section 6.1.
//
// Here we hard-code the formatting function given in Appendix A.
//
void dsk_aead_ccm_encrypt (DskBlockCipher128InplaceFunc func,
                           void                        *cipher_object,
                           size_t                     plaintext_len,
                           const uint8_t             *plaintext,
                           size_t                     associated_data_len,
                           const uint8_t             *associated_data,
                           size_t                     iv_len,
                           const uint8_t             *iv,
                           uint8_t                   *ciphertext,
                           size_t                     authentication_tag_len,
                           uint8_t                   *authentication_tag)
{
  // Steps 1-4.
  compute_T (func, cipher_object,
             plaintext_len, plaintext,
             associated_data_len, associated_data,
             iv_len, iv,
             authentication_tag_len, authentication_tag);

  // Compute Ctr_0.
  unsigned q = 15 - iv_len;
  uint8_t ctr[16];
  ctr[0] = q - 1;
  memcpy (ctr + 1, iv, iv_len);
  memset (ctr + 1 + iv_len, 0, q);

  uint8_t ctr_copy[16];
  memcpy (ctr_copy, ctr, 16);
  increment_ctr (ctr, q);
  func (cipher_object, ctr_copy);               // S0
  for (unsigned i = 0; i < authentication_tag_len; i++)
    authentication_tag[i] ^= ctr_copy[i];               // Second half of Step 8.

  // Steps 6-8, when 16-byte sections of P remain.
  size_t at = 0;
  while (at + 16 <= plaintext_len)
    {
      memcpy (ctr_copy, ctr, 16);
      increment_ctr (ctr, q);
      func (cipher_object, ctr_copy);

      for (unsigned j = 0; j < 16; j++)
        *ciphertext++ = *plaintext++ ^ ctr_copy[j];
      at += 16;
    }

  // Steps 6-8, when less than 16-bytes of P remains.
  if (at < plaintext_len)
    {
      memcpy (ctr_copy, ctr, 16);
      func (cipher_object, ctr_copy);
      for (unsigned j = 0; at < plaintext_len; at++, j++)
        *ciphertext++ = *plaintext++ ^ ctr_copy[j];
    }
}

bool dsk_aead_ccm_decrypt (DskBlockCipher128InplaceFunc func,
                           void                        *cipher_object,
                           size_t                     ciphertext_len,
                           const uint8_t             *ciphertext,
                           size_t                     associated_data_len,
                           const uint8_t             *associated_data,
                           size_t                     iv_len,
                           const uint8_t             *iv,
                           uint8_t                   *plaintext,
                           size_t                     authentication_tag_len,
                           const uint8_t             *authentication_tag)
{
  // Compute Ctr_0.
  unsigned q = 15 - iv_len;
  uint8_t ctr[16];
  uint8_t ctr_copy[16];
  ctr[0] = q - 1;
  memcpy (ctr + 1, iv, iv_len);
  memset (ctr + 1 + iv_len, 0, q);

  memcpy (ctr_copy, ctr, 16);
  increment_ctr (ctr, q);
  func (cipher_object, ctr_copy);     // S0

  uint8_t T[16];
  for (unsigned i = 0; i < authentication_tag_len; i++)
    T[i] = authentication_tag[i] ^ ctr_copy[i];

  // Steps 2-4, when less than 16-bytes of P remains.
  size_t at = 0;
  while (at + 16 <= ciphertext_len)
    {
      memcpy (ctr_copy, ctr, 16);
      increment_ctr (ctr, q);
      func (cipher_object, ctr_copy);

      for (unsigned j = 0; j < 16; j++)
        *plaintext++ = *ciphertext++ ^ ctr_copy[j];
      at += 16;
    }

  // Steps 2-4, when less than 16-bytes of P remains.
  if (at < ciphertext_len)
    {
      memcpy (ctr_copy, ctr, 16);
      func (cipher_object, ctr_copy);
      for (unsigned j = 0; at < ciphertext_len; at++, j++)
        *plaintext++ = *ciphertext++ ^ ctr_copy[j];
    }

  // reset to start of plaintext.
  plaintext -= ciphertext_len;

  uint8_t T2[16];
  compute_T (func, cipher_object,
             ciphertext_len, plaintext,
             associated_data_len, associated_data,
             iv_len, iv,
             authentication_tag_len, T2);
  if (memcmp (T, T2, authentication_tag_len) != 0)
    return false;
  return true;
}

