#include "../dsk.h"
#include <string.h>
#include <stdio.h>

// Some optimizations from https://eprint.iacr.org/2015/625.pdf


static const uint32_t base_point_u[] = {
  0x00000005,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
};
static const uint32_t p[] = {
  0xffffffff,
  0xffffffff,
  0xffffffff,
  0xffffffff,
  0xffffffff,
  0xffffffff,
  0xffffffff,
  0xfffffffe,
  0xffffffff,
  0xffffffff,
  0xffffffff,
  0xffffffff,
  0xffffffff,
  0xffffffff,
};
static const uint32_t x448__barrett_mu[] = {
  0x00000002,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000001,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000001,
};
static DskTlsMontgomeryInfo x448__mont = {
  14,
  p,
  0x00000001,
  x448__barrett_mu
};

static const uint32_t p_minus_2[] = {
  0xfffffffd,
  0xffffffff,
  0xffffffff,
  0xffffffff,
  0xffffffff,
  0xffffffff,
  0xffffffff,
  0xfffffffe,
  0xffffffff,
  0xffffffff,
  0xffffffff,
  0xffffffff,
  0xffffffff,
  0xffffffff,
};

static void
x448_multiply_karatsuba (const uint32_t *in0,
                         const uint32_t *in1,
                         uint32_t       *out)
{
  const uint32_t *a = in0;
  const uint32_t *b = in0 + 7;
  const uint32_t *c = in1;
  const uint32_t *d = in1 + 7;
  uint32_t ac[14], bd[14];
  uint32_t a_plus_b[8], c_plus_d[8];
  uint32_t a_plus_b_times_c_plus_d[16];
  dsk_tls_bignum_multiply (7, a, 7, c, ac);
  dsk_tls_bignum_multiply (7, b, 7, d, bd);
  a_plus_b[7] = dsk_tls_bignum_add_with_carry (7, a, b, 0, a_plus_b);
  c_plus_d[7] = dsk_tls_bignum_add_with_carry (7, c, d, 0, c_plus_d);
  dsk_tls_bignum_multiply (8, a_plus_b, 8, c_plus_d,
                           a_plus_b_times_c_plus_d);
  a_plus_b_times_c_plus_d[14] -= 
     dsk_tls_bignum_subtract_with_borrow (14,
                                          a_plus_b_times_c_plus_d,
                                          ac,
                                          0,
                                          a_plus_b_times_c_plus_d);
  out[14] = dsk_tls_bignum_add_with_carry (14, ac, bd, 0, out);
  out[15] = out[16] = out[16] = out[17] = out[18] = out[19] = out[20] = out[21] = 0;
  out[22] = dsk_tls_bignum_add_with_carry (15, a_plus_b_times_c_plus_d, out + 7, 0, out + 7);
  out[23] = out[24] = out[25] = out[26] = out[27] = 0;
}
                         
static void
x448_square_karatsuba (const uint32_t *in0,
                       uint32_t       *out)
{
  const uint32_t *a = in0;
  const uint32_t *b = in0 + 7;
  //const uint32_t *c = a;
  //const uint32_t *d = b;
  uint32_t ac[14], bd[14];
  uint32_t ab[14];
  //(a+b)^2 = aa+bb+2ab
  dsk_tls_bignum_square (7, a, ac);
  dsk_tls_bignum_square (7, b, bd);
  dsk_tls_bignum_multiply (7, a, 7, b, ab);
  // (a+b)^2-a^2 = 2ab + b^2
  unsigned carry = 0;
  uint32_t ab2ma2[15];
  for (unsigned i = 0; i < 14; i++)
    {
      uint64_t v = (uint64_t) ab[i] * 2 + bd[i] + carry;
      ab2ma2[i] = v;
      carry = v >> 32;
    }
  ab2ma2[14] = carry;

  out[14] = dsk_tls_bignum_add_with_carry (14, ac, bd, 0, out);
  out[15] = out[16] = out[16] = out[17] = out[18] = out[19] = out[20] = out[21] = 0;
  out[22] = dsk_tls_bignum_add_with_carry (15, ab2ma2, out + 7, 0, out + 7);
  out[23] = out[24] = out[25] = out[26] = out[27] = 0;
}
                         


#if 0
static void pr(const char *name, const uint32_t *arr)
{
  printf("%s=0x",name);
  for (unsigned i = 14; i--; )
    printf("%08x", arr[i]);
  printf("\n");
}
#define PR(var) pr(#var, var)
#endif

