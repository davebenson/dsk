#include <string.h>
#include "dsk.h"

#define UNESCAPED_CHAR  32

typedef struct _DskOctetFilterCUnquoterClass DskOctetFilterCUnquoterClass;
typedef struct _DskOctetFilterCUnquoter DskOctetFilterCUnquoter;
struct _DskOctetFilterCUnquoterClass
{
  DskOctetFilterClass base_class;
};
struct _DskOctetFilterCUnquoter
{
  DskOctetFilter base_instance;
  uint8_t partial_octal;
  uint8_t state;
  uint8_t remove_initial_quote : 1;
  uint8_t remove_quotes : 1;
};
typedef enum
{
  STATE_DEFAULT,
  STATE_AFTER_BACKSLASH,
  STATE_AFTER_1_OCTAL,
  STATE_AFTER_2_OCTAL
} State;
#define dsk_octet_filter_c_unquoter_init NULL
#define dsk_octet_filter_c_unquoter_finalize NULL
#define WRITE_CASE_SIMPLE_ESCAPED(c, str) \
            case c: \
              dsk_buffer_append (out, 2, str);  \
              in_data++; \
              if (--in_length == 0) \
                { \
                  cunquoter->last_char = UNESCAPED_CHAR; \
                  return DSK_TRUE; \
                } \
              goto unescaped_char_loop
#define WRITE_SINGLE_CHAR_ESCAPED_CASES \
            WRITE_CASE_SIMPLE_ESCAPED ('\a', "\\a"); \
            WRITE_CASE_SIMPLE_ESCAPED ('\b', "\\b"); \
            WRITE_CASE_SIMPLE_ESCAPED ('\f', "\\f"); \
            WRITE_CASE_SIMPLE_ESCAPED ('\n', "\\n"); \
            WRITE_CASE_SIMPLE_ESCAPED ('\r', "\\r"); \
            WRITE_CASE_SIMPLE_ESCAPED ('\t', "\\t"); \
            WRITE_CASE_SIMPLE_ESCAPED ('\v', "\\v"); \
            WRITE_CASE_SIMPLE_ESCAPED ('"' , "\\\""); \
            WRITE_CASE_SIMPLE_ESCAPED ('\\', "\\\\");
