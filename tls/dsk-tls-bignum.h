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
// for modular multiplication, as well as methods for pseudo-mersenne
// and mersenne primes.
//
// TODO: in many cases, aliasing is allowed
// (esp for add,subtract functions), we should document exactly when
// aliasing is allowed and assert test that in the implementations.
// Also, add and subtract allow aliasing, but only all-or-nothing
// aliasing... for example add(len, a+1, a, a+2) is not allowed.
// OTOH, multiplication currently uses a temp area which allows
// for almost any aliasing situation.
//    

typedef uint32_t DSK_WORD;
typedef uint64_t DSK_DWORD;
#define DSK_WORD_SIZE         4
#define DSK_WORD_BIT_SIZE    32

bool     dsk_tls_bignum_is_zero              (unsigned        len,
                                              const DSK_WORD *v);
DSK_WORD dsk_tls_bignum_subtract_with_borrow (unsigned        len,
                                              const DSK_WORD *a,
                                              const DSK_WORD *b,
                                              DSK_WORD        borrow,
                                              DSK_WORD       *out);
#define dsk_tls_bignum_subtract(len,a,b,out) \
        dsk_tls_bignum_subtract_with_borrow(len,a,b,0,out)
DSK_WORD dsk_tls_bignum_add_with_carry       (unsigned   len,
                                              const DSK_WORD *a,
                                              const DSK_WORD *b,
                                              DSK_WORD carry,
                                              DSK_WORD *out);
#define dsk_tls_bignum_add(len,a,b,out) \
        dsk_tls_bignum_add_with_carry(len,a,b,0,out)
DSK_WORD dsk_tls_bignum_add_word_inplace     (unsigned len,
                                              DSK_WORD *v,
                                              DSK_WORD carry);
DSK_WORD dsk_tls_bignum_subtract_word_inplace(unsigned len,
                                              DSK_WORD *v,
                                              DSK_WORD carry);
DSK_WORD dsk_tls_bignum_add_word             (unsigned len,
                                              const DSK_WORD *in,
                                              DSK_WORD carry,
                                              DSK_WORD *out);
DSK_WORD dsk_tls_bignum_subtract_word_inplace(unsigned len,
                                              DSK_WORD *v,
                                              DSK_WORD carry);
DSK_WORD dsk_tls_bignum_subtract_word        (unsigned len,
                                              const DSK_WORD *in,
                                              DSK_WORD carry,
                                              DSK_WORD *out);

void     dsk_tls_bignum_multiply             (unsigned p_len,
                                              const DSK_WORD *p_words,
                                              unsigned q_len,
                                              const DSK_WORD *q_words,
                                              DSK_WORD *out);
void     dsk_tls_bignum_multiply_truncated   (unsigned a_len,
                                              const DSK_WORD *a_words,
                                              unsigned b_len,
                                              const DSK_WORD *b_words,
                                              unsigned out_len,
                                              DSK_WORD *product_words_out);
DSK_WORD dsk_tls_bignum_multiply_word        (unsigned len,
                                              const DSK_WORD *in,
                                              DSK_WORD word,
                                              DSK_WORD *out);
void     dsk_tls_bignum_multiply_2x2         (const DSK_WORD *a,
                                              const DSK_WORD *b,
                                              DSK_WORD       *out);
void     dsk_tls_bignum_multiply_3x3         (const DSK_WORD *a,
                                              const DSK_WORD *b,
                                              DSK_WORD       *out);
void     dsk_tls_bignum_multiply_4x4         (const DSK_WORD *a,
                                              const DSK_WORD *b,
                                              DSK_WORD       *out);
void     dsk_tls_bignum_multiply_5x5         (const DSK_WORD *a,
                                              const DSK_WORD *b,
                                              DSK_WORD       *out);
void     dsk_tls_bignum_multiply_6x6         (const DSK_WORD *a,
                                              const DSK_WORD *b,
                                              DSK_WORD       *out);
void     dsk_tls_bignum_multiply_7x7         (const DSK_WORD *a,
                                              const DSK_WORD *b,
                                              DSK_WORD       *out);
void     dsk_tls_bignum_multiply_8x8         (const DSK_WORD *a,
                                              const DSK_WORD *b,
                                              DSK_WORD       *out);
void     dsk_tls_bignum_square               (unsigned len,
                                              const DSK_WORD *words,
                                              DSK_WORD *out);

unsigned dsk_tls_bignum_actual_len           (unsigned len,
                                              const DSK_WORD *v);

// highest bit that's one, or -1 if v is zero.
int      dsk_tls_bignum_max_bit              (unsigned len,
                                              const DSK_WORD *v);

void     dsk_tls_bignum_shiftleft_truncated  (unsigned len,
                                              const DSK_WORD *in,
                                              unsigned amount,
                                              unsigned out_len,
                                              DSK_WORD *out);
void     dsk_tls_bignum_shiftright_truncated (unsigned len,
                                              const DSK_WORD *in,
                                              unsigned amount,
                                              unsigned out_len,
                                              DSK_WORD *out);
void    dsk_tls_bignum_shiftright_inplace    (unsigned len,
                                              DSK_WORD *in_out,
                                              unsigned n_bits);

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
                                              const DSK_WORD *p_words,
                                              unsigned q_len,
                                              const DSK_WORD *q_words,
                                              DSK_WORD *quotient_out,
                                              DSK_WORD *remainder_out);

int      dsk_tls_bignum_compare              (unsigned len,
                                              const DSK_WORD *a,
                                              const DSK_WORD *b);
