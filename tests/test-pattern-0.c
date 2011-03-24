#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../dsk.h"

#define MK_PTR(integer)     ((void*)(size_t)(integer))
#define GET_INT(pointer)     ((unsigned)(size_t)(pointer))

static dsk_boolean verbose = DSK_FALSE;

static void
test_0 (void)
{
  DskPatternEntry entry = { "(a|b)c", MK_PTR(1) };
  DskError *error = NULL;
  DskPattern *pattern = dsk_pattern_compile (1, &entry, &error);
  dsk_assert (dsk_pattern_match (pattern, "ac"));
  dsk_assert (dsk_pattern_match (pattern, "bc"));
  dsk_assert (!dsk_pattern_match (pattern, "b"));
  dsk_assert (!dsk_pattern_match (pattern, "ab"));
  dsk_assert (!dsk_pattern_match (pattern, "acc"));
  dsk_pattern_free (pattern);
}

static void chomp (char *buf)
{
  char *nl = strchr (buf, '\n');
  if (nl) *nl = 0;
}

static void test_from_file (const char *filename)
{
  FILE *fp = fopen (filename, "r");
  char buf[8192];
  DskPatternEntry entries[100];
  unsigned line_no = 0;
  DskPattern *pattern = NULL;
  dsk_boolean in_test = DSK_FALSE;
  if (fp == NULL)
    dsk_die ("test_from_file: error opening %s", filename);
  while (fgets (buf, sizeof (buf), fp) != NULL)
    {
      line_no++;
      chomp (buf);
      if (verbose)
        printf ("%s:%u: %s\n", filename, line_no, buf);
restart:
      if (strncmp (buf, "PATTERNS:", 9) == 0)
        {
          unsigned n_entries = 0;
          unsigned pat_line = line_no;
          fprintf (stderr, "p");
          while (fgets (buf, sizeof (buf), fp) != NULL)
            {
              line_no++;
              if (strncmp (buf, "  ", 2) != 0)
                {
                  /* make pattern */
                  DskError *error = NULL;
                  unsigned i;
                  if (pattern)
                    dsk_pattern_free (pattern);
                  pattern = dsk_pattern_compile (n_entries, entries, &error);
                  if (pattern == NULL)
                    {
                      dsk_die ("error compiling pattern %s:%u: %s",
                               filename, pat_line, error->message);
                    }
                  for (i = 0; i < n_entries; i++)
                    dsk_free ((char*) entries[i].pattern);
                  goto restart;
                }
              chomp (buf);

              if (n_entries == DSK_N_ELEMENTS (entries))
                dsk_die ("too many patterns in test");
              entries[n_entries].pattern = dsk_strdup (buf + 2);
              entries[n_entries].result = MK_PTR (n_entries+1);
              n_entries++;
            }
          dsk_die ("pattern must not be last thing in file (%s:%u)",
                   filename, pat_line);
        }

      if (strncmp (buf, "NO MATCH:", 9) == 0)
        {
          chomp (buf);
          if (dsk_pattern_match (pattern, buf+9))
            dsk_die ("test at %s:%u: no match expected but got match (test='%s')",
                     filename, line_no, buf+9);
          fprintf (stderr, "x");
        }
      else if (strncmp (buf, "MATCH ", 6) == 0)
        {
          char *end;
          unsigned num = strtoul (buf+6, &end, 10);
          unsigned rv;
          if (*end != ':')
            dsk_die ("expected ':' after match number %s:%u",
                     filename, line_no);
          end++;
          chomp (end);
          rv = GET_INT (dsk_pattern_match (pattern, end));
          if (rv == 0)
            dsk_die ("no match at %s:%u: test='%s'", 
                     filename, line_no, end);
          else if (rv - 1 != num)
            dsk_die ("expected match of entry %u, got entry %u (%s:%u) test='%s'",
                     num, rv-1, filename, line_no, end);
          fprintf (stderr, "o");
        }
      else if (buf[0] == '\n' || buf[0] == 0 || buf[0] == '#')
        {
        }
      else if (strncmp (buf, "BANNER:", 7) == 0)
        {
          char *banner = buf + 7;
          chomp (banner);
          while (dsk_ascii_isspace (*banner))
            banner++;
          if (in_test)
            fprintf (stderr, " done.\n");
          fprintf (stderr, "Test: %s... ", banner);
          in_test = DSK_TRUE;
        }
      else
        {
          chomp (buf);
          dsk_die ("unexpected line at %s:%u (%s)", filename, line_no, buf);
        }
    }
  if (in_test)
    fprintf (stderr, " done.\n");
  fclose (fp);
  dsk_pattern_free (pattern);
}


static struct 
{
  const char *name;
  void (*test)(void);
} tests[] =
{
  { "simple test", test_0 },
};
int main(int argc, char **argv)
{
  unsigned i;
  dsk_cmdline_init ("test pattern handling",
                    "Test various DSK pattern matching",
                    NULL, 0);
  dsk_cmdline_add_boolean ("verbose", "extra logging", NULL, 0,
                           &verbose);
  dsk_cmdline_process_args (&argc, &argv);

  for (i = 0; i < DSK_N_ELEMENTS (tests); i++)
    {
      fprintf (stderr, "Test: %s... ", tests[i].name);
      tests[i].test ();
      fprintf (stderr, " done.\n");
    }
  test_from_file ("tests/pattern-test.input");
  dsk_cleanup ();
  return 0;
}
