#include <string.h>
#include <ctype.h>
#include "dsk.h"

/* Relevant RFC's.
2045: MIME: Format of Message Bodies
2046: MIME: Media Types
 */

#define DEBUG_MIME_DECODER 0

/* Relevant, but the extensions are mostly unimplemented:
2183: Content-Disposition header type
2387: mime type: MultipartDecoder/Related
2388: mime type: MultipartDecoder/Form-Data
 */

/* TODO: the lines in headers can not be broken up with newlines yet! */

typedef enum
{
  /* waiting for an initial boundary string (could be terminal);
     non-boundary lines are the "preamble" and must be ignored. */
  STATE_WAITING_FOR_FIRST_SEP_LINE,

  /* Gathering header of next piece in 'buffer'.

     When the full header is gathered, a new 
     last_piece will be allocated.
     (this may cause writability notification to start.)

     However, if the 'part' is in memory mode,
     but the full data has not been received,
     then it is NOT appropriate to notify the user
     of the availability of this part, because
     memory mode implies that we gather the 
     full content before notifying the user.  */
  STATE_READING_HEADER,

  /* In these state, content must be written to
     FEED_STREAM, noting that we must
     scan for the "boundary".

     We generally process the entire buffer,
     except if there is ambiguity and then we 
     keep the whole beginning of the line around.

     So the parse algorithm is:
       while we have data to be processed:
         Scan for line starts,
	 noting what they are followed with:
	   - Is Boundary?
	     Process pending data, shutdown and break.
	   - Is Boundary prefix?
	     Process pending data, and break.
	   - Cannot be boundary?
	     Continue scanning for next line start.
	     The line is considered pending data.
   */
  STATE_CONTENT_LINE_START_CRLF,
  STATE_CONTENT_LINE_START,
  STATE_CONTENT_MIDLINE,

  STATE_ENDED
} State;

typedef struct _DskMimeMultipartDecoderClass DskMimeMultipartDecoderClass;
struct _DskMimeMultipartDecoderClass
{
  DskObjectClass base_class;
};

struct _DskMimeMultipartDecoder
{
  DskObject base_instance;
  State state;
  DskBuffer incoming;

  /* configuration from the Content-Type header */
  char *start, *start_info, *boundary_str;

  /* cached for optimization */
  unsigned boundary_str_len;
  char *boundary_buf;

  DskCgiVariable cur_piece;
  DskBuffer cur_buffer;
  DskOctetFilter *transfer_decoder;
  DskMemPool header_storage;
#define DSK_MIME_MULTIPART_DECODER_INIT_HEADER_SPACE  1024
  char *header_pad;
  
  unsigned n_pieces;
  unsigned pieces_alloced;
  DskCgiVariable *pieces;
#define DSK_MIME_MULTIPART_DECODER_N_INIT_PIECES 4
  DskCgiVariable pieces_init[DSK_MIME_MULTIPART_DECODER_N_INIT_PIECES];
#define BOUNDARY_BUF_PAD_SIZE                    64
  char boundary_buf_init[BOUNDARY_BUF_PAD_SIZE];
};



/* --- functions --- */

#if DEBUG_MIME_DECODER
static const char *
state_to_string (guint state)
{
  switch (state)
    {
#define CASE_RET_AS_STRING(st)  case st: return #st
    CASE_RET_AS_STRING(STATE_INITED);
    CASE_RET_AS_STRING(STATE_WAITING_FOR_FIRST_SEP_LINE);
    CASE_RET_AS_STRING(STATE_READING_HEADER);
    CASE_RET_AS_STRING(STATE_CONTENT_LINE_START);
    CASE_RET_AS_STRING(STATE_CONTENT_MIDLINE);
    CASE_RET_AS_STRING(STATE_ENDED);
#undef CASE_RET_AS_STRING
    }
  dsk_return_val_if_reached (NULL);
}
#define YELL(decoder) dsk_message ("at line %u, state=%s [buf-size=%u]",__LINE__,state_to_string ((decoder)->state), (decoder)->buffer.size)
#endif

