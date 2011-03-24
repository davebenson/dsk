/*
 * As per the HTTP 1.1 Specification, RFC 2616.
 *
 * TODO:  Compliance notes.
 */

typedef struct _DskHttpHeaderClass DskHttpHeaderClass;
typedef struct _DskHttpHeader DskHttpHeader;
typedef struct _DskHttpAuthorization DskHttpAuthorization;
typedef struct _DskHttpAuthenticate DskHttpAuthenticate;
typedef struct _DskHttpCharSet DskHttpCharSet;
typedef struct _DskHttpResponseCacheDirective DskHttpResponseCacheDirective;
typedef struct _DskHttpRequestCacheDirective DskHttpRequestCacheDirective;
typedef struct _DskHttpCookie DskHttpCookie;
typedef struct _DskHttpLanguageSet DskHttpLanguageSet;
typedef struct _DskHttpMediaTypeSet DskHttpMediaTypeSet;
typedef struct _DskHttpContentEncodingSet DskHttpContentEncodingSet;
typedef struct _DskHttpTransferEncodingSet DskHttpTransferEncodingSet;
typedef struct _DskHttpRangeSet DskHttpRangeSet;


typedef enum
{
  DSK_HTTP_STATUS_CONTINUE                      = 100,
  DSK_HTTP_STATUS_SWITCHING_PROTOCOLS           = 101,
  DSK_HTTP_STATUS_OK                            = 200,
  DSK_HTTP_STATUS_CREATED                       = 201,
  DSK_HTTP_STATUS_ACCEPTED                      = 202,
  DSK_HTTP_STATUS_NONAUTHORITATIVE_INFO         = 203,
  DSK_HTTP_STATUS_NO_CONTENT                    = 204,
  DSK_HTTP_STATUS_RESET_CONTENT                 = 205,
  DSK_HTTP_STATUS_PARTIAL_CONTENT               = 206,
  DSK_HTTP_STATUS_MULTIPLE_CHOICES              = 300,
  DSK_HTTP_STATUS_MOVED_PERMANENTLY             = 301,
  DSK_HTTP_STATUS_FOUND                         = 302,
  DSK_HTTP_STATUS_SEE_OTHER                     = 303,
  DSK_HTTP_STATUS_NOT_MODIFIED                  = 304,
  DSK_HTTP_STATUS_USE_PROXY                     = 305,
  DSK_HTTP_STATUS_TEMPORARY_REDIRECT            = 306,
  DSK_HTTP_STATUS_BAD_REQUEST                   = 400,
  DSK_HTTP_STATUS_UNAUTHORIZED                  = 401,
  DSK_HTTP_STATUS_PAYMENT_REQUIRED              = 402,
  DSK_HTTP_STATUS_FORBIDDEN                     = 403,
  DSK_HTTP_STATUS_NOT_FOUND                     = 404,
  DSK_HTTP_STATUS_METHOD_NOT_ALLOWED            = 405,
  DSK_HTTP_STATUS_NOT_ACCEPTABLE                = 406,
  DSK_HTTP_STATUS_PROXY_AUTH_REQUIRED           = 407,
  DSK_HTTP_STATUS_REQUEST_TIMEOUT               = 408,
  DSK_HTTP_STATUS_CONFLICT                      = 409,
  DSK_HTTP_STATUS_GONE                          = 410,
  DSK_HTTP_STATUS_LENGTH_REQUIRED               = 411,
  DSK_HTTP_STATUS_PRECONDITION_FAILED           = 412,
  DSK_HTTP_STATUS_ENTITY_TOO_LARGE              = 413,
  DSK_HTTP_STATUS_URI_TOO_LARGE                 = 414,
  DSK_HTTP_STATUS_UNSUPPORTED_MEDIA             = 415,
  DSK_HTTP_STATUS_BAD_RANGE                     = 416,
  DSK_HTTP_STATUS_EXPECTATION_FAILED            = 417,
  DSK_HTTP_STATUS_INTERNAL_SERVER_ERROR         = 500,
  DSK_HTTP_STATUS_NOT_IMPLEMENTED               = 501,
  DSK_HTTP_STATUS_BAD_GATEWAY                   = 502,
  DSK_HTTP_STATUS_SERVICE_UNAVAILABLE           = 503,
  DSK_HTTP_STATUS_GATEWAY_TIMEOUT               = 504,
  DSK_HTTP_STATUS_UNSUPPORTED_VERSION           = 505
} DskHttpStatus;

