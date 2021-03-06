#include "dsk.h"

#include <sys/time.h>
#include <math.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

DskRand *dsk_rand_new (DskRandType *type)
{
  DskRand *rv = dsk_malloc (sizeof(DskRandType *) + type->sizeof_instance);
  rv->type = type;
  return rv;
}
void     dsk_rand_seed_array     (DskRand* rand,
                                  size_t seed_length,
                                  const uint32_t *seed)
{
  rand->type->seed (rand + 1, seed_length, seed);
}

void
dsk_rand_seed (DskRand *rand)
{
  uint32_t seed[4];
  struct timeval now;
  static bool dev_urandom_exists = true;

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
	    dev_urandom_exists = false;

	  fclose (dev_urandom);
	}	
      else
	dev_urandom_exists = false;
    }

  if (!dev_urandom_exists)
    {  
      gettimeofday (&now, NULL);
      seed[0] = now.tv_sec;
      seed[1] = now.tv_usec;
      seed[2] = getpid ();
      seed[3] = getppid ();
    }

  return dsk_rand_seed_array (rand, 4, seed);
}

static void
seed32 (uint32_t seed,
             unsigned state_length,
             uint32_t       *state_out)
{
  unsigned i;
  /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
  /* In the previous version (see above), MSBs of the    */
  /* seed affect only MSBs of the array s[].            */
      
  state_out[0] = seed;
  for (i = 1; i < state_length; i++)
    state_out[i] = 1812433253ULL * (state_out[i-1] ^ (state_out[i-1] >> 30)) + i;
}

// XXX: these are not 64-bit seeders!!!!!!!
void
dsk_rand_protected_seed_array (unsigned seed_length,
                               const uint32_t *seed,
                               unsigned state_length,
                               uint32_t       *state_out)

{
  unsigned i, j, k;

  dsk_return_if_fail (seed_length >= 1, "bad seed_length");

  seed32 (19650218UL, state_length, state_out);

  i=1; j=0;
  unsigned N = state_length;
  k = (N > seed_length ? N : seed_length);
  for (; k; k--)
    {
      state_out[i] = (state_out[i] ^
		     ((state_out[i-1] ^ (state_out[i-1] >> 30)) * 1664525UL))
	      + seed[j] + j; /* non linear */
      state_out[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
      i++; j++;
      if (i>=N)
        {
	  state_out[0] = state_out[N-1];
	  i=1;
	}
      if (j>=seed_length)
	j=0;
    }
  for (k=N-1; k; k--)
    {
      state_out[i] = (state_out[i] ^
		     ((state_out[i-1] ^ (state_out[i-1] >> 30)) * 1566083941UL))
	      - i; /* non linear */
      state_out[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
      i++;
      if (i>=N)
        {
	  state_out[0] = state_out[N-1];
	  i=1;
	}
    }

  state_out[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */ 
}

uint64_t
dsk_rand_uint64 (DskRand* rand)
{
  uint32_t rv[2];
  rand->type->generate32 (rand + 1, 2, rv);
  return (((uint64_t)rv[0] << 32)) | ((uint64_t)rv[1]);
}

uint32_t
dsk_rand_uint32 (DskRand* rand)
{
  uint32_t rv[1];
  rand->type->generate32 (rand + 1, 1, rv);
  return rv[0];
}

int64_t 
dsk_rand_int_range (DskRand* rand, int64_t begin, int64_t end)
{
  uint64_t dist = end - begin;
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
  union { uint32_t r[2]; uint64_t rand; } u;
  u.rand = dsk_rand_uint64 (rand);
  double retval = u.r[0] * DSK_RAND_DOUBLE_TRANSFORM;
  retval = (retval + u.r[1]) * DSK_RAND_DOUBLE_TRANSFORM;

  /* The following might happen due to very bad rounding luck, but
   * actually this should be more than rare, we just try again then */
  if (DSK_UNLIKELY (retval >= 1.0))
    return dsk_rand_double (rand);

  return retval;
#else
  union { uint64_t i; double v; } u;
  uint64_t r52 = dsk_rand_uint64 (rand) & 0xfffffffffffffULL;
  // TODO: if r52==0, then we should maybe repeat and use an exponent 52 less.
  //       But I think it's also nice not to require callers to handle
  //       very small non-zero numbers.
  u.i = 0x1ff0000000000000ULL | r52;
  return u.v - 1.0;
#endif
}

double 
dsk_rand_double_range (DskRand* rand, double begin, double end)
{
  return dsk_rand_double (rand) * (end - begin) + begin;
}


DskRand *dsk_rand_get_global     (void)
{
  static DskRand *rv = NULL;
  if (rv == NULL)
    {
      rv = dsk_rand_new (&dsk_rand_type_xorshift1024);
    }
  return rv;
}

