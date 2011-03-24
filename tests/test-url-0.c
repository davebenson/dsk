#include <string.h>
#include <stdio.h>
#include "../dsk.h"

static dsk_boolean cmdline_verbose = DSK_FALSE;

#define SCAN(url, scan)                                                \
  do{                                                                  \
    DskError *error = NULL;                                            \
    if (cmdline_verbose)                                               \
      dsk_warning ("scanning '%s' at %s:%u", url, __FILE__, __LINE__); \
    if (!dsk_url_scan (url, scan, &error))                             \
      dsk_die ("error scanning url %s at %s:%u: %s",                   \
               url, __FILE__, __LINE__, error->message);               \
  }while(0)

static void
test_simple_http (void)
{
  DskUrlScanned scan;
  const char *url;

  url = "http://foo.com/bar";
  SCAN (url, &scan);
  dsk_assert (scan.scheme == DSK_URL_SCHEME_HTTP);
  dsk_assert (scan.scheme_start == url);
  dsk_assert (scan.scheme_end == url + 4);
  dsk_assert (scan.username_start == NULL);
  dsk_assert (scan.username_end == NULL);
  dsk_assert (scan.password_start == NULL);
  dsk_assert (scan.password_end == NULL);
  dsk_assert (scan.host_start == url + 7);
  dsk_assert (scan.host_end == url + 14);
  dsk_assert (scan.port_start == NULL);
  dsk_assert (scan.port_end == NULL);
  dsk_assert (scan.port == 0);
  dsk_assert (scan.path_start == scan.host_end);
  dsk_assert (scan.path_end == url + 18);
  dsk_assert (scan.query_start == NULL);
  dsk_assert (scan.query_end == NULL);
  dsk_assert (scan.fragment_start == NULL);
  dsk_assert (scan.fragment_end == NULL);
}

static void
test_simple_ftp (void)
{
  DskUrlScanned scan;
  const char *url;

  url = "ftp://foo.com/bar";
  SCAN (url, &scan);
  dsk_assert (scan.scheme == DSK_URL_SCHEME_FTP);
  dsk_assert (scan.scheme_start == url);
  dsk_assert (scan.scheme_end == url + 3);
  dsk_assert (scan.username_start == NULL);
  dsk_assert (scan.username_end == NULL);
  dsk_assert (scan.password_start == NULL);
  dsk_assert (scan.password_end == NULL);
  dsk_assert (scan.host_start == url + 6);
  dsk_assert (scan.host_end == url + 13);
  dsk_assert (scan.port_start == NULL);
  dsk_assert (scan.port_end == NULL);
  dsk_assert (scan.port == 0);
  dsk_assert (scan.path_start == scan.host_end);
  dsk_assert (scan.path_end == url + 17);
  dsk_assert (scan.query_start == NULL);
  dsk_assert (scan.query_end == NULL);
  dsk_assert (scan.fragment_start == NULL);
  dsk_assert (scan.fragment_end == NULL);
}

static void
test_http_port (void)
{
  DskUrlScanned scan;
  const char *url;

  url = "http://foo.com:666/bar";
  SCAN (url, &scan);
  dsk_assert (scan.scheme == DSK_URL_SCHEME_HTTP);
  dsk_assert (scan.scheme_start == url);
  dsk_assert (scan.scheme_end == url + 4);
  dsk_assert (scan.username_start == NULL);
  dsk_assert (scan.username_end == NULL);
  dsk_assert (scan.password_start == NULL);
  dsk_assert (scan.password_end == NULL);
  dsk_assert (scan.host_start == url + 7);
  dsk_assert (scan.host_end == url + 14);
  dsk_assert (scan.port_start == url + 15);
  dsk_assert (scan.port_end == url + 18);
  dsk_assert (scan.port == 666);
  dsk_assert (scan.path_start == scan.port_end);
  dsk_assert (scan.path_end == url + 22);
  dsk_assert (scan.query_start == NULL);
  dsk_assert (scan.query_end == NULL);
  dsk_assert (scan.fragment_start == NULL);
  dsk_assert (scan.fragment_end == NULL);
}

