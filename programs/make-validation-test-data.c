#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "../dsk.h"

static void
load_file_to_buffer (const char *filename,
                     DskBuffer  *out)
{
  int fd = open (filename, O_RDONLY);
  if (fd < 0)
    dsk_die ("error opening %s: %s", filename, strerror (errno));
  for (;;)
    {
      int read_rv = dsk_buffer_readv (out, fd);
      if (read_rv < 0)
        {
          if (errno == EINTR || errno == EAGAIN)
            continue;
          dsk_die ("error reading from %s: %s", filename, strerror (errno));
        }
      if (read_rv == 0)
        break;
    }
  close (fd);
}
int main(int argc, char **argv)
{
  DskPrint *print;
  char *xmlpath = (char*) "TESTCASES/TEST";
  dsk_cmdline_init ("generate XML conformance testing C file",
                    "Generate data that will be included by tests/test-xml-validation-suite\n",
                    NULL, 0);
  dsk_cmdline_add_string ("xmlpath", "Path to TEST nodes", "XPATH",
                          0, &xmlpath);
  dsk_cmdline_process_args (&argc, &argv);

  DskXmlParserConfig *config;
  DskXmlParser *parser;

  config = dsk_xml_parser_config_new_simple (0, xmlpath);
  parser = dsk_xml_parser_new (config, "standard-input");
  print = dsk_print_new_fp (stdout);

  for (;;)
    {
      /* read from file */
      char buf[4096];
      int nread = fread (buf, 1, sizeof (buf), stdin);
      DskError *error = NULL;
      if (nread < 0)
        dsk_die ("error reading from stdin");
      if (nread == 0)
        break;

      /* parse */
      if (!dsk_xml_parser_feed (parser, nread, buf, &error))
        dsk_die ("error parsing xml: %s", error->message);

      /* handle XML nodes */
      DskXml *xml;
      while ((xml=dsk_xml_parser_pop (parser, NULL)) != NULL)
        {
          const char *uri = dsk_xml_find_attr (xml, "URI");
          const char *type = dsk_xml_find_attr (xml, "TYPE");
          const char *entities = dsk_xml_find_attr (xml, "ENTITIES");
          const char *out = dsk_xml_find_attr (xml, "OUTPUT");
          char *text;
          dsk_assert (uri);
          dsk_assert (type);
          //dsk_assert (xml->n_children == 1);
          //dsk_assert (xml->children[0]->type == DSK_XML_TEXT);
          if (entities)
            dsk_warning ("entities: %s", entities);
          text = dsk_xml_get_all_text (xml);
          dsk_boolean good;
          good = (strcmp (type, "valid") == 0);
          if (!good)
            {
              if (strcmp (type, "invalid") != 0
               && strcmp (type, "not-wf") != 0
               && strcmp (type, "error") != 0)
                dsk_die ("got unknown type '%s': expected 'valid', 'invalid', 'not-wf'",
                         type);
              out = NULL;
            }

          /* Load uri, render as c-escaped */
          dsk_print_push (print);
          DskBuffer buffer = DSK_BUFFER_INIT;
          load_file_to_buffer (uri, &buffer);
          dsk_print_set_uint (print, "in_size", buffer.size);
          dsk_print_set_filtered_buffer (print, "in", &buffer, dsk_c_quoter_new (DSK_TRUE, DSK_TRUE));
          dsk_buffer_clear (&buffer);
          if (out == NULL)
            {
              dsk_print_set_uint (print, "out_size", 0);
              dsk_print_set_string (print, "out", "NULL");
            }
          else
            {
              load_file_to_buffer (out, &buffer);
              dsk_print_set_uint (print, "out_size", buffer.size);
              dsk_print_set_filtered_buffer (print, "out", &buffer, dsk_c_quoter_new (DSK_TRUE, DSK_TRUE));
              dsk_buffer_clear (&buffer);
            }
          dsk_print_set_string (print, "uri", uri);
          dsk_print_set_filtered_string (print, "description",
                                         text,
                                         dsk_octet_filter_chain_new_take_list (
                                           dsk_whitespace_trimmer_new (),
                                           dsk_c_quoter_new (DSK_TRUE, DSK_TRUE),
                                           NULL
                                         ));
          dsk_free (text);
          dsk_print (print, "{\n"
                           "  \"$uri\",\n"
                           "  $in_size,\n"
                           "  $in,\n"
                           "  $out_size,\n"
                           "  $out,\n"
                           "  $description\n"
                           "},");
          dsk_print_pop (print);

          dsk_xml_unref (xml);
        }
    }
  return 0;
}
