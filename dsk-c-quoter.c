#include "dsk.h"

#define UNESCAPED_CHAR  32

typedef enum
{
  STATE_DEFAULT,
  STATE_OCTAL,
  STATE_TRIGRAPH_1,
  STATE_TRIGRAPH_2
} State;

typedef struct _DskOctetFilterCQuoterClass DskOctetFilterCQuoterClass;
typedef struct _DskOctetFilterCQuoter DskOctetFilterCQuoter;
struct _DskOctetFilterCQuoterClass
{
  DskOctetFilterClass base_class;
};
struct _DskOctetFilterCQuoter
{
  DskOctetFilter base_instance;
  uint8_t last_char;
  uint8_t state;
  uint8_t add_quotes : 1;
  uint8_t protect_trigraphs : 1;
  uint8_t needs_initial_quote : 1;
};
static void dsk_octet_filter_c_quoter_init (DskOctetFilterCQuoter *cquoter)
{
  cquoter->last_char = UNESCAPED_CHAR;
}
#define dsk_octet_filter_c_quoter_finalize NULL
static void write_octal (DskBuffer *buffer, uint8_t c)
{
  char buf[4];
  buf[0] = '\\';
  if (c < 8)
    {
      buf[1] = '0' + c;
      dsk_buffer_append (buffer, 2, buf);
    }
  else if (c < 64)
    {
      buf[1] = '0' + c/8;
      buf[2] = '0' + c%8;
      dsk_buffer_append (buffer, 3, buf);
    }
  else
    {
      buf[1] = '0' + c/64;
      buf[2] = '0' + c/8%8;
      buf[3] = '0' + c%8;
      dsk_buffer_append (buffer, 4, buf);
    }
}

static inline unsigned
scan_unquoted_matter (unsigned length, const uint8_t *data)
{
  unsigned rv = 0;
  while (rv < length
         && ' ' <= data[rv] && data[rv] <= 126
         && data[rv] != '"'
         && data[rv] != '\\')
    rv++;
  return rv;
}

static inline unsigned
scan_unquoted_matter__trigraph (unsigned length, const uint8_t *data)
{
  unsigned rv = 0;
  while (rv < length
         && ' ' <= data[rv] && data[rv] <= 126
         && data[rv] != '"'
         && data[rv] != '?'
         && data[rv] != '\\')
    rv++;
  return rv;
}


