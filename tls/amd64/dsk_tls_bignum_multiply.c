#include <stdint.h>
#include <string.h>

uint32_t dsk_tls_bignum_mul64word_addto (uint32_t value,
                                         unsigned n64,
                                         const uint64_t *data64,
                                         uint64_t  *in_out);
uint32_t dsk_tls_bignum_mul64word       (uint32_t value,
                                         unsigned n64,
                                         const uint64_t *data64,
                                         uint64_t  *out);
void     dsk_tls_bignum_multiply64      (unsigned  a_len,
                                         const uint64_t *a_data,
                                         unsigned  b_len,
                                         const uint64_t *b_data,
                                         uint64_t  *out);


//
// This routine uses several helper routines to do the hard work;
//
// I guess for bigger numbers we might use a Karatsuba method.
// Cryptographic-size numbers are not benefited by the fft-based method,
// the overhead is too high.
//
// what we're left with is a bunch of conditionals and bookkeeping.
//
void
dsk_tls_bignum_multiply (unsigned a_len, const uint32_t *a_data,
                         unsigned b_len, const uint32_t *b_data,
                         uint32_t *out)
{
  // Sort a,b so that a_len >= b_len.
  if (a_len < b_len)
    {
      unsigned tmp_len = a_len; a_len = b_len; b_len = tmp_len;
      const uint32_t *tmp_data = a_data; a_data = b_data; b_data = tmp_data;
    }

  // Handle 0-length and 1-length products separately.
  // Since 'b' is smaller, after these if statements, a_len >= b_len >= 2.
  if (b_len == 0)
    {
      memset (out, 0, a_len * 4);
      return;
    }
  if (b_len == 1)
    {
      uint32_t carry_a = dsk_tls_bignum_mul64word(b_data[0],
                                                  a_len / 2,
                                                  (uint64_t *) a_data,
                                                  (uint64_t *) out);
      if (a_len & 1)
        ((uint64_t *)out)[a_len/2]
            = (uint64_t) b_data[0] * a_data[a_len-1] + carry_a;
      else
        out[a_len] = carry_a;
      return;
    }

  //
  // Workhorse function to deal with uint64-based multiplies.
  //
  // This routine is optimized for the case that a_len is longer than b_len.
  //
  dsk_tls_bignum_multiply64 (a_len/2, (const uint64_t *) a_data,
                             b_len/2, (const uint64_t *) b_data,
                             (uint64_t *) out);

  //
  // The remaining functions assume that they are adding-to rather
  // than initializing the memory.  So zero out any extra words.
  //
  // We also bail here if a_len and b_len are both even.
  // 

  if (((a_len | b_len) & 1) == 0)
    return;                     // both a_len and b_len are even: terminate.

  //
  // Deal with trailing uint32 entries (the highest word) on a_data and b_data.
  //
  // The workhorse routine dsk_tls_bignum_mul64word_addto
  // takes a uint32 and a vector of uint64s.  This is (untested) more efficient,
  // and avoids the problem of double-counting the final uint32*uint32 if both
  // a_len and b_len are odd.
  //
  if (a_len & 1)
    {
      uint32_t carry_a = dsk_tls_bignum_mul64word_addto (
        a_data[a_len - 1],
        b_len / 2,
        (const uint64_t *) b_data,
        (uint64_t *) (out + a_len / 2 * 2)
      );
      if (b_len & 1)
        {
          uint32_t carry_b = dsk_tls_bignum_mul64word_addto (
            b_data[b_len - 1],
            a_len / 2,
            (const uint64_t *) a_data,
            (uint64_t *) (out + b_len / 2 * 2)
          );
          uint64_t fixup = (uint64_t) a_data[a_len - 1] * b_data[b_len - 1]
                         + (uint64_t) carry_a
                         + (uint64_t) carry_b;
          ((uint64_t *) out)[a_len/2 + b_len/2] = fixup;
        }
      else
        {
          out[a_len + b_len - 1] = carry_a;
        }
    }
  else    // since extra was >0, (a_len&1) == 0 => (b_len & 1) == 1
    {
      // b_len is odd case (a_len is even).
      uint32_t carry_b = dsk_tls_bignum_mul64word_addto (
        b_data[b_len - 1],
        a_len / 2,
        (const uint64_t *) a_data,
        (uint64_t *) (out + b_len / 2 * 2)
      );
      out[a_len + b_len - 1] = carry_b;
    }
}
