#include <stdlib.h>
#include <string.h>
#include "dsk.h"

typedef enum
{
  DSK_URL_INTERPRETATION_UNKNOWN,
  DSK_URL_INTERPRETATION_RELATIVE,
  DSK_URL_INTERPRETATION_ABSOLUTE,
  DSK_URL_INTERPRETATION_REMOTE
} DskUrlInterpretation;

/* general sanity check */
static dsk_boolean
is_valid_hostname (const char *hostname_start,
                   const char *hostname_end)
{
  const char *hostname;
  if (hostname_start == hostname_end)
    return DSK_FALSE;
  if (!dsk_ascii_isalnum (hostname_start[0]))
    return DSK_FALSE;
  for (hostname = hostname_start + 1; hostname < hostname_end; hostname++)
    if (!dsk_ascii_istoken (*hostname) && *hostname != '.')
      return DSK_FALSE;
  return DSK_TRUE;
}

static dsk_boolean
is_valid_generic_component (const char *start, const char *end)
{
  const char *at;
  for (at = start; at < end; at++)
    if (!(33 <= *at && *at <= 126))
      return DSK_FALSE;
  return DSK_TRUE;
}

dsk_boolean
dsk_url_scheme_parse (const char   *url,
                      unsigned     *scheme_length_out,
                      DskUrlScheme *scheme_out,
                      DskError    **error)
{
  const char *p = url;
  while (dsk_ascii_isalpha (*p))
    p++;
  if (*p != ':')
    {
      dsk_set_error (error, "missing ':' in URL");
      return DSK_FALSE;
    }
  *scheme_length_out = p + 1 - url;
   if ((url[0] == 'h' || url[0] == 'H')
    && (url[1] == 't' || url[1] == 'T')
    && (url[2] == 't' || url[2] == 'T')
    && (url[3] == 'p' || url[3] == 'P'))
    {
      if (p - url == 4)
        {
          *scheme_out = DSK_URL_SCHEME_HTTP;
          return DSK_TRUE;
        }
      else if (p - url == 5 && (p[4] == 's' || p[4] == 'S'))
        {
          *scheme_out = DSK_URL_SCHEME_HTTPS;
          return DSK_TRUE;
        }
    }
  else if ((url[0] == 'f' || url[0] == 'F')
        && (url[1] == 't' || url[1] == 'T')
        && (url[2] == 'p' || url[2] == 'P')
        && p - url == 3)
    {
      *scheme_out = DSK_URL_SCHEME_FTP;
      return DSK_TRUE;
    }
  dsk_set_error (error, "unknown scheme %.*s", (int)(p - url), url);
  return DSK_FALSE;
}



