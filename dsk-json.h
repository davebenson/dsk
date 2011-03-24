typedef struct _DskJsonMember DskJsonMember;
typedef struct _DskJsonValue DskJsonValue;
typedef struct _DskJsonParser DskJsonParser;

typedef enum
{
  DSK_JSON_VALUE_BOOLEAN,
  DSK_JSON_VALUE_NULL,
  DSK_JSON_VALUE_OBJECT,
  DSK_JSON_VALUE_ARRAY,
  DSK_JSON_VALUE_STRING,
  DSK_JSON_VALUE_NUMBER
} DskJsonValueType;

struct _DskJsonValue
{
  DskJsonValueType type;
  union {
    dsk_boolean v_boolean;
    struct {
      unsigned n_members;
      DskJsonMember *members;
    } v_object;
    struct {
      unsigned n_values;
      DskJsonValue **values;
    } v_array;
    struct {
      unsigned length;            /* in bytes (!) */
      char *str;
    } v_string;
    double v_number;
  } value;
};

struct _DskJsonMember
{
  char *name;
  DskJsonValue *value;
};

/* --- parser --- */
DskJsonParser *dsk_json_parser_new      (void);
dsk_boolean    dsk_json_parser_feed     (DskJsonParser *parser,
                                         size_t         len,
                                         const uint8_t *data,
                                         DskError     **error);
DskJsonValue * dsk_json_parser_pop      (DskJsonParser *parser);
dsk_boolean    dsk_json_parser_finish   (DskJsonParser *parser,
                                         DskError     **error);
void           dsk_json_parser_destroy  (DskJsonParser *parser);

/* --- values --- */
DskJsonValue *dsk_json_value_new_null   (void);
DskJsonValue *dsk_json_value_new_boolean(dsk_boolean    value);
/* takes ownership of subvalues, but not of the members array */
DskJsonValue *dsk_json_value_new_object (unsigned       n_members,
                                         DskJsonMember *members);
/* takes ownership of subvalues, but not of the values array */
DskJsonValue *dsk_json_value_new_array  (unsigned       n_values,
                                         DskJsonValue **values);
DskJsonValue *dsk_json_value_new_string (unsigned       n_bytes,
                                         char          *str);
DskJsonValue *dsk_json_value_new_number (double         value);

void          dsk_json_value_free       (DskJsonValue  *value);

/* --- writing --- */

/* generalized target for output */
typedef void (*DskJsonAppendFunc) (unsigned      length,
                                   const void   *data,
                                   void         *append_data);

/* if indent == -1, no decorative spaces are added */
void    dsk_json_value_serialize  (const DskJsonValue  *value,
                                   int                  indent,
                                   DskJsonAppendFunc    append_func,
                                   void                *append_data);
void    dsk_json_value_to_buffer  (const DskJsonValue  *value,
                                   int                  indent,
                                   DskBuffer           *out);
