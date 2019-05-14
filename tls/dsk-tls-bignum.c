//
// See http://cacr.uwaterloo.ca/hac/about/chap14.pdf
//
#include "../dsk.h"
#include <stdlib.h>
#include <string.h>

uint32_t 
dsk_tls_bignum_subtract_with_borrow (unsigned        len,
                                     const uint32_t *a,
                                     const uint32_t *b,
                                     uint32_t        borrow,
                                     uint32_t       *out)
{
  for (unsigned i = 0; i < len; i++)
    {
      uint32_t a_value = a[i];
      uint32_t a_minus_b_value = a_value - b[i];
      uint32_t a_minus_b_minus_borrow_value = a_minus_b_value - borrow;
      borrow = (a_minus_b_value > a_value || a_minus_b_minus_borrow_value > a_minus_b_value);
      out[i] = a_minus_b_minus_borrow_value;
    }
  return borrow;
} 

uint32_t
dsk_tls_bignum_add_with_carry (unsigned   len,
                               const uint32_t *a,
                               const uint32_t *b,
                               uint32_t carry,
                               uint32_t *out)
{
  for (unsigned i = 0; i < len; i++)
    {
      uint32_t a_value = a[i];
      uint32_t a_plus_b_value = a_value + b[i];
      uint32_t a_plus_b_plus_carry_value = a_plus_b_value + carry;
      carry = (a_plus_b_value < a_value || a_plus_b_plus_carry_value < a_plus_b_value);
      out[i] = a_plus_b_plus_carry_value;
    }
  return carry;
}

void dsk_tls_bignum_multiply (unsigned p_len,
                              const uint32_t *p_words,
                              unsigned q_len,
                              const uint32_t *q_words,
                              uint32_t *out)
{
  uint64_t *scratch = alloca (8 * (p_len + q_len));
  memset (scratch, 0, 8 * (p_len + q_len));
  for (unsigned i = 0; i < p_len; i++)
    for (unsigned j = 0; j < q_len; j++)
      {
        uint64_t prod = (uint64_t) p_words[i] * q_words[j];
        scratch[i+j] += prod & 0xffffffff;
        scratch[i+j+1] += prod >> 32;
      }
  uint64_t carry = scratch[0] >> 32;
  out[0] = scratch[0];
  for (unsigned i = 1; i < p_len + q_len; i++)
    {
      uint64_t tmp = scratch[i] + carry;
      out[i] = tmp;
      carry = tmp >> 32;
    }
  assert (carry == 0);
}
#if 0
void dsk_tls_bignum_multiply_samesize (unsigned len,
                              const uint32_t *a_words,
                              const uint32_t *b_words,
                              uint32_t *product_words_out)
{
  // we don't care about the highest order bit,
  // but treating it specially is probably more expensive
  // so we compute it anyways.
  uint64_t *scratch = alloca (8 * (len+1));
  memset (scratch, 0, 8 * (len + 1));
  for (unsigned i = 0; i < len; i++)
    for (unsigned j = 0; j < len - i; j++)
      {
        uint64_t prod = (uint64_t) a_words[i] * b_words[j];
        scratch[i+j] += prod & 0xffffffff;
        scratch[i+j+1] += prod >> 32;
      }
  uint64_t carry = scratch[0] >> 32;
  product_words_out[0] = scratch[0];
  for (unsigned i = 1; i < len; i++)
    {
      uint64_t tmp = scratch[i] + carry;
      product_words_out[i] = tmp;
      carry = tmp >> 32;
    }
}
#endif

