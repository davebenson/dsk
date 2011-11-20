#include "dsk.h"

/* NOTE: leaves session->incoming untouched. */
static DskSpdyHeaders *
parse_name_value_block (DskSpdySession *session,
                        unsigned        n_bytes,
                        DskError      **error)
{
  DskBuffer decompressed = DSK_BUFFER_STATIC_INIT;
  if (!dsk_octet_filter_process_buffer (session->decompressor, &decompressed,
                                        n_bytes, &session->incoming, DSK_FALSE,
                                        error)
   || !dsk_zlib_decompressor_sync (session->decompressor, &decompressed,
                                   error))
    return NULL;

  if (decompressed.size < 2)
    {
      ...
    }

  uint8_t tmp[2];
  uint16_t n_kv;
  dsk_buffer_read (&decompressed, 2, tmp);
  n_kv = dsk_uint16be_parse (tmp);

  /* "key" and "value" must have length at least one,
     plus 2 bytes for the key's length, and 2 bytes for the value's
     length gives a minumum of 6 bytes per key/value pair. */
  if (decompressed.size < (4+2) * n_kv)
    {
      ...
    }

  alloc_size = (decompressed.size - 4*n_kv)
             + sizeof (DskSpdyHeaders)
             + sizeof (DskSpdyKeyValue) * n_kv
             + 2 * n_kv         /* NULs */
             ;
  DskSpdyHeaders *headers = dsk_malloc (alloc_size);
  headers->n_pairs = n_kv;
  headers->pairs = (DskSpdyKeyValue*)(headers+1);
  uint8_t *slab = (uint8_t*)(headers->pairs + n_kv);
  for (i = 0; i < n_kv; i++)
    {
      dsk_buffer_read (&decompressed, 2, tmp);
      uint16_t key_length = dsk_uint16be_parse (tmp);
      headers->pairs[i].key = (char*) slab;
      if (dsk_buffer_read (&decompressed, key_length, slab) != key_length)
        {
          dsk_set_error (error, "key exceeded buffer's length");
          dsk_free (headers);
          return NULL;
        }
      slab += key_length;
      *slab++ = 0;

      dsk_buffer_read (&decompressed, 2, tmp);
      uint16_t value_length = dsk_uint16be_parse (tmp);
      headers->pairs[i].value_length = value_length;
      headers->pairs[i].value = (char*) slab;
      if (dsk_buffer_read (&decompressed, value_length, slab) != value_length)
        {
          dsk_set_error (error, "value exceeded buffer's length");
          dsk_free (headers);
          return NULL;
        }
      slab += value_length;
      *slab++ = 0;
    }
  if (decompressed.size != 0)
    {
      dsk_set_error (error, "trailing garbage after header block's data");
      dsk_free (headers);
      return NULL;
    }
  return headers;
}

static dsk_boolean
try_processing_incoming (DskOctetSource *source,
                         DskSpdySession *session,
                         DskError      **error)
{
  dsk_assert (session->source == source);
  while (session->incoming.size >= 8)
    {
      uint8_t hdr[8];
      dsk_buffer_peek (&session->incoming, 8, hdr);
      dsk_boolean is_control = (hdr[0] & 0x80) != 0;
      uint16_t version = ((unsigned)(hdr[0] & 0x7f) << 8) + (hdr[1]);
      uint16_t type = ((unsigned)hdr[2] << 8) + hdr[3];
      uint8_t flags = hdr[4];
      uint32_t length = ((unsigned)hdr[5] << 16)
                      + ((unsigned)hdr[6] << 8)
                      + ((unsigned)hdr[7]);
      if (session->incoming.size < 8 + length)
        break;

      dsk_buffer_discard (&session->incoming, 8);

      if (is_control)
        {
          switch (type)
            {
            case SPDY_TYPE__SYN_STREAM:
              if (length < 12)
                goto packet_too_short;
              {
                uint8_t subhdr[10];
                dsk_buffer_read (&session->incoming, 10, subhdr);
                uint32_t stream_id = dsk_uint32be_parse (subhdr) & 0x7fffffff;
                uint32_t assoc_to_stream_id = dsk_uint32be_parse (subhdr + 4) & 0x7fffffff;
                uint8_t priority = subhdr[8] >> 6;
                DskSpdyHeaders *headers = parse_name_value_block (session, length - 10, error);
                dsk_buffer_discard (&session->incoming, length - 10);
                if (headers == NULL)
                  {
                    /* must respond with RST_STREAM */
                    ...

                    return DSK_FALSE;
                  }
                ...
              }
              if (DSK_SPDY_SESSION_IS_CLIENT (session) == (stream_id & 1))
                {
                  /* client originated even stream,
                     or server originated odd stream. */
                  ...
                }
              DskSpdyStream *stream = dsk_spdy_session_lookup_stream (session, stream_id);
              if (stream != NULL)
                {
                  /* destroy stream, RST_STREAM */
                  ...
                  break;
                }
              stream = dsk_object_new (&dsk_spdy_stream_class);
              if ((flags & SPDY_FLAG_UNIDIRECTIONAL) == 0)
                {
                  stream->source = dsk_object_new (&dsk_spdy_source_class);
                }
              if ((flags & SPDY_FLAG_FIN) == 0)
                {
                  stream->sink = dsk_object_new (&dsk_spdy_sink_class);
                }

              if ((flags & SPDY_FLAG_UNIDIRECTIONAL) == 0)
                {
                  /* Send SYN_REPLY */
                  ...
                }
              break;
            case SPDY_TYPE__SYN_REPLY:
              ...
              break;
            case SPDY_TYPE__RST_STREAM:
              ...
              break;
            case SPDY_TYPE__SETTINGS:
              ...
              break;
            case SPDY_TYPE__NOOP:
              ...
              break;
            case SPDY_TYPE__PING:
              ...
              break;
            case SPDY_TYPE__GOAWAY:
              ...
              break;
            case SPDY_TYPE__HEADERS:
              ...
              break;
            default:
              ...
            }
        }
      else
        {
          /* Handle data packet */
          ...
        }
    }
  return DSK_TRUE;
}

