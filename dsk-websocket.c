#include "dsk.h"

static void
dsk_websocket_init (DskWebsocket *websocket)
{
  dsk_hook_init (&websocket->readable, websocket);
  dsk_hook_init (&websocket->error_hook, websocket);
}

static void
dsk_websocket_finalize (DskWebsocket *websocket)
{
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
}

typedef DskObjectClass DskWebsocketClass;

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskWebsocket);
DskObjectClass dsk_websocket_class =
DSK_OBJECT_CLASS_DEFINE(DskWebsocket, &dsk_object_class,
                        dsk_websocket_init, dsk_websocket_finalize);

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

  if (websocket->n_pending == 0 && websocket->incoming.size >= 9)
    maybe_parse_incoming_packet (websocket);
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
  ...
}

static dsk_boolean
handle_sink_writable (DskOctetSink *sink,
                      DskWebsocket *websocket)
{
  ...
}

static dsk_boolean
handle_source_readable (DskOctetSource *source,
                        DskWebsocket   *websocket)
{
  ...
}

void
_dsk_websocket_server_init (DskWebsocket *websocket,
                            DskOctetSource *source,
                            DskOctetSink *sink)
{
  websocket->source = source;
  websocket->sink = sink;
  if (websocket->outgoing.size > 0)
    websocket->write_trap = dsk_hook_trap (&sink->writable,
                                           (DskHookFunc) handle_sink_writable,
                                           websocket, NULL);
  websocket->read_trap = dsk_hook_trap (&sink->readable,
                                        (DskHookFunc) handle_source_readable,
                                        websocket, NULL);
}

