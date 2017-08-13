
typedef struct DskThreadPoolClass DskThreadPoolClass;
typedef struct DskThreadPool DskThreadPool;

typedef void *(*DskThreadPoolFunc)         (void *func_data);
typedef void  (*DskThreadPoolResponseFunc) (void *result,
                                            void *func_data);


struct DskThreadPoolClass {
  DskObjectClass base_class;

  void (*run)(DskThreadPool     *pool,
              DskThreadPoolFunc  background_func,
              DskThreadPoolResponseFunc done_func,
              void              *func_data);
};
extern DskThreadPoolClass dsk_thread_pool_class;
struct DskThreadPool {
  DskObject base_instance;
};

void dsk_thread_pool_run (DskThreadPool     *pool,
                          DskThreadPoolFunc  background_func,
                          DskThreadPoolResponseFunc done_func,
                          void              *func_data);

// NOTE: the default class just runs background_func in
// the foreground and is ONLY useful for debugging.

// macro for subclassing DskThreadPool
#define DSK_THREAD_POOL_CLASS_DEFINE(name, lc_name)            \
  {                                                            \
    DSK_OBJECT_CLASS_DEFINE(name,                              \
                            parent_class,                      \
                            lc_name##_init,                    \
                            lc_name##_finalize),               \
    lc_name##_run                                              \
  }