static void
x448(const uint32_t *k,
     const uint32_t *u,
     uint32_t *out)
{
  uint32_t x_1[14];
  uint32_t x_2_buf[14] = {1,0,0,0,0,0,0,0,0,0,0,0,0,0};
  uint32_t z_2_buf[14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  uint32_t x_3_buf[14];
  uint32_t z_3_buf[14] = {1,0,0,0,0,0,0,0,0,0,0,0,0,0};
  uint32_t *x_2 = x_2_buf;
  uint32_t *z_2 = z_2_buf;
  uint32_t *x_3 = x_3_buf;
  uint32_t *z_3 = z_3_buf;
  intptr_t swap = 0;

  memcpy (x_1, u, 14*4);
  memcpy (x_3_buf, u, 14*4);

#define CSWAP_XZ_23(swap) { \
  intptr_t mask; \
  mask = (-swap) & ((intptr_t) x_2 ^ (intptr_t) x_3); \
  x_2 = (uint32_t *) ((intptr_t) x_2 ^ mask); \
  x_3 = (uint32_t *) ((intptr_t) x_3 ^ mask); \
  mask = (-swap) & ((intptr_t) z_2 ^ (intptr_t) z_3); \
  z_2 = (uint32_t *) ((intptr_t) z_2 ^ mask); \
  z_3 = (uint32_t *) ((intptr_t) z_3 ^ mask); \
}
#define MOD_TMP(out) \
  dsk_tls_bignum_modulus_with_barrett_mu (14 * 2, tmp, 14, p, \
                                          x448__barrett_mu, \
                                          (out))

  uint32_t A[14], AA[14], B[14], BB[14], E[14], C[14], D[14], DA[14], CB[14];
  uint32_t tmp[28];

  for (unsigned i = 448; i--; )
    {
      intptr_t k_t = (k[i/32] >> (i%32)) & 1;
      swap ^= k_t;
      CSWAP_XZ_23(swap);
      swap = k_t;

      // A = x_2 + z_2
      dsk_tls_bignum_modular_add (14, x_2, z_2, p, A);

      // AA = A * A
      x448_square_karatsuba (A, tmp);
      MOD_TMP(AA);

      // B = x_2 - z_2
      dsk_tls_bignum_modular_subtract (14, x_2, z_2, p, B);

      // BB = B * B
      x448_square_karatsuba (B, tmp);
      MOD_TMP(BB);

      // E = AA - BB
      dsk_tls_bignum_modular_subtract (14, AA, BB, p, E);

      // C = x_3 + z_3
      dsk_tls_bignum_modular_add (14, x_3, z_3, p, C);

      // D = x_3 - z_3
      dsk_tls_bignum_modular_subtract (14, x_3, z_3, p, D);

      // DA = D * A
      x448_multiply_karatsuba (D, A, tmp);
      MOD_TMP(DA);

      // CB = C * B
      x448_multiply_karatsuba (C, B, tmp);
      MOD_TMP(CB);

      // x_3 = (DA + CB)^2
      dsk_tls_bignum_modular_add (14, DA, CB, p, x_3);
      x448_square_karatsuba (x_3, tmp);
      MOD_TMP(x_3);

      // z_3 = x_1 * (DA - CB)^2
      dsk_tls_bignum_modular_subtract (14, DA, CB, p, z_3);
      x448_square_karatsuba (z_3, tmp);
      MOD_TMP(z_3);
      x448_multiply_karatsuba (z_3, x_1, tmp);
      MOD_TMP(z_3);

      // x_2 = AA * BB
      x448_multiply_karatsuba (AA, BB, tmp);
      MOD_TMP(x_2);

      // z_2 = E * (AA + 39081 * E)
      tmp[14] = dsk_tls_bignum_multiply_word (14, E, 39081, tmp);
      uint32_t quot[2];
      dsk_tls_bignum_divide (15, tmp, 14, p, quot, z_2);
      dsk_tls_bignum_modular_add (14, z_2, AA, p, z_2);
      x448_multiply_karatsuba (z_2, E, tmp);
      MOD_TMP(z_2);
    }
  CSWAP_XZ_23(swap);

  // Compute output = x_2 * (z_2 ** (p-2))
  uint32_t z2_mont[14];
  dsk_tls_bignum_to_montgomery (&x448__mont, z_2, z2_mont);
  uint32_t z2_pow_mont[14];
  dsk_tls_bignum_exponent_montgomery (&x448__mont, z2_mont, 14, p_minus_2, z2_pow_mont);
  dsk_tls_bignum_multiply_montgomery (&x448__mont, x_2, z2_pow_mont, out);
}


void
dsk_curve448_private_to_public (const uint8_t *private_key,
                                uint8_t       *public_key_out)
{
  uint32_t k[14];
#if DSK_IS_LITTLE_ENDIAN
  memcpy (k, private_key, 14*4);
#else
  for (unsigned i = 0; i < 14; i++)
    k[i] = dsk_uint32le_parse (private_key + 4*i);
#endif

  uint32_t out[14];
  x448 (k, base_point_u, out);
#if DSK_IS_LITTLE_ENDIAN
  memcpy (public_key_out, out, 14*4);
#else
  for (unsigned i = 0; i < 14; i++)
    dsk_uint32le_pack (out[i], public_key_out + 4*i);
#endif
}

void dsk_curve448_private_to_shared (const uint8_t *private_key,
                                       const uint8_t *other_public_key,
                                       uint8_t       *shared_key_out)
{
  uint32_t k[14], u[14];
#if DSK_IS_LITTLE_ENDIAN
  memcpy (k, private_key, 14*4);
  memcpy (u, other_public_key, 14*4);
#else
  for (unsigned i = 0; i < 14; i++)
    k[i] = dsk_uint32le_parse (private_key + 4*i);
  for (unsigned i = 0; i < 14; i++)
    u[i] = dsk_uint32le_parse (other_public_key + 4*i);
#endif

  uint32_t out[14];
  x448 (k, u, out);
#if DSK_IS_LITTLE_ENDIAN
  memcpy (shared_key_out, out, 14*4);
#else
  for (unsigned i = 0; i < 14; i++)
    dsk_uint32le_pack (out[i], shared_key_out + 4*i);
#endif
}

