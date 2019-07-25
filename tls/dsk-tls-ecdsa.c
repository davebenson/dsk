#include "../dsk.h"
#include <alloca.h>
#include <string.h>

//
// The definitive reference for ECDSA is ANS X9.62
//
// Unfortunately you have to pay for that document,
// but it's probably pretty similar to the working draft:
//
//     http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.202.2977&rep=rep1&type=pdf
//
//
// The wikipedia page is probably the best resource:
//
//        http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.202.2977&rep=rep1&type=pdf
// 

//https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.186-4.pdf

void
dsk_tls_ecdsa_generate_key_pair   (const DskTls_ECPrime_Group *group,
                                   uint32_t *d_A,                       // private key
                                   uint32_t *Q_A_x,                     // public key
                                   uint32_t *Q_A_y)                     // public key
{
retry_make_d_A:
  dsk_get_cryptorandom_data (group->len * 4, (uint8_t *) d_A);
  if (group->n_bit_length % 32 != 0)
    d_A[group->len - 1] &= (1 << (group->n_bit_length % 32)) - 1;
  if (dsk_tls_bignum_is_zero (group->len, d_A)
   || dsk_tls_bignum_compare (group->len, d_A, group->n) >= 0)
    goto retry_make_d_A;

  dsk_tls_ecprime_multiply_int (group, group->x, group->y,
                                group->len, d_A,
                                Q_A_x, Q_A_y);
}

static void
compute_z (const DskTls_ECPrime_Group *group,
           size_t                      content_hash_length,
           const uint8_t              *content_hash_data,
           uint32_t                   *z_out)
{
  if (group->n_bit_length >= content_hash_length * 8)
    {
      //
      // Since all known hashes are multiples of 4 bytes (==32 bits),
      // we only handle that case.
      //
      assert (content_hash_length % 4 == 0);
      unsigned i;
      for (i = 0; i < content_hash_length / 4; i++)
        z_out[i] = dsk_uint32be_parse (content_hash_data + content_hash_length - 4 * (i+1));
      for (     ; i < group->len; i++)
        z_out[i] = 0;
    }
  else
    {
      assert (content_hash_length >= 4 * group->len);
      for (unsigned i = 0; i < group->len; i++)
        z_out[i] = dsk_uint32be_parse (content_hash_data + 4 * (group->len - 1 - i));
      if (group->n_bit_length % 32 != 0)
        {
          // shift the number right by this many bits.
          unsigned shr = 32 - group->n_bit_length % 32;
          dsk_tls_bignum_shiftright_inplace (group->len, z_out, shr);
        }
    }
}
            

void
dsk_tls_ecdsa_sign   (const DskTls_ECPrime_Group *group,
                      const uint32_t             *d_A,                  // private key
                      size_t                      content_hash_length,
                      const uint8_t              *content_hash_data,
                      uint32_t                   *r_out,
                      uint32_t                   *s_out)
{
  //
  // Compute 'z', the left-most L_n bits of content_hash_number_data.
  //
  uint32_t *z = alloca (4 * group->len);
  compute_z (group, content_hash_length, content_hash_data, z);

  uint32_t *slab = alloca (group->len * 4 * 4);
  uint32_t *k = slab;
  uint32_t *x1 = k + group->len;
  uint32_t *y1 = x1 + group->len;
  uint32_t *k_inv = y1 + group->len;

  uint32_t *tmp = alloca (group->len * 4 * 2);

  //
  // Find k, a cryptographic random number from [1, group->p - 1].
  // Rarely, we'll choose 'k' that leads to a degenerate signature:
  // in such cases, we'll retry.
  //
choose_random_k:
  dsk_get_cryptorandom_data (group->len * 4, (uint8_t *) k);
  if (group->bit_length % 32 != 0)
    k[group->len - 1] &= (1 << (group->bit_length % 32)) - 1;
  if (dsk_tls_bignum_is_zero (group->len, k)
   || dsk_tls_bignum_compare (group->len, k, group->p) >= 0)
    goto choose_random_k;
  
  //
  // Compute (x1,y1) = k x G, where G is the base-point.
  //
  dsk_tls_ecprime_multiply_int (group,
                                group->x, group->y,
                                group->len, k,
                                x1, y1);


  //
  // Calculate r = x1 (mod n).  If r=0, pick another 'k'.
  //
  if (dsk_tls_bignum_is_zero (group->len, x1))
    goto choose_random_k;
  memcpy (r_out, x1, 4 * group->len);

  //
  // Calculate s = k^{-1} (z + r * d_A) (mod n).
  // If s=0, pick another 'k'.
  //
  if (!dsk_tls_bignum_modular_inverse (group->len, k, group->n, k_inv))
    goto choose_random_k;
  dsk_tls_bignum_multiply (group->len, x1, group->len, d_A, tmp);
  dsk_tls_bignum_modulus_with_barrett_mu (group->len * 2, tmp,
                                          group->len, group->n, group->n_barrett_mu,
                                          s_out);
  dsk_tls_bignum_modular_add (group->len, s_out, z, group->n, s_out);
  dsk_tls_bignum_multiply (group->len, k_inv, group->len, s_out, tmp);
  dsk_tls_bignum_modulus_with_barrett_mu (group->len * 2, tmp,
                                          group->len, group->n, group->n_barrett_mu,
                                          s_out);
  if (dsk_tls_bignum_is_zero (group->len, s_out))
    goto choose_random_k;
}

