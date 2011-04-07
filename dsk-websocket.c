#include "dsk.h"

#define MAX_PENDING     32

static uint8_t close_packet[9] = { 0xff, 0,0,0,0, 0,0,0,0 };

struct _DskWebsocketPacket
{
  unsigned length;
  uint8_t *data;
};

static void
dsk_websocket_init (DskWebsocket *websocket)
{
  dsk_hook_init (&websocket->readable, websocket);
  dsk_hook_init (&websocket->error_hook, websocket);
  websocket->max_pending = MAX_PENDING;
  websocket->pending = dsk_malloc (sizeof (DskWebsocketPacket) * MAX_PENDING);
}

static void
dsk_websocket_finalize (DskWebsocket *websocket)
{
  unsigned i, at;
  if (websocket->read_trap)
    dsk_hook_trap_destroy (websocket->read_trap);
  if (websocket->write_trap)
    dsk_hook_trap_destroy (websocket->write_trap);
  if (websocket->source)
    dsk_object_unref (websocket->source);
  if (websocket->sink)
    dsk_object_unref (websocket->sink);
  dsk_buffer_clear (&websocket->incoming);
  dsk_buffer_clear (&websocket->outgoing);
  dsk_hook_clear (&websocket->readable);
  dsk_hook_clear (&websocket->error_hook);
  at = websocket->first_pending;
  for (i = 0; i < websocket->n_pending; i++)
    {
      dsk_free (websocket->pending[at].data);
      if (++at == websocket->max_pending)
        at = 0;
    }
}

typedef DskObjectClass DskWebsocketClass;

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskWebsocket);
DskObjectClass dsk_websocket_class =
DSK_OBJECT_CLASS_DEFINE(DskWebsocket, &dsk_object_class,
                        dsk_websocket_init, dsk_websocket_finalize);

/* read-hook function */
static void ensure_has_read_trap (DskWebsocket *websocket);
static void ensure_has_write_trap (DskWebsocket *websocket);
static void
do_deferred_shutdown (DskWebsocket *websocket)
{
  websocket->is_closed = DSK_TRUE;
  if (websocket->outgoing.size == 0)
    do_shutdown (websocket);
  else
    websocket->deferred_shutdown = DSK_TRUE;
}
static void
maybe_parse_incoming_packets(DskWebsocket *websocket)
{
retry_discard:
  if (websocket->to_discard > 0)
    {
      if (websocket->to_discard > websocket->incoming.size)
        {
          websocket->to_discard -= websocket->incoming.size;
          dsk_buffer_reset (&websocket->incoming);
          ensure_has_read_trap (websocket);
          return;
        }
      dsk_buffer_discard (&websocket->incoming, websocket->to_discard);
      websocket->to_discard = 0;
    }
  while (websocket->incoming.size >= 9
      && websocket->n_pending < websocket->max_pending
      && !websocket->is_closed)
    {
      uint8_t hdr[9];
      uint64_t length;
      dsk_buffer_peek (&websocket->incoming, 9, hdr);
      length = dsk_uint64be_parse (hdr + 1);
      if (hdr[0] == 0xff)
        {
          /* uh oh - shutdown packet */
          dsk_buffer_discard (&websocket->incoming, 9 + length);
          do_deferred_shutdown (websocket);
        }
      else if (hdr[0] == 0x00)
        {
          /* normal data packet */
          if (length > websocket->max_packet_length)
            {
              ...
            }ZZ
          ...
        }
      else
        {
          /* error */
          switch (websocket->unknown_packet_type_mode)
            {
            case DSK_WEBSOCKET_MODE_CLOSE:
              set_error (websocket, dsk_error_new ("bad packet type 0x%02x", hdr[0]));
              do_deferred_shutdown (websocket);
              break;
            case DSK_WEBSOCKET_MODE_DROP:
              websocket->to_discard = length + 9;
              goto retry_discard;
            }
        }
    }
}

