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
                                      const DskJsonValue *config);
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

#define JMUX_FEED_CLASS_DEFINE(Name, name, nickname, parent_class) \
  { DSK_OBJECT_CLASS_DEFINE(Name, parent_class, name##_init, name#_finalize), \
     sizeof(Name##Request), \
     nickname##_register_properties, \
     nickname##_configure, \

JmuxFeed *jmux_feed_new (const DskJsonValue *config,
                         DskError          **error);

