
/* See: http://www.secg.org/sec2-v2.pdf
   
   For point compression:
        https://crypto.stackexchange.com/questions/8914/ecdsa-compressed-public-key-point-back-to-uncompressed-public-key-point

   For test vectors: http://point-at-infinity.org/ecc/nisttv

   For faster versions that use projective coordinates:
       http://www.hyperelliptic.org/EFD/g1p/auto-shortw-projective.html#doubling-dbl-2007-bl
 */
#include "../dsk.h"
#include <alloca.h>
#include <string.h>

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
  0x2BCE3357,
  0x7C0F9E16,
  0x8EE7EB4A,
  0xFE1A7F9B,
  0x4FE342E2,
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

static uint32_t secp256r1__xy0[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

const DskTls_ECPrime_Group dsk_tls_ecprime_group_secp256r1 = {
  "SECP256R1",
  8,
  secp256r1__p,
  secp256r1__barrett_mu,
  secp256r1__a,
  secp256r1__b,
  secp256r1__x,
  secp256r1__y,
  secp256r1__xy0,
  true,                 // a is small
  true,                 // a is negative
  3                     // a = -3
};

static const uint32_t secp384r1__p[] = {
  0xFFFFFFFF,
  0x00000000,
  0x00000000,
  0xFFFFFFFF,
  0xFFFFFFFE,
  0xFFFFFFFF,
  0xFFFFFFFF,
  0xFFFFFFFF,
  0xFFFFFFFF,
  0xFFFFFFFF,
  0xFFFFFFFF,
  0xFFFFFFFF,
};

static const uint32_t secp384r1__barrett_mu[] = {
  0x00000001,
  0xffffffff,
  0xffffffff,
  0x00000000,
  0x00000001,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x00000001,
};
static const uint32_t secp384r1__a[] = {
  0xFFFFFFFC,
  0x00000000,
  0x00000000,
  0xFFFFFFFF,
  0xFFFFFFFE,
  0xFFFFFFFF,
  0xFFFFFFFF,
  0xFFFFFFFF,
  0xFFFFFFFF,
  0xFFFFFFFF,
  0xFFFFFFFF,
  0xFFFFFFFF,
};
static const uint32_t secp384r1__b[] = {
  0xD3EC2AEF,
  0x2A85C8ED,
  0x8A2ED19D,
  0xC656398D,
  0x5013875A,
  0x0314088F,
  0xFE814112,
  0x181D9C6E,
  0xE3F82D19,
  0x988E056B,
  0xE23EE7E4,
  0xB3312FA7,
};
static const uint32_t secp384r1__x[] = {
  0x72760AB7,
  0x3A545E38,
  0xBF55296C,
  0x5502F25D,
  0x82542A38,
  0x59F741E0,
  0x8BA79B98,
  0x6E1D3B62,
  0xF320AD74,
  0x8EB1C71E,
  0xBE8B0537,
  0xAA87CA22,
};
static const uint32_t secp384r1__y[] = {
  0x90EA0E5F,
  0x7A431D7C,
  0x1D7E819D,
  0x0A60B1CE,
  0xB5F0B8C0,
  0xE9DA3113,
  0x289A147C,
  0xF8F41DBD,
  0x9292DC29,
  0x5D9E98BF,
  0x96262C6F,
  0x3617DE4A,
};
static const uint32_t secp384r1__xy0[24] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};
const DskTls_ECPrime_Group dsk_tls_ecprime_group_secp384r1 = {
  "SECP384R1",
  12,
  secp384r1__p,
  secp384r1__barrett_mu,
  secp384r1__a,
  secp384r1__b,
  secp384r1__x,
  secp384r1__y,
  secp384r1__xy0,
  true,                 // a is small
  true,                 // a is negative
  3                     // a = -3
};

static const uint32_t secp521r1__p[] = {
0xFFFFFFFF,
0xFFFFFFFF,
0xFFFFFFFF,
0xFFFFFFFF,
0xFFFFFFFF,
0xFFFFFFFF,
0xFFFFFFFF,
0xFFFFFFFF,
0xFFFFFFFF,
0xFFFFFFFF,
0xFFFFFFFF,
0xFFFFFFFF,
0xFFFFFFFF,
0xFFFFFFFF,
0xFFFFFFFF,
0xFFFFFFFF,
0x000001FF,
};

