#include "dsk.h"

#include <sys/time.h>
#include <math.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

/* Period parameters */  
#define N DSK_MERSENNE_TWISTER_N
#define M 397
#define MATRIX_A 0x9908b0df   /* constant vector a */
#define UPPER_MASK 0x80000000 /* most significant w-r bits */
#define LOWER_MASK 0x7fffffff /* least significant r bits */

/* Tempering parameters */   
#define TEMPERING_MASK_B 0x9d2c5680
#define TEMPERING_MASK_C 0xefc60000
#define TEMPERING_SHIFT_U(y)  (y >> 11)
#define TEMPERING_SHIFT_S(y)  (y << 7)
#define TEMPERING_SHIFT_T(y)  (y << 15)
#define TEMPERING_SHIFT_L(y)  (y >> 18)

#define DSK_MERSENNE_TWISTER_N 624
typedef struct MersenneTwister MersenneTwister;
struct MersenneTwister
{
  uint32_t mt[DSK_MERSENNE_TWISTER_N]; /* the array for the state vector  */
  unsigned mt_index; 
};

static void
mersenne_twister_generate32 (void     *instance,
                             size_t    count,
                             uint32_t *out)
{
  MersenneTwister *mt = instance;

  static const uint32_t mag01[2]={0x0, MATRIX_A};
  /* mag01[x] = x * MATRIX_A  for x=0,1 */
  uint32_t *mt_arr = mt->mt;

  while (count > 0) 
    {
      unsigned rem = N - mt->mt_index;
      if (rem > count)
        rem = count;
      memcpy (out, mt_arr + mt->mt_index, sizeof (uint32_t) * rem);
      mt->mt_index += rem;
      count -= rem;
      out += rem;

      if (mt->mt_index >= N)
        { /* generate N words at one time */
          int kk;
        
          for (kk = 0; kk < N-M; kk++)
            {
              uint32_t y;
              y = (mt_arr[kk]&UPPER_MASK)|(mt_arr[kk+1]&LOWER_MASK);
              mt_arr[kk] = mt_arr[kk+M] ^ (y >> 1) ^ mag01[y & 0x1];
            }
          for (; kk < N-1; kk++)
            {
              uint32_t y;
              y = (mt_arr[kk]&UPPER_MASK)|(mt_arr[kk+1]&LOWER_MASK);
              mt_arr[kk] = mt_arr[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1];
            }

          /* the last index is special */
          {
            uint32_t y = (mt_arr[N-1]&UPPER_MASK)|(mt_arr[0]&LOWER_MASK);
            mt_arr[N-1] = mt_arr[M-1] ^ (y >> 1) ^ mag01[y & 0x1];
          }

          mt->mt_index = 0;
        }
    }
}

static void
mersenne_twister_seed (void           *instance,
                       size_t          count,
                       const uint32_t *seed)
{
  MersenneTwister *mt = instance;
  mt->mt_index = 0;
  dsk_rand_protected_seed_array (count, seed, DSK_MERSENNE_TWISTER_N, mt->mt);
}

DskRandType
dsk_rand_type_mersenne_twister =
{
  "MersenneTwister",
  sizeof(MersenneTwister),
  mersenne_twister_seed,
  mersenne_twister_generate32
};