/* TODO: rewrite this thing! */
dsk_boolean  dsk_url_scan  (const char     *url_string,
                            DskUrlScanned  *out,
                            DskError      **error)
{
  int num_slashes;
  const char *at = url_string;
  DskUrlInterpretation interpretation = DSK_URL_INTERPRETATION_UNKNOWN;
  const char *query_start;
  const char *frag_start;
  const char *end_string;
  out->scheme_start = at;
  /* XXX: use dsk_url_scheme_parse() */
  while (dsk_ascii_isalnum (*at))
    at++;
  if (out->scheme_start == at)
    {
      dsk_set_error (error, "no scheme found in URL");
      return DSK_FALSE;
    }
  if (*at != ':')
    {
      dsk_set_error (error, "missing : after scheme in URL");
      return DSK_FALSE;
    }
  out->scheme_end = at;
  at++;         /* skip : */

  /* Parse scheme */
  out->scheme = DSK_URL_SCHEME_UNKNOWN;
  switch (out->scheme_end - out->scheme_start)
    {
    case 3:
      if (dsk_ascii_strncasecmp (out->scheme_start, "ftp", 3) == 0)
        out->scheme = DSK_URL_SCHEME_FTP;
      break;
    case 4:
      if (dsk_ascii_strncasecmp (out->scheme_start, "http", 4) == 0)
        out->scheme = DSK_URL_SCHEME_HTTP;
      else if (dsk_ascii_strncasecmp (out->scheme_start, "file", 4) == 0)
        out->scheme = DSK_URL_SCHEME_FILE;
      break;
    case 5:
      if (dsk_ascii_strncasecmp (out->scheme_start, "https", 5) == 0)
        out->scheme = DSK_URL_SCHEME_HTTPS;
      break;
    }

  num_slashes = 0;
  while (*at == '/')
    {
      num_slashes++;
      at++;
    }
  if (out->scheme == DSK_URL_SCHEME_FILE)
    interpretation = DSK_URL_INTERPRETATION_ABSOLUTE;
  else
    switch (num_slashes)
      {
	case 0:
	  interpretation = DSK_URL_INTERPRETATION_RELATIVE;
	  break;
	case 1:
	  interpretation = DSK_URL_INTERPRETATION_ABSOLUTE;
	  break;
	case 2:
	  /* ``schemes including a top hierarchical element for a naming
	   *   authority'' (Section 3.2)
	   */
	  interpretation = DSK_URL_INTERPRETATION_REMOTE;
	  break;
	case 3:
	  /* File urls (well those are now handled above so this
	   * is pretty dubious)
	   */
	  interpretation = DSK_URL_INTERPRETATION_ABSOLUTE;
	  break;
	default:
          /* hmm */
	  interpretation = DSK_URL_INTERPRETATION_ABSOLUTE;
	  break;
      }

          out->host_start = out->host_end = NULL;
          out->username_start = out->username_end = NULL;
          out->password_start = out->password_end = NULL;
          out->port_start = out->port_end = NULL;
          out->port = 0;


  if (interpretation == DSK_URL_INTERPRETATION_REMOTE)
    {
      /* rfc 2396, section 3.2.2. */
      const char *end_hostport;
      const char *at_sign;
      const char *colon;
      /* basically the syntax is:
       *    USER@HOST:PORT/
       *        ^    |    ^
       *     at_sign ^  end_hostport
       *            colon
       */             
      end_hostport = strchr (at, '/');
      if (end_hostport == NULL)
        end_hostport = strchr (at, 0);
      at_sign = memchr (at, '@', end_hostport - at);
      out->host_start = at_sign != NULL ? (at_sign + 1) : at;
      colon = memchr (out->host_start, ':', end_hostport - out->host_start);
      if (at_sign != NULL)
        {
          const char *password_sep = memchr (at, ':', at_sign - at);
          if (password_sep)
            {
              out->username_start = at;
              out->username_end = password_sep;
              out->password_start = password_sep + 1;
              out->password_end = at_sign;
            }
          else
            {
              out->username_start = at;
              out->username_end = at_sign;
              out->password_start = NULL;
              out->password_end = NULL;
            }
          /* XXX: should validate username against 
           *         GSK_URL_USERNAME_CHARSET
           */
        }
      else
        {
          out->username_start = NULL;
          out->username_end = NULL;
          out->password_start = NULL;
          out->password_end = NULL;
        }
      out->host_end = colon != NULL ? colon : end_hostport;

      if (colon != NULL)
        {
          out->port_start = colon + 1;
          out->port_end = end_hostport;
          out->port = atoi (out->port_start);
        }

      at = end_hostport;
    }
  else if (interpretation == DSK_URL_INTERPRETATION_UNKNOWN)
    {
      dsk_set_error (error, "cannot guess how to interpret %.*s URL",
                     (int)(out->scheme_end - out->scheme_start), out->scheme_start);
      return DSK_FALSE;
    }


  if (num_slashes > 0
   && interpretation == DSK_URL_INTERPRETATION_ABSOLUTE)
    at--;
  query_start = strchr (at, '?');
  frag_start = strchr (query_start != NULL ? query_start : at, '#');
  end_string = strchr (at, 0);
  out->path_start = at;
  if (query_start != NULL)
    out->path_end = query_start;
  else if (frag_start != NULL)
    out->path_end = frag_start;
  else
    out->path_end = end_string;
  if (query_start != NULL)
    {
      out->query_start = query_start + 1;
      out->query_end = frag_start ? frag_start : end_string;
    }
  else
    out->query_start = out->query_end = NULL;
  if (frag_start != NULL)
    {
      out->fragment_start = frag_start + 1;
      out->fragment_end = end_string;
    }
  else
    out->fragment_start = out->fragment_end = NULL;

#define CHECK(base, function)                                 \
  if (out->base##_start != NULL                               \
   && !function (out->base##_start, out->base##_end))         \
    {                                                         \
      dsk_set_error (error, "invalid character in %s", #base);\
      return DSK_FALSE;                                       \
    }
  CHECK (host, is_valid_hostname)
  CHECK (path, is_valid_generic_component)
  CHECK (query, is_valid_generic_component)
#undef CHECK
  return DSK_TRUE;
}

static char *strcut (const char *start, const char *end)
{
  if (start == NULL)
    return NULL;
  char *rv = dsk_malloc (end + 1 - start);
  memcpy(rv, start, end-start);
  rv[end - start] = 0;
  return rv;
}

// TODO: optimize to use a single allocation for all the strings
DskUrl *dsk_url_new           (const char *str, 
                               DskError  **error)
{
  DskUrlScanned scanned;
  if (!dsk_url_scan (str, &scanned, error))
    return NULL;
  DskUrl *url = dsk_object_new (&dsk_url_class);
  url->scheme_str = strcut (scanned.scheme_start, scanned.scheme_end);
  url->scheme = scanned.scheme;
  url->username = strcut (scanned.username_start, scanned.username_end);
  url->password = strcut (scanned.password_start, scanned.password_end);
  url->host = strcut (scanned.host_start, scanned.host_end);
  url->port = scanned.port;
  url->path = strcut (scanned.path_start, scanned.path_end);
  url->query = strcut (scanned.query_start, scanned.query_end);
  url->fragment = strcut (scanned.fragment_start, scanned.fragment_end);
  return url;
};

#define dsk_url_init NULL
static void
dsk_url_finalize (DskUrl *url)
{
  dsk_free (url->scheme_str);
  dsk_free (url->username);
  dsk_free (url->password);
  dsk_free (url->host);
  dsk_free (url->path);
  dsk_free (url->query);
  dsk_free (url->fragment);
}

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskUrl);
DskUrlClass dsk_url_class = { DSK_OBJECT_CLASS_DEFINE (DskUrl, &dsk_object_class, NULL, dsk_url_finalize) };

DskUrl *dsk_url_new_from_base (DskUrl *base,
                               const char *path,
                               DskError  **error);
DskUrl *dsk_url_new_from_parts(const char *scheme,
                               const char *username,
                               const char *password,
                               const char *host,
                               int         port,
                               const char *path,
                               const char *query,
                               const char *fragment);

