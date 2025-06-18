
typedef struct DskTlsCryptoRNG DskTlsCryptoRNG;
struct DskTlsCryptoRNG
{
  void (*read)(DskTlsCryptoRNG *rng,
               size_t           len,
               uint8_t         *data);
};


extern DskTlsCryptoRNG dsk_tls_crypto_rng_default;

struct DskTlsCryptoRNG_Test
{
  DskTlsCryptoRNG base;
  uint8_t next;
};
#define DSK_TLS_CRYPTO_RNG_TEST_INIT(init_value) \
  { { dsk_tls_crypto_rng_test_read }, (init_value) }


typedef struct DskTlsCryptoRNG_Nonzero DskTlsCryptoRNG_Nonzero;
struct DskTlsCryptoRNG_Nonzero
{
  DskTlsCryptoRNG base;
  DskTlsCryptoRNG *underlying;
};
#define DSK_TLS_CRYPTO_RNG_NONZERO_INIT(under) \
  { { dsk_tls_crypto_rng_nonzero_read }, (under) }



extern void dsk_tls_crypto_rng_test_read   (DskTlsCryptoRNG *rng,
                                            size_t           len,
                                            uint8_t         *data);
extern void dsk_tls_crypto_rng_nonzero_read(DskTlsCryptoRNG *rng,
                                            size_t           len,
                                            uint8_t         *data);

void dsk_get_cryptorandom_data (size_t length,
                                uint8_t *data);
