#include "../dsk.h"

static dsk_boolean did_bind = DSK_FALSE;
static DskHttpServer *server = NULL;

static void
parse_config (DskJsonValue  *value)
{
  /* match: { constraints },
     action:  { "serve_file": { base_dir:"...",
                                ... } },
     children:  [ ... ]
  */
}

static DSK_CMDLINE_CALLBACK_DECLARE(handle_config_file)
{
  DskJsonValue *value = dsk_json_parse_file (arg_value, error);
  if (value == NULL)
    return DSK_FALSE;
  parse_config (value);
  dsk_json_value_free (value);
  return DSK_TRUE;
}

int main(int argc, char **argv)
{
  dsk_cmdline_init ("generic/sample HTTP server",
                    "This is a demo of DskHttpServer, as well\n"
                    "a testing utility for seeing how browsers\n"
                    "interact with various features.\n",
                    NULL, 0);
  dsk_cmdline_add_func ("config", "JSON configuration file", "FILE",
                        DSK_CMDLINE_MANDATORY|DSK_CMDLINE_REPEATABLE,
                        handle_config_file, NULL);
  server = dsk_http_server_new ();
  dsk_cmdline_process_args (&argc, &argv);
  if (!did_bind)
    dsk_die ("server did not bind to any ports: configuration error");
  return dsk_main_run ();
}
