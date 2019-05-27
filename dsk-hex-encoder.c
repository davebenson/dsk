#include <string.h>
#include "dsk.h"

/* --- encoder --- */
typedef struct _DskHexEncoderClass DskHexEncoderClass;
struct _DskHexEncoderClass
{
  DskSyncFilterClass base_class;
};
typedef struct _DskHexEncoder DskHexEncoder;
struct _DskHexEncoder
{
  DskSyncFilter base_instance;

  unsigned char spaces;
  unsigned char newlines;
  unsigned char cur_line_len;
};



#define dsk_hex_encoder_init NULL
#define dsk_hex_encoder_finalize NULL

static bool
dsk_hex_encoder_process (DskSyncFilter *filter,
                            DskBuffer      *out,
                            unsigned        in_length,
                            const uint8_t  *in_data,
                            DskError      **error)
{
  DskHexEncoder *hexenc = (DskHexEncoder *) filter;
  DSK_UNUSED (error);
  while (in_length--)
    {
      unsigned n = 2;
      char buf[3];
      buf[0] = dsk_ascii_hex_digits[*in_data >> 4];
      buf[1] = dsk_ascii_hex_digits[*in_data & 15];
      in_data++;
      if (hexenc->spaces)
        {
	  if (hexenc->newlines && hexenc->cur_line_len == 60)
	    {
	      buf[2] = '\n';
	      hexenc->cur_line_len = 0;
	    }
	  else 
	    {
	      buf[2] = ' ';
	      hexenc->cur_line_len += 3;
	    }
	  n = 3;
	}
      else if (hexenc->newlines && hexenc->cur_line_len == 60)
        {
	  buf[2] = '\n';
	  n = 3;
	  hexenc->cur_line_len = 0;
	}
      else
	hexenc->cur_line_len += 2;
      dsk_buffer_append_small (out, n, buf);
    }
  return true;
}

static bool
dsk_hex_encoder_finish   (DskSyncFilter *filter,
                          DskBuffer      *out,
                          DskError      **error)
{
  DskHexEncoder *enc = (DskHexEncoder *) filter;
  DSK_UNUSED (error);
  if (enc->newlines && enc->cur_line_len != 0)
    dsk_buffer_append_byte (out, '\n');
  return true;
}

DSK_SYNC_FILTER_SUBCLASS_DEFINE(static, DskHexEncoder, dsk_hex_encoder);


DskSyncFilter *dsk_hex_encoder_new (bool break_lines,
                                     bool include_spaces)
{
  DskHexEncoder *rv = dsk_object_new (&dsk_hex_encoder_class);
  rv->newlines = break_lines ? 1 : 0;
  rv->spaces = include_spaces ? 1 : 0;
  return DSK_SYNC_FILTER (rv);
}
