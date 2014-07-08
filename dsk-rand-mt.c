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

void
dsk_rand_init (DskRand *rand)
{
  uint32_t seed[4];
  struct timeval now;
  static dsk_boolean dev_urandom_exists = DSK_TRUE;

  if (dev_urandom_exists)
    {
      FILE* dev_urandom;

      do
        {
	  errno = 0;
	  dev_urandom = fopen("/dev/urandom", "rb");
	}
      while (errno == EINTR);

      if (dev_urandom)
	{
	  int r;

	  setvbuf (dev_urandom, NULL, _IONBF, 0);
	  do
	    {
	      errno = 0;
	      r = fread (seed, sizeof (seed), 1, dev_urandom);
	    }
	  while (errno == EINTR);

	  if (r != 1)
	    dev_urandom_exists = DSK_FALSE;

	  fclose (dev_urandom);
	}	
      else
	dev_urandom_exists = DSK_FALSE;
    }

  if (!dev_urandom_exists)
    {  
      gettimeofday (&now, NULL);
      seed[0] = now.tv_sec;
      seed[1] = now.tv_usec;
      seed[2] = getpid ();
      seed[3] = getppid ();
    }

  return dsk_rand_init_seed_array (rand, 4, seed);
}

void
dsk_rand_init_seed (DskRand* rand, uint32_t seed)
{
  /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
  /* In the previous version (see above), MSBs of the    */
  /* seed affect only MSBs of the array mt[].            */
      
  rand->mt[0]= seed;
  for (rand->mt_index=1; rand->mt_index<N; rand->mt_index++)
    rand->mt[rand->mt_index] = 1812433253UL * 
      (rand->mt[rand->mt_index-1] ^ (rand->mt[rand->mt_index-1] >> 30)) + rand->mt_index; 
}

void
dsk_rand_init_seed_array (DskRand* rand,
                          unsigned seed_length,
                          const uint32_t *seed)
{
  unsigned i, j, k;

  dsk_return_if_fail (seed_length >= 1, "bad seed_length");

  dsk_rand_init_seed (rand, 19650218UL);

  i=1; j=0;
  k = (N>seed_length ? N : seed_length);
  for (; k; k--)
    {
      rand->mt[i] = (rand->mt[i] ^
		     ((rand->mt[i-1] ^ (rand->mt[i-1] >> 30)) * 1664525UL))
	      + seed[j] + j; /* non linear */
      rand->mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
      i++; j++;
      if (i>=N)
        {
	  rand->mt[0] = rand->mt[N-1];
	  i=1;
	}
      if (j>=seed_length)
	j=0;
    }
  for (k=N-1; k; k--)
    {
      rand->mt[i] = (rand->mt[i] ^
		     ((rand->mt[i-1] ^ (rand->mt[i-1] >> 30)) * 1566083941UL))
	      - i; /* non linear */
      rand->mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
      i++;
      if (i>=N)
        {
	  rand->mt[0] = rand->mt[N-1];
	  i=1;
	}
    }

  rand->mt[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */ 
}

uint32_t
dsk_rand_uint32 (DskRand* rand)
{
  uint32_t y;
  static const uint32_t mag01[2]={0x0, MATRIX_A};
  /* mag01[x] = x * MATRIX_A  for x=0,1 */

  if (rand->mt_index >= N) { /* generate N words at one time */
    int kk;
    
    for (kk=0;kk<N-M;kk++) {
      y = (rand->mt[kk]&UPPER_MASK)|(rand->mt[kk+1]&LOWER_MASK);
      rand->mt[kk] = rand->mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1];
    }
    for (;kk<N-1;kk++) {
      y = (rand->mt[kk]&UPPER_MASK)|(rand->mt[kk+1]&LOWER_MASK);
      rand->mt[kk] = rand->mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1];
    }
    y = (rand->mt[N-1]&UPPER_MASK)|(rand->mt[0]&LOWER_MASK);
    rand->mt[N-1] = rand->mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1];
    
    rand->mt_index = 0;
  }
  
  y = rand->mt[rand->mt_index++];
  y ^= TEMPERING_SHIFT_U(y);
  y ^= TEMPERING_SHIFT_S(y) & TEMPERING_MASK_B;
  y ^= TEMPERING_SHIFT_T(y) & TEMPERING_MASK_C;
  y ^= TEMPERING_SHIFT_L(y);
  
  return y; 
}

/* transform [0..2^32] -> [0..1] */
#define DSK_RAND_DOUBLE_TRANSFORM 2.3283064365386962890625e-10

int32_t 
dsk_rand_int_range (DskRand* rand, int32_t begin, int32_t end)
{
  uint32_t dist = end - begin;
  uint32_t random;

  /* maxvalue is set to the predecessor of the greatest
   * multiple of dist less or equal 2^32. */
  uint32_t maxvalue;
  if (dist <= 0x80000000u) /* 2^31 */
    {
      /* maxvalue = 2^32 - 1 - (2^32 % dist) */
      uint32_t leftover = (0x80000000u % dist) * 2;
      if (leftover >= dist) leftover -= dist;
      maxvalue = 0xffffffffu - leftover;
    }
  else
    maxvalue = dist - 1;
  
  do
    random = dsk_rand_uint32 (rand);
  while (random > maxvalue);
  
  random %= dist;
  return begin + random;
}

/* XXX: shouldn't we use the -1 hack. */
double 
dsk_rand_double (DskRand* rand)
{    
#if !DSK_IEEE754
  /* We set all 52 bits after the point for this, not only the first
     32. Thats why we need two calls to dsk_rand_int */
  double retval = dsk_rand_uint32 (rand) * DSK_RAND_DOUBLE_TRANSFORM;
  retval = (retval + dsk_rand_uint32 (rand)) * DSK_RAND_DOUBLE_TRANSFORM;

  /* The following might happen due to very bad rounding luck, but
   * actually this should be more than rare, we just try again then */
  if (DSK_UNLIKELY (retval >= 1.0))
    return dsk_rand_double (rand);

  return retval;
#else
  union { uint64_t i; double v; } u;
  u.i = 0x1ff0000000000000ULL
      | ((uint64_t)(dsk_rand_uint32 (rand) & 0xfffff) << 32)
      | dsk_rand_uint32 (rand);
  return u.v - 1.0;
#endif
}

double 
dsk_rand_double_range (DskRand* rand, double begin, double end)
{
  return dsk_rand_double (rand) * (end - begin) + begin;
}



/* NOTE: in future, may return a thread-local rand */
static inline DskRand *
get_global_rand ()
{
  static DskRand global;
  static dsk_boolean global_initialized = DSK_FALSE;
  if (!global_initialized)
    {
      global_initialized = DSK_TRUE;
      dsk_rand_init (&global);
    }
  return &global;
}

uint32_t dsk_random_uint32       (void)
{
  return dsk_rand_uint32 (get_global_rand ());
}

int32_t  dsk_random_int_range    (int32_t begin,
                                  int32_t end)
{
  return dsk_rand_int_range (get_global_rand (), begin, end);
}

double   dsk_random_double       (void)
{
  return dsk_rand_double (get_global_rand ());
}

double   dsk_random_double_range (double begin,
                                  double end)
{
  return dsk_rand_double_range (get_global_rand (), begin, end);
}

