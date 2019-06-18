
#define DSK_RAND_GET_CLASS(object) DSK_OBJECT_CAST_GET_CLASS(DskRand, object, &dsk_rand_class)

typedef struct DskRandType DskRandType;
struct DskRandType
{
  const char *name;
  unsigned sizeof_instance;
  void (*seed) (void *instance, size_t N, const uint32_t *seed);
  void (*generate32)(void *instance, size_t N, uint32_t *out);
};
typedef struct DskRand DskRand;
struct DskRand
{
  DskRandType *type;    // instance immediately follows "type".
};

DskRandType dsk_rand_type_mersenne_twister;


// xorshift1024*
// http://xorshift.di.unimi.it/xorshift1024star.c

DskRandType dsk_rand_type_xorshift1024;

DskRand *dsk_rand_new (DskRandType *type);

/* seed from /dev/urandom etc */
void     dsk_rand_seed           (DskRand* rand);

/* constant seed */
void     dsk_rand_seed_array     (DskRand* rand,
                                  size_t seed_length,
                                  const uint32_t *seed);

/* Generating random numbers.
      dsk_rand_uint32        [0,UINT32_MAX]
      dsk_rand_int_range     [start, end)  NOTE: half-open
      dsk_rand_double        [0,1)
      dsk_rand_double_range  [start, end)
 */
uint32_t dsk_rand_uint32         (DskRand* rand);
uint64_t dsk_rand_uint64         (DskRand* rand);
int64_t  dsk_rand_int_range      (DskRand* rand,
                                  int64_t  begin,
                                  int64_t  end);
double   dsk_rand_double         (DskRand* rand);
double   dsk_rand_double_range   (DskRand* rand,
                                  double   begin,
                                  double   end);

DskRand *dsk_rand_get_global     (void);

void     dsk_rand_gaussian_pair  (DskRand  *rand,
                                  double   *values);



void
dsk_rand_protected_seed_array (unsigned seed_length,
                               const uint32_t *seed,
                               unsigned state_length,
                               uint32_t       *state_out);
