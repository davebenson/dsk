#include "json-cb-parser.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#if 0
#include <stdarg.h>
# define DEBUG_CODE(code)  code
static void debug_printf(const char *format, ...)
{
  va_list args;
  va_start (args, format);
  vfprintf(stderr, format, args);
  fprintf(stderr, "\n");
  va_end (args);
}
# define DEBUG_PRINTF(args)   debug_printf args
#else
# define DEBUG_CODE(code)
# define DEBUG_PRINTF(args)  
#endif

// Names of each byte.  See byte_name() for the 'public' interface.
static const char byte_name_str[] =
"NUL\0" "^A\0" "^B\0" "^C\0" "^D\0" "^E\0" "^F\0" "^G\0" "^H\0" "TAB\0"   /* 0 .. 9 */
"NEWLINE\0" "^K\0" "^L\0" "CARRIAGE-RETURN\0" "^N\0" "^O\0" "^P\0" "^Q\0" "^R\0" "^S\0"   /* 10 .. 19 */
"^T\0" "^U\0" "^V\0" "^W\0" "^X\0" "^Y\0" "^Z\0" "0x1b\0" "0x1c\0" "0x1d\0"   /* 20 .. 29 */
"0x1e\0" "0x1f\0" "SPACE\0" "'!'\0" "'\"'\0" "'#'\0" "'$'\0" "'%'\0" "'&'\0" "'\\''\0"   /* 30 .. 39 */
"'('\0" "')'\0" "'*'\0" "'+'\0" "','\0" "'-'\0" "'.'\0" "'/'\0" "'0'\0" "'1'\0"   /* 40 .. 49 */
"'2'\0" "'3'\0" "'4'\0" "'5'\0" "'6'\0" "'7'\0" "'8'\0" "'9'\0" "':'\0" "';'\0"   /* 50 .. 59 */
"'<'\0" "'='\0" "'>'\0" "'?'\0" "'@'\0" "'A'\0" "'B'\0" "'C'\0" "'D'\0" "'E'\0"   /* 60 .. 69 */
"'F'\0" "'G'\0" "'H'\0" "'I'\0" "'J'\0" "'K'\0" "'L'\0" "'M'\0" "'N'\0" "'O'\0"   /* 70 .. 79 */
"'P'\0" "'Q'\0" "'R'\0" "'S'\0" "'T'\0" "'U'\0" "'V'\0" "'W'\0" "'X'\0" "'Y'\0"   /* 80 .. 89 */
"'Z'\0" "'['\0" "'\\'\0" "']'\0" "'^'\0" "'_'\0" "'`'\0" "'a'\0" "'b'\0" "'c'\0"   /* 90 .. 99 */
"'d'\0" "'e'\0" "'f'\0" "'g'\0" "'h'\0" "'i'\0" "'j'\0" "'k'\0" "'l'\0" "'m'\0"   /* 100 .. 109 */
"'n'\0" "'o'\0" "'p'\0" "'q'\0" "'r'\0" "'s'\0" "'t'\0" "'u'\0" "'v'\0" "'w'\0"   /* 110 .. 119 */
"'x'\0" "'y'\0" "'z'\0" "'{'\0" "'|'\0" "'}'\0" "'~'\0" "DEL\0" "0x80\0" "0x81\0"   /* 120 .. 129 */
"0x82\0" "0x83\0" "0x84\0" "0x85\0" "0x86\0" "0x87\0" "0x88\0" "0x89\0" "0x8a\0" "0x8b\0"   /* 130 .. 139 */
"0x8c\0" "0x8d\0" "0x8e\0" "0x8f\0" "0x90\0" "0x91\0" "0x92\0" "0x93\0" "0x94\0" "0x95\0"   /* 140 .. 149 */
"0x96\0" "0x97\0" "0x98\0" "0x99\0" "0x9a\0" "0x9b\0" "0x9c\0" "0x9d\0" "0x9e\0" "0x9f\0"   /* 150 .. 159 */
"0xa0\0" "0xa1\0" "0xa2\0" "0xa3\0" "0xa4\0" "0xa5\0" "0xa6\0" "0xa7\0" "0xa8\0" "0xa9\0"   /* 160 .. 169 */
"0xaa\0" "0xab\0" "0xac\0" "0xad\0" "0xae\0" "0xaf\0" "0xb0\0" "0xb1\0" "0xb2\0" "0xb3\0"   /* 170 .. 179 */
"0xb4\0" "0xb5\0" "0xb6\0" "0xb7\0" "0xb8\0" "0xb9\0" "0xba\0" "0xbb\0" "0xbc\0" "0xbd\0"   /* 180 .. 189 */
"0xbe\0" "0xbf\0" "0xc0\0" "0xc1\0" "0xc2\0" "0xc3\0" "0xc4\0" "0xc5\0" "0xc6\0" "0xc7\0"   /* 190 .. 199 */
"0xc8\0" "0xc9\0" "0xca\0" "0xcb\0" "0xcc\0" "0xcd\0" "0xce\0" "0xcf\0" "0xd0\0" "0xd1\0"   /* 200 .. 209 */
"0xd2\0" "0xd3\0" "0xd4\0" "0xd5\0" "0xd6\0" "0xd7\0" "0xd8\0" "0xd9\0" "0xda\0" "0xdb\0"   /* 210 .. 219 */
"0xdc\0" "0xdd\0" "0xde\0" "0xdf\0" "0xe0\0" "0xe1\0" "0xe2\0" "0xe3\0" "0xe4\0" "0xe5\0"   /* 220 .. 229 */
"0xe6\0" "0xe7\0" "0xe8\0" "0xe9\0" "0xea\0" "0xeb\0" "0xec\0" "0xed\0" "0xee\0" "0xef\0"   /* 230 .. 239 */
"0xf0\0" "0xf1\0" "0xf2\0" "0xf3\0" "0xf4\0" "0xf5\0" "0xf6\0" "0xf7\0" "0xf8\0" "0xf9\0"   /* 240 .. 249 */
"0xfa\0" "0xfb\0" "0xfc\0" "0xfd\0" "0xfe\0" "0xff\0" ;
static const unsigned short byte_name_offsets[256] = {
0,4,7,10,13,16,19,22,25,28,  /* 0.. 9 */
32,40,43,46,62,65,68,71,74,77,  /* 10.. 19 */
80,83,86,89,92,95,98,101,106,111,  /* 20.. 29 */
116,121,126,132,136,140,144,148,152,156,  /* 30.. 39 */
161,165,169,173,177,181,185,189,193,197,  /* 40.. 49 */
201,205,209,213,217,221,225,229,233,237,  /* 50.. 59 */
241,245,249,253,257,261,265,269,273,277,  /* 60.. 69 */
281,285,289,293,297,301,305,309,313,317,  /* 70.. 79 */
321,325,329,333,337,341,345,349,353,357,  /* 80.. 89 */
361,365,369,373,377,381,385,389,393,397,  /* 90.. 99 */
401,405,409,413,417,421,425,429,433,437,  /* 100.. 109 */
441,445,449,453,457,461,465,469,473,477,  /* 110.. 119 */
481,485,489,493,497,501,505,509,513,518,  /* 120.. 129 */
523,528,533,538,543,548,553,558,563,568,  /* 130.. 139 */
573,578,583,588,593,598,603,608,613,618,  /* 140.. 149 */
623,628,633,638,643,648,653,658,663,668,  /* 150.. 159 */
673,678,683,688,693,698,703,708,713,718,  /* 160.. 169 */
723,728,733,738,743,748,753,758,763,768,  /* 170.. 179 */
773,778,783,788,793,798,803,808,813,818,  /* 180.. 189 */
823,828,833,838,843,848,853,858,863,868,  /* 190.. 199 */
873,878,883,888,893,898,903,908,913,918,  /* 200.. 209 */
923,928,933,938,943,948,953,958,963,968,  /* 210.. 219 */
973,978,983,988,993,998,1003,1008,1013,1018,  /* 220.. 229 */
1023,1028,1033,1038,1043,1048,1053,1058,1063,1068,  /* 230.. 239 */
1073,1078,1083,1088,1093,1098,1103,1108,1113,1118,  /* 240.. 249 */
1123,1128,1133,1138,1143,1148,};

#define IS_SPACE(c)  ((c)=='\t' || (c)=='\r' || (c)==' ' || (c)=='\n')
#define IS_HEX_DIGIT(c)     (('0' <= (c) && (c) <= '9')                 \
                          || ('a' <= (c) && (c) <= 'f')                 \
                          || ('A' <= (c) && (c) <= 'F'))
#define IS_OCT_DIGIT(c)    ('0' <= (c) && (c) <= '7')
#define IS_DIGIT(c)    ('0' <= (c) && (c) <= '9')

#define IS_ASCII_ALPHA(c)   (('a' <= (c) && (c) <= 'z')                 \
                          || ('A' <= (c) && (c) <= 'Z'))
#define IS_ALNUM(c)       (('0' <= (c) && (c) <= '0')                   \
                          || IS_ASCII_ALPHA(c))

#define IS_HI_SURROGATE(u) (0xd800 <= (u) && (u) <= (0xd800 + 2047 - 64))
#define IS_LO_SURROGATE(u) (0xdc00 <= (u) && (u) <= (0xdc00 + 1023))

// WARNING: this may return values greater than 1<<20, which is a UTF-16 encoding error.
#define COMBINE_SURROGATES(hi, lo)   (((uint32_t)((hi) - 0xd800) << 10)     \
                                     + (uint32_t)((lo) - 0xdc00) + 0x10000)

typedef enum
{
  JSON_CALLBACK_PARSER_STATE_INTERIM,
  JSON_CALLBACK_PARSER_STATE_INTERIM_EXPECTING_COMMA,
  JSON_CALLBACK_PARSER_STATE_INTERIM_GOT_COMMA,
  JSON_CALLBACK_PARSER_STATE_INTERIM_VALUE,
  JSON_CALLBACK_PARSER_STATE_IN_ARRAY_INITIAL,
  JSON_CALLBACK_PARSER_STATE_IN_ARRAY,
  JSON_CALLBACK_PARSER_STATE_IN_ARRAY_EXPECTING_COMMA,
  JSON_CALLBACK_PARSER_STATE_IN_ARRAY_GOT_COMMA,
  JSON_CALLBACK_PARSER_STATE_IN_ARRAY_VALUE,   // flat_value_state is valid
  JSON_CALLBACK_PARSER_STATE_IN_OBJECT_INITIAL,
  JSON_CALLBACK_PARSER_STATE_IN_OBJECT,
  JSON_CALLBACK_PARSER_STATE_IN_OBJECT_EXPECTING_COLON,
  JSON_CALLBACK_PARSER_STATE_IN_OBJECT_FIELDNAME,     // flat_value_state is valid
  JSON_CALLBACK_PARSER_STATE_IN_OBJECT_BARE_FIELDNAME,
  JSON_CALLBACK_PARSER_STATE_IN_OBJECT_GOT_COLON,
  JSON_CALLBACK_PARSER_STATE_IN_OBJECT_VALUE, // flat_value_state is valid
  JSON_CALLBACK_PARSER_STATE_IN_OBJECT_EXPECTING_COMMA,
} JSON_CallbackParserState;

#define state_is_interim(state) ((state) <= JSON_CALLBACK_PARSER_STATE_INTERIM_EXPECTING_EOL)

