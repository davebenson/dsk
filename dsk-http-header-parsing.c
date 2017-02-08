#include <string.h>
#include <stdlib.h>
#include <alloca.h>
#include "dsk.h"

#define DEFAULT_INIT_N_MISC_HEADERS             8       /* must be power-of-two */

typedef struct _ParseInfo ParseInfo;
struct _ParseInfo
{
  char *slab;
  unsigned n_kv_pairs;
  char **kv_pairs;

  void *free_list;
  unsigned scratch_remaining;
  char *scratch;
};

static void
parse_info_clear (ParseInfo *pi)
{
  void *at = pi->free_list;
  while (at != NULL)
    {
      void *next = * (void **) at;
      dsk_free (at);
      at = next;
    }
}

static void *
parse_info_alloc (ParseInfo *pi, unsigned size)
{
  /* round 'size' up to a multiple of the size of a pointer */
  if (size & (sizeof(void*)-1))
    size += sizeof(void*) - (size & (sizeof(void*)-1));
  if (size > pi->scratch_remaining)
    {
      void *rv = dsk_malloc (sizeof (void*) + size);
      *(void**)rv = pi->free_list;
      pi->free_list = rv;
      return ((void**)rv) + 1;
    }
  else
    {
      void *rv = pi->scratch;
      pi->scratch += size;
      return rv;
    }
}

static void
normalize_whitespace (char *inout)
{
  char *in = inout;
  char *out = inout;
  char *first;
  char *end;
  while (dsk_ascii_isspace (*in))
    in++;
  first = end = in;
  while (*in)
    {
      if (!dsk_ascii_isspace (*in))
        end = in + 1;
      in++;
    }
  if (in == out)
    *end = 0;
  else
    {
      memmove (out, first, end - first);
      out[end - first] = 0;
    }
}

static dsk_boolean
parse_info_init (ParseInfo *pi,
                 DskBuffer *buffer,
                 unsigned   header_len,
                 unsigned   scratch_len,
                 char      *scratch_pad,
                 DskError **error)
{
  unsigned n_newlines;
  unsigned i;
  char *at;
  unsigned n_kv;
  if (header_len < 4)
    {
      dsk_set_error (error, "HTTP header too short");
      return DSK_FALSE;
    }
  pi->scratch_remaining = scratch_len;
  pi->scratch = scratch_pad;
  pi->free_list = NULL;
  pi->slab = parse_info_alloc (pi, header_len+1);
  dsk_buffer_peek (buffer, header_len, pi->slab);

  /* get rid of blank terminal line, if supplied */
  if (pi->slab[header_len-1] == '\n'
   && pi->slab[header_len-2] == '\n')
    header_len--;
  else if (pi->slab[header_len-1] == '\n'
      &&   pi->slab[header_len-2] == '\r'
      &&   pi->slab[header_len-3] == '\n')
    header_len -= 2;

  /* add NUL */
  pi->slab[header_len] = 0;

  /* count newlines */
  n_newlines = 0;
  at = pi->slab;
  for (i = 0; i < header_len; i++)
    if (at[i] == '\n')
      n_newlines++;

  /* align scratch pad */
  pi->kv_pairs = parse_info_alloc (pi, sizeof (char *) * (n_newlines - 1) * 2);

  /* gather key/value pairs, lowercasing keys */
  at = pi->slab;
  at = strchr (at, '\n');    /* skip initial line (not key-value format) */
  if (at == NULL)
    {
      dsk_set_error (error, "NUL in header");
      goto FAIL;
    }
  if (at > pi->slab && *(at-1) == '\r')
    *(at-1) = 0;
  else
    *at = 0;
  at++;

  /* is that the end of the header? */
  if (*at == 0)
    {
      pi->n_kv_pairs = 0;
      return DSK_TRUE;
    }

  /* first key-value line must not be whitespace */
  if (*at == ' ' || *at == '\t')
    {
      dsk_set_error (error, "unexpected whitespace after first HTTP header line");
      goto FAIL;
    }

  n_kv = 0;
  while (*at != 0)
    {
      static uint8_t ident_chartable[64] = {
#include "dsk-http-ident-chartable.inc"
      };
      char *end;
      /* note key location */
      pi->kv_pairs[n_kv*2] = at;

      /* lowercase until ':' (test character validity) */
      for (;;)
        {
          uint8_t a = *at;
          switch ((ident_chartable[a/4] >> (a%4*2)) & 3)
            {
            case 0: /* invalid */
              dsk_set_error (error, "invalid character %s in HTTP header [offset %d]",
                             dsk_ascii_byte_name (a),
                             (int)(at - pi->slab));
              goto FAIL;
            case 1: /* passthough */
            case 2: /* lowercase -- NO LONGER LOWERCASING */
              at++;
              continue;
              //*at += 'a' - 'A';
              //at++;
              //continue;
            case 3: /* colon */
              goto at_colon;
            }
        }

    at_colon:
      *at++ = 0;
      while (*at == ' ' || *at == '\t')
        at++;
      pi->kv_pairs[n_kv*2+1] = at;
      n_kv++;

      end = NULL;
    scan_value_end_line:
      /* skip to cr-nl */
      while (*at != '\r' && *at != '\n')
        at++;
      end = at;
      if (*at == '\r')
        {
          at++;
          if (*at != '\n')
            {
              dsk_set_error (error, "got CR without LF in HTTP header");
              return DSK_FALSE;
            }
        }
      at++;             /* skip \n */

      /* if a line begins with whitespace,
         then it is actually a continuation of this header.
         Never seen in the wild, this appears in the HTTP spec a lot. */
      if (*at == ' ' || *at == '\t')
        goto scan_value_end_line;

      /* mark end of value */
      *end = 0;
      normalize_whitespace (pi->kv_pairs[n_kv*2-1]);
    }
  pi->n_kv_pairs = n_kv;
  return DSK_TRUE;

FAIL:
  parse_info_clear (pi);
  return DSK_FALSE;
}

