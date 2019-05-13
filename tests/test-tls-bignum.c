#include "../dsk.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


static void
test_uint32_inverse ()
{
  static uint32_t test_examples[] = {
    1,
    3,
    17,
    0x101,
    0x1001,
    0x10001,
    0x100001,
    0x1000001,
    0x10000001,
    0x20000001,
    0x30000001,
    0x12345671,
    0xffffffff,
  };
  for (unsigned i = 0; i < DSK_N_ELEMENTS (test_examples); i++)
    {
      uint32_t value = test_examples[i];
      uint32_t inverse = dsk_tls_bignum_invert_mod_wordsize32 (value);
      uint32_t product = value * inverse;
      assert (product == 1);
    }
}

struct Num {
  unsigned len;
  uint32_t value[1];
};
struct Num *parse_hex(const char *str)
{
  unsigned len = strlen (str);
  unsigned n_words = (len + 7) / 8;
  struct Num *rv = malloc(sizeof(struct Num) + (n_words-1) * 4);
  char hexbuf[9];
  rv->len = n_words;
  for (unsigned i = 0; i < n_words - 1; i++)
    {
      memcpy(hexbuf, str + len - (8*(i+1)), 8);
      hexbuf[8] = 0;
      rv->value[i] = (uint32_t) strtoul (hexbuf, NULL, 16);
    }
  unsigned last_len = len - 8 * (n_words - 1);
  memcpy (hexbuf, str, last_len);
  hexbuf[last_len] = 0;
  rv->value[n_words - 1] = strtoul (hexbuf, NULL, 16);
  return rv;
}

#define PR_BIGNUM(label, len, v) \
  do{ \
    unsigned pr_len = (len); \
    const uint32_t *pr_v = (v); \
    fprintf(stderr, "%s = [", label); \
    for (unsigned pr_index = 0; pr_index + 1 < pr_len; pr_index++) \
      fprintf(stderr,"%08x ", pr_v[pr_index]); \
    if (pr_len > 0) \
      fprintf(stderr,"%08x", pr_v[pr_len - 1]); \
    fprintf(stderr,"]\n"); \
  }while(0)