// The flat-value-state is used to share the many substates possible for literal values.
// The following states use this substate:
//    JSON_CALLBACK_PARSER_STATE_INTERIM_IN_ARRAY_VALUE
//    JSON_CALLBACK_PARSER_STATE_IN_OBJECT_VALUE_SPACE
typedef enum
{
  // literal strings - parse states
  FLAT_VALUE_STATE_STRING,
#define FLAT_VALUE_STATE__STRING_START FLAT_VALUE_STATE_STRING
  FLAT_VALUE_STATE_IN_BACKSLASH_SEQUENCE,
  FLAT_VALUE_STATE_GOT_HI_SURROGATE,
  FLAT_VALUE_STATE_GOT_HI_SURROGATE_IN_BACKSLASH_SEQUENCE,
  FLAT_VALUE_STATE_GOT_HI_SURROGATE_IN_BACKSLASH_SEQUENCE_U,
  FLAT_VALUE_STATE_IN_UTF8_CHAR,        // parser->utf8_state gives details
  FLAT_VALUE_STATE_GOT_BACKSLASH_0,
  FLAT_VALUE_STATE_IN_BACKSLASH_UTF8,
  FLAT_VALUE_STATE_IN_BACKSLASH_CR,
  FLAT_VALUE_STATE_IN_BACKSLASH_X,
  FLAT_VALUE_STATE_IN_BACKSLASH_U,
#define FLAT_VALUE_STATE__STRING_END FLAT_VALUE_STATE_IN_BACKSLASH_U

  // literal numbers - parse states
  FLAT_VALUE_STATE_GOT_SIGN,
#define FLAT_VALUE_STATE__NUMBER_START FLAT_VALUE_STATE_GOT_SIGN
  FLAT_VALUE_STATE_GOT_0,
  FLAT_VALUE_STATE_IN_DIGITS,
  FLAT_VALUE_STATE_IN_OCTAL,
  FLAT_VALUE_STATE_IN_HEX_EMPTY,
  FLAT_VALUE_STATE_IN_HEX,
  FLAT_VALUE_STATE_GOT_E,
  FLAT_VALUE_STATE_GOT_E_PM,            // must get at least 1 digit
  FLAT_VALUE_STATE_GOT_E_DIGITS,
  FLAT_VALUE_STATE_GOT_LEADING_DECIMAL_POINT,
  FLAT_VALUE_STATE_GOT_DECIMAL_POINT,
  FLAT_VALUE_STATE_GOT_DECIMAL_POINT_DIGITS,
#define FLAT_VALUE_STATE__NUMBER_END FLAT_VALUE_STATE_GOT_DECIMAL_POINT_DIGITS

  // literal barewords - true, false, null
  FLAT_VALUE_STATE_IN_NULL,
  FLAT_VALUE_STATE_IN_TRUE,
  FLAT_VALUE_STATE_IN_FALSE,

} FlatValueState;

static inline bool
flat_value_state_is_string(FlatValueState state)
{
  return FLAT_VALUE_STATE__STRING_START  <= state
      && state <= FLAT_VALUE_STATE__STRING_END;
}
static inline bool
flat_value_state_is_number(FlatValueState state)
{
  return FLAT_VALUE_STATE__NUMBER_START  <= state
      && state <= FLAT_VALUE_STATE__NUMBER_END;
}


typedef enum
{
  WHITESPACE_STATE_DEFAULT,
  WHITESPACE_STATE_SLASH,
  WHITESPACE_STATE_EOL_COMMENT,
  WHITESPACE_STATE_MULTILINE_COMMENT,
  WHITESPACE_STATE_MULTILINE_COMMENT_STAR,
  WHITESPACE_STATE_UTF8_E1,
  WHITESPACE_STATE_UTF8_E2,
  WHITESPACE_STATE_UTF8_E2_80,
  WHITESPACE_STATE_UTF8_E2_81,
  WHITESPACE_STATE_UTF8_E3,
  WHITESPACE_STATE_UTF8_E3_80,
} WhitespaceState;


typedef enum
{
  SCAN_END,
  SCAN_IN_VALUE,
  SCAN_ERROR
} ScanResult;

typedef enum
{
  UTF8_STATE_2_1,
  UTF8_STATE_3_1_got_zero,
  UTF8_STATE_3_1_got_nonzero,
  UTF8_STATE_3_2,
  UTF8_STATE_4_1_got_zero,
  UTF8_STATE_4_1_got_nonzero,
  UTF8_STATE_4_2,
  UTF8_STATE_4_3,
} UTF8State;


typedef ScanResult (*WhitespaceScannerFunc) (JSON_CallbackParser *parser,
                                             const uint8_t **p_at,
                                             const uint8_t  *end);


#define FLAT_VALUE_STATE_REPLACE_ENUM_BITS(in, enum_value) \
  do{                                                      \
    in &= ~FLAT_VALUE_STATE_MASK;                          \
    in |= (enum_value);                                    \
  }while(0)

typedef struct JSON_CallbackParser_StackNode {
  uint8_t is_object : 1;                        // otherwise, it's an array
} JSON_CallbackParser_StackNode;

struct JSON_CallbackParser {
  JSON_CallbackParser_Options options;

  JSON_Callbacks callbacks;
  void *callback_data;

  uint64_t line_no, byte_no;            // 1-based, by tradition

  // NOTE: The stack doesn't include the outside array for ARRAY_OF_OBJECTS
  unsigned stack_depth;
  JSON_CallbackParser_StackNode *stack_nodes;

  JSON_CallbackParserState state;
  JSON_CallbackParserError error_code;
  uint8_t error_byte;

  // this is for strings and numbers; we also use it for quoted object field names.
  FlatValueState flat_value_state;
  char quote_char;

  WhitespaceScannerFunc whitespace_scanner;
  WhitespaceState whitespace_state;

  // Meaning of these two members depends on flat_value_state:
  //    * backslash sequence or UTF8 partial character
  unsigned flat_len;
  char flat_data[8];
  uint16_t hi_surrogate;                // only for GOT_HI_SURROGATE* states

  UTF8State utf8_state;

  size_t buffer_alloced;
  size_t buffer_length;
  char *buffer;
};

static inline void
buffer_set    (JSON_CallbackParser *parser,
               size_t               len,
               const uint8_t       *data)
{
  if (len > parser->buffer_alloced)
    {
      parser->buffer_alloced *= 2;
      while (len > parser->buffer_alloced)
        parser->buffer_alloced *= 2;
      free (parser->buffer);
      parser->buffer = malloc (parser->buffer_alloced);
    }
  memcpy (parser->buffer, data, len);
  parser->buffer_length = len;
}

static inline void
buffer_empty    (JSON_CallbackParser *parser)
{
  parser->buffer_length = 0;
}

static inline void
buffer_append (JSON_CallbackParser *parser,
               size_t               len,
               const uint8_t       *data)
{
  if (parser->buffer_length + len > parser->buffer_alloced)
    {
      parser->buffer_alloced *= 2;
      while (parser->buffer_length + len > parser->buffer_alloced)
        parser->buffer_alloced *= 2;
      parser->buffer = realloc (parser->buffer, parser->buffer_alloced);
    }
  memcpy (parser->buffer + parser->buffer_length, data, len);
  parser->buffer_length += len;
}

static inline void
buffer_append_byte (JSON_CallbackParser *parser, uint8_t c)
{
  if (parser->buffer_length + 1 > parser->buffer_alloced)
    {
      parser->buffer_alloced *= 2;
      parser->buffer = realloc (parser->buffer, parser->buffer_alloced);
    }
  parser->buffer[parser->buffer_length++] = (char) c;
}

static inline const char *
buffer_nul_terminate (JSON_CallbackParser *parser)
{
  if (parser->buffer_length == parser->buffer_alloced)
    {
      parser->buffer_alloced *= 2;
      parser->buffer = realloc (parser->buffer, parser->buffer_alloced);
    }
  parser->buffer[parser->buffer_length] = 0;
  return parser->buffer;
}

static const char *
error_code_to_string (JSON_CallbackParserError code)
{
#define IMPL_CASE(shortname) \
        case JSON_CALLBACK_PARSER_ERROR_##shortname: \
          return #shortname

  switch (code)
    {
    IMPL_CASE(NONE);
    IMPL_CASE(EXPECTED_MEMBER_NAME);
    IMPL_CASE(EXPECTED_COLON);
    IMPL_CASE(EXPECTED_VALUE);
    IMPL_CASE(EXPECTED_COMMA_OR_RBRACKET);
    IMPL_CASE(EXPECTED_COMMA_OR_RBRACE);
    IMPL_CASE(EXPECTED_HEX_DIGIT);
    IMPL_CASE(EXPECTED_STRUCTURED_VALUE);
    IMPL_CASE(EXPECTED_COMMA);
    IMPL_CASE(EXTRA_COMMA);
    IMPL_CASE(TRAILING_COMMA);
    IMPL_CASE(SINGLE_QUOTED_STRING_NOT_ALLOWED);
    IMPL_CASE(STACK_DEPTH_EXCEEDED);
    IMPL_CASE(UNEXPECTED_CHAR);
    IMPL_CASE(BAD_BAREWORD);
    IMPL_CASE(PARTIAL_RECORD);
    IMPL_CASE(UTF8_OVERLONG);
    IMPL_CASE(UTF8_BAD_INITIAL_BYTE);
    IMPL_CASE(UTF8_BAD_TRAILING_BYTE);
    IMPL_CASE(UTF16_BAD_SURROGATE_PAIR);
    IMPL_CASE(STRING_CONTROL_CHARACTER);
    IMPL_CASE(QUOTED_NEWLINE);
    IMPL_CASE(DIGIT_NOT_ALLOWED_AFTER_NUL);
    IMPL_CASE(BACKSLASH_X_NOT_ALLOWED);
    IMPL_CASE(HEX_NOT_ALLOWED);
    IMPL_CASE(OCTAL_NOT_ALLOWED);
    IMPL_CASE(NON_OCTAL_DIGIT);
    IMPL_CASE(LEADING_DECIMAL_POINT_NOT_ALLOWED);
    IMPL_CASE(TRAILING_DECIMAL_POINT_NOT_ALLOWED);
    IMPL_CASE(BAD_NUMBER);
    IMPL_CASE(INTERNAL);
    }
  return NULL;

#undef IMPL_CASE
};
 
static const char *
error_code_to_message (JSON_CallbackParserError code)
{
  switch (code)
    {
    case JSON_CALLBACK_PARSER_ERROR_NONE:
      return "No error";                        // should not happen

    case JSON_CALLBACK_PARSER_ERROR_EXPECTED_MEMBER_NAME:
      return "Expected member name";

    case JSON_CALLBACK_PARSER_ERROR_EXPECTED_COLON:
      return "Expected colon (':')";

    case JSON_CALLBACK_PARSER_ERROR_EXPECTED_VALUE:
      return "Expected value (a number, string, object, array or constant)";

    case JSON_CALLBACK_PARSER_ERROR_EXPECTED_COMMA_OR_RBRACKET:
      return "Expected ',' or ']' in array";

    case JSON_CALLBACK_PARSER_ERROR_EXPECTED_COMMA_OR_RBRACE:
      return "Expected ',' or ']' in object";

    case JSON_CALLBACK_PARSER_ERROR_EXPECTED_HEX_DIGIT:
      return "Expected hexidecimal digit (0-9, a-f, A-F)";

    case JSON_CALLBACK_PARSER_ERROR_EXPECTED_STRUCTURED_VALUE:
      return "Expected object or array (aka a Structured Value) at toplevel";

    case JSON_CALLBACK_PARSER_ERROR_EXPECTED_COMMA:
      return "Expected ','";

    case JSON_CALLBACK_PARSER_ERROR_EXTRA_COMMA:
      return "Got multiple commas (','): not allowed";

    case JSON_CALLBACK_PARSER_ERROR_TRAILING_COMMA:
      return "Got trailing comma (',') at end of object or array";

    case JSON_CALLBACK_PARSER_ERROR_SINGLE_QUOTED_STRING_NOT_ALLOWED:
      return "Single-quoted strings are not allowed";

    case JSON_CALLBACK_PARSER_ERROR_STACK_DEPTH_EXCEEDED:
      return "JSON nested too deeply: stack-depth exceeded";

    case JSON_CALLBACK_PARSER_ERROR_UNEXPECTED_CHAR:
      return "Unexpected character";

    case JSON_CALLBACK_PARSER_ERROR_BAD_BAREWORD:
      return "Unknown bareword value";

    case JSON_CALLBACK_PARSER_ERROR_PARTIAL_RECORD:
      return "Partial record (end-of-file in middle of value)";

    case JSON_CALLBACK_PARSER_ERROR_UTF8_OVERLONG:
      return "Invalid UTF-8 data (overlong)";

    case JSON_CALLBACK_PARSER_ERROR_UTF8_BAD_INITIAL_BYTE:
      return "Bad initial UTF-8 byte";

    case JSON_CALLBACK_PARSER_ERROR_UTF8_BAD_TRAILING_BYTE:
      return "Bad non-initial UTF-8 byte";

    case JSON_CALLBACK_PARSER_ERROR_UTF16_BAD_SURROGATE_PAIR:
      return "Bad UTF-16 Surrogate pair sequence";

    case JSON_CALLBACK_PARSER_ERROR_STRING_CONTROL_CHARACTER:
      return "String contained raw control character";

    case JSON_CALLBACK_PARSER_ERROR_QUOTED_NEWLINE:
      return "Quoted newline not allowed";

    case JSON_CALLBACK_PARSER_ERROR_DIGIT_NOT_ALLOWED_AFTER_NUL:
      return "Digit not allowed after \\0";

    case JSON_CALLBACK_PARSER_ERROR_BACKSLASH_X_NOT_ALLOWED:
      return "Unicode \\x not allowed";

    case JSON_CALLBACK_PARSER_ERROR_HEX_NOT_ALLOWED:
      return "Hexidecimal number not allowed";

    case JSON_CALLBACK_PARSER_ERROR_OCTAL_NOT_ALLOWED:
      return "Octal number not allowed";

    case JSON_CALLBACK_PARSER_ERROR_NON_OCTAL_DIGIT:
      return "Non-octal digit encountered";

    case JSON_CALLBACK_PARSER_ERROR_LEADING_DECIMAL_POINT_NOT_ALLOWED:
      return "Leading decimal point ('.') not allowed";

    case JSON_CALLBACK_PARSER_ERROR_TRAILING_DECIMAL_POINT_NOT_ALLOWED:
      return "Trailing decimal point ('.') not allowed";

    case JSON_CALLBACK_PARSER_ERROR_BAD_NUMBER:
      return "Invalid number";

    case JSON_CALLBACK_PARSER_ERROR_INTERNAL:
      return "JSON Callback Parser Internal error";

    }
  return "Invalid JSON Parser Error Code";
}

