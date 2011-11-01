#include "dsk-common.h"
#include "dsk-ascii.h"

char dsk_ascii_hex_digits[16] = {
  '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'
};
char dsk_ascii_uppercase_hex_digits[16] = {
  '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'
};
unsigned char dsk_ascii_chartable[256] = 
{
#include "dsk-ascii-chartable.inc"
};

/* xdigit_values and digit_values are machine-generated */
#include "dsk-digit-chartables.inc"

int dsk_ascii_xdigit_value (int c)
{
  return xdigit_values[(uint8_t)c];
}

int dsk_ascii_digit_value (int c)
{
  return digit_values[(uint8_t)c];
}
#include "dsk-byte-name-table.inc"
const char *dsk_ascii_byte_name(unsigned char byte)
{
  return byte_name_str + (byte_name_offsets[byte]);
}

int dsk_ascii_strcasecmp  (const char *a, const char *b)
{
  while (*a && *b)
    {
      char A = dsk_ascii_tolower (*a);
      char B = dsk_ascii_tolower (*b);
      if (A < B)
        return -1;
      else if (A > B)
        return +1;
      a++;
      b++;
    }
  if (*a)
    return +1;
  else if (*b)
    return -1;
  else
    return 0;
}

int dsk_ascii_strncasecmp (const char *a, const char *b, size_t max_len)
{
  unsigned rem = max_len;
  while (*a && *b && rem)
    {
      char A = dsk_ascii_tolower (*a);
      char B = dsk_ascii_tolower (*b);
      if (A < B)
        return -1;
      else if (A > B)
        return +1;
      a++;
      b++;
      rem--;
    }
  if (rem == 0)
    return 0;
  if (*a)
    return +1;
  else if (*b)
    return -1;
  else
    return 0;
}

void
dsk_ascii_strchomp (char *inout)
{
  char *end;
  for (end = inout; *end; end++)
    {
    }
  while (end > inout && dsk_ascii_isspace (*(end-1)))
    end--;
  *end = '\0';
}
void  dsk_ascii_strup   (char *str)
{
  while (*str)
    {
      if ('a' <= *str && *str <= 'z')
        *str -= ('a' - 'A');
      str++;
    }
}
void  dsk_ascii_strdown (char *str)
{
  while (*str)
    {
      if ('A' <= *str && *str <= 'Z')
        *str += ('a' - 'A');
      str++;
    }
}
