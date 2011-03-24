#include "../dsk.h"
#include <string.h>
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

int main(int argc, char **argv)
{
  DskJsonValue *v;

  dsk_cmdline_init ("test JSON handling",
                    "Test JSON parsing",
                    NULL, 0);
  dsk_cmdline_add_boolean ("verbose", "extra logging", NULL, 0,
                           &cmdline_verbose);
  dsk_cmdline_process_args (&argc, &argv);

  v = parse_value ("true");
  dsk_assert (v->type == DSK_JSON_VALUE_BOOLEAN);
  dsk_assert (v->value.v_boolean == DSK_TRUE);
  dsk_json_value_free (v);

  v = parse_value ("true ");
  dsk_assert (v->type == DSK_JSON_VALUE_BOOLEAN);
  dsk_assert (v->value.v_boolean == DSK_TRUE);
  dsk_json_value_free (v);

  v = parse_value ("false");
  dsk_assert (v->type == DSK_JSON_VALUE_BOOLEAN);
  dsk_assert (v->value.v_boolean == DSK_FALSE);
  dsk_json_value_free (v);

  v = parse_value ("false ");
  dsk_assert (v->type == DSK_JSON_VALUE_BOOLEAN);
  dsk_assert (v->value.v_boolean == DSK_FALSE);
  dsk_json_value_free (v);

  v = parse_value ("null");
  dsk_assert (v->type == DSK_JSON_VALUE_NULL);
  dsk_json_value_free (v);

  v = parse_value ("null ");
  dsk_assert (v->type == DSK_JSON_VALUE_NULL);
  dsk_json_value_free (v);

  v = parse_value ("42 ");
  dsk_assert (v->type == DSK_JSON_VALUE_NUMBER);
  dsk_assert (v->value.v_number == 42.0);
  dsk_json_value_free (v);

  v = parse_value ("42.0");
  dsk_assert (v->type == DSK_JSON_VALUE_NUMBER);
  dsk_assert (v->value.v_number == 42.0);
  dsk_json_value_free (v);

  v = parse_value ("1e3");
  dsk_assert (v->type == DSK_JSON_VALUE_NUMBER);
  dsk_assert (v->value.v_number == 1000.0);
  dsk_json_value_free (v);

  v = parse_value ("{}");
  dsk_assert (v->type == DSK_JSON_VALUE_OBJECT);
  dsk_assert (v->value.v_object.n_members == 0);
  dsk_json_value_free (v);

  v = parse_value ("{ \"a\": 1 }");
  dsk_assert (v->type == DSK_JSON_VALUE_OBJECT);
  dsk_assert (v->value.v_object.n_members == 1);
  dsk_assert (strcmp (v->value.v_object.members[0].name, "a") == 0);
  dsk_assert (v->value.v_object.members[0].value->type == DSK_JSON_VALUE_NUMBER);
  dsk_assert (v->value.v_object.members[0].value->value.v_number == 1.0);
  dsk_json_value_free (v);

  v = parse_value ("{ \"a\": 1, \"bbb\": false }");
  dsk_assert (v->type == DSK_JSON_VALUE_OBJECT);
  dsk_assert (v->value.v_object.n_members == 2);
  dsk_assert (strcmp (v->value.v_object.members[0].name, "a") == 0);
  dsk_assert (v->value.v_object.members[0].value->type == DSK_JSON_VALUE_NUMBER);
  dsk_assert (v->value.v_object.members[0].value->value.v_number == 1.0);
  dsk_assert (strcmp (v->value.v_object.members[1].name, "bbb") == 0);
  dsk_assert (v->value.v_object.members[1].value->type == DSK_JSON_VALUE_BOOLEAN);
  dsk_assert (v->value.v_object.members[1].value->value.v_boolean == DSK_FALSE);
  dsk_json_value_free (v);

  v = parse_value ("[]");
  dsk_assert (v->type == DSK_JSON_VALUE_ARRAY);
  dsk_assert (v->value.v_array.n_values == 0);
  dsk_json_value_free (v);

  v = parse_value ("[\"a\",false,666]");
  dsk_assert (v->type == DSK_JSON_VALUE_ARRAY);
  dsk_assert (v->value.v_array.n_values == 3);
  dsk_assert (v->value.v_array.values[0]->type == DSK_JSON_VALUE_STRING);
  dsk_assert (v->value.v_array.values[0]->value.v_string.length == 1);
  dsk_assert (strcmp (v->value.v_array.values[0]->value.v_string.str, "a") == 0);
  dsk_assert (v->value.v_array.values[1]->type == DSK_JSON_VALUE_BOOLEAN);
  dsk_assert (v->value.v_array.values[1]->value.v_boolean == DSK_FALSE);
  dsk_assert (v->value.v_array.values[2]->type == DSK_JSON_VALUE_NUMBER);
  dsk_assert (v->value.v_array.values[2]->value.v_number == 666);
  dsk_json_value_free (v);

  v = parse_value ("[{\"a\":1}, [1,2,4,9]]");
  dsk_assert (v->type == DSK_JSON_VALUE_ARRAY);
  dsk_assert (v->value.v_array.n_values == 2);
  dsk_assert (v->value.v_array.values[0]->type == DSK_JSON_VALUE_OBJECT);
  dsk_assert (v->value.v_array.values[0]->value.v_object.n_members == 1);
  dsk_assert (strcmp (v->value.v_array.values[0]->value.v_object.members[0].name, "a") == 0);
  dsk_assert (v->value.v_array.values[0]->value.v_object.members[0].value->type == DSK_JSON_VALUE_NUMBER);
  dsk_assert (v->value.v_array.values[0]->value.v_object.members[0].value->value.v_number == 1);
  dsk_assert (v->value.v_array.values[1]->type == DSK_JSON_VALUE_ARRAY);
  dsk_assert (v->value.v_array.values[1]->value.v_array.n_values == 4);
  dsk_assert (v->value.v_array.values[1]->value.v_array.values[0]->type == DSK_JSON_VALUE_NUMBER);
  dsk_assert (v->value.v_array.values[1]->value.v_array.values[0]->value.v_number == 1);
  dsk_assert (v->value.v_array.values[1]->value.v_array.values[1]->type == DSK_JSON_VALUE_NUMBER);
  dsk_assert (v->value.v_array.values[1]->value.v_array.values[1]->value.v_number == 2);
  dsk_assert (v->value.v_array.values[1]->value.v_array.values[2]->type == DSK_JSON_VALUE_NUMBER);
  dsk_assert (v->value.v_array.values[1]->value.v_array.values[2]->value.v_number == 4);
  dsk_assert (v->value.v_array.values[1]->value.v_array.values[3]->type == DSK_JSON_VALUE_NUMBER);
  dsk_assert (v->value.v_array.values[1]->value.v_array.values[3]->value.v_number == 9);
  dsk_json_value_free (v);

  dsk_cleanup ();
  
  return 0;
}
