//
// These functions are NOT intended to provide
// an easy big-integer library.
//
// Instead they are intended to be used
// to implement various cryptographic functions:
//
// Various operations:
//    * Diffie-Hellman Finite-Field Key Enchange
//    * Elliptic Curve Point Addition/Doubling
//    * RSA-style key-generation (ie generating primes)
//
// Modular Square-Root:
//    * needed for Elliptic Point decompression
//
// Montgomery's method and Barrett's method are implemented
// for modular multiplication.
//
// TODO: in many cases, aliasing is allowed
// (esp for add,subtract functions), we should document exactly when
// aliasing is allowed and assert test that in the implementations.
// Also, add and subtract allow aliasing, but only all-or-nothing
// aliasing... for example add(len, a+1, a, a+2) is not allowed.
// OTOH, multiplication currently uses a temp area which allows
// for almost any aliasing situation.
//    

bool     dsk_tls_bignum_is_zero              (unsigned        len,
                                              const uint32_t *v);
uint32_t dsk_tls_bignum_subtract_with_borrow (unsigned        len,
                                              const uint32_t *a,
                                              const uint32_t *b,
                                              uint32_t        borrow,
                                              uint32_t       *out);
#define dsk_tls_bignum_subtract(len,a,b,out) \
        dsk_tls_bignum_subtract_with_borrow(len,a,b,0,out)
uint32_t dsk_tls_bignum_add_with_carry       (unsigned   len,
                                              const uint32_t *a,
                                              const uint32_t *b,
                                              uint32_t carry,
                                              uint32_t *out);
#define dsk_tls_bignum_add(len,a,b,out) \
        dsk_tls_bignum_add_with_carry(len,a,b,0,out)
uint32_t dsk_tls_bignum_add_word_inplace     (unsigned len,
                                              uint32_t *v,
                                              uint32_t carry);
uint32_t dsk_tls_bignum_subtract_word_inplace(unsigned len,
                                              uint32_t *v,
                                              uint32_t carry);
uint32_t dsk_tls_bignum_add_word             (unsigned len,
                                              const uint32_t *in,
                                              uint32_t carry,
                                              uint32_t *out);
uint32_t dsk_tls_bignum_subtract_word_inplace(unsigned len,
                                              uint32_t *v,
                                              uint32_t carry);
uint32_t dsk_tls_bignum_subtract_word        (unsigned len,
                                              const uint32_t *in,
                                              uint32_t carry,
                                              uint32_t *out);

void     dsk_tls_bignum_multiply             (unsigned p_len,
                                              const uint32_t *p_words,
                                              unsigned q_len,
                                              const uint32_t *q_words,
                                              uint32_t *out);
void     dsk_tls_bignum_multiply_truncated   (unsigned a_len,
                                              const uint32_t *a_words,
                                              unsigned b_len,
                                              const uint32_t *b_words,
                                              unsigned out_len,
                                              uint32_t *product_words_out);
uint32_t dsk_tls_bignum_multiply_word        (unsigned len,
                                              const uint32_t *in,
                                              uint32_t word,
                                              uint32_t *out);
void     dsk_tls_bignum_multiply_2x2         (const uint32_t *a,
                                              const uint32_t *b,
                                              uint32_t       *out);
void     dsk_tls_bignum_multiply_3x3         (const uint32_t *a,
                                              const uint32_t *b,
                                              uint32_t       *out);
void     dsk_tls_bignum_multiply_4x4         (const uint32_t *a,
                                              const uint32_t *b,
                                              uint32_t       *out);
void     dsk_tls_bignum_multiply_5x5         (const uint32_t *a,
                                              const uint32_t *b,
                                              uint32_t       *out);
void     dsk_tls_bignum_multiply_6x6         (const uint32_t *a,
                                              const uint32_t *b,
                                              uint32_t       *out);
void     dsk_tls_bignum_multiply_7x7         (const uint32_t *a,
                                              const uint32_t *b,
                                              uint32_t       *out);
void     dsk_tls_bignum_multiply_8x8         (const uint32_t *a,
                                              const uint32_t *b,
                                              uint32_t       *out);
void     dsk_tls_bignum_square               (unsigned len,
                                              const uint32_t *words,
                                              uint32_t *out);

unsigned dsk_tls_bignum_actual_len           (unsigned len,
                                              const uint32_t *v);

// highest bit that's one, or -1 if v is zero.
int      dsk_tls_bignum_max_bit              (unsigned len,
                                              const uint32_t *v);

void     dsk_tls_bignum_shiftleft_truncated  (unsigned len,
                                              const uint32_t *in,
                                              unsigned amount,
                                              unsigned out_len,
                                              uint32_t *out);
void     dsk_tls_bignum_shiftright_truncated (unsigned len,
                                              const uint32_t *in,
                                              unsigned amount,
                                              unsigned out_len,
                                              uint32_t *out);

//
//
// Compute p / q and p % q (where p, q are positive integers).
//
// Preconditions:
//
// * p_len >= q_len.      (otherwise, quotient==0, remainder==p)
// * quotient_out is of size p_len - q_len + 1.
// * remainder_out is of size q_len.
// * q_words[q_len-1] != 0.
//
void     dsk_tls_bignum_divide               (unsigned p_len,
                                              const uint32_t *p_words,
                                              unsigned q_len,
                                              const uint32_t *q_words,
                                              uint32_t *quotient_out,
                                              uint32_t *remainder_out);