static inline char to_lowercase (char a)
{
  return ('A' <= a && a <= 'Z') ? (a + 'a' - 'A') : a;
}

static dsk_boolean
ascii_caseless_equals (const char *a,
                       const char *b)
{
  for (;;)
    {
      char A = to_lowercase (*a++);
      char B = to_lowercase (*b++);
      if (A != B)
        return DSK_FALSE;
      if (A == 0)
        return DSK_TRUE;
    }
}

/* TODO: implement a real parser */
static dsk_boolean
handle_content_encoding (const char  *value,
                         dsk_boolean *is_gzip_out,
                         DskError   **error)
{
  if (ascii_caseless_equals (value, "gzip"))
    *is_gzip_out = DSK_TRUE;
  else if (ascii_caseless_equals (value, "identity")
      ||   ascii_caseless_equals (value, "none"))
    *is_gzip_out = DSK_FALSE;
  else
    {
      dsk_set_error (error, "unhandled content-encoding '%s'", value);
      return DSK_FALSE;
    }
  return DSK_TRUE;
}

static dsk_boolean
handle_content_length (const char *value,
                       int64_t    *len_out,
                       DskError  **error)
{
  char *end;
  *len_out = strtoull (value, &end, 10);
  if (value == end)
    {
      dsk_set_error (error, "parsing Content-Length header");
      return DSK_FALSE;
    }
  while (dsk_ascii_isspace (*end))
    end++;
  if (*end == 0)
    return DSK_TRUE;
  dsk_set_error (error, "garbage after Content-Length header");
  return DSK_FALSE;
}