void dsk_tls_bignum_multiply_truncated (unsigned a_len,
                                       const uint32_t *a_words,
                                       unsigned b_len,
                                       const uint32_t *b_words,
                                       unsigned out_len,
                                       uint32_t *product_words_out)
{
  uint64_t *scratch = alloca (8 * out_len);
  memset (scratch, 0, 8 * out_len);
  unsigned a_max = a_len < out_len ? a_len : out_len;
  for (unsigned i = 0; i < a_max; i++)
    {
      unsigned j;
      unsigned out_b_2word_max = out_len - i - 1;
      unsigned loop_b_max = b_len < out_b_2word_max ? b_len : out_b_2word_max;
      for (j = 0; j < loop_b_max; j++)
        {
          uint64_t prod = (uint64_t) a_words[i] * b_words[j];
          scratch[i+j] += prod & 0xffffffff;
          scratch[i+j+1] += prod >> 32;
        }
      if (j < b_len && i + j < out_len)
        {
          uint64_t prod = (uint64_t) a_words[i] * b_words[j];
          scratch[i+j] += prod & 0xffffffff;
        }
    }

  uint64_t carry = scratch[0] >> 32;
  product_words_out[0] = scratch[0];
  for (unsigned i = 1; i < out_len; i++)
    {
      uint64_t tmp = scratch[i] + carry;
      product_words_out[i] = tmp;
      carry = tmp >> 32;
    }
}

int
dsk_tls_bignum_compare (unsigned len,
                        const uint32_t *a,
                        const uint32_t *b)
{
  const uint32_t *a_at = a + len - 1;
  const uint32_t *b_at = b + len - 1;
  while (a_at >= a)
    {
      if (*a_at > *b_at)
        return 1;
      if (*a_at < *b_at)
        return -1;
      a_at--;
      b_at--;
    }
  return 0;
}

uint32_t dsk_tls_bignum_add_word (unsigned len, uint32_t *v, uint32_t carry)
{
  for (unsigned i = 0; i < len; i++)
    {
      *v += carry;
      if (*v >= carry)
        return 0;
      v++;
      carry = 1;
    }
  return carry;
}

void
dsk_tls_bignum_divide_1 (unsigned x_len,
                         const uint32_t *x_words,
                         uint32_t y,
                         uint32_t *quotient_out,
                         uint32_t *remainder_out)
{
  uint32_t *remaining = alloca (x_len * 4);
  memcpy (remaining, x_words, x_len * 4);

  // handle highest digit
  quotient_out[x_len - 1] = x_words[x_len - 1] / y;
  remaining[x_len - 1] %= y;

  for (int q_digit = x_len - 2; q_digit >= 0; q_digit--)
    {
      uint32_t higher = remaining[q_digit + 1];
      uint32_t rem = remaining[q_digit];
      uint64_t v = ((uint64_t) higher << 32) | rem;
      quotient_out[q_digit] = v / y;
      remaining[q_digit + 1] = 0;
      remaining[q_digit] = v % y;
    }
  *remainder_out = remaining[0];
}

static uint32_t
do_scaled_subtraction (unsigned len,
                       const uint32_t *sub,
                       uint32_t *inout,
                       uint32_t scale)
{
  uint32_t borrow = 0;
  for (unsigned i = 0; i < len; i++)
    {
      uint64_t s = (uint64_t)(*sub++) * scale;
      uint32_t s_lo = s;
      uint32_t s_hi = s >> 32;
      uint32_t io = *inout;
      uint32_t io_m_s_lo = io - s_lo;
      *inout = io_m_s_lo - borrow;
      if (*inout > io_m_s_lo)
        s_hi++;
      if (io_m_s_lo > io)
        s_hi++;
      borrow = s_hi;
      inout++;
    }
  return borrow;
}

