//
//
//
// Wyrand generator function (requires __uint128_t support)
inline uint64_t wyrand(void) {
    wyrand_seed += 0xa0761d6478bd642fULL;
    __uint128_t t = (__uint128_t)(wyrand_seed ^ 0xe7037ed1a0b428dbULL) * wyrand_seed;
    return (uint64_t)(t >> 64) ^ (uint64_t)t;
}

