typedef struct _DskJsonMember DskJsonMember;
typedef union _DskJsonValue DskJsonValue;
typedef struct _DskJsonParser DskJsonParser;

typedef struct _DskJsonValueObject DskJsonValueObject;
typedef struct _DskJsonValueArray DskJsonValueArray;
typedef struct _DskJsonValueString DskJsonValueString;
typedef struct _DskJsonValueNumber DskJsonValueNumber;

typedef enum
{
  DSK_JSON_VALUE_BOOLEAN,
  DSK_JSON_VALUE_NULL,
  DSK_JSON_VALUE_OBJECT,
  DSK_JSON_VALUE_ARRAY,
  DSK_JSON_VALUE_STRING,
  DSK_JSON_VALUE_NUMBER
} DskJsonValueType;
const char *dsk_json_value_type_name (DskJsonValueType type);

typedef struct _DskJsonValueBase DskJsonValueBase;
struct _DskJsonValueBase
{
  DskJsonValueType type;
  dsk_boolean value;
};

typedef struct _DskJsonValueBoolean DskJsonValueBoolean;
struct _DskJsonValueBoolean
{
  DskJsonValueBase base;
  dsk_boolean value;
};
struct _DskJsonValueObject
{
  DskJsonValueBase base;
  unsigned n_members;
  DskJsonMember *members;
  DskJsonMember **members_sorted_by_name; /* private */
};
struct _DskJsonValueArray
{
  DskJsonValueBase base;
  unsigned n_values;
  DskJsonValue **values;
};
struct _DskJsonValueString
{
  DskJsonValueBase base;
  unsigned length;            /* in bytes (!) */
  char *str;
};
struct _DskJsonValueNumber
{
  DskJsonValueBase base;
  double value;
};

union _DskJsonValue
{
  DskJsonValueType type;
  DskJsonValueBase base;
  DskJsonValueBoolean v_boolean;
  DskJsonValueObject v_object;
  DskJsonValueArray v_array;
  DskJsonValueString v_string;
  DskJsonValueNumber v_number;
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

DskJsonValue * dsk_json_parse           (size_t         len,
                                         const uint8_t *data,
                                         DskError     **error);
DskJsonValue * dsk_json_parse_file      (const char    *filename,
                                         DskError     **error);

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
                                         const char    *str);
DskJsonValue *dsk_json_value_new_number (double         value);
DskJsonValue *dsk_json_value_copy       (const DskJsonValue *value);

void          dsk_json_value_free       (DskJsonValue  *value);

DskJsonValue *dsk_json_object_get_value (DskJsonValue  *object,
                                         const char    *name);
DskJsonMember*dsk_json_object_get_member(DskJsonValue  *object,
                                         const char    *name);

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


/* --- parsing internals --- */
char   *dsk_json_string_dequote   (const char          *init_quote,
                                   const char         **past_end_quote_out,
                                   unsigned            *rv_len_out,
                                   DskError           **error);