static dsk_boolean
process_header_line (DskMimeMultipartDecoder *decoder,
                     const char              *line,
                     DskError               **error)
{
  DskCgiVariable *cur = &decoder->cur_piece;
  if (dsk_ascii_strncasecmp (line, "content-type:", 13) == 0)
    {
      /* Example:
	 text/plain; charset=latin-1

	 See RFC 2045, Section 5.

         We ignore all pieces except charset.
       */
      const char *start = line + 13;
      const char *slash = strchr (start, '/');
      while (dsk_ascii_isspace (*start))
        start++;
      if (slash == NULL)
	{
	  dsk_set_error (error, "content-type expected to contain a '/'");
	  return DSK_FALSE;
	}

      {
      const char *content_main_type_start = start;
      const char *content_main_type_end = slash;
      const char *at = slash + 1;
      const char *content_subtype_start = at;
      const char *content_charset_start = NULL;
      const char *content_charset_end = NULL;
      const char *content_subtype_end;
      unsigned main_len, sub_len, len;
      char *ct;
#define IS_SUBTYPE_CHAR(c) dsk_ascii_istoken (c)
#define IS_SEP_TYPE_CHAR(c) (dsk_ascii_isspace(c) || ((c)==';'))
      while (IS_SUBTYPE_CHAR (*at))
        at++;
      content_subtype_end = at;
      for (;;)
	{
	  const char *key_start, *key_end;
	  const char *value_start, *value_end;
	  const char *equals;
          while (IS_SEP_TYPE_CHAR (*at))
            at++;
	  if (*at == '\0')
	    break;
          key_start = at;
	  equals = at;
          while (IS_SUBTYPE_CHAR (*equals))
            equals++;
	  if (*equals != '=')
	    {
	      dsk_set_error (error, "expected '=' in key-value pairs on content-type");
	      return DSK_FALSE;
	    }
          key_end = equals;
	  value_start = equals + 1;
	  value_end = value_start;
	  if (*value_start == '"')
	    {
	      value_end = strchr (value_start + 1, '"');
	      if (value_end == NULL)
		{
		  dsk_set_error (error, "missing terminal '\"' in key/value pair in content-type");
		  return DSK_FALSE;
		}

              at = value_end + 1;
              value_start++;
	    }
	  else
	    {
              while (IS_SUBTYPE_CHAR (*value_end))
                value_end++;
              at = value_end;
	    }
	  if (dsk_ascii_strncasecmp (key_start, "charset", 7) == 0
              && key_end - key_start == 7)
	    {
              content_charset_start = value_start;
              content_charset_end = value_end;
	      continue;
	    }
	}
      main_len = content_main_type_end - content_main_type_start;
      sub_len = content_subtype_end - content_subtype_start;
      len = main_len + 1 + sub_len + 1;
      if (content_charset_start != NULL)
        len += content_charset_end - content_charset_start
             + 1;
      ct = dsk_mem_pool_alloc_unaligned (&decoder->header_storage, len);
      memcpy (ct, content_main_type_start, main_len);
      ct[main_len] = '/';
      memcpy (ct + main_len + 1, content_subtype_start, sub_len);
      if (content_charset_start)
        {
          ct[main_len + 1 + sub_len] = '/';
          memcpy (ct + main_len + 1 + sub_len + 1, content_charset_start,
                  content_charset_end - content_charset_start);
        }
      ct[len-1] = 0;
      cur->content_type = ct;
      }
    }
  else if (dsk_ascii_strncasecmp (line, "content-id:", 11) == 0)
    {
      const char *start = strchr(line, ':') + 1;
      char *id;
      while (dsk_ascii_isspace (*start))
        start++;
      id = dsk_mem_pool_strdup (&decoder->header_storage, start);
      dsk_ascii_strchomp (id);
      cur->key = id;
    }
  else if (dsk_ascii_strncasecmp (line, "content-location:", 17) == 0)
    {
      const char *start = strchr(line, ':') + 1;
      char *location;
      while (dsk_ascii_isspace (*start))
        start++;
      location = dsk_mem_pool_strdup (&decoder->header_storage, start);
      dsk_ascii_strchomp (location);
      cur->content_location = location;
    }
  else if (dsk_ascii_strncasecmp (line, "content-transfer-encoding:", 26) == 0)
    {
      const char *encoding = strchr(line, ':') + 1;
      while (dsk_ascii_isspace (*encoding))
        encoding++;

      if (decoder->transfer_decoder != NULL)
        {
          dsk_object_unref (decoder->transfer_decoder);
          decoder->transfer_decoder = NULL;
        }

      /* parse various transfer-encodings */
      if (dsk_ascii_strncasecmp (encoding, "identity", 8) == 0)
        decoder->transfer_decoder = dsk_octet_filter_identity_new ();
      else if (dsk_ascii_strncasecmp (encoding, "base64", 6) == 0)
        decoder->transfer_decoder = dsk_base64_decoder_new ();
      else if (dsk_ascii_strncasecmp (encoding, "quoted-printable", 16) == 0)
        decoder->transfer_decoder = dsk_unquote_printable_new ();
      else
        {
          dsk_set_error (error,
                         "unknown transfer encoding '%s' making decoder stream",
                         encoding);
          return DSK_FALSE;
        }
    }
  else if (dsk_ascii_strncasecmp (line, "content-description:", 20) == 0)
    {
      const char *start = strchr(line, ':') + 1;
      char *description;
      while (dsk_ascii_isspace (*start))
        start++;
      description = dsk_mem_pool_strdup (&decoder->header_storage, start);
      dsk_ascii_strchomp (description);
      cur->content_description = description;
    }
  else if (dsk_ascii_strncasecmp (line, "content-disposition:", 20) == 0)
    {
      DskMimeContentDisposition dispos;
      line += 20;
      if (!dsk_parse_mime_content_disposition_header (line, &dispos, error))
        return DSK_FALSE;
      if (dispos.name_start)
        cur->key = dsk_mem_pool_strcut (&decoder->header_storage,
                                        dispos.name_start,
                                        dispos.name_end);
      /* TODO: handle filename */
    }
  else
    {
      dsk_set_error (error,
                     "could not parse multipart_decoder message line: '%s'",
                     line);
      return DSK_FALSE;
    }
  return DSK_TRUE;
}

