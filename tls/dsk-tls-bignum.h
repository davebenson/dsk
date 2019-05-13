
uint32_t dsk_tls_bignum_subtract_with_borrow (unsigned        len,
                                              const uint32_t *a,
                                              const uint32_t *b,
                                              uint32_t        borrow,
                                              uint32_t       *out);
uint32_t dsk_tls_bignum_add_with_carry       (unsigned   len,
                                              const uint32_t *a,
                                              const uint32_t *b,
                                              uint32_t carry,
                                              uint32_t *out);
uint32_t dsk_tls_bignum_add_word             (unsigned len,
                                              uint32_t *v,
                                              uint32_t carry);

void     dsk_tls_bignum_multiply             (unsigned p_len,
                                              const uint32_t *p_words,
                                              unsigned q_len,
                                              const uint32_t *q_words,
                                              uint32_t *out);

unsigned dsk_tls_bignum_actual_len           (unsigned len,
                                              const uint32_t *v);

//
// Preconditions:
//
// p_len >= q_len.
// quotient_out is of size p_len - q_len + 1.
// remainder_out is of size q.
// q_words[q_len-1] != 0.
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

//
// Compute the number X_inv such that X * X_inv == 1 (mod modulus).
//
// This routine is only used in computing some compile-time tables
// for various primes such with Diffie-Hellman Finite-Field Key Exchange.
//
// Returns whether an inverse existed.
//
bool     dsk_tls_bignum_modular_inverse      (unsigned len,
                                              const uint32_t *X_words,
                                              const uint32_t *modulus_words,
                                              uint32_t *X_inv_out);


typedef struct {
  unsigned len;                 // number of 32-bit words in N.

  // must be odd (in fact, it'll be prime for our purposes).
  const uint32_t *N;

  // "R" in the Montgomery reduction, etc, is 1 << (32 * len).

  // a number such that Rinv * R == 1 (mod N).
  const uint32_t *Rinv;

  // a number such that N*Nprime = -1 (mod R)
  uint32_t Nprime;

  // of length len+1; this is needed to efficiently convert
  // number to montgomery form.
  const uint32_t *barrett_mu;   
} DskTlsMontgomeryInfo;


void dsk_tls_bignum_to_montgomery (DskTlsMontgomeryInfo *info,
                                   const uint32_t       *in,
                                   uint32_t             *out_mont);
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

//
// Compute base^exponent (mod info->modulus).
//
// Note that the runtime is dominated by log2(exponent),
// so we benefit from shorter secret_keys.  The recommended 
// secret_size is given by RFC 7919 Appendix A and also Section 5.2.
//
void dsk_tls_bignum_exponent_montgomery
                                  (DskTlsMontgomeryInfo *info,
                                   uint32_t             *base,
                                   uint32_t             *exponent,
                                   uint32_t             *out);


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
