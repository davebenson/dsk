
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


/* 6.3 Multiplication Operation on Blocks */
static void
multiply (const uint32_t *x, const uint32_t *y, uint8_t *out)
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
      carry = v[0] & 1;
      v[0] >>= 1;
      carry2 = v[1] & 1;
      v[1] >>= 1;
      v[1] |= carry << 31;
      carry = v[2] & 1;
      v[2] >>= 1;
      v[2] |= carry2 << 31;
      carry2 = v[3] & 1;
      v[3] >>= 1;
      v[3] |= carry << 31;
      if (carry2)
        v[0] ^= 0xe1000000;
    }

  memcpy (out, z, 16);
}

/* Section 6.4 GHASH function */
static void
ghash (const uint32_t *h,               // length 4
       unsigned n_blocks_in_x,
       const uint8_t *x,
       uint32_t *out)
{
  uint32_t y[4] = {0,0,0,0};
  uint32_t xtmp[4] = {0,0,0,0};
  for (unsigned i = 0; i < n_blocks_in_x; i++)
    {
      bytes_to_be32 (x + 16*i, xtmp, 4);
      y[0] ^= xtmp[0];
      y[1] ^= xtmp[1];
      y[2] ^= xtmp[2];
      y[3] ^= xtmp[3];
      /* y := y * h */
      multiply (y, h, y);
    }
  memcpy (out, y, 16);
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
                           size_t iv_len,
                           const uint8_t *iv,
                           uint8_t *ciphertext,
                           size_t authentication_tag_len,
                           uint8_t *authentication_tag);