static const uint32_t secp521r1__barrett_mu[] = {
  0x00000000,
  0x00004000,
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
  0x00000000,
  0x00000000,
  0x00800000,
};
static const uint32_t secp521r1__a[] = {
0xFFFFFFFC,
0xFFFFFFFF,
0xFFFFFFFF,
0xFFFFFFFF,
0xFFFFFFFF,
0xFFFFFFFF,
0xFFFFFFFF,
0xFFFFFFFF,
0xFFFFFFFF,
0xFFFFFFFF,
0xFFFFFFFF,
0xFFFFFFFF,
0xFFFFFFFF,
0xFFFFFFFF,
0xFFFFFFFF,
0xFFFFFFFF,
0x000001FF,
};
static const uint32_t secp521r1__b[] = {
0x6B503F00,
0xEF451FD4,
0x3D2C34F1,
0x3573DF88,
0x3BB1BF07,
0x1652C0BD,
0xEC7E937B,
0x56193951,
0x8EF109E1,
0xB8B48991,
0x99B315F3,
0xA2DA725B,
0xB68540EE,
0x929A21A0,
0x8E1C9A1F,
0x953EB961,
0x00000051,
};
static const uint32_t secp521r1__x[] = {
0xC2E5BD66,
0xF97E7E31,
0x856A429B,
0x3348B3C1,
0xA2FFA8DE,
0xFE1DC127,
0xEFE75928,
0xA14B5E77,
0x6B4D3DBA,
0xF828AF60,
0x053FB521,
0x9C648139,
0x2395B442,
0x9E3ECB66,
0x0404E9CD,
0x858E06B7,
0x000000C6,
};
static const uint32_t secp521r1__y[] = {
0x9FD16650,
0x88BE9476,
0xA272C240,
0x353C7086,
0x3FAD0761,
0xC550B901,
0x5EF42640,
0x97EE7299,
0x273E662C,
0x17AFBD17,
0x579B4468,
0x98F54449,
0x2C7D1BD9,
0x5C8A5FB4,
0x9A3BC004,
0x39296A78,
0x00000118,
};
static const uint32_t secp521r1__xy0[34] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};
const DskTls_ECPrime_Group dsk_tls_ecprime_group_secp521r1 = {
  "SECP521R1",
  17,
  secp521r1__p,
  secp521r1__barrett_mu,
  secp521r1__a,
  secp521r1__b,
  secp521r1__x,
  secp521r1__y,
  secp521r1__xy0,
  true,                 // a is small
  true,                 // a is negative
  3                     // a = -3
};
bool dsk_tls_ecprime_is_zero (const DskTls_ECPrime_Group *group,
                              const uint32_t *x,
                              const uint32_t *y)
{
  return memcmp (x, group->xy0, group->len * 4) == 0
      && memcmp (y, group->xy0 + group->len, group->len * 4) == 0;
}

/*
 * From Understanding Cryptography by Christof Paar.
 * Page 244.
 */
