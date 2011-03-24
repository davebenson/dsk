#include <string.h>
#include "dsk.h"

#define BASE64_LINE_LENGTH             64        /* must be a multiple of 4; may be 1 longer at end */

/* --- decoder --- */
typedef struct _DskBase64DecoderClass DskBase64DecoderClass;
struct _DskBase64DecoderClass
{
  DskOctetFilterClass base_class;
};
typedef struct _DskBase64Decoder DskBase64Decoder;
struct _DskBase64Decoder
{
  DskOctetFilter base_instance;

  unsigned char state;		/* 0..3 */
  unsigned char partial;

};



#define dsk_base64_decoder_init NULL
#define dsk_base64_decoder_finalize NULL

static dsk_boolean
dsk_base64_decoder_process (DskOctetFilter *filter,
                            DskBuffer      *out,
                            unsigned        in_length,
                            const uint8_t  *in_data,
                            DskError      **error)
{
  DskBase64Decoder *dec = (DskBase64Decoder *) filter;
  unsigned char state = dec->state;
  unsigned char partial = dec->partial;
  while (in_length > 0)
    {
      int8_t v = dsk_base64_char_to_value[*in_data];
      if ((v & 0x80) == 0)
	{
	  switch (state)
	    {
	    case 0:
	      partial = v << 2;
	      state = 1;
	      break;
	    case 1:
	      dsk_buffer_append_byte (out, partial | (v >> 4));
	      partial = v << 4;
	      state = 2;
	      break;
	    case 2:
	      dsk_buffer_append_byte (out, partial | (v >> 2));
	      partial = v << 6;
	      state = 3;
	      break;
	    case 3:
	      dsk_buffer_append_byte (out, partial | v);
	      state = 0;
	      break;
	    }
	}
      else
        {
	  if (v == -1)
            {
              dsk_set_error (error, "bad base64-character %s", dsk_ascii_byte_name (*in_data));
              return DSK_FALSE;
            }
	}
      in_data++;
      in_length--;
    }
  return DSK_TRUE;
}
#if 0
static dsk_boolean
dsk_base64_decoder_finish(DskOctetFilter *filter,
                          DskBuffer      *out,
                          DskError      **error)
{
  DskBase64Decoder *enc = (DskBase64Decoder *) filter;
  if (enc->state != 0 && enc->partial != 0)
    {
      dsk_set_error (error, "stray bits at end of base64 data");
      return DSK_FALSE;
    }
  return DSK_TRUE;
}
#else
#define dsk_base64_decoder_finish NULL
#endif

DSK_OCTET_FILTER_SUBCLASS_DEFINE(static, DskBase64Decoder, dsk_base64_decoder);


DskOctetFilter *dsk_base64_decoder_new             (void)
{
  DskBase64Decoder *rv = dsk_object_new (&dsk_base64_decoder_class);
  return DSK_OCTET_FILTER (rv);
}
