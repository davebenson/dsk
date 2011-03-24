#include "dsk.h"
/* --- decoder --- */
typedef struct _DskUrlEncoderClass DskUrlEncoderClass;
struct _DskUrlEncoderClass
{
  DskOctetFilterClass base_class;
};
typedef struct _DskUrlEncoder DskUrlEncoder;
struct _DskUrlEncoder
{
  DskOctetFilter base_instance;
};

#define dsk_url_encoder_init NULL
#define dsk_url_encoder_finalize NULL

static dsk_boolean
dsk_url_encoder_process    (DskOctetFilter *filter,
                            DskBuffer      *out,
                            unsigned        in_length,
                            const uint8_t  *in_data,
                            DskError      **error)
{
  DSK_UNUSED (filter);
  DSK_UNUSED (error);
  while (in_length > 0)
    {
      unsigned unquoted_len = 0;
      while (unquoted_len < in_length
          && dsk_ascii_istoken (in_data[unquoted_len]))
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
      if (*in_data == ' ')
        {
          dsk_buffer_append_byte (out, '+');
        }
      else
        {
          char buf[3] = { '%', dsk_ascii_hex_digits[*in_data >> 4], dsk_ascii_hex_digits[*in_data & 0xf] };
          dsk_buffer_append_small (out, 3, buf);
        }
      in_data++;
      in_length--;
    }
  return DSK_TRUE;
}
#define dsk_url_encoder_finish NULL

DSK_OCTET_FILTER_SUBCLASS_DEFINE(static, DskUrlEncoder, dsk_url_encoder);

DskOctetFilter *
dsk_url_encoder_new (void)
{
  return dsk_object_new (&dsk_url_encoder_class);
}
