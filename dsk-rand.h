
#define DSK_RAND_GET_CLASS(object) DSK_OBJECT_CAST_GET_CLASS(DskRand, object, &dsk_rand_class)

typedef struct _DskRandClass DskRandClass;
typedef struct _DskRand DskRand;
struct _DskRandClass
{
  DskObjectClass base_class;
  void (*seed) (DskRand *rand, size_t N, const uint32_t *seed);
};
struct _DskRand
{
  DskObject base;
  void (*generate32)(DskRand *rand, size_t N, uint32_t *out);
};

#define DSK_MERSENNE_TWISTER_N 624
typedef struct DskRandMersenneTwisterClass DskRandMersenneTwisterClass;
typedef struct DskRandMersenneTwister DskRandMersenneTwister;
struct DskRandMersenneTwisterClass
{
  DskRandClass base_class;
};
struct DskRandMersenneTwister
{
  DskRand base_instance;
  uint32_t mt[DSK_MERSENNE_TWISTER_N]; /* the array for the state vector  */
  unsigned mt_index; 
};
DskRand *dsk_rand_new_mersenne_twister (void);


// xorshift1024*
// http://xorshift.di.unimi.it/xorshift1024star.c
typedef struct _DskRandXorshift1024Class DskRandXorshift1024Class;
typedef struct _DskRandXorshift1024 DskRandXorshift1024;
struct _DskRandXorshift1024Class
{
  DskRandClass base_instance;
};
struct _DskRandXorshift1024
{
  DskRand base_instance;
  uint64_t s[16];
  unsigned p;
  bool has_extra;
  uint32_t extra;
};

DskRand *dsk_rand_new_xorshift1024 (void);

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



#if 0
uint32_t dsk_random_uint32       (void);
int32_t  dsk_random_int_range    (int32_t begin,
                                  int32_t end);
double   dsk_random_double       (void);
double   dsk_random_double_range (double begin,
                                  double end);
#endif

extern DskRandClass dsk_rand_class;
#define DSK_RAND_SUBCLASS_DEFINE(class_static, ClassName, class_name)         \
DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(ClassName);                                \
class_static ClassName##Class class_name ## _class = { {                      \
  DSK_OBJECT_CLASS_DEFINE(ClassName, &dsk_rand_class,                         \
                          class_name ## _init,                                \
                          class_name ## _finalize),                           \
  class_name ## _seed                                                         \
} }

void
dsk_rand_protected_seed_array (unsigned seed_length,
                               const uint32_t *seed,
                               unsigned state_length,
                               uint32_t       *state_out);