void
dsk_tls_bignum_divide  (unsigned x_len,
                        const uint32_t *x_words,
                        unsigned y_len,
                        const uint32_t *y_words,
                        uint32_t *quotient_out,
                        uint32_t *remainder_out)
{
  uint32_t *remaining = alloca (x_len * 4);
  memcpy (remaining, x_words, x_len * 4);

  // for trial subtractions
  uint32_t *tmp = alloca (y_len * 4);

  /* Prepare for trial division. */
  assert(y_len > 0);
  assert(y_words[y_len - 1] > 0);
  if (y_len == 1)
    {
      dsk_tls_bignum_divide_1 (x_len, x_words, y_words[0], quotient_out, remainder_out);
      return;
    }

  //
  // Compute the high 32-bits of y, not necessarily word-aligned.
  //
  // This is used for computing trial divisions to within 1.
  //
  uint32_t y_hi = y_words[y_len - 1];
  uint64_t y_hi_shifted = y_hi;
  uint32_t shift_in = y_words[y_len - 2];
  unsigned shift = 0;
  while ((y_hi_shifted & 0x80000000) == 0)
    {
      y_hi_shifted <<= 1;
      y_hi_shifted |= shift_in >> 31;
      shift_in <<= 1;
      shift++;
    }

  // this +1 can cause y_hi_shifted to overflow into 1<<32,
  // so y_hi_shifted must be a uint64.
  y_hi_shifted += 1;

  if (x_len == 2 && y_len == 2)
    {
      uint64_t X = ((uint64_t)x_words[1] << 32) | x_words[0];
      uint64_t Y = ((uint64_t)y_words[1] << 32) | y_words[0];
      uint64_t Q = X / Y;
      uint64_t R = X % Y;
      quotient_out[0] = Q;
      quotient_out[1] = Q >> 32;
      remainder_out[0] = R;
      remainder_out[1] = R >> 32;
      return;
    }

  int q_digit = x_len - y_len;
  {
    // handle highest digit
    uint32_t rem = remaining[q_digit + y_len - 1];
    if (rem < y_hi)
      {
        quotient_out[q_digit] = 0;
      }
    else
      {
        uint64_t x_shifted = ((uint64_t) rem << shift)
                           | (remaining[q_digit + y_len - 2] >> (32-shift));
        uint32_t q = x_shifted / y_hi_shifted;
        // do scaled subtract from remainder
        uint32_t borrow = do_scaled_subtraction (y_len, y_words, remaining + (x_len - y_len), q);
        assert(borrow == 0);
        // do trial subtraction, use result if borrow not necessary.
        if (dsk_tls_bignum_subtract_with_borrow (y_len, remaining + (x_len - y_len), y_words, 0, tmp) == 0)
          {
            memcpy (remaining + (x_len - y_len), tmp, y_len * 4);
            q++;
          }
        quotient_out[q_digit] = q;
      }
  }
  for (q_digit = x_len - y_len - 1; q_digit >= 0; q_digit--)
    {
      uint32_t higher = remaining[q_digit + y_len];
      uint32_t rem = remaining[q_digit + y_len - 1];
      if (rem < y_hi && higher == 0)
        {
          quotient_out[q_digit] = 0;
        }
      else
        {
          uint64_t x_shifted = ((uint64_t)higher << (shift+32))
                             | ((uint64_t) rem << shift)
                             | ((uint64_t) remaining[q_digit + y_len - 2] >> (32-shift));
          uint32_t q = x_shifted / y_hi_shifted;
          // do scaled subtract from remainder
          uint32_t borrow = do_scaled_subtraction (y_len, y_words, remaining + q_digit, q);
          assert(borrow <= remaining[q_digit + y_len]);
          remaining[q_digit + y_len] -= borrow;

          // do trial subtraction, use result if borrow not necessary.
          int ccc=0;
          for (;;)
            {
            borrow = dsk_tls_bignum_subtract_with_borrow (y_len, remaining + q_digit, y_words, 0, tmp);
            if (borrow <= remaining[q_digit + y_len])
              {
                memcpy (remaining + q_digit, tmp, y_len * 4);
                assert(remaining[q_digit + y_len] == borrow);
                remaining[q_digit + y_len] = 0;           // Not necessary
                q++;
                ccc++;
              }
            else
              break;
            }
          quotient_out[q_digit] = q;
        }
    }
  memcpy (remainder_out, remaining, 4 * y_len);
}

static void do_shiftleft32(unsigned len,
                           const uint32_t *x,
                           unsigned shift,
                           uint32_t *out)
{
  unsigned shift_mod_32 = shift % 32;
  uint32_t *o = out;
  for (unsigned i = 0; i < shift / 32; i++)
    *o++ = 0;
  if (shift_mod_32 == 0)
    {
      memcpy (out, x, (len - shift / 32) * 4);
    }
  else
    {
      const uint32_t *d = x;
      // fill in low-order 0s
      uint32_t last_d = 0;
      while (o < out + len)
        {
          *o++ = (last_d >> (32 - shift_mod_32))
               | (*d << shift_mod_32);
          last_d = *d++;
        }
    }
}


static inline unsigned
highest_bit_in_uint32 (uint32_t v)
{
  for (int b = 31; b >= 0; b++)
    if (v >> b)
      return b;
  assert(false);
  return -1;
}

