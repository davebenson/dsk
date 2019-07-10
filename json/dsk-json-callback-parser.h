/* Sort-of a SAX-type parser for JSON.
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct DskJsonCallbackParser DskJsonCallbackParser;

#define DSK_JSON_CALLBACK_PARSER_DEFAULT_MAX_DEPTH 64

typedef struct DskJsonCallbackParserOptions DskJsonCallbackParserOptions;
struct DskJsonCallbackParserOptions
{
  unsigned max_stack_depth;

  // These flags are chosen so that 0 is the strict/standard JSON interpretation.
  unsigned ignore_utf8_errors : 1;              // UNIMPL
  unsigned ignore_utf16_errors : 1;             // UNIMPL
  unsigned ignore_utf8_surrogate_pairs : 1;     // UNIMPL
  unsigned permit_backslash_x : 1;
  unsigned permit_backslash_0 : 1;
  unsigned permit_trailing_commas : 1;
  unsigned ignore_single_line_comments : 1;
  unsigned ignore_multi_line_comments : 1;
  unsigned ignore_multiple_commas : 1;
  unsigned ignore_missing_commas : 1;
  unsigned permit_bare_fieldnames : 1;
  unsigned permit_single_quote_strings : 1;
  unsigned permit_leading_decimal_point : 1;
  unsigned permit_trailing_decimal_point : 1;
  unsigned permit_hex_numbers : 1;
  unsigned permit_octal_numbers : 1;
  unsigned disallow_extra_whitespace : 1;       // brutal non-conformant optimization (NOT IMPL)
  unsigned ignore_unicode_whitespace : 1;
  unsigned permit_line_continuations_in_strings : 1;

  // These values describe the encapsulation of the JSON records
  unsigned permit_bare_values : 1;
  unsigned permit_array_values : 1;
  unsigned permit_toplevel_commas : 1;
  unsigned require_toplevel_commas : 1;

  // Used for error information.
  unsigned start_line_number;
};

extern DskJsonCallbackParserOptions json_callback_parser_options_json;
extern DskJsonCallbackParserOptions json_callback_parser_options_json5;
typedef enum
{
  DSK_JSON_CALLBACK_PARSER_ERROR_NONE,

  DSK_JSON_CALLBACK_PARSER_ERROR_EXPECTED_MEMBER_NAME,
  DSK_JSON_CALLBACK_PARSER_ERROR_EXPECTED_COLON,
  DSK_JSON_CALLBACK_PARSER_ERROR_EXPECTED_VALUE,
  DSK_JSON_CALLBACK_PARSER_ERROR_EXPECTED_COMMA_OR_RBRACKET,
  DSK_JSON_CALLBACK_PARSER_ERROR_EXPECTED_COMMA_OR_RBRACE,
  DSK_JSON_CALLBACK_PARSER_ERROR_EXPECTED_HEX_DIGIT,
  DSK_JSON_CALLBACK_PARSER_ERROR_EXPECTED_STRUCTURED_VALUE,
  DSK_JSON_CALLBACK_PARSER_ERROR_EXPECTED_COMMA,
  DSK_JSON_CALLBACK_PARSER_ERROR_EXTRA_COMMA,
  DSK_JSON_CALLBACK_PARSER_ERROR_TRAILING_COMMA,
  DSK_JSON_CALLBACK_PARSER_ERROR_SINGLE_QUOTED_STRING_NOT_ALLOWED,
  DSK_JSON_CALLBACK_PARSER_ERROR_STACK_DEPTH_EXCEEDED,
  DSK_JSON_CALLBACK_PARSER_ERROR_UNEXPECTED_CHAR,
  DSK_JSON_CALLBACK_PARSER_ERROR_BAD_BAREWORD,
  DSK_JSON_CALLBACK_PARSER_ERROR_PARTIAL_RECORD,          // occurs at unexpected EOF

  // errors that can occur while validating a string.
  DSK_JSON_CALLBACK_PARSER_ERROR_UTF8_OVERLONG,
  DSK_JSON_CALLBACK_PARSER_ERROR_UTF8_BAD_INITIAL_BYTE,
  DSK_JSON_CALLBACK_PARSER_ERROR_UTF8_BAD_TRAILING_BYTE,
  DSK_JSON_CALLBACK_PARSER_ERROR_UTF16_BAD_SURROGATE_PAIR,
  DSK_JSON_CALLBACK_PARSER_ERROR_STRING_CONTROL_CHARACTER,
  DSK_JSON_CALLBACK_PARSER_ERROR_QUOTED_NEWLINE,
  DSK_JSON_CALLBACK_PARSER_ERROR_DIGIT_NOT_ALLOWED_AFTER_NUL,
  DSK_JSON_CALLBACK_PARSER_ERROR_BACKSLASH_X_NOT_ALLOWED,

  // errors that can occur while validating a number
  DSK_JSON_CALLBACK_PARSER_ERROR_HEX_NOT_ALLOWED,
  DSK_JSON_CALLBACK_PARSER_ERROR_OCTAL_NOT_ALLOWED,
  DSK_JSON_CALLBACK_PARSER_ERROR_NON_OCTAL_DIGIT,
  DSK_JSON_CALLBACK_PARSER_ERROR_LEADING_DECIMAL_POINT_NOT_ALLOWED,
  DSK_JSON_CALLBACK_PARSER_ERROR_TRAILING_DECIMAL_POINT_NOT_ALLOWED,
  DSK_JSON_CALLBACK_PARSER_ERROR_BAD_NUMBER,

  DSK_JSON_CALLBACK_PARSER_ERROR_INTERNAL,

} DskJsonCallbackParserError;

typedef struct {
  DskJsonCallbackParserError code;
  const char *code_str;
  uint64_t line_no;
  uint64_t byte_no;
  const char *message;
  const char *message2;         /// may be NULL
} DskJsonCallbackParserErrorInfo;

//---------------------------------------------------------------------
//                           Functions
//---------------------------------------------------------------------
typedef struct DskJsonCallbacks DskJsonCallbacks;
struct DskJsonCallbacks {
  bool (*start_object)  (void *callback_data);
  bool (*end_object)    (void *callback_data);
  bool (*start_array)   (void *callback_data);
  bool (*end_array)     (void *callback_data);
  bool (*object_key)    (unsigned key_length,
                         const char *key,
                         void *callback_data);
  bool (*number_value)  (unsigned number_length,
                         const char *number,
                         void *callback_data);
  // TODO: partial_string_value optional callback
  bool (*string_value)  (unsigned string_length,
                         const char *str,
                         void *callback_data);
  bool (*boolean_value) (int boolean_value,
                       void *callback_data);
  bool (*null_value)    (void *callback_data);

  void (*error)         (const DskJsonCallbackParserErrorInfo *error,
                         void *callback_data);

  void (*destroy)       (void *callback_data);
};
#define JSON_CALLBACKS_DEF(prefix, suffix) \
  {                                        \
    prefix ## start_object ## suffix,      \
    prefix ## end_object   ## suffix,      \
    prefix ## start_array  ## suffix,      \
    prefix ## end_array    ## suffix,      \
    prefix ## object_key   ## suffix,      \
    prefix ## number_value ## suffix,      \
    prefix ## string_value ## suffix,      \
    prefix ## boolean_value## suffix,      \
    prefix ## null_value   ## suffix,      \
    prefix ## error        ## suffix,      \
    prefix ## destroy      ## suffix       \
  }
// note thtat DskJsonCallbackParser allocates no further memory, so there
DskJsonCallbackParser *
dsk_json_callback_parser_new (const DskJsonCallbacks *callbacks,
                              void *callback_data,
                              const DskJsonCallbackParserOptions *options);
 


bool
dsk_json_callback_parser_feed (DskJsonCallbackParser *callback_parser,
                               size_t          len,
                               const uint8_t  *data);


bool
dsk_json_callback_parser_end_feed (DskJsonCallbackParser *callback_parser);

// Reset all unprocessed input, errors etc,
// but leave configuration as is.
void
dsk_json_callback_parser_reset (DskJsonCallbackParser *parser);

void
dsk_json_callback_parser_destroy (DskJsonCallbackParser *callback_parser);


// long-winded option definitions
#define DSK_JSON_CALLBACK_PARSER_OPTIONS_INIT                 \
(DskJsonCallbackParserOptions) {                              \
  .max_stack_depth = JSON_CALLBACK_PARSER_DEFAULT_MAX_DEPTH,  \
  .ignore_utf8_errors = 0,                                    \
  .ignore_utf16_errors = 0,                                   \
  .ignore_utf8_surrogate_pairs = 0,                           \
  .permit_backslash_x = 0,                                    \
  .permit_backslash_0 = 0,                                    \
  .permit_trailing_commas = 0,                                \
  .ignore_single_line_comments = 0,                           \
  .ignore_multi_line_comments = 0,                            \
  .ignore_multiple_commas = 0,                                \
  .ignore_missing_commas = 0,                                 \
  .permit_bare_fieldnames = 0,                                \
  .permit_single_quote_strings = 0,                           \
  .permit_leading_decimal_point = 0,                          \
  .permit_trailing_decimal_point = 0,                         \
  .permit_hex_numbers = 0,                                    \
  .permit_octal_numbers = 0,                                  \
  .disallow_extra_whitespace = 0,                             \
  .ignore_unicode_whitespace = 0,                             \
  .permit_line_continuations_in_strings = 0,                  \
  .permit_bare_values = 0,                                    \
  .permit_array_values = 0,                                   \
  .permit_toplevel_commas = 0,                                \
  .require_toplevel_commas = 0,                               \
  .start_line_number = 1,                                     \
}

#define DSK_JSON_CALLBACK_PARSER_OPTIONS_INIT_JSON5           \
(DskJsonCallbackParserOptions) {                              \
  .max_stack_depth = JSON_CALLBACK_PARSER_DEFAULT_MAX_DEPTH,  \
  .ignore_utf8_errors = 0,                                    \
  .ignore_utf16_errors = 0,                                   \
  .ignore_utf8_surrogate_pairs = 0,                           \
  .permit_backslash_x = 1,                                    \
  .permit_backslash_0 = 1,                                    \
  .permit_trailing_commas = 1,                                \
  .ignore_single_line_comments = 1,                           \
  .ignore_multi_line_comments = 1,                            \
  .ignore_multiple_commas = 0,                                \
  .ignore_missing_commas = 0,                                 \
  .permit_bare_fieldnames = 1,                                \
  .permit_single_quote_strings = 1,                           \
  .permit_leading_decimal_point = 1,                          \
  .permit_trailing_decimal_point = 1,                         \
  .permit_hex_numbers = 1,                                    \
  .permit_octal_numbers = 1,                                  \
  .disallow_extra_whitespace = 0,                             \
  .ignore_unicode_whitespace = 1,                             \
  .permit_line_continuations_in_strings = 1,                  \
  .permit_bare_values = 0,                                    \
  .permit_array_values = 0,                                   \
  .permit_toplevel_commas = 0,                                \
  .require_toplevel_commas = 0,                               \
  .start_line_number = 1,                                     \
}
