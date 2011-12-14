#include <stdio.h>
#include <string.h>
#include "../dsk.h"
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

static dsk_boolean is_element (DskXml *xml, const char *name)
{
  return xml->type == DSK_XML_ELEMENT && strcmp (xml->str, name) == 0;
}
static dsk_boolean is_text (DskXml *xml, const char *name)
{
  return xml->type == DSK_XML_TEXT && strcmp (xml->str, name) == 0;
}
static DskXml *
load_valid_xml (const char *str,
                DskXmlParserConfig *config)
{
  unsigned str_length = strlen (str);

  /* patterns: an array of 0-terminated arrays signifying
   * what size chunks to feed the data to parser in.
   */
  static unsigned patterns[] = {
    1,0,               /* load byte-by-byte */
    1000000000,0,      /* load the entire thing in one go */
    1,13,0,
    1,10,0,
    1,100,0,
    1,1000,0,
    1,10000,0,
    0
  };
  unsigned *patterns_at = patterns;
  DskXml *rv = NULL;
  while (*patterns_at != 0)
    {
      unsigned *start_pattern = patterns_at;
      DskXmlParser *parser = dsk_xml_parser_new (config, NULL);
      const char *str_at = str;
      unsigned rem = str_length;
      DskXml *xml;
      DskXml *second_xml;
      dsk_assert (parser);
      //dsk_warning ("starting with new pattern");
      while (rem != 0)
        {
          for (patterns_at = start_pattern; *patterns_at && rem != 0; patterns_at++)
            {
              unsigned use = rem < *patterns_at ? rem : *patterns_at;
              DskError *error = NULL;
              if (!dsk_xml_parser_feed (parser, use, (const uint8_t *) str_at, &error))
                dsk_die ("error feeding xml to parser: %s", error->message);
              rem -= use;
              str_at += use;
            }
        }
      xml = dsk_xml_parser_pop (parser, NULL);
      dsk_assert (xml != NULL);
      second_xml = dsk_xml_parser_pop (parser, NULL);
      dsk_assert (second_xml == NULL);
      if (start_pattern == patterns)
        rv = xml;
      else
        {
          if (!xml_equal (rv, xml))
            dsk_die ("feeding different ways led to different xml");
          dsk_xml_unref (xml);
        }
      dsk_xml_parser_free (parser);

      while (*start_pattern)
        start_pattern++;
      patterns_at = start_pattern + 1;  /* skip pattern's 0 */
    }
  return rv;
}


static void
test_simple_0 (void)
{
  char *empty = "*";
  DskError *error = NULL;
  DskXml *xml;
  DskXmlParserConfig *config = dsk_xml_parser_config_new (0, 0, NULL,
                                                          1, &empty,
                                                          &error);
  if (config == NULL)
    dsk_die ("error creating parser-config: %s", error->message);
  xml = load_valid_xml ("<abc>tmp</abc>", config);
  dsk_assert (xml->type == DSK_XML_ELEMENT);
  dsk_assert (strcmp (xml->str, "abc") == 0);
  dsk_assert (xml->n_children == 1);
  dsk_assert (xml->children[0]->type == DSK_XML_TEXT);
  dsk_assert (strcmp (xml->children[0]->str, "tmp") == 0);
  dsk_xml_unref (xml);
  dsk_xml_parser_config_destroy (config);
}