int      dsk_tls_bignum_compare              (unsigned len,
                                              const uint32_t *a,
                                              const uint32_t *b);


void     dsk_tls_bignum_modular_add          (unsigned        len,
                                              const uint32_t *a_words,
                                              const uint32_t *b_words,
                                              const uint32_t *modulus_words,
                                              uint32_t       *out);
void     dsk_tls_bignum_modular_subtract     (unsigned        len,
                                              const uint32_t *a_words,
                                              const uint32_t *b_words,
                                              const uint32_t *modulus_words,
                                              uint32_t       *out);

//
// Compute the number X_inv such that X * X_inv == 1 (mod modulus).
//
// This routine is only used in computing some compile-time tables
// for various primes such with Diffie-Hellman Finite-Field Key Exchange.
//
// Returns whether an inverse existed (which is true iff X and modulus
// are relatively prime).
//
bool     dsk_tls_bignum_modular_inverse      (unsigned len,
                                              const uint32_t *X_words,
                                              const uint32_t *modulus_words,
                                              uint32_t *X_inv_out);

//
// This is fairly slow in some cases (modulus==1 % 4),
// but (log modulus)^2 order.  
//
// This routine may return a false negative
// if modulus_words is non-prime.
// (modular sqrt is as hard as factorization for composite numbers)
//
// TODO: there's an optimization for modulus==5 % 8.
// Not sure if that's important in Elliptic Curve decompression;
// the modulus is carefully chosen in those applications.
// See the wiki page for quadratic residues.
//
bool     dsk_tls_bignum_modular_sqrt         (unsigned len,
                                              const uint32_t *X_words,
                                              const uint32_t *modulus_words,
                                              uint32_t *X_sqrt_out);

//
// Montgomery Representation mod p.
//
// The montgomery representation of n (mod p)
// is simply (n << (len*32)) % p.
// (We are using b = 2^32 throughout this code for the base).
//
typedef struct {
  unsigned len;                 // number of 32-bit words in N.

  // must be odd (in fact, it'll be prime for our purposes).
  const uint32_t *N;

  // "R" in the Montgomery reduction, etc, is 1 << (32 * len).

  // a number such that Rinv * R == 1 (mod N).
  //const uint32_t *Rinv;

  // a number such that N*Nprime = -1 (mod R)
  uint32_t Nprime;

  // of length len+1; this is needed to efficiently convert
  // number to montgomery form.
  const uint32_t *barrett_mu;   
} DskTlsMontgomeryInfo;


void dsk_tls_montgomery_info_init  (DskTlsMontgomeryInfo *info,
                                    unsigned len,
                                    const uint32_t *N);
void dsk_tls_montgomery_info_clear  (DskTlsMontgomeryInfo *info);
void dsk_tls_bignum_to_montgomery (DskTlsMontgomeryInfo *info,
                                   const uint32_t       *in,
                                   uint32_t             *out_mont);
void dsk_tls_bignum_word_to_montgomery (DskTlsMontgomeryInfo *info,
                                        uint32_t              word,
                                        uint32_t             *out);

// NOTE: if one of a_mont or b_mont is not in montgomery form,
// then this outputs a non-montgomery form.
void dsk_tls_bignum_multiply_montgomery
                                  (DskTlsMontgomeryInfo *info,
                                   const uint32_t       *a_mont,
                                   const uint32_t       *b_mont,
                                   uint32_t             *out_mont);
void dsk_tls_bignum_square_montgomery
                                  (DskTlsMontgomeryInfo *info,
                                   const uint32_t       *a_mont,
                                   uint32_t             *out_mont);
void dsk_tls_bignum_from_montgomery
                                  (DskTlsMontgomeryInfo *info,
                                   const uint32_t       *in_mont,
                                   uint32_t             *out);
void dsk_tls_bignum_dump_montgomery(DskTlsMontgomeryInfo *info,
                                    const char *label);

void
dsk_tls_bignum_montgomery_reduce (DskTlsMontgomeryInfo *info,
                                  uint32_t             *T,
                                  uint32_t             *out);
//
// Compute base^exponent (mod info->modulus).
//
// Note that the runtime is dominated by log2(exponent),
// so we benefit from shorter secret_keys.  The recommended 
// secret_size is given by RFC 7919 Appendix A and also Section 5.2.
//
void dsk_tls_bignum_exponent_montgomery
                                  (DskTlsMontgomeryInfo *info,
                                   const uint32_t       *base_mont,
                                   unsigned              exponent_len,
                                   const uint32_t       *exponent,
                                   uint32_t             *out_mont);


void
dsk_tls_bignum_compute_barrett_mu (unsigned              len,
                                   const uint32_t       *modulus,
                                   uint32_t             *out            // of length len+2, but out[len+1]==0
                                   );
// ASSERT: for now, len == 2 * modulus_len
void
dsk_tls_bignum_modulus_with_barrett_mu (unsigned        len,
                                        const uint32_t *value,
                                        unsigned        modulus_len,
                                        const uint32_t *modulus,
                                        const uint32_t *barrett_mu,
                                        uint32_t       *mod_value_out);


uint32_t dsk_tls_bignum_invert_mod_wordsize32 (uint32_t v);


//
// Primality testing.
//
bool dsk_tls_bignum_find_probable_prime (unsigned len,
                                         uint32_t *inout);
bool dsk_tls_bignum_is_probable_prime (unsigned len,
                                       const uint32_t *p);
