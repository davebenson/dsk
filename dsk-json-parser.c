#include "dsk.h"
#include <string.h>
#include <stdlib.h>

typedef enum
{
  JSON_LEX_STATE_INIT,      /* in between tokens */
  JSON_LEX_STATE_TRUE,      /* parsing 'true' or 'TRUE' */
  JSON_LEX_STATE_FALSE,     /* parsing 'false' or 'FALSE' */
  JSON_LEX_STATE_NULL,      /* parsing 'null' or 'NULL'  */
  JSON_LEX_STATE_IN_DQ,     /* in a double-quoted string */
  JSON_LEX_STATE_IN_DQ_BS,  /* in a back-slash sequence in a " string */
  JSON_LEX_STATE_IN_NUMBER  /* in a number */
} JsonLexState;
static const char *lex_state_names[] =
{
  "in between tokens",
  "scanning partial TRUE",
  "scanning partial FALSE",
  "scanning partial NULL",
  "in double-quoted string",
  "in escape-sequence in double-quoted string",
  "in number"
};

typedef enum
{
  STACK_NODE_OBJECT,
  STACK_NODE_ARRAY
} StackNodeType;
typedef struct _StackNode StackNode;
struct _StackNode
{
  unsigned n_subs;
  unsigned subs_alloced;
  StackNodeType type;
  union
  {
    DskJsonMember *members;             /* for object */
    DskJsonValue **values;              /* for array */
  } u;
};

typedef enum
{
  PARSE_INIT,                           /* expecting value */
  PARSE_EXPECTING_MEMBER,
  PARSE_GOT_MEMBER_NAME,
  PARSE_GOT_MEMBER_COLON,               /* expecting subvalue */
  PARSE_GOT_MEMBER,                     /* expecting , or } */
  PARSE_EXPECTING_ELEMENT,              /* expecting subvalue */
  PARSE_GOT_ELEMENT                     /* expecting , or ] */
} ParseState;
static const char *parse_state_expecting_strings[] =
{
  "expecting value",                    /* INIT */
  "expecting member name",              /* EXPECTING_MEMBER */
  "expecting :",                        /* GOT_MEMBER_NAME */
  "expecting member subvalue",          /* GOT_MEMBER_COLON */
  "expecting , or }",                   /* GOT_MEMBER */
  "expecting array subvalue",           /* EXPECTING_ELEMENT */
  "expecting , or ]",                   /* GOT_ELEMENT */
};

typedef struct _ValueQueue ValueQueue;
struct _ValueQueue
{
  DskJsonValue *value;
  ValueQueue *next;
};

struct _DskJsonParser
{
  /* --- lexing --- */
  JsonLexState lex_state;
  char *str;             /* for IN_NUMBER and IN_DQ* */
  unsigned str_len;
  unsigned str_alloced;

  char bs_sequence[7];      /* in IN_DQ_BS -- no backslash */
  unsigned char bs_sequence_len;
  unsigned fixed_n_chars;   /* for TRUE, FALSE, NULL */
  unsigned line_no;
  uint16_t utf16_surrogate;     /* must be 0 outside of IN_DQ and IN_DQ_BS */
  const char *static_syntax_error;

  /* --- parsing --- */
  ParseState parse_state;

  unsigned stack_size;
  unsigned stack_alloced;
  StackNode *stack;

  /* --- queue of values --- */
  ValueQueue *queue_head, *queue_tail;
};
static void
append_to_string_buffer (DskJsonParser *parser,
                         size_t         len,
                         const uint8_t *data)
{
  if (parser->str_len + len > parser->str_alloced)
    {
      parser->str_alloced = parser->str_len + len;
      parser->str = dsk_realloc (parser->str, parser->str_alloced);
    }
  memcpy (parser->str + parser->str_len, data, len);
  parser->str_len += len;
}
static void
append_char_to_string_buffer (DskJsonParser *parser,
                              char           c)
{
  uint8_t b = c;
  append_to_string_buffer (parser, 1, &b);
}