static void
test_simple_attrs_0 (void)
{
  char *empty = "*";
  static const char *xml_texts[] = {"<abc a=\"b\">tmp</abc>",
                                    "<abc a='b'>tmp</abc>"};

  DskError *error = NULL;
  unsigned i;
  DskXmlParserConfig *config = dsk_xml_parser_config_new (0, 0, NULL,
                                                          1, &empty,
                                                          &error);
  if (config == NULL)
    dsk_die ("error creating parser-config: %s", error->message);
  for (i = 0; i < 2; i++)
    {
      DskXml *xml;
      xml = load_valid_xml (xml_texts[i], config);
      dsk_assert (is_element (xml, "abc"));
      dsk_assert (strcmp (xml->attrs[0], "a") == 0);
      dsk_assert (strcmp (xml->attrs[1], "b") == 0);
      dsk_assert (xml->attrs[2] == NULL);
      dsk_assert (xml->n_children == 1);
      dsk_assert (is_text (xml->children[0], "tmp"));
      dsk_xml_unref (xml);
    }
  dsk_xml_parser_config_destroy (config);
}
static void
test_simple_attrs_1 (void)
{
  char *empty = "*";
  static const char *xml_texts[] = {"<abc a=\"b\" cc='dd'>tmp</abc>",
                                    "<abc a='b' cc=\"dd\">tmp</abc>"};

  DskError *error = NULL;
  unsigned i;
  DskXmlParserConfig *config = dsk_xml_parser_config_new (0, 0, NULL,
                                                          1, &empty,
                                                          &error);
  if (config == NULL)
    dsk_die ("error creating parser-config: %s", error->message);
  for (i = 0; i < 2; i++)
    {
      DskXml *xml;
      xml = load_valid_xml (xml_texts[i], config);
      dsk_assert (is_element (xml, "abc"));
      dsk_assert (strcmp (xml->attrs[0], "a") == 0);
      dsk_assert (strcmp (xml->attrs[1], "b") == 0);
      dsk_assert (strcmp (xml->attrs[2], "cc") == 0);
      dsk_assert (strcmp (xml->attrs[3], "dd") == 0);
      dsk_assert (xml->attrs[4] == NULL);
      dsk_assert (xml->n_children == 1);
      dsk_assert (is_text (xml->children[0], "tmp"));
      dsk_xml_unref (xml);
    }
  dsk_xml_parser_config_destroy (config);
}
static void
test_simple_attrs_2 (void)
{
  char *empty = "*";
  static const char *xml_texts[] = {"<abc a=\"b\" cc='d  d'>tmp</abc>",
                                    "<abc a='b' cc=\"d  d\">tmp</abc>"};

  DskError *error = NULL;
  unsigned i;
  DskXmlParserConfig *config = dsk_xml_parser_config_new (0, 0, NULL,
                                                          1, &empty,
                                                          &error);
  if (config == NULL)
    dsk_die ("error creating parser-config: %s", error->message);
  for (i = 0; i < 2; i++)
    {
      DskXml *xml;
      xml = load_valid_xml (xml_texts[i], config);
      dsk_assert (is_element (xml, "abc"));
      dsk_assert (strcmp (xml->attrs[0], "a") == 0);
      dsk_assert (strcmp (xml->attrs[1], "b") == 0);
      dsk_assert (strcmp (xml->attrs[2], "cc") == 0);
      dsk_assert (strcmp (xml->attrs[3], "d  d") == 0);
      dsk_assert (xml->attrs[4] == NULL);
      dsk_assert (xml->n_children == 1);
      dsk_assert (is_text (xml->children[0], "tmp"));
      dsk_xml_unref (xml);
    }
  dsk_xml_parser_config_destroy (config);
}
static void
test_simple_tree (void)
{
  char *empty = "*";
  DskError *error = NULL;
  DskXml *xml;
  DskXmlParserConfig *config = dsk_xml_parser_config_new (0, 0, NULL,
                                                          1, &empty,
                                                          &error);
  if (config == NULL)
    dsk_die ("error creating parser-config: %s", error->message);
  xml = load_valid_xml ("<abc><a><b>cc</b><b>dd</b></a><foo>q</foo></abc>", config);

  dsk_assert (is_element (xml, "abc"));
  dsk_assert (xml->n_children == 2);
  dsk_assert (is_element (xml->children[0], "a"));
  dsk_assert (xml->children[0]->n_children == 2);
  dsk_assert (is_element (xml->children[0]->children[0], "b"));
  dsk_assert (xml->children[0]->children[0]->n_children == 1);
  dsk_assert (is_text (xml->children[0]->children[0]->children[0], "cc"));
  dsk_assert (is_element (xml->children[0]->children[1], "b"));
  dsk_assert (xml->children[0]->children[1]->n_children == 1);
  dsk_assert (is_text (xml->children[0]->children[1]->children[0], "dd"));
  dsk_assert (is_element (xml->children[1], "foo"));
  dsk_assert (xml->children[1]->n_children == 1);
  dsk_assert (is_text (xml->children[1]->children[0], "q"));
  dsk_xml_unref (xml);
  dsk_xml_parser_config_destroy (config);
}
static void
test_simple_comment (void)
{
  char *empty = "*";
  static const char *xml_texts[] = {
      "<!-- this is a comment-->\n<abc a=\"b\">tmp</abc>",
      "<abc a='b'>t<!--this is a comment-->mp</abc>",
      "<abc a='b'>t<!--comment-->m<!--another comment? -->p</abc>"
  };

  DskError *error = NULL;
  unsigned i;
  DskXmlParserConfig *config = dsk_xml_parser_config_new (0, 0, NULL,
                                                          1, &empty,
                                                          &error);
  if (config == NULL)
    dsk_die ("error creating parser-config: %s", error->message);
  for (i = 0; i < sizeof(xml_texts)/sizeof(xml_texts[0]); i++)
    {
      DskXml *xml;
      xml = load_valid_xml (xml_texts[i], config);
      dsk_assert (xml->type == DSK_XML_ELEMENT);
      dsk_assert (strcmp (xml->str, "abc") == 0);
      dsk_assert (strcmp (xml->attrs[0], "a") == 0);
      dsk_assert (strcmp (xml->attrs[1], "b") == 0);
      dsk_assert (xml->attrs[2] == NULL);
      dsk_assert (xml->n_children == 1);
      dsk_assert (xml->children[0]->type == DSK_XML_TEXT);
      dsk_assert (strcmp (xml->children[0]->str, "tmp") == 0);
      dsk_xml_unref (xml);
    }
  dsk_xml_parser_config_destroy (config);
}
static void
test_simple_comment_handling (void)
{
  char *empty = "*";
  static const char *xml_texts[] = {
      /* XXX: there's no way to capture toplevel comments!
       * this is, however, per spec.  We should have a magic path
       * that maps to comments */
      "<!-- this is a comment-->\n<abc a=\"b\">tmp</abc>",

      "<abc a='b'>t<!--this is a comment-->mp</abc>",
      "<abc a='b'>t<!--comment-->m<!--another comment?-->p</abc>",
      "<abc a='b'><!--comment-->tmp</abc>",
      "<abc a='b'>tmp<!--comment--></abc>"
  };

  DskError *error = NULL;
  unsigned i;
  DskXmlParserConfig *config = dsk_xml_parser_config_new (DSK_XML_PARSER_INCLUDE_COMMENTS, 0, NULL,
                                                          1, &empty,
                                                          &error);
  if (config == NULL)
    dsk_die ("error creating parser-config: %s", error->message);
  for (i = 0; i < sizeof(xml_texts)/sizeof(xml_texts[0]); i++)
    {
      DskXml *xml;
      xml = load_valid_xml (xml_texts[i], config);
      dsk_assert (xml->type == DSK_XML_ELEMENT);
      dsk_assert (strcmp (xml->str, "abc") == 0);
      dsk_assert (strcmp (xml->attrs[0], "a") == 0);
      dsk_assert (strcmp (xml->attrs[1], "b") == 0);
      dsk_assert (xml->attrs[2] == NULL);
      switch (i)
        {
        case 0:
          dsk_assert (xml->n_children == 1);
          dsk_assert (xml->children[0]->type == DSK_XML_TEXT);
          dsk_assert (strcmp (xml->children[0]->str, "tmp") == 0);
          break;
        case 1:
          dsk_assert (xml->n_children == 3);
          dsk_assert (xml->children[0]->type == DSK_XML_TEXT);
          dsk_assert (strcmp (xml->children[0]->str, "t") == 0);
          dsk_assert (xml->children[1]->type == DSK_XML_COMMENT);
          dsk_assert (strcmp (xml->children[1]->str, "this is a comment") == 0);
          dsk_assert (xml->children[2]->type == DSK_XML_TEXT);
          dsk_assert (strcmp (xml->children[2]->str, "mp") == 0);
          break;
        case 2:
          dsk_assert (xml->n_children == 5);
          dsk_assert (xml->children[0]->type == DSK_XML_TEXT);
          dsk_assert (strcmp (xml->children[0]->str, "t") == 0);
          dsk_assert (xml->children[1]->type == DSK_XML_COMMENT);
          dsk_assert (strcmp (xml->children[1]->str, "comment") == 0);
          dsk_assert (xml->children[2]->type == DSK_XML_TEXT);
          dsk_assert (strcmp (xml->children[2]->str, "m") == 0);
          dsk_assert (xml->children[3]->type == DSK_XML_COMMENT);
          dsk_assert (strcmp (xml->children[3]->str, "another comment?") == 0);
          dsk_assert (xml->children[4]->type == DSK_XML_TEXT);
          dsk_assert (strcmp (xml->children[4]->str, "p") == 0);
          break;
        case 3:
          dsk_assert (xml->n_children == 2);
          dsk_assert (xml->children[0]->type == DSK_XML_COMMENT);
          dsk_assert (strcmp (xml->children[0]->str, "comment") == 0);
          dsk_assert (xml->children[1]->type == DSK_XML_TEXT);
          dsk_assert (strcmp (xml->children[1]->str, "tmp") == 0);
          break;
        case 4:
          dsk_assert (xml->n_children == 2);
          dsk_assert (xml->children[0]->type == DSK_XML_TEXT);
          dsk_assert (strcmp (xml->children[0]->str, "tmp") == 0);
          dsk_assert (xml->children[1]->type == DSK_XML_COMMENT);
          dsk_assert (strcmp (xml->children[1]->str, "comment") == 0);
          break;
        }
      dsk_xml_unref (xml);
    }
  dsk_xml_parser_config_destroy (config);
}
static void
test_empty_element (void)
{
  char *empty = "*";
  static const char *xml_texts[] = {"<abc a=\"b\" cc='dd' />",
                                    "<abc a='b' cc=\"dd\" />",
                                    "<abc a='b' cc=\"dd\" ></abc>"};

  DskError *error = NULL;
  unsigned i;
  DskXmlParserConfig *config = dsk_xml_parser_config_new (0, 0, NULL,
                                                          1, &empty,
                                                          &error);
  if (config == NULL)
    dsk_die ("error creating parser-config: %s", error->message);
  for (i = 0; i < 3; i++)
    {
      DskXml *xml;
      xml = load_valid_xml (xml_texts[i], config);
      dsk_assert (is_element (xml, "abc"));
      dsk_assert (strcmp (xml->attrs[0], "a") == 0);
      dsk_assert (strcmp (xml->attrs[1], "b") == 0);
      dsk_assert (strcmp (xml->attrs[2], "cc") == 0);
      dsk_assert (strcmp (xml->attrs[3], "dd") == 0);
      dsk_assert (xml->attrs[4] == NULL);
      dsk_assert (xml->n_children == 0);
      dsk_xml_unref (xml);
    }
  dsk_xml_parser_config_destroy (config);
}