bool
dsk_tls_ecdsa_validate_public_key (const DskTls_ECPrime_Group *group,
                                   const uint32_t             *Q_A_x,    // public key
                                   const uint32_t             *Q_A_y)    // public key
{
  // Check that Q_A is not equal to the identity element O,
  // and its coordinates are otherwise valid
  if (dsk_tls_ecprime_is_zero (group, Q_A_x, Q_A_y)
   || dsk_tls_bignum_compare (group->len, Q_A_x, group->p) >= 0
   || dsk_tls_bignum_compare (group->len, Q_A_y, group->p) >= 0)
    return false;

  // Check that Q_A lies on the curve
  if (!dsk_tls_ecprime_is_on_curve (group, Q_A_x, Q_A_y))
    return false;

  // Check that n x Q_A = O
  uint32_t *n_minus_one = alloca (4 * group->len);
  uint32_t *xn1 = alloca (4 * group->len);
  uint32_t *yn1 = alloca (4 * group->len);
  memcpy (n_minus_one, group->n, 4 * group->len);
  dsk_tls_bignum_subtract_word_inplace (group->len, n_minus_one, 1);
  dsk_tls_ecprime_multiply_int (group,
                                Q_A_x, Q_A_y,
                                group->len, n_minus_one,
                                xn1, yn1);
  assert(memcmp (xn1, Q_A_x, group->len * 4) == 0);
  assert(!dsk_tls_bignum_is_zero (group->len, yn1));
  dsk_tls_bignum_add (group->len, yn1, Q_A_y, yn1);
  assert (memcmp (yn1, group->p, 4 * group->len) == 0);

  return true;
}
  
bool
dsk_tls_ecdsa_verify (const DskTls_ECPrime_Group *group,
                      const uint32_t             *Q_A_x,    // public key
                      const uint32_t             *Q_A_y,    // public key
                      size_t                      content_hash_length,
                      const uint8_t              *content_hash_data,
                      const uint32_t             *r,
                      const uint32_t             *s)
{
  // ---------------------------
  // Validate that Q_A is valid.
  // ---------------------------
  if (!dsk_tls_ecdsa_validate_public_key (group, Q_A_x, Q_A_y))
    return false;

  // --------------------
  // Signature Validation
  // --------------------

  // Check that r, s are in [1, n-1]
  if (dsk_tls_bignum_is_zero (group->len, r)
   || dsk_tls_bignum_is_zero (group->len, s)
   || dsk_tls_bignum_compare (group->len, r, group->n) >= 0
   || dsk_tls_bignum_compare (group->len, s, group->n) >= 0)
   return false;

  // Compute 'z', the left-most L_n bits of content_hash_number_data.
  uint32_t *z = alloca (4 * group->len);
  compute_z (group, content_hash_length, content_hash_data, z);

  // Compute s^{-1} (mod n)
  uint32_t *s_inv = alloca (4 * group->len);
  if (!dsk_tls_bignum_modular_inverse (group->len, s, group->n, s_inv))
    return false;

  uint32_t *tmp = alloca (4 * 2 * group->len);

  // Calculate u1 = z s^{-1} (mod n)
  uint32_t *u1 = alloca (4 * group->len);
  dsk_tls_bignum_multiply (group->len, z, group->len, s_inv, tmp);
  dsk_tls_bignum_modulus_with_barrett_mu (group->len * 2, tmp,
                                          group->len, group->n, group->n_barrett_mu,
                                          u1);

  // Calculate u2 = r s^{-1} (mod n)
  uint32_t *u2 = alloca (4 * group->len);
  dsk_tls_bignum_multiply (group->len, r, group->len, s_inv, tmp);
  dsk_tls_bignum_modulus_with_barrett_mu (group->len * 2, tmp,
                                          group->len, group->n, group->n_barrett_mu,
                                          u2);

  // Calculate (x1,y1) = u1 x G + u2 x Q_A.
  uint32_t *u1G = alloca (4 * 2 * group->len);
  uint32_t *u2QA = alloca (4 * 2 * group->len);
  dsk_tls_ecprime_multiply_int (group, group->x, group->y,
                                group->len, u1,
                                u1G, u1G + group->len);
  dsk_tls_ecprime_multiply_int (group, Q_A_x, Q_A_y,
                                group->len, u2, u2QA, u2QA + group->len);
  uint32_t *x1 = alloca (4 * group->len);
  uint32_t *y1 = alloca (4 * group->len);
  dsk_tls_ecprime_add (group->len,
                       u1G, u1G + group->len,
                       u2QA, u2QA + group->len,
                       x1, y1);

  // If (x1,y1) is the identity, then fail.
  if (dsk_tls_ecprime_is_zero (group, x1, y1))
    return false;

  // Return r == x1.
  return dsk_tls_bignum_compare (group->len, r, x1) == 0;
}