// invoking callbacks
static inline bool
do_callback_start_object (JSON_CallbackParser *parser)
{
  return parser->callbacks.start_object (parser->callback_data);
}
static inline bool
do_callback_end_object (JSON_CallbackParser *parser)
{
  return parser->callbacks.end_object (parser->callback_data);
}

static inline bool
do_callback_start_array (JSON_CallbackParser *parser)
{
  return parser->callbacks.start_array (parser->callback_data);
}

static inline bool
do_callback_end_array (JSON_CallbackParser *parser)
{
  return parser->callbacks.end_array (parser->callback_data);
}

static inline bool
do_callback_object_key  (JSON_CallbackParser *parser)
{
  return parser->callbacks.object_key (parser->buffer_length,
                                       buffer_nul_terminate (parser),
                                       parser->callback_data);
}
static inline bool
do_callback_string      (JSON_CallbackParser *parser)
{
  return parser->callbacks.string_value (parser->buffer_length,
                                         buffer_nul_terminate (parser),
                                         parser->callback_data);
}
static inline bool
do_callback_number      (JSON_CallbackParser *parser)
{
  return parser->callbacks.number_value (parser->buffer_length,
                                         buffer_nul_terminate (parser),
                                         parser->callback_data);
}
static inline bool
do_callback_boolean     (JSON_CallbackParser *parser, bool v)
{
  return parser->callbacks.boolean_value (v, parser->callback_data);
}
static inline bool
do_callback_null        (JSON_CallbackParser *parser)
{
  return parser->callbacks.null_value (parser->callback_data);
}
static inline void
do_callback_error       (JSON_CallbackParser *parser)
{
  JSON_CallbackParser_ErrorInfo error_info;
  error_info.code = parser->error_code;
  error_info.code_str = error_code_to_string (parser->error_code);
  error_info.line_no = parser->line_no;
  error_info.byte_no = parser->byte_no;
  error_info.message = error_code_to_message (parser->error_code);
  switch (parser->error_code)
    {
    case JSON_CALLBACK_PARSER_ERROR_UNEXPECTED_CHAR:
      error_info.message2 = byte_name_str + byte_name_offsets[parser->error_byte];
      break;
    default:
      error_info.message2 = NULL;
      break;
    }
  parser->callbacks.error (&error_info, parser->callback_data);
}

static ScanResult
scan_whitespace_json   (JSON_CallbackParser *parser,
                        const uint8_t **p_at,
                        const uint8_t  *end)
{
  const uint8_t *at = *p_at;
  (void) parser;
  while (at < end && IS_SPACE (*at))
    at++;
  *p_at = at;
  return SCAN_END;
}
static ScanResult
scan_whitespace_json5  (JSON_CallbackParser *parser,
                        const uint8_t **p_at,
                        const uint8_t  *end)
{
#define CASE(shortname) case_##shortname: case WHITESPACE_STATE_##shortname
#define GOTO_WHITESPACE_STATE(shortname)                      \
do {                                                          \
  parser->whitespace_state = WHITESPACE_STATE_##shortname; \
  if (at == end) {                                            \
    *p_at = at;                                               \
    return SCAN_IN_VALUE;                                     \
  }                                                           \
  goto case_##shortname;                                      \
} while(0)
  const uint8_t *at = *p_at;
  (void) parser;
  switch (parser->whitespace_state)
    {
    CASE(DEFAULT):
      while (at < end && IS_SPACE(*at))
        at++;
      if (at == end)
        {
          *p_at = at;
          return SCAN_END;
        }
      if (*at == '/')
        {
          at++;
          GOTO_WHITESPACE_STATE(SLASH);
        }
      else if (*at < 128)
        {
          *p_at = at;
          return SCAN_END;
        }
      else if (*at == 0xe1)
        {
          at++;
          GOTO_WHITESPACE_STATE(UTF8_E1);
        }
      else if (*at == 0xe2)
        {
          at++;
          GOTO_WHITESPACE_STATE(UTF8_E2);
        }
      else if (*at == 0xe3)
        {
          at++;
          GOTO_WHITESPACE_STATE(UTF8_E3);
        }
      else
        goto nonws_nonascii;
    CASE(SLASH):
      if (*at == '*')
        {
          at++;
          GOTO_WHITESPACE_STATE(MULTILINE_COMMENT);
        }
      else if (*at == '/')
        {
          at++;
          GOTO_WHITESPACE_STATE(EOL_COMMENT);
        }
      else
        {
          *p_at = at;
          parser->error_byte = *at;
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_UNEXPECTED_CHAR;
          return SCAN_ERROR;
        }

    CASE(EOL_COMMENT):
      while (at < end && *at != '\n')
        at++;
      if (at == end)
        {
          *p_at = at;
          return SCAN_IN_VALUE;
        }
      at++;
      GOTO_WHITESPACE_STATE(DEFAULT);

    CASE(MULTILINE_COMMENT):
      while (at < end)
        {
          if (*at == '*')
            {
              at++;
              GOTO_WHITESPACE_STATE(MULTILINE_COMMENT_STAR);
            }
          else
            at++;
        }
      *p_at = at;
      return SCAN_IN_VALUE;

    CASE(MULTILINE_COMMENT_STAR):
      if (*at == '/')
        {
          at++;
          GOTO_WHITESPACE_STATE(DEFAULT);
        }
      else if (*at == '*')
        {
          at++;
          GOTO_WHITESPACE_STATE(MULTILINE_COMMENT_STAR);
        }
      else
        {
          at++;
          GOTO_WHITESPACE_STATE(MULTILINE_COMMENT);
        }

    CASE(UTF8_E1):
      if (*at == 0x9a)
        {
          at++;
          GOTO_WHITESPACE_STATE(DEFAULT);
        }
      else
        goto nonws_nonascii;

    CASE(UTF8_E2):
      if (*at == 0x80)
        GOTO_WHITESPACE_STATE(UTF8_E2_80);
      else if (*at == 0x81)
        GOTO_WHITESPACE_STATE(UTF8_E2_81);
      else
        goto nonws_nonascii;
    CASE(UTF8_E2_80):
      if (*at == 0x80 /* u+2000 en quad */
       || *at == 0x81 /* u+2001 em quad */
       || *at == 0x84 /* u+2004 three-per-em space */
       || *at == 0x85 /* u+2005 FOUR-PER-EM SPACE */
       || *at == 0x87 /* u+2007 FIGURE SPACE  */
       || *at == 0x88 /* u+2008 PUNCTUATION SPACE */
       || *at == 0x89 /* u+2009 THIN SPACE  */
       || *at == 0x8a /* u+200A HAIR SPACE  */
       || *at == 0x8b)/* u+200B ZERO WIDTH SPACE  */
      {
        at++;
        GOTO_WHITESPACE_STATE(DEFAULT);
      }
    CASE(UTF8_E2_81):
      if (*at == 0x9f) /* U+205F MEDIUM MATHEMATICAL SPACE  */
        {
          at++;
          GOTO_WHITESPACE_STATE(DEFAULT);
        }
      else
        goto nonws_nonascii;

    CASE(UTF8_E3):
      if (*at == 0x80)
        {
          at++;
          GOTO_WHITESPACE_STATE(UTF8_E3_80);
        }
      else
        goto nonws_nonascii;

    CASE(UTF8_E3_80):
      if (*at == 0x80)
        {
          /* U+3000 IDEOGRAPHIC SPACE */
          at++;
          GOTO_WHITESPACE_STATE(DEFAULT);
        }
      else
        goto nonws_nonascii;
    }
#undef CASE
#undef GOTO_WHITESPACE_STATE


nonws_nonascii:
  parser->error_byte = *at;
  parser->error_code = JSON_CALLBACK_PARSER_ERROR_UNEXPECTED_CHAR;
  return SCAN_ERROR;
}

static ScanResult
scan_whitespace_no_whitespace_allowed (JSON_CallbackParser *parser,
                                       const uint8_t **p_at,
                                       const uint8_t  *end)
{
  return SCAN_END;
}

