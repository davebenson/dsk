#include "dsk-common.h"
#include "dsk-utf8.h"
#include <string.h>

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
void
dsk_utf8_skip_nonwhitespace (const char **p_str)
{
  const unsigned char *str = (const unsigned char *) *p_str;
  while (*str)
    {
      switch (*str)
        {
        case ' ': case '\t': case '\r': case '\n': *p_str = (char*) str; return;
        default: str++;
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

dsk_boolean dsk_utf8_decode_unichar (unsigned    buf_len,
                                     const char *buf,
                                     unsigned   *bytes_used_out,
                                     uint32_t   *unicode_value_out)
{
  if (buf_len == 0)
    return DSK_FALSE;
  uint8_t d = *buf;
  if (d < 0x80)
    {
      *bytes_used_out = 1;
      *unicode_value_out = d;
      return DSK_TRUE;
    }
  else if (d < 0xc0)
    return DSK_FALSE;
  else if (d < 0xe0)
    {
      /* two byte sequence */
      if (buf_len < 2)
        return DSK_FALSE;
      *bytes_used_out = 2;
      *unicode_value_out = ((d & 0x1f) << 6) | (((uint8_t*)buf)[1] & 0x3f);
      return DSK_TRUE;
    }
  else if (d < 0xf0)
    {
      /* three byte sequence */
      if (buf_len < 3)
        return DSK_FALSE;
      *bytes_used_out = 3;
      *unicode_value_out = ((d & 0x0f) << 12)
                         | ((((uint8_t*)buf)[1] & 0x3f) << 6)
                         | ((((uint8_t*)buf)[2] & 0x3f) << 0);
      return DSK_TRUE;
    }
  else if (d < 0xf8)
    {
      /* four byte sequence */
      if (buf_len < 4)
        return DSK_FALSE;
      *bytes_used_out = 4;
      *unicode_value_out = ((d & 0x07) << 18)
                         | ((((uint8_t*)buf)[1] & 0x3f) << 12)
                         | ((((uint8_t*)buf)[2] & 0x3f) << 6)
                         | ((((uint8_t*)buf)[3] & 0x3f) << 0);
      return DSK_TRUE;
    }
  else
    return DSK_FALSE;
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

char **
dsk_utf8_split_on_whitespace(const char *str)
{
  unsigned n = 0;
  char *pad[32];
  unsigned alloced = DSK_N_ELEMENTS (pad);
  char **rv = pad;
  for (;;)
    {
      const char *start;
      dsk_utf8_skip_whitespace (&str);
      if (*str == 0)
        break;
      start = str;
      dsk_utf8_skip_nonwhitespace (&str);
      if (n == alloced)
        {
          if (rv == pad)
            {
              rv = dsk_malloc (sizeof(char*) * alloced * 2);
              memcpy (rv, pad, sizeof (pad));
            }
          else
            {
              rv = dsk_realloc (rv, sizeof(char*) * alloced * 2);
            }
          alloced *= 2;
        }
      rv[n++] = dsk_strdup_slice (start, str);
    }
  if (rv == pad)
    {
      char **rrv = dsk_malloc (sizeof (char*) * (n+1));
      memcpy (rrv, rv, sizeof (char*) * n);
      rrv[n] = NULL;
      return rrv;
    }
  else if (n == alloced)
    rv = dsk_realloc (rv, sizeof(char*) * (n+1));
  rv[n] = NULL;
  return rv;
}
