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

static void append_spaces(DskPrint *print, unsigned count)
{
  static const char spaces64[64] = {
    32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
    32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
    32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
    32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
  };
  while (count > sizeof (spaces64))
    {
      dsk_print_append_data (print,
                             sizeof (spaces64), (uint8_t*)spaces64,
                             NULL);
      count -= sizeof (spaces64);
    }
  if (count > 0)
    dsk_print_append_data (print,
                           count, (uint8_t*)spaces64,
                           NULL);
}

static void
append_char (DskPrint *print, char c)
{
  uint8_t buf[1] = {(uint8_t)c};
  dsk_print_append_data (print, 1, buf, NULL);
}

static void
append_string (DskPrint *print, const char *str)
{
  dsk_print_append_data (print, strlen (str), (const uint8_t *) str, NULL);
}

void dsk_codegen_function_render (DskCodegenFunction *function,
                                  DskPrint           *print)
{
  // compute column widths
  unsigned function_name_pos;
  unsigned function_name_width;
  unsigned function_argtypes_width;
  unsigned function_argnames_width;
  if (function->function_name_pos < 0)
    {
      function_name_pos = strlen (function->return_type);
      if (function->storage != NULL)
        function_name_pos += 1 + strlen (function->storage);
    }
  else
    {
      function_name_pos = function->function_name_pos;
    }
  if (function->function_name_width < 0)
    {
      function_name_width = strlen (function->name);
    }
  else
    {
      function_name_width = function->function_name_width;
    }
  if (function->function_argtypes_width < 0)
    {
      unsigned i;
      function_argtypes_width = 0;
      for (i = 0; i < function->n_args; i++)
        {
          unsigned len = strlen (function->args[i].type);
          if (len > function_argtypes_width)
            function_argtypes_width = len;
        }
    }
  else
    {
      function_argtypes_width = function->function_argtypes_width;
    }
  if (function->function_argnames_width < 0)
    {
      unsigned i;
      function_argnames_width = 0;
      for (i = 0; i < function->n_args; i++)
        {
          unsigned len = strlen (function->args[i].name);
          if (len > function_argnames_width)
            function_argnames_width = len;
        }
    }
  else
    {
      function_argnames_width = function->function_argnames_width;
    }

  // place strings
  if (function->storage == NULL)
    {
      unsigned len = strlen (function->return_type);
      append_string (print, function->return_type);
      if (len + 1 <= function_name_pos)
        {
          append_spaces (print, function_name_pos - len);
        }
      else
        {
          append_char (print, '\n');
          append_spaces (print, function_name_pos);
        }
    }
  else
    {
      unsigned slen = strlen (function->storage);
      unsigned rtlen = strlen (function->return_type);
      if (slen + 1 + rtlen + 1 <= function_name_pos)
        {
          append_string (print, function->storage);
          append_char (print, ' ');
          append_string (print, function->return_type);
          append_spaces (print, function_name_pos - slen - 1 - rtlen);;
        }
      else if (rtlen + 1 <= function_name_pos)
        {
          append_string (print, function->storage);
          append_char (print, '\n');
          append_string (print, function->return_type);
          append_spaces (print, function_name_pos - rtlen);;
        }
      else
        {
          /* place storage + return_type on line above */
          append_string (print, function->storage);
          append_char (print, ' ');
          append_string (print, function->return_type);
          append_char (print, '\n');
          append_spaces (print, function_name_pos);
        }
    }
  append_string (print, function->name);
  if (strlen (function->name) > function_name_width)
    {
      append_char (print, '\n');
      append_spaces (print, function_name_pos + function_name_width);
    }
  append_char (print, '(');
  if (function->n_args == 0)
    {
      if (function->render_void_args)
        append_string (print, "void");
    }
  else
    {
      for (unsigned i = 0; i < function->n_args; i++)
        { 
          if (i > 0)
            {
              append_string (print, ",\n");
              append_spaces (print, function_name_pos + function_name_width);
            }
          unsigned tlen = strlen (function->args[i].type);
          char last_char = function->args[i].type[tlen - 1];
          unsigned min_spaces = dsk_ascii_istoken (last_char) ? 1 : 0;
          unsigned align_spaces = tlen < function_argtypes_width ? (function_argtypes_width - tlen) : 0;
          unsigned spaces = DSK_MAX (min_spaces, align_spaces);
          append_string (print, function->args[i].type);
          append_spaces (print, spaces);
          append_string (print, function->args[i].name);
        }
    }
  append_char (print, ')');
  if (function->semicolon)
    append_char (print, ';');
  if (function->newline)
    append_char (print, '\n');
}

void dsk_codegen_function_render_buffer
                                 (DskCodegenFunction *function,
                                  DskBuffer *buffer)
{
  DskPrint *pr = dsk_print_new_buffer (buffer);
  dsk_codegen_function_render (function, pr);
  dsk_print_free (pr);
}
