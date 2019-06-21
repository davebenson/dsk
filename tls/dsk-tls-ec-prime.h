/*
 * Elliptic Curve Point "Addition", where the curve
 * is defined in the space (Z_p)^2.
 *
 * Points on the curve satisfy:
 *          y^2 = x^3 + a*x + b
 * which is called the "shortened Weierstass form"
 * whose only limitation is that is doesn't work
 * p=2 or p=3.  (Or for prime-power fields).
 * Those need a different set of routines.
 *
 * It is also required that the right-hand side not have
 * any multiple roots, meaning the discriminant := 27*a*a*a + 27*b*b != 0 (mod p).
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
  const char *name;
  unsigned len;
  const uint32_t *p;
  const uint32_t *barrett_mu;
  const uint32_t *a;          // coeff of x in curve formula
  const uint32_t *b;          // constant in curve formula
  const uint32_t *x;          // base point (x)
  const uint32_t *y;          // base point (y)
  const uint32_t *xy0;        // identity, x+y concatenated

  bool is_a_small;
  bool is_a_negative;
  uint32_t small_a;
};

extern const DskTls_ECPrime_Group dsk_tls_ecprime_group_secp192r1;
extern const DskTls_ECPrime_Group dsk_tls_ecprime_group_secp224r1;
extern const DskTls_ECPrime_Group dsk_tls_ecprime_group_secp256r1;
extern const DskTls_ECPrime_Group dsk_tls_ecprime_group_secp384r1;
extern const DskTls_ECPrime_Group dsk_tls_ecprime_group_secp521r1;


bool dsk_tls_ecprime_is_zero (const DskTls_ECPrime_Group *group,
                              const uint32_t *x,
                              const uint32_t *y);
        
void dsk_tls_ecprime_add (const DskTls_ECPrime_Group *group,
                           const uint32_t *x1,
                           const uint32_t *y1,
                           const uint32_t *x2,
                           const uint32_t *y2,
                           uint32_t *x_out,
                           uint32_t *y_out);
    

//
// This is designed for large values of 'factor'.
// So it first converts to Montgomery/Projective form,
// and works in that representation until it is done,
// then converts back to x,y encoding.
//
void dsk_tls_ecprime_multiply_int (const DskTls_ECPrime_Group *group,
                                    const uint32_t *x,
                                    const uint32_t *y,
                                    unsigned factor_len,
                                    const uint32_t *factor,
                                    uint32_t *x_out,
                                    uint32_t *y_out);

void dsk_tls_ecprime_xy_to_prj (const DskTls_ECPrime_Group *group,
                                const uint32_t *x,
                                const uint32_t *y,
                                uint32_t       *xyz_prj_out);
void dsk_tls_ecprime_prj_to_xy (const DskTls_ECPrime_Group *group,
                                const uint32_t *xyz_prj,
                                uint32_t *x,
                                uint32_t *y);
void dsk_tls_ecprime_prj_double(const DskTls_ECPrime_Group *group,
                                const uint32_t *xyz_prj_in,
                                uint32_t *xyz_prg_out);
void dsk_tls_ecprime_prj_add   (const DskTls_ECPrime_Group *group,
                                const uint32_t *xyz_prj_1,
                                const uint32_t *xyz_prj_2,
                                uint32_t *xyz_prg_out);

// Needed for decompressing points.
bool
dsk_tls_ecprime_y_from_x (const DskTls_ECPrime_Group *group,
                                    const uint32_t *x,
                                    uint32_t *y_out);


extern const DskTlsKeyShareMethod dsk_tls_key_share_secp192r1;
extern const DskTlsKeyShareMethod dsk_tls_key_share_secp224r1;
extern const DskTlsKeyShareMethod dsk_tls_key_share_secp256r1;
extern const DskTlsKeyShareMethod dsk_tls_key_share_secp384r1;
extern const DskTlsKeyShareMethod dsk_tls_key_share_secp521r1;
