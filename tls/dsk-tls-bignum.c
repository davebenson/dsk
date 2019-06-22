//
// See http://cacr.uwaterloo.ca/hac/about/chap14.pdf
//
#include "../dsk.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <alloca.h>

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
void dsk_tls_bignum_square (unsigned len,
                              const uint32_t *words,
                              uint32_t *out)
{
  uint64_t *scratch = alloca (8 * len * 2);
  memset (scratch, 0, 8 * len * 2);
  for (unsigned i = 0; i < len; i++)
    {
      for (unsigned j = 0; j < i; j++)
        {
          uint64_t prod = (uint64_t) words[i] * words[j];
          scratch[i+j] += (prod & 0x7fffffff) << 1;
          scratch[i+j+1] += prod >> 31;
        }
      {
        uint64_t prod = (uint64_t) words[i] * words[i];
        scratch[i*2] += (prod & 0xffffffff);
        scratch[i*2+1] += prod >> 32;
      }
    }

  uint64_t carry = scratch[0] >> 32;
  out[0] = scratch[0];
  for (unsigned i = 1; i < len * 2; i++)
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

uint32_t dsk_tls_bignum_multiply_word        (unsigned len,
                                              const uint32_t *in,
                                              uint32_t word,
                                              uint32_t *out)
{
  uint32_t carry = 0;
  for (unsigned i = 0; i < len; i++)
    {
      uint64_t prod = (uint64_t) in[i] * word + carry;
      out[i] = (uint32_t) prod;
      carry = prod >> 32;
    }
  return carry;
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
void     dsk_tls_bignum_modular_add          (unsigned        len,
                                              const uint32_t *a_words,
                                              const uint32_t *b_words,
                                              const uint32_t *modulus_words,
                                              uint32_t       *out)
{
  if (dsk_tls_bignum_add_with_carry (len, a_words, b_words, 0, out))
    dsk_tls_bignum_subtract (len, out, modulus_words, out);
  else if (dsk_tls_bignum_compare (len, out, modulus_words) >= 0)
    dsk_tls_bignum_subtract (len, out, modulus_words, out);
}

void     dsk_tls_bignum_modular_subtract     (unsigned        len,
                                              const uint32_t *a_words,
                                              const uint32_t *b_words,
                                              const uint32_t *modulus_words,
                                              uint32_t       *out)
{
  if (dsk_tls_bignum_subtract_with_borrow (len, a_words, b_words, 0, out))
    dsk_tls_bignum_add (len, out, modulus_words, out);
}


uint32_t dsk_tls_bignum_add_word_inplace (unsigned len, uint32_t *v, uint32_t carry)
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
uint32_t dsk_tls_bignum_add_word             (unsigned len,
                                              const uint32_t *in,
                                              uint32_t carry,
                                              uint32_t *out)
{
  for (unsigned i = 0; i < len; i++)
    {
      *out = *in + carry;
      if (*out >= carry)
        return 0;
      out++;
      in++;
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

          //
          // Do trial subtraction, use result if borrow not necessary.
          // Usage of the top 32-bits of 'y' but addition 1 to force a lower-bound,
          // should bound the error from 'y' at 1 part in (1<<31).
          //
          // Additional error comes from rounding down,
          // but I haven't found values that cause the following
          // loop to run more than twice.
          // 

          //
          // XXX: We might be able to tighten up the math
          //      to get so that at most one subtraction is necessary.
          //
          // XXX: I believe at most 2 subtractions are necessary, but a
          //      proof would be good.
          //
          for (;;)
            {
              borrow = dsk_tls_bignum_subtract_with_borrow (y_len, remaining + q_digit, y_words, 0, tmp);
              if (borrow <= remaining[q_digit + y_len])
                {
                  memcpy (remaining + q_digit, tmp, y_len * 4);
                  assert(remaining[q_digit + y_len] == borrow);
                  remaining[q_digit + y_len] = 0;           // Not necessary
                  q++;
                }
              else
                break;
            }
          quotient_out[q_digit] = q;
        }
    }
  memcpy (remainder_out, remaining, 4 * y_len);
}

void
dsk_tls_bignum_shiftright_truncated (unsigned in_len,
                                     const uint32_t *in,
                                     unsigned shift,
                                     unsigned out_len,
                                     uint32_t *out)
{
  // Treat shifts >= 32 by changing in/in_len.
  if (shift / 32 >= in_len)
    {
      memset(out, 0, 4*out_len);
      return;
    }
  in_len -= shift / 32;
  in += shift / 32;
  shift %= 32;

  if (shift == 0)
    {
      if (in_len >= out_len)
        {
          memmove (out, in, out_len * 4);
        }
      else
        {
          memmove (out, in, in_len * 4);
          memset (out + in_len, 0, 4 * (out_len - in_len));
        }
    }
  else
    {
      unsigned i;
      for (i = 0; i + 1 < in_len && i < out_len; i++)
        {
          out[i] = (in[i] >> shift) | (in[i+1] << (32-shift));
        }
      if (i < out_len)
        {
          out[i] = in[i] >> shift;
          i++;
        }
      while (i < out_len)
        out[i++] = 0;
    }
}
void
dsk_tls_bignum_shiftleft_truncated (unsigned in_len,
                                    const uint32_t *in,
                                    unsigned shift,
                                    unsigned out_len,
                                    uint32_t *out)
{
  // Treat shifts >= 32 by changing in/in_len.
  if (shift / 32 >= out_len)
    {
      memset(out, 0, 4*out_len);
      return;
    }
  memset(out, 0, (shift/32)*4);
  out_len -= shift / 32;
  out += shift / 32;
  shift %= 32;
  if (shift == 0)
    {
      if (in_len >= out_len)
        {
          memmove (out, in, out_len * 4);
        }
      else
        {
          memmove (out, in, in_len * 4);
          memset (out + in_len, 0, 4 * (out_len - in_len));
        }
    }
  else
    {
      int o = out_len - 1;
      while (o > (int) in_len + 1)
        {
          out[o] = 0;
          o--;
        }

      //o == in_len+1
      if (o == (int) in_len)
        {
          out[o] = in[o-1] >> (32 - shift);
          o--;
        }

      while (o >= 1)
        {
          out[o] = (in[o] << shift) | (in[o-1] >> (32 - shift));
          o--;
        }
      if (o == 0)
        {
          out[o] = (in[o] << shift);
        }
    }
}
static inline int
highest_bit_in_uint32 (uint32_t v)
{
  for (int b = 31; b >= 0; b--)
    if (v >> b)
      return b;
  assert(false);
  return -1;
}

static unsigned find_highest_set_bit (unsigned len, const uint32_t *v)
{
  for (int i = len - 1; i >= 0; i--)
    {
      if (v[i])
        return highest_bit_in_uint32 (v[i]) + i * 32;
    }
  assert(0);
  return -1;
}

bool
dsk_tls_bignum_is_zero (unsigned len, const uint32_t *v)
{
  for (unsigned i = 0; i < len; i++)
    if (v[i])
      return false;
  return true;
}

unsigned
dsk_tls_bignum_actual_len (unsigned len, const uint32_t *v)
{
  while (len > 0 && v[len-1] == 0)
    len--;
  return len;
}

// Precondition: v != 0
static unsigned
highbit32 (uint32_t v)
{
  static unsigned highbits_per_nibble[16] = {
    0, 0, 1, 1, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3
  };
  if (v >= 0x10000)
    {
      if (v >= 0x1000000)
       {
         if (v >= 0x10000000)
           return highbits_per_nibble[v >> 28] + 28;
         else
           return highbits_per_nibble[v >> 24] + 24;
       }
     else
       {
         if (v >= 0x100000)
           return highbits_per_nibble[v >> 20] + 20;
         else
           return highbits_per_nibble[v >> 16] + 16;
       }
    }
  else
    {
      if (v >= 0x100)
       {
         if (v >= 0x1000)
           return highbits_per_nibble[v >> 12] + 12;
         else
           return highbits_per_nibble[v >> 8] + 8;
       }
     else
       {
         if (v >= 0x10)
           return highbits_per_nibble[v >> 4] + 4;
         else
           return highbits_per_nibble[v];
       }
    }
} 

// highest bit that's one, or -1 if v is zero.
int      dsk_tls_bignum_max_bit              (unsigned len,
                                              const uint32_t *v)
{
  for (unsigned k = len; k--; )
    if (v[k])
      return k * 32 + highbit32 (v[k]);
  return -1;
}

void
dsk_tls_bignum_negate (unsigned len, const uint32_t *in, uint32_t *out)
{
  for (unsigned i = 0; i < len; i++)
    out[i] = ~in[i];
  dsk_tls_bignum_add_word_inplace (len, out, 1);
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
      if (dsk_tls_bignum_is_zero (r_cur_len, remainder))
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

//
// Return whether 'x' is a quadratic residue (ie whether it has a square-root).
// Uses Euler's criterion, which says that:
//
//   x^((p-1)/2) == 1 (mod p)    <===>     x is a quadratic residue.
//
// This only works for prime p > 2, and x != 0.
//
static bool
is_quadratic_residue (DskTlsMontgomeryInfo *mont,
                      uint32_t              x,
                      uint32_t             *mont_1)
{
  uint32_t *x_mont_pow_pow2 = alloca(4 * mont->len);
  uint32_t *x_mont_pow = alloca(4 * mont->len);
  dsk_tls_bignum_word_to_montgomery (mont, x, x_mont_pow_pow2);
  bool x_mont_pow_inited = false;
  assert(mont->N[mont->len - 1] != 0);
  unsigned end_bit = 32 * (mont->len-1) + highest_bit_in_uint32(mont->N[mont->len - 1]);
  for (unsigned i = 1; i < end_bit; i++)
    {
      if ((mont->N[i/32] & (1<<(i%32))) != 0)
        {
          if (x_mont_pow_inited)
            dsk_tls_bignum_multiply_montgomery (mont, x_mont_pow, x_mont_pow_pow2, x_mont_pow);
          else
            {
              memcpy (x_mont_pow, x_mont_pow_pow2, 4 * mont->len);
              x_mont_pow_inited = true;
            }
        }
      dsk_tls_bignum_square_montgomery (mont, x_mont_pow_pow2, x_mont_pow_pow2);
    }

  // highest bit is always 1.
  if (x_mont_pow_inited)
    dsk_tls_bignum_multiply_montgomery (mont, x_mont_pow, x_mont_pow_pow2, x_mont_pow);
  else
    {
      memcpy (x_mont_pow, x_mont_pow_pow2, 4 * mont->len);
      x_mont_pow_inited = true;
    }

  return memcmp (x_mont_pow, mont_1, mont->len * 4) == 0;
}

static uint32_t
find_quadratic_nonresidue (DskTlsMontgomeryInfo *mont,
                      uint32_t             *mont_1)
 
{
  uint32_t rv = 2;
  while (is_quadratic_residue (mont, rv, mont_1))
    {
      rv += 1;
      assert(rv != 0);
    }
  return rv;
}

#if 0
static void
pr_mont(DskTlsMontgomeryInfo *mont, const char *label, const uint32_t *m)
{
  uint32_t *mm = alloca(mont->len * 4);
  dsk_tls_bignum_from_montgomery (mont, m, mm);
  printf("%s:", label);
  for (unsigned i =0 ; i < mont->len; i++) printf(" %08x", mm[i]);
  printf("\n");
}
#endif
//
// https://en.wikipedia.org/wiki/Tonelli-Shanks_algorithm
//
// TODO: wiki page suggests the algo can be generalized to any n-th root.
//
// NOTE: modulus_words must be prime.
//
static bool
dsk_tls_bignum_modular_sqrt_1mod4   (DskTlsMontgomeryInfo *mont,
                                     const uint32_t *X_words,
                                     uint32_t *X_sqrt_out)
{
  unsigned len = mont->len;

  //
  // The lowest nonzero bit is bit 0, because any large prime is odd.
  //
  // The second_lowest_nonzero_bit is the next non-zero bit.
  //
  unsigned second_lowest_nonzero_bit = 1;
  while (second_lowest_nonzero_bit < len*32
      && ((mont->N[second_lowest_nonzero_bit/32] >> (second_lowest_nonzero_bit%32)) & 1) == 0)
    second_lowest_nonzero_bit += 1;
  if (second_lowest_nonzero_bit == len*32)
    {
      // sqrt(0) == 0.
      memset (X_sqrt_out, 0, 4*len);
      return true;
    }

  //
  // Compute Q, S as in the wikipedia algorithm description.
  //
  unsigned Q_len = len - second_lowest_nonzero_bit/32;
  uint32_t *Q = alloca(Q_len * 4);
  dsk_tls_bignum_shiftright_truncated (len, mont->N, second_lowest_nonzero_bit, Q_len, Q);
  assert((Q[0] & 1) == 1);
  Q_len = dsk_tls_bignum_actual_len (Q_len, Q);
  unsigned S = second_lowest_nonzero_bit;     // as in Wikipedia's Tonelli-Shank's algo

  // Find the montgomery representation of 1, since we commonly need to check if values are 1.
  uint32_t *mont1 = alloca (4 * mont->len);
  dsk_tls_bignum_word_to_montgomery (mont, 1, mont1);

  //
  // Compute Q1_half == (Q+1) / 2.
  //
  // Note that Q is odd, so this is always an int.
  //
  uint32_t *Q1_half = alloca(Q_len * 4);
  memcpy (Q1_half, Q, Q_len * 4);
  dsk_tls_bignum_add_word_inplace (Q_len, Q1_half, 1);
  dsk_tls_bignum_shiftright_truncated (Q_len, Q1_half, 1, Q_len, Q1_half);
  unsigned Q1_half_len = dsk_tls_bignum_actual_len (Q_len, Q1_half);

  //
  // Find a quadratic non-residue z (a number which does NOT have sqrt mod p).
  //
  uint32_t z = find_quadratic_nonresidue (mont, mont1);
  uint32_t *z_mont = alloca(4 * mont->len);
  dsk_tls_bignum_word_to_montgomery (mont, z, z_mont);

  //
  // Step 3.  Variable names are again taken from the wiki.
  //
  unsigned M = S;
  uint32_t *c_mont = alloca (mont->len * 4);
  dsk_tls_bignum_exponent_montgomery (mont, z_mont, Q_len, Q, c_mont);
  uint32_t *X_mont = alloca (mont->len * 4);
  dsk_tls_bignum_to_montgomery (mont, X_words, X_mont);
  uint32_t *t_mont = alloca(mont->len * 4);
  dsk_tls_bignum_exponent_montgomery (mont, X_mont, Q_len, Q, t_mont);
  uint32_t *R_mont = alloca(mont->len * 4);
  dsk_tls_bignum_exponent_montgomery (mont, X_mont, Q1_half_len, Q1_half, R_mont);

  //
  // Step 4.  Loop:
  //
  uint32_t *t_2_i_mont = alloca (len * 4);
  uint32_t *b_mont = alloca (len * 4);
  for (;;)
    {
      // if t=0, return 0.
      if (dsk_tls_bignum_is_zero (len, t_mont))
        {
          memset (X_sqrt_out, 0, 4 * len);
          return dsk_tls_bignum_is_zero (mont->len, X_words);   /// ???
        }

      // if t=1, return R.
      if (memcmp (t_mont, mont1, len * 4) == 0)
        {
          dsk_tls_bignum_from_montgomery (mont, R_mont, X_sqrt_out);
          return true;
        }

      // Otherwise, use repeated squaring to find the least i (mod p),
      // 0 < i < M, such that t^{2^{i}-1} = 1.
      unsigned i = 0;
      memcpy (t_2_i_mont, t_mont, 4 * len);
      while (i < M)
        {
          if (memcmp (t_2_i_mont, mont1, 4 * len) == 0)
            break;
          dsk_tls_bignum_square_montgomery (mont, t_2_i_mont, t_2_i_mont);
          i++;
        }
      if (i == M)
        return false;

      //
      // Update loop variables (mod p).
      //
      // Let b := c^{2^{M-i-1}}.
      memcpy (b_mont, c_mont, 4 * len);
      for (unsigned j = 0; j < M - i - 1; j++)
        dsk_tls_bignum_square_montgomery (mont, b_mont, b_mont);
      M = i;
      // c := b^2
      dsk_tls_bignum_square_montgomery (mont, b_mont, c_mont);

      // t := t*b^2
      dsk_tls_bignum_multiply_montgomery (mont, t_mont, c_mont, t_mont);

      // R := R * b
      dsk_tls_bignum_multiply_montgomery (mont, R_mont, b_mont, R_mont);
    }
}

static bool
dsk_tls_bignum_modular_sqrt_3mod4   (DskTlsMontgomeryInfo *mont,
                                     const uint32_t *X_words,
                                     uint32_t *X_sqrt_out)
{
  // Compute r = X^((p+1)/4).

  // First, compute (p+1)/4 == floor(p/4)+1, since p%4==3.
  // The latter expression definitely won't overflow, so use it.
  uint32_t *p1_over_4 = alloca(mont->len * 4);
  dsk_tls_bignum_shiftright_truncated (mont->len, mont->N, 2, mont->len, p1_over_4);
  dsk_tls_bignum_add_word_inplace (mont->len, p1_over_4, 1);
  unsigned p1_over_4_len = dsk_tls_bignum_actual_len (mont->len, p1_over_4);

  uint32_t *X_mont = alloca(mont->len * 4);
  dsk_tls_bignum_to_montgomery (mont, X_words, X_mont);

  uint32_t *candidate = alloca (4 * mont->len);
  dsk_tls_bignum_exponent_montgomery (mont, X_mont, p1_over_4_len, p1_over_4, candidate);

  // If r^2 == 2, then r is the sqrt; otherwise, X is not a quadratic residue.
  dsk_tls_bignum_square_montgomery (mont, candidate, X_sqrt_out);
  if (memcmp (X_sqrt_out, X_mont, mont->len * 4) == 0)
    {
      dsk_tls_bignum_from_montgomery (mont, candidate, X_sqrt_out);
      return true;
    }
  return false;
}


bool
dsk_tls_bignum_modular_sqrt         (unsigned len,
                                     const uint32_t *X_words,
                                     const uint32_t *modulus_words,
                                     uint32_t *X_sqrt_out)
{
  DskTlsMontgomeryInfo mont;
  dsk_tls_montgomery_info_init (&mont, len, modulus_words);
  assert (len > 0);
  assert ((modulus_words[0] & 1) == 1);
  bool rv;
  if (modulus_words[0] & 2)
    rv = dsk_tls_bignum_modular_sqrt_3mod4 (&mont, X_words, X_sqrt_out);
  else
    rv = dsk_tls_bignum_modular_sqrt_1mod4 (&mont, X_words, X_sqrt_out);
  dsk_tls_montgomery_info_clear (&mont);
  return rv;
}

void dsk_tls_montgomery_info_init  (DskTlsMontgomeryInfo *info,
                                    unsigned len,
                                    const uint32_t *N)
{
  assert (N[len - 1] != 0);
  assert (N[0] % 2 == 1);

  // Compute multiplicative inverse of -N (mod 2^32)  == N_prime
  uint32_t mNmodb = -N[0];
  info->Nprime = dsk_tls_bignum_invert_mod_wordsize32 (mNmodb);

  info->len = len;
  info->N = malloc (len * 4);
  memcpy ((uint32_t *) info->N, N, len * 4);

  // compute barrett's mu
  info->barrett_mu = malloc ((len + 2) * 4);
  dsk_tls_bignum_compute_barrett_mu (len, N, (uint32_t *) info->barrett_mu);
}

void dsk_tls_montgomery_info_clear  (DskTlsMontgomeryInfo *info)
{
  free ((void *) info->N);
  free ((void *) info->barrett_mu);
  info->N = NULL;
  info->barrett_mu = NULL;
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
  assert(len == 2 * modulus_len);

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
  //printf("r:");for(unsigned i = 0; i < modulus_len; i++)printf(" %08x", r[i]);printf("\n");
  //printf("mod:");for(unsigned i = 0; i < modulus_len; i++)printf(" %08x", modulus[i]);printf("\n");
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
void dsk_tls_bignum_word_to_montgomery (DskTlsMontgomeryInfo *info,
                                        uint32_t              word,
                                        uint32_t             *out)
{
  // compute q = in * R (which is just shifting it left by len words)
  uint32_t *q = alloca(info->len * 2 * 4);
  memset(q, 0, 4 * info->len);
  q[info->len] = word;
  memset(q + info->len + 1, 0, 4 * (info->len - 1));

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
      dsk_tls_bignum_add_word_inplace (A_len - i - info->len, A + i + info->len, carry);
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

void dsk_tls_bignum_square_montgomery
                                  (DskTlsMontgomeryInfo *info,
                                   const uint32_t       *a_mont,
                                   uint32_t             *out_mont)
{
  uint32_t *product = alloca (4 * info->len*2);
  dsk_tls_bignum_square (info->len, a_mont, product);
  dsk_tls_bignum_montgomery_reduce (info, product, out_mont);
}

void dsk_tls_bignum_exponent_montgomery
                                  (DskTlsMontgomeryInfo *info,
                                   const uint32_t       *base_mont,
                                   unsigned              exponent_len,
                                   const uint32_t       *exponent,
                                   uint32_t             *out_mont)
{
  int hi_bit = find_highest_set_bit (exponent_len, exponent);
  if (hi_bit == -1)
    {
      dsk_tls_bignum_word_to_montgomery (info, 1, out_mont);
      return;
    }
  uint32_t *base_2_i = alloca (info->len * 4);
  bool out_inited = false;
  memcpy (base_2_i, base_mont, info->len * 4);
  for (unsigned i = 0; i < (unsigned) hi_bit; i++)
    {
      if (((exponent[i/32] >> (i%32)) & 1) == 1)
        {
          if (out_inited)
            dsk_tls_bignum_multiply_montgomery (info, out_mont, base_2_i, out_mont);
          else
            {
              memcpy (out_mont, base_2_i, 4 * info->len);
              out_inited = true;
            }
        }

      dsk_tls_bignum_square_montgomery (info, base_2_i, base_2_i);
    }

  // Find (highest) bit is always 1.
  if (out_inited)
    {
      dsk_tls_bignum_multiply_montgomery (info, out_mont, base_2_i, out_mont);
    }
  else
    {
      memcpy (out_mont, base_2_i, 4 * info->len);
    }
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

void dsk_tls_bignum_dump_montgomery(DskTlsMontgomeryInfo *info,
                                    const char *label)
{
  printf("static const uint32_t %s__N[] = {\n", label);
  for (unsigned i = 0 ; i < info->len; i++)
    printf("  0x%08x,\n", info->N[i]);
  printf("};\n");

  printf("static const uint32_t %s__barrett_mu[] = {\n", label);
  for (unsigned i = 0 ; i < info->len + 1; i++)
    printf("  0x%08x,\n", info->barrett_mu[i]);
  printf("};\n");

  printf("static DskTlsMontgomeryInfo %s__mont = {\n"
         "  %u,\n"
         "  %s__N,\n"
         "  0x%08x,\n"
         "  %s__barrett_mu\n"
         "};\n"
         , label, info->len, label, info->Nprime, label);
}

uint32_t dsk_tls_bignum_mod_word (unsigned len,
                                   const uint32_t *value,
                                   uint32_t modulus)
{
  uint32_t pow = (uint32_t) ((1ULL << 32) % modulus);
  uint32_t rv = 0;
  for (unsigned i = 0; i < len; i++)
    rv = ((uint64_t) rv * pow % modulus + value[len - 1 - i]) % modulus;
  return rv;
}

//
// Returns whether the number is definitely composite.
//
static bool
miller_rabin_test_step_is_composite
                       (DskTlsMontgomeryInfo *mont,
                        unsigned              d_len,
                        const uint32_t       *d,
                        unsigned              r,
                        uint32_t              a,
                        const uint32_t       *mont_1,
                        const uint32_t       *mont_m1)
{
  uint32_t *a_mont = alloca(4 * mont->len);
  uint32_t *x_mont = alloca(4 * mont->len);
  dsk_tls_bignum_word_to_montgomery (mont, a, a_mont);
  dsk_tls_bignum_exponent_montgomery (mont, a_mont, d_len, d, x_mont);
  if (memcmp (x_mont, mont_1, mont->len * 4) == 0
   || memcmp (x_mont, mont_m1, mont->len * 4) == 0)
    {
      return false;
    }

  for (unsigned i = 0; i < r - 1; i++)
    {
      dsk_tls_bignum_square_montgomery (mont, x_mont, x_mont);
      if (memcmp (x_mont, mont_m1, mont->len * 4) == 0)
        {
          return false;
        }
    }
  return true;
}

static uint32_t miller_rabin_rounds[] =
{
     2,   3,   5,   7,  11,  13,  17,  19,  23,  29,  31,
    37,  41,  43,  47,  53,  59,  61,  67,  71,  73,  79,
    83,  89,  97, 101, 103, 107, 109, 113, 127, 131, 137,
   139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193,
   197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 257,
   263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317,
   331, 337, 347, 349
};

static bool
is_probable_prime  (unsigned       len,
                    const uint32_t *n,
                    unsigned        rounds)
{
  // Ensure the number is 2, or it must be odd.
  if (len == 1)
    {
      if (n[0] == 2)
        return true;
    }
  if ((n[0] & 1) == 0)
    return false;

  DskTlsMontgomeryInfo info;
  dsk_tls_montgomery_info_init (&info, len, n);
  uint32_t *mont_1 = alloca(4 * len);
  dsk_tls_bignum_word_to_montgomery (&info, 1, mont_1);
  uint32_t *mont_m1 = alloca(4 * len);
  dsk_tls_bignum_subtract_with_borrow (len, n, mont_1, 0, mont_m1);

  // Compute d,d_len,r such that n=2^r*d + 1 with d odd.
  // r is the second-lowest bit position
  // (the first-lowest bit position is 0 since n is odd)
  unsigned r = 1;
  while (r < len*32
      && ((n[r/32] >> (r%32)) & 1) == 0)
    r += 1;
  uint32_t *d = alloca(len * 4);
  dsk_tls_bignum_shiftright_truncated (len, n, r, len, d);
  unsigned d_len = dsk_tls_bignum_actual_len (len, d);

  rounds = DSK_MIN (DSK_N_ELEMENTS (miller_rabin_rounds), rounds);
  for (unsigned i = 0; i < rounds; i++)
    {
      // pick a.
      uint32_t a = miller_rabin_rounds[i];
      if (miller_rabin_test_step_is_composite (&info,
                                               d_len, d, r,
                                               a, mont_1, mont_m1))
        {
          dsk_tls_montgomery_info_clear (&info);
          return false;
        }
    }
  dsk_tls_montgomery_info_clear (&info);
  return true;
}


//
// This comes from the Table 7.2 from Understanding Crytography by Christof Paar,
// which claims that this number of rounds leaves an error probability < 2^{-80}.
// 
static inline unsigned
rounds_from_len_in_words (unsigned len)
{
  if (len < 250/32)
    return 11;
  else if (len < 300/32)
    return 9;
  else if (len < 400/32)
    return 6;
  else if (len < 500/32)
    return 5;
  else
    return 3;
}
    
bool
dsk_tls_bignum_is_probable_prime (unsigned len,
                                  const uint32_t *p)
{
  if (len == 1)
    {
      for (unsigned i = 0; i < DSK_N_ELEMENTS (miller_rabin_rounds); i++)
        if (miller_rabin_rounds[i] == p[0])
          return true;
    }
  unsigned rounds = rounds_from_len_in_words (len);
  return is_probable_prime (len, p, rounds);
}

bool
dsk_tls_bignum_find_probable_prime (unsigned len,
                                    uint32_t *inout)
{
  uint32_t mod30 = dsk_tls_bignum_mod_word (len, inout, 30);

  // This function is for finding cryptographic primes,
  // not general purpose use.
  assert (len > 1);
  assert (inout[len-1] != 0);

  //
  // Add 1 to the number of rounds which even without the +1 
  // gives a 2^{-80} error rate.
  //
  unsigned rounds = rounds_from_len_in_words (len) + 1;
  for (;;)
    {
      if (mod30 % 5 != 0 && mod30 % 2 != 0 && mod30 % 3 != 0)
        {
          if (is_probable_prime (len, inout, rounds))
            return true;
        }
      if (++mod30 == 30)
        mod30 = 0;
      if (dsk_tls_bignum_add_word_inplace  (len, inout, 1))
        // failed to find prime before wrapping.
        return false;
    }
}

