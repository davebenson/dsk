#include "dsk.h"

extern DskObjectClass dsk_websocket_class;

DSK_OBJECT_CLASS_DEFINE(dsk_websocket, &dsk_object_class,
                        dsk_websocket_init, dsk_websocket_finalize);

dsk_boolean
dsk_websocket_retrieve (DskWebsocket *websocket,
                        unsigned     *length_out,
                        uint8_t     **data_out)
{
  ...
}

dsk_boolean
dsk_websocket_send     (DskWebsocket *websocket,
                        unsigned      length,
                        uint8_t      *data,
                        DskError    **error)
{
  ...
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

