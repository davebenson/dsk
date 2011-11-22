#include "dsk.h"

typedef enum
{
  SPDY_STATUS__PROTOCOL_ERROR = 1,
  SPDY_STATUS__INVALID_STREAM = 2,
  SPDY_STATUS__REFUSED_STREAM = 3,
  SPDY_STATUS__UNSUPPORTED_VERSION = 4,
  SPDY_STATUS__CANCEL = 5,
  SPDY_STATUS__INTERNAL_ERROR = 6,
  SPDY_STATUS__FLOW_CONTROL_ERROR = 7
} SpdyStatusCode;

/* NOTE: leaves session->incoming untouched. */
/* NOTE: caller must typically handle errors by doing a stream reset. */
static DskSpdyHeaders *
parse_name_value_block (DskSpdySession *session,
                        unsigned        n_bytes,
                        DskError      **error)
{
  DskBuffer decompressed = DSK_BUFFER_STATIC_INIT;
  DskError *e = NULL;
  if (!dsk_octet_filter_process_buffer (session->decompressor, &decompressed,
                                        n_bytes, &session->incoming, DSK_FALSE,
                                        &e)
   || !dsk_zlib_decompressor_sync (session->decompressor, &decompressed,
                                   &e))
    {
      dsk_set_error (error, "error decompressing name/value block: %s",
                     e->message);
      dsk_error_unref (e);
      return NULL;
    }

  if (decompressed.size < 2)
    {
      dsk_set_error (error, "decompressed data much too short");
      return NULL;
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
      dsk_set_error (error, "decompressed data too short for %u name/values",
                     n_kv);
      return NULL;
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

static inline void
session_ensure_has_write_trap (DskSpdySession *session)
{
  if (session->write_trap != NULL)
    return;
  session->write_trap = dsk_hook_trap (&session->sink->writable_hook,
                                       handle_sink_writable,
                                       session,
                                       NULL);
}

struct _DskSpdyMessage
{
  /* For data, settings, header messages which are not
     reodered in the stream. */
  DskSpdyStream *owner;
  DskSpdyMessage *next_in_stream, *prev_in_stream;

  unsigned length;
  /* The data follows Message. */
};

static DskSpdyMessage *
allocate_message (DskSpdySession *session,
                  DskSpdyStream  *stream,
                  unsigned        length)
{
  unsigned pri = stream ? stream->priority : 4;
  DskSpdyMessage *rv = dsk_malloc (sizeof (DskSpdyMessage) + length);
  rv->owner = stream;
  GSK_LIST_APPEND (GET_MESSAGE_LIST (session, pri), rv);
  rv->length = length;
  return rv;
}

static void
send_reset_stream_message (DskSpdySession *session,
                           uint32_t        stream_id,
                           SpdyStatusCode  status_code)
{
  uint8_t buf[16];
  buf[0] = 0x80;
  buf[1] = 2;
  buf[2] = SPDY_TYPE__RST_STREAM >> 8;
  buf[3] = SPDY_TYPE__RST_STREAM;
  buf[4] = 0;               /* no flags for RST_STREAM */
  buf[5] = 0;               /* buf[5..7] is the payload length */
  buf[6] = 0;
  buf[7] = 8;
  dsk_uint32be_pack (stream_id, buf + 8);
  dsk_uint32be_pack (status_code, buf + 12);

  DskSpdyMessage *message = allocate_message (session, NULL, 16);
  memcpy (message + 1, buf, 16);
  session_ensure_has_write_trap (session);
}


typedef struct _Info_SYN_STREAM Info_SYN_STREAM;
struct _Info_SYN_STREAM
{
  uint32_t stream_id;
  uint32_t assoc_to_stream_id;
  uint8_t priority;
  uint8_t flags;
  uint32_t header_length;
};

/* Should only respond FALSE if there is a framing layer error
   that cannot be recovered from. */
static dsk_boolean
handle_SYN_STREAM (DskSpdySession *session,
                   uint32_t        stream_id,
                   uint32_t        assoc_to_stream_id,
                   uint8_t         priority,
                   uint8_t         flags,
                   uint8_t         header_length,
                   DskError      **error)
{
  DskError *e = NULL;
  DskSpdyHeaders *headers;
  DskSpdyStream *stream;

  /* Duplicate stream_id is considered a recoverable error. */
  stream = dsk_spdy_session_lookup_stream (session, stream_id);
  if (stream != NULL)
    {
      /* destroy stream, RST_STREAM */
      send_reset_stream_message (session, stream_id);
      return DSK_TRUE;
    }

  /* Failure parsing the name/value block is considered a
     recoverably error (for some unknown reason, really). */
  headers = parse_name_value_block (session, length - 10, &e);
  dsk_buffer_discard (&session->incoming, length - 10);
  if (headers == NULL)
    {
      if (session->print_errors)
        dsk_warning ("SPDY: bad header for new stream: %s",
                     e->message);
      dsk_error_unref (e);
      send_reset_stream_message (stream_id);
      return DSK_TRUE;
    }
  if (DSK_SPDY_SESSION_IS_CLIENT (session) == (stream_id & 1))
    {
      /* client originated even stream,
         or server originated odd stream:
         the spec says this is an error, but 
         doesn't seem to say how to handle it.
         We disband the session. */
      dsk_boolean peer_client = !DSK_SPDY_SESSION_IS_CLIENT (session);
      dsk_set_error (error,
                 "SPDY: %s sent message with id %u (should be %s)",
                     peer_client ? "client" : "server",
                     stream_id,
                     peer_client ? "odd" : "even");
      return DSK_FALSE;
    }
  stream = dsk_object_new (&dsk_spdy_stream_class);
  if (flags & SPDY_FLAG_UNIDIRECTIONAL)
    stream->unidirectional = 1;
  if (!stream->unidirectional)
    {
      stream->source = dsk_object_new (&dsk_spdy_source_class);
    }
  if (!(flags&SPDY_FLAG_FIN))
    {
      stream->sink = dsk_object_new (&dsk_spdy_sink_class);
    }

}

/* Should only respond FALSE if there is a framing layer error
   that cannot be recovered from. */
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
                SynStreamInfo info;
                dsk_buffer_read (&session->incoming, 10, subhdr);
                if (!parse_SYN_STREAM (subhdr, length, &info, error))
                  return DSK_FALSE;
                if (!handle_SYN_STREAM (session, &info, error))
                  return DSK_FALSE;
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

packet_too_short:
  dsk_set_error (error, "SPDY session: packet too short for message type: terminating");
  return DSK_FALSE;
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
  DskSpdyStream *stream = dsk_spdy_session_lookup_stream (stream, stream_id);
  if (stream == NULL)
    return NULL;
  if (stream->state != DSK_SPDY_STREAM_STATE_PENDING)
    {
      ...
    }
  GSK_LIST_REMOVE (GET_PENDING_STREAM_LIST (session), stream);
  stream->state = DSK_SPDY_STREAM_STATE_OK;
  /* There's no need to emit a stream-state-change notification,
     because no end user could have accessed this stream yet. */
  if (!stream->unidirectional)
    {
      /* send SYN_REPLY */
      ...
    }
  else
    {
      if (reply_headers && reply_headers->n_kv)
        dsk_warning ("reply_headers specified for unidirectional stream");
    }
  stream->state = DSK_SPDY_STREAM_STATE_OK;
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
