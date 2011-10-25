#include <stdio.h>
#define DSK_INCLUDE_TS0
#include "../dsk.h"

static dsk_boolean cmdline_verbose = DSK_FALSE;

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

static void
test_trivial_stanza (void)
{
  DskTs0Stanza *stanza;
  {
    DEFINE_DATA("hi mom", hi_mom);
    PARSE_STANZA_FROM_DATA(stanza, hi_mom);
    dsk_ts0_stanza_free (stanza);
  }
}

static struct 
{
  const char *name;
  void (*test)(void);
} tests[] =
{
  { "trivial stanzas", test_trivial_stanza },
};

int main(int argc, char **argv)
{
  unsigned i;

  dsk_cmdline_init ("test TS0",
                    "Test the first template system",
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