static unsigned find_highest_set_bit (unsigned len, const uint32_t *v)
{
  for (int i = len - 1; i >= 0; i--)
    if (v[i])
      return highest_bit_in_uint32 (v[i]) + i * 32;
  assert(0);
  return -1;
}

static bool
is_zero (unsigned len, const uint32_t *v)
{
  for (unsigned i = 0; i < len; i++)
    if (v[i])
      return false;
  return true;
}
static void
shiftright32_inplace_1 (unsigned len,
                       uint32_t *inout)
{
  uint32_t carry_bit = 0;
  for (unsigned i = 0; i < len; i++)
    {
      uint32_t next_carry_bit = inout[i] & 1;
      inout[i] >>= 1;
      inout[i] |= carry_bit;
      carry_bit = next_carry_bit;
    }
  assert (carry_bit == 0);
}

void dsk_tls_bignum_divide_samesize   (unsigned len,
                                       const uint32_t *numer_words,
                                       const uint32_t *denom_words,
                                       uint32_t *quotient_words,
                                       uint32_t *modulus_words)
{

  uint32_t *cur = alloca (len * 4);
  memcpy (modulus_words, numer_words, len * 4);
  memcpy (quotient_words, 0, len * 4);

  // this next function asserts that denom != 0
  unsigned hi_bit = find_highest_set_bit (len, denom_words);

  // Note that this formula 
  // Set cur to denom_words << (32*len-1 - hi_bit).
  unsigned shift = 32 * len - 1 - hi_bit;
  do_shiftleft32 (len, denom_words, shift, cur);
  for (int b = shift; b >= 0; b--)
    {
      for (unsigned i = 0; i < len; i++)
        {
          if (modulus_words[len - 1 - i] > cur[len - 1 - i])
            break;
          else if (modulus_words[len - 1 - i] < cur[len - 1 - i])
            goto done_bit;                      // this bit of output is 0
        }

      // Insert a bit into the quotient and adjust the modulus.
      quotient_words[b/32] |= 1 << (b%32);
      dsk_tls_bignum_subtract_with_borrow (len, modulus_words, cur, 0, modulus_words);

done_bit:
      // prepare for next bit.
      shiftright32_inplace_1 (len, cur);
    }
}

static void
do_negate32 (unsigned len, uint32_t *v)
{
  for (unsigned i = 0; i < len; i++)
    v[i] = ~v[i];
  for (unsigned i = 0; i < len; i++)
    {
      v[i] += 1;
      if (v[i] != 0)
        break;
    }
}

#if 0
// Compute out = a + scale * b,
//
// where out, a, b may be negated,
// per their "_signed" parameters.
static void
add_signed_product (unsigned   len,
                  bool       a_signed,
                  uint32_t  *a,
                  uint32_t  *scale,
                  bool       b_signed,
                  uint32_t  *b,
                  bool      *out_signed,
                  uint32_t  *out)
{
  uint32_t *tmp = alloca (len * 4);
  dsk_tls_bignum_multiply_samesize (len, scale, b, tmp);
  if (a_signed != b_signed)
    {
      // Compute out = a - tmp, ignoring signs.
      if (dsk_tls_bignum_subtract_with_borrow (len, a, tmp, 0, out))
        {
          // In this case, |a| < |b*scale|
          // Negate out and copy sign from b.
          do_negate32 (len, out);
          *out_signed = b_signed;
        }
      else
        {
          // |a| >= |b*scale|.   Ideally we'd avoid returning -0, but we don't care or guard against it.
          *out_signed = a_signed;
        }
    }
  else
    {
      dsk_tls_bignum_add_with_carry (len, a, tmp, 0, out);
      *out_signed = a_signed;   // == b_signed
    }
}
#endif

unsigned
dsk_tls_bignum_actual_len (unsigned len, const uint32_t *v)
{
  while (len > 0 && v[len-1] == 0)
    len--;
  return len;
}

void
dsk_tls_bignum_shiftleft (unsigned in_size,
                          const uint32_t *in,
                          unsigned out_size,
                          const uint32_t *out,
                          unsigned amount);