static char *
parse_content_type (char *value,
                    DskError **error)
{
  /* RFC 2616 3.7 (referenced from 14.17) */
  const char *charset = NULL;
  char *rv = value;
  char *end_subtype;
  if (*value == 0 || *value == '/')
    {
      dsk_set_error (error, "content-type begins with a '/'");
      return NULL;
    }
  while (*value && *value != '/')
    {
      if (!dsk_ascii_istoken (*value))
        {
          dsk_set_error (error, "bad character %s in content-type",
                         dsk_ascii_byte_name (*value));
          return NULL;
        }
      if (dsk_ascii_isupper (*value))
        *value += ('a' - 'A');
      value++;
    }
  value++;
  while (dsk_ascii_istoken (*value))
    value++;
  end_subtype = value;
  for (;;)
    {
      char *pname, *pvalue;
      while (dsk_ascii_isspace (*value))
        value++;
      if (*value == 0)
        break;
      if (*value != ';')
        {
          dsk_set_error (error, "expected ';' or end-of-string, got %s in content-type",
                         dsk_ascii_byte_name (*value));
          return DSK_FALSE;
        }
      value++;
      while (dsk_ascii_isspace (*value))
        value++;
      pname = value;
      while (dsk_ascii_istoken (*value))
        value++;
      if (*value != '=')
        {
          dsk_set_error (error, "expected '=', got %s in content-type",
                         dsk_ascii_byte_name (*value));
          return DSK_FALSE;
        }
      *value++ = 0;
      if (*value == '"')
        {
          value++;
          pvalue = value;
          while (*value && *value != '"')
            value++;
          if (*value == 0)
            {
              dsk_set_error (error, "unterminated double-quoted string in Content-Type");
              return DSK_FALSE;
            }
          else
            *value++ = 0;
        }
      else
        {
          pvalue = value;
          while (dsk_ascii_istoken (*value))
            value++;
          if (*value != 0)
            {
              if (*value != ';' && !dsk_ascii_isspace (*value))
                {
                  dsk_set_error (error, "bad character %s in unquoted parameter value for %s in Content-Type",
                                 dsk_ascii_byte_name (*value), pname);
                  return DSK_FALSE;
                }
              *value++ = 0;
            }
        }
      if (ascii_caseless_equals (pname, "charset"))
        {
          charset = pvalue;
        }
      else
        {
          /* Ignore any parameter beside charset */
        }
    }
  if (charset)
    {
      *end_subtype++ = '/';
      memmove (end_subtype, charset, strlen (charset) + 1);
    }
  else
    *end_subtype = 0;
  return rv;
}

static dsk_boolean
handle_date (const char *header_name,
             const char *value,
             dsk_time_t *date_out,
             DskError  **error)
{
  DskDate date;
  if (!dsk_date_parse (value, NULL, &date, error))
    {
      dsk_warning ("date=%s",value);
      dsk_add_error_prefix (error, "in %s header", header_name);
      return DSK_FALSE;
    }
  *date_out = dsk_date_to_unixtime (&date);
  return DSK_TRUE;
}

static char *
strip_string (char *v)
{
  dsk_boolean last_was_space = DSK_FALSE;
  char *out, *rv;
  while (dsk_ascii_isspace (*v))
    v++;
  rv = v;
  out = v;
  while (*v)
    {
      if (dsk_ascii_isspace (*v))
        {
          if (!last_was_space)
            *out++ = ' ';
          last_was_space = DSK_TRUE;
        }
      else
        {
          *out++ = *v;
          last_was_space = DSK_FALSE;
        }
      v++;
    }
  if (last_was_space)
    out--;
  *out = 0;
  return rv;
}

static dsk_boolean
parse_http_version (char **at_inout,
                    unsigned *maj_out,
                    unsigned *min_out,
                    DskError **error)
{
  char *at = *at_inout;
  if (!(at[0] == 'h' || at[0] == 'H')
   || !(at[1] == 't' || at[1] == 'T')
   || !(at[2] == 't' || at[2] == 'T')
   || !(at[3] == 'p' || at[3] == 'P')
   || at[4] != '/')
    {
      dsk_set_error (error, "expected HTTP/#.#");
      return DSK_FALSE;
    }
  at += 5;              /* skip 'http/' */

  /* parse major version */
  if (!dsk_ascii_isdigit (*at))
    {
      dsk_set_error (error, "expecting digit after HTTP/");
      return DSK_FALSE;
    }
  *maj_out = dsk_ascii_digit_value (*at++);
  while (dsk_ascii_isdigit (*at))
    {
      *maj_out *= 10;
      *maj_out += dsk_ascii_digit_value (*at++);
    }
  if (*at != '.')
    {
      dsk_set_error (error, "expecting . after HTTP/#");
      return DSK_FALSE;
    }
  at++;
  if (!dsk_ascii_isdigit (*at))
    {
      dsk_set_error (error, "expecting digit after HTTP/#.");
      return DSK_FALSE;
    }
  *min_out = dsk_ascii_digit_value (*at++);
  while (dsk_ascii_isdigit (*at))
    {
      *min_out *= 10;
      *min_out += dsk_ascii_digit_value (*at++);
    }
  *at_inout = at;
  return DSK_TRUE;
}

