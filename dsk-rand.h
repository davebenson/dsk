
#define DSK_MERSENNE_TWISTER_N 624
typedef struct _DskRand DskRand;
struct _DskRand
{
  uint32_t mt[DSK_MERSENNE_TWISTER_N]; /* the array for the state vector  */
  unsigned mt_index; 
};

/* these functions may be used instead of init -- which seeds
   'rand' from /dev/urandom. */
void     dsk_rand_init           (DskRand *rand);
void     dsk_rand_init_seed      (DskRand* rand,
                                  uint32_t seed);
void     dsk_rand_init_seed_array(DskRand* rand,
                                  unsigned seed_length,
                                  const uint32_t *seed);

/* Generating random numbers.
      dsk_rand_uint32        [0,UINT32_MAX]
      dsk_rand_int_range     [start, end)  NOTE: half-open
      dsk_rand_double        [0,1]
      dsk_rand_double_range  [start, end]
 */
uint32_t dsk_rand_uint32         (DskRand* rand);
int32_t  dsk_rand_int_range      (DskRand* rand,
                                  int32_t begin,
                                  int32_t end);
double   dsk_rand_double         (DskRand* rand);
double   dsk_rand_double_range   (DskRand* rand,
                                  double begin,
                                  double end);