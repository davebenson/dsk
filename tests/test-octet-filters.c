#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "../dsk.h"

#define MAX_FILTERS     128

static dsk_boolean cmdline_verbose = DSK_FALSE;
static const char *cmdline_filename = "tests/test-octet-filters.txt";

static void
dequote (const char *c_quoted, unsigned line,
         unsigned *len_out, uint8_t **data_out)
{
  DskBuffer buffer = DSK_BUFFER_INIT;
  DskOctetFilter *filter = dsk_c_unquoter_new (DSK_FALSE);
  if (!dsk_octet_filter_process (filter, &buffer, strlen (c_quoted),
                                 (uint8_t*) c_quoted, NULL)
   || !dsk_octet_filter_finish (filter, &buffer, NULL))
    dsk_die ("error dequoting at line %u", line);
  dsk_object_unref (filter);
  *len_out = buffer.size;
  *data_out = dsk_malloc (buffer.size+1);
  dsk_buffer_read (&buffer, *len_out, *data_out);
  (*data_out)[*len_out] = 0;
}
static DskOctetFilter *
create_filter (const char *code)
{
  DskOctetFilter *filters[MAX_FILTERS];
  unsigned n_filters = 0;
  while (*code)
    {
      dsk_assert (n_filters < MAX_FILTERS);
      switch (*code)
	{
	case 'u': filters[n_filters++] = dsk_url_encoder_new (); break;
	case 'U': filters[n_filters++] = dsk_url_decoder_new (); break;
	case 'c': filters[n_filters++] = dsk_c_quoter_new (DSK_FALSE, DSK_FALSE); break;
	case 'C': filters[n_filters++] = dsk_c_unquoter_new (DSK_FALSE); break;
	case 'h': filters[n_filters++] = dsk_hex_encoder_new (DSK_FALSE, DSK_FALSE); break;
	case 'H': filters[n_filters++] = dsk_hex_decoder_new (); break;
	case 'b': filters[n_filters++] = dsk_base64_encoder_new (DSK_FALSE); break;
	case 'B': filters[n_filters++] = dsk_base64_decoder_new (); break;
	case 'q': filters[n_filters++] = dsk_quote_printable_new (); break;
	case 'Q': filters[n_filters++] = dsk_unquote_printable_new (); break;
	case 'x': filters[n_filters++] = dsk_xml_escaper_new (); break;
	case 'w': filters[n_filters++] = dsk_whitespace_trimmer_new (); break;
	default: dsk_die ("unexpected char %c in filter creation string",
	                  *code);
	}
      code++;
    }
  return dsk_octet_filter_chain_new_take (n_filters, filters);
}

int main(int argc, char **argv)
{
  char buf[8192];
  int lineno = 0;
  FILE *fp;

  dsk_cmdline_init ("test octet filters",
                    "Test the various Octet Filters",
                    NULL, 0);
  dsk_cmdline_add_boolean ("verbose", "extra logging", NULL, 0,
                           &cmdline_verbose);
  dsk_cmdline_add_string ("filename", "filename", "FILENAME", 0,
                           (char**) &cmdline_filename);
  dsk_cmdline_process_args (&argc, &argv);

  fp = fopen (cmdline_filename, "r");
  if (fp == NULL)
    dsk_die ("error opening %s: %s", cmdline_filename, strerror (errno));
  while (fgets (buf, sizeof (buf), fp) != NULL)
    {
      const char *at = buf;
      char *nl = strchr (buf, '\n');
      unsigned test_lineno = ++lineno;
      unsigned input_len;
      uint8_t *input_data;
      unsigned filter_len;
      uint8_t *filter_data;
      unsigned output_len;
      uint8_t *output_data;
      DskOctetFilter *filter;
      DskBuffer output_buffer;
      unsigned i;
      DskError *error = NULL;
      uint8_t *tmp;

      while (dsk_ascii_isspace (*at))
	at++;
      if (*at == 0 || *at == '#')
        continue;
      if (nl)
        *nl = 0;
      if (strncmp (at, "input=", 6) != 0)
        dsk_die ("expected 'input='");
      dequote (at+6, lineno, &input_len, &input_data);

      ++lineno;
      if (fgets (buf, sizeof (buf), fp) == NULL
        || strncmp (buf, "filter=", 7) != 0)
        dsk_die ("expected 'filter=' after 'input=' [%s:%u]",
                 cmdline_filename, lineno);
      nl = strchr (buf, '\n');
      if (nl)
        *nl = 0;
      dequote (at+7, lineno, &filter_len, &filter_data);

      ++lineno;
      if (fgets (buf, sizeof (buf), fp) == NULL
        || strncmp (buf, "output=", 7) != 0)
        dsk_die ("expected 'output=' after 'input=' [%s:%u]",
                 cmdline_filename, lineno);
      nl = strchr (buf, '\n');
      if (nl)
        *nl = 0;
      dequote (at+7, lineno, &output_len, &output_data);

      /* feed data in byte-by-byte */
      filter = create_filter ((char*) filter_data);
      dsk_buffer_init (&output_buffer);
      for (i = 0; i < input_len; i++)
        if (!dsk_octet_filter_process (filter, &output_buffer,
	                               1, input_data+i, &error))
          dsk_die ("error running filter: %s", error->message);
      if (!dsk_octet_filter_finish (filter, &output_buffer, &error))
	dsk_die ("error running filter: %s", error->message);
      if (output_buffer.size != output_len)
        dsk_die ("output of filter was %u bytes long, expected %u [%s:%u]",
	         (unsigned)output_buffer.size,
		 output_len,
		 cmdline_filename, test_lineno);
      tmp = dsk_malloc (output_len);
      dsk_buffer_read (&output_buffer, output_len, tmp);
      if (memcmp (output_data, tmp, output_len) != 0)
        dsk_die ("data mismatch [%s:%u]", cmdline_filename, test_lineno);
      dsk_free (tmp);
      dsk_object_unref (filter);

      /* feed data in one-shot */
      filter = create_filter ((char*) filter_data);
      dsk_buffer_init (&output_buffer);
      if (!dsk_octet_filter_process (filter, &output_buffer,
	                             input_len, input_data, &error))
          dsk_die ("error running filter: %s", error->message);
      if (!dsk_octet_filter_finish (filter, &output_buffer, &error))
	dsk_die ("error running filter: %s", error->message);
      if (output_buffer.size != output_len)
        dsk_die ("output of filter was %u bytes long, expected %u [%s:%u]",
	         (unsigned)output_buffer.size,
		 output_len,
		 cmdline_filename, test_lineno);
      tmp = dsk_malloc (output_len);
      dsk_buffer_read (&output_buffer, output_len, tmp);
      if (memcmp (output_data, tmp, output_len) != 0)
        dsk_die ("data mismatch [%s:%u]",
		 cmdline_filename, test_lineno);
      dsk_free (tmp);
      dsk_object_unref (filter);


      dsk_free (output_data);
      dsk_free (input_data);
      dsk_free (filter_data);
    }
  fclose (fp);
  dsk_cleanup ();
  return 0;
}
