//

struct RandDualMix128 {
  uint64_t state0;
  uint64_t state1;
  dsk_boolean has_extra;
  uint32_t extra;
};

void rand_dual_mix128_seed (void *instance, size_t N, const uint32_t *seed)
{
  DskRandDualMix128 *mix128 = instance;
  ...
}

inline uint64_t dualMix128(RandDualMix128 *mix128) {
  uint64_t mix = state0 + state1;
  mix128->state0 = mix + rotateLeft(mix128->state0, 16);
  mix128->state1 = mix + rotateLeft(mix128->state1, 2);
  return GR * mix;
}


void rand_dual_mix128_generate32(void *instance, size_t N, uint32_t *out)
{
  DskRandDualMix128 *mix128 = instance;
  if (N > 1 && mix128->has_extra)
    {
      *out++ = mix128->extra;
      mix128->has_extra = false;
      N--;
    }
  while (N >= 2)
    {
      uint64_t v = dualMix128(mix128);
      *out++ = (uint32_t) v;
      *out++ = v>>32;
      N -= 2;
    }
  if (N)
    {
      uint64_t v = dualMix128(mix128);
      *out++ = (uint32_t) v;
      mix128->extra = v>>32;
      mix128->has_extra = true;
    }
}

DskRandType dsk_rand_type_dual_mix128 = {
  "DualMix128",
  sizeof(DskRandDualMix128),
  rand_dual_mix128_seed,
  rand_dual_mix128_generate32
};
  
// DualMix128 generator function
}


// Wyrand generator function (requires __uint128_t support)
inline uint64_t wyrand(void) {
    wyrand_seed += 0xa0761d6478bd642fULL;
    __uint128_t t = (__uint128_t)(wyrand_seed ^ 0xe7037ed1a0b428dbULL) * wyrand_seed;
    return (uint64_t)(t >> 64) ^ (uint64_t)t;
}


// xoroshiro128++ generator function
inline uint64_t xoroshiro128pp(void) {
    const uint64_t s0 = xoro_s[0];
    uint64_t s1 = xoro_s[1];
    const uint64_t result = rotateLeft(s0 + s1, 17) + s0;

    s1 ^= s0;
    xoro_s[0] = rotateLeft(s0, 49) ^ s1 ^ (s1 << 21); // a, b
    xoro_s[1] = rotateLeft(s1, 28); // c
    return result;
}


DskRandType
dsk_rand_type_xorshift1024 =
{
  "XORSHIFT 1024",
  sizeof(Xorshift1024),
  xorshift1024_seed,
  xorshift1024_generate32,
};
