#include <string.h>
#include "dsk.h"
#include "dsk-tmp-array-macros.h"

size_t
dsk_strv_length  (char **strs)
{
  unsigned i;
  for (i = 0; strs[i]; i++)
    ;
  return i;
}
void
dsk_strv_lengths (char **strs,
                  size_t *n_strings_out,            /* optional */
                  size_t *total_string_bytes_out)   /* optional */
{
  if (total_string_bytes_out == NULL)
    {
      if (n_strings_out != NULL)
        *n_strings_out = dsk_strv_length (strs);
    }
  else
    {
      size_t n, total;
      for (n = total = 0; *strs != NULL; strs++, n++)
        total += strlen (*strs);
      if (n_strings_out != NULL)
        *n_strings_out = n;
      *total_string_bytes_out = total;
    }
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
  DSK_TMP_ARRAY_DECLARE(char *, arr, 128);
  unsigned sep_len = strlen (sep);
  char *next_sep = strstr (str, sep);
  while (next_sep)
    {
      char *str = dsk_strdup_slice (str, next_sep);
      DSK_TMP_ARRAY_APPEND (arr, str);
      str = next_sep + sep_len;
      next_sep = strstr (str + sep_len, sep);
    }
  DSK_TMP_ARRAY_APPEND (arr, NULL);
  char **rv;
  DSK_TMP_ARRAY_CLEAR_TO_ALLOCATION (arr, rv);
  return rv;
}

char **
dsk_strv_copy_compact (char **strv)
{
  size_t i, n, total;
  char **rv, *rv_chars;
  dsk_strv_lengths (strv, &n, &total);
  rv = dsk_malloc ((n + 1) * sizeof (char *) + total + n);
  rv_chars = (char *) (rv + n + 1);
  for (i = 0; i < n; i++)
    {
      size_t len = strlen (strv[i]);
      rv[i] = rv_chars;
      memcpy (rv_chars, strv[i], len + 1);
      rv_chars += len + 1;
    }
  rv[n] = NULL;
  return rv;
}
