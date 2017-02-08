#include <string.h>
#include <stdlib.h>
#include "dsk.h"

typedef struct
{
  int         status_code;
  const char *description;
} HttpStatusDescription;

static int
status_description_comparator(const HttpStatusDescription *desc_a,
                              const HttpStatusDescription *desc_b)
{
  int a = desc_a->status_code;
  int b = desc_b->status_code;
  if (a < b)
    return -1;
  if (a > b)
    return +1;
  return 0;
}

const char* 
dsk_http_status_get_message(DskHttpStatus status)
{
  static HttpStatusDescription descriptions[] = {
    {100, "Continue"},
    {101, "Switching Protocols"},
    {200, "OK"},
    {201, "Created"},
    {202, "Accepted"},
    {203, "Non-Authoritative Information"},
    {204, "No Content"},
    {205, "Reset Content"},
    {206, "Partial Content"},
    {300, "Multiple Choices"},
    {301, "Moved Permanently"},
    {302, "Found"},
    {303, "See Other"},
    {304, "Not Modified"},
    {305, "Use Proxy"},
    {307, "Temporary Redirect"},
    {400, "Bad Request"},
    {401, "Unauthorized"},
    {402, "Payment Required"},
    {403, "Forbidden"},
    {404, "Not Found"},
    {405, "Method Not Allowed"},
    {406, "Not Acceptable"},
    {407, "Proxy Authentication Required"},
    {408, "Request Time-out"},
    {409, "Conflict"},
    {410, "Gone"},
    {411, "Length Required"},
    {412, "Precondition Failed"},
    {413, "Request Entity Too Large"},
    {414, "Request-URI Too Large"},
    {415, "Unsupported Media Type"},
    {416, "Requested range not satisfiable"},
    {417, "Expectation Failed"},
    {500, "Internal Server Error"},
    {501, "Not Implemented"},
    {502, "Bad Gateway"},
    {503, "Service Unavailable"},
    {504, "Gateway Time-out"},
    {505, "HTTP Version not supported"}
  };
  HttpStatusDescription *description;
  HttpStatusDescription id = { status, NULL };

  description = bsearch (&id,
			 descriptions,
			 DSK_N_ELEMENTS (descriptions),
			 sizeof (descriptions[0]),
			 (int(*)(const void*, const void*)) status_description_comparator);
  if (description)
    return description->description;
  return "Unknown HTTP Status Code being proxied";
}

const char *dsk_http_verb_name (DskHttpMethod method)
{
  switch (method)
    {
    case DSK_HTTP_METHOD_GET: return "GET";
    case DSK_HTTP_METHOD_POST: return "POST";
    case DSK_HTTP_METHOD_PUT: return "PUT";
    case DSK_HTTP_METHOD_HEAD: return "HEAD";
    case DSK_HTTP_METHOD_OPTIONS: return "OPTIONS";
    case DSK_HTTP_METHOD_DELETE: return "DELETE";
    case DSK_HTTP_METHOD_TRACE: return "TRACE";
    case DSK_HTTP_METHOD_CONNECT: return "CONNECT";
    default: return "*BAD_METHOD*";
    }
}
dsk_boolean dsk_http_has_response_body (DskHttpMethod request_method,
                                        DskHttpStatus response_status_code)
{
  /* See RFC 2616, 4.3 */
  return request_method != DSK_HTTP_METHOD_HEAD
      && response_status_code >= 200
      && response_status_code != 204
      && response_status_code != 304;
}

/* --- object cruft and raw initialization --- */
static void
dsk_http_request_init (DskHttpRequest *request)
{
  request->content_length = -1;
}

static void
dsk_http_request_finalize (DskHttpRequest *request)
{
  dsk_free (request->_slab);
}

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA (DskHttpRequest);
DskHttpRequestClass dsk_http_request_class =
{
  DSK_OBJECT_CLASS_DEFINE (DskHttpRequest, &dsk_object_class,
                           dsk_http_request_init,
                           dsk_http_request_finalize),
};