/*
 * The Verb is the first text transmitted
 * (from the user-agent to the server).
 */
typedef enum
{
  DSK_HTTP_VERB_GET,
  DSK_HTTP_VERB_POST,
  DSK_HTTP_VERB_PUT,
  DSK_HTTP_VERB_HEAD,
  DSK_HTTP_VERB_OPTIONS,
  DSK_HTTP_VERB_DELETE,
  DSK_HTTP_VERB_TRACE,
  DSK_HTTP_VERB_CONNECT
} DskHttpVerb;

/* A DskHttpRange is a unit in which partial content ranges
 * may be specified and transferred.
 */
typedef enum
{
  DSK_HTTP_RANGE_BYTES
} DskHttpRange;

typedef enum {
  DSK_HTTP_CONTENT_ENCODING_IDENTITY,
  DSK_HTTP_CONTENT_ENCODING_GZIP,
  DSK_HTTP_CONTENT_ENCODING_COMPRESS,
  DSK_HTTP_CONTENT_ENCODING_UNRECOGNIZED = 0x100
} DskHttpContentEncoding;

/*
 * The Transfer-Encoding field of HTTP/1.1.
 *
 * In particular, HTTP/1.1 compliant clients and proxies
 * MUST support `chunked'.  The compressed Transfer-Encodings
 * are rarely (if ever) used.
 *
 * Note that:
 *   - we interpret this field, even for HTTP/1.0 clients.
 */
typedef enum {
  DSK_HTTP_TRANSFER_ENCODING_NONE    = 0,
  DSK_HTTP_TRANSFER_ENCODING_CHUNKED = 1,
  DSK_HTTP_TRANSFER_ENCODING_UNRECOGNIZED = 0x100
} DskHttpTransferEncoding;

/*
 * The Connection: header enables or disables http-keepalive.
 *
 * For HTTP/1.0, NONE should be treated like CLOSE.
 * For HTTP/1.1, NONE should be treated like KEEPALIVE.
 *
 * Use dsk_http_header_get_connection () to do this automatically -- it
 * always returns DSK_HTTP_CONNECTION_CLOSE or DSK_HTTP_CONNECTION_KEEPALIVE.
 *
 * See RFC 2616, Section 14.10.
 */
typedef enum
{
  DSK_HTTP_CONNECTION_NONE       = 0,
  DSK_HTTP_CONNECTION_CLOSE      = 1,
  DSK_HTTP_CONNECTION_KEEPALIVE  = 2,
} DskHttpConnection;

/*
 * The Cache-Control response directive.
 * See RFC 2616, Section 14.9 (cache-response-directive)
 */
struct _DskHttpResponseCacheDirective
{
  /*< public (read/write) >*/
  /* the http is `public' and `private'; is_ is added
   * for C++ users.
   */
  guint   is_public : 1;
  guint   is_private : 1;

  guint   no_cache : 1;
  guint   no_store : 1;
  guint   no_transform : 1;

  guint   must_revalidate : 1;
  guint   proxy_revalidate : 1;
  guint   max_age;
  guint   s_max_age;

  /*< public (read-only) >*/
  char   *private_name;
  char   *no_cache_name;

  /* TODO: what about cache-extensions? */
};

/*
 * The Cache-Control request directive.
 * See RFC 2616, Section 14.9 (cache-request-directive)
 */
struct _DskHttpRequestCacheDirective
{
  guint no_cache : 1;
  guint no_store : 1;
  guint no_transform : 1;
  guint only_if_cached : 1;

  guint max_age;
  guint min_fresh;

 /* 
  * We need to be able to indicate that max_stale is set without the 
  * optional argument. So:
  *		      0 not set
  *		     -1 set no arg
  *		     >0 set with arg.	  
  */
  gint  max_stale;

  /* TODO: what about cache-extensions? */
};


DskHttpResponseCacheDirective *dsk_http_response_cache_directive_new (void);
void dsk_http_response_cache_directive_set_private_name (
				     DskHttpResponseCacheDirective *directive,
				     const char            *name,
				     gsize                  name_len);
void dsk_http_response_cache_directive_set_no_cache_name (
				     DskHttpResponseCacheDirective *directive,
				     const char            *name,
				     gsize                  name_len);
void dsk_http_response_cache_directive_free(
				    DskHttpResponseCacheDirective *directive);


