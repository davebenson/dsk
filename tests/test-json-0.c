#include "../dsk.h"
#include <string.h>
#include <stdio.h>

static dsk_boolean cmdline_verbose = DSK_FALSE;

DskJsonValue *parse_value (const char *str)
{
  DskJsonParser *parser = dsk_json_parser_new ();
  DskError *error = NULL;
  DskJsonValue *v;
  if (cmdline_verbose)
    dsk_warning ("parsing '%s'", str);
  if (!dsk_json_parser_feed (parser, strlen (str), (const uint8_t *) str, &error))
    dsk_die ("error parsing json (%s): %s", str, error->message);
  if (!dsk_json_parser_finish (parser, &error))
    dsk_die ("error finishing json parser (%s): %s", str, error->message);
  v = dsk_json_parser_pop (parser);
  dsk_assert (v != NULL);
  if (dsk_json_parser_pop (parser) != NULL)
    dsk_die ("got two values?");
  dsk_json_parser_destroy (parser);
  return v;
}
void parse_fail (const char *str)
{
  DskJsonParser *parser = dsk_json_parser_new ();
  DskError *error = NULL;
  DskJsonValue *v;
  if (cmdline_verbose)
    dsk_warning ("parsing '%s'", str);
  if (!dsk_json_parser_feed (parser, strlen (str), (const uint8_t *) str, &error))
    {
      dsk_json_parser_destroy (parser);
      return;
    }
  if (!dsk_json_parser_finish (parser, &error))
    {
      dsk_json_parser_destroy (parser);
      return;
    }
  v = dsk_json_parser_pop (parser);
  dsk_assert (v == NULL);
  dsk_json_parser_destroy (parser);
}

char *print_value (DskJsonValue *value)
{
  DskBuffer buffer = DSK_BUFFER_INIT;
  dsk_json_value_to_buffer (value, -1, &buffer);
  return dsk_buffer_empty_to_string (&buffer);
}