static ScanResult
scan_whitespace_generic(JSON_CallbackParser *parser,
                        const uint8_t **p_at,
                        const uint8_t  *end)
{
#define CASE(shortname) case_##shortname: case WHITESPACE_STATE_##shortname
#define GOTO_WHITESPACE_STATE(shortname)                      \
do {                                                          \
  parser->whitespace_state = WHITESPACE_STATE_##shortname; \
  if (at == end) {                                            \
    *p_at = at;                                               \
    return SCAN_IN_VALUE;                                     \
  }                                                           \
  goto case_##shortname;                                      \
} while(0)
  const uint8_t *at = *p_at;
  (void) parser;
  switch (parser->whitespace_state)
    {
    CASE(DEFAULT):
      while (at < end && IS_SPACE(*at))
        at++;
      if (*at == '/' && (parser->options.ignore_single_line_comments
                       || parser->options.ignore_multi_line_comments))
        {
          at++;
          GOTO_WHITESPACE_STATE(SLASH);
        }
      else if (*at == 0xe1 && parser->options.ignore_unicode_whitespace)
        {
          at++;
          GOTO_WHITESPACE_STATE(UTF8_E1);
        }
      else if (*at == 0xe2 && parser->options.ignore_unicode_whitespace)
        {
          at++;
          GOTO_WHITESPACE_STATE(UTF8_E2);
        }
      else if (*at == 0xe3 && parser->options.ignore_unicode_whitespace)
        {
          at++;
          GOTO_WHITESPACE_STATE(UTF8_E3);
        }
      else
        goto nonws_nonascii;
    CASE(SLASH):
      if (*at == '*' && parser->options.ignore_multi_line_comments)
        {
          at++;
          GOTO_WHITESPACE_STATE(MULTILINE_COMMENT);
        }
      else if (*at == '/' && parser->options.ignore_single_line_comments)
        {
          at++;
          GOTO_WHITESPACE_STATE(EOL_COMMENT);
        }
      else
        {
          *p_at = at;
          parser->error_byte = *at;
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_UNEXPECTED_CHAR;
          return SCAN_ERROR;
        }

    CASE(EOL_COMMENT):
      while (at < end && *at != '\n')
        at++;
      if (at == end)
        {
          *p_at = at;
          return SCAN_IN_VALUE;
        }
      at++;
      GOTO_WHITESPACE_STATE(DEFAULT);

    CASE(MULTILINE_COMMENT):
      while (at < end)
        {
          if (*at == '*')
            {
              at++;
              GOTO_WHITESPACE_STATE(MULTILINE_COMMENT_STAR);
            }
          else
            at++;
        }
      *p_at = at;
      return SCAN_IN_VALUE;

    CASE(MULTILINE_COMMENT_STAR):
      if (*at == '/')
        {
          at++;
          GOTO_WHITESPACE_STATE(DEFAULT);
        }
      else if (*at == '*')
        {
          at++;
          GOTO_WHITESPACE_STATE(MULTILINE_COMMENT_STAR);
        }
      else
        {
          at++;
          GOTO_WHITESPACE_STATE(MULTILINE_COMMENT);
        }

    CASE(UTF8_E1):
      if (*at == 0x9a)
        {
          at++;
          GOTO_WHITESPACE_STATE(DEFAULT);
        }
      else
        goto nonws_nonascii;

    CASE(UTF8_E2):
      if (*at == 0x80)
        GOTO_WHITESPACE_STATE(UTF8_E2_80);
      else if (*at == 0x81)
        GOTO_WHITESPACE_STATE(UTF8_E2_81);
      else
        goto nonws_nonascii;
    CASE(UTF8_E2_80):
      if (*at == 0x80 /* u+2000 en quad */
       || *at == 0x81 /* u+2001 em quad */
       || *at == 0x84 /* u+2004 three-per-em space */
       || *at == 0x85 /* u+2005 FOUR-PER-EM SPACE */
       || *at == 0x87 /* u+2007 FIGURE SPACE  */
       || *at == 0x88 /* u+2008 PUNCTUATION SPACE */
       || *at == 0x89 /* u+2009 THIN SPACE  */
       || *at == 0x8a /* u+200A HAIR SPACE  */
       || *at == 0x8b)/* u+200B ZERO WIDTH SPACE  */
      {
        at++;
        GOTO_WHITESPACE_STATE(DEFAULT);
      }
    CASE(UTF8_E2_81):
      if (*at == 0x9f) /* U+205F MEDIUM MATHEMATICAL SPACE  */
        {
          at++;
          GOTO_WHITESPACE_STATE(DEFAULT);
        }
      else
        goto nonws_nonascii;

    CASE(UTF8_E3):
      if (*at == 0x80)
        {
          at++;
          GOTO_WHITESPACE_STATE(UTF8_E3_80);
        }
      else
        goto nonws_nonascii;

    CASE(UTF8_E3_80):
      if (*at == 0x80)
        {
          /* U+3000 IDEOGRAPHIC SPACE */
          at++;
          GOTO_WHITESPACE_STATE(DEFAULT);
        }
      else
        goto nonws_nonascii;
    }
#undef CASE
#undef GOTO_WHITESPACE_STATE


nonws_nonascii:
  parser->error_byte = *at;
  parser->error_code = JSON_CALLBACK_PARSER_ERROR_UNEXPECTED_CHAR;
  return SCAN_ERROR;
}


size_t
json_callback_parser_options_get_memory_size (const JSON_CallbackParser_Options *options)
{
  return sizeof (JSON_CallbackParser)
       + options->max_stack_depth * sizeof (JSON_CallbackParser_StackNode);
}

// note that JSON_CallbackParser allocates no further memory, so this can 
// be embedded somewhere.
JSON_CallbackParser *
json_callback_parser_new (const JSON_Callbacks *callbacks,
                          void *callback_data,
                          const JSON_CallbackParser_Options *options)
{
  JSON_CallbackParser *parser = malloc (sizeof (JSON_CallbackParser) + sizeof (JSON_CallbackParser_StackNode) * options->max_stack_depth);
  parser->options = *options;
  parser->stack_depth = 0;
  parser->stack_nodes = (JSON_CallbackParser_StackNode *) (parser + 1);
  parser->state = JSON_CALLBACK_PARSER_STATE_INTERIM;
  parser->error_code = JSON_CALLBACK_PARSER_ERROR_NONE;
  parser->callbacks = *callbacks;
  parser->callback_data = callback_data;
  parser->buffer_alloced = 64;
  parser->buffer_length = 0;
  parser->buffer = malloc (parser->buffer_alloced);

  // select optimized whitespace scanner
  if (!options->ignore_single_line_comments
   && !options->ignore_multi_line_comments
   && !options->disallow_extra_whitespace
   && !options->ignore_unicode_whitespace)
    parser->whitespace_scanner = scan_whitespace_json;
  else
  if ( options->ignore_single_line_comments
   &&  options->ignore_multi_line_comments
   && !options->disallow_extra_whitespace
   &&  options->ignore_unicode_whitespace)
    parser->whitespace_scanner = scan_whitespace_json5;
  else
  if ( !options->ignore_single_line_comments
   &&  !options->ignore_multi_line_comments
   &&   options->disallow_extra_whitespace)
    parser->whitespace_scanner = scan_whitespace_no_whitespace_allowed;
  else
    parser->whitespace_scanner = scan_whitespace_generic;
    
  return parser;
}

static inline ScanResult
utf8_validate_char (JSON_CallbackParser *parser,
                    const uint8_t **p_at,
                    const uint8_t *end)
{
  const uint8_t *at = *p_at;
  if ((*at & 0xe0) == 0xc0)
    {
      if ((*at & 0x1f) < 2)
        {
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_UTF8_OVERLONG;
          return SCAN_ERROR;
        }
      // 2 byte sequence
      if (at + 1 == end)
        {
          parser->utf8_state = UTF8_STATE_2_1;
          *p_at = at + 1;
          return SCAN_IN_VALUE;
        }
      if ((at[1] & 0xc0) != 0x80)
        {
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_UTF8_BAD_TRAILING_BYTE;
          return SCAN_ERROR;
        }
      *p_at = at + 2;
      return SCAN_END;
    }
  else if ((*at & 0xf0) == 0xe0)
    {
      // 3 byte sequence
      if (at + 1 == end)
        {
          *p_at = at + 1;
          parser->utf8_state = ((at[0] & 0x0f) != 0)
                                ? UTF8_STATE_3_1_got_nonzero
                                : UTF8_STATE_3_1_got_zero;
          return SCAN_IN_VALUE;
        }
      if ((at[1] & 0xc0) != 0x80)
        {
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_UTF8_BAD_TRAILING_BYTE;
          return SCAN_ERROR;
        }
      if ((at[0] & 0x0f) == 0 && (at[1] & 0x40) == 0)
        {
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_UTF8_OVERLONG;
          return SCAN_ERROR;
        }
      if (at + 2 == end)
        {
          *p_at = at + 2;
          parser->utf8_state = UTF8_STATE_3_2;
          return SCAN_IN_VALUE;
        }
      if ((at[2] & 0xc0) != 0x80)
        {
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_UTF8_BAD_TRAILING_BYTE;
          return SCAN_ERROR;
        }
      *p_at = at + 3;
      return SCAN_END;
    }
  else if ((*at & 0xf8) == 0xf0)
    {
      // 4 byte sequence
      if (at + 1 == end)
        {
          parser->utf8_state = ((at[0] & 0x07) != 0)
                                ? UTF8_STATE_4_1_got_nonzero
                                : UTF8_STATE_4_1_got_zero;
          *p_at = at + 1;
          return SCAN_IN_VALUE;
        }
      if ((at[1] & 0xc0) != 0x80)
        {
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_UTF8_BAD_TRAILING_BYTE;
          return SCAN_ERROR;
        }
      if ((at[0] & 0x07) == 0 && (at[1] & 0x30) == 0)
        {
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_UTF8_OVERLONG;
          return SCAN_ERROR;
        }
      if (at + 2 == end)
        {
          parser->utf8_state = UTF8_STATE_4_2;
          *p_at = at + 2;
          return SCAN_IN_VALUE;
        }
      if ((at[2] & 0xc0) != 0x80)
        {
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_UTF8_BAD_TRAILING_BYTE;
          return SCAN_ERROR;
        }
      if (at + 3 == end)
        {
          parser->utf8_state = UTF8_STATE_4_3;
          *p_at = at + 3;
          return SCAN_IN_VALUE;
        }
      if ((at[3] & 0xc0) != 0x80)
        {
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_UTF8_BAD_TRAILING_BYTE;
          return SCAN_ERROR;
        }
      *p_at = at + 4;
      return SCAN_END;
    }
  else
    {
      // invalid initial utf8-byte
      parser->error_code = JSON_CALLBACK_PARSER_ERROR_UTF8_BAD_INITIAL_BYTE;
      return SCAN_ERROR;
    }
}
static inline ScanResult
utf8_continue_validate_char (JSON_CallbackParser *parser,
                             const uint8_t **p_at,
                             const uint8_t *end)
{
  const uint8_t *at = *p_at;
  switch (parser->utf8_state)
    {
    case UTF8_STATE_2_1:
      if ((at[0] & 0xc0) != 0x80)
        {
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_UTF8_BAD_TRAILING_BYTE;
          return SCAN_ERROR;
        }
      *p_at = at + 1;
      return SCAN_END;

    case UTF8_STATE_3_1_got_zero:
      if ((at[0] & 0x40) == 0)
        {
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_UTF8_OVERLONG;
          return SCAN_ERROR;
        }
      // fall through

    case UTF8_STATE_3_1_got_nonzero:
      if ((at[0] & 0xc0) != 0x80)
        {
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_UTF8_BAD_TRAILING_BYTE;
          return SCAN_ERROR;
        }
      if (at + 1 == end)
        {
          parser->utf8_state = UTF8_STATE_3_2;
          *p_at = at + 1;
          return SCAN_IN_VALUE;
        }
      if ((at[1] & 0xc0) != 0x80)
        {
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_UTF8_BAD_TRAILING_BYTE;
          parser->utf8_state = UTF8_STATE_3_1_got_nonzero;
          return SCAN_ERROR;
        }
      *p_at = at + 2;
      return SCAN_END;

    case UTF8_STATE_3_2:
      if ((at[0] & 0xc0) != 0x80)
        {
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_UTF8_BAD_TRAILING_BYTE;
          parser->utf8_state = UTF8_STATE_3_2;
          return SCAN_ERROR;
        }
      *p_at = at + 1;
      return SCAN_END;

    case UTF8_STATE_4_1_got_zero:
      if ((at[0] & 0x30) == 0)
        {
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_UTF8_OVERLONG;
          return SCAN_ERROR;
        }
      // fall through
    case UTF8_STATE_4_1_got_nonzero:
      if ((at[0] & 0xc0) != 0x80)
        {
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_UTF8_BAD_TRAILING_BYTE;
          parser->utf8_state = UTF8_STATE_4_1_got_nonzero;
          return SCAN_ERROR;
        }
      at++;
      if (at == end)
        {
          *p_at = at;
          parser->utf8_state = UTF8_STATE_4_2;
          return SCAN_IN_VALUE;
        }
      // fall-through

    case UTF8_STATE_4_2:
      if ((at[0] & 0xc0) != 0x80)
        {
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_UTF8_BAD_TRAILING_BYTE;
          parser->utf8_state = UTF8_STATE_4_2;
          return SCAN_ERROR;
        }
      at++;
      if (at == end)
        {
          *p_at = at;
          parser->utf8_state = UTF8_STATE_4_3;
          return SCAN_IN_VALUE;
        }
      // fall-through

    case UTF8_STATE_4_3:
      if ((at[0] & 0xc0) != 0x80)
        {
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_UTF8_BAD_TRAILING_BYTE;
          parser->utf8_state = UTF8_STATE_4_3;
          return SCAN_ERROR;
        }
      *p_at = at + 1;
      return SCAN_END;
    }
  assert(0);
  return SCAN_ERROR;
}

#define MAX_UTF8_LEN 4
static unsigned
unicode_to_utf8 (uint32_t c, uint8_t *out)
{
  if (c < 0x80)
    {
      *out = c;
      return 1;
    }
  else if (c < 0x800)
    {
      out[1] = (c & 0x3f) | 0x80;
      c >>= 6;
      out[0] = c | 0xc0;
      return 2;
    }
  else if (c < 0x10000)
    {
      out[2] = (c & 0x3f) | 0x80;
      c >>= 6;
      out[1] = (c & 0x3f) | 0x80;
      c >>= 6;
      out[0] = c | 0xe0;
      return 3;
    }
   else if (c < 0x200000)
    {
      out[3] = (c & 0x3f) | 0x80;
      c >>= 6;
      out[2] = (c & 0x3f) | 0x80;
      c >>= 6;
      out[1] = (c & 0x3f) | 0x80;
      c >>= 6;
      out[0] = c | 0xf0;
      return 4;
    }
  else
    {
      return 0;
    }
}

