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

static uint32_t
addto (unsigned     len,
       unsigned     inout_extra,
       uint32_t *inout,
       const uint32_t *term)
{
  uint32_t carry = dsk_tls_bignum_add_with_carry (len, inout, term, 0, inout);
  if (carry)
    return dsk_tls_bignum_add_word (inout_extra, inout + len, carry);
  return 0;
}

static uint64_t
scaled_subtract_from (unsigned len,
                      unsigned inout_extra,
                      uint32_t *inout,
                      uint32_t  scale,
                      const uint32_t *sub)
{
  uint64_t borrow = 0;
  for (unsigned i = 0; i < len; i++)
    {
      uint64_t p = (uint64_t)scale * *sub++ + borrow;
      uint32_t old = *inout;
      *inout -= p;
      borrow = p >> 32;
      if (old < *inout)
        borrow++;
      inout++;
    }
  for (unsigned i = 0; i < inout_extra && borrow != 0; i++)
    {
      uint32_t old = *inout;
      *inout -= borrow;
      borrow >>= 32;
      if (old < *inout)
        borrow++;
      inout++;
    }
  return borrow;
}

// on output, 'x' is the remainder.
static void
divide_normalized (unsigned x_len,
                   uint32_t *x,
                   unsigned y_len,
                   const uint32_t *y,
                   uint32_t *q)
{
  uint32_t y_hi = y[y_len];
  assert (y_hi >= (1U << 31));

  // Step 1:  q_j <- 0
  memset (q, 0, 4 * (x_len - y_len + 1));

  // Step 2: While (x ≥ y*b^(n−t)) do:
  //           q_{n−t} := q_{n−t} + 1
  //           x := x − yb^{n−t}
  // Because y[y_len-1] >= (1 << 31),
  // this while loop can execute at most once,
  // and is hence replaced by an 'if'.
  {
    int cmp = dsk_tls_bignum_compare (y_len, x + (x_len - y_len), y);
    if (cmp >= 0)
      {
        dsk_tls_bignum_subtract_with_borrow (y_len,
                                             x + (x_len - y_len),
                                             y,
                                             0,     // no borrow
                                             x + (x_len - y_len));
        q[x_len - y_len] += 1;
      }
  }

  //
  // Step 3: For i from x_len-1 down to (x_len-y_len):
  //
  for (int i = x_len - 1; i >= (int)(x_len - y_len); i--)
    {
      // Step 3.1: If x[i] = y[y_len - 1] then:
      //               q[i−t−1] := b − 1
      //           Otherwise:
      //               q[i−t−1] := floor((x[i] * b + x[i-1]) / y[y_len - 1])
      if (x[i] == y_hi)
        q[i - y_len] = 0xffffffff;
      else
        q[i - y_len] = (((uint64_t)x[i] << 32) + x[i-1]) / y_hi;

      // Step 3.2: While (q[i-y_len] * (y_hi * b + y[y_len-2]))
      //                                >
      //                 x[i]*b*b + x[i-1]*b + x[i-2]) do:
      //             q[i - y_len] -= 1;
      {
        uint64_t lhs_hi = (uint64_t) q[i - y_len] * y_hi;
        uint64_t lhs_lo_tmp = (uint64_t) q[i - y_len] * y[y_len - 2];
        lhs_hi += lhs_lo_tmp >> 32;
        uint32_t lhs_lo = lhs_lo_tmp;

        uint64_t rhs_hi = ((uint64_t) x[i] << 32) + x[i-1];
        uint64_t rhs_lo = x[i-2];

        while (lhs_hi > rhs_hi || (lhs_hi == rhs_hi && lhs_lo > rhs_lo))
          {
            q[i - y_len] -= 1;

            // update lhs for the lower q[i-y_len]
            // lhs -= y_hi * b + y_{y_len-2}
            int borrow = (lhs_lo < y[y_len - 2]);
            lhs_lo -= y[y_len - 2];
            lhs_hi -= y_hi - borrow;
          }
      }

      // Step 3.3.
      // x -= q[i-y_len] * y * b**{i-y_len}
      uint64_t borrow = scaled_subtract_from (y_len, x_len - i, x + (i - y_len), q[i-y_len], y);
      assert (borrow <= 1);
      if (borrow)
        {
          // Step 3.4. If x < 0 then set x := x + y*b^{i−y_len} and q[i−y_len] -= 1.
          // x is negative (2s-complement)
          addto (y_len, x_len - i, x + (i - y_len), y);

          assert(q[i - y_len] != 0);
          q[i - y_len] -= 1;
        }
    }
}


