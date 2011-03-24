#include "../dsk.h"
#include <stdio.h>
#include <string.h>
#include "generated/xml-binding-test.c"

static dsk_boolean
is_text_child (DskXml *xml, const char *name, const char *contents)
{
  return dsk_xml_is_element (xml, name)
      && xml->n_children == 1
      && xml->children[0]->type == DSK_XML_TEXT
      && strcmp (xml->children[0]->str, contents) == 0;
}

void test_struct_0 (void)
{
  DskXml *xml;
  My__Test__A test_a;
  DskError *error = NULL;
  xml = dsk_xml_new_take_list ("A",
                               dsk_xml_text_child_new ("a", "42"),
                               dsk_xml_text_child_new ("c", "1"),
                               dsk_xml_text_child_new ("c", "2"),
                               dsk_xml_text_child_new ("c", "3"),
                               dsk_xml_text_child_new ("d", "100"),
                               dsk_xml_text_child_new ("d", "110"),
                               dsk_xml_text_child_new ("d", "120"),
                               NULL);
  if (!my__test__a__parse (xml, &test_a, &error))
    dsk_die ("error unpacking from xml (line %u): %s",
             __LINE__, error->message);
  dsk_assert (test_a.a == 42);
  dsk_assert (!test_a.has_b);
  dsk_assert (test_a.n_c == 3);
  dsk_assert (test_a.c[0] == 1);
  dsk_assert (test_a.c[1] == 2);
  dsk_assert (test_a.c[2] == 3);
  dsk_assert (test_a.n_d == 3);
  dsk_assert (test_a.d[0] == 100);
  dsk_assert (test_a.d[1] == 110);
  dsk_assert (test_a.d[2] == 120);
  dsk_xml_unref (xml);
  xml = my__test__a__to_xml (&test_a, &error);
  if (xml == NULL)
    dsk_die ("error converting to xml (line %u): %s",
             __LINE__, error->message);
  my__test__a__clear (&test_a);
  dsk_assert (dsk_xml_is_element (xml, "A"));
  dsk_assert (xml->n_children == 7);
  dsk_assert (is_text_child (xml->children[0], "a", "42"));
  dsk_assert (is_text_child (xml->children[1], "c", "1"));
  dsk_assert (is_text_child (xml->children[2], "c", "2"));
  dsk_assert (is_text_child (xml->children[3], "c", "3"));
  dsk_assert (is_text_child (xml->children[4], "d", "100"));
  dsk_assert (is_text_child (xml->children[5], "d", "110"));
  dsk_assert (is_text_child (xml->children[6], "d", "120"));
  dsk_xml_unref (xml);
}

void test_union_0 (void)
{
  DskXml *xml;
  My__Test__B test_b;
  DskError *error = NULL;

  xml = dsk_xml_text_child_new ("foo", "42");
  if (!my__test__b__parse (xml, &test_b, &error))
    dsk_die ("error unpacking from xml (line %u): %s",
             __LINE__, error->message);
  dsk_assert (test_b.type == MY__TEST__B__TYPE__FOO);
  dsk_assert (test_b.variant.foo == 42);
  dsk_xml_unref (xml);
  xml = my__test__b__to_xml (&test_b, &error);
  if (xml == NULL)
    dsk_die ("error converting to xml (line %u): %s",
             __LINE__, error->message);
  my__test__b__clear (&test_b);
  dsk_assert (is_text_child (xml, "foo", "42"));
  dsk_xml_unref (xml);

  xml = dsk_xml_text_child_new ("bar", "42");
  if (!my__test__b__parse (xml, &test_b, &error))
    dsk_die ("error unpacking from xml (line %u): %s",
             __LINE__, error->message);
  dsk_assert (test_b.type == MY__TEST__B__TYPE__BAR);
  dsk_assert (strcmp (test_b.variant.bar, "42") == 0);
  dsk_xml_unref (xml);
  xml = my__test__b__to_xml (&test_b, &error);
  if (xml == NULL)
    dsk_die ("error converting to xml (line %u): %s",
             __LINE__, error->message);
  my__test__b__clear (&test_b);
  dsk_assert (is_text_child (xml, "bar", "42"));
  dsk_xml_unref (xml);
}

void test_union_with_elided_0 (void)
{
  DskXml *xml;
  My__Test__C test_c;
  DskError *error = NULL;

  xml = dsk_xml_new_take_list ("foo",
                               dsk_xml_text_child_new ("z", "1"),
                               dsk_xml_text_child_new ("zz", "100"),
                               dsk_xml_text_child_new ("z", "2"),
                               NULL);
  if (!my__test__c__parse (xml, &test_c, &error))
    dsk_die ("error unpacking from xml (line %u): %s",
             __LINE__, error->message);
  dsk_assert (test_c.type == MY__TEST__C__TYPE__FOO);
  dsk_assert (test_c.variant.foo.n_z == 2);
  dsk_assert (test_c.variant.foo.z[0] == 1);
  dsk_assert (test_c.variant.foo.z[1] == 2);
  dsk_assert (test_c.variant.foo.n_zz == 1);
  dsk_assert (strcmp (test_c.variant.foo.zz[0], "100") == 0);
  dsk_xml_unref (xml);
  xml = my__test__c__to_xml (&test_c, &error);
  if (xml == NULL)
    dsk_die ("error converting to xml (line %u): %s",
             __LINE__, error->message);
  my__test__c__clear (&test_c);
  dsk_assert (dsk_xml_is_element (xml, "foo"));
  dsk_assert (xml->n_children == 3);
  dsk_assert (is_text_child (xml->children[0], "z", "1"));
  dsk_assert (is_text_child (xml->children[1], "z", "2"));
  dsk_assert (is_text_child (xml->children[2], "zz", "100"));
  dsk_xml_unref (xml);
}


static struct 
{
  const char *name;
  void (*test)(void);
} tests[] =
{
  { "simple structure handling", test_struct_0 },
  { "simple union handling", test_union_0 },
  { "simple struct-in-union handling", test_union_with_elided_0 }
};
int main(int argc, char **argv)
{
  unsigned i;
  dsk_cmdline_init ("test xml binding code",
                    "Test XML Binding Code",
                    NULL, 0);
  //dsk_cmdline_add_boolean ("verbose", "extra logging", NULL, 0,
                           //&cmdline_verbose);
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