static inline int
is_number_end_char (char c)
{
  return IS_SPACE(c) || c == ',' || c == '}' || c == ']';
}

#if 0
static inline int
is_flat_value_char (JSON_CallbackParser *parser,
                    char            c)
{
  if (c == '"') return 1;
  if ('0' <= c && c <= '9') return 1;
  if (c == '\'' && parser->options.permit_single_quote_strings) return 1;
  if (c == '.' && parser->options.permit_leading_decimal_point) return 1;
  if (c == 't' || c == 'f' || c == 'n') return 1;
  return 0;
}
#endif

static inline bool
maybe_setup_flat_value_state (JSON_CallbackParser *parser, uint8_t c)
{
  DEBUG_PRINTF(("maybe_setup_flat_value_state: bot is object=%u c=%c",parser->stack_nodes[0].is_object, c));
  switch (c)
    {
    case '\'':
      if (!parser->options.permit_single_quote_strings)
        return false;
      /* fall-through */

    case '"':
      parser->quote_char = c;
      parser->flat_value_state = FLAT_VALUE_STATE_STRING;
      buffer_empty (parser);
      return true;

    case '1': case '2': case '3':
    case '4': case '5': case '6':
    case '7': case '8': case '9':
      buffer_set (parser, 1, &c);
      parser->flat_value_state = FLAT_VALUE_STATE_IN_DIGITS;
      return true;

    case '0':
      buffer_set (parser, 1, &c);
      parser->flat_value_state = FLAT_VALUE_STATE_GOT_0;
      return true;

    case '+': case '-':
      buffer_set (parser, 1, &c);
      parser->flat_value_state = FLAT_VALUE_STATE_GOT_SIGN;
      return true;

    case '.':
      if (parser->options.permit_leading_decimal_point)
        {
          buffer_set (parser, 1, &c);
          parser->flat_value_state = FLAT_VALUE_STATE_GOT_LEADING_DECIMAL_POINT;
          return true;
        }
      return false;

    case 't':
      buffer_set (parser, 1, &c);
      parser->flat_value_state = FLAT_VALUE_STATE_IN_TRUE;
      return true;
    case 'f':
      buffer_set (parser, 1, &c);
      parser->flat_value_state = FLAT_VALUE_STATE_IN_FALSE;
      return true;
    case 'n':
      buffer_set (parser, 1, &c);
      parser->flat_value_state = FLAT_VALUE_STATE_IN_NULL;
      return true;
    default:
      return false;
    }
}

static ScanResult
generic_bareword_handler (JSON_CallbackParser *parser,
                          const uint8_t**p_at,
                          const uint8_t *end,
                          const char    *bareword,
                          size_t         bareword_len)
{
  const uint8_t *at = *p_at;
  while (at < end
      && parser->buffer_length < bareword_len
      && (char)(*at) == bareword[parser->buffer_length])
    {
      buffer_append_byte (parser, *at);
      at++;
    }
  if (at == end)
    {
      *p_at = at;
      return SCAN_IN_VALUE;
    }
  if (is_number_end_char(*at))
    {
      *p_at = at;
      return SCAN_END;
    }
  parser->error_code = JSON_CALLBACK_PARSER_ERROR_BAD_BAREWORD;
  *p_at = at;
  return SCAN_ERROR;
}