#define PR_NUM(num) \
  do{ \
    struct Num *pr_num = (num); \
    PR_BIGNUM(#num, pr_num->len, pr_num->value); \
  }while(0)

static void
test_multiply_full (void)
{
  struct {
    const char *a;
    const char *b;
    const char *product;
  } tests[] = {
    { "2", "3", "6" },
    { "1001", "100001", "100101001"},
#include "gmp-compare-multiply.generated.c"
  };

  for (unsigned i = 0; i < DSK_N_ELEMENTS (tests); i++)
    {
      struct Num *A = parse_hex (tests[i].a);
      struct Num *B = parse_hex (tests[i].b);
      struct Num *product = parse_hex (tests[i].product);
      uint32_t *result = malloc(4 * (A->len + B->len));
      //PR_NUM(A);
      //PR_NUM(B);
      //PR_NUM(product);
      //assert(A->len + B->len == product->len);          // test correctly formed?
      dsk_tls_bignum_multiply (A->len, A->value, B->len, B->value, result);
      unsigned len = dsk_tls_bignum_actual_len (A->len + B->len, result);
      //PR_BIGNUM("result 1", A->len + B->len, result);
      assert(memcmp (product->value, result, 4 * len) == 0);
      dsk_tls_bignum_multiply (B->len, B->value, A->len, A->value, result);
      len = dsk_tls_bignum_actual_len (A->len + B->len, result);
      assert(memcmp (product->value, result, 4 * len) == 0);
      free (A);
      free (B);
      free (product);
      free (result);
    }
}
static void
test_divide_qr (void)
{
  struct {
    const char *a;
    const char *b;
    const char *quotient;
    const char *remainder;
  } tests[] = {
    { "7", "3", "2", "1" },
    { "6", "3", "2", "0" },
    { "1000", "100", "10", "0" },
#include "gmp-compare-divide.generated.c"
  };
  for (unsigned i = 0; i < DSK_N_ELEMENTS (tests); i++)
    {
      struct Num *A = parse_hex (tests[i].a);
      struct Num *B = parse_hex (tests[i].b);
      struct Num *quotient = parse_hex (tests[i].quotient);
      struct Num *remainder = parse_hex (tests[i].remainder);
      assert(A->len >= B->len);
      uint32_t *q = malloc(4 * (A->len - B->len + 1));
      uint32_t *r = malloc(4 * B->len);
      //PR_NUM(A);
      //PR_NUM(B);
      //PR_NUM(quotient);
      //PR_NUM(remainder);
      //assert(A->len + B->len == product->len);          // test correctly formed?
      dsk_tls_bignum_divide (A->len, A->value, B->len, B->value, q, r);
      unsigned len_q = dsk_tls_bignum_actual_len (A->len - B->len + 1, q);
      unsigned len_r = dsk_tls_bignum_actual_len (B->len, r);
      //PR_BIGNUM("result 1", A->len + B->len, result);
      if (len_q == 0)
        {
          assert(quotient->len == 1);
          assert(quotient->value[0] == 0);
        }
      else
        {
          assert(len_q == quotient->len);
          assert(memcmp (quotient->value, q, 4 * len_q) == 0);
        }
      if (len_r == 0)
        {
          assert(remainder->len == 1);
          assert(remainder->value[0] == 0);
        }
      else
        {
          assert(len_r == remainder->len);
          assert(memcmp (remainder->value, r, 4 * len_r) == 0);
        }
      assert(memcmp (quotient->value, q, 4 * len_q) == 0);
      free (A);
      free (B);
      free (quotient);
      free (remainder);
      free (q);
      free (r);
    }
}

static void
test_compare (void)
{
  struct {
    const char *a;
    const char *b;
    int result;
  } tests[] = {
    { "2", "3", -1 },
    { "3", "3", 0 },
    { "149291241458451423712371301", "149291241458451423712371302", -1 },
    { "149291241458451423712371301", "149291241458451423712371301", 0 },
  };

  for (unsigned i = 0; i < DSK_N_ELEMENTS (tests); i++)
    {
      struct Num *A = parse_hex (tests[i].a);
      struct Num *B = parse_hex (tests[i].b);
      assert(A->len == B->len);                 // test correctly formed?
      assert(A->len > 0);
      //PR_NUM(A);
      //PR_NUM(B);
      assert(dsk_tls_bignum_compare (A->len, A->value, B->value) == tests[i].result);
      assert(dsk_tls_bignum_compare (A->len, B->value, A->value) == -tests[i].result);
      free (A);
      free (B);
    }
}

static void
test_modular_invert (void)
{
  struct {
    const char *x;
    const char *p;
    const char *inverse;
  } tests[] = {
    { "2", "3" , "2" },
    { "2", "100000000000000000000001", NULL },
    { "3", "100000000000000000000001", NULL },
    { "1", "100000000000000000000001", NULL },
    { "9999999999999999", "10000000000000000000000000", NULL },
#include "gmp-compare-inverse.generated.c"
  };

  for (unsigned i = 0; i < DSK_N_ELEMENTS (tests); i++)
    {
      struct Num *X = parse_hex (tests[i].x);
      struct Num *P = parse_hex (tests[i].p);
      uint32_t *in = malloc(4 * P->len);
      memset (in, 0, 4 * P->len);
      memcpy (in, X->value, 4 * X->len);
      uint32_t *out = malloc(4 * P->len);
      if (!dsk_tls_bignum_modular_inverse (P->len, in, P->value, out))
        assert(false);

      if (tests[i].inverse != NULL)
        {
          struct Num *INV = parse_hex (tests[i].inverse);
          unsigned out_len = dsk_tls_bignum_actual_len (P->len, out);
          assert (INV->len == out_len);
          assert (memcmp (INV->value, out, out_len * 4) == 0);
        }

      uint32_t *xinv = malloc(4 * (P->len + P->len));
      dsk_tls_bignum_multiply (P->len, in, P->len, out, xinv);
      uint32_t *q = malloc(4 * (P->len + 1));
      uint32_t *r = malloc(4 * P->len);
      dsk_tls_bignum_divide (P->len * 2, xinv, P->len, P->value, q, r);
      assert(r[0] == 1);
      for (unsigned i = 1; i < P->len; i++)
        assert(r[i] == 0);
      free(X);
      free(P);
      free(in);
      free(out);
      free(xinv);
      free(q);
      free(r);
    }
}

static void
test_barrett_mu (void)
{
  static struct {
    const char *big;
    const char *modulus;
    const char *mod;
  } tests[] = {
#include "gmp-compare-modular-reduce.generated.c"
  };

  for (unsigned i = 0; i < DSK_N_ELEMENTS (tests); i++)
    {
      struct Num *big = parse_hex (tests[i].big);
      struct Num *modulus = parse_hex (tests[i].modulus);
      struct Num *mod = parse_hex (tests[i].mod);
      uint32_t *in = malloc(4 * (modulus->len * 2));
      uint32_t *mu = malloc(4 * (modulus->len + 2));
      uint32_t *out = malloc(4 * modulus->len);
      dsk_tls_bignum_compute_barrett_mu (modulus->len, modulus->value, mu);
      memcpy (in, big->value, big->len * 4);
      memset (in + big->len, 0, 4 * (modulus->len * 2 - big->len));
      dsk_tls_bignum_modulus_with_barrett_mu (modulus->len * 2, in,
                                              modulus->len, modulus->value,
                                              mu, out);
      unsigned out_len = dsk_tls_bignum_actual_len (modulus->len, out);
      assert(out_len == mod->len);
      assert(memcmp(mod->value, out, out_len * 4) == 0);

      printf("bm: out_len=%u\n", out_len);

      free(big);
      free(modulus);
      free(mod);
      free(in);
      free(mu);
      free(out);
    }
}

static void
test_montgomery_multiply (void)
{
  static struct {
    const char *a;
    const char *b;
    const char *modulus;
    const char *product_mod;
  } tests[] = {
#include "gmp-compare-modular-multiply.generated.c"
  };
  for (unsigned i = 0; i < DSK_N_ELEMENTS (tests); i++)
    {
      struct Num *A = parse_hex (tests[i].a);
      struct Num *B = parse_hex (tests[i].b);
      struct Num *MOD = parse_hex (tests[i].modulus);
      struct Num *PM = parse_hex (tests[i].product_mod);
      uint32_t *Ma = malloc(MOD->len * 4);
      uint32_t *Mb = malloc(MOD->len * 4);
      uint32_t *Mout = malloc(MOD->len * 4);
      DskTlsMontgomeryInfo info;
      dsk_tls_montgomery_info_init (&info, MOD->len, MOD->value);
      memcpy (Ma, A->value, A->len * 4);
      memset (Ma + A->len, 0, (MOD->len - A->len) * 4);
      dsk_tls_bignum_to_montgomery (&info, Ma, Ma);
      memcpy (Mb, B->value, B->len * 4);
      memset (Mb + B->len, 0, (MOD->len - B->len) * 4);
      dsk_tls_bignum_to_montgomery (&info, Mb, Mb);
      dsk_tls_bignum_multiply_montgomery (&info, Ma, Mb, Mout);
      dsk_tls_bignum_from_montgomery (&info, Mout, Mout);
      unsigned out_len = dsk_tls_bignum_actual_len (MOD->len, Mout);
      assert(out_len == PM->len);
      assert(memcmp(Mout, PM->value, PM->len * 4) == 0);
    }
}

static struct 
{
  const char *name;
  void (*test)(void);
} tests[] =
{
  { "Multiplicative inverse mod 1<<32", test_uint32_inverse },
  { "Bignum Multiply", test_multiply_full },
  { "Bignum Divide", test_divide_qr },
  { "Bignum Compare", test_compare },
  { "Bignum Modular Invert", test_modular_invert },
  { "Barrett's method", test_barrett_mu },
  { "Montgomery multiplication", test_montgomery_multiply },
};


int main(int argc, char **argv)
{
  unsigned i;
  dsk_boolean cmdline_verbose = false;

  dsk_cmdline_init ("test various large number handling functions for TLS",
                    "Test TLS bignum functions",
                    NULL, 0);
  dsk_cmdline_add_boolean ("verbose", "extra logging", NULL, 0,
                           &cmdline_verbose);
  dsk_cmdline_process_args (&argc, &argv);

  for (i = 0; i < DSK_N_ELEMENTS (tests); i++)
    {
      fprintf (stderr, "Test: %s... ", tests[i].name);
      tests[i].test ();
      fprintf (stderr, " done.\n");
    }
  dsk_cleanup ();
  return 0;
}