static void
test_http_relative (void)
{
  DskUrlScanned scan;
  const char *url;

  url = "http:bar";
  SCAN (url, &scan);
  dsk_assert (scan.scheme == DSK_URL_SCHEME_HTTP);
  dsk_assert (scan.scheme_start == url);
  dsk_assert (scan.scheme_end == url + 4);
  dsk_assert (scan.username_start == NULL);
  dsk_assert (scan.username_end == NULL);
  dsk_assert (scan.password_start == NULL);
  dsk_assert (scan.password_end == NULL);
  dsk_assert (scan.host_start == NULL);
  dsk_assert (scan.host_end == NULL);
  dsk_assert (scan.port_start == NULL);
  dsk_assert (scan.port_end == NULL);
  dsk_assert (scan.port == 0);
  dsk_assert (scan.path_start == url + 5);
  dsk_assert (scan.path_end == url + 8);
  dsk_assert (scan.query_start == NULL);
  dsk_assert (scan.query_end == NULL);
  dsk_assert (scan.fragment_start == NULL);
  dsk_assert (scan.fragment_end == NULL);
}

static void
test_http_absolute (void)
{
  DskUrlScanned scan;
  const char *url;

  url = "http:/bar";
  SCAN (url, &scan);
  dsk_assert (scan.scheme == DSK_URL_SCHEME_HTTP);
  dsk_assert (scan.scheme_start == url);
  dsk_assert (scan.scheme_end == url + 4);
  dsk_assert (scan.username_start == NULL);
  dsk_assert (scan.username_end == NULL);
  dsk_assert (scan.password_start == NULL);
  dsk_assert (scan.password_end == NULL);
  dsk_assert (scan.host_start == NULL);
  dsk_assert (scan.host_end == NULL);
  dsk_assert (scan.port_start == NULL);
  dsk_assert (scan.port_end == NULL);
  dsk_assert (scan.port == 0);
  dsk_assert (scan.path_start == url + 5);
  dsk_assert (scan.path_end == url + 9);
  dsk_assert (scan.query_start == NULL);
  dsk_assert (scan.query_end == NULL);
  dsk_assert (scan.fragment_start == NULL);
  dsk_assert (scan.fragment_end == NULL);
}
static void
test_userpass (void)
{
  DskUrlScanned scan;
  const char *url;

  url = "http://user:pass@foo.com/bar";
  SCAN (url, &scan);
  dsk_assert (scan.scheme == DSK_URL_SCHEME_HTTP);
  dsk_assert (scan.scheme_start == url);
  dsk_assert (scan.scheme_end == url + 4);
  dsk_assert (scan.username_start == url + 7);
  dsk_assert (scan.username_end == url + 11);
  dsk_assert (scan.password_start == url + 12);
  dsk_assert (scan.password_end == url + 16);
  dsk_assert (scan.host_start == url + 17);
  dsk_assert (scan.host_end == url + 24);
  dsk_assert (scan.port_start == NULL);
  dsk_assert (scan.port_end == NULL);
  dsk_assert (scan.port == 0);
  dsk_assert (scan.path_start == scan.host_end);
  dsk_assert (scan.path_end == url + 28);
  dsk_assert (scan.query_start == NULL);
  dsk_assert (scan.query_end == NULL);
  dsk_assert (scan.fragment_start == NULL);
  dsk_assert (scan.fragment_end == NULL);
}

static void
test_user (void)
{
  DskUrlScanned scan;
  const char *url;

  url = "http://user@foo.com/bar";
  SCAN (url, &scan);
  dsk_assert (scan.scheme == DSK_URL_SCHEME_HTTP);
  dsk_assert (scan.scheme_start == url);
  dsk_assert (scan.scheme_end == url + 4);
  dsk_assert (scan.username_start == url + 7);
  dsk_assert (scan.username_end == url + 11);
  dsk_assert (scan.password_start == NULL);
  dsk_assert (scan.password_end == NULL);
  dsk_assert (scan.host_start == url + 12);
  dsk_assert (scan.host_end == url + 19);
  dsk_assert (scan.port_start == NULL);
  dsk_assert (scan.port_end == NULL);
  dsk_assert (scan.port == 0);
  dsk_assert (scan.path_start == scan.host_end);
  dsk_assert (scan.path_end == url + 23);
  dsk_assert (scan.query_start == NULL);
  dsk_assert (scan.query_end == NULL);
  dsk_assert (scan.fragment_start == NULL);
  dsk_assert (scan.fragment_end == NULL);
}

