#include "../dsk.h"
#include "generated/wikiformats.c"
#include <string.h>
#include <stdio.h>



unsigned doc_index = 0;

static void
process_page (Wikiformats__Page *page)
{
  printf ("%s\n", page->title);
}

int main(int argc, char **argv)
{
  DskOctetFilter *decompressor = NULL;
  char *input_filename = NULL;
  FILE *fp;
  const char *extension;
  DskXmlParserConfig *config;
  DskXmlParser *parser;

  dsk_cmdline_add_string ("input", "Input Filename of Wikipedia Dump",
                          "FILE", DSK_CMDLINE_MANDATORY, &input_filename);

  dsk_cmdline_process_args (&argc, &argv);

  if (strcmp (input_filename, "-") == 0)
    fp = stdin;
  else
    fp = fopen (input_filename, "rb");
  if (fp == NULL)
    dsk_die ("error opening file %s: %s", input_filename, strerror (errno));

  extension = strrchr (input_filename, '.');
  if (extension != NULL)
    {
      if (strcmp (extension, ".gz") == 0)
        decompressor = dsk_zlib_decompressor_new (DSK_ZLIB_GZIP);
      else if (strcmp (extension, ".bz2") == 0)
        decompressor = dsk_bz2lib_decompressor_new ();
    }
  
  if (decompressor == NULL)
    decompressor = dsk_octet_filter_identity_new ();

  config = dsk_xml_parser_config_new_simple (0, "mediawiki/page");
  parser = dsk_xml_parser_new (config, input_filename);

  size_t nread;
  uint8_t buf[4096];
  DskBuffer buffer = DSK_BUFFER_INIT;
  while ((nread=fread (buf, 1, sizeof (buf), fp)) != 0)
    {
      DskError *error = NULL;
      if (!dsk_octet_filter_process (decompressor, &buffer, nread, buf, &error))
        {
          dsk_error ("error decompressing data: %s", error->message);
        }
      while (buffer.size > 0)
        {
          DskBufferFragment *frag = buffer.first_frag;
          dsk_warning ("feeding xml parser %.*s", (int)frag->buf_length, frag->buf + frag->buf_start);
          if (!dsk_xml_parser_feed (parser,
                                    frag->buf_length,
                                    frag->buf + frag->buf_start,
                                    &error))
            {
              dsk_error ("error parsing XML: %s", error->message);
            }
          dsk_buffer_discard (&buffer, frag->buf_length);
        }
      DskXml *xml;
      while ((xml=dsk_xml_parser_pop (parser, NULL)) != NULL)
        {
          Wikiformats__Page page;
          if (!wikiformats__page__parse (xml, &page, &error))
            {
              dsk_error ("error parsing XML document %u: %s",
                         doc_index, error->message);
            }

          /* main workhorse */
          process_page (&page);

          wikiformats__page__clear (&page);
          dsk_xml_unref (xml);
        }
    }
  if (ferror (fp))
    dsk_warning ("error reading from %s (ignoring)", input_filename);
    /*
  <page>
    <title>AccessibleComputing</title>
    <id>10</id>
    <redirect />
    <revision>
      <id>381202555</id>
      <timestamp>2010-08-26T22:38:36Z</timestamp>
      <contributor>
        <username>OlEnglish</username>
        <id>7181920</id>
      </contributor>
      <minor />
      <comment>[[Help:Reverting|Reverted]] edits by [[Special:Contributions/76.28.186.133|76.28.186.133]] ([[User talk:76.28.186.133|talk]]) to last version by Gurch</comment>
      <text xml:space="preserve">#REDIRECT [[Computer accessibility]] {{R from CamelCase}}</text>
    </revision>
  </page>
  */
  return 0;
}