// NOTE: only handles non-initial characters
// (the first character is handled by maybe_setup_flat_value_state())
static ScanResult
scan_flat_value (JSON_CallbackParser *parser,
                 const uint8_t **p_at,
                 const uint8_t  *end)
{
  const uint8_t *at = *p_at;
  DEBUG_PRINTF(("scan_flat_value: *at=%c (%02x), bot obj=%u state=%u",*at,*at,parser->stack_nodes[0].is_object, parser->flat_value_state));

#define FLAT_VALUE_GOTO_STATE(st_shortname) \
  do{ \
    DEBUG_PRINTF(("scan_flat_value: goto %s\n",#st_shortname)); \
    parser->flat_value_state = FLAT_VALUE_STATE_##st_shortname; \
    if (at == end) \
      { \
        *p_at = at; \
        return SCAN_IN_VALUE; \
      } \
    goto case_##st_shortname; \
  }while(0)

  switch (parser->flat_value_state)
    {
    case_STRING:
    case FLAT_VALUE_STATE_STRING:
      while (at < end)
        {
          if (*at == '\\')
            {
              at++;
              parser->flat_len = 0;
              FLAT_VALUE_GOTO_STATE(IN_BACKSLASH_SEQUENCE);
            }
          else if (*at == parser->quote_char)
            {
              *p_at = at + 1;
              return SCAN_END;
            }
          else if ((*at & 0x80) == 0)
            {
              // ascii range, but are naked control sequences allowed?
              if (*at < 32 || *at == 127)
                {
                  parser->error_code = JSON_CALLBACK_PARSER_ERROR_STRING_CONTROL_CHARACTER;
                  return SCAN_ERROR;
                }
              buffer_append_byte (parser, *at);
              at++;
              continue;
            }
          else
            {
              const uint8_t *start = at;
              switch (utf8_validate_char (parser, &at, end))
                {
                case SCAN_END:
                  buffer_append (parser, at - start, start);
                  break;
                case SCAN_IN_VALUE:
                  // note that parser->utf8_state is set
                  // when we get this return-value.
                  assert(at == end);
                  buffer_append (parser, at-start, start);
                  parser->flat_value_state = FLAT_VALUE_STATE_IN_UTF8_CHAR;
                  *p_at = at;
                  return SCAN_IN_VALUE;
                case SCAN_ERROR:
                  return SCAN_ERROR;
                }
            }
        }
      *p_at = at;
      return SCAN_IN_VALUE;

    case_IN_BACKSLASH_SEQUENCE:
    case FLAT_VALUE_STATE_IN_BACKSLASH_SEQUENCE:
      switch (*at)
        {
        // one character ecape codes handled here.
        case 'r':
          buffer_append_byte (parser, '\r');
          at++;
          FLAT_VALUE_GOTO_STATE(STRING);
        case 't':
          buffer_append_byte (parser, '\t');
          at++;
          FLAT_VALUE_GOTO_STATE(STRING);
        case 'n':
          buffer_append_byte (parser, '\n');
          at++;
          FLAT_VALUE_GOTO_STATE(STRING);
        case 'b':
          buffer_append_byte (parser, '\b');
          at++;
          FLAT_VALUE_GOTO_STATE(STRING);
        case 'v':
          buffer_append_byte (parser, '\v');
          at++;
          FLAT_VALUE_GOTO_STATE(STRING);
        case 'f':
          buffer_append_byte (parser, '\f');
          at++;
          FLAT_VALUE_GOTO_STATE(STRING);
        case '"': case '\\': case '\'':
          buffer_append_byte (parser, *at);
          at++;
          FLAT_VALUE_GOTO_STATE(STRING);

        case '0':      
          if (parser->options.permit_backslash_0)
            {
              buffer_append_byte (parser, 0);
              at++;
              FLAT_VALUE_GOTO_STATE(GOT_BACKSLASH_0);
            }
          else
            {
              // literal "0" digit
              buffer_append_byte (parser, '0');
              at++;
              FLAT_VALUE_GOTO_STATE(STRING);
            }

        case '\n':
          if (!parser->options.permit_line_continuations_in_strings)
            {
              parser->error_code = JSON_CALLBACK_PARSER_ERROR_QUOTED_NEWLINE;
              return SCAN_ERROR;
            }
          at++;
          FLAT_VALUE_GOTO_STATE(STRING);

        case '\r':
          // must also accept CRLF = 13,10
          if (!parser->options.permit_line_continuations_in_strings)
            {
              parser->error_code = JSON_CALLBACK_PARSER_ERROR_QUOTED_NEWLINE;
              return SCAN_ERROR;
            }
          at++;
          FLAT_VALUE_GOTO_STATE(IN_BACKSLASH_CR);

        case 'x': case 'X':
          if (!parser->options.permit_backslash_x)
            {
              parser->error_code = JSON_CALLBACK_PARSER_ERROR_BACKSLASH_X_NOT_ALLOWED;
              return SCAN_ERROR;
            }
          at++;
          parser->flat_len = 0;
          FLAT_VALUE_GOTO_STATE(IN_BACKSLASH_X);

        case 'u': case 'U':
          at++;
          parser->flat_len = 0;
          FLAT_VALUE_GOTO_STATE(IN_BACKSLASH_U);

        default:
          if (*at < 0x80)
            {
              // single-byte pass-through
              at++;
              FLAT_VALUE_GOTO_STATE(STRING);
            }
          else
            {
              // multi-byte passthrough character
              const uint8_t *start = at;
              switch (utf8_validate_char (parser, &at, end))
                {
                case SCAN_END:
                  buffer_append (parser, at - start, start);
                  FLAT_VALUE_GOTO_STATE(STRING);

                case SCAN_IN_VALUE:
                  *p_at = at;
                  buffer_append (parser, at - start, start);
                  parser->flat_value_state = FLAT_VALUE_STATE_IN_BACKSLASH_UTF8;
                  return SCAN_IN_VALUE;

                case SCAN_ERROR:
                  *p_at = at;
                  return SCAN_ERROR;
                }
            }
        }

    case_IN_BACKSLASH_CR:
    case FLAT_VALUE_STATE_IN_BACKSLASH_CR:
      if (*at == '\n')
        at++;
      FLAT_VALUE_GOTO_STATE(STRING);

    case_IN_BACKSLASH_X:
    case FLAT_VALUE_STATE_IN_BACKSLASH_X:
      while (parser->flat_len < 2 && at < end && IS_HEX_DIGIT(*at))
        {
          parser->flat_len++;
          at++;
        }
      if (parser->flat_len == 2)
        FLAT_VALUE_GOTO_STATE(STRING);
      else if (at == end)
        {
          *p_at = at;
          return SCAN_IN_VALUE;
        }
      else
        {
          *p_at = at;
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_EXPECTED_HEX_DIGIT;
          return SCAN_ERROR;
        }

    case_IN_BACKSLASH_U:
    case FLAT_VALUE_STATE_IN_BACKSLASH_U:
      while (parser->flat_len < 4 && at < end && IS_HEX_DIGIT(*at))
        {
          parser->flat_data[parser->flat_len] = *at;
          parser->flat_len++;
          at++;
        }
      if (parser->flat_len == 4)
        {
          parser->flat_data[4] = 0;
          uint32_t code = strtoul (parser->flat_data, NULL, 16);
          if (IS_HI_SURROGATE(code))
            {
              parser->hi_surrogate = code;
              FLAT_VALUE_GOTO_STATE(GOT_HI_SURROGATE);
            }
          else if (IS_LO_SURROGATE(code))
            {
              parser->error_code = JSON_CALLBACK_PARSER_ERROR_UTF16_BAD_SURROGATE_PAIR;
              return SCAN_ERROR;
            }
          else
            {
              // any other validation to do?
              FLAT_VALUE_GOTO_STATE(STRING);
            }
        }
      else if (at == end)
        {
          *p_at = at;
          return SCAN_IN_VALUE;
        }
      else
        {
          *p_at = at;
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_EXPECTED_HEX_DIGIT;
          return SCAN_ERROR;
        }
    case_GOT_HI_SURROGATE:
    case FLAT_VALUE_STATE_GOT_HI_SURROGATE:
      if (*at == '\\')
        {
          at++;
          FLAT_VALUE_GOTO_STATE(GOT_HI_SURROGATE_IN_BACKSLASH_SEQUENCE);
        }
      else
        {
          *p_at = at;
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_UTF16_BAD_SURROGATE_PAIR;
          return SCAN_ERROR;
        }
    
    case_GOT_HI_SURROGATE_IN_BACKSLASH_SEQUENCE:
    case FLAT_VALUE_STATE_GOT_HI_SURROGATE_IN_BACKSLASH_SEQUENCE:
      if (*at == 'u' || *at == 'U')
        {
          at++;
          parser->flat_len = 0;
          FLAT_VALUE_GOTO_STATE(GOT_HI_SURROGATE_IN_BACKSLASH_SEQUENCE_U);
        }
      else
        {
          *p_at = at;
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_UTF16_BAD_SURROGATE_PAIR;
          return SCAN_ERROR;
        }

    case_GOT_HI_SURROGATE_IN_BACKSLASH_SEQUENCE_U:
    case FLAT_VALUE_STATE_GOT_HI_SURROGATE_IN_BACKSLASH_SEQUENCE_U:
      while (parser->flat_len < 4 && at < end && IS_HEX_DIGIT(*at))
        {
          parser->flat_data[parser->flat_len] = *at;
          parser->flat_len++;
          at++;
        }
      if (parser->flat_len == 4)
        {
          parser->flat_data[4] = 0;
          uint32_t code = strtoul (parser->flat_data, NULL, 16);
          if (IS_LO_SURROGATE(code))
            {
              uint32_t unicode = COMBINE_SURROGATES (parser->hi_surrogate, code);
              uint8_t utf8[MAX_UTF8_LEN];
              unsigned utf8len = unicode_to_utf8(unicode, utf8);
              if (utf8len == 0) {
                // COMBINE_SURROGATES can return values just over 1<<20,
                // which is the max supported by unicode.
                *p_at = at;
                parser->error_code = JSON_CALLBACK_PARSER_ERROR_UTF16_BAD_SURROGATE_PAIR;
                return SCAN_ERROR;
              }
              DEBUG_PRINTF(( "combined surrogates %04x %04x -> %06x",
               parser->hi_surrogate, code, unicode));
              buffer_append (parser, utf8len, utf8);
              FLAT_VALUE_GOTO_STATE(STRING);
            }
          else
            {
              *p_at = at;
              parser->error_code = JSON_CALLBACK_PARSER_ERROR_UTF16_BAD_SURROGATE_PAIR;
              return SCAN_ERROR;
            }
        }
      else if (at < end)
        {
          *p_at = at;
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_EXPECTED_HEX_DIGIT;
          return SCAN_ERROR;
        }
      else
        {
          *p_at = at;
          return SCAN_IN_VALUE;
        }

    case_GOT_BACKSLASH_0:
    case FLAT_VALUE_STATE_GOT_BACKSLASH_0:
      if (IS_DIGIT(*at))
        {
          *p_at = at;
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_DIGIT_NOT_ALLOWED_AFTER_NUL;
          return SCAN_ERROR;
        }
      FLAT_VALUE_GOTO_STATE(STRING);

    //case_IN_BACKSLASH_UTF8:
    case FLAT_VALUE_STATE_IN_BACKSLASH_UTF8:
      {
        const uint8_t *start = at;
        switch (utf8_continue_validate_char (parser, &at, end))
          {
          case SCAN_END:
            buffer_append (parser, at - start, start);
            FLAT_VALUE_GOTO_STATE(STRING);

          case SCAN_IN_VALUE:
            buffer_append (parser, at - start, start);
            *p_at = at;
            return SCAN_IN_VALUE;
          case SCAN_ERROR:
            *p_at = at;
            return SCAN_ERROR;
          }
      }

    //case_IN_UTF8_CHAR:
    case FLAT_VALUE_STATE_IN_UTF8_CHAR:
      {
        const uint8_t *start = at;
        switch (utf8_continue_validate_char (parser, &at, end))
          {
          case SCAN_END:
            buffer_append (parser, at - start, start);
            FLAT_VALUE_GOTO_STATE(STRING);

          case SCAN_IN_VALUE:
            buffer_append (parser, at - start, start);
            *p_at = at;
            return SCAN_IN_VALUE;

          case SCAN_ERROR:
            *p_at = at;
            return SCAN_ERROR;
          }
      }

    //case_GOT_SIGN:
    case FLAT_VALUE_STATE_GOT_SIGN:
      if (*at == '0')
        {
          buffer_append_byte (parser, '0');
          at++;
          FLAT_VALUE_GOTO_STATE(GOT_0);
        }
      else if ('1' <= *at && *at <= '9')
        {
          buffer_append_byte (parser, *at);
          at++;
          FLAT_VALUE_GOTO_STATE(IN_DIGITS);
        }
      else if (*at == '.' && parser->options.permit_leading_decimal_point)
        {
          buffer_append_byte (parser, *at);
          at++;
          FLAT_VALUE_GOTO_STATE(GOT_LEADING_DECIMAL_POINT);
        }
      else
        {
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_BAD_NUMBER;
          *p_at = at;
          return SCAN_ERROR;
        }

    case_GOT_0:
    case FLAT_VALUE_STATE_GOT_0:
      if ((*at == 'x' || *at == 'X') && parser->options.permit_hex_numbers)
        {
          buffer_append_byte (parser, *at);
          at++;
          FLAT_VALUE_GOTO_STATE(IN_HEX_EMPTY);
        }
      else if (('0' <= *at && *at <= '7') && parser->options.permit_octal_numbers)
        {
          buffer_append_byte (parser, *at);
          at++;
          FLAT_VALUE_GOTO_STATE(IN_OCTAL);
        }
      else if (*at == '.')
        {
          buffer_append_byte (parser, *at);
          at++;
          FLAT_VALUE_GOTO_STATE(GOT_DECIMAL_POINT);
        }
      else if (*at == 'e' || *at == 'E')
        {
          buffer_append_byte (parser, *at);
          at++;
          FLAT_VALUE_GOTO_STATE(GOT_E);
        }
      else if ('0' <= *at && *at <= '9')
        {
          buffer_append_byte (parser, *at);
          at++;
          FLAT_VALUE_GOTO_STATE(IN_DIGITS);
        }
      else if (is_number_end_char (*at))
        {
          // simply 0
          *p_at = at;
          return SCAN_END;
        }
      else
        {
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_BAD_NUMBER;
          *p_at = at;
          return SCAN_ERROR;
        }
    
    case_IN_DIGITS:
    case FLAT_VALUE_STATE_IN_DIGITS:
        DEBUG_PRINTF(( "IN_DIGITS: at=%p end=%p *at=%c\n",at,end,*at));
        while ((at < end) && ('0' <= *at && *at <= '9'))
          {
            buffer_append_byte (parser, *at);
            at++;
          }
        if (at == end)
          {
            *p_at = at;
            return SCAN_IN_VALUE;
          }
        if (*at == 'e' || *at == 'E')
          {
            buffer_append_byte (parser, *at);
            at++;
            FLAT_VALUE_GOTO_STATE(GOT_E);
          }
        else if (*at == '.')
          {
            buffer_append_byte (parser, *at);
            at++;
            FLAT_VALUE_GOTO_STATE(GOT_DECIMAL_POINT);
          }
        else if (is_number_end_char (*at))
          {
            *p_at = at;
            DEBUG_PRINTF(("end of IN_DIGITS: at=%c",*at));
            return SCAN_END;
          }
        else
          {
            parser->error_code = JSON_CALLBACK_PARSER_ERROR_BAD_NUMBER;
            return SCAN_ERROR;
          }

    case_IN_OCTAL:
    case FLAT_VALUE_STATE_IN_OCTAL:
      while (at < end && IS_OCT_DIGIT (*at))
        at++;
      if (at == end)
        {
          *p_at = at;
          return SCAN_IN_VALUE;
        }
      else if (is_number_end_char (*at))
        {
          *p_at = at;
          return SCAN_END;
        }
      else
        {
          *p_at = at;
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_BAD_NUMBER;
          return SCAN_ERROR;
        }

    case_IN_HEX_EMPTY:
    case FLAT_VALUE_STATE_IN_HEX_EMPTY:
      if (!IS_HEX_DIGIT (*at))
        {
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_BAD_NUMBER;
          return SCAN_END;
        }
      buffer_append_byte (parser, *at);
      at++;
      FLAT_VALUE_GOTO_STATE(IN_HEX);

    case_IN_HEX:
    case FLAT_VALUE_STATE_IN_HEX:
      while (at < end && IS_HEX_DIGIT(*at))
        {
          buffer_append_byte (parser, *at);
          at++;
        }
      *p_at = at;
      if (at == end)
        return SCAN_IN_VALUE;
      return SCAN_END;

    case_GOT_E:
    case FLAT_VALUE_STATE_GOT_E:
      if (*at == '-' || *at == '+')
        {
          buffer_append_byte (parser, *at);
          at++;
          FLAT_VALUE_GOTO_STATE(GOT_E_DIGITS);
        }
      else
        FLAT_VALUE_GOTO_STATE(GOT_E_PM);

    case_GOT_E_PM:
    case FLAT_VALUE_STATE_GOT_E_PM:
      if (IS_DIGIT (*at))
        {
          buffer_append_byte (parser, *at);
          at++;
          FLAT_VALUE_GOTO_STATE(GOT_E_DIGITS);
        }
      else
        {
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_BAD_NUMBER;
          return SCAN_END;
        }

    case_GOT_E_DIGITS:
    case FLAT_VALUE_STATE_GOT_E_DIGITS:
      while (at < end && IS_DIGIT (*at))
        {
          buffer_append_byte (parser, *at);
          at++;
        }
      if (at == end)
        {
          *p_at = at;
          return SCAN_IN_VALUE;
        }
      if (is_number_end_char (*at))
        {
          *p_at = at;
          return SCAN_END;
        }
      else
        {
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_BAD_NUMBER;
          return SCAN_ERROR;
        }
      

    case_GOT_LEADING_DECIMAL_POINT:
    case FLAT_VALUE_STATE_GOT_LEADING_DECIMAL_POINT:
      if (!IS_DIGIT (*at))
        {
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_BAD_NUMBER;
          *p_at = at;
          return SCAN_ERROR;
        }
      buffer_append_byte (parser, *at);
      at++;
      FLAT_VALUE_GOTO_STATE(GOT_DECIMAL_POINT_DIGITS);

    case_GOT_DECIMAL_POINT:
    case FLAT_VALUE_STATE_GOT_DECIMAL_POINT:
      if (IS_DIGIT (*at))
        {
          buffer_append_byte (parser, *at);
          at++;
          FLAT_VALUE_GOTO_STATE(GOT_DECIMAL_POINT_DIGITS);
        }
      else if (parser->options.permit_trailing_decimal_point
            && (*at == 'e' || *at == 'E'))
        {
          buffer_append_byte (parser, *at);
          at++;
          FLAT_VALUE_GOTO_STATE(GOT_E);
        }
      if (!is_number_end_char(*at))
        {
          *p_at = at;
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_BAD_NUMBER;
          return SCAN_ERROR;
        }
      if (!parser->options.permit_trailing_decimal_point)
        {
          *p_at = at;
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_BAD_NUMBER;
          return SCAN_ERROR;
        }
      *p_at = at;
      return SCAN_END;

    case_GOT_DECIMAL_POINT_DIGITS:
    case FLAT_VALUE_STATE_GOT_DECIMAL_POINT_DIGITS:
      while (at < end && IS_DIGIT (*at))
        {
          buffer_append_byte (parser, *at);
          at++;
        }
      if (at == end)
        {
          *p_at = at;
          return SCAN_IN_VALUE;
        }
      if (*at == 'e' || *at == 'E')
        {
          buffer_append_byte (parser, *at);
          at++;
          FLAT_VALUE_GOTO_STATE(GOT_E);
        }
      if (!is_number_end_char(*at))
        {
          *p_at = at;
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_BAD_NUMBER;
          return SCAN_ERROR;
        }
      *p_at = at;
      return SCAN_END;

    //case_IN_NULL:
    case FLAT_VALUE_STATE_IN_NULL:
      DEBUG_PRINTF(("generic_bareword_handler: *p_at=%s %u",*p_at,parser->flat_len));
      *p_at = at;
      return generic_bareword_handler (parser, p_at, end, "null", 4);

    //case_IN_TRUE:
    case FLAT_VALUE_STATE_IN_TRUE:
      DEBUG_PRINTF(("calling generic_bareword_handler with true"));
      *p_at = at;
      return generic_bareword_handler (parser, p_at, end, "true", 4);

    //case_IN_FALSE:
    case FLAT_VALUE_STATE_IN_FALSE:
      *p_at = at;
      return generic_bareword_handler (parser, p_at, end, "false", 5);
    }

#undef FLAT_VALUE_GOTO_STATE
  assert(0);
  return SCAN_ERROR;
}

static int
flat_value_can_terminate (JSON_CallbackParser *parser)
{
  switch (parser->flat_value_state)
    {
    case FLAT_VALUE_STATE_STRING:
    case FLAT_VALUE_STATE_IN_BACKSLASH_SEQUENCE:
    case FLAT_VALUE_STATE_GOT_HI_SURROGATE:
    case FLAT_VALUE_STATE_GOT_HI_SURROGATE_IN_BACKSLASH_SEQUENCE:
    case FLAT_VALUE_STATE_GOT_HI_SURROGATE_IN_BACKSLASH_SEQUENCE_U:
    case FLAT_VALUE_STATE_IN_UTF8_CHAR:
    case FLAT_VALUE_STATE_GOT_BACKSLASH_0:
    case FLAT_VALUE_STATE_IN_BACKSLASH_UTF8:
    case FLAT_VALUE_STATE_IN_BACKSLASH_CR:
    case FLAT_VALUE_STATE_IN_BACKSLASH_X:
    case FLAT_VALUE_STATE_IN_BACKSLASH_U:
      return 0;

    // literal numbers - parse states
    case FLAT_VALUE_STATE_GOT_0:
    case FLAT_VALUE_STATE_IN_DIGITS:
    case FLAT_VALUE_STATE_IN_OCTAL:
    case FLAT_VALUE_STATE_GOT_E_DIGITS:
    case FLAT_VALUE_STATE_IN_HEX:
    case FLAT_VALUE_STATE_GOT_DECIMAL_POINT_DIGITS:
      return 1;
    case FLAT_VALUE_STATE_GOT_DECIMAL_POINT:
      return parser->options.permit_trailing_decimal_point;

    case FLAT_VALUE_STATE_IN_HEX_EMPTY:
    case FLAT_VALUE_STATE_GOT_SIGN:
    case FLAT_VALUE_STATE_GOT_E:
    case FLAT_VALUE_STATE_GOT_E_PM:            // must get at least 1 digit
    case FLAT_VALUE_STATE_GOT_LEADING_DECIMAL_POINT:
      return 0;

  // literal barewords - true, false, null
    case FLAT_VALUE_STATE_IN_NULL:
    case FLAT_VALUE_STATE_IN_TRUE:
    case FLAT_VALUE_STATE_IN_FALSE:
      return 0;
    }
}

static bool
do_callback_flat_value (JSON_CallbackParser *parser)
{
  if (flat_value_state_is_string (parser->flat_value_state))
    return do_callback_string (parser);
  if (flat_value_state_is_number (parser->flat_value_state))
    return do_callback_number (parser);
  if (parser->flat_value_state == FLAT_VALUE_STATE_IN_FALSE)
    return do_callback_boolean (parser, false);
  if (parser->flat_value_state == FLAT_VALUE_STATE_IN_TRUE)
    return do_callback_boolean (parser, true);
  if (parser->flat_value_state == FLAT_VALUE_STATE_IN_NULL)
    return do_callback_null (parser);
  assert(0);
  return false;
}

JSON_CALLBACK_PARSER_FUNC_DEF
bool
json_callback_parser_feed (JSON_CallbackParser *parser,
                           size_t          len,
                           const uint8_t  *data)
{
  const uint8_t *end = data + len;
  const uint8_t *at = data;

  DEBUG_PRINTF(("json_callback_parser_feed: len=%u",(unsigned)len));

#define SKIP_CHAR_TYPE(predicate)                                    \
  do {                                                               \
    while (at < end  &&  predicate(*at))                             \
      at++;                                                          \
    if (at == end)                                                   \
      goto at_end;                                                   \
  }while(0)

#define SKIP_WS()                                                    \
  do {                                                               \
    switch (parser->whitespace_scanner (parser, &at, end))           \
      {                                                              \
      case SCAN_IN_VALUE:                                            \
        assert(at == end);                                           \
        return true;                                                 \
                                                                     \
      case SCAN_ERROR:                                               \
        do_callback_error (parser);                                  \
        return false;                                                \
                                                                     \
      case SCAN_END:                                                 \
        break;                                                       \
      }                                                              \
  }while(0)

#define RETURN_ERROR(error_code_shortname)                            \
  do{                                                                 \
    parser->error_code = JSON_CALLBACK_PARSER_ERROR_ ## error_code_shortname; \
    do_callback_error (parser);                                       \
    return false;                                                     \
  }while(0)

#define GOTO_STATE(state_shortname)                                   \
  do{                                                                 \
    DEBUG_PRINTF(("scan_json: goto %s (used=%d, len=%d) [bottom object=%u, depth=%u]", #state_shortname, (int)(at-data), (int)len, parser->stack_nodes[0].is_object,parser->stack_depth)); \
    parser->state = JSON_CALLBACK_PARSER_STATE_ ## state_shortname;   \
    if (at == end)                                                    \
      goto at_end;                                                    \
    goto case_##state_shortname;                                      \
  }while(0)

#define PUSH(is_obj)                                                  \
  do{                                                                 \
    if (parser->stack_depth == parser->options.max_stack_depth)       \
      RETURN_ERROR(STACK_DEPTH_EXCEEDED);                             \
    JSON_CallbackParser_StackNode *n = parser->stack_nodes + parser->stack_depth;\
    n->is_object = (is_obj);                                          \
    DEBUG_PRINTF(("PUSH(%u) at depth %u", n->is_object, parser->stack_depth));\
    ++parser->stack_depth;                                            \
  }while(0)
  
#define PUSH_OBJECT()                                                 \
  do{                                                                 \
    if (!do_callback_start_object(parser))                            \
      return false;                                                   \
    PUSH(1);                                                          \
  }while(0)
#define PUSH_ARRAY()                                                  \
  do{                                                                 \
    if (!do_callback_start_array(parser))                             \
      return false;                                                   \
    PUSH(0);                                                          \
  }while(0)

#define POP()                                                         \
  do{                                                                 \
    DEBUG_PRINTF(("POP: current depth=%u", parser->stack_depth));\
    assert(parser->stack_depth > 0);                                  \
    DEBUG_PRINTF(("  ... stack-top.is_object=%u", parser->stack_nodes[parser->stack_depth - 1].is_object));       \
    if (parser->stack_nodes[parser->stack_depth - 1].is_object)       \
      do_callback_end_object(parser);                                 \
    else                                                              \
      do_callback_end_array(parser);                                  \
    --parser->stack_depth;                                            \
    if (parser->stack_depth == 0)                                     \
      GOTO_STATE(INTERIM_EXPECTING_COMMA);                            \
    else if (parser->stack_nodes[parser->stack_depth-1].is_object)    \
      GOTO_STATE(IN_OBJECT_EXPECTING_COMMA);                          \
    else                                                              \
      GOTO_STATE(IN_ARRAY_EXPECTING_COMMA);                           \
  }while(0)


// Define a label and a c-level case,
// we use the label when we are midstring for efficiency,
// and the case-statement if we run out of data.
#define CASE(shortname) \
        case_##shortname: \
        case JSON_CALLBACK_PARSER_STATE_##shortname

  if (parser->whitespace_state != WHITESPACE_STATE_DEFAULT)
    {
      switch (parser->whitespace_scanner (parser, &at, end))
        {
        case SCAN_END:
          break;
        case SCAN_ERROR:
          do_callback_error (parser);
          return false;
        case SCAN_IN_VALUE:
          return true;
        }
    }

  while (at < end)
    {
      switch (parser->state)
        {
          // Toplevel states (those in between returned objects) are all
          // tagged with INTERIM.
          //
          // The following is the initial state and is the start of each line in
          // some formats.
          CASE(INTERIM):
            SKIP_WS();
            if (at == end)
              goto at_end;
            if (*at == '{')
              {
                // push object marker onto stack
                at++;
                PUSH_OBJECT();
                GOTO_STATE(IN_OBJECT_INITIAL);
              }
            else if (*at == '[')
              {
                // push object marker onto stack
                at++;
                PUSH_ARRAY();
                GOTO_STATE(IN_ARRAY_INITIAL);
              }
            else if (*at == ',')
              {
                if (parser->options.permit_trailing_commas)
                  {
                    at++;
                    GOTO_STATE(INTERIM_GOT_COMMA);
                  }
              }
            else if (maybe_setup_flat_value_state (parser, *at))
              {
                if (!parser->options.permit_bare_values)
                  RETURN_ERROR(EXPECTED_STRUCTURED_VALUE);
                at++;
                GOTO_STATE(INTERIM_VALUE);
              }
            else
              {
                parser->error_byte = *at;
                RETURN_ERROR(UNEXPECTED_CHAR);
              }
            break;

          CASE(INTERIM_EXPECTING_COMMA):
            SKIP_WS();
            if (*at == ',') 
              {
                at++;
                GOTO_STATE(INTERIM);
              }
            else if (IS_SPACE(*at))
              {
                at++;
                continue;
              }
            else if (parser->options.require_toplevel_commas)
              {
                RETURN_ERROR(UNEXPECTED_CHAR);
              }
            else
              {
                GOTO_STATE(INTERIM);
              }

          CASE(INTERIM_GOT_COMMA):
            switch (parser->whitespace_scanner (parser, &at, end))
              {
                case SCAN_END:
                  break;

                case SCAN_ERROR:
                  do_callback_error (parser);
                  return false;

                case SCAN_IN_VALUE:
                  GOTO_STATE(INTERIM_VALUE);
              }
            if (*at == ',' && parser->options.ignore_multiple_commas)
              {
                at++;
                GOTO_STATE(INTERIM_GOT_COMMA);
              }
            if (*at == '[')
              {
                at++;
                PUSH_ARRAY();
                GOTO_STATE(IN_ARRAY_INITIAL);
              }
            if (*at == '{')
              {
                // push object marker onto stack
                at++;
                PUSH_OBJECT();
                GOTO_STATE(IN_OBJECT_INITIAL);
              }
            if (*at == ',')
              {
                if (parser->options.permit_trailing_commas)
                  {
                    at++;
                    GOTO_STATE(INTERIM_GOT_COMMA);
                  }
              }
            if (maybe_setup_flat_value_state (parser, *at))
              {
                if (!parser->options.permit_bare_values)
                  RETURN_ERROR(EXPECTED_STRUCTURED_VALUE);
                at++;
                GOTO_STATE(INTERIM_VALUE);
              }
            else
              {
                parser->error_byte = *at;
                RETURN_ERROR(UNEXPECTED_CHAR);
              }
            break;

          CASE(INTERIM_VALUE):
            switch (scan_flat_value (parser, &at, end))
              {
              case SCAN_END:
                do_callback_flat_value (parser);
                GOTO_STATE(INTERIM_EXPECTING_COMMA);

              case SCAN_ERROR:
                do_callback_error (parser);
                return false;

              case SCAN_IN_VALUE:
                return true;
              }

          CASE(IN_OBJECT_INITIAL):
            switch (parser->whitespace_scanner (parser, &at, end))
              {
              case SCAN_IN_VALUE:
                assert(at == end);
                return true;

              case SCAN_ERROR:
                do_callback_error (parser);
                return false;

              case SCAN_END:
                if (at == end)
                  return true;
                break;
              }
            if (*at == '"'
             || (*at == '\'' && parser->options.permit_single_quote_strings))
              {
                parser->quote_char = *at;
                at++;
                parser->flat_value_state = FLAT_VALUE_STATE_STRING;
                buffer_empty (parser);
                GOTO_STATE (IN_OBJECT_FIELDNAME);
              }
            else if (IS_ASCII_ALPHA (*at) || *at == '_')
              {
                if (!parser->options.permit_bare_fieldnames)
                  {
                    RETURN_ERROR(BAD_BAREWORD);
                  }
                buffer_set (parser, 1, at);
                at++;
                GOTO_STATE (IN_OBJECT_BARE_FIELDNAME);
              }
            else if (*at == '}')
              {
                at++;
                POP();
              }
            else
              {
                parser->error_byte = *at;
                RETURN_ERROR(UNEXPECTED_CHAR);
              }

          CASE(IN_OBJECT):
            switch (parser->whitespace_scanner (parser, &at, end))
              {
              case SCAN_IN_VALUE:
                assert(at == end);
                return true;

              case SCAN_ERROR:
                do_callback_error (parser);
                return false;

              case SCAN_END:
                if (at == end)
                  return true;
                break;
              }
            if (*at == '"'
            || (*at == '\'' && parser->options.permit_single_quote_strings))
              {
                parser->quote_char = *at;
                at++;
                parser->flat_value_state = FLAT_VALUE_STATE_STRING;
                buffer_empty (parser);
                GOTO_STATE (IN_OBJECT_FIELDNAME);
              }
            else if (IS_ASCII_ALPHA (*at) || *at == '_')
              {
                if (!parser->options.permit_bare_fieldnames)
                  {
                    RETURN_ERROR(BAD_BAREWORD);
                  }
                buffer_set (parser, 1, at);
                at++;
                GOTO_STATE (IN_OBJECT_BARE_FIELDNAME);
              }
            else if (*at == '}')
              {
                if (!parser->options.permit_trailing_commas)
                  RETURN_ERROR(TRAILING_COMMA);
                at++;
                POP();
              }
            else
              {
                parser->error_byte = *at;
                RETURN_ERROR(UNEXPECTED_CHAR);
              }

          CASE(IN_OBJECT_FIELDNAME):
            switch (scan_flat_value (parser, &at, end))
              {
              case SCAN_END:
                do_callback_object_key (parser);
                GOTO_STATE(IN_OBJECT_EXPECTING_COLON);

              case SCAN_IN_VALUE:
                assert(at == end);
                goto at_end;

              case SCAN_ERROR:
                do_callback_error (parser);
                return false;
              }
            break;
            
          CASE(IN_OBJECT_BARE_FIELDNAME):
            while (at < end && (IS_ASCII_ALPHA (*at) || *at == '_'))
              {
                buffer_append_byte (parser, *at);
                at++;
              }
            if (at == end)
              goto at_end;
            if (IS_SPACE (*at))
              {
                do_callback_object_key (parser);
                at++;
                GOTO_STATE(IN_OBJECT_EXPECTING_COLON);
              }
            else if (*at == ':')
              {
                do_callback_object_key (parser);
                at++;
                GOTO_STATE(IN_OBJECT_GOT_COLON);
              }
            else
              {
                parser->error_byte = *at;
                RETURN_ERROR(UNEXPECTED_CHAR);
              }
            break;

          CASE(IN_OBJECT_EXPECTING_COLON):
            while (at < end && IS_SPACE (*at))
              at++;
            if (at == end)
              goto at_end;
            if (*at == ':')
              {
                at++;
                GOTO_STATE(IN_OBJECT_GOT_COLON);
              }
            else
              {
                RETURN_ERROR(EXPECTED_COLON);
              }
            break;

          CASE(IN_OBJECT_GOT_COLON):
            SKIP_WS();
            if (at == end)
              goto at_end;
            if (*at == '{')
              {
                PUSH_OBJECT();
                at++;
                GOTO_STATE(IN_OBJECT_INITIAL);
              }
            else if (*at == '[')
              {
                PUSH_ARRAY();
                at++;
                GOTO_STATE(IN_ARRAY_INITIAL);
              }
            else if (maybe_setup_flat_value_state (parser, *at))
              {
                at++;
                GOTO_STATE(IN_OBJECT_VALUE);
              }
            else
              {
                parser->error_byte = *at;
                RETURN_ERROR(UNEXPECTED_CHAR);
              }

          // this state is only used for non-structured values;
          // otherwise, we use the stack.
          CASE(IN_OBJECT_VALUE):
            switch (scan_flat_value (parser, &at, end))
              {
              case SCAN_END:
                do_callback_flat_value (parser);
                GOTO_STATE(IN_OBJECT_EXPECTING_COMMA);

              case SCAN_IN_VALUE:
                DEBUG_PRINTF(( "scan_flat_value=IN_VALUE"));;
                assert(at == end);
                goto at_end;

              case SCAN_ERROR:
                do_callback_error (parser);
                return false;
              }
          CASE(IN_OBJECT_EXPECTING_COMMA):
            SKIP_WS();
            if (at == end)
              goto at_end;
            if (*at == ',')
              {
                at++;
                GOTO_STATE(IN_OBJECT);
              }
            else if (*at == '}')
              {
                at++;
                POP();
              }
            else if (!parser->options.ignore_missing_commas)
              {
                RETURN_ERROR(EXPECTED_COMMA);
              }
            else
              GOTO_STATE(IN_OBJECT);
            break;


          CASE(IN_ARRAY_INITIAL):
          CASE(IN_ARRAY):
            if (*at == '{')
              {
                at++;
                PUSH_OBJECT();
                GOTO_STATE(IN_OBJECT_INITIAL);
              }
            else if (*at == '[')
              {
                at++;
                PUSH_ARRAY();
                GOTO_STATE(IN_ARRAY_INITIAL);
              }
            else if (*at == ']')
              {
                at++;
                POP();
              }
            else if (*at == ',' && parser->options.permit_trailing_commas)
              GOTO_STATE(IN_ARRAY_GOT_COMMA);
            else if (maybe_setup_flat_value_state (parser, *at))
              {
                at++;
                GOTO_STATE(IN_ARRAY_VALUE);
              }
            else 
              {
                parser->error_byte = *at;
                if (*at == '.')
                  RETURN_ERROR(BAD_NUMBER);
                else
                  RETURN_ERROR(UNEXPECTED_CHAR);
              }
            break;

          CASE(IN_ARRAY_VALUE):
            switch (scan_flat_value (parser, &at, end))
              {
              case SCAN_ERROR:
                do_callback_error (parser);
                return false;
              case SCAN_END:
                do_callback_flat_value (parser);
                GOTO_STATE(IN_ARRAY_EXPECTING_COMMA);

              case SCAN_IN_VALUE:
                assert(at == end);
                return true;
              }

          CASE(IN_ARRAY_EXPECTING_COMMA):
            switch (parser->whitespace_scanner (parser, &at, end))
              {
              case SCAN_IN_VALUE:
                assert(at == end);
                return true;

              case SCAN_ERROR:
                do_callback_error (parser);
                return false;

              case SCAN_END:
                if (at == end)
                  return true;
                break;
              }
            if (*at == ',')
              {
                at++;
                GOTO_STATE(IN_ARRAY_GOT_COMMA);
              }
            else if (*at == ']')
              {
                at++;
                POP();
              }
            else if (parser->options.ignore_missing_commas && IS_SPACE(*at))
              {
                GOTO_STATE(IN_ARRAY);
              }
            else
              {
                parser->error_byte = *at;
                RETURN_ERROR(UNEXPECTED_CHAR);
              }

          CASE(IN_ARRAY_GOT_COMMA):
            switch (parser->whitespace_scanner (parser, &at, end))
              {
              case SCAN_END:
                break;
              case SCAN_IN_VALUE:
                assert(at==end);
                return true;
              case SCAN_ERROR:
                do_callback_error (parser);
                return false;
              }
            if (at == end)
              return true;

            if (*at == ']')
              {
                if (!parser->options.permit_trailing_commas)
                  RETURN_ERROR(TRAILING_COMMA);
                at++;
                POP();
              }
            else if (*at == '{')
              {
                at++;
                PUSH_OBJECT();
                GOTO_STATE(IN_OBJECT_INITIAL);
              }
            else if (*at == '[')
              {
                at++;
                PUSH_ARRAY();
                GOTO_STATE(IN_ARRAY_INITIAL);
              }
            else if (maybe_setup_flat_value_state (parser, *at))
              {
                at++;
                GOTO_STATE(IN_ARRAY_VALUE);
              }
            else if (*at == ',' && parser->options.ignore_missing_commas)
              {
                at++;
                GOTO_STATE(IN_ARRAY_GOT_COMMA);
              }
            else
              {
                parser->error_byte = *at;
                RETURN_ERROR(UNEXPECTED_CHAR);
              }
        }
    }

at_end:
  return true;

#undef SKIP_CHAR_TYPE
#undef IS_SPACE
#undef SKIP_WS
#undef RETURN_ERROR
#undef GOTO_STATE
#undef PUSH
#undef PUSH_OBJECT
#undef PUSH_ARRAY
#undef POP
#undef CASE
}

JSON_CALLBACK_PARSER_FUNC_DEF
bool
json_callback_parser_end_feed (JSON_CallbackParser *parser)
{
  switch (parser->state)
    {
    case JSON_CALLBACK_PARSER_STATE_INTERIM:
      return true;
    case JSON_CALLBACK_PARSER_STATE_INTERIM_EXPECTING_COMMA:
      return true;
    case JSON_CALLBACK_PARSER_STATE_INTERIM_GOT_COMMA:
      if (parser->options.permit_trailing_commas)
        return true;
      else
        {
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_TRAILING_COMMA;
          return false;
        }
        
    case JSON_CALLBACK_PARSER_STATE_INTERIM_VALUE:
      if (flat_value_can_terminate (parser))
        {
          if (flat_value_state_is_string (parser->flat_value_state))
            {
              do_callback_string(parser);
            }
          else if (flat_value_state_is_number (parser->flat_value_state))
            {
              do_callback_number(parser);
            }
          else if (parser->flat_value_state == FLAT_VALUE_STATE_IN_TRUE && parser->flat_len == 4)
            {
              do_callback_boolean(parser, true);
            }
          else if (parser->flat_value_state == FLAT_VALUE_STATE_IN_FALSE && parser->flat_len == 5)
            {
              do_callback_boolean(parser, false);
            }
          else if (parser->flat_value_state == FLAT_VALUE_STATE_IN_NULL && parser->flat_len == 4)
            {
              do_callback_null(parser);
            }
          else
            {
              assert(false);
            }
          return true;
        }
      else
        {
          parser->error_code = JSON_CALLBACK_PARSER_ERROR_PARTIAL_RECORD;
          return false;
        }

    case JSON_CALLBACK_PARSER_STATE_IN_ARRAY_INITIAL:
    case JSON_CALLBACK_PARSER_STATE_IN_ARRAY:
    case JSON_CALLBACK_PARSER_STATE_IN_ARRAY_EXPECTING_COMMA:
    case JSON_CALLBACK_PARSER_STATE_IN_ARRAY_GOT_COMMA:
    case JSON_CALLBACK_PARSER_STATE_IN_ARRAY_VALUE:
    case JSON_CALLBACK_PARSER_STATE_IN_OBJECT_INITIAL:
    case JSON_CALLBACK_PARSER_STATE_IN_OBJECT:
    case JSON_CALLBACK_PARSER_STATE_IN_OBJECT_EXPECTING_COLON:
    case JSON_CALLBACK_PARSER_STATE_IN_OBJECT_FIELDNAME:
    case JSON_CALLBACK_PARSER_STATE_IN_OBJECT_BARE_FIELDNAME:
    case JSON_CALLBACK_PARSER_STATE_IN_OBJECT_VALUE:
    case JSON_CALLBACK_PARSER_STATE_IN_OBJECT_GOT_COLON:
    case JSON_CALLBACK_PARSER_STATE_IN_OBJECT_EXPECTING_COMMA:
      parser->error_code = JSON_CALLBACK_PARSER_ERROR_PARTIAL_RECORD;
      return false;
    }

  return false;
}


JSON_CALLBACK_PARSER_FUNC_DEF
JSON_CallbackParserError
json_callback_parser_get_error_info(JSON_CallbackParser *parser)
{
  return parser->error_code;
}

//JSON_CALLBACK_PARSER_FUNC_DEF
//void
//json_callback_parser_reset (JSON_CallbackParser *parser)
//{
//}

JSON_CALLBACK_PARSER_FUNC_DEF
void
json_callback_parser_destroy (JSON_CallbackParser *callback_parser)
{
  if (callback_parser->buffer != NULL)
    free (callback_parser->buffer);
  free (callback_parser);
}

