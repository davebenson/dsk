#include "dsk.h"

typedef struct Xorshift1024 Xorshift1024;
struct Xorshift1024
{
  uint64_t s[16];
  unsigned p;
  bool has_extra;
  uint32_t extra;
};

static inline uint64_t gen64 (Xorshift1024 *rand)
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
xorshift1024_generate32(void *instance, size_t N, uint32_t *out)
{
  Xorshift1024 *xs = instance;
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
///  #if DSK_IS_LITTLE_ENDIAN    maybe *(uint64_t*)out = rv64 on LE platforms?  alignment?
      *out++ = rv64 & 0xffffffff;
      *out++ = rv64 >> 32;
      N -= 2;
    }
  if (N == 1)
    {
      uint64_t rv64 = gen64 (xs);
      *out++ = rv64 & 0xffffffff;
      xs->extra = rv64 >> 32;
      xs->has_extra = 1;
    }
  else
    xs->has_extra = 0;
}
static void
xorshift1024_seed(void *instance, size_t N, const uint32_t *seed)
{
  Xorshift1024 *xs = instance;
  xs->p = 0;
  xs->has_extra = false;
  dsk_rand_protected_seed_array (N, seed, 32, (uint32_t *) xs->s);
}

DskRandType
dsk_rand_type_xorshift1024 =
{
  "XORSHIFT 1024",
  sizeof(Xorshift1024),
  xorshift1024_seed,
  xorshift1024_generate32,
};