DskHttpRequestCacheDirective *dsk_http_request_cache_directive_new (void);
void dsk_http_request_cache_directive_free(
				     DskHttpRequestCacheDirective *directive);


/*
 * The Accept: request-header.
 *
 * See RFC 2616, Section 14.1.
 *
 * TODO: support level= accept-extension.
 */
struct _DskHttpMediaTypeSet
{
  /*< public: read-only >*/
  char                 *type;
  char                 *subtype;
  gfloat                quality;                /* -1 if not present */

  /*< private >*/
  DskHttpMediaTypeSet  *next;
};
DskHttpMediaTypeSet *dsk_http_media_type_set_new (const char *type,
                                                  const char *subtype,
                                                  gfloat      quality);
void                 dsk_http_media_type_set_free(DskHttpMediaTypeSet *set);


/*
 * The Accept-Charset: request-header.
 *
 * See RFC 2616, Section 14.2.
 */
struct _DskHttpCharSet
{
  /*< public: read-only >*/
  char                 *charset_name;
  gfloat                quality;                /* -1 if not present */

  /*< private >*/
  DskHttpCharSet       *next;
};
DskHttpCharSet       *dsk_http_char_set_new (const char *charset_name,
                                             gfloat      quality);
void                  dsk_http_char_set_free(DskHttpCharSet *char_set);

/*
 * The Accept-Encoding: request-header.
 *
 * See RFC 2616, Section 14.3.
 */
struct _DskHttpContentEncodingSet
{
  /*< public: read-only >*/
  DskHttpContentEncoding       encoding;
  gfloat                       quality;       /* -1 if not present */

  /*< private >*/
  DskHttpContentEncodingSet   *next;
};
DskHttpContentEncodingSet *
dsk_http_content_encoding_set_new (DskHttpContentEncoding encoding,
				   gfloat                 quality);
void
dsk_http_content_encoding_set_free(DskHttpContentEncodingSet *encoding_set);

/*
 * for the TE: request-header.
 *
 * See RFC 2616, Section 14.39.
 */
struct _DskHttpTransferEncodingSet
{
  /*< public: read-only >*/
  DskHttpTransferEncoding      encoding;
  gfloat                       quality;       /* -1 if not present */

  /*< private >*/
  DskHttpTransferEncodingSet   *next;
};
DskHttpTransferEncodingSet *
dsk_http_transfer_encoding_set_new (DskHttpTransferEncoding     encoding,
				    gfloat                      quality);
void
dsk_http_transfer_encoding_set_free(DskHttpTransferEncodingSet *encoding_set);

struct _DskHttpRangeSet
{
  /*< public: read-only >*/
  DskHttpRange          range_type;

  /*< private >*/
  DskHttpRangeSet   *next;
};
DskHttpRangeSet *dsk_http_range_set_new (DskHttpRange range_type);
void             dsk_http_range_set_free(DskHttpRangeSet *set);


/*
 * The Accept-Language: request-header.
 *
 * See RFC 2616, Section 14.4.
 */
struct _DskHttpLanguageSet
{
  /*< public: read-only >*/

  /* these give a language (with optional dialect specifier) */
  char                 *language;
  gfloat                quality;                /* -1 if not present */

  /*< private >*/
  DskHttpLanguageSet   *next;
};
DskHttpLanguageSet *dsk_http_language_set_new (const char *language,
                                               gfloat      quality);
void                dsk_http_language_set_free(DskHttpLanguageSet *set);

typedef enum
{
  DSK_HTTP_AUTH_MODE_UNKNOWN,
  DSK_HTTP_AUTH_MODE_BASIC,
  DSK_HTTP_AUTH_MODE_DIGEST
} DskHttpAuthMode;

/* HTTP Authentication.
   
   These structures give map to the WWW-Authenticate/Authorization headers,
   see RFC 2617.

   The outline is:
     If you get a 401 (Unauthorized) header, the server will
     accompany that with information about how to authenticate,
     in the WWW-Authenticate header.
     
     The user-agent should prompt the user for a username/password.

     Then the client tries again: but this time with an appropriate Authorization.
     If the server is satified, it will presumably give you the content.
 */
