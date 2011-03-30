#include "dsk-common.h"
#include "dsk-utf8.h"

void
dsk_utf8_skip_whitespace (const char **p_str)
{
  const unsigned char *str = (const unsigned char *) *p_str;
  while (*str)
    {
      switch (*str)
        {
        case ' ': case '\t': case '\r': case '\n': str++;
        default: *p_str = (const char *) str; return;
                 /* TODO: handle other spaces */
        }
    }
  *p_str = (const char *) str;
}


unsigned dsk_utf8_encode_unichar (char *outbuf,
                                  uint32_t c)
{
  /* stolen from glib */
  unsigned len = 0;    
  int first;
  int i;

  if (c < 0x80)
    {
      first = 0;
      len = 1;
    }
  else if (c < 0x800)
    {
      first = 0xc0;
      len = 2;
    }
  else if (c < 0x10000)
    {
      first = 0xe0;
      len = 3;
    }
   else if (c < 0x200000)
    {
      first = 0xf0;
      len = 4;
    }
  else if (c < 0x4000000)
    {
      first = 0xf8;
      len = 5;
    }
  else
    {
      first = 0xfc;
      len = 6;
    }

  if (outbuf)
    {
      for (i = len - 1; i > 0; --i)
	{
	  outbuf[i] = (c & 0x3f) | 0x80;
	  c >>= 6;
	}
      outbuf[0] = c | first;
    }

  return len;
}


DskUtf8ValidationResult
dsk_utf8_validate        (unsigned    length,
                          const char *data,
                          unsigned   *length_out)
{
  const char *start = data;
  const char *end = data + length;
  while (data < end)
    {
      uint8_t d = *data;
      if (d < 0x80)
        {
          data++;
        }
      else if (d < 0xc0)
        goto invalid;
      else if (d < 0xe0)
        {
          /* two byte sequence */
          if (data + 1 == end)
            goto too_short;
          if ((d & 0x1e) == 0)
            goto invalid;
          data += 2;
        }
      else if (d < 0xf0)
        {
          /* three byte sequence */
          if (data + 1 == end)
            goto too_short;
          if ((d & 0x0f) == 0 && (data[1] & 0x20) == 0)
            goto invalid;
          if ((((uint8_t)data[1]) & 0xc0) != 0x80)
            goto invalid;
          if (data + 2 == end)
            goto too_short;
          if ((((uint8_t)data[2]) & 0xc0) != 0x80)
            goto invalid;
          data += 3;
        }
      else if (d < 0xf8)
        {
          /* four byte sequence */
          if (data + 1 == end)
            goto too_short;
          if ((d & 0x07) == 0 && (data[1] & 0x30) == 0)
            goto invalid;
          if ((((uint8_t)data[1]) & 0xc0) != 0x80)
            goto invalid;
          if (data + 2 == end)
            goto too_short;
          if ((((uint8_t)data[2]) & 0xc0) != 0x80)
            goto invalid;
          if (data + 3 == end)
            goto too_short;
          if ((((uint8_t)data[3]) & 0xc0) != 0x80)
            goto invalid;
          data += 4;
        }
      else
        goto invalid;
    }
  *length_out = end - start;
  return DSK_UTF8_VALIDATION_SUCCESS;

invalid:
  *length_out = data - start;
  return DSK_UTF8_VALIDATION_INVALID;

too_short:
  *length_out = data - start;
  return DSK_UTF8_VALIDATION_PARTIAL;
}