static void
dsk_http_response_init (DskHttpResponse *response)
{
  response->content_length = -1;
}

static void
dsk_http_response_finalize (DskHttpResponse *response)
{
  dsk_free (response->_slab);
}

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA (DskHttpResponse);
DskHttpResponseClass dsk_http_response_class =
{
  DSK_OBJECT_CLASS_DEFINE (DskHttpResponse, &dsk_object_class,
                           dsk_http_response_init,
                           dsk_http_response_finalize),
};

const char *
dsk_http_request_get (DskHttpRequest *request,
                      const char     *header)
{
  unsigned i;
  switch (header[0])
    {
    case 'c': case 'C':
      if (dsk_ascii_strcasecmp (header, "Connection") == 0)
        {
          if (request->connection_close)
            return "close";
          else if (request->connection_upgrade)
            return "upgrade";
          else
            return NULL;
        }
      break;
    case 'h': case 'H':
      if (dsk_ascii_strcasecmp (header, "Host") == 0)
        return request->host;
      break;
    case 'u': case 'U':
      if (dsk_ascii_strcasecmp (header, "User-Agent") == 0)
        return request->user_agent;
      break;
    case 'r': case 'R':
      if (dsk_ascii_strcasecmp (header, "Referer") == 0)
        return request->referrer;
      break;
    case 'p': case 'P':
      if (dsk_ascii_strcasecmp (header, "Path") == 0)
        return request->path;
      break;
    }
  for (i = 0; i < request->n_unparsed_headers; i++)
    if (dsk_ascii_strcasecmp (request->unparsed_headers[i].key, header) == 0)
      return request->unparsed_headers[i].value;
  return NULL;
}

/* --- construction --- */
typedef struct _ToCopy ToCopy;
struct _ToCopy
{
  unsigned object_offset;   /* offset into DskHttpRequest/Response structure */
  unsigned str_offset;      /* offset in str-heap */
  const char *src;          /* pointer to options structure */
  unsigned length;
};
#define MAX_TO_COPY 10  /* TODO: needs exactly measurement */

typedef struct _StrFixup StrFixup;
struct _StrFixup
{
  unsigned str_offset;
  char c;
};

#define MAX_STR_FIXUPS 10  /* TODO: needs exactly measurement */

static void
apply_copies_and_fixups (void *object,
                         unsigned n_to_copy, ToCopy *to_copy,
                         unsigned n_fixups, StrFixup *fixups,
                         char *str_slab)
{
  unsigned i;
  for (i = 0; i < n_to_copy; i++)
    {
      char *dest = str_slab + to_copy[i].str_offset;
      memcpy (dest, to_copy[i].src, to_copy[i].length);
      if (to_copy[i].object_offset != 0)
        * (char **)((char*)object + to_copy[i].object_offset) = dest;
    }
  for (i = 0; i < n_fixups; i++)
    str_slab[fixups[i].str_offset] = fixups[i].c;
}

static unsigned
phase1_handle_unparsed_headers(unsigned  n_unparsed,
                           char    **unparsed,
                           unsigned *str_alloc_inout,
                           unsigned *aligned_alloc_inout)
{
  unsigned i;
  unsigned unparsed_headers_start = *str_alloc_inout;
  for (i = 0; i < n_unparsed * 2; i++)
    *str_alloc_inout += strlen (unparsed[i]) + 1;
  *aligned_alloc_inout += n_unparsed * sizeof (DskHttpHeaderMisc);
  return unparsed_headers_start;
}

static DskHttpHeaderMisc *
phase2_handle_unparsed_headers (unsigned n_unparsed, char **unparsed,
                            char **aligned_slab_at,
                            char *str_slab_at)
{
  char *at = str_slab_at;
  DskHttpHeaderMisc *unparsed_headers = (DskHttpHeaderMisc *) (*aligned_slab_at);
  unsigned i;
  *aligned_slab_at += sizeof (DskHttpHeaderMisc) * n_unparsed;
  for (i = 0; i < n_unparsed; i++)
    {
      unparsed_headers[i].key = at;
      at = dsk_stpcpy (at, unparsed[2*i+0]) + 1;
      unparsed_headers[i].value = at;
      at = dsk_stpcpy (at, unparsed[2*i+1]) + 1;
    }
  return unparsed_headers;
}