typedef enum
{
  JSON_TOKEN_LBRACE,
  JSON_TOKEN_RBRACE,
  JSON_TOKEN_COMMA,
  JSON_TOKEN_STRING,
  JSON_TOKEN_COLON,
  JSON_TOKEN_LBRACKET,
  JSON_TOKEN_RBRACKET,
  JSON_TOKEN_NUMBER,
  JSON_TOKEN_TRUE,
  JSON_TOKEN_FALSE,
  JSON_TOKEN_NULL
} JsonTokenType;
static const char *token_names[] =
{ "left-brace '{'",
  "right-brace '}'",
  "comma ','",
  "quoted-string",
  "colon ':'",
  "left-bracket '['",
  "right-bracket ']'",
  "number",
  "true",
  "false",
  "null"
};


#define STACK_TOP(p)  ((p)->stack[(p)->stack_size - 1])

static void
stack_increase_subs_alloced (StackNode *node)
{
  node->subs_alloced += 1;
  node->u.members = DSK_RENEW (DskJsonMember, node->u.members, node->subs_alloced);
}

/* --- parsing --- */
static void
handle_subvalue          (DskJsonParser *parser,
                          DskJsonValue  *take)
{
  if (parser->stack_size > 0)
    {
      /* add to last stack node */
      if (STACK_TOP (parser).type == STACK_NODE_OBJECT)
        {
          STACK_TOP (parser).u.members[STACK_TOP (parser).n_subs-1].value = take;
          parser->parse_state = PARSE_GOT_MEMBER;
        }
      else
        {
          if (STACK_TOP (parser).n_subs == STACK_TOP (parser).subs_alloced)
            stack_increase_subs_alloced (&STACK_TOP (parser));
          STACK_TOP (parser).u.values[STACK_TOP (parser).n_subs++] = take;
          parser->parse_state = PARSE_GOT_ELEMENT;
        }
    }
  else
    {
      /* add to queue */
 DskBuffer b = DSK_BUFFER_INIT;dsk_json_value_to_buffer (take, -1, &b);
 dsk_warning("adding value to queue: %s\n",dsk_buffer_empty_to_string (&b));
      ValueQueue *q = DSK_NEW (ValueQueue);
      q->value = take;
      q->next = NULL;
      if (parser->queue_head == NULL)
        parser->queue_head = q;
      else
        parser->queue_tail->next = q;
      parser->queue_tail = q;
      parser->parse_state = PARSE_INIT;
    }
}

static dsk_boolean
is_allowed_subvalue_token (JsonTokenType  token)
{
  return token == JSON_TOKEN_LBRACE
      || token == JSON_TOKEN_STRING
      || token == JSON_TOKEN_LBRACKET
      || token == JSON_TOKEN_NUMBER
      || token == JSON_TOKEN_TRUE
      || token == JSON_TOKEN_FALSE
      || token == JSON_TOKEN_NULL;
}


static void
object_finished (DskJsonParser *parser)
{
  /* create object */
  DskJsonValue *object;
  unsigned i;
  unsigned n_members = STACK_TOP(parser).n_subs;
  DskJsonMember *members = STACK_TOP(parser).u.members;
  object = dsk_json_value_new_object (n_members, members);

  /* pop the stack (free members' names) */
  for (i = 0; i < n_members; i++)
    dsk_free (members[i].name);
  parser->stack_size--;

  /* deal with the new object */
  handle_subvalue (parser, object);
}

static void
array_finished (DskJsonParser *parser)
{
  /* create array */
  DskJsonValue *array;
  unsigned n_values = STACK_TOP(parser).n_subs;
  DskJsonValue **values = STACK_TOP(parser).u.values;
  array = dsk_json_value_new_array (n_values, values);

  /* pop the stack */
  parser->stack_size--;

  /* deal with the new array */
  handle_subvalue (parser, array);
}

