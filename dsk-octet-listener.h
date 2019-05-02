
typedef struct _DskStreamListenerClass DskStreamListenerClass;
typedef struct _DskStreamListener DskStreamListener;

struct _DskStreamListenerClass
{
  DskObjectClass base_class;
  DskIOResult (*accept) (DskStreamListener        *listener,
                         DskStream               **stream,
                         DskError               **error);
  void        (*shutdown)(DskStreamListener       *listener);
};

struct _DskStreamListener
{
  DskObject base_instance;
  DskHook   incoming;
};

DskIOResult dsk_stream_listener_accept (DskStreamListener        *listener,
                                       DskStream                **stream_out,
                                       DskError               **error);
void        dsk_stream_listener_shutdown (DskStreamListener *listener);

extern const DskStreamListenerClass dsk_stream_listener_class;
#define DSK_STREAM_LISTENER_GET_CLASS(object) DSK_OBJECT_CAST_GET_CLASS(DskStreamListener, object, &dsk_stream_listener_class)
#define DSK_STREAM_LISTENER(object) DSK_OBJECT_CAST(DskStreamListener, object, &dsk_stream_listener_class)