static dsk_boolean
dsk_octet_filter_c_unquoter_process (DskOctetFilter *filter,
                                   DskBuffer      *out,
                                   unsigned        in_length,
                                   const uint8_t  *in_data,
                                   DskError      **error)
{
  DskOctetFilterCUnquoter *cunquoter = (DskOctetFilterCUnquoter *) filter;
  if (in_length == 0)
    return DSK_TRUE;
  if (cunquoter->remove_initial_quote)
    {
      if (*in_data == '"')
        {
          in_data++;
          in_length--;
          if (in_length == 0)
            return DSK_TRUE;
        }
      else
        {
          dsk_set_error (error, "expected '\"' start of string");
          cunquoter->remove_initial_quote = 0;
          return DSK_FALSE;
        }
    }
  switch (cunquoter->state)
    {
    case STATE_DEFAULT:
    state_default:
      {
        const uint8_t *bs = memchr (in_data, '\\', in_length);
        unsigned n;
        if (bs == NULL)
          {
            dsk_buffer_append (out, in_length, in_data);
            cunquoter->state = STATE_DEFAULT;
            return DSK_TRUE;
          }
        n = bs - in_data;
        dsk_buffer_append (out, n, in_data);
        in_data = bs + 1;
        in_length -= n + 1;
        if (in_length == 0)
          {
            cunquoter->state = STATE_AFTER_BACKSLASH;
            return DSK_TRUE;
          }
      }
      goto state_after_backslash;
    case STATE_AFTER_BACKSLASH:
    state_after_backslash:
      switch (*in_data)
        {
#define WRITE_CASE(post_bs_letter, out_char) \
        case post_bs_letter: \
          dsk_buffer_append_byte (out, out_char); \
          in_data++; \
          in_length--; \
          if (in_length == 0) \
            { \
              cunquoter->state = STATE_DEFAULT; \
              return DSK_TRUE; \
            } \
          goto state_default
        WRITE_CASE ('a', '\a');
        WRITE_CASE ('b', '\b');
        WRITE_CASE ('f', '\f');
        WRITE_CASE ('n', '\n');
        WRITE_CASE ('r', '\r');
        WRITE_CASE ('t', '\t');
        WRITE_CASE ('v', '\v');
        case '\'': case '"': case '\\': case '?':
          dsk_buffer_append_byte (out, *in_data);
          in_data++;
          in_length--;
          if (in_length == 0)
            {
              cunquoter->state = STATE_DEFAULT;
              return DSK_TRUE;
            }
          goto state_default;
        case '0': case '1': case '2': case '3':
        case '4': case '5': case '6': case '7':
          cunquoter->partial_octal = *in_data - '0';
          in_data++;
          in_length--;
          if (in_length == 0)
            {
              cunquoter->state = STATE_AFTER_1_OCTAL;
              return DSK_TRUE;
            }
          goto state_after_1_octal;

        default:
          dsk_set_error (error, "expected character %s after backslash",
                         dsk_ascii_byte_name (*in_data));
          return DSK_FALSE;
        }
#undef WRITE_CASE
    case STATE_AFTER_1_OCTAL:
    state_after_1_octal:
      if ('0' <= *in_data && *in_data <= '7')
        {
          cunquoter->partial_octal <<= 3;
          cunquoter->partial_octal += *in_data - '0';
          in_data++;
          in_length--;
          if (in_length == 0)
            {
              cunquoter->state = STATE_AFTER_2_OCTAL;
              return DSK_TRUE;
            }
          goto state_after_2_octal;
        }
      else
        {
          dsk_buffer_append_byte (out, cunquoter->partial_octal);
          goto state_default;
        }
    case STATE_AFTER_2_OCTAL:
    state_after_2_octal:
      if ('0' <= *in_data && *in_data <= '7')
        {
          if (cunquoter->partial_octal >= 32)
            {
              dsk_set_error (error, "octal value too large parsing c-quoted data");
              return DSK_FALSE;
            }
          cunquoter->partial_octal <<= 3;
          cunquoter->partial_octal += *in_data - '0';
          dsk_buffer_append_byte (out, cunquoter->partial_octal);
          in_data++;
          in_length--;
          if (in_length == 0)
            {
              cunquoter->state = STATE_DEFAULT;
              return DSK_TRUE;
            }
          goto state_default;
        }
      else
        {
          dsk_buffer_append_byte (out, cunquoter->partial_octal);
          goto state_default;
        }
    }
  dsk_return_val_if_reached (NULL, DSK_FALSE);
}

static dsk_boolean
dsk_octet_filter_c_unquoter_finish  (DskOctetFilter *filter,
                                   DskBuffer      *out,
                                   DskError      **error)
{
  DskOctetFilterCUnquoter *cunquoter = (DskOctetFilterCUnquoter *) filter;
  switch (cunquoter->state)
    {
    case STATE_DEFAULT:
      break;
    case STATE_AFTER_BACKSLASH:
      dsk_set_error (error, "unterminated backslash escape sequence");
      return DSK_FALSE;
    case STATE_AFTER_1_OCTAL:
    case STATE_AFTER_2_OCTAL:
      dsk_buffer_append_byte (out, cunquoter->partial_octal);
      break;
    }
  return DSK_TRUE;
}

DSK_OCTET_FILTER_SUBCLASS_DEFINE(static, DskOctetFilterCUnquoter, dsk_octet_filter_c_unquoter);

DskOctetFilter *
dsk_c_unquoter_new (dsk_boolean remove_quotes)
{
  DskOctetFilterCUnquoter *cu = dsk_object_new (&dsk_octet_filter_c_unquoter_class);
  if (remove_quotes)
    cu->remove_quotes = cu->remove_initial_quote = 1;
  return DSK_OCTET_FILTER (cu);
}
