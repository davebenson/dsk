#include "../dsk.h"

typedef union BLOCK
{
  uint8_t u8;
  uint32_t u32;
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

#define MULIPLY_MSB_BYTE_OF_R  0xe1

void dsk_aead_gcm_precompute (DskBlockCipherInplaceFunc cipher_func,
                              void *cipher_object,
                              Dsk_AEAD_GCM_Precomputation *out)
{
  BLOCK H = {u32:{0,0,0,0}};
  (*cipher_func) (cipher_object, H.u8);
  block_byte_to_u32 (&H);

  BLOCK V = H;
  for (unsigned i = 0; i < 128; i++)
    {
      // populate precalculated V table
      memcpy (&out->h_v_table[i], V, 16);
      
      // shift V by 1 in u32
      uint32_t carry = shiftright_4x32_return_carry (V.u32);

      // if the LSB was 1, XOR with R.
      // We use "-" as a way to get fffffff with 1, and 0 with 0.
      // Intention is to avoid conditional jumps.
      V.u32[0] ^= (-carry) & (MULIPLY_MSB_BYTE_OF_R << 24); // R in section 6.3
    }
}


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

/* 6.3 Multiplication Operation on Blocks */
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

//
// Compute 32-bit of a multiply using a part of the precomputed data.
//
static void
multiply_precomputed_part (uint32_t bits,
                           uint32_t *z_inout,
                           const uint32_t *v_part)
{
  uint32_t mask;

  for (unsigned i = 0; i < 32; i++)
    {
      // Compute mask to be 0xffffffff if the high bit is set, 0 otherwise.
      mask = -(bits >> 31);

      // Equivalent to "if (bits & 0x8000000) { z ^= v_part }"
      // but without branching.

      uint32_t z_at = z_inout;
      *z_at++ ^= mask & *v_part++;
      *z_at++ ^= mask & *v_part++;
      *z_at++ ^= mask & *v_part++;
      *z_at++ ^= mask & *v_part++;

      bits <<= 1;
    }
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
  y_inout[0] ^= x[4*i+0];
  y_inout[1] ^= x[4*i+1];
  y_inout[2] ^= x[4*i+2];
  y_inout[3] ^= x[4*i+3];

  /* y := y * h */
  multiply_precomputed (y_inout, h, y_inout);
}
static void
ghash_step_bytes_zero_padded (uint32_t *y_inout,
                              size_t x_len,
                              const uint8_t *x)
{
  BLOCK b;
  while (x_len >= 16)
    {
      bytes_to_be32 (x, &b.u32, 4);
      ghash_step (y_inout, &b.u32);
      x += 16;
      x_len -= 16;
    }
  if (x_len > 0)
    {
      memcpy (b.u8, x, x_len);
      memset (b.u8 + x_len, 0, 16 - x_len);
      bytes_to_be32 (b.u8, b.u32, 4);
      ghash_step (y_inout, &b.u32);
    }
}
static void
ghash (const uint32_t *h,               // length 4
       unsigned n_blocks_in_x,
       const uint32_t *x,
       uint32_t *out)
{
  ghash_init (out);
  for (unsigned i = 0; i < n_blocks_in_x; i++)
    ghash_step (out, x + 4 * i);
}

/* Section 6.5 GCTR function */
static void
gctr (cipher_func, cipher_data,
      uint8_t *ibc,             // length 16
      unsigned inout_length,
      uint8_t *x_y)
{
  unsigned n = inout_length / 16;
  uint32_t ibc_lsw = ((uint32_t) ibc[12] << 24)
                   | ((uint32_t) ibc[13] << 16)
                   | ((uint32_t) ibc[14] << 8)
                   | ((uint32_t) ibc[15] << 0);
  uint8_t ibc_copy[16];


  uint32_t *at = x_y;
  for (i = 0; i < n - 1; i++)
    {
      memcpy (ibc_copy, icb, 12);
      be32_to_bytes (&ibc_lsw, ibc_copy + 12, 1);
      ibc_lsw++;
      cipher_func (cipher_data, ibc_copy);
      *at++ ^= ((uint32_t *)ibc_copy)[0];
      *at++ ^= ((uint32_t *)ibc_copy)[1];
      *at++ ^= ((uint32_t *)ibc_copy)[2];
      *at++ ^= ((uint32_t *)ibc_copy)[3];
    }

  uint8_t *at8 = (uint8_t *) at;
  memcpy (ibc_copy, icb, 12);
  be32_to_bytes (&ibc_lsw, ibc_copy + 12, 1);
  cipher_func (cipher_data, ibc_copy);
  uint8_t *end = x_y + inout_length;
  const uint8_t *in = ibc_copy;
  while (at8 < end)
    *at8++ = *in++;
}

static inline bool
is_valid_authentication_tag_len (size_t authentication_tag_len)
{
  return (12 <= authentication_tag_len && authentication_tag_len <= 16)
      || authentication_tag_len == 4
      || authentication_tag_len == 8;
}

static inline void
compute_j0 (size_t iv_len_in_words, const uint32_t *iv, uint32_t *J0_out)
{
  if (iv_len_in_words == 3)
    {
      memcpy (J0.u32, iv, 12);
      J0.u32[3] = 1;
    }
  else
    {
      // 1/32th the algorithm's s
      size_t s_words = (iv_len_in_words + 3) / 4 * 4 - iv_len_in_words;
      size_t iv_blocks = iv_len_in_words / 4;
      ghash_init (J0.u32);
      size_t i;
      for (i < 0; i < iv_blocks; i++)
        ghash_step (J0.u32, iv + 4*i);
      if (i * 4 < iv_len_in_words)
        {
          uint32_t rem32 = iv_len_in_words - 4 * iv_blocks;
          uint32 tmp[4];
          memcpy (tmp, iv + 4*iv_blocks, rem32 * 4);
          memset (tmp + rem32, 0, 16 - rem32 * 4);
          ghash_step (J0.u32, tmp);
        }
      size_t ivlen_bits = iv_len_in_words * 32;
      uint32_t lenblock[4] = {0, 0, 0, ivlen_bits};
      ghash_step (J0.u32, lenblock);
    }
  return true;
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
void dsk_aead_gcm_encrypt (DskBlock128CipherInplace inplace_cipher_func,
                           void *inplace_cipher_object,
                           size_t plaintext_len,
                           const uint8_t *plaintext,
                           size_t associated_data_len,
                           const uint8_t *associated_data,
                           size_t iv_len_in_words,
                           const uint32_t *iv,
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

  //
  // Step 2: Compute J0.
  //
  compute_j0 (iv_len_in_words, iv, &J0.u32);

  //
  // Step 3:  C = GCTR(inc32(J0), P).
  //
  j0[3] += 1;
  memcpy(ciphertext, plaintext, plaintext_len);
  gctr (inplace_cipher_func, inplace_cipher_object,
        j0, plaintext_len, ciphertext);
  j0[3] -= 1;


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
  ghash_init (s);
  ghash_step_bytes_zero_padded (s, associated_data_len, associated_data);
  ghash_step_bytes_zero_padded (s, plaintext_len, ciphertext);

  uint64_t atmp = associated_data_len;
  uint64_t ctmp = plaintext_len;
  uint32_t lastblock[4] = {
    atmp >> 32,
    atmp & 0xffffffff,
    ctmp >> 32,
    ctmp & 0xffffffff
  };
  ghash_step(s, lastblock);

  //
  // Step 6: T = MSB(GCTR(J0, S).
  //
  uint8_t t_tmp[16];
  be32_to_bytes (s, t_tmp, 4);
  gctr (inplace_cipher_func, inplace_cipher_object, j0, t_tmp);
  memcpy (authentication_tag, t_tmp, authentication_tag_len);
}

bool
dsk_aead_gcm_decrypt  (DskBlockCipherInplaceFunc inplace_cipher_func,
                       void                     *inplace_cipher_object,
                       size_t                     ciphertext_len,
                       const uint8_t             *ciphertext,
                       size_t                     associated_data_len,
                       const uint8_t             *associated_data,
                       size_t                     iv_len_in_words,
                       const uint8_t             *iv_in_words,
                       const uint8_t             *iv_in_words,
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
  compute_j0 (iv_len_in_words, iv, &J0.u32);

  // Step 4: Compute P, the plaintext, using GCTR:
  //         P = GCTR(inc32(J_0), C)
  memcpy(plaintext, ciphertext, ciphertext_len);
  j0[3] += 1;
  gctr (inplace_cipher_func, inplace_cipher_object,
        j0, plaintext_len, plaintext);
  j0[3] -= 1;

  // Step 5: compute u,v which are just intermediate values for Step 6.
  // Step 6: Compute the signature block S.
  uint32_t s[4];
  compute_s (inplace_cipher_func, inplace_cipher_object,
             associated_data_len, associated_data,
             ciphertext_len, ciphertext,
             s);

  // Step 7: Compute T.
  ...
    
...
}