void
dsk_tls_bignum_negate (unsigned len, const uint32_t *in, uint32_t *out)
{
  for (unsigned i = 0; i < len; i++)
    out[i] = ~in[i];
  dsk_tls_bignum_add_word (len, out, 1);
}

bool
dsk_tls_bignum_modular_inverse (unsigned len,
                                const uint32_t *x,
                                const uint32_t *p,
                                uint32_t *x_inv_out)
{
  // Let's use the Extended Euclidean Algorithm
  // to compute the inverse of x mod p.
  uint32_t *r_old = alloca(4 * len), *t_old = alloca(4 * len);
  uint32_t *r_cur = alloca(4 * len), *t_cur = alloca(4 * len);
  uint32_t *quotient = alloca(4 * len), *remainder = alloca(4 * len);
  uint32_t *t_tmp = alloca(4 * len);
  bool t_old_is_signed = false, t_cur_is_signed = false, t_tmp_is_signed;
  
  memcpy (r_old, p, len * 4);
  unsigned r_old_len = dsk_tls_bignum_actual_len (len, r_old);
  memcpy (r_cur, x, len * 4);
  unsigned r_cur_len = dsk_tls_bignum_actual_len (len, r_cur);
  memset (t_old, 0, len * 4);
  memset (t_cur, 0, len * 4);
  t_cur[0] = 1;

  for (;;)
    {
      unsigned quotient_len = r_old_len - r_cur_len + 1;
      dsk_tls_bignum_divide (r_old_len, r_old, r_cur_len, r_cur, quotient, remainder);
      if (is_zero (r_cur_len, remainder))
        {
          if (r_cur_len != 1 || r_cur[0] != 1)
            return false;
          if (t_cur_is_signed)
            {
              // x_inv_out = p - t_cur
              dsk_tls_bignum_subtract_with_borrow (len, p, t_cur, 0, x_inv_out);
            }
          else
            memcpy (x_inv_out, t_cur, len * 4);

          return true;
        }

      //
      //       t_tmp := t_old - quotient * t_cur
      //
      // First set t_tmp to quotient * t_cur
      dsk_tls_bignum_multiply_truncated (len, t_cur, quotient_len, quotient, len, t_tmp);                       // t_tmp has same sign as t_cur
      // Second, perform sign-sensitive subtraction.
      if (t_old_is_signed == !t_cur_is_signed)
        {
          // opposite signs, so subtraction becomes addition
          dsk_tls_bignum_add_with_carry (len, t_tmp, t_old, 0, t_tmp);
          t_tmp_is_signed = t_old_is_signed;
        }
      else
        {
          // same sign, so it's subtraction.
          // compute t_old - t_tmp.
          t_tmp_is_signed = t_old_is_signed;
          if (dsk_tls_bignum_subtract_with_borrow (len, t_old, t_tmp, 0, t_tmp))
            {
              // result switched signs
              dsk_tls_bignum_negate (len, t_tmp, t_tmp);
              t_tmp_is_signed = !t_tmp_is_signed;
            }
        }

      // r_old := r_cur
      // r_cur := remainder
      uint32_t *tmp_ptr = r_old;
      r_old_len = r_cur_len;
      r_old = r_cur;
      r_cur_len = dsk_tls_bignum_actual_len (r_cur_len, remainder);
      assert (r_cur_len > 0);
      r_cur = remainder;
      remainder = tmp_ptr;

      // t_old := t_cur
      // t_cur := t_tmp
      t_old_is_signed = t_cur_is_signed;
      t_cur_is_signed = t_tmp_is_signed;
      tmp_ptr = t_old;
      t_old = t_cur;
      t_cur = t_tmp;
      t_tmp = tmp_ptr;
    }
}
void
dsk_tls_bignum_modular_inverse_pow2 (unsigned len,
                                     const uint32_t *x,
                                     unsigned log2_p,
                                     uint32_t *x_inv_out)
{
  uint32_t *xx = alloca ((len+1) * 4);
  uint32_t *pp = alloca ((len+1) * 4);
  uint32_t *tmp = alloca ((len+1) * 4);
  memcpy (xx, x, len * 4);
  xx[len] = 0;
  memset (pp, 0, (len+1) * 4);
  pp[log2_p/32] |= 1 << (log2_p % 32);
  dsk_tls_bignum_modular_inverse (len + 1, xx, pp, tmp);
  memcpy (x_inv_out, tmp, 4 * len);
}