/* Used to implement hard-coded hash-tables as switch statements */
#define UNSIGNED_FROM_4_BYTES(a, b, c, d) \
    ( (((unsigned)(uint8_t)(a)) << 24)    \
    | (((unsigned)(uint8_t)(b)) << 16)    \
    | (((unsigned)(uint8_t)(c)) << 8)     \
    | (((unsigned)(uint8_t)(d))) )

/* Implementations of headers that are shared between
   requests and responses */
#define HANDLE_CONNECTION_CASE                                   \
          case UNSIGNED_FROM_4_BYTES('c', 'o', 10, 'n'):         \
            if (ascii_caseless_equals (name, "connection"))      \
              {                                                  \
                if (ascii_caseless_equals (value, "close"))      \
                  {                                              \
                    options.parsed_connection_close = 1;         \
                    continue;                                    \
                  }                                              \
                else if (ascii_caseless_equals (value, "upgrade"))\
                  {                                              \
                    options.connection_upgrade = 1;              \
                    continue;                                    \
                  }                                              \
              }                                                  \
            break;
#define HANDLE_CONTENT_ENCODING_CASE                             \
          case UNSIGNED_FROM_4_BYTES('c', 'o', 16, 'g'):         \
            if (ascii_caseless_equals (name, "content-encoding"))\
              {                                                  \
                if (!handle_content_encoding (value,             \
                                            &options.content_encoding_gzip, \
                                            error))              \
                  goto FAIL;                                     \
                continue;                                        \
              }                                                  \
            break;
#define HANDLE_CONTENT_LENGTH_CASE                               \
          case UNSIGNED_FROM_4_BYTES('c', 'o', 14, 'h'):         \
            if (ascii_caseless_equals (name, "content-length"))  \
              {                                                  \
                if (!handle_content_length (value,               \
                                            &options.content_length, \
                                            error))              \
                  goto FAIL;                                     \
                continue;                                        \
              }                                                  \
            break;
#define HANDLE_CONTENT_TYPE_CASE                                 \
          case UNSIGNED_FROM_4_BYTES('c', 'o', 12, 'e'):         \
            if (ascii_caseless_equals (name, "content-type"))    \
              {                                                  \
                options.content_type = parse_content_type (value, error); \
                if (options.content_type == NULL)                \
                  goto FAIL;                                     \
                continue;                                        \
              }                                                  \
            break;
#define HANDLE_DATE_CASE                                         \
          case UNSIGNED_FROM_4_BYTES('d', 'a', 4, 'e'):          \
            if (ascii_caseless_equals (name, "date"))            \
              {                                                  \
                if (!handle_date ("Date", value, &options.date, error)) \
                  goto FAIL;                                     \
                options.has_date = DSK_TRUE;                     \
                continue;                                        \
              }                                                  \
            break;
#define HANDLE_TRANSFER_ENCODING_HEADER                          \
          case UNSIGNED_FROM_4_BYTES('t', 'r', 17, 'g'):         \
            if (ascii_caseless_equals (name, "transfer-encoding"))\
              {                                                  \
                char *tmp = strip_string (value);                \
                if (ascii_caseless_equals (tmp, "chunked"))      \
                  options.parsed_transfer_encoding_chunked = 1;  \
                continue;                                        \
              }                                                  \
            break;
