#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "dsk-common.h"

void
dsk_die(const char *format, ...)
{
  va_list args;
  fprintf (stderr, "ERROR: ");
  va_start (args, format);
  vfprintf (stderr, format, args);
  va_end (args);
  fprintf (stderr, "\n");
  abort ();
}
void
dsk_error(const char *format, ...)
{
  va_list args;
  fprintf (stderr, "ERROR: ");
  va_start (args, format);
  vfprintf (stderr, format, args);
  va_end (args);
  fprintf (stderr, "\n");
  exit (1);
}
void
dsk_warning(const char *format, ...)
{
  va_list args;
  fprintf (stderr, "WARNING: ");
  va_start (args, format);
  vfprintf (stderr, format, args);
  va_end (args);
  fprintf (stderr, "\n");
}

void *
dsk_malloc (size_t size)
{
  void *rv;
  if (size == 0)
    return NULL;
  rv = malloc (size);
  if (rv == NULL)
    dsk_error ("out-of-memory allocating %u bytes", (unsigned) size);
  return rv;
}
void *
dsk_malloc0 (size_t size)
{
  void *rv;
  if (size == 0)
    return NULL;
  rv = malloc (size);
  if (rv == NULL)
    dsk_error ("out-of-memory allocating %u bytes", (unsigned) size);
  memset (rv, 0, size);
  return rv;
}
void
dsk_free (void *ptr)
{
  if (ptr)
    free (ptr);
}
void *
dsk_realloc (void *ptr, size_t size)
{
  if (ptr == NULL)
    return dsk_malloc (size);
  else if (size == 0)
    {
      dsk_free (ptr);
      return NULL;
    }
  else
    {
      void *rv = realloc (ptr, size);
      if (rv == NULL)
        dsk_error ("out-of-memory re-allocating %u bytes", (unsigned) size);
      return rv;
    }
}

char *
dsk_strdup (const char *str)
{
  if (str == NULL)
    return NULL;
  else
    {
      unsigned length = strlen (str);
      char *rv = dsk_malloc (length + 1);
      memcpy (rv, str, length + 1);
      return rv;
    }
}
char *
dsk_strndup (size_t length, const char *str)
{
  char *rv = dsk_malloc (length + 1);
  memcpy (rv, str, length);
  rv[length] = 0;
  return rv;
}
void *
dsk_memdup (size_t size, const void *ptr)
{
  void *rv = dsk_malloc (size);
  memcpy (rv, ptr, size);
  return rv;
}
char *dsk_strdup_slice (const char *str, const char *end_str)
{
  unsigned length = end_str - str;
  char *rv = dsk_malloc (length + 1);
  memcpy (rv, str, length);
  rv[length] = '\0';
  return rv;
}
void
dsk_bzero_pointers (void *ptrs,
                    unsigned n_ptrs)
{
  void **at = ptrs;
  while (n_ptrs--)
    *at++ = NULL;
}

dsk_boolean
dsk_parse_boolean (const char *str,
                   dsk_boolean *out)
{
  switch (str[0])
    {
    case '0': if (strcmp (str, "0") == 0) goto is_false; break;
    case '1': if (strcmp (str, "1") == 0) goto is_true; break;
    case 'n': if (strcmp (str, "no") == 0) goto is_false; break;
    case 'y': if (strcmp (str, "yes") == 0) goto is_true; break;
    case 'N': if (strcmp (str, "NO") == 0) goto is_false; break;
    case 'Y': if (strcmp (str, "YES") == 0) goto is_true; break;
    case 'f': if (strcmp (str, "false") == 0) goto is_false; break;
    case 't': if (strcmp (str, "true") == 0) goto is_true; break;
    case 'F': if (strcmp (str, "FALSE") == 0) goto is_false; break;
    case 'T': if (strcmp (str, "TRUE") == 0) goto is_true; break;
    default: return DSK_FALSE;
    }
is_true:
  *out = DSK_TRUE;
  return DSK_TRUE;
is_false:
  *out = DSK_FALSE;
  return DSK_TRUE;
}
char *dsk_stpcpy (char *dst, const char *src)
{
  while ((*dst = *src) != '\0')
    {
      src++;
      dst++;
    }
  return dst;
}
