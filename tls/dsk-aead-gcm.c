#include "../dsk.h"
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>

typedef union BLOCK
{
  uint8_t u8[16];
  uint32_t u32[4];
} BLOCK;

/* References:
RFC 5116: An Interface and Algorithms for Authenticated Encryption (this is the root RFC for all things about AEAD)

GCM: https://nvlpubs.nist.gov/nistpubs/Legacy/SP/nistspecialpublication800-38d.pdf
*/

static inline void
bytes_to_be32 (const uint8_t *bytes, uint32_t *out, unsigned n_out)
{
#if DSK_IS_LITTLE_ENDIAN
  for (unsigned i = 0; i < n_out; i++)
    {
      out[i] = ((uint32_t) bytes[4*i+0] << 24)
             | ((uint32_t) bytes[4*i+1] << 16)
             | ((uint32_t) bytes[4*i+2] << 8)
             | ((uint32_t) bytes[4*i+3] << 0);
    }
#else
  memcpy (out, bytes, 4*n_out);
#endif
}
static inline void
be32_to_bytes (const uint32_t *in, uint8_t *bytes_out, unsigned n_word32)
{
#if DSK_IS_LITTLE_ENDIAN
  for (unsigned i = 0; i < n_word32; i++)
    {
      bytes_out[4*i + 0] = in[i] >> 24;
      bytes_out[4*i + 1] = in[i] >> 16;
      bytes_out[4*i + 2] = in[i] >> 8;
      bytes_out[4*i + 3] = in[i] >> 0;
    }
#else
  memcpy (bytes_out, in, 4*n_out);
#endif
}

static inline void
block_byte_to_u32 (BLOCK *inout)
{
#if DSK_IS_LITTLE_ENDIAN
  inout->u32[0] = htonl (inout->u32[0]);
  inout->u32[1] = htonl (inout->u32[1]);
  inout->u32[2] = htonl (inout->u32[2]);
  inout->u32[3] = htonl (inout->u32[3]);
#endif
}
#define block_u32_to_byte block_byte_to_u32

#define MULIPLY_MSB_BYTE_OF_R  0xe1

static inline uint32_t
shiftright_4x32_return_carry (uint32_t *v)
{
  uint32_t carry = v[0] & 1;
  v[0] >>= 1;
  uint32_t carry2 = v[1] & 1;
  v[1] >>= 1;
  v[1] |= carry << 31;
  carry = v[2] & 1;
  v[2] >>= 1;
  v[2] |= carry2 << 31;
  carry2 = v[3] & 1;
  v[3] >>= 1;
  v[3] |= carry << 31;
  return carry2;
}


void dsk_aead_gcm_precompute (DskBlockCipherInplaceFunc cipher_func,
                              void *cipher_object,
                              Dsk_AEAD_GCM_Precomputation *out)
{
  BLOCK H = {.u32 = {0,0,0,0}};
  (*cipher_func) (cipher_object, H.u8);
  block_byte_to_u32 (&H);

  BLOCK V = H;
  for (unsigned i = 0; i < 128; i++)
    {
      // populate precalculated V table
      memcpy (out->h_v_table + i * 4, &V, 16);
      
      // shift V by 1 in u32
      uint32_t carry = shiftright_4x32_return_carry (V.u32);

      // if the LSB was 1, XOR with R.
      // We use "-" as a way to get fffffff with 1, and 0 with 0.
      // Intention is to avoid conditional jumps.
      V.u32[0] ^= (-carry) & (MULIPLY_MSB_BYTE_OF_R << 24); // R in section 6.3
    }

  out->block_cipher_object = cipher_object;
  out->block_cipher = cipher_func;
}


#if 0
static unsigned
shiftright_4x32_return_carry (uint32_t *v)
{
  uint32_t n0 = v[0] << 31;
  v[0] >>= 1;
  uint32_t n1 = v[1] << 31;
  v[1] >>= 1;
  v[1] |= n0;
  uint32_t n2 = v[2] << 31;
  v[2] >>= 1;
  v[2] |= n1;
  uint32_t rv = v[3] & 1;
  v[3] >>= 1;
  v[3] |= n2;
  return rv;
}
#endif

