#include "../dsk.h"
#include <stdio.h>
#include <string.h>

int
main(int argc, char **argv)
{
  const char *input_filename = NULL;
  const char *output_filename = NULL;
  const char *function_name = "svg_function";
  DskError *error = NULL;
  DskXmlParserConfig *cfg;
  FILE *input_fp;
  DskXmlParser *parser;
  DskPrint *print;
  DskXmlParserNamespaceConfig namespace_configs[] = {
    {},
    {}
  };
  char *svg_string = "svg";

  dsk_cmdline_permit_extra_arguments(true);
  dsk_cmdline_add_string ("svg-function", "name of function to generate",
                          "FCTNAME", 0, &function_name);
  dsk_cmdline_process_args(&argc, &argv);

  if (argc > 1)
    input_filename = argv[1];
  if (argc > 2)
    output_filename = argv[2];
  if (argc > 3)
    dsk_error("too many arguments");

  cfg = dsk_xml_parser_config_new (0,
                         DSK_N_ELEMENTS(namespace_configs), namespace_configs,
                         1, &svg_string, &error);
  dsk_assert(cfg);

  if (input_filename != NULL)
    {
      parser = dsk_xml_parser_new (cfg, input_filename);
      input_fp = fopen (input_filename, "rb");
    }
  else
    {
      parser = dsk_xml_parser_new (cfg, "*stdin*");
      input_fp = stdin;
    }

  uint8_t databuf[4096];
  size_t nread;
  while ((nread=fread(databuf, 1, sizeof(databuf), input_fp)) != 0)
    {
      if (!dsk_xml_parser_feed (parser, nread, databuf, &error))
        dsk_error ("error parsing XML: %s", error->message);
    }
  if (input_filename != NULL)
    fclose (input_fp);

  DskXml *curxml = dsk_xml_parser_pop (parser, NULL);
  dsk_xml_parser_free (parser);
  dsk_xml_parser_config_destroy (cfg);

  if (output_filename != NULL)
    {
      FILE *fp = fopen (output_filename, "wb");
      if (fp == NULL)
        dsk_error ("error creating %s: %s",
                   output_filename, strerror (errno));
      print = dsk_print_new_fp_fclose (fp);
    }
  else
    {
      print = dsk_print_new_fp (stdout);
    }

  {
    DskCodegenArg args[] = { { "CGContextRef", "ctx" } };
    DskCodegenFunction fct = DSK_CODEGEN_FUNCTION_INIT;
    fct.storage = "static";
    fct.return_type = "void";
    fct.name = function_name;
    fct.n_args = DSK_N_ELEMENTS (args);
    fct.args = args;
    dsk_codegen_function_render (&fct, print);
  }

  dsk_print(print, "{");
  dsk_print(print, "}");

  dsk_xml_unref (curxml);

  return 0;
}