static void
test_standard_char_entities (void)
{
  char *empty = "*";
  DskError *error = NULL;
  static struct {
    const char *xml_str;
    const char *center_str;
  } xml_char_tests[] = {
    { "<abc>&lt;</abc>", "<" },
    { "<abc>&gt;</abc>", ">" },
    { "<abc>&apos;</abc>", "'" },
    { "<abc>&quot;</abc>", "\"" },
    { "<abc>&lt;hi&gt;</abc>", "<hi>" },
    { "<abc>what&apos;s up?</abc>", "what's up?" },
  };
  unsigned i;
  DskXmlParserConfig *config = dsk_xml_parser_config_new (0, 0, NULL,
                                                          1, &empty,
                                                          &error);
  if (config == NULL)
    dsk_die ("error creating parser-config: %s", error->message);
  for (i = 0; i < DSK_N_ELEMENTS (xml_char_tests); i++)
    {
      DskXml *xml = load_valid_xml (xml_char_tests[i].xml_str, config);
      dsk_assert (xml->type == DSK_XML_ELEMENT);
      dsk_assert (strcmp (xml->str, "abc") == 0);
      dsk_assert (xml->n_children == 1);
      dsk_assert (xml->children[0]->type == DSK_XML_TEXT);
      dsk_assert (strcmp (xml->children[0]->str, xml_char_tests[i].center_str) == 0);
      dsk_xml_unref (xml);
    }
  dsk_xml_parser_config_destroy (config);
}