// Compute 'tInv' such that v * vInv == 1 (mod (1<<32)).
uint32_t dsk_tls_bignum_invert_mod_wordsize32 (uint32_t v)
{
  uint64_t t_prev = 0;
  uint64_t r_prev = 1ULL << 32;

  int64_t t_cur = 1;
  uint64_t r_cur = v;

  for (;;) {
    uint64_t r_next = r_prev % r_cur;
    if (r_next == 0)
      break;
    uint64_t q_cur = r_prev / r_cur;
    int64_t t_next = t_prev - (int64_t)q_cur * t_cur;

    t_prev = t_cur;
    t_cur = t_next;
    r_prev = r_cur;
    r_cur = r_next;
  }
  return t_cur;
}

void dsk_tls_montgomery_info_init  (DskTlsMontgomeryInfo *info,
                                    unsigned len,
                                    const uint32_t *N)
{
  assert (N[len - 1] != 0);
  assert (N[0] % 2 == 1);

  // Rred < N and Rred == R (mod N).
  // Which is really Rred = R - n.
  //uint32_t *Rred = alloca (4 * len);
  //dsk_tls_bignum_negate (len, N, Rred);
  //uint32_t q;
  //dsk_tls_bignum_divide (len, Rred, len, N, &q, Rred);

  //info->Rinv = malloc (sizeof (uint32_t) * len);
  //dsk_tls_bignum_modular_inverse (len, Rred, N, (uint32_t *) info->Rinv);

  // Compute multiplicative inverse of -N (mod 2^32)  == N_prime
  uint32_t mNmodb = -N[0];
  info->Nprime = dsk_tls_bignum_invert_mod_wordsize32 (mNmodb);

#if 0
  uint32_t *Rbig = alloca (4 * (len + 1));
  uint32_t *minusNbig = alloca (4 * (len + 1));
  memset (Rbig, 0, 4 * (len + 1));
  Rbig[len] = 1;
  uint32_t *Nbig = alloca (4 * (2 * len + 1));
  memcpy (Nbig, N, 4*len);
  memset(Nbig+len, 0, 4 * (len + 1));
  dsk_tls_bignum_subtract_with_borrow (len + 1, Rbig, Nbig, 0, minusNbig);
  dsk_tls_bignum_modular_inverse (len + 1, minusNbig, Rbig, (uint32_t *) info->Nprime);
  assert(info->Nprime[len] == 0);
#endif

  info->len = len;
  info->N = malloc (len * 4);
  memcpy ((uint32_t *) info->N, N, len * 4);

  // compute barrett's mu
  info->barrett_mu = malloc ((len + 2) * 4);
  dsk_tls_bignum_compute_barrett_mu (len, N, (uint32_t *) info->barrett_mu);
}

void
dsk_tls_bignum_compute_barrett_mu (unsigned              len,
                                   const uint32_t       *modulus,
                                   uint32_t             *out            // of length len+2, but out[len+1]==0
                                  )
{
  // compute Barrett's mu
  assert(modulus[len - 1] != 0);
  uint32_t *numer = alloca (4 * (len * 2 + 1));
  memset (numer, 0, 4 * (len * 2 + 1));
  numer[len*2] = 1;
  uint32_t *modout = alloca (4 * (len * 2 + 1));
  dsk_tls_bignum_divide (len * 2 + 1, numer, len, modulus, out, modout);
  assert(out[len + 1] == 0);
}

