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
