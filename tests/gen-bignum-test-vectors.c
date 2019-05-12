#include <stdio.h>
#include <gmp.h>
#include "../dsk.h"
#include <stdlib.h>
#include <string.h>

struct N {
  __mpz_struct v;
  char *hex;
};

DskRand *rng;

struct N *random_N(unsigned n_hex_digits)
{
  struct N *rv = malloc (sizeof (struct N) + n_hex_digits + 1);
  rv->hex = (char *) (rv + 1);
  for (unsigned i = 0; i < n_hex_digits; i++)
    rv->hex[i] = "0123456789abcdef"[dsk_rand_uint32 (rng) & 15];
  while (rv->hex[n_hex_digits-1] == '0')
    rv->hex[n_hex_digits] = "0123456789abcdef"[dsk_rand_uint32 (rng) & 15];
  rv->hex[n_hex_digits] = 0;
  mpz_init_set_str (&rv->v, rv->hex, 16);
  return rv;
}

typedef struct ScheduleElement ScheduleElement;
struct ScheduleElement
{
  unsigned count;
  unsigned a_len, b_len;
};

typedef void (*GenFunc) (unsigned n_element, ScheduleElement *elements);

static void
gen_func__multiply (unsigned n_element, ScheduleElement *elements)
{
  for (unsigned e = 0; e < n_element; e++)
    for (unsigned i = 0; i < elements[e].count; i++)
      {
        struct N *A = random_N(elements[e].a_len);
        struct N *B = random_N(elements[e].b_len);
        __mpz_struct product;
        mpz_init (&product);
        mpz_mul (&product, &A->v, &B->v);
        printf("{\n  \"");
        mpz_out_str (stdout, 16, &A->v);
        printf("\",\n  \"");
        mpz_out_str (stdout, 16, &B->v);
        printf("\",\n  \"");
        mpz_out_str (stdout, 16, &product);
        printf("\"\n},\n");
      }
}

static void
parse_spec (const char *spec,
            unsigned   *n_out,
            ScheduleElement **out)
{
  printf("parse_spec: %s\n", spec);
  char **pieces = dsk_strsplit (spec, ",");
  *n_out = dsk_strv_length (pieces);
  *out = DSK_NEW_ARRAY (*n_out, ScheduleElement);
  printf("parse_spec: n=%u\n",*n_out);
  for (unsigned i = 0; i < *n_out; i++)
    {
      unsigned count, alen, blen;
      if (sscanf (pieces[i], "%u@%ux%u", &count, &alen, &blen) != 3)
        assert(false);
      (*out)[i].count = count;
      (*out)[i].a_len = alen;
      (*out)[i].b_len = blen;
    }
  dsk_strv_free (pieces);
}

static struct {
  const char *mode;
  GenFunc func;
} gen_table[] = {
  { "multiply", gen_func__multiply },
};

int main(int argc, char **argv)
{
  unsigned i;
  dsk_boolean cmdline_verbose = false;
  const char *cmdline_spec = "10@20x20";
  const char *cmdline_mode = "multiply";

  dsk_cmdline_init ("test various large number handling functions for TLS",
                    "Test TLS bignum functions",
                    NULL, 0);
  dsk_cmdline_add_boolean ("verbose", "extra logging", NULL, 0,
                           &cmdline_verbose);
  dsk_cmdline_add_string ("mode", "mode of operation", "MODE", 0,
                           &cmdline_mode);
  dsk_cmdline_add_string ("spec", "count and size spec", "SPEC", 0,
                           &cmdline_spec);
  dsk_cmdline_process_args (&argc, &argv);

  unsigned n_elements;
  ScheduleElement *elements;
  parse_spec (cmdline_spec, &n_elements, &elements);

  rng = dsk_rand_new_xorshift1024 ();
  dsk_rand_seed (rng);

  for (i = 0; i < DSK_N_ELEMENTS(gen_table); i++)
    if (strcmp (cmdline_mode, gen_table[i].mode) == 0)
      {
        gen_table[i].func(n_elements, elements);
        return 0;
      }
  dsk_die("bad mode");
  return 1;
}