bool     dsk_tls_bignums_equal               (unsigned len,
                                              const DSK_WORD *a,
                                              const DSK_WORD *b);


void     dsk_tls_bignum_modular_add          (unsigned        len,
                                              const DSK_WORD *a_words,
                                              const DSK_WORD *b_words,
                                              const DSK_WORD *modulus_words,
                                              DSK_WORD       *out);
void     dsk_tls_bignum_modular_subtract     (unsigned        len,
                                              const DSK_WORD *a_words,
                                              const DSK_WORD *b_words,
                                              const DSK_WORD *modulus_words,
                                              DSK_WORD       *out);

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
                                              const DSK_WORD *X_words,
                                              const DSK_WORD *modulus_words,
                                              DSK_WORD *X_inv_out);

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
                                              const DSK_WORD *X_words,
                                              const DSK_WORD *modulus_words,
                                              DSK_WORD *X_sqrt_out);

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
  const DSK_WORD *N;

  // "R" in the Montgomery reduction, etc, is 1 << (32 * len).

  // a number such that Rinv * R == 1 (mod N).
  //const DSK_WORD *Rinv;

  // a number such that N*Nprime = -1 (mod R)
  DSK_WORD Nprime;

  // of length len+1; this is needed to efficiently convert
  // number to montgomery form.
  const DSK_WORD *barrett_mu;   
} DskTlsMontgomeryInfo;


void dsk_tls_montgomery_info_init  (DskTlsMontgomeryInfo *info,
                                    unsigned len,
                                    const DSK_WORD *N);
void dsk_tls_montgomery_info_clear  (DskTlsMontgomeryInfo *info);
void dsk_tls_bignum_to_montgomery (DskTlsMontgomeryInfo *info,
                                   const DSK_WORD       *in,
                                   DSK_WORD             *out_mont);
void dsk_tls_bignum_word_to_montgomery (DskTlsMontgomeryInfo *info,
                                        DSK_WORD              word,
                                        DSK_WORD             *out);

// NOTE: if one of a_mont or b_mont is not in montgomery form,
// then this outputs a non-montgomery form.
void dsk_tls_bignum_multiply_montgomery
                                  (DskTlsMontgomeryInfo *info,
                                   const DSK_WORD       *a_mont,
                                   const DSK_WORD       *b_mont,
                                   DSK_WORD             *out_mont);
void dsk_tls_bignum_square_montgomery
                                  (DskTlsMontgomeryInfo *info,
                                   const DSK_WORD       *a_mont,
                                   DSK_WORD             *out_mont);
void dsk_tls_bignum_from_montgomery
                                  (DskTlsMontgomeryInfo *info,
                                   const DSK_WORD       *in_mont,
                                   DSK_WORD             *out);
void dsk_tls_bignum_dump_montgomery(DskTlsMontgomeryInfo *info,
                                    const char *label);

void
dsk_tls_bignum_montgomery_reduce (DskTlsMontgomeryInfo *info,
                                  DSK_WORD             *T,
                                  DSK_WORD             *out);
//
// Compute base^exponent (mod info->modulus).
//
// Note that the runtime is dominated by log2(exponent),
// so we benefit from shorter secret_keys.  The recommended 
// secret_size is given by RFC 7919 Appendix A and also Section 5.2.
//
void dsk_tls_bignum_exponent_montgomery
                                  (DskTlsMontgomeryInfo *info,
                                   const DSK_WORD       *base_mont,
                                   unsigned              exponent_len,
                                   const DSK_WORD       *exponent,
                                   DSK_WORD             *out_mont);


void
dsk_tls_bignum_compute_barrett_mu (unsigned              len,
                                   const DSK_WORD       *modulus,
                                   DSK_WORD             *out            // of length len+2, but out[len+1]==0
                                   );
// ASSERT: for now, len == 2 * modulus_len
void
dsk_tls_bignum_modulus_with_barrett_mu (unsigned        len,
                                        const DSK_WORD *value,
                                        unsigned        modulus_len,
                                        const DSK_WORD *modulus,
                                        const DSK_WORD *barrett_mu,
                                        DSK_WORD       *mod_value_out);


// Compute     base^exponent (mod modulus)     as fast as possible
// given no preparation, as is needed for RSA signing and verifying.
//
// In many applications:
//    the modulus is a pseudomercenne prime - use dsk_tls_bignum_modulus_pseudomersenne32 instead
//    the modulus is fixed - use 
// in most EC situations be chosen as a friendly prime.
// 

void
dsk_tls_bignum_modular_exponent        (unsigned        base_len,
                                        const DSK_WORD *base,
                                        unsigned        exponent_len,
                                        const DSK_WORD *exponent,
                                        unsigned        modulus_len,
                                        const DSK_WORD *modulus,
                                        DSK_WORD       *mod_value_out);

//
// p = 2**(32*prime_size) - c.
//
// Assume N = N_hi || N_lo.
//
// N % p = N_lo + N_hi * c
// The latter side only has one word of overflow.
//
// 'in' is of size prime_size*2, 'out' is of size prime_size.
//
void dsk_tls_bignum_modulus_pseudomersenne32 (unsigned prime_size,
                                              uint16_t c,
                                              const DSK_WORD *in,
                                              DSK_WORD *out);


DSK_WORD dsk_tls_bignum_invert_mod_wordsize32 (DSK_WORD v);


//
// Primality testing.
//
bool dsk_tls_bignum_find_probable_prime (unsigned len,
                                         DSK_WORD *inout);
bool dsk_tls_bignum_is_probable_prime (unsigned len,
                                       const DSK_WORD *p);
