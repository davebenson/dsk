
/* See: http://www.secg.org/sec2-v2.pdf
   
   For point compression:
        https://crypto.stackexchange.com/questions/8914/ecdsa-compressed-public-key-point-back-to-uncompressed-public-key-point
 */


static uint32_t secp256r1__p[] = {
  0xFFFFFFFF,
  0xFFFFFFFF,
  0xFFFFFFFF,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000001,
  0xFFFFFFFF,
};
static uint32_t secp256r1__a[] = {
  0xFFFFFFFC,
  0xFFFFFFFF,
  0xFFFFFFFF,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000001,
  0xFFFFFFFF
};
static uint32_t secp256r1__b[] = {
  0x27D2604B,
  0x3BCE3C3E,
  0xCC53B0F6,
  0x651D06B0,
  0x769886BC,
  0xB3EBBD55,
  0xAA3A93E7,
  0x5AC635D8
};
static uint32_t secp256r1__x[] = {
  0xD898C296,
  0xF4A13945,
  0x2DEB33A0,
  0x77037D81,
  0x63A440F2,
  0xF8BCE6E5,
  0xE12C4247,
  0x6B17D1F2
};
static uint32_t secp256r1__y[] = {
  0x37BF51F5,
  0xCBB64068,
  0x6B315ECE,
  0x4FE342E2,
  0xFE1A7F9B,
  0x8EE7EB4A,
  0x7C0F9E16,
  0x2BCE3357
};

static const uint32_t secp256r1__barrett_mu[] = {
  0x00000003,
  0x00000000,
  0xffffffff,
  0xfffffffe,
  0xfffffffe,
  0xfffffffe,
  0xffffffff,
  0x00000000,
  0x00000001,
};

static DskTls_ECPrime_Group secp256r1_params = {
  8,
  secp256r1__p,
  secp256r1__barrett_mu
  secp256r1__a,
  secp256r1__b,
  secp256r1__x,
  secp256r1__y,
};


/*
 * From Understanding Cryptography by Christof Paar.
 * Page 244.
 */
void dsk_tls_ec_prime_add (DskTls_ECPrime_Group *group,
                           const uint32_t *x1,
                           const uint32_t *y1,
                           const uint32_t *x2,
                           const uint32_t *y2,
                           uint32_t *x_out,
                           uint32_t *y_out)
{
  unsigned len = group->len;
  const uint32_t *p = group->p;

  uint32_t *y21 = alloca(4 * len);
  uint32_t *x21 = alloca(4 * len);
  uint32_t *tmp = alloca(4 * len * 2);
  uint32_t *s = alloca(4 * len);
  
  // Given a point, if x==0
  if (dsk_tls_bignum_is_zero (len, x1))
    {
      memcpy (x_out, x2, len*4);
      memcpy (y_out, y2, len*4);
    }
  else if (dsk_tls_bignum_is_zero (len, x2))
    {
      memcpy (x_out, x1, len*4);
      memcpy (y_out, y1, len*4);
    }
  else if (memcmp (x1, x2, 4 * len) == 0)
    {
      if (memcmp (y1, y2, 4 * len) == 0)
        {
          // Point doubling.
          //
          //             3*x1^2 + a
          // Compute s = ----------   (mod p)
          //                2*y1
          dsk_tls_bignum_square (len, x1, tmp);
          dsk_tls_bignum_modulus_with_barrett_mu (len * 2, tmp, len, p, group->mont.barrett_mu, tmp);
          tmp[len] = 0;
          dsk_tls_bignum_multiply_word (len + 1, tmp, 3, tmp);
          while (tmp[len] > 0 || dsk_tls_bignum_compare (len, tmp, p) >= 0)
            tmp[len] -= dsk_tls_bignum_subtract_with_borrow (len, tmp, p, 0, tmp);
          dsk_tls_bignum_modular_add (len, tmp, group->a, p, s);
        }
      else
        {
          // assert y1 = -y2.
          dsk_tls_bignum_add (len, y1, y2, s);
          assert (dsk_tls_bignum_is_zero (len, s));

          // addition with inverse, code as 0,0
          memset(x_out, 0, 4*len);
          memset(y_out, 0, 4*len);
        }
    }
  else
    {
      // Adding distinct points.
      //
      //             y2 - y1
      // Compute s = -------   = (y2 - y1) * (x2 - x1)^{-1}    (mod p)
      //             x2 - x1
      //
      dsk_tls_bignum_modular_subtract (len, y2, y1, p, y21);
      dsk_tls_bignum_modular_subtract (len, x2, x1, p, x21);
      if (!dsk_tls_bignum_modular_inverse (len, x21, p, x21_inv))
        assert(false);
      dsk_tls_bignum_multiply (len, y21, len, x21_inv, tmp);
      dsk_tls_bignum_modulus_with_barrett_mu (len * 2, tmp, len, p, group->mont.barrett_mu, s);
    }

  // Compute x_out := s^2 - x1 - x2 (mod p)
  dsk_tls_bignum_square (len, s, tmp);
  dsk_tls_bignum_modulus_with_barrett_mu (len * 2, tmp, len, p, group->mont.barrett_mu, x_out);
  dsk_tls_bignum_modular_subtract (len, x_out, x1, p, x_out);
  dsk_tls_bignum_modular_subtract (len, x_out, x2, p, x_out);

  // Compute y_out := s*(x1 - x3) - y1
  dsk_tls_bignum_modular_subtract(len, x1, x_out, p, tmp);
  dsk_tls_bignum_multiply(len, tmp, len, s, tmp);
  dsk_tls_bignum_modulus_with_barrett_mu (len * 2, tmp, len, p, group->mont.barrett_mu, y_out);
  dsk_tls_bignum_modular_subtract(len, y_out, y1, p, y_out);
}

