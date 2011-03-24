#include <string.h>
#include "dsk.h"

/* Dump key-values after dsk_mime_key_values_scan() */
#define DEBUG_MIME_KEY_VALUES  0

#define SKIP_WS(ptr) while (dsk_ascii_isspace (*ptr)) ptr++

dsk_boolean dsk_mime_key_values_scan (const char *str,
                                      unsigned    max_kv,
                                      DskMimeKeyValueInplace *kv,
                                      unsigned   *n_kv_out,
                                      DskError  **error)
{
  unsigned n = 0;

  /* we assume in what follows that kv[n] is value,
     so i guess we have to handle max_kv==0 somewhere.
     bleh.  maybe should be assertion (max_kv != 0) instead. */
  if (max_kv == 0)
    goto return_true;

  while (*str)
    {
      SKIP_WS (str);
      if (*str == 0)
        goto return_true;
      if (dsk_ascii_istoken (*str))
        {
          kv[n].key_start = str++;
          while (dsk_ascii_istoken (*str))
            str++;
          kv[n].key_end = str;
          SKIP_WS (str);
          if (*str == '=')
            {
              str++;
              SKIP_WS (str);
              if (*str == '"')
                {
                  /* quoted value */
                  str++;                /* skip " */
                  kv[n].value_start = str;
                  while (*str != '"' && *str != '\0')
                    str++;
                  if (*str == 0)
                    {
                      /* error: no matching quote */
                      dsk_set_error (error, "no matching '\"'");
                      return DSK_FALSE;
                    }
                  kv[n].value_end = str;
                  n++;
                  str++;                /* skip " */

                  SKIP_WS (str);
                  if (*str == ';')
                    str++;
                }
              else
                {
                  /* bare-value */
                  kv[n].value_start = str;
                  while (*str != ';'
                      && *str != '\0'
                      && !dsk_ascii_isspace (*str))
                    str++;
                  kv[n].value_end = str;
                  n++;
                  if (*str == 0 || n == max_kv)
                    goto return_true;
                  str++;            /* skip ';' */
                  SKIP_WS (str);
                }
            }
          else if (*str == ';')
            {
              /* no value */
              kv[n].value_start = kv[n].value_end = NULL;
              n++;
              if (n == max_kv)
                goto return_true;
              str++;
            }
          else if (*str == 0)
            {
              /* no value */
              kv[n].value_start = kv[n].value_end = NULL;
              n++;
              goto return_true;
            }
          else
            {
              /* syntax error */
              dsk_set_error (error, "unexpected char %s in MIME header",
                             dsk_ascii_byte_name (*str));
              return DSK_FALSE;
            }
        }
      else
        {
          dsk_set_error (error, "unexpected char %s in MIME header",
                         dsk_ascii_byte_name (*str));
          return DSK_FALSE;
        }
    }
return_true:
#if DEBUG_MIME_KEY_VALUES
  {
    unsigned i;
    for (i = 0; i < n; i++)
      dsk_warning ("param %u: %.*s -> %.*s",
                   i,
                   (int)(kv[i].key_end-kv[i].key_start),
                   kv[i].key_start,
                   (int)(kv[i].value_end-kv[i].value_start),
                   kv[i].value_start);
  }
#endif
  *n_kv_out = n;
  return DSK_TRUE;
}

dsk_boolean dsk_parse_mime_content_disposition_header (const char *line,
                                                       DskMimeContentDisposition *out,
                                                       DskError **error)
{
  DskMimeKeyValueInplace kvs[32];
  unsigned n, i;
  SKIP_WS (line);
  memset (out, 0, sizeof (DskMimeContentDisposition));
  if (dsk_ascii_strncasecmp (line, "inline", 6) == 0)
    {
      out->is_inline = DSK_TRUE;
      line += 6;
    }
  else if (dsk_ascii_strncasecmp (line, "attachment", 10) == 0)
    {
      out->is_inline = DSK_FALSE;
      line += 10;
    }
  else
    {
      out->is_inline = DSK_FALSE;   /* form-data is another dispos */
      while (dsk_ascii_istoken (*line))
        line++;
    }
  SKIP_WS (line);
  if (*line == ';')
    {
      line++;
      SKIP_WS (line);
    }
  if (!dsk_mime_key_values_scan (line, DSK_N_ELEMENTS (kvs), kvs, &n, error))
    return DSK_FALSE;
#define KEY_IS(kv, str) ((kv).key_end - (kv).key_start == strlen (str) \
                     && dsk_ascii_strncasecmp (str, (kv).key_start, \
                                               (kv).key_end - (kv).key_start) == 0)
  for (i = 0; i < n; i++)
    if (KEY_IS (kvs[i], "name"))
      {
        out->name_start = kvs[i].value_start;
        out->name_end = kvs[i].value_end;
      }
    else if (KEY_IS (kvs[i], "filename"))
      {
        out->filename_start = kvs[i].value_start;
        out->filename_end = kvs[i].value_end;
      }
  return DSK_TRUE;
}