static dsk_boolean
done_with_header (DskMimeMultipartDecoder *decoder,
                  DskError               **error)
{
  DSK_UNUSED (error);
  if (decoder->transfer_decoder == NULL)
    decoder->transfer_decoder = dsk_octet_filter_identity_new ();
  return DSK_TRUE;
}

static dsk_boolean
done_with_content_body (DskMimeMultipartDecoder *decoder,
                        DskError               **error)
{
  unsigned str_length;
  char *at, *out;
  DskCgiVariable cur;
  if (!dsk_octet_filter_finish (decoder->transfer_decoder,
                                &decoder->cur_buffer,
                                error))
    {
      dsk_add_error_prefix (error, "in mime-multipart decoding");
      return DSK_FALSE;
    }

  /* consolidate string pieces into a single allocation */
  str_length = 0;
#define MAYBE_ADD_FIELD_LENGTH(field) \
  do { if (decoder->cur_piece.field) \
         str_length += strlen (decoder->cur_piece.field) + 1; } while (0)
  MAYBE_ADD_FIELD_LENGTH (key);
  MAYBE_ADD_FIELD_LENGTH (content_type);
  MAYBE_ADD_FIELD_LENGTH (content_location);
  MAYBE_ADD_FIELD_LENGTH (content_description);
#undef MAYBE_ADD_FIELD_LENGTH
  str_length += decoder->cur_buffer.size + 1;
  at = dsk_malloc (str_length);
  out = at;
#define MAYBE_SETUP(field)                                          \
  do { if (decoder->cur_piece.field)                                \
         {                                                          \
           cur.field = out;                                         \
           out = dsk_stpcpy (out, decoder->cur_piece.field) + 1;    \
         }                                                          \
       else                                                         \
         cur.field = NULL;             } while(0)
  cur.is_get = DSK_FALSE;
  MAYBE_SETUP (key);
  cur.value = out;
  cur.value_length = decoder->cur_buffer.size;
  out += decoder->cur_buffer.size;
  *out++ = '\0';
  dsk_buffer_read (&decoder->cur_buffer, decoder->cur_buffer.size, cur.value);
  MAYBE_SETUP (content_type);
  MAYBE_SETUP (content_location);
  MAYBE_SETUP (content_description);
#undef MAYBE_ADD_FIELD_LENGTH

  /* append CGI to queue */
  if (decoder->n_pieces == decoder->pieces_alloced)
    {
      if (decoder->pieces_alloced == DSK_MIME_MULTIPART_DECODER_N_INIT_PIECES)
        {
          decoder->pieces = DSK_NEW_ARRAY (DskCgiVariable, decoder->pieces_alloced * 2);
          memcpy (decoder->pieces, decoder->pieces_init,
                  DSK_MIME_MULTIPART_DECODER_N_INIT_PIECES * sizeof (DskCgiVariable));
        }
      else
        decoder->pieces = DSK_RENEW (DskCgiVariable, decoder->pieces,
                                     decoder->pieces_alloced * 2);
      decoder->pieces_alloced *= 2;
    }
  decoder->pieces[decoder->n_pieces++] = cur;

  /* reset the header storage area */
  dsk_mem_pool_clear (&decoder->header_storage);
  dsk_mem_pool_init_buf (&decoder->header_storage,
                         DSK_MIME_MULTIPART_DECODER_INIT_HEADER_SPACE,
                         decoder->header_pad);
  return DSK_TRUE;
}


