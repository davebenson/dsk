#include "dsk.h"

/* See RFC 2045, Section 6.7 */
typedef struct _DskQuotePrintableClass DskQuotePrintableClass;
struct _DskQuotePrintableClass
{
  DskOctetFilterClass base_class;
};
typedef struct _DskQuotePrintable DskQuotePrintable;
struct _DskQuotePrintable
{
  DskOctetFilter base_filter;
  unsigned cur_line_length;
};
#define IS_PASSTHROUGH(c)   \
      ((33 <= (c) && (c) <= 60) || (62 <= (c) && (c) <= 126))
#define MAX_LINE_LENGTH 76
static dsk_boolean
dsk_quote_printable_process (DskOctetFilter *filter,
                             DskBuffer      *out,
                             unsigned        in_length,
                             const uint8_t  *in_data,
                             DskError      **error)
{
  DskQuotePrintable *qp = (DskQuotePrintable *) filter;
  unsigned cur_len = qp->cur_line_length;
  unsigned i;
  DSK_UNUSED (error);
  for (i = 0; i < in_length; i++)
    {
      uint8_t c = in_data[i];
      if (IS_PASSTHROUGH (c))
        {
          dsk_buffer_append_byte (out, c);
          cur_len++;
          if (cur_len >= MAX_LINE_LENGTH)
            {
              dsk_buffer_append (out, 3, "=\r\n");
              cur_len = 0;
            }
        }
      else
        {
          char tmp[3];
          if (cur_len + 3 > MAX_LINE_LENGTH)
            {
              dsk_buffer_append (out, 3, "=\r\n");
              cur_len = 0;
            }
          tmp[0] = '=';
          tmp[1] = dsk_ascii_uppercase_hex_digits[c>>4];
          tmp[2] = dsk_ascii_uppercase_hex_digits[c&15];
          dsk_buffer_append (out, 3, tmp);
        }
    }
  qp->cur_line_length = cur_len;
  return DSK_TRUE;
}

/* TODO: if we never need this, simple #define dsk_quote_printable_finish to NULL */
static dsk_boolean
dsk_quote_printable_finish  (DskOctetFilter *filter,
                             DskBuffer      *out,
                             DskError      **error)
{
  DSK_UNUSED (filter);
  DSK_UNUSED (out);
  DSK_UNUSED (error);
  return DSK_TRUE;
}
#define dsk_quote_printable_init NULL
#define dsk_quote_printable_finalize NULL
DSK_OCTET_FILTER_SUBCLASS_DEFINE(static, DskQuotePrintable, dsk_quote_printable);

DskOctetFilter *dsk_quote_printable_new            (void)
{
  return dsk_object_new (&dsk_quote_printable_class);
}
