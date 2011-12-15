typedef struct _DskCryptoOptions DskCryptoOptions;
typedef struct _DskCipherInfo DskCipherInfo;

struct _DskCryptoOptions
{
  const char *algo;
  int algo_index;
  unsigned key_length;
  const uint8_t *key_data;
  dsk_boolean encrypt;
};

struct _DskCipherInfo
{
  const char *name;
  unsigned key_size;
  void *(*internal0)(void);
  void *internal1;
};

DskOctetFilter *dsk_crypto_filter_new (DskCryptoOptions *options,
                                       DskError        **error);

