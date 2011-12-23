typedef struct _JmuxFeedRequestHandler JmuxFeedRequestHandler;
typedef struct _JmuxFeedRequest JmuxFeedRequest;
typedef struct _JmuxFeedClass JmuxFeedClass;
typedef struct _JmuxFeed JmuxFeed;

struct _JmuxFeedRequestHandler
{
  void (*notify) (JmuxFeedRequestHandler *handler,
                  JmuxFeedRequest        *request,
                  const DskJsonValue     *value);
  void (*done)   (JmuxFeedRequestHandler *handler,
                  JmuxFeedRequest        *request);
  void (*failed) (JmuxFeedRequestHandler *handler,
                  JmuxFeedRequest        *request,
                  DskError               *error);
  void (*destroy)(JmuxFeedRequestHandler *handler);

  unsigned call_dsk_free_on_destroy : 1;

  JmuxFeedRequest *parent;              /* if non-NULL */
  void *data;
};

struct _JmuxFeedRequest
{
  JmuxFeed *feed;

  /* note: notify-within-notify causes a warning! */
  unsigned is_notifying : 1;
};

struct _JmuxFeedClass
{
  DskObjectClass base_class;
  size_t         sizeof_request;
  const char   *type_nickname;
  void        (*register_properties) (JmuxFeedClass      *feed_class);
  dsk_boolean (*configure)           (JmuxFeed           *feed,
                                      const DskJsonValue *config_object);
  void        (*init_request)        (JmuxFeed           *feed,
                                      JmuxFeedRequest    *request);
  void        (*start)               (JmuxFeedRequest    *request,
                                      const DskJsonValue *value);
  void        (*add_json)            (JmuxFeedRequest    *request,
                                      const DskJsonValue *value);
  void        (*cancel)              (JmuxFeedRequest    *request);
  void        (*finalize_request)    (JmuxFeedRequest    *request);
};

struct _JmuxFeed
{
  DskObject base_instance;

  /* flags */
  unsigned requires_initial_value : 1;
  unsigned permits_subsequent_values : 1;

  /* internal flags */
  unsigned has_configured : 1;
};

#define JMUX_FEED_SUBCLASS_DEFINE(class_static, ClassName, class_name)        \
DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(ClassName);                                \
class_static ClassName##Class class_name ## _class = { {                      \
  DSK_OBJECT_CLASS_DEFINE(ClassName, &dsk_octet_filter_class,                 \
                          class_name ## _init,                                \
                          class_name ## _finalize),                           \
  class_name##_register_properties,                                           \
  class_name##_configure                                                      \
  class_name##_init_request,                                                  \
  class_name##_start,                                                         \
  class_name##_add_json,                                                      \
  class_name##_cancel,                                                        \
  class_name##_finalize,                                                      \
} }


/* 'config' must be a JSON object. */
JmuxFeed *jmux_feed_new (const DskJsonValue *config,
                         DskError          **error);

