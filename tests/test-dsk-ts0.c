#include <unistd.h>
#include <stdio.h>
#include <string.h>
#define DSK_INCLUDE_TS0
#include "../dsk.h"

static dsk_boolean cmdline_verbose = DSK_FALSE;
static dsk_boolean cmdline_print_expected_errors = DSK_FALSE;

#define DEFINE_DATA(contents, name) \
  static char name##_data[] = (contents); \
  /*static unsigned name##_length = sizeof(name##_data);*/ \
  static unsigned name##_lineno = __LINE__

#define PARSE_STANZA_FROM_DATA(var_name, name) \
  do{ \
    DskError *e_e = NULL; \
    var_name = dsk_ts0_stanza_parse_str (name##_data, \
                                         __FILE__, \
                                         name##_lineno, \
                                         &e_e); \
    if (var_name == NULL) \
      dsk_die ("error parsing stanza on line %u: %s", __LINE__, e_e->message); \
  }while(0)


#define EVALUATE_STANZA(result, init_ns_code, stanza) \
  do{ \
    DskError *e_e = NULL; \
    DskBuffer buffer = DSK_BUFFER_INIT; \
    DskTs0Namespace *ns = dsk_ts0_namespace_new (dsk_ts0_namespace_global ()); \
    init_ns_code; \
    if (!dsk_ts0_stanza_evaluate (ns, stanza, &buffer, &e_e)) \
      dsk_die ("error evaluating stanza %s:%u:  %s", \
               __FILE__, __LINE__, e_e->message); \
    result = dsk_buffer_empty_to_string (&buffer); \
    dsk_ts0_namespace_unref (ns); \
  }while(0)
#define EVALUATE_STANZA_FAIL(init_ns_code, stanza) \
  do{ \
    DskError *e_e = NULL; \
    DskBuffer buffer = DSK_BUFFER_INIT; \
    DskTs0Namespace *ns = dsk_ts0_namespace_new (dsk_ts0_namespace_global ()); \
    init_ns_code; \
    if (!dsk_ts0_stanza_evaluate (ns, stanza, &buffer, &e_e)) \
      { \
        if (cmdline_print_expected_errors) \
          fprintf (stderr, "error evaluating stanza %s:%u:  %s", \
                   __FILE__, __LINE__, e_e->message); \
      } \
    else \
      { \
        dsk_die ("expected error at %s:%u: got '%s'", \
                 __FILE__, __LINE__, dsk_buffer_empty_to_string (&buffer)); \
      } \
    dsk_buffer_clear (&buffer); \
    dsk_ts0_namespace_unref (ns); \
  }while(0)
#define DOT()  fprintf(stderr, ".")

void dump_stanza (DskTs0Stanza *stanza)
{
  DskBuffer buffer = DSK_BUFFER_INIT;
  DskError *error = NULL;
  dsk_ts0_stanza_dump (stanza, 2, &buffer);
  if (!dsk_buffer_write_all_to_fd (&buffer, STDERR_FILENO, &error))
    dsk_die ("dsk_buffer_write_all_to_fd: %s", error->message);
}

static void
test_trivial_stanza (void)
{
  DskTs0Stanza *stanza;
  DOT();
  {
    DEFINE_DATA("hi mom", hi_mom);
    PARSE_STANZA_FROM_DATA(stanza, hi_mom);
    char *result;
    EVALUATE_STANZA (result, , stanza);
    dsk_assert (strcmp (result, "hi mom") == 0);
    dsk_ts0_stanza_free (stanza);
    dsk_free (result);
  }
  DOT();
  {
    DEFINE_DATA("i own $$4", hi_mom);
    PARSE_STANZA_FROM_DATA(stanza, hi_mom);
    char *result;
    EVALUATE_STANZA (result, , stanza);
    dsk_assert (strcmp (result, "i own $4") == 0);
    dsk_ts0_stanza_free (stanza);
    dsk_free (result);
  }
}
static void
test_variable_interp_0 (void)
{
  DskTs0Stanza *stanza;
  DOT();
  {
    DEFINE_DATA("hi $foo", hi_mom);
    PARSE_STANZA_FROM_DATA(stanza, hi_mom);
    char *result;
    //dump_stanza (stanza);
    EVALUATE_STANZA (result, dsk_ts0_namespace_set_variable (ns, "foo", "bar"), stanza);
    dsk_assert (strcmp (result, "hi bar") == 0);
    dsk_ts0_stanza_free (stanza);
    dsk_free (result);
  }
  DOT();
  {
    DEFINE_DATA("i own $undefined", hi_mom);
    PARSE_STANZA_FROM_DATA(stanza, hi_mom);
    EVALUATE_STANZA_FAIL (   , stanza);
    dsk_ts0_stanza_free (stanza);
  }
}

/* TODO: test_expression_interp_0 */
/* TODO: test_set_nodollar */
/* TODO: test_tag_if */
/* TODO: test_tag_catch */

static struct 
{
  const char *name;
  void (*test)(void);
} tests[] =
{
  { "trivial stanzas", test_trivial_stanza },
  { "trivial variable interpolation", test_variable_interp_0 },
};

int main(int argc, char **argv)
{
  unsigned i;

  dsk_cmdline_init ("test TS0",
                    "Test the first template system",
                    NULL, 0);
  dsk_cmdline_add_boolean ("verbose", "extra logging", NULL, 0,
                           &cmdline_verbose);
  dsk_cmdline_add_boolean ("print-errors", "print out expected error messages", NULL, 0,
                           &cmdline_print_expected_errors);
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