static dsk_boolean
dsk_octet_filter_c_quoter_process (DskOctetFilter *filter,
                                   DskBuffer      *out,
                                   unsigned        in_length,
                                   const uint8_t  *in_data,
                                   DskError      **error)
{
  DskOctetFilterCQuoter *cquoter = (DskOctetFilterCQuoter *) filter;
  uint8_t last_char = cquoter->last_char;
  State state = cquoter->state;
  DSK_UNUSED (error);
  if (cquoter->needs_initial_quote)
    {
      dsk_buffer_append_byte (out, '"');
      cquoter->needs_initial_quote = 0;
    }
  if (in_length == 0)
    return DSK_TRUE;
  switch (state)
    {
    case STATE_DEFAULT:
    label_STATE_DEFAULT:
      {
        unsigned sublen = cquoter->protect_trigraphs
                        ? scan_unquoted_matter__trigraph (in_length, in_data)
                        : scan_unquoted_matter (in_length, in_data);
        if (sublen > 0)
          dsk_buffer_append (out, sublen, in_data);
        in_data += sublen;
        in_length -= sublen;
        if (in_length == 0)
          goto return_true;
        else
          {
            /* handle single-character escapes */
            switch (*in_data)
              {
#define WRITE_CASE_SIMPLE_ESCAPED(c, str) \
            case c: \
              dsk_buffer_append (out, 2, str);  \
              in_data++; \
              if (--in_length == 0) \
                { \
                  goto return_true; \
                } \
              goto label_STATE_DEFAULT
              WRITE_CASE_SIMPLE_ESCAPED ('\a', "\\a");
              WRITE_CASE_SIMPLE_ESCAPED ('\b', "\\b");
              WRITE_CASE_SIMPLE_ESCAPED ('\f', "\\f");
              WRITE_CASE_SIMPLE_ESCAPED ('\n', "\\n");
              WRITE_CASE_SIMPLE_ESCAPED ('\r', "\\r");
              WRITE_CASE_SIMPLE_ESCAPED ('\t', "\\t");
              WRITE_CASE_SIMPLE_ESCAPED ('\v', "\\v");
              WRITE_CASE_SIMPLE_ESCAPED ('"' , "\\\"");
              WRITE_CASE_SIMPLE_ESCAPED ('\\', "\\\\");
              case '?':
                dsk_assert (cquoter->protect_trigraphs);
                state = STATE_TRIGRAPH_1;
                dsk_buffer_append_byte (out, '?');
                in_length--;
                in_data++;
                if (in_length == 0)
                  goto return_true;
                goto label_STATE_TRIGRAPH_1;
              }

            last_char = *in_data++;
            state = STATE_OCTAL;
            in_length--;
            if (in_length == 0)
              goto return_true;
            goto label_STATE_OCTAL;
          }
      }
    case STATE_OCTAL:
    label_STATE_OCTAL:
      if (dsk_ascii_isdigit (*in_data))
        {
          /* Write three-octal variant */
          char buf[4] = { '\\',
                          '0' + (*in_data>>6),
                          '0' + ((*in_data>>3)&7),
                          '0' + ((*in_data)&7) };
          dsk_buffer_append (out, 4, buf);
        }
      else
        {
          write_octal (out, last_char);
        }
      state = STATE_DEFAULT;
      goto label_STATE_DEFAULT;
    case STATE_TRIGRAPH_1:
    label_STATE_TRIGRAPH_1:
      if (*in_data == '?')
        goto label_STATE_TRIGRAPH_2;
      state = STATE_DEFAULT;
      goto label_STATE_DEFAULT;
    case STATE_TRIGRAPH_2:
    label_STATE_TRIGRAPH_2:
      if (*in_data == '='               /* trigraph final characters */
       || *in_data == '/'
       || *in_data == '('
       || *in_data == ')'
       || *in_data == '!'
       || *in_data == '<'
       || *in_data == '>'
       || *in_data == '-')
        {
          /* emit quoted ? and next char of non-trigraph */
          dsk_buffer_append_byte (out, '\\');
          dsk_buffer_append_byte (out, '?');
          goto label_STATE_DEFAULT;
        }
      else if (*in_data == '?')
        {
          dsk_buffer_append_byte (out, '?');
          in_data++;
          if (--in_length == 0)
            goto return_true;
          goto label_STATE_TRIGRAPH_2;
        }
      else
        {
          state = STATE_DEFAULT;
          goto label_STATE_DEFAULT;
        }
    }
return_true:
  cquoter->state = state;
  cquoter->last_char = last_char;
  return DSK_TRUE;
}

static dsk_boolean
dsk_octet_filter_c_quoter_finish  (DskOctetFilter *filter,
                                   DskBuffer      *out,
                                   DskError      **error)
{
  DskOctetFilterCQuoter *cquoter = (DskOctetFilterCQuoter *) filter;
  DSK_UNUSED (error);
  if (cquoter->last_char != UNESCAPED_CHAR)
    write_octal (out, cquoter->last_char);
  if (cquoter->add_quotes)
    {
      if (cquoter->needs_initial_quote)
        dsk_buffer_append_byte (out, '"');
      dsk_buffer_append_byte (out, '"');
    }
  return DSK_TRUE;
}

DSK_OCTET_FILTER_SUBCLASS_DEFINE(static, DskOctetFilterCQuoter, dsk_octet_filter_c_quoter);

DskOctetFilter *
dsk_c_quoter_new (dsk_boolean add_quotes,
                  dsk_boolean protect_trigraphs)
{
  DskOctetFilterCQuoter *cq = dsk_object_new (&dsk_octet_filter_c_quoter_class);
  if (add_quotes)
    cq->add_quotes = cq->needs_initial_quote = 1;
  cq->protect_trigraphs = protect_trigraphs ? 1 : 0;
  return DSK_OCTET_FILTER (cq);
}

