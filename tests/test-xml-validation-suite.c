#include <stdio.h>
#include <string.h>
#include "../dsk.h"

static dsk_boolean print_errors = DSK_FALSE;
static dsk_boolean verbose = DSK_FALSE;


static dsk_boolean
xml_equal (DskXml *a, DskXml *b)
{
  if (a->type != b->type)
    return DSK_FALSE;
  if (strcmp (a->str, b->str) != 0)
    return DSK_FALSE;
  if (a->type == DSK_XML_ELEMENT)
    {
      unsigned i;
      for (i = 0; ; i++)
        {
          if (a->attrs[i] == NULL && b->attrs[i] == NULL)
            break;
          if (a->attrs[i] == NULL || b->attrs[i] == NULL)
            return DSK_FALSE;
          if (strcmp (a->attrs[i], b->attrs[i]) != 0)
            return DSK_FALSE;
        }
      if (a->n_children != b->n_children)
        return DSK_FALSE;
      for (i = 0; i < a->n_children; i++)
        if (!xml_equal (a->children[i], b->children[i]))
          return DSK_FALSE;
    }
  return DSK_TRUE;
}

typedef struct
{
  const char *name;
  unsigned in_length;
  const char *in;
  unsigned out_length;
  const char *out;
  const char *description;
  } Test;

static Test tests[] = {
#include "xml-validation-suite.inc"
};

int main(int argc, char **argv)
{
  DskXmlParser *parser;
  unsigned i;
  char *star = "*";
  unsigned n_correct_valid = 0;
  unsigned n_correct_invalid = 0;
  unsigned n_failed_valid = 0;
  unsigned n_failed_invalid = 0;
  dsk_boolean print_failure_descriptions = DSK_FALSE;
  DskXmlParserConfig *config;

  dsk_cmdline_init ("standard xml conformance test",
                    "Run some standard conformance test on our XML parser",
                    NULL, 0);
  dsk_cmdline_add_boolean ("print-error", "print error messages for bad-response tests", NULL, 0,
                           &print_errors);
  dsk_cmdline_add_boolean ("verbose", "print all test statuses", NULL, 0,
                           &verbose);
  dsk_cmdline_add_boolean ("desc", "print description of failed tests", NULL, 0,
                           &print_failure_descriptions);
  dsk_cmdline_process_args (&argc, &argv);


  config = dsk_xml_parser_config_new (DSK_XML_PARSER_IGNORE_NS,
                                                          0, NULL,
                                                          1, &star,
                                                          NULL);
  for (i = 0; i < DSK_N_ELEMENTS (tests); i++)
    {
      DskXml *xml = NULL, *oxml = NULL;
      DskError *error = NULL, *oerror = NULL;
      if (verbose)
        fprintf (stderr, "%s... ", tests[i].name);
      parser = dsk_xml_parser_new (config, tests[i].name);
      if (dsk_xml_parser_feed (parser,
                               tests[i].in_length,
                               (const uint8_t *) tests[i].in,
                               &error))
        xml = dsk_xml_parser_pop (parser, NULL);
      else
        {
          if (print_errors)
            dsk_warning ("for regular xml: error: %s", error->message);
          dsk_error_unref (error);
        }
      if (tests[i].out != NULL)
        {
          DskXmlParser *oparser = dsk_xml_parser_new (config, tests[i].name);
          if (dsk_xml_parser_feed (oparser,
                                   tests[i].out_length,
                                   (uint8_t *) tests[i].out,
                                   &oerror))
            oxml = dsk_xml_parser_pop (oparser, NULL);
          else
            {
              if (print_errors)
                dsk_warning ("for simplified xml: error: %s", oerror->message);
              dsk_error_unref (oerror);
            }
        }
      if (xml == NULL && tests[i].out != NULL)
        {
          if (oxml == NULL)
            {
              if (verbose)
                fprintf (stderr, "FAILED  [unable to parse any xml]\n");
            }
          else
            {
              if (verbose)
                fprintf (stderr, "FAILED  [unable to parse original xml]\n");
            }
          if (print_failure_descriptions)
            fprintf (stderr, "   >> %s\n", tests[i].description);
          n_failed_valid++;
        }
      else if (xml != NULL && oxml != NULL)
        {
          /* equal? */
          if (xml_equal (xml, oxml))
            {
              if (verbose)
                fprintf (stderr, "PASS\n");
              n_correct_valid++;
            }
          else
            {
              if (verbose)
                fprintf (stderr, "FAILED  [xml mismatch]\n");
              if (print_failure_descriptions)
                fprintf (stderr, "   >> %s\n", tests[i].description);
              n_failed_valid++;
            }
        }
      else if (xml != NULL && tests[i].out == NULL)
        {
          if (verbose)
            fprintf (stderr, "TOO LAX [accepted invalid xml]\n");
          if (print_failure_descriptions)
            fprintf (stderr, "   >> %s\n", tests[i].description);
          n_failed_invalid++;
        }
      else if (xml == NULL && tests[i].out == NULL)
        {
          if (verbose)
            fprintf (stderr, "PASS    [rejected invalid xml]\n");
          n_correct_invalid++;
        }
      else if (xml != NULL && tests[i].out != NULL && oxml == NULL)
        {
          if (verbose)
            fprintf (stderr, "FAILED  [parsing simplified xml]\n");
          if (print_failure_descriptions)
            fprintf (stderr, "   >> %s\n", tests[i].description);
          n_failed_valid++;
        }
    }

  fprintf (stderr, "Correctly handled valid documents: %u\n"
                   "Rejected invalid documents: %u\n\n"
                   "Incorrectly handled valid documents: %u\n"
                   "Invalid documents accepted: %u\n",
                   n_correct_valid, n_correct_invalid,
                   n_failed_valid, n_failed_invalid);

  return 0;
}