static void
feed_string (DskXmlParser *parser, const char *str)
{
  DskError *error = NULL;
  if (!dsk_xml_parser_feed (parser, strlen (str), (const uint8_t *) str, &error))
    dsk_die ("feed_string: %s", error->message);
}
static void
feed_string_1by1 (DskXmlParser *parser, const char *str)
{
  DskError *error = NULL;
  while (*str)
    {
      if (!dsk_xml_parser_feed (parser, 1, (const uint8_t *) str, &error))
        dsk_die ("feed_string_1by1: %s", error->message);
      str++;
    }
}
static void
test_path_handling_0 (void)
{
  char *paths[] = { "abc/a", "abc/b", "abc" };
  DskError *error = NULL;
  DskXmlParserConfig *config = dsk_xml_parser_config_new (0, 0, NULL,
                                                          3, paths,
                                                          &error);
  DskXmlParser *parser;
  DskXml *xml;
  unsigned i;
  if (config == NULL)
    dsk_die ("error creating parser-config: %s", error->message);
  parser = dsk_xml_parser_new (config, NULL);
  feed_string (parser, "<abc><a>foo");
  dsk_assert (dsk_xml_parser_pop (parser, NULL) == NULL);
  feed_string (parser, "</a>");
  xml = dsk_xml_parser_pop (parser, &i);
  dsk_assert (i == 0);
  dsk_assert (is_element (xml, "a"));
  dsk_assert (xml->n_children == 1);
  dsk_assert (is_text (xml->children[0], "foo"));
  dsk_xml_unref (xml);
  feed_string_1by1 (parser, "<b>GOO</b>");
  xml = dsk_xml_parser_pop (parser, &i);
  dsk_assert (i == 1);
  dsk_assert (is_element (xml, "b"));
  dsk_assert (xml->n_children == 1);
  dsk_assert (is_text (xml->children[0], "GOO"));
  dsk_xml_unref (xml);
  feed_string (parser, "</abc>");
  xml = dsk_xml_parser_pop (parser, &i);
  dsk_assert (i == 2);
  dsk_assert (is_element (xml, "abc"));
  dsk_assert (xml->n_children == 2);
  dsk_assert (is_element (xml->children[0], "a"));
  dsk_assert (xml->children[0]->n_children == 1);
  dsk_assert (is_text (xml->children[0]->children[0], "foo"));
  dsk_assert (is_element (xml->children[1], "b"));
  dsk_assert (xml->children[1]->n_children == 1);
  dsk_assert (is_text (xml->children[1]->children[0], "GOO"));
  dsk_xml_unref (xml);

  dsk_xml_parser_free (parser);
  dsk_xml_parser_config_destroy (config);
}
static void
test_path_handling_1 (void)
{
  char *paths[] = { "abc/a", "abc/*", "abc" };
  DskError *error = NULL;
  DskXmlParserConfig *config = dsk_xml_parser_config_new (0, 0, NULL,
                                                          3, paths,
                                                          &error);
  DskXmlParser *parser;
  DskXml *xml;
  unsigned i;
  if (config == NULL)
    dsk_die ("error creating parser-config: %s", error->message);
  parser = dsk_xml_parser_new (config, NULL);
  feed_string (parser, "<abc><a>foo");
  dsk_assert (dsk_xml_parser_pop (parser, NULL) == NULL);
  feed_string (parser, "</a>");
  xml = dsk_xml_parser_pop (parser, &i);
  dsk_assert (i == 0);
  dsk_assert (is_element (xml, "a"));
  dsk_assert (xml->n_children == 1);
  dsk_assert (is_text (xml->children[0], "foo"));
  dsk_xml_unref (xml);
  xml = dsk_xml_parser_pop (parser, &i);
  dsk_assert (i == 1);
  dsk_assert (is_element (xml, "a"));
  dsk_assert (xml->n_children == 1);
  dsk_assert (is_text (xml->children[0], "foo"));
  dsk_xml_unref (xml);
  feed_string_1by1 (parser, "<b>GOO</b>");
  xml = dsk_xml_parser_pop (parser, &i);
  dsk_assert (i == 1);
  dsk_assert (is_element (xml, "b"));
  dsk_assert (xml->n_children == 1);
  dsk_assert (is_text (xml->children[0], "GOO"));
  dsk_xml_unref (xml);
  feed_string (parser, "</abc>");
  xml = dsk_xml_parser_pop (parser, &i);
  dsk_assert (i == 2);
  dsk_assert (is_element (xml, "abc"));
  dsk_assert (xml->n_children == 2);
  dsk_assert (is_element (xml->children[0], "a"));
  dsk_assert (xml->children[0]->n_children == 1);
  dsk_assert (is_text (xml->children[0]->children[0], "foo"));
  dsk_assert (is_element (xml->children[1], "b"));
  dsk_assert (xml->children[1]->n_children == 1);
  dsk_assert (is_text (xml->children[1]->children[0], "GOO"));
  dsk_xml_unref (xml);

  dsk_xml_parser_free (parser);
  dsk_xml_parser_config_destroy (config);
}

