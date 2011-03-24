#include "dsk-common.h"
#include "dsk-mem-pool.h"
#include "dsk-hook.h"
#include "dsk-object.h"
#include "dsk-error.h"
#include "dsk-buffer.h"
#include "dsk-octet-io.h"
#include "dsk-octet-listener.h"

DskIOResult dsk_octet_listener_accept (DskOctetListener        *listener,
                                       DskOctetStream         **stream_out,
			               DskOctetSource         **source_out,
			               DskOctetSink           **sink_out,
                                       DskError               **error)
{
  DskOctetListenerClass *class = DSK_OCTET_LISTENER_GET_CLASS (listener);
  dsk_assert (class != NULL);
  dsk_assert (class->accept != NULL);
  return class->accept (listener, stream_out, source_out, sink_out, error);
}

static void
dsk_octet_listener_init (DskOctetListener *listener)
{
  dsk_hook_init (&listener->incoming, listener);
}
static void
dsk_octet_listener_finalize (DskOctetListener *listener)
{
  dsk_hook_clear (&listener->incoming);
}

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskOctetListener);
const DskOctetListenerClass dsk_octet_listener_class =
{
  DSK_OBJECT_CLASS_DEFINE(DskOctetListener,
                          &dsk_object_class,
                          dsk_octet_listener_init,
                          dsk_octet_listener_finalize),
  NULL,         /* no accept implementation */
  NULL          /* no shutdown implementation */
};