/* Process data from the incoming buffer into decoded buffer,
   scanning for lines starting with "--boundary",
   where "boundary" is replaced with multipart_decoder->boundary.
   
   If we reach the end of this piece, we must enqueue it.
   After that, we may be ready to start a new piece (and we try to do so),
   or we may done (depending on whether the boundary-mark
   ends with an extra "--").
 */
dsk_boolean
dsk_mime_multipart_decoder_feed  (DskMimeMultipartDecoder *decoder,
                                  size_t                   length,
                                  const uint8_t           *data,
                                  size_t                  *n_parts_ready_out,
                                  DskError               **error)
{
  char tmp_buf[256];
  dsk_buffer_append (&decoder->incoming, length, data);
  switch (decoder->state)
    {
    state__WAITING_FOR_FIRST_SEP_LINE:
    case STATE_WAITING_FOR_FIRST_SEP_LINE:
      {
        int nl = dsk_buffer_index_of (&decoder->incoming, '\n');
        const char *after;
        if (nl < 0)
          goto return_true;

        /* if the line is too long for boundary_buf,
           then it cannot be a boundary, and we just skip it. */
        if ((unsigned)nl > decoder->boundary_str_len + 5
         || (unsigned)nl < decoder->boundary_str_len + 2)
          {
            dsk_buffer_discard (&decoder->incoming, nl + 1);
            goto state__WAITING_FOR_FIRST_SEP_LINE;
          }

        dsk_buffer_read (&decoder->incoming, nl + 1, decoder->boundary_buf);
        if (decoder->boundary_buf[0] != '-'
         || decoder->boundary_buf[1] != '-'
         || memcmp (decoder->boundary_buf + 2, decoder->boundary_str,
                    decoder->boundary_str_len) != 0)
          {
            /* not a boundary line */
            goto state__WAITING_FOR_FIRST_SEP_LINE;
          }
        after = decoder->boundary_buf + decoder->boundary_str_len + 2;
        if (dsk_ascii_isspace (after[0]))
          {
            decoder->state = STATE_READING_HEADER;
            goto state__READING_HEADER;
          }
        else if (after[1] == '-')
          {
            decoder->state = STATE_ENDED;
            /* TODO: complain about garbage after terminator? */
            goto return_true;
          }
        else
          {
            dsk_buffer_discard (&decoder->incoming, nl + 1);
            goto state__WAITING_FOR_FIRST_SEP_LINE;
          }


      }

    state__READING_HEADER:
    case STATE_READING_HEADER:
      {
        int nl = dsk_buffer_index_of (&decoder->incoming, '\n');
        char *line_buf;
        if (nl < 0)
          goto return_true;
        line_buf = ((unsigned)nl + 1 > sizeof (tmp_buf))
                 ? (char*)dsk_malloc (nl+1)
                 : tmp_buf;
        dsk_buffer_read (&decoder->incoming, nl + 1, line_buf);

        /* cut-off newline (and possible "CR" character) */
        if (nl > 0 && line_buf[nl-1] == '\r')
          nl--;
        line_buf[nl] = '\0';    /* chomp */

        /* If the line is empty (or it be corrupt and start with a NUL??? (FIXME)),
           it terminates the header */
        if (line_buf[0] == '\0')
          {
            dsk_boolean rv = done_with_header (decoder, error);
            if (line_buf != tmp_buf)
              dsk_free (line_buf);
            if (!rv)
              return DSK_FALSE;
            decoder->state = STATE_CONTENT_LINE_START;
            goto state__CONTENT;
          }
        else
          {
            dsk_boolean rv = process_header_line (decoder, line_buf, error);
            if (line_buf != tmp_buf)
              dsk_free (line_buf);
            if (!rv)
              return DSK_FALSE;
            goto state__READING_HEADER;
          }
      }
      break;
    state__CONTENT:
    case STATE_CONTENT_MIDLINE:
    case STATE_CONTENT_LINE_START:
    case STATE_CONTENT_LINE_START_CRLF:
      {
        int nl = dsk_buffer_index_of (&decoder->incoming, '\n');
        unsigned amount;
        dsk_boolean crlf_pending = DSK_FALSE;
        if (decoder->state != STATE_CONTENT_MIDLINE)
          {
            if (nl < 0
             && decoder->incoming.size < decoder->boundary_str_len + 6)
              goto return_true;
            else if (nl < 0)
              {
                /* cannot be boundary: fall though */
              }
            else if (nl < (int)decoder->boundary_str_len + 2
                  || nl > (int)decoder->boundary_str_len + 5)
              {
                /* cannot be boundary: fall though */
              }
            else
              {
                int got = dsk_buffer_peek (&decoder->incoming,
                                           2 + decoder->boundary_str_len + 2 + 2,
                                           decoder->boundary_buf);
                if (decoder->boundary_buf[0] == '-'
                 && decoder->boundary_buf[1] == '-'
                 && memcmp (decoder->boundary_buf + 2, decoder->boundary_str,
                            decoder->boundary_str_len) == 0)
                  {
                    int rem = got - decoder->boundary_str_len - 2;
                    const char *at = decoder->boundary_buf + decoder->boundary_str_len + 2;
                    const char *nl = memchr (at, '\n', rem);
                    unsigned discard;
                    if (nl == NULL)
                      goto return_true;
                    discard = nl + 1 - decoder->boundary_buf;
                    if (at[0] == '-' && at[1] == '-')
                      {
                        dsk_buffer_discard (&decoder->incoming, discard);

                        if (!done_with_content_body (decoder, error))
                          return DSK_FALSE;

                        decoder->state = STATE_ENDED;

                        /* TODO: complain about garbage after terminator? */

                        goto return_true;
                      }
                    else if (dsk_ascii_isspace (*at))
                      {
                        /* TODO: more precise check for spaces up to 'nl'? */
                        dsk_buffer_discard (&decoder->incoming, discard);

                        if (!done_with_content_body (decoder, error))
                          return DSK_FALSE;

                        decoder->state = STATE_READING_HEADER;
                        goto state__READING_HEADER;
                      }
                    else
                      {
                        /* cannot be boundary: fall though */
                      }
                  }
              }
          }
        if (decoder->state == STATE_CONTENT_LINE_START_CRLF)
          {
            if (!dsk_octet_filter_process (decoder->transfer_decoder,
                                           &decoder->cur_buffer,
                                           2, (uint8_t*) "\r\n",
                                           error))
              return DSK_FALSE;
          }
        if (nl < 0)
          {
            amount = decoder->incoming.size;
            if (amount > 0
              && dsk_buffer_get_last_byte (&decoder->incoming) == '\r')
              amount--;
          }
        else
          {
            if (nl > 0
             && dsk_buffer_get_byte_at (&decoder->incoming, nl - 1) == '\r')
              {
                crlf_pending = DSK_TRUE;
                amount = nl - 1;
              }
            else
              amount = nl + 1;
          }
        if (!dsk_octet_filter_process_buffer (decoder->transfer_decoder,
                                              &decoder->cur_buffer,
                                              amount,
                                              &decoder->incoming,
                                              DSK_TRUE,  /* discard from incoming */
                                              error))
          {
            dsk_add_error_prefix (error, "in mime-multipart decoding");
            return DSK_FALSE;
          }
        if (nl < 0)
          {
            /* fed all data we can into decoder */
            decoder->state = STATE_CONTENT_MIDLINE;
            goto return_true;
          }
        if (crlf_pending)
          {
            dsk_buffer_discard (&decoder->incoming, 2);
            decoder->state = STATE_CONTENT_LINE_START_CRLF;
          }
        else
          decoder->state = STATE_CONTENT_LINE_START;
        goto state__CONTENT;
      }
    case STATE_ENDED:
      dsk_set_error (error, "got data after final terminator");
      return DSK_FALSE;
    }

return_true:
  if (n_parts_ready_out)
    *n_parts_ready_out = decoder->n_pieces;
  return DSK_TRUE;
}
dsk_boolean
dsk_mime_multipart_decoder_done  (DskMimeMultipartDecoder *decoder,
                                  size_t                  *n_parts_ready_out,
                                  DskError               **error)
{
  if (decoder->state != STATE_ENDED)
    {
      dsk_set_error (error, "MIME multipart message ended without terminal boundary");
      return DSK_FALSE;
    }
  if (n_parts_ready_out)
    *n_parts_ready_out = decoder->n_pieces;
  return DSK_TRUE;
}