/* 6.3 Multiplication Operation on Blocks */
#if 0
static void
multiply (const uint32_t *x, const uint32_t *y, uint32_t *out)
{
  uint32_t z[4] = {0,0,0,0};
  uint32_t v[4];
  memcpy (v, y, 16);

  uint32_t *xi_at = x;
  uint32_t mask = 0x8000000;
  for (i = 0; i < 128; i++)
    {
      if (*xi_at & mask)
        {
          z[0] ^= v[0];
          z[1] ^= v[1];
          z[2] ^= v[2];
          z[3] ^= v[3];
        }
      mask >>= 1;
      if (mask == 0)
        {
          xi_at++;
          mask = 0x8000000;
        }
      if (shiftright_4x32_return_carry (v))
        v[0] |= ...
    }

  memcpy (out, z, 16);
}
#endif

//
// Compute 32-bit of a multiply using a part of the precomputed data.
//
static void
multiply_precomputed_part (uint32_t bits,
                           const uint32_t *v_part,
                           uint32_t *z_inout)
{
  uint32_t mask;

  for (unsigned i = 0; i < 32; i++)
    {
      // Compute mask to be 0xffffffff if the high bit is set, 0 otherwise.
      mask = -(bits >> 31);
      //fprintf(stderr,"mask=%08x v_parts=%08x %08x %08x %08x\n", mask, v_part[0], v_part[1], v_part[2], v_part[3]);

      // Equivalent to "if (bits & 0x8000000) { z ^= v_part }"
      // but without branching.

      uint32_t *z_at = z_inout;
      *z_at++ ^= mask & *v_part++;
      *z_at++ ^= mask & *v_part++;
      *z_at++ ^= mask & *v_part++;
      *z_at++ ^= mask & *v_part++;

      bits <<= 1;
    }
}
static void
multiply_precomputed      (const uint32_t *A,
                           Dsk_AEAD_GCM_Precomputation *B,
                           uint32_t *out)
{
  uint32_t tmp[4] = {0,0,0,0};
  multiply_precomputed_part (A[0], B->h_v_table, tmp);
  multiply_precomputed_part (A[1], B->h_v_table + 128, tmp);
  multiply_precomputed_part (A[2], B->h_v_table + 256, tmp);
  multiply_precomputed_part (A[3], B->h_v_table + 384, tmp);
  memcpy (out, tmp, 4*4);
}

/* Section 6.4 GHASH function */
static void
ghash_init (uint32_t *y_out)              // length 4
{
  memset (y_out, 0, 16);
}
static void
ghash_step (uint32_t *y_inout,
            const uint32_t *x,
            Dsk_AEAD_GCM_Precomputation *precompute)
{
  /* y ^= x_i */
  y_inout[0] ^= x[0];
  y_inout[1] ^= x[1];
  y_inout[2] ^= x[2];
  y_inout[3] ^= x[3];

  /* y := y * h */
  multiply_precomputed (y_inout, precompute, y_inout);
}
static void
ghash_step_bytes_zero_padded (uint32_t *y_inout,
                              size_t x_len,
                              const uint8_t *x,
                              Dsk_AEAD_GCM_Precomputation *precompute)
{
  BLOCK b;
  while (x_len >= 16)
    {
      bytes_to_be32 (x, b.u32, 4);
      ghash_step (y_inout, b.u32, precompute);
      x += 16;
      x_len -= 16;
    }
  if (x_len > 0)
    {
      memcpy (b.u8, x, x_len);
      memset (b.u8 + x_len, 0, 16 - x_len);
      bytes_to_be32 (b.u8, b.u32, 4);
      ghash_step (y_inout, b.u32, precompute);
    }
}
static void
ghash (unsigned n_blocks_in_x,
       const uint32_t *x,
       Dsk_AEAD_GCM_Precomputation *precompute,
       uint32_t *out)
{
  ghash_init (out);
  for (unsigned i = 0; i < n_blocks_in_x; i++)
    ghash_step (out, x + 4 * i, precompute);
}