dsk_boolean
dsk_websocket_retrieve (DskWebsocket *websocket,
                        unsigned     *length_out,
                        uint8_t     **data_out)
{
  if (websocket->n_pending == 0)
    return DSK_FALSE;
  *length_out = websocket->pending[websocket->first_pending].length;
  *data_out = websocket->pending[websocket->first_pending].data;
  websocket->first_pending += 1;
  if (websocket->first_pending == MAX_PENDING)
    websocket->first_pending = 0;
  websocket->n_pending -= 1;

  if (websocket->n_pending < websocket->max_pending
   && websocket->incoming.size >= 9)
    maybe_parse_incoming_packet (websocket);

  if (websocket->n_pending < websocket->max_pending
   && websocket->read_trap == NULL
   && websocket->source != NULL)
    ensure_has_read_trap (websocket);

  return DSK_TRUE;
}

void
dsk_websocket_send     (DskWebsocket *websocket,
                        unsigned      length,
                        uint8_t      *data)
{
  uint8_t header[9] = { 0xff,
                        0, 0, 0, 0,
                        length>>24, length>>16, length>>8, length };
  dsk_buffer_append_small (&websocket->outgoing, 9, header);
  dsk_buffer_append (&websocket->outgoing, length, data);
  return DSK_TRUE;
}


void
dsk_websocket_shutdown (DskWebsocket *websocket)
{
  if (!websocket->is_closed)
    {
      dsk_buffer_append (&websocket->outgoing, sizeof (close_packet), close_packet);
      ensure_has_write_trap (websocket);
      do_deferred_shutdown (websocket);
    }
}

static dsk_boolean
handle_sink_writable (DskOctetSink *sink,
                      DskWebsocket *websocket)
{
  switch (dsk_octet_sink_write_buffer (sink, &websocket->outgoing, &error))
    {
    case DSK_IO_RESULT_SUCCESS:
      ...
    case DSK_IO_RESULT_AGAIN:
      ...
    case DSK_IO_RESULT_EOF:
      ...
    case DSK_IO_RESULT_ERROR:
      ...
    }

  if (websocket->outgoing.size == 0)
    {
      if (websocket->deferred_shutdown)
        {
          websocket->write_trap = NULL;
          do_shutdown (websocket);
          return DSK_FALSE;
        }
      else
        {
          websocket->write_trap = NULL;
          return DSK_FALSE;
        }
    }
  else
    return DSK_TRUE;
}

static dsk_boolean
handle_source_readable (DskOctetSource *source,
                        DskWebsocket   *websocket)
{
  switch (dsk_octet_source_read_buffer (source, &websocket->incoming, &error))
    {
    case DSK_IO_RESULT_SUCCESS:
      if (websocket->n_pending == websocket->max_pending)
        {
          websocket->read_trap = NULL;
          return DSK_FALSE;
        }
      /* see if we have a new message */
      ...
      break;
    case DSK_IO_RESULT_AGAIN:
      break;
    case DSK_IO_RESULT_ERROR:
      ...
    case DSK_IO_RESULT_EOF:
      websocket->read_trap = NULL;
      dsk_object_unref (websocket->source);
      websocket->source = NULL;
      return DSK_FALSE;
    default:
      dsk_assert_not_reached ();
    }
  return DSK_TRUE;
}

static void
ensure_has_write_trap (DskWebsocket *websocket)
{
  if (websocket->sink == NULL || websocket->write_trap != NULL)
    return;
  websocket->write_trap = dsk_hook_trap (&websocket->sink->writable,
                                        (DskHookFunc) handle_sink_writable,
                                        websocket, NULL);
}

static void
ensure_has_read_trap (DskWebsocket *websocket)
{
  if (websocket->source == NULL || websocket->read_trap != NULL)
    return;
  websocket->read_trap = dsk_hook_trap (&websocket->source->readable,
                                        (DskHookFunc) handle_source_readable,
                                        websocket, NULL);
}

void
_dsk_websocket_server_init (DskWebsocket *websocket,
                            DskOctetSource *source,
                            DskOctetSink *sink)
{
  websocket->source = source;
  websocket->sink = sink;
  if (websocket->outgoing.size > 0)
    ensure_has_write_trap (websocket);
  ensure_has_read_trap (websocket);
}

