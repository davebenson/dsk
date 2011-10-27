#include <string.h>
#include "dsk.h"

unsigned dsk_strv_length  (char **strs)
{
  unsigned i;
  for (i = 0; strs[i]; i++)
    ;
  return i;
}

char **dsk_strv_concat  (char **a_strs, char **b_strs)
{
  unsigned L = dsk_strv_length (a_strs) + dsk_strv_length (b_strs);
  char **rv = dsk_malloc (sizeof (char *) * (L + 1));
  char **at = rv;
  while (*a_strs)
    *at++ = dsk_strdup (*a_strs++);
  while (*b_strs)
    *at++ = dsk_strdup (*b_strs++);
  *at = NULL;
  return rv;
}

char **dsk_strvv_concat (char ***str_arrays)
{
  unsigned L = 0;
  unsigned i;
  char **rv, **at;
  for (i = 0; str_arrays[i]; i++)
    L += dsk_strv_length (str_arrays[i]);
  at = rv = dsk_malloc (sizeof (char *) * (L + 1));
  for (i = 0; str_arrays[i]; i++)
    {
      char **src = str_arrays[i];
      while (*src)
        *at++ = dsk_strdup (*src++);
    }
  *at = NULL;
  return rv;
}

char **dsk_strv_copy    (char **strs)
{
  char **rv = dsk_malloc (sizeof (char *) * (dsk_strv_length (strs) + 1));
  unsigned i;
  for (i = 0; strs[i]; i++)
    rv[i] = dsk_strdup (strs[i]);
  rv[i] = NULL;
  return rv;
}

void   dsk_strv_free    (char **strs)
{
  if (strs)
    {
      unsigned i;
      for (i = 0; strs[i]; i++)
        dsk_free (strs[i]);
      dsk_free (strs);
    }
}

char    **dsk_strsplit     (const char *str,
                            const char *sep)
{
  char *init[128];
  char **rv = init;
  unsigned alloced = DSK_N_ELEMENTS (init);
  unsigned n = 0;
  unsigned sep_len = strlen (sep);
  char *next_sep = strstr (str, sep);
  while (next_sep)
    {
      if (n == alloced)
        {
          if (rv == init)
            {
              rv = dsk_malloc (sizeof(char*) * alloced * 2);
              memcpy (rv, init, sizeof (char*) * alloced);
            }
          else
            {
              rv = dsk_realloc (rv, sizeof(char*) * alloced * 2);
            }
          alloced += alloced;
        }
      rv[n++] = dsk_strdup_slice (str, next_sep);
      str = next_sep + sep_len;
      next_sep = strstr (str + sep_len, sep);
    }
  if (rv == init)
    {
      rv = dsk_malloc (sizeof (char *) * (n + 2));
      memcpy (rv, init, sizeof (char *) * n);
    }
  else if (n + 1 >= alloced)
    {
      rv = dsk_realloc (rv, sizeof (char *) * (n + 2));
    }
  rv[n++] = dsk_strdup (str);
  rv[n] = NULL;
  return rv;
}
