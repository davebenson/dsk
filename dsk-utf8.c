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