static void test_ns_simple_0 (void)
{
  char *empty = "*";
  DskXmlParserNamespaceConfig ns_config[] =
    { { "http://decreation.com/daveb-test-0", "dt" } };
  static const char *xml_texts[] = {
      "<abc a=\"b\" xmlns='http://decreation.com/daveb-test-0'><b>foo</b></abc>",
      "<xyz:abc a=\"b\" xmlns:xyz='http://decreation.com/daveb-test-0'><xyz:b>foo</xyz:b></xyz:abc>",
      "<xyz:abc a=\"b\" xmlns:xyz='http://decreation.com/daveb-test-0'><zzz:b xmlns:zzz='http://decreation.com/daveb-test-0'>foo</zzz:b></xyz:abc>",
  };

  DskError *error = NULL;
  unsigned i;
  DskXmlParserConfig *config = dsk_xml_parser_config_new (0, 1, ns_config,
                                                          1, &empty,
                                                          &error);
  if (config == NULL)
    dsk_die ("error creating parser-config: %s", error->message);
  for (i = 0; i < sizeof(xml_texts)/sizeof(xml_texts[0]); i++)
    {
      DskXml *xml;
      xml = load_valid_xml (xml_texts[i], config);
      dsk_assert (is_element (xml, "dt:abc"));
      dsk_assert (strcmp (xml->attrs[0], "a") == 0);
      dsk_assert (strcmp (xml->attrs[1], "b") == 0);
      dsk_assert (xml->attrs[2] == NULL);
      dsk_assert (xml->n_children == 1);
      dsk_assert (is_element (xml->children[0], "dt:b"));
      dsk_assert (xml->children[0]->n_children == 1);
      dsk_assert (is_text (xml->children[0]->children[0], "foo"));
      dsk_xml_unref (xml);
    }
  dsk_xml_parser_config_destroy (config);
}

