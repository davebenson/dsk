#include "../dsk.h"
#include <string.h>
#include <stdio.h>

static dsk_boolean cmdline_verbose = DSK_FALSE;

static DskHttpRequest *
try_parse_request_from_string (const char *str,
                               DskError  **error)
{
  unsigned len = strlen (str);
  DskBuffer buffer = DSK_BUFFER_INIT;
  DskHttpRequest *rv;
  dsk_buffer_append (&buffer, len, str);
  rv = dsk_http_request_parse_buffer (&buffer, len, error);
  dsk_buffer_clear (&buffer);
  return rv;
}

static DskHttpRequest *
parse_request_from_string (const char *str)
{
  DskError *error = NULL;
  DskHttpRequest *rv = try_parse_request_from_string (str, &error);
  if (rv == NULL)
    dsk_die ("error parsing request from string: %s", error->message);
  return rv;
}

static void
test_parse_request_0 (void)
{
  DskHttpRequest *r;

  r = parse_request_from_string ("GET /f HTTP/1.0\r\n"
                                 "Host: example.com\r\n");
  dsk_assert (r->verb == DSK_HTTP_VERB_GET);
  dsk_assert (strcmp (r->path, "/f") == 0);
  dsk_assert (strcmp (r->host, "example.com") == 0);
  dsk_assert (strcmp (dsk_http_request_get (r, "host"), "example.com") == 0);
  dsk_assert (strcmp (dsk_http_request_get (r, "path"), "/f") == 0);
  dsk_object_unref (r);

}

static struct 
{
  const char *name;
  void (*test)(void);
} tests[] =
{
  { "simple request parsing", test_parse_request_0 },
};

int main(int argc, char **argv)
{
  unsigned i;

  dsk_cmdline_init ("test HTTP-Header parsing",
                    "Test the HTTP-Header Parsing Code",
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
