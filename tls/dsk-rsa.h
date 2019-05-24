//
// RSA's encryption/decryption algorithms are basically
// deprecated.  But their signing algo still sees a lot of use.
//
//
typedef struct DskRSAPrivateKey DskRSAPrivateKey;


#if 0  /// i don't think anyone uses these
typedef struct DskRSAPrivateKeyOtherPrime DskRSAPrivateKeyOtherPrime;
struct DskRSAPrivateKeyOtherPrime
{
  unsigned ri_len;              // prime
  uint32_t *ri;
  unsigned di_len;              // exponent
  uint32_t *di;
  unsigned ti_len;              // coefficient
  uint32_t *ti;
};
#endif

// RFC 3447 Appendix 1.2.
struct DskRSAPrivateKey
{
  unsigned version;
  unsigned signature_length;            // in bytes
  unsigned modulus_len;
  uint32_t *modulus;
  unsigned public_exp_len;
  uint32_t *public_exp;
  unsigned private_exp_len;
  uint32_t *private_exp;
  unsigned p_len;
  uint32_t *p;
  unsigned q_len;
  uint32_t *q;
  unsigned exp1_len;
  uint32_t *exp1;
  unsigned exp2_len;
  uint32_t *exp2;
  unsigned coefficient_len;
  uint32_t *coefficient;

#if 0
  unsigned n_other_primes;
  DskRSAPrivateKeyOtherPrime *other_primes;
#endif
};

DskRSAPrivateKey *
dsk_rsa_private_key_new  (DskASN1Value            *value);

void
dsk_rsa_private_key_free (DskRSAPrivateKey *pk);




//
// Functions predominately for internal use.
//
void dsk_rsassa_pkcs1_sign  (DskRSAPrivateKey *key,
                             size_t            message_length,
                             const uint8_t    *message_data,
                             uint8_t          *signature_out);

void dsk_rsassa_pss_sign    (DskRSAPrivateKey *key,
                             size_t            message_length,
                             const uint8_t    *message_data,
                             uint8_t          *signature_out);
