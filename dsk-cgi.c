#include <string.h>
#include "dsk.h"

/* query_string starts (and includes) the '?'  */
dsk_boolean dsk_cgi_parse_query_string (const char *query_string,
                                        size_t     *n_cgi_variables_out,
                                        DskCgiVariable **cgi_variables_out,
                                        DskError  **error)
{
  const char *at;
  unsigned n_ampersand = 0;
  unsigned idx;
  unsigned n_cgi = 0;
  if (*query_string == '?')
    query_string++;
  for (at = query_string; *at; at++)
    if (*at == '&')
      n_ampersand++;
  /* TODO: need max_cgi_vars for security????? */
  *cgi_variables_out = dsk_malloc (sizeof (DskCgiVariable) * (n_ampersand+1));
  idx = 0;
  for (at = query_string; *at; )
    {
      const char *start;
      const char *eq = NULL;
      unsigned key_len, value_len = 0;
      char *kv;
      if (*at == '&')
        {
          at++;
          continue;
        }
      start = at;
      while (*at && *at != '&' && *at != '=')
        at++;
      if (*at == '=')
        {
          eq = at;

          /* handle value */
          value_len = 0;
          at++;
          while (*at && *at != '&')
            {
              if (*at == '%')
                {
                  if (!dsk_ascii_isxdigit (at[1])
                   || !dsk_ascii_isxdigit (at[2]))
                    {
                      unsigned i;
                      for (i = 0; i < n_cgi; i++)
                        dsk_free ((*cgi_variables_out)[i].key);
                      dsk_free (*cgi_variables_out);
                      dsk_set_error (error, "'%%' in CGI-variable not followed by two hexidecimal digits");
                      return DSK_FALSE;

                    }
                  value_len++;
                  at += 3;
                }
              else
                {
                  value_len++;
                  at++;
                }
            }
        }
      at++;
      key_len = (eq - start);

      kv = dsk_malloc (key_len + 1 + value_len + 1);
      memcpy (kv, start, key_len);
      kv[key_len] = 0;
      (*cgi_variables_out)[n_cgi].key = kv;

      /* unescape value */
      if (eq)
        {
          char *out = kv + key_len + 1;
          char *value_start = out;
          (*cgi_variables_out)[n_cgi].value = out;
          for (at = eq + 1; *at != '&' && *at != '\0'; at++)
            if (*at == '%')
              {
                *out++ = (dsk_ascii_xdigit_value(at[1]) << 4)
                       | dsk_ascii_xdigit_value(at[2]);
                at += 2;
              }
            else if (*at == '+')
              *out++ = ' ';
            else
              *out++ = *at;
          *out = '\0';
          (*cgi_variables_out)[n_cgi].value_length = out - value_start;
        }
      else
        {
          (*cgi_variables_out)[n_cgi].value = NULL;
          (*cgi_variables_out)[n_cgi].value_length = 0;
        }
      (*cgi_variables_out)[n_cgi].content_type = NULL;
      (*cgi_variables_out)[n_cgi].is_get = DSK_TRUE;

      (*cgi_variables_out)[n_cgi].content_type = NULL;
      (*cgi_variables_out)[n_cgi].content_location = NULL;
      (*cgi_variables_out)[n_cgi].content_description = NULL;
      n_cgi++;
    }
  *n_cgi_variables_out = n_cgi;
  return DSK_TRUE;
}

dsk_boolean dsk_cgi_parse_post_data (const char *content_type,
                                     char      **content_type_kv_pairs,
                                     size_t      post_data_length,
                                     const uint8_t *post_data,
                                     size_t     *n_cgi_var_out,
                                     DskCgiVariable **cgi_var_out,
                                     DskError  **error)
{
  if (strcmp (content_type, "application/x-www-form-urlencoded") == 0
   || strcmp (content_type, "application/x-url-encoded") == 0)
    {
      char *pd_str = dsk_malloc (post_data_length + 2);
      unsigned i;
      memcpy (pd_str + 1, post_data, post_data_length);
      pd_str[0] = '?';
      pd_str[post_data_length+1] = '\0';
      if (!dsk_cgi_parse_query_string (pd_str, n_cgi_var_out, cgi_var_out,
                                       error))
        {
          dsk_free (pd_str);
          return DSK_FALSE;
        }
      for (i = 0; i < *n_cgi_var_out; i++)
        (*cgi_var_out)[i].is_get = DSK_FALSE;
      dsk_free (pd_str);
      return DSK_TRUE;
    }
  else if (strcmp (content_type, "multipart/form-data") == 0)
    {
      DskMimeMultipartDecoder *decoder = dsk_mime_multipart_decoder_new (content_type_kv_pairs, error);
      if (decoder == NULL)
        return DSK_FALSE;
      if (!dsk_mime_multipart_decoder_feed (decoder,
                                            post_data_length, post_data,
                                            NULL, error)
       || !dsk_mime_multipart_decoder_done (decoder, n_cgi_var_out, error))
        {
          dsk_object_unref (decoder);
          return DSK_FALSE;
        }
      *cgi_var_out = dsk_malloc (*n_cgi_var_out * sizeof (DskCgiVariable));
      dsk_mime_multipart_decoder_dequeue_all (decoder, *cgi_var_out);
      dsk_object_unref (decoder);
      return DSK_TRUE;
    }
  else
    {
      dsk_set_error (error, 
                     "parsing POST form data: unhandled content-type %s",
                     content_type);
      return DSK_FALSE;
    }
}

void        dsk_cgi_variable_clear       (DskCgiVariable *variable)
{
  if (variable->key == NULL)
    {
      dsk_assert (!variable->is_get);
      dsk_free (variable->value);
    }
  else
    dsk_free (variable->key);
}
