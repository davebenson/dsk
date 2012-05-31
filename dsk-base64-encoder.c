#include <string.h>
#include "dsk.h"

#define BASE64_LINE_LENGTH             64        /* must be a multiple of 4; may be 1 longer at end */

/* --- encoder --- */
typedef struct _DskBase64EncoderClass DskBase64EncoderClass;
struct _DskBase64EncoderClass
{
  DskOctetFilterClass base_class;
};
typedef struct _DskBase64Encoder DskBase64Encoder;
struct _DskBase64Encoder
{
  DskOctetFilter base_instance;

  /* if -1, then don't break lines; in output bytes; never 0 */
  int length_remaining;

  unsigned char state;
  unsigned char partial[3];
};



#define dsk_base64_encoder_init NULL
#define dsk_base64_encoder_finalize NULL

static inline void
pump_block (const uint8_t *input,
            uint8_t       *output)
{
  output[0] = dsk_base64_value_to_char[input[0] >> 2];
  output[1] = dsk_base64_value_to_char[((input[0]&3) << 4) | (input[1] >> 4)];
  output[2] = dsk_base64_value_to_char[((input[1]&15) << 2) | (input[2] >> 6)];
  output[3] = dsk_base64_value_to_char[input[2] & 63];
}

static inline void
emit_block (DskBase64Encoder *enc,
            const uint8_t *input,
            DskBuffer *out)
{
  uint8_t output[4];
  pump_block (input, output);
  dsk_buffer_append (out, 4, output);
  if (enc->length_remaining != -1)
    {
      if (enc->length_remaining <= 4)
        {
          dsk_buffer_append_byte (out, '\n');
          enc->length_remaining = BASE64_LINE_LENGTH;
        }
      else
        enc->length_remaining -= 4;
    }
}

static dsk_boolean
dsk_base64_encoder_process (DskOctetFilter *filter,
                            DskBuffer      *out,
                            unsigned        in_length,
                            const uint8_t  *in_data,
                            DskError      **error)
{
  DskBase64Encoder *enc = (DskBase64Encoder *) filter;
  DSK_UNUSED (error);
  if (enc->state + in_length < 3)
    {
      memcpy (enc->partial + enc->state, in_data, in_length);
      enc->state += in_length;
      return DSK_TRUE;
    }
  memcpy (enc->partial + enc->state, in_data, 3 - enc->state);
  emit_block (enc, enc->partial, out);
  in_data += 3 - enc->state;
  in_length -= 3 - enc->state;
  while (in_length >= 3)
    {
      emit_block (enc, in_data, out);
      in_data += 3;
      in_length -= 3;
    }
  memcpy (enc->partial, in_data, in_length);
  enc->state = in_length;
  return DSK_TRUE;
}
static dsk_boolean
dsk_base64_encoder_finish(DskOctetFilter *filter,
                          DskBuffer      *out,
                          DskError      **error)
{
  DskBase64Encoder *enc = (DskBase64Encoder *) filter;
  DSK_UNUSED (error);
  if (enc->state != 0)
    {
      uint8_t output[4];
      /* Zero out remainder of 'partial' */
      enc->partial[enc->state] = 0;
      enc->partial[2] = 0;

      /* Base-64 encoder it */
      pump_block (enc->partial, output);

      /* Emit the base64 data */
      if (enc->state == 1)
        dsk_buffer_append (out, 2, output);
      else
        dsk_buffer_append (out, 3, output);
    }
  dsk_buffer_append_byte (out, '=');
  return DSK_TRUE;
}

DSK_OCTET_FILTER_SUBCLASS_DEFINE(static, DskBase64Encoder, dsk_base64_encoder);


DskOctetFilter *dsk_base64_encoder_new             (dsk_boolean break_lines)
{
  DskBase64Encoder *rv = dsk_object_new (&dsk_base64_encoder_class);
  rv->length_remaining = break_lines ? BASE64_LINE_LENGTH : -1;
  return DSK_OCTET_FILTER (rv);
}