void dsk_tls_ec_prime_multiply_int (DskTls_ECPrime_Group *group,
                                    const uint32_t *x,
                                    const uint32_t *y,
                                    unsigned factor_len,
                                    const uint32_t *factor,
                                    uint32_t *x_out,
                                    uint32_t *y_out)
{
  uint32_t *xans = alloca(len * 4);
  uint32_t *yans = alloca(len * 4);
  uint32_t *xans_tmp = alloca(len * 4);
  uint32_t *yans_tmp = alloca(len * 4);
  uint32_t *xcur = alloca(len * 4);
  uint32_t *ycur = alloca(len * 4);
  uint32_t *xcur_tmp = alloca(len * 4);
  uint32_t *ycur_tmp = alloca(len * 4);
  int bit = dsk_tls_bignum_max_bit (factor_len, factor);
  if (bit < 0)
    {
      memset (x_out, 0, 4*len);
      memset (y_out, 0, 4*len);
      return;
    }
  for (unsigned i = 0; i < (unsigned) bit; i++)
    {
      if ((factor[i/32] >> (i % 32)) & 1)
        {
          dsk_tls_ec_prime_add (group, xans, yans, xcur, ycur, xans_tmp, yans_tmp);
          swap = xans; xans = xans_tmp; xans_tmp = swap;
          swap = yans; yans = yans_tmp; yans_tmp = swap;
        }
      dsk_tls_ec_prime_add (group, xcur, ycur, xcur, ycur, xcur_tmp, ycur_tmp);
      swap = xcur; xcur = xcur_tmp; xcur_tmp = swap;
      swap = ycur; ycur = ycur_tmp; ycur_tmp = swap;
    }

  // Last bit is always high (1).
  dsk_tls_ec_prime_add (group, xans, yans, xcur, ycur, x_out, y_out);
}

bool
dsk_tls_ec_prime_y_from_x (DskTls_ECPrime_Group *group,
                                    const uint32_t *x,
                                    uint32_t *y_out)
{
  // Compute x^3 (mod p)
  unsigned N = group->len;
  uint32_t *x_tmp = alloca (N * 4 * 2);
  dsk_tls_bignum_multiply (N, x, N, x, x_tmp);
  uint32_t *x_squared_mod_p = alloca (N * 4);
  dsk_tls_bignum_modulus_with_barrett_mu (N * 2, x_tmp,
                                          N, group->p,
                                          group->barrett_mu,
                                          x_squared_mod_p);

  dsk_tls_bignum_multiply (N, x, N, x_squared_mod_p, x_tmp);
  uint32_t *x_cubed_mod_p = alloca (N * 4);
  dsk_tls_bignum_modulus_with_barrett_mu (N * 2, x_squared,
                                          N, group->p,
                                          group->barrett_mu,
                                          x_cubed_mod_p);

  // Compute a*x mod p.
  dsk_tls_bignum_multiply (N, x, N, group->a, x_tmp);
  uint32_t *ax_mod_p = alloca (N * 4);
  dsk_tls_bignum_modulus_with_barrett_mu (N * 2, x_squared,
                                          N, group->p,
                                          group->barrett_mu,
                                          ax_mod_p);


  // Compute y^2 = x^3 + a*x + b (mod p)
  uint32_t *y_squared = alloca (N * 4);
  uint32_t *x3_ax_mod_p = alloca (N * 4);
  dsk_tls_bignum_modular_add (N, x_cubed_mod_p, ax_mod_p, group->p, x3_ax_mod_p);
  dsk_tls_bignum_modular_add (N, x3_ax_mod_p, group->b, group->p, y_squared);

  if (!dsk_tls_bignum_modular_sqrt (N, y_squared, group->p, y_out))
    return false;
  return true;
}