void
dsk_mime_multipart_decoder_dequeue_all (DskMimeMultipartDecoder *decoder,
                                        DskCgiVariable          *out)
{
  memcpy (out, decoder->pieces, sizeof (DskCgiVariable) * decoder->n_pieces);
  decoder->n_pieces = 0;
}


static void
dsk_mime_multipart_decoder_init (DskMimeMultipartDecoder *decoder)
{
  decoder->header_pad = dsk_malloc (DSK_MIME_MULTIPART_DECODER_INIT_HEADER_SPACE);
  dsk_mem_pool_init_buf (&decoder->header_storage,
                         DSK_MIME_MULTIPART_DECODER_INIT_HEADER_SPACE,
                         decoder->header_pad);
  decoder->pieces = decoder->pieces_init;
  decoder->pieces_alloced = DSK_MIME_MULTIPART_DECODER_N_INIT_PIECES;
}

static void
dsk_mime_multipart_decoder_finalize (DskMimeMultipartDecoder *decoder)
{
  unsigned i;
  dsk_mem_pool_clear (&decoder->header_storage);
  dsk_free (decoder->start);
  dsk_free (decoder->start_info);
  dsk_free (decoder->boundary_str);
  if (decoder->boundary_buf != decoder->boundary_buf_init)
    dsk_free (decoder->boundary_buf);
  dsk_buffer_clear (&decoder->cur_buffer);
  if (decoder->transfer_decoder)
    dsk_object_unref (decoder->transfer_decoder);
  dsk_free (decoder->header_pad);
  for (i = 0; i < decoder->n_pieces; i++)
    dsk_cgi_variable_clear (&decoder->pieces[i]);
  if (decoder->pieces != decoder->pieces_init)
    dsk_free (decoder->pieces);
}

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskMimeMultipartDecoder);
static DskMimeMultipartDecoderClass dsk_mime_multipart_decoder_class =
{
  DSK_OBJECT_CLASS_DEFINE (DskMimeMultipartDecoder,
                           &dsk_object_class,
                           dsk_mime_multipart_decoder_init,
                           dsk_mime_multipart_decoder_finalize)
};