static dsk_boolean
has_request_body (DskHttpMethod method)
{
  return (method == DSK_HTTP_METHOD_POST || method == DSK_HTTP_METHOD_PUT);
}


/* Macros shared between request/response new */

/* Add a string to the object given a source string */
#define ADD_STR(src_string, object_member)                                   \
  do{                                                                        \
    const char *the_str = (src_string);                                      \
    unsigned the_str_length = strlen (the_str) + 1;                          \
    to_copy[n_to_copy].length = the_str_length;                              \
    to_copy[n_to_copy].src = the_str;                                        \
    to_copy[n_to_copy].str_offset = str_alloc;                               \
    to_copy[n_to_copy].object_offset = offsetof (ObjectType, object_member); \
    n_to_copy++;                                                             \
    str_alloc += the_str_length;                                             \
  }while(0)
#define APPEND_CHAR_THEN_STR(char_to_append, src_string)                     \
  do{                                                                        \
    const char *the_str = (src_string);                                      \
    unsigned the_str_length = strlen (the_str) + 1;                          \
    fixups[n_fixups].c = char_to_append;                                     \
    fixups[n_fixups].str_offset = str_alloc - 1;                             \
    n_fixups++;                                                              \
    to_copy[n_to_copy].length = the_str_length;                              \
    to_copy[n_to_copy].src = the_str;                                        \
    to_copy[n_to_copy].str_offset = str_alloc;                               \
    to_copy[n_to_copy].object_offset = 0;                                    \
    n_to_copy++;                                                             \
  }while(0)
#define MAYBE_ADD_STR(src_string, object_member)                             \
  do{ if (src_string) ADD_STR (src_string, object_member); } while(0)
#define STR_DEFAULT(opt_str, default_value)                                  \
   ((opt_str) == NULL ? (default_value) : (opt_str))

#define GET_OPTIONS_UNPARSED_HEADER(options) \
      ((options)->unparsed_headers \
           ? (options)->unparsed_headers \
           : (char**)(options)->unparsed_misc_headers)