DskSpdySession *
dsk_spdy_session_new  (dsk_boolean     is_client_side,
                       DskOctetStream *stream,
                       DskOctetSource *source,
                       DskOctetSink   *sink,
                       DskBuffer      *initial_data,
                       DskError      **error)
{
  DskSpdySession *session = dsk_object_new (&dsk_spdy_session_class);
  session->next_stream_id = session->next_ping_id = (is_client_side ? 1 : 2);
  session->stream = dsk_object_ref (stream);
  session->source = dsk_object_ref (source);
  session->sink = dsk_object_ref (sink);
  if (initial_data)
    dsk_buffer_drain (&session->incoming, initial_data);
  session->read_trap = dsk_hook_trap (&source->readable,
                                      (DskHookFunc) handle_connection_readable,
                                      session,
                                      NULL);
  if (session->incoming.size > 0)
    {
      if (!try_processing_incoming (session, error))
        {
          dsk_object_unref (session);
          return NULL;
        }
    }
  return session;
}

DskSpdySession *
dsk_spdy_session_new_client_tcp       (const char     *name,
                                       DskIpAddress   *address,
                                       unsigned        port,
                                       DskError      **error)
{
  DskClientStream *stream;
  ...
}

/* Returns the number of stream requests from the other side,
   which may be larger than max_return. */
unsigned
dsk_spdy_session_get_pending_stream_ids(DskSpdySession *session,
                                        unsigned        max_return,
                                        uint32_t       *stream_ids_out)
{
  unsigned n = DSK_MIN (max_return, session->n_pending_streams);
  DskSpdyStream *stream = session->first_pending_stream;
  for (i = 0; i < n; i++)
    {
      *stream_ids_out++ = stream->id;
      stream = stream->next_pending;
    }
  return session->n_pending_streams;
}

/* Returns a reference that you must unref */
DskSpdyStream  *
dsk_spdy_session_get_stream           (DskSpdySession *session,
                                       uint32_t        stream_id,
                                       DskSpdyHeaders *reply_headers)
{
  ...
}

DskSpdyHeaders *
dsk_spdy_session_peek_stream_headers  (DskSpdySession *session,
                                       uint32_t        stream_id)
{
  ...
}


DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskSpdyStream);
const DskSpdyStreamClass dsk_spdy_stream_class = { 
  DSK_OBJECT_CLASS_DEFINE(DskSpdyStream, &dsk_object_class,
                          dsk_spdy_stream_init, dsk_spdy_stream_finalize)
};

DskSpdyStream *dsk_spdy_session_make_stream           (DskSpdySession *session,
						       DskSpdyHeaders *request_headers,
                                                       DskSpdyResponseFunc func,
                                                       void           *func_data);

void           dsk_spdy_stream_cancel                 (DskSpdyStream  *stream);

/* --- low-level HTTP support --- */
DskSpdyHeaders *dsk_spdy_headers_from_http_request    (DskHttpRequest *request);
DskSpdyHeaders *dsk_spdy_headers_from_http_response   (DskHttpResponse*response);
DskHttpRequest *dsk_spdy_headers_to_http_request      (DskSpdyHeaders *request,
                                                       DskError      **error);
DskHttpResponse*dsk_spdy_headers_to_http_response     (DskSpdyHeaders *response,
                                                       DskError      **error);
