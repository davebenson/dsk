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
    { "2", "3", "000000006" },
    { "1001", "100001", "100101001"},
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
      assert(A->len + B->len == product->len);          // test correctly formed?
      dsk_tls_bignum_multiply (A->len, A->value, B->len, B->value, result);
      //PR_BIGNUM("result 1", A->len + B->len, result);
      assert(memcmp (product->value, result, 4 * (A->len + B->len)) == 0);
      dsk_tls_bignum_multiply (B->len, B->value, A->len, A->value, result);
      assert(memcmp (product->value, result, 4 * (A->len + B->len)) == 0);
      free (A);
      free (B);
      free (product);
      free (result);
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

static struct 
{
  const char *name;
  void (*test)(void);
} tests[] =
{
  { "Multiplicative inverse mod 1<<32", test_uint32_inverse },
  { "Bignum Multiply", test_multiply_full },
  { "Bignum Compare", test_compare },
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
