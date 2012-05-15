#include <string.h>
#include "dsk.h"

/* --- encoder --- */
typedef struct _DskJsonPrettierClass DskJsonPrettierClass;
struct _DskJsonPrettierClass
{
  DskOctetFilterClass base_class;
};
typedef struct _DskJsonPrettier DskJsonPrettier;
struct _DskJsonPrettier
{
  DskOctetFilter base_instance;
  DskJsonParser *parser;
};



static void dsk_json_prettier_init (DskJsonPrettier *prettier)
{
  prettier->parser = dsk_json_parser_new ();
}
static void dsk_json_prettier_finalize (DskJsonPrettier *prettier)
{
  dsk_json_parser_destroy (prettier->parser);
}

static void flush_parser (DskJsonParser *parser,
                          DskBuffer     *out)
{
  DskJsonValue *json;
  while ((json = dsk_json_parser_pop (parser)) != NULL)
    {
      dsk_json_value_to_buffer (json, 0, out);
      dsk_buffer_append_byte (out, '\n');
      dsk_json_value_free (json);
    }
}

static dsk_boolean
dsk_json_prettier_process  (DskOctetFilter *filter,
                            DskBuffer      *out,
                            unsigned        in_length,
                            const uint8_t  *in_data,
                            DskError      **error)
{
  DskJsonPrettier *prettier = (DskJsonPrettier *) filter;
  if (!dsk_json_parser_feed (prettier->parser, in_length, in_data, error))
    return DSK_FALSE;
  flush_parser (prettier->parser, out);
  return DSK_TRUE;
}
static dsk_boolean
dsk_json_prettier_finish(DskOctetFilter *filter,
                         DskBuffer      *out,
                         DskError      **error)
{
  DskJsonPrettier *prettier = (DskJsonPrettier *) filter;
  if (!dsk_json_parser_finish (prettier->parser, error))
    return DSK_FALSE;
  flush_parser (prettier->parser, out);
  return DSK_TRUE;
}

DSK_OCTET_FILTER_SUBCLASS_DEFINE(static, DskJsonPrettier, dsk_json_prettier);


DskOctetFilter *dsk_json_prettier_new (void)
{
  DskJsonPrettier *rv = dsk_object_new (&dsk_json_prettier_class);
  return DSK_OCTET_FILTER (rv);
}