#define ObjectType DskHttpRequest
DskHttpRequest *
dsk_http_request_new (DskHttpRequestOptions *options,
                      DskError             **error)
{
  unsigned aligned_alloc = 0;
  unsigned str_alloc = 0;
  unsigned n_to_copy = 0;
  ToCopy to_copy[MAX_TO_COPY];
  unsigned n_fixups = 0;
  StrFixup fixups[MAX_STR_FIXUPS];
  DskHttpRequest *request = dsk_object_new (&dsk_http_request_class);
  unsigned unparsed_headers_start;
  char *slab;
  char *aligned_at;
  char *str_slab;

  request->method = options->method;

  /* ---- Pass 1:  compute memory needed ---- */
  /* We try to store as much information as we can
     in the "to_copy" and "fixups" arrays,
     to minimize the amount of custom work to be done in phase 2. */
  if (options->full_path != NULL)
    ADD_STR (options->full_path, path);
  else if (options->path != NULL)
    {
      ADD_STR (options->path, path);
      if (options->query != NULL)
        APPEND_CHAR_THEN_STR ('?', options->query);
    }
  else
    dsk_return_val_if_reached (NULL, NULL);

  if (options->content_type)
    {
      ADD_STR (options->content_type, content_type);
    }
  else if (options->content_main_type)
    {
      ADD_STR (options->content_type, content_type);
      APPEND_CHAR_THEN_STR ('/', STR_DEFAULT (options->content_sub_type, "*"));
      if (options->content_charset)
        APPEND_CHAR_THEN_STR ('/', options->content_charset);
    }
  MAYBE_ADD_STR (options->referrer, referrer);
  MAYBE_ADD_STR (options->user_agent, user_agent);
  MAYBE_ADD_STR (options->host, host);
  request->http_major_version = options->http_major_version;
  request->http_minor_version = options->http_minor_version;

  if (options->has_date)
    {
      request->has_date = 1;
      request->date = options->date;
    }
  if (options->parsed)
    {
      if (options->content_length >= 0)
        request->content_length = options->content_length;
      if (options->parsed_transfer_encoding_chunked)
        request->transfer_encoding_chunked = 1;
      if (options->parsed_connection_close)
        request->connection_close = 1;
    }
  else if (options->content_length >= 0 
        || options->transfer_encoding_chunked
        || has_request_body (options->method))
    {
      if (options->content_length >= 0)
        request->content_length = options->content_length;
      else if (options->http_minor_version >= 1)
        request->transfer_encoding_chunked = 1;
      else
        {
          /// not really true, although it's certainly unconventional
          dsk_set_error (error, "POST/PUT data must use Transfer-Encoding: chunked (requires HTTP/1.1) or Content-length");
          dsk_object_unref (request);
          return NULL;
        }
    }
  request->content_encoding_gzip = options->content_encoding_gzip ? 1 : 0;
  request->connection_upgrade = options->connection_upgrade;

  unparsed_headers_start
    = phase1_handle_unparsed_headers (options->n_unparsed_headers,
                                  GET_OPTIONS_UNPARSED_HEADER (options),
                                  &str_alloc, &aligned_alloc);
  /* allocate memory */
  slab = dsk_malloc (aligned_alloc + str_alloc);
  aligned_at = slab;
  str_slab = slab + aligned_alloc;

  /* ---- phase 2:  initialize structure ---- */
  request->_slab = slab;
  apply_copies_and_fixups (request,
                           n_to_copy, to_copy, n_fixups, fixups,
                           str_slab);

  /* NOTE: any special cases can go here */

  /* - Handle uninterpreted headers - */
  request->n_unparsed_headers = options->n_unparsed_headers;
  request->unparsed_headers
    = phase2_handle_unparsed_headers (options->n_unparsed_headers,
                                  GET_OPTIONS_UNPARSED_HEADER (options),
                                  &aligned_at,
                                  str_slab + unparsed_headers_start);

  /* Determine if this is a websocket request. */
  if (request->connection_upgrade)
    {
      const char *upgrade = dsk_http_request_get (request, "Upgrade");
      if (upgrade != NULL
       && dsk_ascii_strcasecmp (upgrade, "WebSocket") == 0)
        request->is_websocket_request = 1;
    }

  return request;
}
#undef ObjectType 

static dsk_boolean
is_valid_status_code (int code)
{
  switch (code/100)
    {
    case 1: return code<=101;
    case 2: return code<=206;
    case 3: return code<=306;
    case 4: return code<=417;
    case 5: return code<=505;
    default: return DSK_FALSE;
    }
}

