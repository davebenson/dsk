#include "dsk.h"
#include <stdio.h>
#include <string.h>

// TODO: UTF-16 decoding


/* amount to add to indent */
#define INDENT 2

static void
pr_spaces (int count, 
           DskJsonAppendFunc     append_func,
           void          *append_data)
{
  static const char space64[] =
    "                                                                ";
  while (count >= 64)
    {
      append_func (64, space64, append_data);
      count -= 64;
    }
  if (count > 0)
    append_func (count, space64, append_data);
}

static void
pr_quoted_string (unsigned              len,
                  const char           *str,
                  DskJsonAppendFunc     append_func,
                  void                 *append_data)
{
  /* if we get invalid UTF-8, alert the user,
     but only once per string... */
  dsk_boolean warned = DSK_FALSE;

  char tmp_buf[64];

  append_func (1, "\"", append_data);
  while (len > 0)
    {
      unsigned n_unquoted = 0;
      while (n_unquoted < len && 
             dsk_ascii_is_no_cquoting_required (str[n_unquoted]))
        n_unquoted++;
      if (n_unquoted > 0)
        {
          append_func (n_unquoted, str, append_data);
          str += n_unquoted;
          len -= n_unquoted;
          if (len == 0)
            break;
        }
      if (*str == '"')
        {
          append_func (2, "\\\"", append_data);
          str++;
          len--;
        }
      else if (*str == '\\')
        {
          append_func (2, "\\\\", append_data);
          str++;
          len--;
        }
      else if (*str == '\n')
        {
          append_func (2, "\\\n", append_data);
          str++;
          len--;
        }
      else if (*str == '\t')
        {
          append_func (2, "\\\t", append_data);
          str++;
          len--;
        }
#if 0
      else if ((uint8_t) (*str) < 128U)
        {
          /* Normally we can use a shortened octal code instead
             of 3 digits, e.g. \0 instead of \000.  But
             if the next character is a digit, that's ambiguous,
             and we fallback to the unabbreviated code. */
          if (len > 1 && dsk_ascii_isdigit (str[1]))
            snprintf (tmp_buf, sizeof (tmp_buf), "\\%03o", str[0]);
          else
            snprintf (tmp_buf, sizeof (tmp_buf), "\\%o", str[0]);
          append_func (strlen (tmp_buf), tmp_buf, append_data);
          str++;
          len--;
        }
#endif
      else
        {
          /* unicode */
          unsigned used;
          uint32_t code;
          if (!dsk_utf8_decode_unichar (len, str, &used, &code))
            {
              if (!warned)
                {
                  dsk_warning ("invalid UTF-8");
                  warned = DSK_TRUE;
                }
              str++;
              len--;
            }
          else
            {
              snprintf (tmp_buf, sizeof (tmp_buf), "\\u%04x", code);
              append_func (6, tmp_buf, append_data);
              str += used;
              len -= used;
            }
        }
    }
  append_func (1, "\"", append_data);
}

void
dsk_json_value_serialize  (const DskJsonValue *value,
                           int               indent,
                           DskJsonAppendFunc append_func,
                           void             *append_data)
{
  switch (value->type)
    {
    case DSK_JSON_VALUE_BOOLEAN:
      if (value->v_boolean.value)
        append_func (4, "true", append_data);
      else
        append_func (5, "false", append_data);
      break;
    case DSK_JSON_VALUE_NULL:
      append_func (4, "null", append_data);
      break;
    case DSK_JSON_VALUE_OBJECT:
      if (indent >= 0)
        {
          unsigned n_members = value->v_object.n_members;
          unsigned i;
          const DskJsonMember *members = value->v_object.members;
          append_func (2, "{\n", append_data);
          for (i = 0; i < n_members; i++)
            {
              pr_spaces (indent + INDENT, append_func, append_data);
              pr_quoted_string (strlen (members[i].name),
                                members[i].name,
                                append_func, append_data);
              append_func (2, ": ", append_data);
              dsk_json_value_serialize (members[i].value, indent + INDENT,
                                        append_func, append_data);
              if (i + 1 < n_members)
                append_func (2, ",\n", append_data);
              else
                append_func (1, "\n", append_data);
            }
          pr_spaces (indent, append_func, append_data);
          append_func (1, "}", append_data);
        }
      else
        {
          unsigned n_members = value->v_object.n_members;
          DskJsonMember *members = value->v_object.members;
          unsigned i;
          append_func (1, "{", append_data);
          for (i = 0; i < n_members; i++)
            {
              pr_quoted_string (strlen (members[i].name),
                                members[i].name,
                                append_func, append_data);
              append_func (1, ":", append_data);
              dsk_json_value_serialize (members[i].value, indent,
                                        append_func, append_data);
              if (i + 1 < n_members)
                append_func (1, ",", append_data);
            }
          append_func (1, "}", append_data);
        }
      break;

    case DSK_JSON_VALUE_ARRAY:
      if (indent >= 0)
        {
          unsigned n_values = value->v_array.n_values;
          DskJsonValue **values = value->v_array.values;
          unsigned i;
          append_func (2, "[\n", append_data);
          for (i = 0; i < n_values; i++)
            {
              pr_spaces (indent + INDENT, append_func, append_data);
              dsk_json_value_serialize (values[i], indent + INDENT,
                                        append_func, append_data);
              if (i + 1 < n_values)
                append_func (2, ",\n", append_data);
              else
                append_func (1, "\n", append_data);
            }
          pr_spaces (indent, append_func, append_data);
          append_func (1, "]", append_data);
        }
      else
        {
          unsigned n_values = value->v_array.n_values;
          DskJsonValue **values = value->v_array.values;
          unsigned i;
          append_func (1, "[", append_data);
          for (i = 0; i < n_values; i++)
            {
              dsk_json_value_serialize (values[i], indent,
                                        append_func, append_data);
              if (i + 1 < n_values)
                append_func (1, ",", append_data);
            }
          append_func (1, "]", append_data);
        }
      break;
    case DSK_JSON_VALUE_STRING:
      pr_quoted_string (value->v_string.length,
                        value->v_string.str,
                        append_func, append_data);
      break;
    case DSK_JSON_VALUE_NUMBER:
      {
        char buf[256];
        snprintf (buf, sizeof (buf), "%.17g", value->v_number.value);
        buf[sizeof(buf)-1] = 0;
        append_func (strlen (buf), buf, append_data);
        break;
      }
    default:
      dsk_assert_not_reached ();
    }
}

static void buffer_append_func (unsigned      length,
                                const void   *data,
                                void         *append_data)
{
  dsk_buffer_append (append_data, length, data);
}

void    dsk_json_value_to_buffer  (const DskJsonValue  *value,
                                   int                  indent,
                                   DskBuffer           *out)
{
  dsk_json_value_serialize (value, indent, buffer_append_func, out);
}