static void
push_stack (DskJsonParser *parser,
            StackNodeType  type)
{
  if (parser->stack_alloced == parser->stack_size)
    {
      parser->stack_alloced += 1;
      parser->stack = DSK_RENEW (StackNode, parser->stack, parser->stack_alloced);
      parser->stack[parser->stack_size].n_subs = 0;
      parser->stack[parser->stack_size].subs_alloced = 0;
      parser->stack[parser->stack_size].u.members = NULL;
    }
  parser->stack[parser->stack_size].n_subs = 0;
  parser->stack[parser->stack_size++].type = type;
}
static void
handle_expected_subvalue (DskJsonParser *parser,
                          JsonTokenType  token)
{
  switch (token)
    {
    case JSON_TOKEN_LBRACE:
      push_stack (parser, STACK_NODE_OBJECT);
      parser->parse_state = PARSE_EXPECTING_MEMBER;
      break;
    case JSON_TOKEN_STRING:
      handle_subvalue (parser, dsk_json_value_new_string (parser->str_len,
                                                          parser->str));
      break;
    case JSON_TOKEN_LBRACKET:
      push_stack (parser, STACK_NODE_ARRAY);
      parser->parse_state = PARSE_EXPECTING_ELEMENT;
      break;
    case JSON_TOKEN_NUMBER:
      {
      /* parse number */
      double value;
      append_char_to_string_buffer (parser, 0);
      value = strtod (parser->str, NULL);

      handle_subvalue (parser, dsk_json_value_new_number (value));
      break;
      }

    case JSON_TOKEN_TRUE:
      handle_subvalue (parser, dsk_json_value_new_boolean (DSK_TRUE));
      break;
    case JSON_TOKEN_FALSE:
      handle_subvalue (parser, dsk_json_value_new_boolean (DSK_FALSE));
      break;
    case JSON_TOKEN_NULL:
      handle_subvalue (parser, dsk_json_value_new_null ());
      break;

    default:
      dsk_assert_not_reached ();
    }
}


static dsk_boolean
handle_token (DskJsonParser *parser,
              JsonTokenType  token,
              DskError     **error)
{
  switch (parser->parse_state)
    {
    case PARSE_INIT:                           /* expecting value */
    case PARSE_GOT_MEMBER_COLON:               /* expecting subvalue */
      if (!is_allowed_subvalue_token (token))
        goto bad_token;
      handle_expected_subvalue (parser, token);
      break;
    case PARSE_EXPECTING_ELEMENT:              /* expecting subvalue */
      if (token == JSON_TOKEN_RBRACKET)
        array_finished (parser);
      else if (!is_allowed_subvalue_token (token))
        goto bad_token;
      else
        handle_expected_subvalue (parser, token);
      break;
    case PARSE_EXPECTING_MEMBER:
      if (token == JSON_TOKEN_STRING)
        {
          /* add new member; copy string */
          char *name;
          DskJsonMember *member;
          if (STACK_TOP (parser).n_subs == STACK_TOP (parser).subs_alloced)
            stack_increase_subs_alloced (&STACK_TOP (parser));
          name = dsk_strndup (parser->str_len, parser->str);
          member = &STACK_TOP (parser).u.members[STACK_TOP (parser).n_subs++];
          member->name = name;
          member->value = NULL;

          parser->parse_state = PARSE_GOT_MEMBER_NAME;
        }
      else if (token == JSON_TOKEN_RBRACE)
        object_finished (parser);
      else
        goto bad_token;
      break;
    case PARSE_GOT_MEMBER_NAME:
      if (token != JSON_TOKEN_COLON)
        goto bad_token;
      else
        parser->parse_state = PARSE_GOT_MEMBER_COLON;
      break;
    case PARSE_GOT_MEMBER:                     /* expecting , or } */
      if (token == JSON_TOKEN_COMMA)
        parser->parse_state = PARSE_EXPECTING_MEMBER;
      else if (token == JSON_TOKEN_RBRACE)
        {
          object_finished (parser);
        }
      else
        goto bad_token;
      break;
    case PARSE_GOT_ELEMENT:                    /* expecting , or ] */
      if (token == JSON_TOKEN_COMMA)
        parser->parse_state = PARSE_EXPECTING_ELEMENT;
      else if (token == JSON_TOKEN_RBRACKET)
        {
          /* create array */
          DskJsonValue *array;
          array = dsk_json_value_new_array (STACK_TOP(parser).n_subs,
                                            STACK_TOP(parser).u.values);

          /* pop the stack */
          parser->stack_size--;

          /* deal with the new array */
          handle_subvalue (parser, array);
        }
      else
        goto bad_token;
      break;
    }
  parser->str_len = 0;
  return DSK_TRUE;

bad_token:
  dsk_set_error (error, "got unexpected token %s: %s (line %u)",
                 token_names[token],
                 parse_state_expecting_strings[parser->parse_state],
                 parser->line_no);
  parser->str_len = 0;
  return DSK_FALSE;
}


