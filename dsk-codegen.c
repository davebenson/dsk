#include <string.h>
#include "dsk.h"

char * dsk_codegen_mixedcase_normalize       (const char *mixedcase)
{
  char *rv = dsk_strdup (mixedcase);
  rv[0] = dsk_ascii_toupper (rv[0]);
  return rv;
}

char * dsk_codegen_mixedcase_to_uppercase    (const char *mixedcase)
{
  unsigned out_len = 0;
  const char *at;
  for (at = mixedcase; *at; at++)
    if (dsk_ascii_isupper (*at))
      out_len++;
  out_len += (at - mixedcase);

  dsk_boolean suppress_underscore = DSK_TRUE;
  char *rv = dsk_malloc (out_len + 1);
  char *rv_at = rv;
  for (at = mixedcase; *at; at++)
    {
      if (dsk_ascii_isupper (*at))
        {
          if (!suppress_underscore)
            *rv_at++ = '_';
          suppress_underscore = DSK_TRUE;
          *rv_at++ = *at;
        }
      else
        {
          *rv_at++ = dsk_ascii_toupper (*at);
          suppress_underscore = DSK_FALSE;
        }
    }
  *rv_at = 0;
  return rv;
}

char * dsk_codegen_mixedcase_to_lowercase    (const char *mixedcase)
{
  unsigned out_len = 0;
  const char *at;
  for (at = mixedcase; *at; at++)
    if (dsk_ascii_isupper (*at))
      out_len++;
  out_len += (at - mixedcase);

  dsk_boolean suppress_underscore = DSK_TRUE;
  char *rv = dsk_malloc (out_len + 1);
  char *rv_at = rv;
  for (at = mixedcase; *at; at++)
    {
      if (dsk_ascii_isupper (*at))
        {
          if (!suppress_underscore)
            *rv_at++ = '_';
          suppress_underscore = DSK_TRUE;
          *rv_at++ = dsk_ascii_tolower (*at);
        }
      else
        {
          *rv_at++ = *at;
          suppress_underscore = DSK_FALSE;
        }
    }
  *rv_at = 0;
  return rv;
}

char * dsk_codegen_lowercase_to_mixedcase    (const char *lowercase)
{
  char *rv = dsk_malloc (strlen (lowercase) + 1);
  const char *in = lowercase;
  char *out = rv;
  *out++ = dsk_ascii_toupper (*in++);
  while (*in)
    {
      if (*in == '_')
        {
          if (in[1] == '_')
            {
              *out++ = '_';
              in++;
            }
          else if (in[1] != 0)
            {
              *out++ = dsk_ascii_toupper (in[1]);
              in += 2;
            }
          else
            {
              in++;
            }
        }
      else
        *out++ = *in++;
    }
  *out = 0;
  return rv;
}

#if 0
void dsk_codegen_function_render (DskCodegenFunction *function,
                                  DskPrint           *target)
{
  ...
}
#endif