void dsk_tls_ecprime_add (const DskTls_ECPrime_Group *group,
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
  if (dsk_tls_ecprime_is_zero (group, x1, y1))
    {
      memcpy (x_out, x2, len*4);
      memcpy (y_out, y2, len*4);
      return;
    }
  if (dsk_tls_ecprime_is_zero (group, x2, y2))
    {
      memcpy (x_out, x1, len*4);
      memcpy (y_out, y1, len*4);
      return;
    }

  if (memcmp (x1, x2, 4 * len) == 0)
    {
      if (memcmp (y1, y2, 4 * len) == 0)
        {
          // Point doubling.
          //
          //             3*x1^2 + a
          // Compute s = ----------   (mod p)
          //                2*y1
          uint32_t *numer = alloca(4 * len);
          uint32_t *denom = alloca(4 * len);
          dsk_tls_bignum_square (len, x1, tmp);
          dsk_tls_bignum_modulus_with_barrett_mu (len * 2, tmp, len, p, group->barrett_mu, tmp);
          tmp[len] = dsk_tls_bignum_multiply_word (len, tmp, 3, tmp);
          while (tmp[len] > 0 || dsk_tls_bignum_compare (len, tmp, p) >= 0)
            tmp[len] -= dsk_tls_bignum_subtract_with_borrow (len, tmp, p, 0, tmp);
          dsk_tls_bignum_modular_add (len, tmp, group->a, p, numer);

          tmp[len] = dsk_tls_bignum_multiply_word (len, y1, 2, tmp);
          if (tmp[len] > 0 || dsk_tls_bignum_compare (len, tmp, p) >= 0)
            tmp[len] -= dsk_tls_bignum_subtract_with_borrow (len, tmp, p, 0, tmp);
          if (!dsk_tls_bignum_modular_inverse (len, tmp, p, denom))
            assert(false);
          
          dsk_tls_bignum_multiply (len, numer, len, denom, tmp);
          dsk_tls_bignum_modulus_with_barrett_mu (len * 2, tmp, len, p, group->barrett_mu, s);
        }
      else
        {
          // assert y1 = -y2.
          dsk_tls_bignum_add (len, y1, y2, s);
          assert (dsk_tls_bignum_compare (len, s, p) == 0);

          // addition with inverse, code as 0,0
          memset(x_out, 0, 4*len);
          memset(y_out, 0, 4*len);
          return;
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
      uint32_t *x21_inv = alloca (4 * len);
      dsk_tls_bignum_modular_subtract (len, y2, y1, p, y21);
      dsk_tls_bignum_modular_subtract (len, x2, x1, p, x21);
      if (!dsk_tls_bignum_modular_inverse (len, x21, p, x21_inv))
        assert(false);
      dsk_tls_bignum_multiply (len, y21, len, x21_inv, tmp);
      dsk_tls_bignum_modulus_with_barrett_mu (len * 2, tmp, len, p, group->barrett_mu, s);
    }
 
  // Compute x_out := s^2 - x1 - x2 (mod p)
  dsk_tls_bignum_square (len, s, tmp);
  dsk_tls_bignum_modulus_with_barrett_mu (len * 2, tmp, len, p, group->barrett_mu, x_out);
  dsk_tls_bignum_modular_subtract (len, x_out, x1, p, x_out);
  dsk_tls_bignum_modular_subtract (len, x_out, x2, p, x_out);

  // Compute y_out := s*(x1 - x3) - y1
  dsk_tls_bignum_modular_subtract(len, x1, x_out, p, tmp);
  dsk_tls_bignum_multiply(len, tmp, len, s, tmp);
  dsk_tls_bignum_modulus_with_barrett_mu (len * 2, tmp, len, p, group->barrett_mu, y_out);
  dsk_tls_bignum_modular_subtract(len, y_out, y1, p, y_out);
}

#define dsk_tls_ecprime_double(group, x, y, xout, yout) \
  dsk_tls_ecprime_add(group, x, y, x, y, xout, yout)


// Factor must be less than the group size.
void dsk_tls_ecprime_multiply_int (const DskTls_ECPrime_Group *group,
                                    const uint32_t *x,
                                    const uint32_t *y,
                                    unsigned factor_len,
                                    const uint32_t *factor,
                                    uint32_t *x_out,
                                    uint32_t *y_out)
{
  unsigned len = group->len;
  uint32_t *ans = alloca(3 * len * 4);
  uint32_t *ans_tmp = alloca(3 * len * 4);
  uint32_t *cur = alloca(3 * len * 4);
  uint32_t *cur_tmp = alloca(3 * len * 4);
  int bit = dsk_tls_bignum_max_bit (factor_len, factor);
  if (bit < 0)
    {
      memcpy (x_out, group->xy0, 4*len);
      memcpy (y_out, group->xy0 + len, 4*len);
      return;
    }
  memcpy (cur, x, 4 * len);
  memcpy (cur + len,  y, 4 * len);
  cur[len*2] = 1;
  memset (cur + len * 2 + 1, 0, 4 * (len - 1));

  bool has_ans = false;
  for (unsigned i = 0; i < (unsigned) bit; i++)
    {
      uint32_t *swap;
      if ((factor[i/32] >> (i % 32)) & 1)
        {
          if (has_ans)
            {
              dsk_tls_ecprime_prj_add (group, ans, cur, ans_tmp);
              swap = ans; ans = ans_tmp; ans_tmp = swap;
            }
          else
            {
              memcpy (ans, cur, 3 * 4 * len);
              has_ans = true;
            }
        }
      dsk_tls_ecprime_prj_double (group, cur, cur_tmp);
      swap = cur; cur = cur_tmp; cur_tmp = swap;
    }

  // Last bit is always high (1).
  if (has_ans)
    dsk_tls_ecprime_prj_add (group, ans, cur, cur_tmp);
  else
    cur_tmp = cur;

  dsk_tls_ecprime_prj_to_xy (group, cur_tmp, x_out, y_out);
}

bool
dsk_tls_ecprime_y_from_x (const DskTls_ECPrime_Group *group,
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
  dsk_tls_bignum_modulus_with_barrett_mu (N * 2, x_tmp,
                                          N, group->p,
                                          group->barrett_mu,
                                          x_cubed_mod_p);

  // Compute a*x mod p.
  dsk_tls_bignum_multiply (N, x, N, group->a, x_tmp);
  uint32_t *ax_mod_p = alloca (N * 4);
  dsk_tls_bignum_modulus_with_barrett_mu (N * 2, x_tmp,
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

//
// dbl-2007-bl
//
// Source: 2007 Bernstein–Lange.
//
void
dsk_tls_ecprime_prj_double (const DskTls_ECPrime_Group *group,
                            const uint32_t *xyz,
                            uint32_t *xyz_out)
{
  unsigned len = group->len;
  uint32_t *tmp = alloca(4 * len * 2);
  const uint32_t *X1 = xyz;
  const uint32_t *Y1 = xyz + len;
  const uint32_t *Z1 = xyz + len*2;
  uint32_t *X3 = xyz_out;
  uint32_t *Y3 = xyz_out + len;
  uint32_t *Z3 = xyz_out + len*2;
#define MOD_TMP(out) \
  dsk_tls_bignum_modulus_with_barrett_mu (len * 2, tmp, len, group->p, \
                                          group->barrett_mu, \
                                          (out))
  uint32_t *xx = alloca(4 * len);
  dsk_tls_bignum_square (len, X1, tmp);
  MOD_TMP(xx);

  uint32_t *zz = alloca(4 * len);
  dsk_tls_bignum_square (len, Z1, tmp);
  MOD_TMP(zz);

  uint32_t *w = alloca(4 * len);
  if (group->is_a_small)
    {
      uint32_t quot[2];
      if (group->is_a_negative)
        {
          uint32_t *wleft = alloca (4 * (len+1));
          uint32_t *wright = alloca (4 * (len+1));
          wleft[len] = dsk_tls_bignum_multiply_word (len, zz, group->small_a, wleft);
          wright[len] = dsk_tls_bignum_multiply_word (len, xx, 3, wright);
          if (dsk_tls_bignum_compare (len + 1, wleft, wright) <= 0)
            {
              dsk_tls_bignum_subtract (len + 1, wright, wleft, wright);
              dsk_tls_bignum_divide (len + 1, wright, len, group->p, quot, w);
            }
          else
            {
              dsk_tls_bignum_subtract (len + 1, wleft, wright, wright);
              dsk_tls_bignum_divide (len + 1, wright, len, group->p, quot, w);
              if (!dsk_tls_bignum_is_zero (len, w))
                dsk_tls_bignum_subtract (len, group->p, w, w);
            }
        }
      else
        {
          uint32_t *wleft = alloca (4 * (len+1));
          uint32_t *wright = alloca (4 * (len+1));
          wleft[len] = dsk_tls_bignum_multiply_word (len, zz, group->small_a, wleft);
          wright[len] = dsk_tls_bignum_multiply_word (len, xx, 3, wright);
          dsk_tls_bignum_add (len+1, wleft, wright, wright);
          dsk_tls_bignum_divide (len + 1, wright, len, group->p, quot, w);
        }
    }
  else
    {
      dsk_tls_bignum_multiply (group->len, group->a, group->len, zz, tmp);
      dsk_tls_bignum_modulus_with_barrett_mu (len * 2, tmp, len, group->p,
                                              group->barrett_mu,
                                              w);
      dsk_tls_bignum_modular_add (len, w, xx, group->p, w);
      dsk_tls_bignum_modular_add (len, w, xx, group->p, w);
      dsk_tls_bignum_modular_add (len, w, xx, group->p, w);
    }

  uint32_t *s = alloca (4 * len);
  dsk_tls_bignum_multiply (len, Y1, len, Z1, tmp);
  MOD_TMP(s);
  dsk_tls_bignum_modular_add (len, s, s, group->p, s);

  uint32_t *ss = alloca (4 * len);
  dsk_tls_bignum_square (len, s, tmp);
  MOD_TMP(ss);

  // Optimization: store 'sss' in 'Z3'.
  dsk_tls_bignum_multiply (len, s, len, ss, tmp);
  MOD_TMP(Z3);

  uint32_t *R = alloca (4 * len);
  dsk_tls_bignum_multiply (len, Y1, len, s, tmp);
  MOD_TMP(R);

  uint32_t *RR = alloca (4 * len);
  dsk_tls_bignum_square (len, R, tmp);
  MOD_TMP(RR);

  uint32_t *B = alloca (4 * len);
  dsk_tls_bignum_modular_add (len, X1, R, group->p, B);
  dsk_tls_bignum_square (len, B, tmp);
  MOD_TMP (B);
  dsk_tls_bignum_modular_subtract (len, B, xx, group->p, B);
  dsk_tls_bignum_modular_subtract (len, B, RR, group->p, B);

  uint32_t *h = alloca (4 * len);
  dsk_tls_bignum_square (len, w, tmp);
  MOD_TMP(h);
  dsk_tls_bignum_modular_subtract (len, h, B, group->p, h);
  dsk_tls_bignum_modular_subtract (len, h, B, group->p, h);

  // X3 = h*s
  dsk_tls_bignum_multiply (len, h, len, s, tmp);
  MOD_TMP(X3);

  // Y3 = w*(B-h) - 2*RR
  dsk_tls_bignum_modular_subtract (len, B, h, group->p, B);  // B' = B-h
  dsk_tls_bignum_multiply (len, B, len, w, tmp);
  MOD_TMP(B);
  dsk_tls_bignum_modular_add (len, RR, RR, group->p, RR); // RR' := 2*RR
  dsk_tls_bignum_modular_subtract (len, B, RR, group->p, Y3);

  // Z3 = sss

#undef MOD_TMP
}

// From http://www.hyperelliptic.org/EFD/g1p/auto-shortw-projective.html
//
// "add-1998-cmo-2"
//
// Source: 1998 Cohen–Miyaji–Ono "Efficient elliptic curve exponentiation
// using mixed coordinates", formula (3),
// plus common-subexpression elimination.

void dsk_tls_ecprime_prj_add   (const DskTls_ECPrime_Group *group,
                                const uint32_t *xyz_prj_1,
                                const uint32_t *xyz_prj_2,
                                uint32_t *xyz_prj_out)
{
  unsigned len = group->len;
  const uint32_t *X1 = xyz_prj_1;
  const uint32_t *Y1 = xyz_prj_1 + len;
  const uint32_t *Z1 = xyz_prj_1 + len*2;
  const uint32_t *X2 = xyz_prj_2;
  const uint32_t *Y2 = xyz_prj_2 + len;
  const uint32_t *Z2 = xyz_prj_2 + len*2;
  uint32_t *X3 = xyz_prj_out;
  uint32_t *Y3 = xyz_prj_out + len;
  uint32_t *Z3 = xyz_prj_out + len*2;
  uint32_t *tmp = alloca(4 * len * 2);
  uint32_t *tmp2 = alloca(4 * len);

  #define MOD_TMP(out) \
  dsk_tls_bignum_modulus_with_barrett_mu (len * 2, tmp, len, group->p, \
                                          group->barrett_mu, \
                                          (out))

  uint32_t *X1Z2 = alloca(4 * len);
  dsk_tls_bignum_multiply (len, X1, len, Z2, tmp);
  MOD_TMP(X1Z2);

  uint32_t *Y1Z2 = alloca(4 * len);
  dsk_tls_bignum_multiply (len, Y1, len, Z2, tmp);
  MOD_TMP(Y1Z2);

  uint32_t *Z1Z2 = alloca(4 * len);
  dsk_tls_bignum_multiply (len, Z1, len, Z2, tmp);
  MOD_TMP(Z1Z2);

  uint32_t *u = alloca(4 * len);
  dsk_tls_bignum_multiply (len, Y2, len, Z1, tmp);
  MOD_TMP(u);
  dsk_tls_bignum_multiply (len, Y1, len, Z2, tmp);
  MOD_TMP(tmp2);
  dsk_tls_bignum_modular_subtract (len, u, tmp2, group->p, u);

  uint32_t *uu = alloca(4 * len);
  dsk_tls_bignum_square (len, u, tmp);
  MOD_TMP(uu);

  uint32_t *v = alloca(4 * len);
  dsk_tls_bignum_multiply (len, X2, len, Z1, tmp);
  MOD_TMP(v);
  dsk_tls_bignum_multiply (len, X1, len, Z2, tmp);
  MOD_TMP(tmp2);
  dsk_tls_bignum_modular_subtract (len, v, tmp2, group->p, v);

  uint32_t *vv = alloca(4 * len);
  dsk_tls_bignum_square (len, v, tmp);
  MOD_TMP(vv);

  uint32_t *vvv = alloca(4 * len);
  dsk_tls_bignum_multiply (len, v, len, vv, tmp);
  MOD_TMP(vvv);

  uint32_t *R = alloca(4 * len);
  dsk_tls_bignum_multiply (len, vv, len, X1Z2, tmp);
  MOD_TMP(R);

  uint32_t *A = alloca(4 * len);
  dsk_tls_bignum_multiply (len, uu, len, Z1Z2, tmp);
  MOD_TMP(A);
  dsk_tls_bignum_modular_subtract (len, A, vvv, group->p, A);
  dsk_tls_bignum_modular_subtract (len, A, R, group->p, A);
  dsk_tls_bignum_modular_subtract (len, A, R, group->p, A);

  dsk_tls_bignum_multiply (len, v, len, A, tmp);
  MOD_TMP(X3);

  dsk_tls_bignum_modular_subtract (len, R, A, group->p, R);
  dsk_tls_bignum_multiply (len, u, len, R, tmp);
  MOD_TMP(Y3);
  dsk_tls_bignum_multiply (len, vvv, len, Y1Z2, tmp);
  MOD_TMP(tmp2);
  dsk_tls_bignum_modular_subtract (len, Y3, tmp2, group->p, Y3);

  dsk_tls_bignum_multiply (len, vvv, len, Z1Z2, tmp);
  MOD_TMP(Z3);
}

void dsk_tls_ecprime_prj_to_xy (const DskTls_ECPrime_Group *group,
                                const uint32_t *xyz_prj,
                                uint32_t *x,
                                uint32_t *y)
{
  unsigned len = group->len;
  uint32_t *zinv = alloca(4 * len);
  uint32_t *tmp = alloca(2 * 4 * len);
  dsk_tls_bignum_modular_inverse (len, xyz_prj + 2*len, group->p, zinv);
  dsk_tls_bignum_multiply (len, zinv, len, xyz_prj, tmp);
  MOD_TMP (x);
  dsk_tls_bignum_multiply (len, zinv, len, xyz_prj + len, tmp);
  MOD_TMP (y);
}
