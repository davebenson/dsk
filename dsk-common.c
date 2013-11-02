#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "dsk-common.h"
#include "dsk-ascii.h"

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
    dsk_die ("out-of-memory allocating %u bytes", (unsigned) size);
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
    dsk_die ("out-of-memory allocating %u bytes", (unsigned) size);
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
        dsk_die ("out-of-memory re-allocating %u bytes", (unsigned) size);
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

char *dsk_strdup_vprintf (const char *str, va_list args)
{
  size_t size;
  va_list a;
  char init_buf[256];
  va_copy (a, args);
  size = vsnprintf (init_buf, sizeof (init_buf), str, a);
  va_end (a);
  if (size >= sizeof (init_buf))
    {
      char *rv = dsk_malloc (size + 1);
      vsnprintf (rv, size+1, str, args);
      return rv;
    }
  else
    return dsk_strdup (init_buf);
}
char *dsk_strdup_printf (const char *str, ...)
{
  va_list args;
  char *rv;
  va_start (args, str);
  rv = dsk_strdup_vprintf (str, args);
  va_end (args);
  return rv;
}
void  dsk_strstrip (char *str_inout)
{
  if (dsk_ascii_isspace (*str_inout))
    {
      char *end_spaces = str_inout + 1;
      while (dsk_ascii_isspace (*end_spaces))
        end_spaces++;
      char *end = end_spaces;
      while (*end)
        end++;
      while (end > end_spaces && dsk_ascii_isspace (*(end-1)))
        end--;
      memmove (str_inout, end_spaces, end - end_spaces);
      str_inout[end - end_spaces] = 0;
    }
  else
    dsk_strchomp (str_inout);
}


void  dsk_strchomp (char *str_inout)
{
  char *end = str_inout;
  while (*end)
    end++;
  while (end > str_inout && dsk_ascii_isspace (*(end-1)))
    end--;
  *end = 0;
}

void *dsk_memrchr  (const void *mem, int c, size_t n)
{
  /* TODO: use builtin on glibc 2.1.91 and above */
  char *at = (char *) mem + n;
  char cc = c;
  while (n--)
    if (*(--at) == cc)
      return at;
  return NULL;
}