static void test_ns_simple_1 (void)
{
  char *empty = "*";
  DskXmlParserNamespaceConfig ns_config[] =
    { { "http://decreation.com/daveb-test-0", "dt0" },
      { "http://decreation.com/daveb-test-1", "dt1" } };
  static const char *xml_texts[] = {
      "<abc a:a=\"b\" xmlns='http://decreation.com/daveb-test-0' xmlns:a='http://decreation.com/daveb-test-1'><a:b>foo</a:b></abc>",
      "<abc foobar:a=\"b\" xmlns='http://decreation.com/daveb-test-0' xmlns:foobar='http://decreation.com/daveb-test-1'><b xmlns='http://decreation.com/daveb-test-1'>foo</b></abc>",
  };

  DskError *error = NULL;
  unsigned i;
  DskXmlParserConfig *config = dsk_xml_parser_config_new (0, 2, ns_config,
                                                          1, &empty,
                                                          &error);
  if (config == NULL)
    dsk_die ("error creating parser-config: %s", error->message);
  for (i = 0; i < sizeof(xml_texts)/sizeof(xml_texts[0]); i++)
    {
      DskXml *xml;
      xml = load_valid_xml (xml_texts[i], config);
      dsk_assert (is_element (xml, "dt0:abc"));
      dsk_assert (strcmp (xml->attrs[0], "dt1:a") == 0);
      dsk_assert (strcmp (xml->attrs[1], "b") == 0);
      dsk_assert (xml->attrs[2] == NULL);
      dsk_assert (xml->n_children == 1);
      dsk_assert (is_element (xml->children[0], "dt1:b"));
      dsk_assert (xml->children[0]->n_children == 1);
      dsk_assert (is_text (xml->children[0]->children[0], "foo"));
      dsk_xml_unref (xml);
    }
  dsk_xml_parser_config_destroy (config);
}

