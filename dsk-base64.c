#include "dsk.h"

#define BASE64_LINE_LENGTH  64    /* TODO: look this up!!!!!! */

/* Map from 0..63 as characters. */
char dsk_base64_value_to_char[64] = {
#include "dsk-base64-char-table.inc"
};


/* Map from character to 0..63
 * or -1 for bad byte; -2 for whitespace; -3 for equals */
int8_t dsk_base64_char_to_value[256] = {
#include "dsk-base64-value-table.inc"
};

ptrdiff_t dsk_base64_decode (DskBase64DecodeProcessor *p,
                             size_t len,
                             const uint8_t *in_data,
                             uint8_t *out_data,
                             DskError **error)
{
  int8_t v;
  uint8_t *init_out_data = out_data;
  uint8_t varr[4];
  unsigned varr_len;

  if (len == 0)
    return 0;

  switch (p->state)
    {
    case 0: goto blockwise_transfer;
    case 1: goto handle_state_1;
    case 2: goto handle_state_2;
    case 3: goto handle_state_3;
    }

handle_state_1:
  while ((v=dsk_base64_char_to_value[*in_data]) < 0)
    {
      if (v == -3)
        goto bad_byte;
      in_data++;
      len--;
      if (len == 0)
        return out_data - init_out_data;
    }
  *out_data++ = p->partial | ((uint8_t)v >> 4);
  p->state = 2;
  p->partial = (uint8_t )v << 4;

handle_state_2:
  while ((v=dsk_base64_char_to_value[*in_data]) < 0)
    {
      if (v == -3)
        goto bad_byte;
      in_data++;
      len--;
      if (len == 0)
        return out_data - init_out_data;
    }
  *out_data++ = p->partial | ((uint8_t)v >> 2);
  p->state = 3;
  p->partial = (uint8_t )v << 6;

handle_state_3:
  while ((v=dsk_base64_char_to_value[*in_data]) < 0)
    {
      if (v == -3)
        goto bad_byte;
      in_data++;
      len--;
      if (len == 0)
        return out_data - init_out_data;
    }
  *out_data++ = p->partial | (uint8_t) v;
  p->state = 0;

blockwise_transfer:
  varr_len = 0;
  while (len > 0)
    {
      // convert 4 input bytes to 3 output bytes
      int8_t tmp = dsk_base64_char_to_value[*in_data];
      switch (tmp)
        {
        case -1:
          goto bad_byte;
        case -2: case -3:
          break;
        default:
          varr[varr_len++] = tmp;
          if (varr_len == 4)
            {
              *out_data++ = (varr[0] << 6) | (varr[1] >> 4);
              *out_data++ = (varr[1] << 4) | (varr[2] >> 2);
              *out_data++ = (varr[2] << 2) | (varr[3]);
              varr_len = 0;
            }
          break;
        }
      in_data++;
      len--;
    }
  switch (len)
    {
    case 0:
      p->state = 0;
      break;
    case 1:
      p->partial = varr[0] << 2;
      p->state = 1;
      break;
    case 2:
      *out_data++ = ((uint8_t)varr[0] << 2) | ((uint8_t)varr[1] >> 4);
      p->partial = (uint8_t)varr[1] << 4;
      p->state = 2;
      break;
    case 3:
      *out_data++ = ((uint8_t)varr[0] << 2) | ((uint8_t)varr[1] >> 4);
      *out_data++ = ((uint8_t)varr[1] << 4) | ((uint8_t)varr[2] >> 2);
      p->partial = (uint8_t)varr[2] << 6;
      p->state = 3;
      break;
    }
  return out_data - init_out_data;

bad_byte:
  *error = dsk_error_new ("invalid byte %s in base64",
                          dsk_ascii_byte_name(*in_data));
  return -1;
}