static void
test_query (void)
{
  DskUrlScanned scan;
  const char *url;

  url = "http://foo.com/bar?a=b";
  SCAN (url, &scan);
  dsk_assert (scan.scheme == DSK_URL_SCHEME_HTTP);
  dsk_assert (scan.scheme_start == url);
  dsk_assert (scan.scheme_end == url + 4);
  dsk_assert (scan.username_start == NULL);
  dsk_assert (scan.username_end == NULL);
  dsk_assert (scan.password_start == NULL);
  dsk_assert (scan.password_end == NULL);
  dsk_assert (scan.host_start == url + 7);
  dsk_assert (scan.host_end == url + 14);
  dsk_assert (scan.port_start == NULL);
  dsk_assert (scan.port_end == NULL);
  dsk_assert (scan.port == 0);
  dsk_assert (scan.path_start == scan.host_end);
  dsk_assert (scan.path_end == url + 18);
  dsk_assert (scan.query_start == url + 19);
  dsk_assert (scan.query_end == url + 22);
  dsk_assert (scan.fragment_start == NULL);
  dsk_assert (scan.fragment_end == NULL);
}

static void
test_fragment (void)
{
  DskUrlScanned scan;
  const char *url;

  url = "http://foo.com/bar#abc";
  SCAN (url, &scan);
  dsk_assert (scan.scheme == DSK_URL_SCHEME_HTTP);
  dsk_assert (scan.scheme_start == url);
  dsk_assert (scan.scheme_end == url + 4);
  dsk_assert (scan.username_start == NULL);
  dsk_assert (scan.username_end == NULL);
  dsk_assert (scan.password_start == NULL);
  dsk_assert (scan.password_end == NULL);
  dsk_assert (scan.host_start == url + 7);
  dsk_assert (scan.host_end == url + 14);
  dsk_assert (scan.port_start == NULL);
  dsk_assert (scan.port_end == NULL);
  dsk_assert (scan.port == 0);
  dsk_assert (scan.path_start == scan.host_end);
  dsk_assert (scan.path_end == url + 18);
  dsk_assert (scan.query_start == NULL);
  dsk_assert (scan.query_end == NULL);
  dsk_assert (scan.fragment_start == url + 19);
  dsk_assert (scan.fragment_end == url + 22);
}

static void
test_query_and_frag (void)
{
  DskUrlScanned scan;
  const char *url;

  url = "http://foo.com/bar?a=b#abc";
  SCAN (url, &scan);
  dsk_assert (scan.scheme == DSK_URL_SCHEME_HTTP);
  dsk_assert (scan.scheme_start == url);
  dsk_assert (scan.scheme_end == url + 4);
  dsk_assert (scan.username_start == NULL);
  dsk_assert (scan.username_end == NULL);
  dsk_assert (scan.password_start == NULL);
  dsk_assert (scan.password_end == NULL);
  dsk_assert (scan.host_start == url + 7);
  dsk_assert (scan.host_end == url + 14);
  dsk_assert (scan.port_start == NULL);
  dsk_assert (scan.port_end == NULL);
  dsk_assert (scan.port == 0);
  dsk_assert (scan.path_start == scan.host_end);
  dsk_assert (scan.path_end == url + 18);
  dsk_assert (scan.query_start == url + 19);
  dsk_assert (scan.query_end == url + 22);
  dsk_assert (scan.fragment_start == url + 23);
  dsk_assert (scan.fragment_end == url + 26);
}


static struct 
{
  const char *name;
  void (*test)(void);
} tests[] =
{
  { "simple HTTP", test_simple_http },
  { "simple FTP", test_simple_ftp },
  { "HTTP with port", test_http_port },
  { "HTTP relative hostless", test_http_relative },
  { "HTTP absolute hostless", test_http_absolute },
  { "username and password", test_userpass },
  { "username", test_user },
  { "query", test_query },
  { "fragment", test_fragment },
  { "query and fragment", test_query_and_frag },
};
int main(int argc, char **argv)
{
  unsigned i;
  dsk_cmdline_init ("test URL parsing",
                    "Test URL scanning for a variety of URLs",
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