struct _DskHttpAuthenticate
{
  DskHttpAuthMode mode;
  char           *auth_scheme_name;
  char           *realm;
  union
  {
    struct {
      char                   *options;
    } unknown;
    /* no members:
    struct {
    } basic;
    */
    struct {
      char                   *domain;
      char                   *nonce;
      char                   *opaque;
      gboolean                is_stale;
      char                   *algorithm;
    } digest;
  } info;
  guint           ref_count;            /*< private >*/
};
DskHttpAuthenticate *dsk_http_authenticate_new_unknown (const char          *auth_scheme_name,
                                                        const char          *realm,
                                                        const char          *options);
DskHttpAuthenticate *dsk_http_authenticate_new_basic   (const char          *realm);
DskHttpAuthenticate *dsk_http_authenticate_new_digest  (const char          *realm,
                                                        const char          *domain,
                                                        const char          *nonce,
                                                        const char          *opaque,
                                                        const char          *algorithm);
DskHttpAuthenticate  *dsk_http_authenticate_ref        (DskHttpAuthenticate *auth);
void                  dsk_http_authenticate_unref      (DskHttpAuthenticate *auth);

struct _DskHttpAuthorization
{
  DskHttpAuthMode mode;
  char           *auth_scheme_name;
  union
  {
    struct {
      char                   *response;
    } unknown;
    struct {
      char                   *user;
      char                   *password;
    } basic;
    struct {
      char                   *realm;
      char                   *domain;
      char                   *nonce;
      char                   *opaque;
      char                   *algorithm;
      char                   *user;
      char                   *password;
      char                   *response_digest;
      char                   *entity_digest;
    } digest;
  } info;
  guint           ref_count;            /*< private >*/
};
DskHttpAuthorization *dsk_http_authorization_new_unknown (const char *auth_scheme_name,
                                                          const char *response);
DskHttpAuthorization *dsk_http_authorization_new_basic   (const char *user,
                                                          const char *password);
DskHttpAuthorization *dsk_http_authorization_new_digest  (const char *realm,
                                                          const char *domain,
                                                          const char *nonce,
                                                          const char *opaque,
                                                          const char *algorithm,
                                                          const char *user,
                                                          const char *password,
                                                          const char *response_digest,
                                                          const char *entity_digest);
DskHttpAuthorization *dsk_http_authorization_new_respond (const DskHttpAuthenticate *,
                                                          const char *user,
                                                          const char *password,
                                                          GError    **error);
DskHttpAuthorization *dsk_http_authorization_new_respond_post(const DskHttpAuthenticate *,
                                                          const char *user,
                                                          const char *password,
                                                          guint       post_data_len,
                                                          gconstpointer post_data,
                                                          GError    **error);
const char           *dsk_http_authorization_peek_response_digest (DskHttpAuthorization *);
DskHttpAuthorization *dsk_http_authorization_copy        (const DskHttpAuthorization *);
void                  dsk_http_authorization_set_nonce   (DskHttpAuthorization *,
                                                          const char *nonce);
DskHttpAuthorization *dsk_http_authorization_ref         (DskHttpAuthorization *);
void                  dsk_http_authorization_unref       (DskHttpAuthorization *);

/* an update to an existing authentication,
   to verify that we're talking to the same host. */
struct _DskHttpAuthenticateInfo
{
  char *next_nonce;
  char *response_digest;
  char *cnonce;
  guint has_nonce_count;
  guint32 nonce_count;
};

/* A single `Cookie' or `Set-Cookie' header.
 *
 * See RFC 2109, HTTP State Management Mechanism 
 */
struct _DskHttpCookie
{
  /*< public: read-only >*/
  char                   *key;
  char                   *value;
  char                   *domain;
  char                   *path;
  char                   *expire_date;
  char                   *comment;
  int                     max_age;
  gboolean                secure;               /* default is FALSE */
  guint                   version;              /* default is 0, unspecified */
};
DskHttpCookie  *dsk_http_cookie_new              (const char     *key,
                                                  const char     *value,
                                                  const char     *path,
                                                  const char     *domain,
                                                  const char     *expire_date,
                                                  const char     *comment,
                                                  int             max_age);
DskHttpCookie  *dsk_http_cookie_copy             (const DskHttpCookie *orig);
void            dsk_http_cookie_free             (DskHttpCookie *orig);


/*
 *                 DskHttpHeader
 *
 * A structure embodying an http header
 * (as in a request or a response).
 */
struct _DskHttpHeaderClass
{
  GObjectClass                  base_class;
};

struct _DskHttpHeader
{
  GObject                       base_instance;

