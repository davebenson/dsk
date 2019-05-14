#include "../dsk.h"
#include <stdlib.h>
#include <string.h>


// If the effective number of bits is N, we actually
// use a value of 2**(N-1) followed by N-1 random bits.
//
// For example if we have key_bits%8 == 0, we want the first
// bit 0x80 to be 1 followed by 7 random bits.
void dsk_tls_ffdhe_gen_private_key    (DskTlsFFDHE   *ffdhe,
                                       uint8_t       *key_inout)
{
  uint8_t bit_set = 1 << ((ffdhe->key_bits - 1) % 8);
  key_inout[0] &= bit_set - 1;
  key_inout[0] |= bit_set;
}


static void
raise_to_private_key_power (DskTlsFFDHE   *ffdhe,
                            uint32_t      *cur_pow_mont,
                            const uint8_t *private_key,
                            uint8_t       *out)
{
  const uint8_t *at = private_key + (ffdhe->key_bits+7) / 8 - 1;
  uint8_t bit = 1;
  uint32_t *result = alloca (ffdhe->mont.len * sizeof(uint32_t));
  bool inited_result = false;
  for (unsigned i = 0; i < ffdhe->key_bits - 1; i++)
    {
      if (*at & bit)
        {
          if (!inited_result)
            {
              memcpy (result,
                      cur_pow_mont,
                      sizeof(uint32_t) * ffdhe->mont.len);
              inited_result = true;
            }
          else
            {
              dsk_tls_bignum_multiply_montgomery (&ffdhe->mont,
                                                  result,
                                                  cur_pow_mont,
                                                  result);
            }
        }
      bit <<= 1;
      if (bit == 0)
        {
          at--;
          bit = 1;
        }
      dsk_tls_bignum_square_montgomery (&ffdhe->mont,
                                        cur_pow_mont,
                                        cur_pow_mont);
    }

  // The last iteration of the loop is broken out because
  // we don't want to do an unnecessary square.
  assert (*at & bit);// always true, since gen_private_key sets the highest bit
  if (!inited_result)
    {
      // should never really happen unless your RNG sucks,
      // or its a truly amazing situation.
      dsk_tls_bignum_from_montgomery (&ffdhe->mont,
                                      cur_pow_mont,
                                      result);
    }
  else
    {
      dsk_tls_bignum_multiply_montgomery (&ffdhe->mont,
                                          result,
                                          cur_pow_mont,
                                          result);
      dsk_tls_bignum_from_montgomery (&ffdhe->mont,
                                      result,
                                      result);
    }

  // Convert to out. [take advantage of the fact that all ffdhe's are
  // multiples of 32 bits]
  for (unsigned i = 0; i < ffdhe->mont.len; i++)
    dsk_uint32be_pack (result[i], out + (ffdhe->mont.len - 1 - i) * 4);
}

//
// Generate a public key from a private key.
//
void dsk_tls_ffdhe_compute_public_key (DskTlsFFDHE   *ffdhe,
                                       const uint8_t *private_key,
                                       uint8_t       *public_key_out)
{
  uint32_t *cur_pow_q_mont = alloca (sizeof(uint32_t) * ffdhe->mont.len);

  // TODO: this should probably be precomputed.
  cur_pow_q_mont[0] = 2;
  memset (cur_pow_q_mont + 1, 0, 4 * (ffdhe->mont.len - 1));
  dsk_tls_bignum_to_montgomery (&ffdhe->mont, cur_pow_q_mont, cur_pow_q_mont);

  raise_to_private_key_power (ffdhe,
                              cur_pow_q_mont,
                              private_key,
                              public_key_out);
}


//
// Generate a public key from a private key.
//
void dsk_tls_ffdhe_compute_shared_key (DskTlsFFDHE   *ffdhe,
                                       const uint8_t *private_key,
                                       const uint8_t *other_public_key,
                                       uint8_t       *shared_key_out)
{
  uint32_t *cur_pow_mont = alloca (sizeof(uint32_t) * ffdhe->mont.len);

  // Convert other_public_key into words,
  // then convert to montgomery representation.
  for (unsigned i = 0; i < ffdhe->mont.len; i++)
    cur_pow_mont[i] = dsk_uint32be_parse (other_public_key + (ffdhe->mont.len - 1 - i) * 4);
  dsk_tls_bignum_to_montgomery (&ffdhe->mont, cur_pow_mont, cur_pow_mont);

  raise_to_private_key_power (ffdhe,
                              cur_pow_mont,
                              private_key,
                              shared_key_out);
}

