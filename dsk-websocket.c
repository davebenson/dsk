#include "dsk.h"

static uint8_t close_packet[9] = { 0xff, 0,0,0,0, 0,0,0,0 };


static void
dsk_websocket_init (DskWebsocket *websocket)
{
  dsk_hook_init (&websocket->readable, websocket);
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
do_shutdown (DskWebsocket *websocket)
{
  websocket->is_shutdown = DSK_TRUE;
  if (websocket->read_trap)
    {
      dsk_hook_trap_destroy (websocket->read_trap);
      websocket->read_trap = NULL;
    }
  if (websocket->write_trap)
    {
      dsk_hook_trap_destroy (websocket->write_trap);
      websocket->write_trap = NULL;
    }
  if (websocket->source)
    {
      dsk_octet_source_shutdown (websocket->source);
      dsk_object_unref (websocket->source);
      websocket->source = NULL;
    }
  if (websocket->sink)
    {
      dsk_octet_sink_shutdown (websocket->sink);
      dsk_object_unref (websocket->sink);
      websocket->sink = NULL;
    }
}

static void
do_deferred_shutdown (DskWebsocket *websocket)
{
  if (websocket->outgoing.size == 0)
    do_shutdown (websocket);
  else
    websocket->is_deferred_shutdown = DSK_TRUE;
}

static void
maybe_discard_data (DskWebsocket *websocket)
{
  if (websocket->to_discard > 0)
    {
      if (websocket->to_discard < websocket->incoming.size)
        {
          dsk_buffer_discard (&websocket->incoming, websocket->to_discard);
          websocket->to_discard = 0;
        }
      else
        {
          websocket->to_discard -= websocket->incoming.size;
          dsk_buffer_reset (&websocket->incoming);
        }
    }
}

static void
update_hooks_with_buffer (DskWebsocket *websocket)
{
  uint8_t header[9];
  uint64_t len;

  if (websocket->is_shutdown || websocket->is_deferred_shutdown)
    return;

  if (websocket->to_discard > 0
   || websocket->incoming.size < 9)
    {
      dsk_hook_set_idle_notify (&websocket->readable, DSK_FALSE);
      ensure_has_read_trap (websocket);
      return;
    }

  dsk_buffer_peek (&websocket->incoming, 9, header);
  len = dsk_uint64be_parse (header + 1);

  if (len > websocket->max_length)
    {
      dsk_hook_set_idle_notify (&websocket->readable, DSK_TRUE);
    }
  else if (websocket->incoming.size >= 9 + websocket->max_length)
    {
      dsk_hook_set_idle_notify (&websocket->readable, DSK_TRUE);
    }
  else
    {
      dsk_hook_set_idle_notify (&websocket->readable, DSK_FALSE);
      ensure_has_read_trap (websocket);
    }
}


DskIOResult
dsk_websocket_receive  (DskWebsocket *websocket,
                        unsigned     *length_out,
                        uint8_t     **data_out,
                        DskError    **error)
{
  uint8_t header[9];
  uint64_t length;
restart:
  maybe_discard_data (websocket);
  if (websocket->incoming.size < 9)
    return DSK_IO_RESULT_AGAIN;
  dsk_buffer_peek (&websocket->incoming, 9, header);
  length = dsk_uint64be_parse (header + 1);
  if (length > websocket->max_length)
    {
      switch (websocket->too_long_mode)
        {
        case DSK_WEBSOCKET_MODE_DROP:
          websocket->to_discard = length + 9;
          goto restart;
        case DSK_WEBSOCKET_MODE_SHUTDOWN:
          do_deferred_shutdown (websocket);
          return DSK_IO_RESULT_ERROR;
        case DSK_WEBSOCKET_MODE_RETURN_ERROR:
          websocket->to_discard = length + 9;
          maybe_discard_data (websocket);
          dsk_set_error (error, "packet too long (%llu bytes)", length);
          return DSK_IO_RESULT_ERROR;
        }
    }
  switch (header[0])
    {
    case 0xff:
      /* uh oh - shutdown packet */
      dsk_buffer_discard (&websocket->incoming, 9 + length);
      do_deferred_shutdown (websocket);
      return DSK_IO_RESULT_EOF;
    case 0x00:
      if (websocket->incoming.size - 9 < length)
        return DSK_IO_RESULT_AGAIN;
      *length_out = length;
      *data_out = dsk_malloc (length);
      dsk_buffer_discard (&websocket->incoming, 9);
      *data_out = dsk_malloc (length);
      dsk_buffer_read (&websocket->incoming, length, *data_out);
      update_hooks_with_buffer (websocket);
      return DSK_IO_RESULT_SUCCESS;
    default:
      /* error */
      switch (websocket->bad_packet_type_mode)
        {
        case DSK_WEBSOCKET_MODE_SHUTDOWN:
          do_deferred_shutdown (websocket);
          break;
        case DSK_WEBSOCKET_MODE_RETURN_ERROR:
          dsk_set_error (error, "packet had bad type: 0x%02x", header[0]);
          websocket->to_discard = length + 9;
          maybe_discard_data (websocket);
          return DSK_IO_RESULT_ERROR;
        case DSK_WEBSOCKET_MODE_DROP:
          websocket->to_discard = length + 9;
          goto restart;
        }
    }
  dsk_assert_not_reached ();
  return DSK_IO_RESULT_ERROR;
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
}


void
dsk_websocket_shutdown (DskWebsocket *websocket)
{
  if (!websocket->is_deferred_shutdown && !websocket->is_shutdown)
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
  DskError *error = NULL;
  switch (dsk_octet_sink_write_buffer (sink, &websocket->outgoing, &error))
    {
    case DSK_IO_RESULT_SUCCESS:
      break;
    case DSK_IO_RESULT_AGAIN:
      break;
    case DSK_IO_RESULT_EOF:
      error = dsk_error_new ("write_buffer returned EOF?");
      /* fallthrough */
    case DSK_IO_RESULT_ERROR:
      do_shutdown (websocket);
      return DSK_FALSE;
    }

  if (websocket->outgoing.size == 0)
    {
      if (websocket->is_deferred_shutdown)
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
  DskError *error = NULL;
  switch (dsk_octet_source_read_buffer (source, &websocket->incoming, &error))
    {
    case DSK_IO_RESULT_SUCCESS:
      update_hooks_with_buffer (websocket);
      break;
    case DSK_IO_RESULT_AGAIN:
      break;
    case DSK_IO_RESULT_ERROR:
      do_shutdown (websocket);
      return DSK_FALSE;
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
  websocket->write_trap = dsk_hook_trap (&websocket->sink->writable_hook,
                                        (DskHookFunc) handle_sink_writable,
                                        websocket, NULL);
}

static void
ensure_has_read_trap (DskWebsocket *websocket)
{
  if (websocket->source == NULL || websocket->read_trap != NULL)
    return;
  websocket->read_trap = dsk_hook_trap (&websocket->source->readable_hook,
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