int main(int argc, char **argv)
{
  DskJsonValue *v;
  DskJsonMember *mem;
  char *str;

  dsk_cmdline_init ("test JSON handling",
                    "Test JSON parsing",
                    NULL, 0);
  dsk_cmdline_add_boolean ("verbose", "extra logging", NULL, 0,
                           &cmdline_verbose);
  dsk_cmdline_process_args (&argc, &argv);

  v = parse_value ("true");
  dsk_assert (v->type == DSK_JSON_VALUE_BOOLEAN);
  dsk_assert (v->v_boolean.value == DSK_TRUE);
  dsk_json_value_free (v);

  v = parse_value ("true ");
  dsk_assert (v->type == DSK_JSON_VALUE_BOOLEAN);
  dsk_assert (v->v_boolean.value == DSK_TRUE);
  dsk_json_value_free (v);

  v = parse_value ("false");
  dsk_assert (v->type == DSK_JSON_VALUE_BOOLEAN);
  dsk_assert (v->v_boolean.value == DSK_FALSE);
  dsk_json_value_free (v);

  v = parse_value ("false ");
  dsk_assert (v->type == DSK_JSON_VALUE_BOOLEAN);
  dsk_assert (v->v_boolean.value == DSK_FALSE);
  dsk_json_value_free (v);

  v = parse_value ("null");
  dsk_assert (v->type == DSK_JSON_VALUE_NULL);
  dsk_json_value_free (v);

  v = parse_value ("null ");
  dsk_assert (v->type == DSK_JSON_VALUE_NULL);
  dsk_json_value_free (v);

  v = parse_value ("42 ");
  dsk_assert (v->type == DSK_JSON_VALUE_NUMBER);
  dsk_assert (v->v_number.value == 42.0);
  dsk_json_value_free (v);

  v = parse_value ("42.0");
  dsk_assert (v->type == DSK_JSON_VALUE_NUMBER);
  dsk_assert (v->v_number.value == 42.0);
  dsk_json_value_free (v);

  v = parse_value ("1e3");
  dsk_assert (v->type == DSK_JSON_VALUE_NUMBER);
  dsk_assert (v->v_number.value == 1000.0);
  dsk_json_value_free (v);

  v = parse_value ("{}");
  dsk_assert (v->type == DSK_JSON_VALUE_OBJECT);
  dsk_assert (v->v_object.n_members == 0);
  dsk_json_value_free (v);

  v = parse_value ("{ \"a\": 1 }");
  dsk_assert (v->type == DSK_JSON_VALUE_OBJECT);
  dsk_assert (v->v_object.n_members == 1);
  dsk_assert (strcmp (v->v_object.members[0].name, "a") == 0);
  dsk_assert (v->v_object.members[0].value->type == DSK_JSON_VALUE_NUMBER);
  dsk_assert (v->v_object.members[0].value->v_number.value == 1.0);
  dsk_json_value_free (v);

  v = parse_value ("{ \"a\": 1, \"bbb\": false }");
  dsk_assert (v->type == DSK_JSON_VALUE_OBJECT);
  dsk_assert (v->v_object.n_members == 2);
  dsk_assert (strcmp (v->v_object.members[0].name, "a") == 0);
  dsk_assert (v->v_object.members[0].value->type == DSK_JSON_VALUE_NUMBER);
  dsk_assert (v->v_object.members[0].value->v_number.value == 1.0);
  dsk_assert (strcmp (v->v_object.members[1].name, "bbb") == 0);
  dsk_assert (v->v_object.members[1].value->type == DSK_JSON_VALUE_BOOLEAN);
  dsk_assert (v->v_object.members[1].value->v_boolean.value == DSK_FALSE);
  dsk_json_value_free (v);

  v = parse_value ("[]");
  dsk_assert (v->type == DSK_JSON_VALUE_ARRAY);
  dsk_assert (v->v_array.n_values == 0);
  dsk_json_value_free (v);

  v = parse_value ("[\"a\",false,666]");
  dsk_assert (v->type == DSK_JSON_VALUE_ARRAY);
  dsk_assert (v->v_array.n_values == 3);
  dsk_assert (v->v_array.values[0]->type == DSK_JSON_VALUE_STRING);
  dsk_assert (v->v_array.values[0]->v_string.length == 1);
  dsk_assert (strcmp (v->v_array.values[0]->v_string.str, "a") == 0);
  dsk_assert (v->v_array.values[1]->type == DSK_JSON_VALUE_BOOLEAN);
  dsk_assert (v->v_array.values[1]->v_boolean.value == DSK_FALSE);
  dsk_assert (v->v_array.values[2]->type == DSK_JSON_VALUE_NUMBER);
  dsk_assert (v->v_array.values[2]->v_number.value == 666);
  dsk_json_value_free (v);

  v = parse_value ("[{\"a\":1}, [1,2,4,9]]");
  dsk_assert (v->type == DSK_JSON_VALUE_ARRAY);
  dsk_assert (v->v_array.n_values == 2);
  dsk_assert (v->v_array.values[0]->type == DSK_JSON_VALUE_OBJECT);
  dsk_assert (v->v_array.values[0]->v_object.n_members == 1);
  dsk_assert (strcmp (v->v_array.values[0]->v_object.members[0].name, "a") == 0);
  dsk_assert (v->v_array.values[0]->v_object.members[0].value->type == DSK_JSON_VALUE_NUMBER);
  dsk_assert (v->v_array.values[0]->v_object.members[0].value->v_number.value == 1);
  dsk_assert (v->v_array.values[1]->type == DSK_JSON_VALUE_ARRAY);
  dsk_assert (v->v_array.values[1]->v_array.n_values == 4);
  dsk_assert (v->v_array.values[1]->v_array.values[0]->type == DSK_JSON_VALUE_NUMBER);
  dsk_assert (v->v_array.values[1]->v_array.values[0]->v_number.value == 1);
  dsk_assert (v->v_array.values[1]->v_array.values[1]->type == DSK_JSON_VALUE_NUMBER);
  dsk_assert (v->v_array.values[1]->v_array.values[1]->v_number.value == 2);
  dsk_assert (v->v_array.values[1]->v_array.values[2]->type == DSK_JSON_VALUE_NUMBER);
  dsk_assert (v->v_array.values[1]->v_array.values[2]->v_number.value == 4);
  dsk_assert (v->v_array.values[1]->v_array.values[3]->type == DSK_JSON_VALUE_NUMBER);
  dsk_assert (v->v_array.values[1]->v_array.values[3]->v_number.value == 9);
  dsk_json_value_free (v);

  v = parse_value ("{\"b\":123, \"a\":\"foo\", \"c\":42.0}");
  dsk_assert (v->type == DSK_JSON_VALUE_OBJECT);
  mem = dsk_json_object_get_member (v, "a");
  dsk_assert (mem != NULL);
  dsk_assert (mem->value->type == DSK_JSON_VALUE_STRING);
  dsk_assert (strcmp (mem->value->v_string.str, "foo") == 0);
  mem = dsk_json_object_get_member (v, "b");
  dsk_assert (mem != NULL);
  dsk_assert (mem->value->type == DSK_JSON_VALUE_NUMBER);
  dsk_assert (mem->value->v_number.value == 123);
  mem = dsk_json_object_get_member (v, "c");
  dsk_assert (mem != NULL);
  dsk_assert (mem->value->type == DSK_JSON_VALUE_NUMBER);
  dsk_assert (mem->value->v_number.value == 42);
  dsk_json_value_free (v);

  v = parse_value ("\"\\u0001\"");
  dsk_assert (v->type == DSK_JSON_VALUE_STRING);
  dsk_assert (v->v_string.length == 1);
  dsk_assert (strcmp (v->v_string.str, "\1") == 0);
  str = print_value (v);
  dsk_assert (strcmp (str, "\"\\u0001\"") == 0);
  dsk_free (str);
  dsk_json_value_free (v);

  // Test surrogate pair.
  //  unicode points:     1         x10dc01     2
  //  utf8, hex           01        f4 8d b0 81  02
  v = parse_value ("\"\\u0001\"\\udbff\\udc01\\u0002\"");
  dsk_assert (v->type == DSK_JSON_VALUE_STRING);
  dsk_assert (v->v_string.length == 6);
  dsk_assert (strcmp (v->v_string.str, "\x01\xf4\x8d\xb0\x81\x02") == 0);
  str = print_value (v);
  //dsk_assert (strcmp (str, "\"\\u0001\"") == 0);
  printf("utf16 surrogate re-encoded is %s\n", str);
  dsk_free (str);
  dsk_json_value_free (v);

  // Isolated surrogate pair parts do not parse.
  parse_fail ("\\udbff");
  parse_fail ("\\udc01");

  dsk_cleanup ();
  
  return 0;
}