// ASSERT: for now, len == 2 * modulus_len
void
dsk_tls_bignum_modulus_with_barrett_mu (unsigned        len,
                                        const uint32_t *value,
                                        unsigned        modulus_len,
                                        const uint32_t *modulus,
                                        const uint32_t *barrett_mu,
                                        uint32_t       *mod_value_out)
{
  const uint32_t *q1 = value + (modulus_len - 1);
  unsigned q1len = len - (modulus_len - 1);
  unsigned q2len = q1len + modulus_len + 1;
  uint32_t *q2 = alloca (q2len * 4);
  dsk_tls_bignum_multiply (q1len, q1, modulus_len+1, barrett_mu, q2);
  uint32_t *q3 = q2 + (modulus_len+1);
  unsigned q3len = q2len - (modulus_len+1);
  //unsigned r1len = modulus_len + 1;
  const uint32_t *r1 = value;
  unsigned r2len = modulus_len + 1;
  uint32_t *r = alloca (r2len * 4);
  dsk_tls_bignum_multiply_truncated (q3len, q3, modulus_len, modulus, r2len, r); //r ==r_2
  dsk_tls_bignum_subtract_with_borrow (r2len, r1, r, 0, r);  // now r is the algorithm's r
  while (r[modulus_len] != 0)
    {
      if (dsk_tls_bignum_subtract_with_borrow (modulus_len, r, modulus, 0, r))
        r[modulus_len] -= 1;
    }
  while (dsk_tls_bignum_compare (modulus_len, r, modulus) >= 0)
    dsk_tls_bignum_subtract_with_borrow (modulus_len, r, modulus, 0, r);
  memcpy (mod_value_out, r, modulus_len * 4);
}

void dsk_tls_bignum_to_montgomery (DskTlsMontgomeryInfo *info,
                                   const uint32_t       *in,
                                   uint32_t             *out)
{
  // compute q = in * R (which is just shifting it left by len words)
  uint32_t *q = alloca(info->len * 2 * 4);
  memset(q, 0, 4 * info->len);
  memcpy(q + info->len, in, info->len * 4);

  // mod(q) using Barrett's method
  dsk_tls_bignum_modulus_with_barrett_mu (info->len * 2, q,
                                          info->len, info->N,
                                          info->barrett_mu, out);
}

// Compute
//      addto += scale * in
// returning carry word.
uint32_t
dsk_tls_bignum_multiply_word_add (unsigned len,
                                  const uint32_t *in,
                                  uint32_t scale,
                                  uint32_t *addto)
{
  uint32_t carry = 0;
  for (unsigned i = 0; i < len; i++)
    {
      uint64_t prod = (uint64_t) scale * *in++ + carry + *addto;
      carry = prod >> 32;
      *addto++ = prod;
    }
  return carry;
}


// big <= info->N * R = info->N << info->R_log2.
// out == big * R^{-1} (mod N).
void
dsk_tls_bignum_montgomery_reduce (DskTlsMontgomeryInfo *info,
                                  uint32_t             *T,
                                  uint32_t             *out)
{
  unsigned A_len = 2 * info->len + 1;
  uint32_t *A = alloca (4 * A_len);
  memcpy (A, T, 4 * 2 * info->len);
  A[2 * info->len] = 0;
  for (unsigned i = 0; i < info->len; i++)
    {
      uint32_t u = A[i] * info->Nprime;
      uint32_t carry = dsk_tls_bignum_multiply_word_add (info->len, info->N, u, A + i);
      assert(A[i] == 0);
      dsk_tls_bignum_add_word (A_len - i - info->len, A + i + info->len, carry);
    }

  A += info->len;
  A_len -= info->len;
  assert (A[info->len] <= 1);
  if (A[info->len] == 1
   || dsk_tls_bignum_compare (info->len, A, info->N) >= 0)
    {
      dsk_tls_bignum_subtract_with_borrow (info->len, A, info->N, 0, out);
    }
  else
    memcpy (out, A, info->len * 4);
}

void
dsk_tls_bignum_multiply_montgomery(DskTlsMontgomeryInfo *info,
                                   const uint32_t       *a,
                                   const uint32_t       *b,
                                   uint32_t             *out)
{
  uint32_t *product = alloca (4 * info->len*2);
  dsk_tls_bignum_multiply (info->len, a, info->len, b, product);
  dsk_tls_bignum_montgomery_reduce (info, product, out);
}

void dsk_tls_bignum_from_montgomery(DskTlsMontgomeryInfo *info,
                                    const uint32_t       *in,
                                    uint32_t             *out)
{
  uint32_t *padded = alloca (4 * info->len*2);
  memcpy (padded, in, 4 * info->len);
  memset (padded + info->len, 0, 4 * info->len);
  dsk_tls_bignum_montgomery_reduce (info, padded, out);
}
