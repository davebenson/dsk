#include <stdio.h>
#include <string.h>
#include "../dsk.h"

static dsk_boolean cmdline_verbose = DSK_FALSE;

static void
test_cgi_get (void)
{
  size_t n_cgi_vars;
  DskCgiVariable *cgi_vars;
  unsigned i;
  DskError *error = NULL;

  if (!dsk_cgi_parse_query_string ("?q=foo+bar&p123=cheese%20cake",
                                   &n_cgi_vars, &cgi_vars,
                                   &error))
    dsk_die ("error parsing CGI query string");
  dsk_assert (n_cgi_vars == 2);
  dsk_assert (cgi_vars[0].is_get);
  dsk_assert (strcmp (cgi_vars[0].key, "q") == 0);
  dsk_assert (strcmp (cgi_vars[0].value, "foo bar") == 0);
  dsk_assert (cgi_vars[0].value_length == 7);
  dsk_assert (cgi_vars[0].content_type == NULL);
  dsk_assert (cgi_vars[1].is_get);
  dsk_assert (strcmp (cgi_vars[1].key, "p123") == 0);
  dsk_assert (strcmp (cgi_vars[1].value, "cheese cake") == 0);
  dsk_assert (cgi_vars[1].value_length == 11);
  dsk_assert (cgi_vars[1].content_type == NULL);
  for (i = 0; i < n_cgi_vars; i++)
    dsk_cgi_variable_clear (cgi_vars + i);
  dsk_free (cgi_vars);
}

static void
test_mime_multipart (void)
{
  dsk_warning ("TODO");
}

static void
test_url_encoded_post (void)
{
  static const uint8_t data[] = "name=Xavier+Xantico&verdict=Yes&colour=Blue&happy=sad&Utfr=Send";
  size_t n_cgi_vars;
  DskCgiVariable *cgi_vars;
  DskError *error = NULL;
  unsigned i;
  if (!dsk_cgi_parse_post_data ("application/x-url-encoded", NULL,
                                strlen((char*)data), data,
                                &n_cgi_vars, &cgi_vars,
                                &error))
    dsk_die ("error parsing URL-encoded POST data: %s", error->message);
  dsk_assert (n_cgi_vars == 5);
  dsk_assert (!cgi_vars[0].is_get);
  dsk_assert (strcmp (cgi_vars[0].key, "name") == 0);
  dsk_assert (strcmp (cgi_vars[0].value, "Xavier Xantico") == 0);
  dsk_assert (cgi_vars[0].content_type == NULL);
  dsk_assert (!cgi_vars[1].is_get);
  dsk_assert (strcmp (cgi_vars[1].key, "verdict") == 0);
  dsk_assert (strcmp (cgi_vars[1].value, "Yes") == 0);
  dsk_assert (cgi_vars[1].value_length == 3);
  dsk_assert (cgi_vars[1].content_type == NULL);
  dsk_assert (!cgi_vars[2].is_get);
  dsk_assert (strcmp (cgi_vars[2].key, "colour") == 0);
  dsk_assert (strcmp (cgi_vars[2].value, "Blue") == 0);
  dsk_assert (cgi_vars[2].value_length == 4);
  dsk_assert (cgi_vars[2].content_type == NULL);
  dsk_assert (!cgi_vars[3].is_get);
  dsk_assert (strcmp (cgi_vars[3].key, "happy") == 0);
  dsk_assert (strcmp (cgi_vars[3].value, "sad") == 0);
  dsk_assert (cgi_vars[3].value_length == 3);
  dsk_assert (cgi_vars[3].content_type == NULL);
  dsk_assert (!cgi_vars[4].is_get);
  dsk_assert (strcmp (cgi_vars[4].key, "Utfr") == 0);
  dsk_assert (strcmp (cgi_vars[4].value, "Send") == 0);
  dsk_assert (cgi_vars[4].value_length == 4);
  dsk_assert (cgi_vars[4].content_type == NULL);
  for (i = 0; i < n_cgi_vars; i++)
    dsk_cgi_variable_clear (cgi_vars + i);
  dsk_free (cgi_vars);
}

static struct 
{
  const char *name;
  void (*test)(void);
} tests[] =
{
  { "simple GET params", test_cgi_get },
  { "MIME Multipart", test_mime_multipart },
  { "URL Encoded POST", test_url_encoded_post },
};

int main(int argc, char **argv)
{
  unsigned i;

  dsk_cmdline_init ("test CGI parsing",
                    "Test the CGI Parsing Code",
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
