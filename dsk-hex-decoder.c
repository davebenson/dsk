#include <string.h>
#include "dsk.h"

/* --- decoder --- */
typedef struct _DskHexDecoderClass DskHexDecoderClass;
struct _DskHexDecoderClass
{
  DskOctetFilterClass base_class;
};
typedef struct _DskHexDecoder DskHexDecoder;
struct _DskHexDecoder
{
  DskOctetFilter base_instance;
  unsigned char has_nibble;
  unsigned char nibble;
};

#define dsk_hex_decoder_init NULL
#define dsk_hex_decoder_finalize NULL

static dsk_boolean
dsk_hex_decoder_process (DskOctetFilter *filter,
                            DskBuffer      *out,
                            unsigned        in_length,
                            const uint8_t  *in_data,
                            DskError      **error)
{
  DskHexDecoder *hexdec = (DskHexDecoder *) filter;
  DSK_UNUSED (error);
  while (in_length)
    {
      if (dsk_ascii_isxdigit (*in_data))
        {
          if (hexdec->has_nibble)
            {
              dsk_buffer_append_byte (out,
                                      (hexdec->nibble << 4)
                                      | dsk_ascii_xdigit_value (*in_data));
              hexdec->has_nibble = DSK_FALSE;
            }
          else
            {
              hexdec->nibble = dsk_ascii_xdigit_value (*in_data);
              hexdec->has_nibble = DSK_TRUE;
            }
          in_data++;
          in_length--;
        }
      else if (dsk_ascii_isspace (*in_data))
        {
          in_data++;
          in_length--;
        }
      else
        {
          dsk_set_error (error, "bad character %s in hex-data",
                         dsk_ascii_byte_name (*in_data));
          return DSK_FALSE;
        }
    }
  return DSK_TRUE;
}
static dsk_boolean
dsk_hex_decoder_finish(DskOctetFilter *filter,
                       DskBuffer      *out,
                       DskError      **error)
{
  DskHexDecoder *dec = (DskHexDecoder *) filter;
  DSK_UNUSED (out);
  if (dec->has_nibble)
    {
      dsk_set_error (error, "stray nibble encountered- incomplete byte in hex-data");
      return DSK_FALSE;
    }
  return DSK_TRUE;
}

DSK_OCTET_FILTER_SUBCLASS_DEFINE(static, DskHexDecoder, dsk_hex_decoder);


DskOctetFilter *dsk_hex_decoder_new             (void)
{
  DskHexDecoder *rv = dsk_object_new (&dsk_hex_decoder_class);
  return DSK_OCTET_FILTER (rv);
}
