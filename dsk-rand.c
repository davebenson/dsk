#include "dsk.h"

#include <sys/time.h>
#include <math.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>


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
  /* seed affect only MSBs of the array s[].            */
      
  rand->s[0] = seed;
  for (unsigned i = 1; i < 16; i++) {
    rand->s[i] = 1812433253ULL * (rand->s[i-1] ^ (rand->s[i-1] >> 30)) + i;
  }
}

// XXX: these are not 64-bit seeders!!!!!!!
void
dsk_rand_init_seed_array (DskRand* rand,
                          unsigned seed_length,
                          const uint32_t *seed)
{
  unsigned i, j, k;

  dsk_return_if_fail (seed_length >= 1, "bad seed_length");

  dsk_rand_init_seed (rand, 19650218UL);

  i=1; j=0;
  unsigned N = 16;
  k = (N>seed_length ? N : seed_length);
  for (; k; k--)
    {
      rand->s[i] = (rand->s[i] ^
		     ((rand->s[i-1] ^ (rand->s[i-1] >> 30)) * 1664525UL))
	      + seed[j] + j; /* non linear */
      rand->s[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
      i++; j++;
      if (i>=N)
        {
	  rand->s[0] = rand->s[N-1];
	  i=1;
	}
      if (j>=seed_length)
	j=0;
    }
  for (k=N-1; k; k--)
    {
      rand->s[i] = (rand->s[i] ^
		     ((rand->s[i-1] ^ (rand->s[i-1] >> 30)) * 1566083941UL))
	      - i; /* non linear */
      rand->s[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
      i++;
      if (i>=N)
        {
	  rand->s[0] = rand->s[N-1];
	  i=1;
	}
    }

  rand->s[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */ 
}

uint64_t
dsk_rand_uint64 (DskRand* rand)
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

// probably optimizable :)
uint32_t dsk_rand_uint32         (DskRand* rand)
{
  return (uint32_t) dsk_rand_uint64 (rand);
}

int64_t 
dsk_rand_int_range (DskRand* rand, int64_t begin, int64_t end)
{
  uint32_t dist = end - begin;
  uint64_t random;

  /* maxvalue is set to the predecessor of the greatest
   * multiple of dist less or equal 2^64. */
  uint64_t maxvalue;
  if (dist <= 0x8000000000000000llu) /* 2^63 */
    {
      /* maxvalue = 2^64 - 1 - (2^64 % dist) */
      uint32_t leftover = (0x8000000000000000llu % dist) * 2;
      if (leftover >= dist) leftover -= dist;
      maxvalue = 0xffffffffffffffffllu - leftover;
    }
  else
    maxvalue = dist - 1;
  
  do
    random = dsk_rand_uint64 (rand);
  while (random > maxvalue);
  
  random %= dist;
  return begin + random;
}

/* transform [0..2^32] -> [0..1] */
#define DSK_RAND_DOUBLE_TRANSFORM 2.3283064365386962890625e-10

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
  uint64_t r52 = dsk_rand_uint64 (rand) & 0xfffffffffffffULL;
  // TODO: if r52==0, then we should maybe repeat and use an exponent 52 less.
  u.i = 0x1ff0000000000000ULL | r52;
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