void
dsk_tls_bignum_shiftleft_small (unsigned len,
                                const uint32_t *in,
                                uint32_t *out,
                                unsigned amount)
{
  uint32_t carry = 0;
  for (unsigned i = 0; i < len; i++)
    {
      uint32_t next_carry = in[i] >> (32 - amount);
      out[i] = (in[i] << amount) | carry;
      carry = next_carry;
    }
}
void
dsk_tls_bignum_shiftright_small (unsigned len,
                                 const uint32_t *in,
                                 uint32_t *out,
                                 unsigned amount)
{
  uint32_t carry = 0;
  for (unsigned i = 0; i < len; i++)
    {
      unsigned ii = len - 1 - i;
      uint32_t next_carry = in[ii] << (32 - amount);
      out[ii] = (in[ii] >> amount) | carry;
      carry = next_carry;
    }
}

void
dsk_tls_bignum_divide  (unsigned x_len,
                        const uint32_t *x_words,
                        unsigned y_len,
                        const uint32_t *y_words,
                        uint32_t *quotient_out,
                        uint32_t *remainder_out)
{
  assert(x_len > 0 && y_len > 0);
  assert(x_len >= y_len);
  assert(y_words[y_len - 1] != 0);

  if (y_words[y_len - 1] & 0x80000000)
    {
      // already normalized
      uint32_t *x_copy = alloca (4 * x_len);
      memcpy (x_copy, x_words, x_len * 4);
      divide_normalized (x_len, x_copy, y_len, y_words, remainder_out);
    }
  else
    {
      unsigned shift = 1;
      while (((y_words[y_len] << shift) & 0x80000000) == 0)
        shift++;

      uint32_t *y_copy = alloca (4 * y_len);
      dsk_tls_bignum_shiftleft_small (y_len, y_words, y_copy, shift);
      if (x_words[x_len] >> (32 - shift))
        {
          uint32_t *x_copy = alloca (4 * (x_len + 1));
          x_copy[x_len] = x_words[x_len - 1] >> (32 - shift);
          dsk_tls_bignum_shiftleft_small (x_len, x_words, x_copy, shift);
          divide_normalized (x_len, x_copy, y_len, y_copy, quotient_out);
          dsk_tls_bignum_shiftright_small (y_len, x_copy, remainder_out, shift);
        }
      else
        {
          uint32_t *x_copy = alloca (4 * x_len);
          dsk_tls_bignum_shiftleft_small (x_len, x_words, x_copy, shift);
          divide_normalized (x_len, x_copy, y_len, y_copy, quotient_out);
          dsk_tls_bignum_shiftright_small (y_len, x_copy, remainder_out, shift);
        }
    }
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

void
dsk_tls_bignum_modular_inverse (unsigned len,
                                const uint32_t *x,
                                const uint32_t *p,
                                uint32_t *x_inv_out)
{
  // Let's use the Extended Euclidean Algorithm
  // to compute the inverse of x mod p.
  uint32_t *r_old = alloca(4 * len), *s_old = alloca(4 * len);
  uint32_t *r_cur = alloca(4 * len), *s_cur = alloca(4 * len);
  uint32_t *quotient = alloca(4 * len), *remainder = alloca(4 * len);
  uint32_t *s_tmp = alloca(4 * len);
  bool s_old_is_signed = false, s_cur_is_signed = false, s_tmp_is_signed;
  
  memcpy (r_old, x, len * 4);
  unsigned r_old_len = dsk_tls_bignum_actual_len (len, r_old);
  memcpy (r_cur, p, len * 4);
  unsigned r_cur_len = dsk_tls_bignum_actual_len (len, r_cur);
  memset (s_old, 0, len * 4);
  memset (s_cur, 0, len * 4);
  s_old[0] = 1;

  for (;;)
    {
      unsigned quotient_len = r_old_len - r_cur_len + 1;
      dsk_tls_bignum_divide (r_old_len, r_old, r_cur_len, r_cur, quotient, remainder);
      if (is_zero (r_cur_len, remainder))
        {
          if (s_cur_is_signed)
            {
              // x_inv_out = p - s_cur
              dsk_tls_bignum_subtract_with_borrow (len, p, s_cur, 0, x_inv_out);
            }
          else
            memcpy (x_inv_out, s_cur, len * 4);
          return;
        }

      //
      //       s_tmp := s_old - quotient * s_cur
      //
      // First set s_tmp to quotient * s_cur
      dsk_tls_bignum_multiply_truncated (len, s_cur, quotient_len, quotient, len, s_tmp);
      // Second, perform sign-sensitive subtraction.
      if (s_old_is_signed == !s_cur_is_signed)
        {
          // opposite signs, so subtraction becomes addition
          dsk_tls_bignum_add_with_carry (len, s_tmp, s_old, 0, s_tmp);
          s_tmp_is_signed = s_old_is_signed;
        }
      else
        {
          // same sign, so it's subtraction.
          // compute s_old - s_tmp; result has opposite sign as s_old
          s_tmp_is_signed = s_old_is_signed;
          if (dsk_tls_bignum_subtract_with_borrow (len, s_tmp, s_old, 0, s_tmp))
            {
              // result switched signs
              dsk_tls_bignum_negate (len, s_tmp, s_tmp);
              s_tmp_is_signed = !s_tmp_is_signed;
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

      // s_old := s_cur
      // s_cur := s_tmp
      s_old_is_signed = s_cur_is_signed;
      s_cur_is_signed = s_tmp_is_signed;
      tmp_ptr = s_old;
      s_old = s_cur;
      s_cur = s_tmp;
      s_tmp = tmp_ptr;
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
  uint32_t *Rred = alloca (4 * len);
  memset (Rred, 0, 4 * len);
  dsk_tls_bignum_subtract_with_borrow (len, Rred, N, 0, Rred);
  uint32_t q;
  dsk_tls_bignum_divide (len, Rred, len, N, &q, Rred);

#if 0
  info->Rinv = malloc (sizeof (uint32_t) * len);
  dsk_tls_bignum_modular_inverse (len, Rred, N, (uint32_t *) info->Rinv);
#endif

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
  dsk_tls_bignum_modulus_with_barrett_mu (info->len * 2, q, info->len, info->N, info->barrett_mu, out);
}

// big <= info->N * R = info->N << info->R_log2.
// out == big * R^{-1} (mod N).
static void
montgomery_reduce (DskTlsMontgomeryInfo *info,
                   uint32_t             *big,
                   uint32_t             *out)
{
  unsigned A_len = 2 * info->len + 1;
  uint32_t *A = alloca (4 * A_len);
  memcpy (A, big, 4 * 2 * info->len);
  A[2 * info->len] = 0;
  uint32_t *um = alloca (4 * (info->len + 1));
  for (unsigned i = 0; i < info->len; i++)
    {
      uint32_t u = A[i] * info->Nprime;
      dsk_tls_bignum_multiply (1, &u, info->len, info->N, um);
      if (dsk_tls_bignum_add_with_carry (info->len + 1, A + i, um, 0, A + i))
        {
          unsigned carry_start = i + info->len + 1;
          dsk_tls_bignum_add_word (A_len - carry_start, A + carry_start, 1);
        }
    }

  A += info->len;
  A_len -= info->len;
  assert (A[A_len - 1] <= 1);
  if (A[A_len - 1] == 1 || dsk_tls_bignum_compare (info->len, A, info->N) >= 0)
    dsk_tls_bignum_subtract_with_borrow (info->len, A, info->N, 0, out);
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
  montgomery_reduce (info, product, out);
}

void dsk_tls_bignum_from_montgomery(DskTlsMontgomeryInfo *info,
                                    const uint32_t       *in,
                                    uint32_t             *out)
{
  uint32_t *padded = alloca (4 * info->len*2);
  memcpy (padded, in, 4 * info->len);
  memset (padded + 4 * info->len, 0, 4 * info->len);
}
