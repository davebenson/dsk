//
// RSA's encryption/decryption algorithms are basically
// deprecated.  But their signing algo still sees a lot of use.
//
//
typedef struct DskRSAPrivateKey DskRSAPrivateKey;
typedef struct DskRSAPublicKey DskRSAPublicKey;


typedef struct DskRSAPrivateKeyOtherPrime DskRSAPrivateKeyOtherPrime;
struct DskRSAPrivateKeyOtherPrime
{
  unsigned ri_len;
  uint32_t *ri;                  // prime
  unsigned di_len;              // exponent
  uint32_t *di;
  unsigned ti_len;              // coefficient
  uint32_t *ti;
};

// RFC 3447 Appendix 1.2.
struct DskRSAPrivateKey
{
  unsigned version;
  unsigned modulus_length_bytes;        // in bytes
  unsigned modulus_len;                 // aka 'n'
  uint32_t *modulus;
  unsigned public_exp_len;              // aka 'e'
  uint32_t *public_exp;
  unsigned private_exp_len;             // aka 'd'
  uint32_t *private_exp;

  // All other fields are unused and optional.
  unsigned p_len;
  uint32_t *p;
  unsigned q_len;
  uint32_t *q;
  unsigned exp1_len;                    // == d mod (p-1)
  uint32_t *exp1;
  unsigned exp2_len;                    // == d mod (q-1)
  uint32_t *exp2;
  unsigned coefficient_len;             // == q^{-1} mod p
  uint32_t *coefficient;

  unsigned n_other_primes;
  DskRSAPrivateKeyOtherPrime *other_primes;
};

DskRSAPrivateKey *
dsk_rsa_private_key_new  (DskASN1Value            *value);

void
dsk_rsa_private_key_free (DskRSAPrivateKey *pk);


// In RFS 8017 Section 5.1.1 this is routine RSADP.
bool
dsk_rsa_private_key_decrypt (const DskRSAPrivateKey *key,
                             const uint32_t *ciphertext,
                             uint32_t *message_out);
struct DskRSAPublicKey
{
  unsigned modulus_len;                 // aka 'n'
  uint32_t *modulus;
  unsigned modulus_length_bytes;
  unsigned public_exp_len;              // aka 'e'
  uint32_t *public_exp;
};

// In RFS 8017 Section 5.1.1 this is routine RSAEP.
bool dsk_rsa_public_key_encrypt(const DskRSAPublicKey *key,
                                const uint32_t *in,
                                uint32_t *out);


//
// Functions predominately for internal use.
//
void dsk_rsassa_pkcs1_5_sign  (DskRSAPrivateKey *key,
                               DskChecksumType  *checksum_type,
                               size_t            message_length,
                               const uint8_t    *message_data,
                               uint8_t          *signature_out);
bool dsk_rsassa_pkcs1_5_verify(DskRSAPublicKey  *key,
                               DskChecksumType  *checksum_type,
                               size_t            message_length,
                               const uint8_t    *message_data,
                               const uint8_t    *signature);

void dsk_rsassa_pss_sign      (DskRSAPrivateKey *key,
                               DskChecksumType  *checksum_type,
                               size_t            salt_len,
                               DskRand          *rng,
                               size_t            message_length,
                               const uint8_t    *message_data,
                               uint8_t          *signature_out);
bool dsk_rsassa_pss_verify    (DskRSAPublicKey  *key,
                               DskChecksumType  *checksum_type,
                               size_t            salt_len,
                               size_t            message_length,
                               const uint8_t    *message_data,
                               const uint8_t    *signature);