#define ObjectType DskHttpResponse
DskHttpResponse *
dsk_http_response_new (DskHttpResponseOptions *options,
                       DskError              **error)
{
  unsigned aligned_alloc = 0;
  unsigned str_alloc = 0;
  unsigned n_to_copy = 0;
  ToCopy to_copy[MAX_TO_COPY];
  unsigned n_fixups = 0;
  StrFixup fixups[MAX_STR_FIXUPS];
  DskHttpResponse *response = dsk_object_new (&dsk_http_response_class);
  unsigned unparsed_headers_start;
  char *slab;
  char *aligned_at;
  char *str_slab;


  /* ---- Pass 1:  compute memory needed ---- */
  /* We try to store as much information as we can
     in the "to_copy" and "fixups" arrays,
     to minimize the amount of custom work to be done in phase 2. */
  if (options->content_type)
    {
      ADD_STR (options->content_type, content_type);
    }
  else if (options->content_main_type)
    {
      ADD_STR (options->content_type, content_type);
      APPEND_CHAR_THEN_STR ('/', STR_DEFAULT (options->content_sub_type, "*"));
      if (options->content_charset)
        APPEND_CHAR_THEN_STR ('/', options->content_charset);
    }
  MAYBE_ADD_STR (options->location, location);

  if (options->connection_close
   || (options->request != NULL && options->request->connection_close))
    response->connection_close = DSK_TRUE;
  if (options->connection_upgrade)
    response->connection_upgrade = options->connection_upgrade;
  response->http_major_version = options->http_major_version;
  response->http_minor_version = options->http_minor_version;
  if (!is_valid_status_code (options->status_code))
    {
      dsk_set_error (error, "invalid status code %u", options->status_code);
      dsk_object_unref (response);
      return NULL;
    }
  response->status_code = options->status_code;
  if (options->request != NULL)
    {
      if (options->request->http_minor_version == 0)
        {
          response->http_minor_version = 0;
          response->connection_close = 1;
        }
      if (options->request->connection_close)
        response->connection_close = 1;
    }
  if (options->has_date)
    {
      response->has_date = 1;
      response->date = options->date;
    }
  if (options->has_expires)
    {
      response->has_expires = 1;
      response->expires = options->expires;
    }
  if (options->parsed)
    {
      if (options->content_length >= 0)
        response->content_length = options->content_length;
      if (options->parsed_transfer_encoding_chunked)
        response->transfer_encoding_chunked = 1;
      if (options->parsed_connection_close)
        response->connection_close = 1;
    }
  else
    {
      if (options->content_length >= 0)
        response->content_length = options->content_length;
      else if (response->http_minor_version >= 1
            && !response->connection_close)
        response->transfer_encoding_chunked = 1;
      else
        response->connection_close = 1;
    }


  response->content_encoding_gzip = options->content_encoding_gzip ? 1 : 0;

  unparsed_headers_start
    = phase1_handle_unparsed_headers (options->n_unparsed_headers,
                                  GET_OPTIONS_UNPARSED_HEADER (options),
                                  &str_alloc, &aligned_alloc);

  /* allocate memory */
  slab = dsk_malloc (aligned_alloc + str_alloc);
  aligned_at = slab;
  str_slab = slab + aligned_alloc;

  /* ---- phase 2:  initialize structure ---- */
  response->_slab = slab;
  apply_copies_and_fixups (response,
                           n_to_copy, to_copy, n_fixups, fixups,
                           str_slab);

  /* NOTE: any special cases can go here */

  /* - Handle uninterpreted headers - */
  response->n_unparsed_headers = options->n_unparsed_headers;
  response->unparsed_headers
    = phase2_handle_unparsed_headers (options->n_unparsed_headers,
                                  options->unparsed_headers ? options->unparsed_headers : (char**)options->unparsed_misc_headers,
                                  &aligned_at,
                                  str_slab + unparsed_headers_start);

  return response;
}
#undef ObjectType
void
dsk_http_request_init_options (DskHttpRequest *header,
                               DskHttpRequestOptions *out)
{
  out->method = header->method;
  /* TODO: simple assignment */
#define ASSIGN(field) out->field = header->field
  ASSIGN (method);
  ASSIGN (http_major_version);
  ASSIGN (http_minor_version);
  out->full_path = header->path;
  ASSIGN (host);
  ASSIGN (content_type);
  ASSIGN (content_length);
  ASSIGN (content_encoding_gzip);
  ASSIGN (transfer_encoding_chunked);
  ASSIGN (has_date);
  ASSIGN (date);
  ASSIGN (referrer);
  ASSIGN (user_agent);
  ASSIGN (n_unparsed_headers);
  out->unparsed_headers = NULL;
  out->unparsed_misc_headers = header->unparsed_headers;
  out->full_path = header->path;
  out->unparsed_headers = NULL;
  out->unparsed_misc_headers = header->unparsed_headers;
#undef ASSIGN

  /* Set to constant values */
  out->path = NULL;
  out->query = NULL;
  out->content_main_type = NULL;
  out->content_sub_type = NULL;
  out->content_charset = NULL;

  /* --- parser interface --- */
  out->parsed = DSK_FALSE;
}