/* Section 6.5 GCTR function */
static void
gctr (DskBlockCipherInplaceFunc cipher_func,
      void *cipher_data,
      uint32_t *icb,             // length 4
      unsigned inout_length,
      uint8_t *x_y)
{
  unsigned n = (inout_length + 15) / 16;
  uint32_t icb_lsw = icb[3];
  uint8_t icb_copy[16];


  uint8_t *at = x_y;
  unsigned i;
  if (n > 1)
    for (i = 0; i < n - 1; i++)
      {
        be32_to_bytes (icb, icb_copy, 3);
        be32_to_bytes (&icb_lsw, icb_copy + 12, 1);
        icb_lsw++;
        cipher_func (cipher_data, icb_copy);
        for (unsigned j = 0; j < 16; j++)
          *at++ ^= icb_copy[j];
      }

  uint8_t *end = x_y + inout_length;
  if (at < end)
    {
      be32_to_bytes (icb, icb_copy, 3);
      be32_to_bytes (&icb_lsw, icb_copy + 12, 1);
      cipher_func (cipher_data, icb_copy);
      const uint8_t *in = icb_copy;
      while (at < end)
        *at++ ^= *in++;
    }
}

static inline bool
is_valid_authentication_tag_len (size_t authentication_tag_len)
{
  return (12 <= authentication_tag_len && authentication_tag_len <= 16)
      || authentication_tag_len == 4
      || authentication_tag_len == 8;
}

static inline void
compute_j0 (size_t iv_len,
            const uint8_t *iv,
            Dsk_AEAD_GCM_Precomputation *precompute,
            uint32_t *J0_out)
{
  if (iv_len == 12)
    {
      bytes_to_be32 (iv, J0_out, 3);
      J0_out[3] = 1;
    }
  else
    {
      // 1/32th the algorithm's s
      //size_t s_words = (iv_len_in_words + 3) / 4 * 4 - iv_len_in_words;
      size_t iv_blocks = iv_len / 16;
      ghash_init (J0_out);
      size_t i;
      for (i = 0; i < iv_blocks; i++)
        {
          uint32_t tmp[4];
          bytes_to_be32 (iv + 16*i, tmp, 4);
          ghash_step (J0_out, tmp, precompute);
        }
      if (i * 16 < iv_len)
        {
          uint32_t rem_bytes = iv_len - 16 * iv_blocks;
          uint32_t tmp[4];
          assert (rem_bytes % 4 == 0);
          bytes_to_be32 (iv + 16*iv_blocks, tmp, rem_bytes / 4);
          memset (tmp + rem_bytes / 4, 0, 16 - rem_bytes);
          ghash_step (J0_out, tmp, precompute);
        }
      size_t ivlen_bits = iv_len * 8;
      uint32_t lenblock[4] = {0, 0, 0, ivlen_bits};
      ghash_step (J0_out, lenblock, precompute);
    }
}