/* --- lexing --- */
dsk_boolean
dsk_json_parser_feed     (DskJsonParser *parser,
                          size_t         n_bytes,
                          const uint8_t *bytes,
                          DskError     **error)
{
  while (n_bytes > 0)
    {
      switch (parser->lex_state)
        {
        case JSON_LEX_STATE_INIT:
          while (n_bytes > 0 && dsk_ascii_isspace (*bytes))
            {
              if (*bytes == '\n')
                parser->line_no++;
              bytes++;
              n_bytes--;
            }
          if (n_bytes == 0)
            break;
          switch (*bytes)
            {
            case 't': case 'T':
              printf("got t for true\n");
              parser->lex_state = JSON_LEX_STATE_TRUE;
              parser->fixed_n_chars = 1;
              bytes++;
              n_bytes--;
              break;
            case 'f': case 'F':
              parser->lex_state = JSON_LEX_STATE_FALSE;
              parser->fixed_n_chars = 1;
              bytes++;
              n_bytes--;
              break;
            case 'n': case 'N':
              parser->lex_state = JSON_LEX_STATE_NULL;
              parser->fixed_n_chars = 1;
              bytes++;
              n_bytes--;
              break;
            case '"':
              parser->lex_state = JSON_LEX_STATE_IN_DQ;
              parser->str_len = 0;
              bytes++;
              n_bytes--;
              break;
            case '-': case '+':
            case '0': case '1': case '2': case '3': case '4': 
            case '5': case '6': case '7': case '8': case '9': 
              parser->lex_state = JSON_LEX_STATE_IN_NUMBER;
              parser->str_len = 0;
              append_to_string_buffer (parser, 1, bytes);
              bytes++;
              n_bytes--;
              break;

#define WRITE_CHAR_TOKEN_CASE(character, SHORTNAME) \
            case character: \
              if (!handle_token (parser, JSON_TOKEN_##SHORTNAME, error)) \
                return DSK_FALSE; \
              n_bytes--; \
              bytes++; \
              break
            WRITE_CHAR_TOKEN_CASE('{', LBRACE);
            WRITE_CHAR_TOKEN_CASE('}', RBRACE);
            WRITE_CHAR_TOKEN_CASE('[', LBRACKET);
            WRITE_CHAR_TOKEN_CASE(']', RBRACKET);
            WRITE_CHAR_TOKEN_CASE(',', COMMA);
            WRITE_CHAR_TOKEN_CASE(':', COLON);
#undef WRITE_CHAR_TOKEN_CASE

            case '\n':
              parser->line_no++;
              n_bytes--;
              bytes++;
              break;
            case '\t': case '\r': case ' ':
              n_bytes--;
              bytes++;
              break;
            default:
              dsk_set_error (error,
                             "unexpected character %s in json (line %u)",
                             dsk_ascii_byte_name (*bytes), parser->line_no);
              return DSK_FALSE;
            }
          break;

#define WRITE_FIXED_BAREWORD_CASE(SHORTNAME, lc, UC, length) \
        case JSON_LEX_STATE_##SHORTNAME: \
          if (parser->fixed_n_chars == length) \
            { \
              /* are we at end of string? */ \
              if (dsk_ascii_isalnum (*bytes)) \
                { \
                  dsk_set_error (error,  \
                                 "got %s after '%s' (line %u)", \
                                 dsk_ascii_byte_name (*bytes), lc, \
                                 parser->line_no); \
                  return DSK_FALSE; \
                } \
              else \
                { \
                  parser->lex_state = JSON_LEX_STATE_INIT; \
                  if (!handle_token (parser, JSON_TOKEN_##SHORTNAME, \
                                     error)) \
                    return DSK_FALSE; \
                } \
            } \
          else if (*bytes == lc[parser->fixed_n_chars] \
                || *bytes == UC[parser->fixed_n_chars]) \
            { \
              parser->fixed_n_chars += 1; \
              n_bytes--; \
              bytes++; \
            } \
          else \
            { \
              dsk_set_error (error, \
                           "unexpected character %s (parsing %s) (line %u)", \
                           dsk_ascii_byte_name (*bytes), UC, parser->line_no); \
              return DSK_FALSE; \
            } \
          break;
        WRITE_FIXED_BAREWORD_CASE(TRUE, "true", "TRUE", 4);
        WRITE_FIXED_BAREWORD_CASE(FALSE, "false", "FALSE", 5);
        WRITE_FIXED_BAREWORD_CASE(NULL, "null", "NULL", 4);
#undef WRITE_FIXED_BAREWORD_CASE

        case JSON_LEX_STATE_IN_DQ:
          if (*bytes == '"')
            {
              // TODO ASSERT utf16_surrogate == 0
              if (!handle_token (parser, JSON_TOKEN_STRING, error))
                return DSK_FALSE;
              bytes++;
              n_bytes--;
              parser->lex_state = JSON_LEX_STATE_INIT;
            }
          else if (*bytes == '\\')
            {
              n_bytes--;
              bytes++;
              parser->bs_sequence_len = 0;
              parser->lex_state = JSON_LEX_STATE_IN_DQ_BS;
            }
          else
            {
              // TODO ASSERT utf16_surrogate == 0
              unsigned i;
              if (*bytes == '\n')
                parser->line_no++;
              for (i = 1; i < n_bytes; i++)
                if (bytes[i] == '"' || bytes[i] == '\\')
                  break;
                else if (bytes[i] == '\n')
                  parser->line_no++;
              append_to_string_buffer (parser, i, bytes);
              n_bytes -= i;
              bytes += i;
            }
          break;
        case JSON_LEX_STATE_IN_DQ_BS:
          if (parser->bs_sequence_len == 0)
            {
              switch (*bytes)
                {
#define WRITE_BS_CHAR_CASE(bschar, cchar) \
                case bschar: \
                  /* TODO ASSERT utf16_surrogate == 0 */ \
                  append_char_to_string_buffer (parser, cchar); \
                  bytes++; \
                  n_bytes--; \
                  parser->lex_state = JSON_LEX_STATE_IN_DQ; \
                  break
                WRITE_BS_CHAR_CASE('b', '\b');
                WRITE_BS_CHAR_CASE('f', '\f');
                WRITE_BS_CHAR_CASE('n', '\n');
                WRITE_BS_CHAR_CASE('r', '\r');
                WRITE_BS_CHAR_CASE('t', '\t');
                WRITE_BS_CHAR_CASE('/', '/');
                WRITE_BS_CHAR_CASE('"', '"');
                WRITE_BS_CHAR_CASE('\\', '\\');
#undef WRITE_BS_CHAR_CASE
                case 'u':
                  parser->bs_sequence[parser->bs_sequence_len++] = *bytes++;
                  n_bytes--;
                  break;
                default:
                  dsk_set_error (error,
                               "invalid character %s after '\\' (line %u)",
                               dsk_ascii_byte_name (*bytes), parser->line_no);
                  return DSK_FALSE;
                }
            }
          else
            {
              /* must be \uxxxx */
              if (!dsk_ascii_isxdigit (*bytes))
                {
                  dsk_set_error (error,
                               "expected 4 hex digits after \\u, got %s (line %u)",
                               dsk_ascii_byte_name (*bytes), parser->line_no);
                  return DSK_FALSE;
                }
              parser->bs_sequence[parser->bs_sequence_len++] = *bytes++;
              n_bytes--;
              if (parser->bs_sequence_len == 5)
                {
                  char utf8buf[8];
                  unsigned value;
                  parser->bs_sequence[5] = 0;
                  value = strtoul (parser->bs_sequence + 1, NULL, 16);
                  if (DSK_UTF16_LO_SURROGATE_START <= value
                   && value <= DSK_UTF16_LO_SURROGATE_END)
                    {
                      if (parser->utf16_surrogate != 0)
                        {
                          dsk_set_error (error,
                                       "expected second half of UTF16 surrogate \\u%04u was followed by \\%04u, line %u",
                                       parser->line_no, parser->utf16_surrogate, value);
                          return DSK_FALSE;
                        }
                      append_to_string_buffer (parser,
                                               dsk_utf8_encode_unichar (utf8buf, value),
                                               (const uint8_t *) utf8buf);
                    }
                  else if (DSK_UTF16_HI_SURROGATE_START <= value
                        && value <= DSK_UTF16_HI_SURROGATE_END)
                    {
                      if (parser->utf16_surrogate != 0)
                        {
                          dsk_set_error (error,
                                       "got two first-half surrogate pairs (UTF16 surrogate \\u%04u was followed by \\%04u), line %u",
                                       parser->utf16_surrogate, value, parser->line_no);
                          return DSK_FALSE;
                        }
                      parser->utf16_surrogate = value;
                    }
                  else
                    {
                      if (parser->utf16_surrogate == 0)
                        {
                          dsk_set_error (error,
                                       "second half of UTF16 surrogate \\u%04u was not preceded by utf16, line %u", 
                                       parser->utf16_surrogate, parser->line_no);
                          return DSK_FALSE;
                        }
                      uint32_t code = dsk_utf16_surrogate_pair_to_codepoint (parser->utf16_surrogate, value);
                      append_to_string_buffer (parser,
                                               dsk_utf8_encode_unichar (utf8buf, code),
                                               (const uint8_t *) utf8buf);
                      parser->utf16_surrogate = 0;
                    }
                  parser->lex_state = JSON_LEX_STATE_IN_DQ;
                }
              else
                {
                  dsk_set_error (error,
                               "internal error: expected 4 hex digits (line %u)",
                               parser->line_no);
                  return DSK_FALSE;
                }
            }
          break;
        case JSON_LEX_STATE_IN_NUMBER:
          if (dsk_ascii_isdigit (*bytes)
           || *bytes == '.'
           || *bytes == 'e'
           || *bytes == 'E'
           || *bytes == '+'
           || *bytes == '-')
            {
              append_to_string_buffer (parser, 1, bytes);
              bytes++;
              n_bytes--;
            }
          else
            {
              /* append the number token */
              if (!handle_token (parser, JSON_TOKEN_NUMBER, error))
                return DSK_FALSE;

              /* go back to init state (do not consume character) */
              parser->lex_state = JSON_LEX_STATE_INIT;
            }
          break;
        default:
          dsk_error ("unhandled lex state %u", parser->lex_state);
        }
    }
  return DSK_TRUE;
}


DskJsonParser *dsk_json_parser_new      (void)
{
  DskJsonParser *parser = DSK_NEW0 (DskJsonParser);
  return parser;
}

DskJsonValue * dsk_json_parser_pop      (DskJsonParser *parser)
{
  DskJsonValue *rv;
  ValueQueue *q;
  if (parser->queue_head == NULL)
    return NULL;
  rv = parser->queue_head->value;
  q = parser->queue_head;
  parser->queue_head = q->next;
  if (parser->queue_head == NULL)
    parser->queue_tail = NULL;
  dsk_free (q);
  return rv;
}

dsk_boolean    dsk_json_parser_finish   (DskJsonParser *parser,
                                         DskError     **error)
{
  switch (parser->lex_state)
    {
    case JSON_LEX_STATE_TRUE:
      if (parser->fixed_n_chars == 4)
        {
          if (!handle_token (parser, JSON_TOKEN_TRUE, error))
            return DSK_FALSE;
          break;
        }
      else
        goto bad_lex_state;
    case JSON_LEX_STATE_FALSE:
      if (parser->fixed_n_chars == 5)
        {
          if (!handle_token (parser, JSON_TOKEN_FALSE, error))
            return DSK_FALSE;
          break;
        }
      else
        goto bad_lex_state;
    case JSON_LEX_STATE_NULL:
      if (parser->fixed_n_chars == 4)
        {
          if (!handle_token (parser, JSON_TOKEN_NULL, error))
            return DSK_FALSE;
          break;
        }
      else
        goto bad_lex_state;
    case JSON_LEX_STATE_IN_DQ:
    case JSON_LEX_STATE_IN_DQ_BS:
      goto bad_lex_state;
    case JSON_LEX_STATE_IN_NUMBER:
      if (!handle_token (parser, JSON_TOKEN_NUMBER, error))
        return DSK_FALSE;
      break;
    }
  if (parser->parse_state != PARSE_INIT)
    {
      dsk_set_error (error, "unfinished %s",
                     parser->stack[0].type == STACK_NODE_OBJECT ? "object" : "array");
      return DSK_FALSE;
    }
  return DSK_TRUE;


bad_lex_state:
  dsk_set_error (error, "invalid lex state %s at end-of-file",
                 lex_state_names[parser->lex_state]);
  return DSK_FALSE;
}

void           dsk_json_parser_destroy  (DskJsonParser *parser)
{
  unsigned i, j;
  DskJsonValue *v;
  while ((v = dsk_json_parser_pop (parser)) != NULL)
    dsk_json_value_free (v);
  for (i = 0; i < parser->stack_size; i++)
    {
      switch (parser->stack[i].type)
        {
        case STACK_NODE_OBJECT:
          for (j = 0; j < parser->stack[i].n_subs; j++)
            {
              dsk_free (parser->stack[i].u.members[j].name);
              if (parser->stack[i].u.members[j].value)
                dsk_json_value_free (parser->stack[i].u.members[j].value);
            }
          break;
        case STACK_NODE_ARRAY:
          for (j = 0; j < parser->stack[i].n_subs; j++)
            dsk_json_value_free (parser->stack[i].u.values[j]);
        }
      dsk_free (parser->stack[i].u.members);
    }
  for (     ; i < parser->stack_alloced; i++)
    dsk_free (parser->stack[i].u.members);
  dsk_free (parser->stack);

  dsk_free (parser->str);

  dsk_free (parser);
}
DskJsonValue * dsk_json_parse           (size_t         len,
                                         const uint8_t *data,
                                         DskError     **error)
{
  DskJsonParser *parser = dsk_json_parser_new ();
  if (!dsk_json_parser_feed (parser, len, data, error)
   || !dsk_json_parser_finish (parser, error))
    {
      dsk_json_parser_destroy (parser);
      return NULL;
    }
  DskJsonValue *rv = dsk_json_parser_pop (parser);
  if (rv == NULL)
    {
      dsk_set_error (error, "json parser did not return any values");
      dsk_json_parser_destroy (parser);
      return NULL;
    }
  DskJsonValue *test = dsk_json_parser_pop (parser);
  if (test != NULL)
    {
      dsk_set_error (error, "json parser returned multiple values, but only one expected");
      dsk_json_parser_destroy (parser);
      dsk_json_value_free (test);
      dsk_json_value_free (rv);
      return NULL;
    }
  dsk_json_parser_destroy (parser);
  return rv;
}
