/*
 * Elliptic Curve Point "Addition", where the curve
 * is defined in the space (Z_p)^2.
 *
 * Points on the curve satisfy:
 *          y^2 = x^3 + a*x + b
 *
 * Similar to the case for real numbers, y^2 = k has either
 * no solutions, or 1 solution for k==0 (the additive identity)
 * or 2 solutions which are negations of each other (mod p).
 */

// http://www.secg.org/sec2-v2.pdf
// https://tools.ietf.org/id/draft-jivsov-ecc-compact-05.html
// 
typedef struct DskTls_ECPrime_Group DskTls_ECPrime_Group;
struct DskTls_ECPrime_Group
{
  unsigned len;
  uint32_t *p;
  uint32_t *barrett_mu;
  uint32_t *a;          // coeff of x in curve formula
  uint32_t *b;          // constant in curve formula
  uint32_t *x;          // base point (x)
  uint32_t *y;          // base point (y)
};

extern const DskTls_ECPrime_Group dsk_tls_ecprime_group_secp192r1;
extern const DskTls_ECPrime_Group dsk_tls_ecprime_group_secp224r1;
extern const DskTls_ECPrime_Group dsk_tls_ecprime_group_secp256r1;
extern const DskTls_ECPrime_Group dsk_tls_ecprime_group_secp384r1;
extern const DskTls_ECPrime_Group dsk_tls_ecprime_group_secp512r1;

void dsk_tls_ec_prime_add (DskTls_ECPrime_Group *group,
                           const uint32_t *x1,
                           const uint32_t *y1,
                           const uint32_t *x2,
                           const uint32_t *y2,
                           uint32_t *x_out,
                           uint32_t *y_out);
    

void dsk_tls_ec_prime_multiply_int (DskTls_ECPrime_Group *group,
                                    const uint32_t *x,
                                    const uint32_t *y,
                                    unsigned factor_len,
                                    const uint32_t *factor,
                                    uint32_t *x_out,
                                    uint32_t *y_out);

// Needed for decompressing points.
bool
dsk_tls_ec_prime_y_from_x (DskTls_ECPrime_Group *group,
                                    const uint32_t *x,
                                    uint32_t *y_out);


extern const DskTlsKeyShareMethod dsk_tls_key_share_secp192r1;
extern const DskTlsKeyShareMethod dsk_tls_key_share_secp224r1;
extern const DskTlsKeyShareMethod dsk_tls_key_share_secp256r1;
extern const DskTlsKeyShareMethod dsk_tls_key_share_secp384r1;
extern const DskTlsKeyShareMethod dsk_tls_key_share_secp512r1;
