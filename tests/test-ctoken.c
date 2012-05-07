#include "../dsk.h"
#include <stdio.h>

static dsk_boolean cmdline_verbose = DSK_FALSE;

#define assert_or_error(call)                                      \
{if (call) {}                                                      \
  else { dsk_die ("running %s failed (%s:%u): %s",                 \
                  #call, __FILE__, __LINE__,                       \
                  error ? error->message : "no error message"); }}

static void
test_trivial (void)
{
  DskCTokenScannerConfig config = DSK_CTOKEN_SCANNER_CONFIG_INIT;
  DskCToken *token;
  DskError *error = NULL;

  {
    const char str[] = "{ foo; }";
    token = dsk_ctoken_scan_str (str, str + sizeof (str) - 1, &config, &error);
    assert_or_error (token != NULL);
    dsk_assert (token->type == DSK_CTOKEN_TYPE_TOPLEVEL);
    dsk_assert (token->n_children == 1);
    dsk_assert (token->children[0].type == DSK_CTOKEN_TYPE_BRACE);
    dsk_assert (token->children[0].n_children == 2);
    dsk_assert (token->children[0].children[0].type == DSK_CTOKEN_TYPE_BAREWORD);
    dsk_assert (token->children[0].children[0].n_bytes == 3);
    dsk_assert (token->children[0].children[1].type == DSK_CTOKEN_TYPE_OPERATOR);
    dsk_assert (token->children[0].children[1].n_bytes == 1);
    dsk_ctoken_destroy (token);
  }
  {
    const char str[] = "{ \"foo;\\\\\" }";
    token = dsk_ctoken_scan_str (str, str + sizeof (str) - 1, &config, &error);
    assert_or_error (token != NULL);
    dsk_assert (token->type == DSK_CTOKEN_TYPE_TOPLEVEL);
    dsk_assert (token->n_children == 1);
    dsk_assert (token->children[0].type == DSK_CTOKEN_TYPE_BRACE);
    dsk_assert (token->children[0].n_children == 1);
    dsk_assert (token->children[0].children[0].type == DSK_CTOKEN_TYPE_DOUBLE_QUOTED_STRING);
    dsk_assert (token->children[0].children[0].n_bytes == 8);
    dsk_ctoken_destroy (token);
  }
  {
    const char str[] = "{ \"foo;\\n\" }";
    token = dsk_ctoken_scan_str (str, str + sizeof (str) - 1, &config, &error);
    assert_or_error (token != NULL);
    dsk_assert (token->type == DSK_CTOKEN_TYPE_TOPLEVEL);
    dsk_assert (token->n_children == 1);
    dsk_assert (token->children[0].type == DSK_CTOKEN_TYPE_BRACE);
    dsk_assert (token->children[0].n_children == 1);
    dsk_assert (token->children[0].children[0].type == DSK_CTOKEN_TYPE_DOUBLE_QUOTED_STRING);
    dsk_assert (token->children[0].children[0].n_bytes == 8);
    dsk_ctoken_destroy (token);
  }
  {
    const char str[] = "{ \"foo;\\000\" }";
    token = dsk_ctoken_scan_str (str, str + sizeof (str) - 1, &config, &error);
    assert_or_error (token != NULL);
    dsk_assert (token->type == DSK_CTOKEN_TYPE_TOPLEVEL);
    dsk_assert (token->n_children == 1);
    dsk_assert (token->children[0].type == DSK_CTOKEN_TYPE_BRACE);
    dsk_assert (token->children[0].n_children == 1);
    dsk_assert (token->children[0].children[0].type == DSK_CTOKEN_TYPE_DOUBLE_QUOTED_STRING);
    dsk_assert (token->children[0].children[0].n_bytes == 10);
    dsk_ctoken_destroy (token);
  }
  {
    const char str[] = "a[b]";
    token = dsk_ctoken_scan_str (str, str + sizeof (str) - 1, &config, &error);
    assert_or_error (token != NULL);
    dsk_assert (token->type == DSK_CTOKEN_TYPE_TOPLEVEL);
    dsk_assert (token->n_children == 2);
    dsk_assert (token->children[0].type == DSK_CTOKEN_TYPE_BAREWORD);
    dsk_assert (token->children[0].start_byte == 0);
    dsk_assert (token->children[0].n_bytes == 1);
    dsk_assert (token->children[0].n_children == 0);
    dsk_assert (token->children[1].type == DSK_CTOKEN_TYPE_BRACKET);
    dsk_assert (token->children[1].start_byte == 1);
    dsk_assert (token->children[1].n_bytes == 3);
    dsk_assert (token->children[1].n_children == 1);
    dsk_assert (token->children[1].children[0].type == DSK_CTOKEN_TYPE_BAREWORD);
    dsk_assert (token->children[1].children[0].start_byte == 2);
    dsk_assert (token->children[1].children[0].n_bytes == 1);
    dsk_assert (token->children[1].children[0].n_children == 0);
    dsk_ctoken_destroy (token);
  }
}

static struct 
{
  const char *name;
  void (*test)(void);
} tests[] =
{
  { "trivial test", test_trivial },
};

int main(int argc, char **argv)
{
  unsigned i;

  dsk_cmdline_init ("test Dsk CToken parsing",
                    "Test Dsk CToken, a scanner for C-like languages",
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
