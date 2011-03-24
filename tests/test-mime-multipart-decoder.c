#include <stdio.h>
#include <string.h>
#include "../dsk.h"

static dsk_boolean cmdline_verbose = DSK_FALSE;


  /* Example from RFC 2046, Section 5.1.1.  Page 21. */
static const uint8_t test_0__body[] =
     "This is the preamble.  It is to be ignored, though it\r\n"
     "is a handy place for composition agents to include an\r\n"
     "explanatory note to non-MIME conformant readers.\r\n"
     "\r\n"
     "--simple boundary\r\n"
     "\r\n"
     "This is implicitly typed plain US-ASCII text.\r\n"
     "It does NOT end with a linebreak.\r\n"
     "--simple boundary\r\n"
     "Content-type: text/plain; charset=us-ascii\r\n"
     "\r\n"
     "This is explicitly typed plain US-ASCII text.\r\n"
     "It DOES end with a linebreak.\r\n"
     "\r\n"
     "--simple boundary--\r\n"
     ;
  
static void
test_0__verify_results (unsigned n_cgi_vars,
                        DskCgiVariable *cgi_vars)
{
  const char *tmp_txt;
  dsk_assert (n_cgi_vars == 2);

  tmp_txt = "This is implicitly typed plain US-ASCII text.\r\n"
            "It does NOT end with a linebreak.";
  dsk_assert (cgi_vars[0].value_length == strlen (tmp_txt));
  dsk_assert (memcmp (tmp_txt, cgi_vars[0].value, strlen (tmp_txt)) == 0);

  tmp_txt = "This is explicitly typed plain US-ASCII text.\r\n"
            "It DOES end with a linebreak.\r\n";
  dsk_assert (cgi_vars[1].value_length == strlen (tmp_txt));
  dsk_assert (memcmp (tmp_txt, cgi_vars[1].value, strlen (tmp_txt)) == 0);
  dsk_assert (strcmp (cgi_vars[1].content_type, "text/plain/us-ascii") == 0);
}

static const uint8_t test_1__body[] =
  "--simple boundary\r\n"
  "Content-Disposition: form-data; name=\"keyname\"\r\n"
  "\r\n"
  "sadfasdf\r\n"
  "--simple boundary--\r\n"
  ;

static void
test_1__verify_results (unsigned n_cgi_vars,
                        DskCgiVariable *cgi_vars)
{
  dsk_assert (n_cgi_vars == 1);

  dsk_assert (!cgi_vars[0].is_get);
  dsk_assert (strcmp (cgi_vars[0].key, "keyname") == 0);
  dsk_assert (strcmp (cgi_vars[0].value, "sadfasdf") == 0);
  dsk_assert (cgi_vars[0].value_length == 8);
  dsk_assert (cgi_vars[0].content_type == NULL);
  dsk_assert (cgi_vars[0].content_location == NULL);
  dsk_assert (cgi_vars[0].content_description == NULL);
}


static void
run_test (const uint8_t *body,
          void (*test_func)(unsigned n_cgi_vars, DskCgiVariable *cgi_vars))
{
  unsigned iter;
  /* Run test twice:  first feed the data as one blob,
     then feed it byte-by-byte. */
  for (iter = 0; iter < 2; iter++)
    {
      unsigned i;
      DskError *error = NULL;
      const char *content_type_kv_pairs[] =
        {
          "boundary",
          "simple boundary",
          NULL
        };
      size_t n_cgi_vars;
      DskCgiVariable *cgi_vars;
      DskMimeMultipartDecoder *decoder = dsk_mime_multipart_decoder_new ((char**)content_type_kv_pairs, &error);
      if (decoder == NULL)
        dsk_die ("dsk_mime_multipart_decoder_new failed: %s", error->message);
      switch (iter)
        {
        case 0:
          {
          /* feed the data all at once */
            unsigned len = strlen ((char*)body);
            if (cmdline_verbose)
              fprintf(stderr, "[one-blob]");
            if (!dsk_mime_multipart_decoder_feed (decoder,
                                                  len, body,
                                                  &n_cgi_vars, &error))
              dsk_die ("error calling dsk_mime_multipart_decoder_feed(%u): %s",
                       len, error->message);
          }
          break;
        case 1:
          /* feed the data a byte at a time */
          {
            unsigned i;
            if (cmdline_verbose)
              fprintf(stderr, "[byte-by-byte]");
            for (i = 0; body[i] != '\0'; i++)
              if (!dsk_mime_multipart_decoder_feed (decoder, 1, (const uint8_t*) body + i,
                                                    &n_cgi_vars, &error))
                dsk_die ("error calling dsk_mime_multipart_decoder_feed(1): %s",
                         error->message);
          }
          break;
        }
      if (!dsk_mime_multipart_decoder_done (decoder, &n_cgi_vars, &error))
        dsk_die ("error calling dsk_mime_multipart_decoder_done: %s", error->message);
      cgi_vars = dsk_malloc (sizeof (DskCgiVariable) * n_cgi_vars);
      dsk_mime_multipart_decoder_dequeue_all (decoder, cgi_vars);

      test_func (n_cgi_vars, cgi_vars);

      for (i = 0; i < n_cgi_vars; i++)
        dsk_cgi_variable_clear (cgi_vars + i);
      dsk_free (cgi_vars);
      dsk_object_unref (decoder);
    }
}

static struct 
{
  const char *name;
  const uint8_t *content;
  void (*test)(unsigned n_cgi_var, DskCgiVariable *cgi_vars);
} tests[] =
{
  { "simple example decoding", test_0__body, test_0__verify_results },
  { "content-disposition-name", test_1__body, test_1__verify_results },
};

int main(int argc, char **argv)
{
  unsigned i;

  dsk_cmdline_init ("test mime multipart decoder",
                    "Test the MIME Multipart Decoder",
                    NULL, 0);
  dsk_cmdline_add_boolean ("verbose", "extra logging", NULL, 0,
                           &cmdline_verbose);
  dsk_cmdline_process_args (&argc, &argv);

  for (i = 0; i < DSK_N_ELEMENTS (tests); i++)
    {
      fprintf (stderr, "Test: %s... ", tests[i].name);
      run_test (tests[i].content, tests[i].test);
      fprintf (stderr, " done.\n");
    }
  dsk_cleanup ();
  return 0;
}
