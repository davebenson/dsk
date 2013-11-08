
struct _DskCipherInfo
{
  const char *name;
  unsigned key_size;
  void *(*internal0)(void);
  void *internal1;
};

#define WRITE_SIMPLE_CIPHER2(our_name, openssl_name, key_size) \
{ #our_name, 0, (void*(*)(void)) EVP_##openssl_name, NULL }
#define WRITE_SIMPLE_CIPHER(name, key_size) \
  WRITE_SIMPLE_CIPHER2(name, name, key_size)

DskCipherInfo dsk_ciphers[] = {
WRITE_SIMPLE_CIPHER(bf_cbc, 16),
WRITE_SIMPLE_CIPHER(bf_cfb, 16),
WRITE_SIMPLE_CIPHER(bf_ecb, 16),
WRITE_SIMPLE_CIPHER(bf_ofb, 16),
WRITE_SIMPLE_CIPHER(cast5_cbc, 16),
WRITE_SIMPLE_CIPHER(cast5_cfb, 16),
WRITE_SIMPLE_CIPHER(cast5_ecb, 16),
WRITE_SIMPLE_CIPHER(cast5_ofb, 16),
WRITE_SIMPLE_CIPHER(des_cbc, 8),
WRITE_SIMPLE_CIPHER(des_cfb, 8),
WRITE_SIMPLE_CIPHER(des_ecb, 8),
WRITE_SIMPLE_CIPHER(des_ede, 16),
WRITE_SIMPLE_CIPHER(des_ede3, 24),
WRITE_SIMPLE_CIPHER(des_ede3_cbc, 24),
WRITE_SIMPLE_CIPHER(des_ede3_cfb, 24),
WRITE_SIMPLE_CIPHER(des_ede3_ofb, 24),
WRITE_SIMPLE_CIPHER(des_ede_cbc, 16),
WRITE_SIMPLE_CIPHER(des_ede_cfb, 16),
WRITE_SIMPLE_CIPHER(des_ede_ofb, 16),
WRITE_SIMPLE_CIPHER(des_ofb, 8),
WRITE_SIMPLE_CIPHER(desx_cbc, 24),
WRITE_SIMPLE_CIPHER2(null, enc_null, 0),
WRITE_SIMPLE_CIPHER(rc2_40_cbc, 5),
WRITE_SIMPLE_CIPHER(rc2_64_cbc, 8),
WRITE_SIMPLE_CIPHER(rc2_cbc, 16),
WRITE_SIMPLE_CIPHER(rc2_cfb, 16),
WRITE_SIMPLE_CIPHER(rc2_ecb, 16),
WRITE_SIMPLE_CIPHER(rc2_ofb, 16),
WRITE_SIMPLE_CIPHER(rc4, -1),
};
unsigned dsk_n_ciphers = DSK_N_ELEMENTS(dsk_ciphers);

DskOctetFilter *dsk_crypto_filter_new (DskCryptoOptions *options);

