#include "dsk.h"
/* --- decoder --- */
typedef struct _DskUrlDecoderClass DskUrlDecoderClass;
struct _DskUrlDecoderClass
{
  DskOctetFilterClass base_class;
};
typedef struct _DskUrlDecoder DskUrlDecoder;
struct _DskUrlDecoder
{
  DskOctetFilter base_instance;
  /* state 0 == default
     state 1 == got percent
     state 2 == got percent and one hex digit */
  uint8_t state;
  uint8_t nibble;
};

#define dsk_url_decoder_init NULL
#define dsk_url_decoder_finalize NULL

static dsk_boolean
dsk_url_decoder_process    (DskOctetFilter *filter,
                            DskBuffer      *out,
                            unsigned        in_length,
                            const uint8_t  *in_data,
                            DskError      **error)
{
  DskUrlDecoder *dec = (DskUrlDecoder *) filter;
  if (in_length == 0)
    return DSK_TRUE;
  if (dec->state != 0)
    {
      if (dec->state == 1)
        {
          if (in_length == 1)
            {
              if (!dsk_ascii_isxdigit (in_data[0]))
                goto ERROR;
              dec->state = 2;
              dec->nibble = dsk_ascii_xdigit_value (in_data[0]);
              return DSK_TRUE;
            }
          if (!dsk_ascii_isxdigit (in_data[0])
           || !dsk_ascii_isxdigit (in_data[1]))
            goto ERROR;
          dsk_buffer_append_byte (out,
                                  (dsk_ascii_xdigit_value (in_data[0]) << 4)
                                | (dsk_ascii_xdigit_value (in_data[1])));
          in_data += 2;
          in_length -= 2;
        }
      else
        {
          /* state == 2 */
          if (!dsk_ascii_isxdigit (in_data[0]))
            goto ERROR;
          dsk_buffer_append_byte (out,
                                  (dec->nibble << 4)
                                | (dsk_ascii_xdigit_value (in_data[0])));
          in_data += 1;
          in_length -= 2;
        }
      dec->state = 0;
      if (in_length == 0)
        return DSK_TRUE;
    }
  while (in_length > 0)
    {
      unsigned unquoted_len = 0;
      while (unquoted_len < in_length
          && (in_data[unquoted_len] != '%' && in_data[unquoted_len] != '+'))
        unquoted_len++;
      if (unquoted_len > 0)
        {
          dsk_buffer_append (out, unquoted_len, in_data);
          in_data += unquoted_len;
          in_length -= unquoted_len;
          if (in_length == 0)
            return DSK_TRUE;
        }

      /* handle special bytes */
      if (*in_data == '%')
        {
          if (in_length == 1)
            {
              dec->state = 1;
              return DSK_TRUE;
            }
          else if (in_length == 2)
            {
              if (!dsk_ascii_isxdigit (in_data[1]))
                goto ERROR;
              dec->state = 2;
              dec->nibble = dsk_ascii_xdigit_value (in_data[1]);
              return DSK_TRUE;
            }
          else
            {
              if (!dsk_ascii_isxdigit (in_data[1])
               || !dsk_ascii_isxdigit (in_data[2]))
                goto ERROR;
              dsk_buffer_append_byte (out,
                                  (dsk_ascii_xdigit_value (in_data[1]) << 4)
                                | (dsk_ascii_xdigit_value (in_data[2])));
              in_data += 3;
              in_length -= 3;
            }
        }
      else
        {
          dsk_assert (*in_data == '+');
          dsk_buffer_append_byte (out, ' ');
          in_data++;
          in_length--;
        }
    }
  return DSK_TRUE;

ERROR:
  dsk_set_error (error, "expected two hex digits after '%%'");
  return DSK_FALSE;
}
static dsk_boolean
dsk_url_decoder_finish (DskOctetFilter *filter,
                        DskBuffer      *buffer,
                        DskError      **error)

{
  DskUrlDecoder *dec = (DskUrlDecoder *) filter;
  DSK_UNUSED (buffer);
  if (dec->state != 0)
    {
      dsk_set_error (error, "unterminated %% sequence");
      return DSK_FALSE;
    }
  return DSK_TRUE;
}

DSK_OCTET_FILTER_SUBCLASS_DEFINE(static, DskUrlDecoder, dsk_url_decoder);

DskOctetFilter *
dsk_url_decoder_new (void)
{
  return dsk_object_new (&dsk_url_decoder_class);
}