DskMimeMultipartDecoder *
dsk_mime_multipart_decoder_new (char **kv_pairs, DskError **error)
{
  DskMimeMultipartDecoder *decoder;
  unsigned i;
  DSK_UNUSED (error);
  decoder = dsk_object_new (&dsk_mime_multipart_decoder_class);
  for (i = 0; kv_pairs[2*i] != NULL; i++)
    {
      const char *key = kv_pairs[2*i+0];
      const char *value = kv_pairs[2*i+1];
      if (dsk_ascii_strcasecmp (key, "start") == 0)
	{
	  dsk_free (decoder->start);
	  decoder->start = dsk_strdup (value);
	}
      else if (dsk_ascii_strcasecmp (key, "start-info") == 0)
	{
	  dsk_free (decoder->start_info);
	  decoder->start_info = dsk_strdup (value);
	}
      else if (dsk_ascii_strcasecmp (key, "boundary") == 0)
	{
	  dsk_free (decoder->boundary_str);
	  decoder->boundary_str = dsk_strdup (value);
	  decoder->boundary_str_len = strlen (decoder->boundary_str);
	}
      else
	{
          /* is an error appropriate here? */
	  dsk_warning ("constructing mime-multipart-decoder: ignoring Key %s", key);
	}
    }
  if (decoder->boundary_str_len + 6 < BOUNDARY_BUF_PAD_SIZE)
    decoder->boundary_buf = decoder->boundary_buf_init;
  else
    decoder->boundary_buf = dsk_malloc (decoder->boundary_str_len + 6);
  return decoder;
}
