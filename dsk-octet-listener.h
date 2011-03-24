
typedef struct _DskOctetListenerClass DskOctetListenerClass;
typedef struct _DskOctetListener DskOctetListener;

struct _DskOctetListenerClass
{
  DskObjectClass base_class;
  DskIOResult (*accept) (DskOctetListener        *listener,
                         DskOctetStream         **stream_out,
			 DskOctetSource         **source_out,
			 DskOctetSink           **sink_out,
                         DskError               **error);
  void        (*shutdown)(DskOctetListener       *listener);
};

struct _DskOctetListener
{
  DskObject base_instance;
  DskHook   incoming;
};

DskIOResult dsk_octet_listener_accept (DskOctetListener        *listener,
                                       DskOctetStream         **stream_out,
			               DskOctetSource         **source_out,
			               DskOctetSink           **sink_out,
                                       DskError               **error);
void        dsk_octet_listener_shutdown (DskOctetListener *listener);

extern const DskOctetListenerClass dsk_octet_listener_class;
#define DSK_OCTET_LISTENER_GET_CLASS(object) DSK_OBJECT_CAST_GET_CLASS(DskOctetListener, object, &dsk_octet_listener_class)
#define DSK_OCTET_LISTENER(object) DSK_OBJECT_CAST(DskOctetListener, object, &dsk_octet_listener_class)
