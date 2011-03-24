#include "dsk.h"

/* See RFC 2045, Section 6.7 */

/* states:
    - spaces that haven't got a non-space
    - \=
    - \=[0-9A-Fa-f]
    - \r
    - =\r
 */
/* conformance notes:
 * - we ignore spaces preceding a newline
 * - we tolerate lowercase hex-digits ['a' - 'f' - technically we only need 'A'-'F']
 * - \r\n (known as CRLF in the spec) is translated to just \n "unix-style"
 */
typedef struct _DskUnquotePrintableClass DskUnquotePrintableClass;
typedef enum
{
  STATE_NORMAL,
  STATE_SPACES,
  STATE_EQUALS,
  STATE_EQUALS_HEX,
  STATE_CR,
  STATE_EQ_CR
} State;
#define MAX_SPACES   76
struct _DskUnquotePrintableClass
{
  DskOctetFilterClass base_class;
};
typedef struct _DskUnquotePrintable DskUnquotePrintable;
struct _DskUnquotePrintable
{
  DskOctetFilter base_filter;
  State state;

  /* state specific state */
  union {
    struct {
      /* for the STATE_SPACES situation.
         TO CONSIDER: Since each space much be either '\t' or ' '
         we could really reduce to a bit-vector */
      unsigned n;
      char buf[MAX_SPACES];
    } spaces;
    uint8_t equal_hex;   /* value of the first nibble */
  } info;
};
static dsk_boolean
dsk_unquote_printable_process (DskOctetFilter *filter,
                               DskBuffer      *out,
                               unsigned        in_length,
                               const uint8_t  *in_data,
                               DskError      **error)
{
  DskUnquotePrintable *qp = (DskUnquotePrintable *) filter;
  const uint8_t *at = in_data;
  unsigned rem = in_length;
  if (rem == 0)
    return DSK_TRUE;
  switch (qp->state)            /* assert: rem != 0 at beginning of cases */
    {
    state__NORMAL:
    case STATE_NORMAL:
      {
        if (*at == ' ' || *at == '\t')
          {
            qp->state = STATE_SPACES;
            qp->info.spaces.n = 1;
            qp->info.spaces.buf[0] = *at;
            at++;
            rem--;
            if (rem == 0)
              return DSK_TRUE;
            goto state__SPACES;
          }
        else if (*at == '=')
          {
            qp->state = STATE_EQUALS;
            at++;
            rem--;
            if (rem == 0)
              return DSK_TRUE;
            goto state__EQUALS;
          }
        else if (*at == '\r')
          {
            at++;
            rem--;
            if (rem == 0)
              return DSK_TRUE;
            goto state__CR;
          }
          /* note that '\n' does not need special handling */
        else
          {
            dsk_buffer_append_byte (out, *at);
            at++;
            rem--;
            if (rem == 0)
              return DSK_TRUE;
            goto state__NORMAL;
          }
      }
    state__SPACES:
    case STATE_SPACES:
      while (rem && (*at == ' ' || *at == '\t'))
        {
          if (qp->info.spaces.n < MAX_SPACES)
            qp->info.spaces.buf[qp->info.spaces.n++] = *at;
          else
            {
              dsk_set_error (error,
                             "unquote-printable: more than %u spaces consecutively",
                             MAX_SPACES);
              return DSK_FALSE;
            }
          at++;
          rem--;
        }
      if (rem == 0)
        return DSK_TRUE;
      if (*at == '\r')
        {
          qp->state = STATE_CR;
          at++;
          rem--;
          if (rem == 0)
            return DSK_TRUE;
          goto state__CR;
        }
      else if (*at == '\n')
        {
          qp->state = STATE_NORMAL;
          goto state__NORMAL;
        }
      else          /* assume non-space */
        {
          dsk_buffer_append (out, qp->info.spaces.n, qp->info.spaces.buf);
          qp->state = STATE_NORMAL;
          goto state__NORMAL;
        }
    state__EQUALS:
    case STATE_EQUALS:
      if (*at == '\n')
        {
          at++;
          rem--;
          qp->state = STATE_NORMAL;
          if (rem == 0)
            return DSK_TRUE;
          goto state__NORMAL;
        }
      else if (*at == '\r')
        {
          at++;
          rem--;
          qp->state = STATE_EQ_CR;
          if (rem == 0)
            return DSK_TRUE;
          goto state__EQ_CR;
        }
      if (rem == 1)
        {
          if (!dsk_ascii_isxdigit (at[0]))
            goto expected_hex_error;
          qp->state = STATE_EQUALS_HEX;
          qp->info.equal_hex = dsk_ascii_xdigit_value (at[0]);
          return DSK_TRUE;
        }
      else if (!dsk_ascii_isxdigit (at[0]) || !dsk_ascii_isxdigit (at[1]))
        goto expected_hex_error;
      else
        {
          dsk_buffer_append_byte (out, (dsk_ascii_xdigit_value (at[0]) << 4)
                                     | dsk_ascii_xdigit_value (at[1]));
          rem -= 2;
          at += 2;
          qp->state = STATE_NORMAL;
          if (rem == 0)
            return DSK_TRUE;
          goto state__NORMAL;
        }

    case STATE_EQUALS_HEX:
      if (!dsk_ascii_isxdigit (at[0]))
        goto expected_hex_error;
      dsk_buffer_append_byte (out, (qp->info.equal_hex << 4)
                                 | dsk_ascii_xdigit_value (at[0]));
      rem--;
      at++;
      qp->state = STATE_NORMAL;
      if (rem == 0)
        return DSK_TRUE;
      goto state__NORMAL;

    state__CR:
    case STATE_CR:
      if (*at == '\n')
        {
          dsk_buffer_append_byte (out, '\n');
          rem--;
          at++;
          qp->state = STATE_NORMAL;
          if (rem == 0)
            return DSK_TRUE;
          goto state__NORMAL;
        }
      else
        goto unexpected_cr;

    state__EQ_CR:
    case STATE_EQ_CR:
      if (*at == '\n')
        {
          rem--;
          at++;
          qp->state = STATE_NORMAL;
          if (rem == 0)
            return DSK_TRUE;
          goto state__NORMAL;
        }
      else
        goto unexpected_cr;
    }
  dsk_assert_not_reached ();

expected_hex_error:
  dsk_set_error (error, "expecting hex-digits after '='");
  return DSK_FALSE;
unexpected_cr:
  dsk_set_error (error, "got CR without newline");
  return DSK_FALSE;
}

static dsk_boolean
dsk_unquote_printable_finish  (DskOctetFilter *filter,
                               DskBuffer      *out,
                               DskError      **error)
{
  DskUnquotePrintable *qp = (DskUnquotePrintable *) filter;
  DSK_UNUSED (out);
  switch (qp->state)
    {
    case STATE_NORMAL:
    case STATE_SPACES:
      return DSK_TRUE;
    case STATE_EQUALS:
    case STATE_EQUALS_HEX:
    case STATE_EQ_CR:
      dsk_set_error (error, "unexpected termination after '='");
      return DSK_FALSE;
    case STATE_CR:
      dsk_set_error (error, "unexpected termination after CR");
      return DSK_FALSE;
    }
  dsk_assert_not_reached ();
  return DSK_TRUE;
}

#define dsk_unquote_printable_init NULL
#define dsk_unquote_printable_finalize NULL
DSK_OCTET_FILTER_SUBCLASS_DEFINE(static, DskUnquotePrintable, dsk_unquote_printable);

DskOctetFilter *dsk_unquote_printable_new            (void)
{
  return dsk_object_new (&dsk_unquote_printable_class);
}
