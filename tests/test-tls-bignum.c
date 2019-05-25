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
test_multiply (void)
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
test_square (void)
{
  struct {
    const char *a;
    const char *a_squared;
  } tests[] = {
    { "2", "4" },
    { "100", "10000" },
#include "gmp-compare-square.generated.c"
  };

  for (unsigned i = 0; i < DSK_N_ELEMENTS (tests); i++)
    {
      struct Num *A = parse_hex (tests[i].a);
      struct Num *product = parse_hex (tests[i].a_squared);
      uint32_t *result = malloc(4 * (A->len + A->len));
      //PR_NUM(A);
      //PR_NUM(B);
      //PR_NUM(product);
      //assert(A->len + B->len == product->len);          // test correctly formed?
      dsk_tls_bignum_square (A->len, A->value, result);
      unsigned len = dsk_tls_bignum_actual_len (A->len + A->len, result);
      //PR_BIGNUM("result 1", len, result);
      assert(product->len == len);
      assert(memcmp (product->value, result, 4 * len) == 0);
      free (A);
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
test_shiftleft (void)
{
  struct {
    const char *a;
    unsigned shift;
    unsigned result_len;
    const char *b;
  } tests[] = {
    { "1", 1, 2, "0000000000000002" },
    { "0123456789abcdef",  4, 2, "123456789abcdef0" },
    { "0123456789abcdef",  8, 2, "23456789abcdef00" },
    { "0123456789abcdef", 12, 2, "3456789abcdef000" },
    { "0123456789abcdef", 16, 2, "456789abcdef0000" },
    { "0123456789abcdef", 20, 2, "56789abcdef00000" },
    { "0123456789abcdef", 24, 2, "6789abcdef000000" },
    { "0123456789abcdef", 28, 2, "789abcdef0000000" },
    { "0123456789abcdef", 32, 2, "89abcdef00000000" },
    { "0123456789abcdef", 36, 2, "9abcdef000000000" },
    { "0123456789abcdef", 40, 2, "abcdef0000000000" },
    { "0123456789abcdef", 44, 2, "bcdef00000000000" },
    { "0123456789abcdef", 48, 2, "cdef000000000000" },
    { "0123456789abcdef", 52, 2, "def0000000000000" },
    { "0123456789abcdef", 56, 2, "ef00000000000000" },
    { "0123456789abcdef", 60, 2, "f000000000000000" },
    { "0123456789abcdef", 64, 2, "0000000000000000" },
    { "0123456789abcdef",  4, 1, "9abcdef0" },
    { "0123456789abcdef",  8, 1, "abcdef00" },
    { "0123456789abcdef", 12, 1, "bcdef000" },
    { "0123456789abcdef", 16, 1, "cdef0000" },
    { "0123456789abcdef", 20, 1, "def00000" },
    { "0123456789abcdef", 24, 1, "ef000000" },
    { "0123456789abcdef", 28, 1, "f0000000" },
    { "0123456789abcdef", 32, 1, "00000000" },
  };

  for (unsigned i = 0; i < DSK_N_ELEMENTS (tests); i++)
    {
      struct Num *A = parse_hex (tests[i].a);
      struct Num *B = parse_hex (tests[i].b);
      unsigned *out = malloc(sizeof(uint32_t) * tests[i].result_len);
      //PR_NUM(A);
      //printf("shift=%u result_len=%u\n",tests[i].shift, tests[i].result_len);
      //PR_NUM(B);
      dsk_tls_bignum_shiftleft_truncated (A->len, A->value, tests[i].shift, tests[i].result_len, out);
      assert(B->len == tests[i].result_len);
      assert(memcmp(B->value, out, tests[i].result_len * 4) == 0);
      free (A);
      free (B);
      free(out);
    }
}

static void
test_shiftright (void)
{
  struct {
    const char *a;
    unsigned shift;
    unsigned result_len;
    const char *b;
  } tests[] = {
    { "2", 1, 2, "0000000000000001" },
    { "0123456789abcdef",  4, 2, "00123456789abcde" },
    { "0123456789abcdef",  8, 2, "000123456789abcd" },
    { "0123456789abcdef", 12, 2, "0000123456789abc" },
    { "0123456789abcdef", 16, 2, "00000123456789ab" },
    { "0123456789abcdef", 20, 2, "000000123456789a" },
    { "0123456789abcdef", 24, 2, "0000000123456789" },
    { "0123456789abcdef", 28, 2, "0000000012345678" },
    { "0123456789abcdef", 32, 2, "0000000001234567" },
    { "0123456789abcdef", 36, 2, "0000000000123456" },
    { "0123456789abcdef", 40, 2, "0000000000012345" },
    { "0123456789abcdef", 44, 2, "0000000000001234" },
    { "0123456789abcdef", 48, 2, "0000000000000123" },
    { "0123456789abcdef", 52, 2, "0000000000000012" },
    { "0123456789abcdef", 56, 2, "0000000000000001" },
    { "0123456789abcdef", 60, 2, "0000000000000000" },
    { "0123456789abcdef", 64, 2, "0000000000000000" },
    { "0123456789abcdef",  4, 1, "789abcde" },
    { "0123456789abcdef",  8, 1, "6789abcd" },
    { "0123456789abcdef", 12, 1, "56789abc" },
    { "0123456789abcdef", 16, 1, "456789ab" },
    { "0123456789abcdef", 20, 1, "3456789a" },
    { "0123456789abcdef", 24, 1, "23456789" },
    { "0123456789abcdef", 28, 1, "12345678" },
    { "0123456789abcdef", 32, 1, "01234567" },
    { "0123456789abcdef", 36, 1, "00123456" },
    { "0123456789abcdef", 40, 1, "00012345" },
    { "0123456789abcdef", 44, 1, "00001234" },
    { "0123456789abcdef", 48, 1, "00000123" },
    { "0123456789abcdef", 52, 1, "00000012" },
    { "0123456789abcdef", 56, 1, "00000001" },
    { "0123456789abcdef", 60, 1, "00000000" },
    { "0123456789abcdef", 64, 1, "00000000" },



  };

  for (unsigned i = 0; i < DSK_N_ELEMENTS (tests); i++)
    {
      struct Num *A = parse_hex (tests[i].a);
      struct Num *B = parse_hex (tests[i].b);
      unsigned *out = malloc(sizeof(uint32_t) * tests[i].result_len);
      //PR_NUM(A);
      //printf("shift=%u result_len=%u\n",tests[i].shift, tests[i].result_len);
      //PR_NUM(B);
      dsk_tls_bignum_shiftright_truncated (A->len, A->value, tests[i].shift, tests[i].result_len, out);
      assert(B->len == tests[i].result_len);
      assert(memcmp(B->value, out, tests[i].result_len * 4) == 0);
      free (A);
      free (B);
      free(out);
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
  // trivial test of reduce with small value 
  {
    DskTlsMontgomeryInfo info;
    uint32_t thirteen = 13;
    dsk_tls_montgomery_info_init (&info, 1, &thirteen);
    assert(info.len == 1);
    assert(info.N[0] == 13);
    uint32_t tmp[2];
    tmp[0] = 0;
    tmp[1] = 12;
    uint32_t out[1];
    dsk_tls_bignum_montgomery_reduce (&info, tmp, out);
    assert(out[0] == 12);
  }

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

      memcpy (Mb, PM->value, PM->len * 4);
      memset (Mb + PM->len, 0, (MOD->len - PM->len) * 4);
      dsk_tls_bignum_to_montgomery (&info, Mb, Mb);

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

static void
test_modular_sqrt (void)
{
  static struct {
    const char *p;
    const char *v;
    const char *root;
  } tests[] = {
#include "mod-sqrt-test.c"
  };

  for (unsigned i = 0; i < DSK_N_ELEMENTS (tests); i++)
    {
      struct Num *p = parse_hex (tests[i].p);
      struct Num *v = parse_hex (tests[i].v);
      struct Num *root = parse_hex (tests[i].root);
      uint32_t *out = malloc(4 * p->len);
      //PR_NUM(p);
      //PR_NUM(v);
      //PR_NUM(root);
      if (!dsk_tls_bignum_modular_sqrt (p->len, v->value, p->value, out))
        assert(false);
      bool eq = memcmp (out, root->value, p->len * 4) == 0;
      dsk_tls_bignum_subtract_with_borrow (p->len, p->value, out, 0, out);
      bool eq_neg = memcmp (out, root->value, p->len * 4) == 0;
      assert(eq || eq_neg);

      free(p);
      free(v);
      free(root);
      free(out);
    }
}

static void
test_isprime (void)
{
  struct {
    const char *p;
    bool is_prime;
  } tests[] = {
    { "4", false },
    { "7", true },
    { "9", false },
    { "17", true },
  };

  for (unsigned i = 0; i < DSK_N_ELEMENTS (tests); i++)
    {
      struct Num *p = parse_hex (tests[i].p);
      PR_NUM(p);
      bool prime = dsk_tls_bignum_is_probable_prime (p->len, p->value);
      assert (prime == tests[i].is_prime);
      free (p);
    }
}

static void
test_next_prime (void)
{
  struct {
    const char *p_start;
    const char *p;
  } tests[] = {
    // Generated with a simple brute-force search.
    { "100000001", "10000000f" },
    { "1000000001", "100000001f" },
    { "2000000001", "2000000009" },
    { "4000000001", "4000000007" },
    { "8000000001", "8000000017" },
    { "10000000001", "1000000000f" },
    { "20000000001", "2000000001b" },
    { "40000000001", "4000000000f" },
    { "80000000001", "8000000001d" },
    { "100000000001", "100000000007" },
    { "200000000001", "20000000003b" },
    { "400000000001", "40000000000f" },
    { "800000000001", "800000000005" },
    { "1000000000001", "1000000000015" },
    { "2000000000001", "2000000000045" },
    { "4000000000001", "4000000000037" },
    { "8000000000001", "8000000000015" },
    { "10000000000001", "10000000000015" },
    { "20000000000001", "20000000000005" },
    { "40000000000001", "4000000000009f" },
    { "80000000000001", "80000000000003" },
    { "100000000000001", "100000000000051" },
    { "200000000000001", "200000000000009" },
    { "400000000000001", "400000000000045" },
    { "800000000000001", "800000000000083" },
    { "1000000000000001", "1000000000000021" },
    { "2000000000000001", "200000000000000f" },
    { "4000000000000001", "4000000000000087" },
    { "8000000000000001", "800000000000001d" },
  };

  for (unsigned i = 0; i < DSK_N_ELEMENTS (tests); i++)
    {
      struct Num *p_start = parse_hex (tests[i].p_start);
      struct Num *p = parse_hex (tests[i].p);
      dsk_tls_bignum_find_probable_prime (p_start->len, p_start->value);
      assert (p->len == p_start->len);
      assert (memcmp (p->value, p_start->value, p_start->len * 4) == 0);
      free (p_start);
      free (p);
    }
}

static struct 
{
  const char *name;
  void (*test)(void);
} tests[] =
{
  { "Multiplicative inverse mod 1<<32", test_uint32_inverse },
  { "Multiply", test_multiply },
  //TODO: dsk_tls_bignum_divide()
  { "Square", test_square },
  { "Divide", test_divide_qr },
  { "Compare", test_compare },
  { "Shift-Left", test_shiftleft },
  { "Shift-Right", test_shiftright },
  { "Modular Invert", test_modular_invert },
  { "Barrett's method", test_barrett_mu },
  { "Montgomery multiplication", test_montgomery_multiply },
  //TODO: dsk_tls_bignum_exponent_montgomery()
  { "Modular square-root", test_modular_sqrt },
  { "Is Prime", test_isprime },
  { "Next Prime", test_next_prime },
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