  /*< public >*/  /* read-write */
  guint16                       http_major_version;             /* always 1 */
  guint16                       http_minor_version;

  DskHttpConnection             connection_type;

  DskHttpTransferEncoding       transfer_encoding_type;
  DskHttpContentEncoding        content_encoding_type;
  DskHttpRangeSet              *accepted_range_units;  /* Accept-Ranges */

  /*< public >*/
  char                         *unrecognized_transfer_encoding;
  char                         *unrecognized_content_encoding;

  char                     *content_encoding;     /* Content-Encoding */

  unsigned                  has_content_type : 1;

  char                     *content_type;             /* Content-Type */
  char                     *content_subtype;
  char                     *content_charset;
  GSList                   *content_additional;

  /* NULL-terminated array of language tags from the Content-Language
   * header, or NULL if header not present. */
  char                    **content_languages;

  /* Bytes ranges. Both with be == -1 if there is no Range tag. */
  int                           range_start;
  int                           range_end;

  /* may be set to ((time_t) -1) to omit them. */
  glong                         date;

  /* From the Content-Length header. */
  gint64                        content_length;

  /*< public >*/

  /* Key/value searchable header lines. */
  GHashTable                   *header_lines;

  /* Error messages.  */
  GSList                       *errors;		      /* list of char* Error: directives */

  /* General headers.  */
  GSList                       *pragmas;

  /* and actual accumulated parse error (a bit of a hack) */
  GError                       *g_error;
};


/* Public methods to parse/write a header. */
typedef enum
{
  DSK_HTTP_PARSE_STRICT = (1 << 0),

  /* instead of giving up on unknown headers, just 
   * add them as misc-headers */
  DSK_HTTP_PARSE_SAVE_ERRORS = (1 << 1)
} DskHttpParseFlags;

DskHttpHeader  *dsk_http_header_from_buffer      (DskBuffer     *input,
						  gboolean       is_request,
                                                  DskHttpParseFlags flags,
                                                  GError        **error);
void            dsk_http_header_to_buffer        (DskHttpHeader *header,
                                                  DskBuffer     *output);


/* response specific functions */
/* unhandled: content-type and friends */
void             dsk_http_header_set_content_encoding(DskHttpHeader *header,
                                                     const char      *encoding);

/*content_type; content_subtype; content_charset; content_additional;*/

typedef struct _DskHttpContentTypeInfo DskHttpContentTypeInfo;
struct _DskHttpContentTypeInfo
{
  const char *type_start;
  guint type_len;
  const char *subtype_start;
  guint subtype_len;
  const char *charset_start;
  guint charset_len;
  guint max_additional;         /* unimplemented */
  guint n_additional;           /* unimplemented */
  const char **additional_starts; /* unimplemented */
  guint *additional_lens;         /* unimplemented */
};
typedef enum
{
  DSK_HTTP_CONTENT_TYPE_PARSE_ADDL = (1<<0) /* unimplemented */
} DskHttpContentTypeParseFlags;

gboolean dsk_http_content_type_parse (const char *content_type_header,
                                      DskHttpContentTypeParseFlags flags,
                                      DskHttpContentTypeInfo *out,
                                      GError                **error);



/* --- miscellaneous key/value pairs --- */
void             dsk_http_header_add_misc     (DskHttpHeader *header,
                                               const char    *key,
                                               const char    *value);
void             dsk_http_header_remove_misc  (DskHttpHeader *header,
                                               const char    *key);
const char      *dsk_http_header_lookup_misc  (DskHttpHeader *header,
                                               const char    *key);

/* XXX: need to figure out the clean way to replace this one
 * (probably some generic dsk_make_randomness (CHAR-SET, LENGTH) trick)
 */
/*char                *dsk_http_header_gen_random_cookie();*/


typedef struct _DskHttpHeaderLineParser DskHttpHeaderLineParser;
typedef gboolean (*DskHttpHeaderLineParserFunc) (DskHttpHeader *header,
						 const char    *text,
						 gpointer       data);
struct _DskHttpHeaderLineParser
{
  const char *name;
  DskHttpHeaderLineParserFunc func;
  gpointer data;
};
  
/* The returned table is a map from g_str_hash(lowercase(header)) to
   DskHttpHeaderLineParser. */
GHashTable        *dsk_http_header_get_parser_table(gboolean       is_request);

/* Standard header constructions... */


/* --- outputting an http header --- */
typedef void (*DskHttpHeaderPrintFunc) (const char       *text,
					gpointer          data);
