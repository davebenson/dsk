#include "dsk.h"

DskIOResult dsk_stream_listener_accept (DskStreamListener        *listener,
                                       DskStream         **stream_out,
                                       DskError               **error)
{
  DskStreamListenerClass *class = DSK_STREAM_LISTENER_GET_CLASS (listener);
  dsk_assert (class != NULL);
  dsk_assert (class->accept != NULL);
  return class->accept (listener, stream_out, error);
}

static void
dsk_stream_listener_init (DskStreamListener *listener)
{
  dsk_hook_init (&listener->incoming, listener);
}
static void
dsk_stream_listener_finalize (DskStreamListener *listener)
{
  dsk_hook_clear (&listener->incoming);
}

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskStreamListener);
const DskStreamListenerClass dsk_stream_listener_class =
{
  DSK_OBJECT_CLASS_DEFINE(DskStreamListener,
                          &dsk_object_class,
                          dsk_stream_listener_init,
                          dsk_stream_listener_finalize),
  NULL,         /* no accept implementation */
  NULL          /* no shutdown implementation */
};