static void
compute_s (Dsk_AEAD_GCM_Precomputation *precompute,
             size_t associated_data_len, const uint8_t *associated_data,
             size_t ciphertext_len, const uint8_t *ciphertext,
             uint32_t *s)
{
  ghash_init (s);
  ghash_step_bytes_zero_padded (s, associated_data_len, associated_data, precompute);
  ghash_step_bytes_zero_padded (s, ciphertext_len, ciphertext, precompute);

  uint64_t atmp = associated_data_len * 8;
  uint64_t ctmp = ciphertext_len * 8;
  uint32_t lastblock[4] = {
    atmp >> 32,
    atmp & 0xffffffff,
    ctmp >> 32,
    ctmp & 0xffffffff
  };
  ghash_step(s, lastblock, precompute);
}

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
void dsk_aead_gcm_encrypt (Dsk_AEAD_GCM_Precomputation *precompute,
                           size_t plaintext_len,
                           const uint8_t *plaintext,
                           size_t associated_data_len,
                           const uint8_t *associated_data,
                           size_t iv_len,
                           const uint8_t *iv,
                           uint8_t *ciphertext,
                           size_t authentication_tag_len,
                           uint8_t *authentication_tag)
{
  //
  // This is Algorithm 4: GCM-AE_k(IV,P,A) in the NIST document above.
  //
  // Steps below refer to that specification.
  //
  // In that algorithm:
  //       CIPH = inplace_cipher_func, precomputed
  //       K    = inplace_cipher_object, 
  //       t    = authentication_tag_len * 8  (they use bits, we use bytes)
  //       IV   = iv_len_in_words
  //       P    = plaintext_len, plaintext
  //       A    = associated_data, associated_data_len
  //       C    = ciphertext [length is plaintext_len]
  //       T    = authentication_tag (of length authentication_tag_len bytes)
  //
  BLOCK J0;

  //
  // Step 1: Compute H.  This is baked into the precomputed data.
  //
  assert (is_valid_authentication_tag_len (authentication_tag_len));
  assert (iv_len % 4 == 0);

  //
  // Step 2: Compute J0.
  //
  compute_j0 (iv_len, iv, precompute, J0.u32);

  //
  // Step 3:  C = GCTR(inc32(J0), P).
  //
  J0.u32[3] += 1;
  memcpy(ciphertext, plaintext, plaintext_len);
  gctr (precompute->block_cipher, precompute->block_cipher_object,
        J0.u32, plaintext_len, ciphertext);
  J0.u32[3] -= 1;


  //
  // Step 4:  compute padding amounts needed for
  // plaintext_len, associated_data to round
  // up to block size.
  //
  //unsigned u_bytes = (plaintext_len + 15) / 16 * 16 - plaintext_len;
  //unsigned v_bytes = (associated_data_len + 15) / 16 * 16 - associated_data_len;


  //
  // Step 5: compute S = GHASH((A || 0^v || C || 0^u || [len(A)]_64 || [len(C)]_64)
  //
  uint32_t s[4];
  compute_s (precompute,
             associated_data_len, associated_data,
             plaintext_len, ciphertext,
             s);

  //
  // Step 6: T = MSB(GCTR(J0, S).
  //
  uint8_t t_tmp[16];
  be32_to_bytes (s, t_tmp, 4);
  gctr (precompute->block_cipher, precompute->block_cipher_object,
        J0.u32, 16, t_tmp);
  memcpy (authentication_tag, t_tmp, authentication_tag_len);
}

bool
dsk_aead_gcm_decrypt  (Dsk_AEAD_GCM_Precomputation *precompute,
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

  //
  // This is Algorithm 5: GCM-AD_k(IV,C,A,T) in the NIST document above.
  //
  // Steps below refer to that specification.
  //

  // Step 1: Verify that IV, A, C lengths are supported.
  assert (is_valid_authentication_tag_len (authentication_tag_len));

  // Step 2: H = CIPH(0).  This is baked into the PrecomputedData
  // and is therefore unnecessary.

  // Step 3: Compute J0.  Re-use code from encryption side.
  BLOCK J0;
  compute_j0 (iv_len, iv, precompute, J0.u32);

  // Step 4: Compute P, the plaintext, using GCTR:
  //         P = GCTR(inc32(J_0), C)
  memcpy(plaintext, ciphertext, ciphertext_len);
  J0.u32[3] += 1;
  gctr (precompute->block_cipher, precompute->block_cipher_object,
        J0.u32, ciphertext_len, plaintext);
  J0.u32[3] -= 1;

  // Step 5: compute u,v which are just intermediate values for Step 6.
  // Step 6: Compute the signature block S.
  uint32_t s[4];
  compute_s (precompute,
             associated_data_len, associated_data,
             ciphertext_len, ciphertext,
             s);

  // Step 7: Compute T.
  uint8_t t_tmp[16];
  be32_to_bytes (s, t_tmp, 4);
  gctr (precompute->block_cipher, precompute->block_cipher_object,
        J0.u32, 16, t_tmp);
  if (memcmp (authentication_tag, t_tmp, authentication_tag_len) != 0)
    return false;

  return true;
}

