#include "dsk.h"

static inline uint64_t gen64 (DskRandXorshift1024 *rand)
{
  uint64_t s0 = rand->s[rand->p];
  rand->p += 1;
  rand->p &= 15;
  uint64_t s1 = rand->s[rand->p];
  s1 ^= s1 << 31; // a
  s1 ^= s1 >> 11; // b
  s0 ^= s0 >> 30; // c
  uint64_t prv = s0 ^ s1;
  rand->s[rand->p] = prv;
  return prv * 1181783497276652981LL; 
}

static void
dsk_rand_xorshift1024_generate32(DskRand *rand, size_t N, uint32_t *out)
{
  DskRandXorshift1024 *xs = (DskRandXorshift1024 *) rand;
  if (N <= 0)
    return;
  if (xs->has_extra)
    {
      *out++ = xs->extra;
      N--;
      xs->extra = 0;
    }
  while (N >= 2)
    {
      uint64_t rv64 = gen64 (xs);
#if DSK_IS_BIG_ENDIAN
      *out++ = rv64 >> 32;
      *out++ = rv64 & 0xffffffff;
#elif DSK_IS_LITTLE_ENDIAN
      *out++ = rv64 & 0xffffffff;
      *out++ = rv64 >> 32;
#else
      dsk_assert(DSK_FALSE);
#endif
      N -= 2;
    }
  if (N == 1)
    {
      uint64_t rv64 = gen64 (xs);
#if DSK_IS_BIG_ENDIAN
      *out++ = rv64 >> 32;
      xs->extra = rv64 & 0xffffffff;
#elif DSK_IS_LITTLE_ENDIAN
      *out++ = rv64 & 0xffffffff;
      xs->extra = rv64 >> 32;
#else
      dsk_assert(DSK_FALSE);
#endif
      xs->has_extra = 1;
    }
}
static void
dsk_rand_xorshift1024_seed(DskRand *rand, size_t N, const uint32_t *seed)
{
  DskRandXorshift1024 *xs = (DskRandXorshift1024 *) rand;
  rand->generate32 = dsk_rand_xorshift1024_generate32;
  xs->p = 0;
  xs->has_extra = DSK_FALSE;
  dsk_rand_protected_seed_array (N, seed, 32, (uint32_t *) xs->s);
}

#define dsk_rand_xorshift1024_init NULL
#define dsk_rand_xorshift1024_finalize NULL
DSK_RAND_SUBCLASS_DEFINE(, DskRandXorshift1024, dsk_rand_xorshift1024);