DskHttpRequest  *
dsk_http_request_parse_buffer  (DskBuffer *buffer,
                                unsigned   header_len,
                                DskError **error)
{
  char scratch[4096];
  ParseInfo pi;
  char *at;
  char *tmp;
  DskHttpRequestOptions options = DSK_HTTP_REQUEST_OPTIONS_INIT;
  DskHttpRequest *rv;
  unsigned i;
#define DEFAULT_INIT_N_MISC_HEADERS             8       /* must be power-of-two */
  char *misc_header_padding[DEFAULT_INIT_N_MISC_HEADERS*2];
  options.unparsed_headers = misc_header_padding;
  options.parsed = DSK_TRUE;
  
  if (!parse_info_init (&pi, buffer, header_len, 
                        sizeof (scratch), scratch,
                        error))
    return NULL;

  /* parse first-line */
  switch (pi.slab[0])
    {
    case 'h': case 'H':
      if ((pi.slab[1] == 'e' || pi.slab[1] == 'E')
       && (pi.slab[2] == 'a' || pi.slab[2] == 'A')
       && (pi.slab[3] == 'd' || pi.slab[3] == 'D'))
        {
          options.method = DSK_HTTP_METHOD_HEAD;
          at = pi.slab + 4;
          break;
        }
      goto handle_unknown_method;
    case 'g': case 'G':
      if ((pi.slab[1] == 'e' || pi.slab[1] == 'E')
       && (pi.slab[2] == 't' || pi.slab[2] == 'T'))
        {
          options.method = DSK_HTTP_METHOD_GET;
          at = pi.slab + 3;
          break;
        }
      goto handle_unknown_method;
    case 'p': case 'P':
      if ((pi.slab[1] == 'o' || pi.slab[1] == 'O')
       && (pi.slab[2] == 's' || pi.slab[2] == 'S')
       && (pi.slab[3] == 't' || pi.slab[3] == 'T'))
        {
          options.method = DSK_HTTP_METHOD_POST;
          at = pi.slab + 4;
          break;
        }
      if ((pi.slab[1] == 'u' || pi.slab[1] == 'U')
       && (pi.slab[2] == 't' || pi.slab[2] == 'T'))
        {
          options.method = DSK_HTTP_METHOD_PUT;
          at = pi.slab + 3;
          break;
        }
      goto handle_unknown_method;
    case 'c': case 'C':
      if ((pi.slab[1] == 'o' || pi.slab[1] == 'O')
       && (pi.slab[2] == 'n' || pi.slab[2] == 'N')
       && (pi.slab[3] == 'n' || pi.slab[3] == 'N')
       && (pi.slab[4] == 'e' || pi.slab[4] == 'E')
       && (pi.slab[5] == 'c' || pi.slab[5] == 'C')
       && (pi.slab[6] == 't' || pi.slab[6] == 'T'))
        {
          options.method = DSK_HTTP_METHOD_CONNECT;
          at = pi.slab + 7;
          break;
        }
      goto handle_unknown_method;
    case 'd': case 'D':
      if ((pi.slab[1] == 'e' || pi.slab[1] == 'E')
       && (pi.slab[2] == 'l' || pi.slab[2] == 'L')
       && (pi.slab[3] == 'e' || pi.slab[3] == 'E')
       && (pi.slab[4] == 't' || pi.slab[4] == 'T')
       && (pi.slab[5] == 'e' || pi.slab[5] == 'E'))
        {
          options.method = DSK_HTTP_METHOD_DELETE;
          at = pi.slab + 6;
          break;
        }
      goto handle_unknown_method;
    case 'o': case 'O':
      if ((pi.slab[1] == 'p' || pi.slab[1] == 'P')
       && (pi.slab[2] == 't' || pi.slab[2] == 'T')
       && (pi.slab[3] == 'i' || pi.slab[3] == 'I')
       && (pi.slab[4] == 'o' || pi.slab[4] == 'O')
       && (pi.slab[5] == 'n' || pi.slab[5] == 'N')
       && (pi.slab[6] == 's' || pi.slab[6] == 'S'))
        {
          options.method = DSK_HTTP_METHOD_OPTIONS;
          at = pi.slab + 7;
          break;
        }
      goto handle_unknown_method;
    default: handle_unknown_method:
      {
        at = pi.slab;
        while (dsk_ascii_isalnum (*at))
          at++;
        *at = 0;
        dsk_set_error (error, "unknown method '%s' in HTTP request", pi.slab);
        parse_info_clear (&pi);
        return DSK_FALSE;
      }
    }

  /* skip space after method */
  while (*at == ' ' || *at == '\t')
    at++;
  options.full_path = at;

  /* scan path */
  for (;;)
    {
      while (*at && *at != ' ' && *at != '\t')
        at++;
      if (*at == 0)
        {
          dsk_set_error (error, "missing HTTP after PATH");
          parse_info_clear (&pi);
          return NULL;
        }
      while (*at == ' ' || *at == '\t')
        at++;
      if ((at[0] == 'h' || at[0] == 'H')
       && (at[1] == 't' || at[1] == 'T')
       && (at[2] == 't' || at[2] == 'T')
       && (at[3] == 'p' || at[3] == 'P')
       &&  at[4] == '/')
        {
          break;
        }
    }

  /* "chomp" the path */
  tmp = at - 1;         /* tmp is on space before HTTP */
  while (tmp > options.full_path && dsk_ascii_isspace (*(tmp-1)))
    tmp--;
  *tmp = 0; 

  /* parse HTTP version */
  if (!parse_http_version (&at, &options.http_major_version,
                           &options.http_minor_version, error))
    goto FAIL;

  /* parse key-value pairs */
  for (i = 0; i < pi.n_kv_pairs; i++)
    {
      /* name has already been lowercased */
      char *name = pi.kv_pairs[2*i];
      unsigned name_len = strlen (name);
      unsigned v;
      char *value = pi.kv_pairs[2*i+1];
      if (name_len == 0)
        {
          dsk_set_error (error, "empty key in HTTP request (approx line %u)", 2+i);
          goto FAIL;
        }
      v = UNSIGNED_FROM_4_BYTES (to_lowercase (name[0]),
                                 to_lowercase (name[1]),
                                 name_len,
                                 to_lowercase (name[name_len-1]));
      switch (v)
        {
          HANDLE_CONNECTION_CASE        /* Connection header */
          HANDLE_CONTENT_ENCODING_CASE  /* Content-Encoding header */
          HANDLE_CONTENT_LENGTH_CASE    /* Content-Length header */
          HANDLE_CONTENT_TYPE_CASE      /* Content-Type header */
          HANDLE_DATE_CASE              /* Date header */
          case UNSIGNED_FROM_4_BYTES('h', 'o', 4, 't'):
            if (ascii_caseless_equals (name, "host"))
              {
                options.host = strip_string (value);
                continue;
              }
            break;
          HANDLE_TRANSFER_ENCODING_HEADER      /* Transfer-Encoding header */
          case UNSIGNED_FROM_4_BYTES('u', 's', 10, 't'):
            if (ascii_caseless_equals (name, "user-agent"))
              {
                options.user_agent = strip_string (value);
                continue;
              }
            break;
        }

      /* insert as misc-header */

      /* maybe we need to grow the array */
      if (options.n_unparsed_headers == DEFAULT_INIT_N_MISC_HEADERS
       && (options.n_unparsed_headers & (options.n_unparsed_headers-1)) == 0)
        {
          unsigned new_alloced = options.n_unparsed_headers == 0 ? 1 : options.n_unparsed_headers * 2;
          char **unparsed_headers = parse_info_alloc (&pi, sizeof (char*) * new_alloced * 2);
          memcpy (unparsed_headers, options.unparsed_headers, options.n_unparsed_headers * sizeof(char*) * 2);
          options.unparsed_headers = unparsed_headers;
        }
      options.unparsed_headers[options.n_unparsed_headers*2] = name;
      options.unparsed_headers[options.n_unparsed_headers*2+1] = value;
      options.n_unparsed_headers++;
    }

  rv = dsk_http_request_new (&options, error);
  parse_info_clear (&pi);
  return rv;

FAIL:
  parse_info_clear (&pi);
  return NULL;
}