void              dsk_http_header_print (DskHttpHeader          *http_header,
		                         DskHttpHeaderPrintFunc  print_func,
		                         gpointer                print_data);


DskHttpConnection    dsk_http_header_get_connection (DskHttpHeader *header);
void                 dsk_http_header_set_version    (DskHttpHeader *header,
						     gint           major,
						     gint           minor);
void                 dsk_http_header_add_pragma     (DskHttpHeader *header,
                                                     const char    *pragma);
void             dsk_http_header_add_accepted_range (DskHttpHeader *header,
                                                     DskHttpRange   range);


#define dsk_http_header_set_connection_type(header, connection_type)	      \
  g_object_set (DSK_HTTP_HEADER(header), "connection", (DskHttpConnection) (connection_type), NULL)
#define dsk_http_header_get_connection_type(header)			      \
  (DSK_HTTP_HEADER(header)->connection_type)
#define dsk_http_header_set_transfer_encoding(header, enc)		      \
  g_object_set (DSK_HTTP_HEADER(header), "transfer-encoding", (DskHttpTransferEncoding) (enc), NULL)
#define dsk_http_header_get_transfer_encoding(header)			      \
  (DSK_HTTP_HEADER(header)->transfer_encoding_type)
#define dsk_http_header_set_content_encoding(header, enc)		      \
  g_object_set (DSK_HTTP_HEADER(header), "content-encoding", (DskHttpContentEncoding) (enc), NULL)
#define dsk_http_header_get_content_encoding(header)			      \
  (DSK_HTTP_HEADER(header)->content_encoding_type)
#define dsk_http_header_set_content_length(header, length)		              \
  g_object_set (DSK_HTTP_HEADER(header), "content-length", (gint64) (length), NULL)
#define dsk_http_header_get_content_length(header)			      \
  (DSK_HTTP_HEADER(header)->content_length)
#define dsk_http_header_set_range(header, start, end)		              \
  g_object_set (DSK_HTTP_HEADER(header), "range-start", (gint) (start), "range-end", (gint) (end), NULL)
#define dsk_http_header_get_range_start(header)			              \
  (DSK_HTTP_HEADER(header)->range_start)
#define dsk_http_header_get_range_end(header)			              \
  (DSK_HTTP_HEADER(header)->range_end)
#define dsk_http_header_set_date(header, date)			      	      \
  g_object_set (DSK_HTTP_HEADER(header), "date", (long) (date), NULL)
#define dsk_http_header_get_date(header)				      \
  (DSK_HTTP_HEADER(header)->date)

/*< private >*/
void dsk_http_header_set_string (gpointer         http_header,
                                 char           **p_str,
                                 const char      *str);
void dsk_http_header_set_string_len (gpointer         http_header,
                                     char           **p_str,
                                     const char      *str,
                                     guint            length);

void dsk_http_header_set_string_val (gpointer         http_header,
                                     char           **p_str,
                                     const GValue    *value);

char * dsk_http_header_cut_string (gpointer    http_header,
                                   const char *start,
                                   const char *end);

void dsk_http_header_free_string (gpointer http_header,
			          char    *str);
void dsk_http_header_set_connection_string (DskHttpHeader *header,
                                            const char    *str);
void dsk_http_header_set_content_encoding_string  (DskHttpHeader *header,
                                                   const char    *str);
void dsk_http_header_set_transfer_encoding_string (DskHttpHeader *header,
                                                   const char    *str);

#define dsk_http_header_set_content_type(header, content_type)	      \
  g_object_set (DSK_HTTP_HEADER(header), "content-type", (const char *)(content_type), NULL)
#define dsk_http_header_get_content_type(header)			      \
  (DSK_HTTP_HEADER(header)->content_type)
#define dsk_http_header_set_content_subtype(header, content_type)	      \
  g_object_set (DSK_HTTP_HEADER(header), "content-subtype", (const char *)(content_type), NULL)
#define dsk_http_header_get_content_subtype(header)			      \
  (DSK_HTTP_HEADER(header)->content_subtype)
#define dsk_http_header_set_content_charset(header, content_type)	      \
  g_object_set (DSK_HTTP_HEADER(header), "content-charset", (const char *)(content_type), NULL)
#define dsk_http_header_get_content_charset(header)			      \
  (DSK_HTTP_HEADER(header)->content_charset)

G_END_DECLS

#endif