static void
test_numeric_char_entities (void)
{
  char *empty = "*";
  DskError *error = NULL;
  DskXml *xml;
  DskXmlParserConfig *config = dsk_xml_parser_config_new (0, 0, NULL,
                                                          1, &empty,
                                                          &error);
  if (config == NULL)
    dsk_die ("error creating parser-config: %s", error->message);
  xml = load_valid_xml ("<abc>&#xf;</abc>", config);
  dsk_assert (xml->type == DSK_XML_ELEMENT);
  dsk_assert (strcmp (xml->str, "abc") == 0);
  dsk_assert (xml->n_children == 1);
  dsk_assert (xml->children[0]->type == DSK_XML_TEXT);
  dsk_assert (strcmp (xml->children[0]->str, "\017") == 0);
  dsk_xml_unref (xml);

  xml = load_valid_xml ("<abc>&#x1234;</abc>", config);
  dsk_assert (xml->type == DSK_XML_ELEMENT);
  dsk_assert (strcmp (xml->str, "abc") == 0);
  dsk_assert (xml->n_children == 1);
  dsk_assert (xml->children[0]->type == DSK_XML_TEXT);
  dsk_assert (strcmp (xml->children[0]->str, "\341\210\264") == 0);
  dsk_xml_unref (xml);

  xml = load_valid_xml ("<abc>&#1234;</abc>", config);
  dsk_assert (xml->type == DSK_XML_ELEMENT);
  dsk_assert (strcmp (xml->str, "abc") == 0);
  dsk_assert (xml->n_children == 1);
  dsk_assert (xml->children[0]->type == DSK_XML_TEXT);
  dsk_assert (strcmp (xml->children[0]->str, "\323\222") == 0);
  dsk_xml_unref (xml);

  dsk_xml_parser_config_destroy (config);
}


static struct 
{
  const char *name;
  void (*test)(void);
} tests[] =
{
  { "simple element", test_simple_0 },
  { "numeric character entities", test_numeric_char_entities },
  { "simple element attribute (0)", test_simple_attrs_0 },
  { "simple element attribute (1)", test_simple_attrs_1 },
  { "simple element attribute (2)", test_simple_attrs_2 },
  { "simple tree", test_simple_tree },
  { "simple comment ignoring", test_simple_comment },
  { "simple comment handling", test_simple_comment_handling },
  { "simple empty element", test_empty_element },
  { "test standard character entities", test_standard_char_entities },
  { "test path handling (0)", test_path_handling_0 },
  { "test path handling (1)", test_path_handling_1 },
  { "simple namespace handling (0)", test_ns_simple_0 },
  { "simple namespace handling (1)", test_ns_simple_1 },
};
int main(void)
{
  unsigned i;
  for (i = 0; i < sizeof(tests)/sizeof(tests[0]); i++)
    {
      fprintf (stderr, "Test: %s... ", tests[i].name);
      tests[i].test ();
      fprintf (stderr, " done.\n");
    }
  return 0;
}