DskHttpResponse  *
dsk_http_response_parse_buffer  (DskBuffer *buffer,
                                 unsigned   header_len,
                                 DskError **error)
{
  char scratch[4096];
  ParseInfo pi;
  char *at;
  DskHttpResponseOptions options = DSK_HTTP_RESPONSE_OPTIONS_INIT;
  DskHttpResponse *rv;
  char *misc_header_padding[DEFAULT_INIT_N_MISC_HEADERS*2];
  unsigned i;
  options.unparsed_headers = misc_header_padding;
  options.parsed = DSK_TRUE;
  
  if (!parse_info_init (&pi, buffer, header_len, 
                        sizeof (scratch), scratch,
                        error))
    return NULL;
  at = pi.slab;

  /* parse HTTP version */
  if (!parse_http_version (&at, &options.http_major_version,
                           &options.http_minor_version, error))
    goto FAIL;

  while (dsk_ascii_isspace (*at))
    at++;

  if (!dsk_ascii_isdigit (at[0])
   || !dsk_ascii_isdigit (at[1])
   || !dsk_ascii_isdigit (at[2]))
    {
      dsk_set_error (error, "expected HTTP status code- got %.3s", at);
      goto FAIL;
    }
  if (dsk_ascii_isdigit (at[3]))
    {
      dsk_set_error (error, "HTTP status code must be exactly three digits");
      goto FAIL;
    }
  options.status_code = dsk_ascii_digit_value(at[0]) * 100
                      + dsk_ascii_digit_value(at[1]) * 10
                      + dsk_ascii_digit_value(at[2]);

  /* parse key-value pairs */
  for (i = 0; i < pi.n_kv_pairs; i++)
    {
      /* name has already been lowercased */
      char *name = pi.kv_pairs[2*i];
      unsigned name_len = strlen (name);
      unsigned v;
      char *value = pi.kv_pairs[2*i+1];

      if (name_len == 0)
        {
          dsk_set_error (error, "empty key in HTTP request (approx line %u)", 2+i);
          goto FAIL;
        }
      v = UNSIGNED_FROM_4_BYTES (to_lowercase (name[0]),
                                 to_lowercase (name[1]),
                                 name_len,
                                 to_lowercase (name[name_len-1]));
      switch (v)
        {
          HANDLE_CONNECTION_CASE                /* Connection header */
          HANDLE_CONTENT_ENCODING_CASE          /* Content-Encoding header */
          HANDLE_CONTENT_LENGTH_CASE            /* Content-Length header */
          HANDLE_CONTENT_TYPE_CASE              /* Content-Type header */
          HANDLE_DATE_CASE                      /* Date header */
          HANDLE_TRANSFER_ENCODING_HEADER       /* Transfer-Encoding header */
          case UNSIGNED_FROM_4_BYTES('s', 'e', 6, 'r'):
            if (ascii_caseless_equals (name, "server"))
              {
                options.server = strip_string (value);
                continue;
              }
            break;
        }

      /* insert as misc-header */

      /* maybe we need to grow the array */
      if (options.n_unparsed_headers == DEFAULT_INIT_N_MISC_HEADERS
       && (options.n_unparsed_headers & (options.n_unparsed_headers-1)) == 0)
        {
          unsigned new_alloced = options.n_unparsed_headers == 0 ? 1 : options.n_unparsed_headers * 2;
          char **unparsed_headers = parse_info_alloc (&pi, sizeof (char*) * new_alloced * 2);
          memcpy (unparsed_headers, options.unparsed_headers, options.n_unparsed_headers * sizeof(char*) * 2);
          options.unparsed_headers = unparsed_headers;
        }
      options.unparsed_headers[options.n_unparsed_headers*2] = name;
      options.unparsed_headers[options.n_unparsed_headers*2+1] = value;
      options.n_unparsed_headers++;
    }

  rv = dsk_http_response_new (&options, error);
  parse_info_clear (&pi);
  return rv;

FAIL:
  parse_info_clear (&pi);
  return NULL;
}
