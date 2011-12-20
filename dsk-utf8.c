#include "dsk-common.h"
#include "dsk-utf8.h"
#include <string.h>



/* See: http://www.bogofilter.org/pipermail/bogofilter/2003-March/001889.html */

/* UTF-8: c2 a0      U+00A0 NO-BREAK SPACE 
   UTF-8: e1 9a 80   U+1680 OGHAM SPACE MARK 
   UTF-8: e2 80 80   U+2000 EN QUAD 
   UTF-8: e2 80 81   U+2001 EM QUAD 
   UTF-8: e2 80 84   U+2004 THREE-PER-EM SPACE 
   UTF-8: e2 80 85   U+2005 FOUR-PER-EM SPACE 
   UTF-8: e2 80 87   U+2007 FIGURE SPACE 
   UTF-8: e2 80 88   U+2008 PUNCTUATION SPACE 
   UTF-8: e2 80 89   U+2009 THIN SPACE 
   UTF-8: e2 80 8a   U+200A HAIR SPACE 
   UTF-8: e2 80 8b   U+200B ZERO WIDTH SPACE 
   UTF-8: e2 81 9f   U+205F MEDIUM MATHEMATICAL SPACE 
   UTF-8: e3 80 80   U+3000 IDEOGRAPHIC SPACE 
 */
void
dsk_utf8_skip_whitespace (const char **p_str)
{
  const unsigned char *str = (const unsigned char *) *p_str;
  while (*str)
    {
      switch (*str)
        {
        case ' ': case '\t': case '\r': case '\n': str++;
        case 0xe1: if (str[1] == 0x9a) str += 2;
                   else { *p_str = (const char *) str; return; }
        case 0xe2: if (str[1] == 0x80
                    && (str[2] == 0x80 /* u+2000 en quad */
                     || str[2] == 0x81 /* u+2001 em quad */
                     || str[2] == 0x84 /* u+2004 three-per-em space */
                     || str[2] == 0x85 /* u+2005 FOUR-PER-EM SPACE */
                     || str[2] == 0x87 /* u+2007 FIGURE SPACE  */
                     || str[2] == 0x88 /* u+2008 PUNCTUATION SPACE */
                     || str[2] == 0x89 /* u+2009 THIN SPACE  */
                     || str[2] == 0x8a /* u+200A HAIR SPACE  */
                     || str[2] == 0x8b))/* u+200B ZERO WIDTH SPACE  */
                     {
                       str += 3;
                     }
                  else if (str[1] == 0x81
                        && str[2] == 0x9f)
                     {
                       /* U+205F MEDIUM MATHEMATICAL SPACE  */
                       str += 3;
                     }
                   else 
                     {
                       *p_str = (const char *) str;
                       return;
                     }
                   break;
        case 0xe3: if (str[1] == 0x80
                    && str[2] == 0x80)
                     {
                       /* U+3000 IDEOGRAPHIC SPACE */
                       str += 3;
                     }
                   else
                     {
                       *p_str = (const char *) str;
                       return;
                     }
                   break;
        default: *p_str = (const char *) str; return;
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
              rv = DSK_NEW_ARRAY (char*, alloced * 2);
              memcpy (rv, pad, sizeof (pad));
            }
          else
            {
              rv = DSK_RENEW (char *, rv, alloced * 2);
            }
          alloced *= 2;
        }
      rv[n++] = dsk_strdup_slice (start, str);
    }
  if (rv == pad)
    {
      char **rrv = DSK_NEW_ARRAY (char *, n + 1);
      memcpy (rrv, rv, sizeof (char*) * n);
      rrv[n] = NULL;
      return rrv;
    }
  else if (n == alloced)
    rv = dsk_realloc (rv, sizeof(char*) * (n+1));
  rv[n] = NULL;
  return rv;
}
