
typedef struct _DskObjectClass DskObjectClass;
typedef struct _DskObject DskObject;
typedef struct _DskObjectClassCacheData DskObjectClassCacheData;
struct _DskObjectClass
{
  const char *name;
  const DskObjectClass *parent_class;
  size_t sizeof_class;
  size_t sizeof_instance;
  unsigned pointers_after_base;
  unsigned object_class_magic;
  void (*init) (DskObject *object);
  void (*finalize) (DskObject *object);
  DskObjectClassCacheData *cache_data;
};
typedef void (*DskObjectInitFunc) (DskObject *object);
typedef void (*DskObjectFinalizeFunc) (DskObject *object);
#define DSK_OBJECT_CLASS_MAGIC          0x159daf3f
#define DSK_OBJECT_CLASS_DEFINE(name, parent_class, init_func, finalize_func) \
       { #name, (const DskObjectClass *) parent_class, \
         sizeof (name ## Class), sizeof (name), \
         (sizeof (name) - sizeof (DskObject)) / sizeof(void*), \
         DSK_OBJECT_CLASS_MAGIC, \
         (DskObjectInitFunc) init_func, (DskObjectFinalizeFunc) finalize_func, \
         (DskObjectClassCacheData *) &name ## __cache_data }
#define DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(name) \
       static DskObjectClassCacheData name##__cache_data = \
              { DSK_FALSE, 0, NULL, 0, NULL, NULL }
#define DSK_OBJECT(object) DSK_OBJECT_CAST(DskObject, object, &dsk_object_class)
#define DSK_OBJECT_CLASS(object) DSK_OBJECT_CLASS_CAST(DskObject, object, &dsk_object_class)
#define DSK_OBJECT_GET_CLASS(object) DSK_OBJECT_CAST_GET_CLASS(DskObject, object, &dsk_object_class)
struct _DskObjectClassCacheData
{
  dsk_boolean instantiated;
  unsigned n_init_funcs;
  DskObjectInitFunc *init_funcs;
  unsigned n_finalizer_funcs;
  DskObjectFinalizeFunc *finalizer_funcs;
  const DskObjectClass *prev_instantiated;
};

typedef struct _DskWeakPointer DskWeakPointer;
struct _DskWeakPointer
{
  unsigned ref_count;
  DskObject *object;
};

typedef struct _DskObjectFinalizeHandler DskObjectFinalizeHandler;
struct _DskObjectFinalizeHandler
{
  DskDestroyNotify destroy;
  void *destroy_data;
  DskObjectFinalizeHandler *next;
};


struct _DskObject
{
  const DskObjectClass *object_class;
  unsigned ref_count;
  DskWeakPointer *weak_pointer;
  DskObjectFinalizeHandler *finalizer_list;
};

/* The object interface */
DSK_INLINE_FUNC void      *dsk_object_new   (const void *object_class);
DSK_INLINE_FUNC void       dsk_object_unref (void *object);
DSK_INLINE_FUNC void      *dsk_object_ref   (void *object);
                 dsk_boolean dsk_object_is_a (void *object,
                                              const void *isa_class);
           dsk_boolean dsk_object_class_is_a (const void *object_class,
                                              const void *isa_class);

/* for use by debugging cast-macro implementations */
                 void       *dsk_object_cast (void *object,
                                              const void *isa_class,
                                              const char *filename,
                                              unsigned line);
                 void       *dsk_object_class_cast (void *object_class,
                                              const void *isa_class,
                                              const char *filename,
                                              unsigned line);
const            void       *dsk_object_cast_get_class (void *object,
                                              const void *isa_class,
                                              const char *filename,
                                              unsigned line);
/* non-inline versions of dsk_object_{ref,unref}
   (useful when you need a function pointer) */
                 void       dsk_object_unref_f (void *object);
                 void    *  dsk_object_ref_f (void *object);

                 void       dsk_object_trap_finalize (DskObject *object,
                                                 DskDestroyNotify destroy,
                                                 void            *destroy_data);
                 void       dsk_object_untrap_finalize(DskObject      *object,
                                                 DskDestroyNotify destroy,
                                                 void            *destroy_data);
DskWeakPointer             *dsk_weak_pointer_ref (DskWeakPointer *);
void                        dsk_weak_pointer_unref (DskWeakPointer *);
DskWeakPointer             *dsk_object_get_weak_pointer (DskObject *);

/* debugging and non-debugging implementations of the various
   cast macros.  */
#ifdef DSK_DISABLE_CAST_CHECKS
#define DSK_OBJECT_CAST(type, object, isa_class)                               \
  ((type*)(object))
#define DSK_OBJECT_CLASS_CAST(type, class, isa_class)                          \
  ((type##Class*)(class))
#define DSK_OBJECT_CAST_GET_CLASS(type, object, isa_class)                     \
    ((type##Class*)(((DskObject*)(object))->object_class))
#else
#define DSK_OBJECT_CAST(type, object, isa_class)                               \
  ((type*)dsk_object_cast(object, isa_class, __FILE__, __LINE__))
#define DSK_OBJECT_CLASS_CAST(type, class, isa_class)                          \
  ((type*)dsk_object_class_cast(class, isa_class, __FILE__, __LINE__))
#define DSK_OBJECT_CAST_GET_CLASS(type, object, isa_class)                     \
  ((type##Class*)dsk_object_cast_get_class(object, isa_class,                  \
                                           __FILE__, __LINE__))
#endif

/* private: but need to be exposed for public macros etc */
void dsk_object_handle_last_unref (DskObject *o);
void _dsk_object_class_first_instance (const DskObjectClass *c);

/* truly private (use dsk_cleanup() instead) */
void _dsk_object_cleanup_classes (void);

#if DSK_CAN_INLINE || defined(DSK_IMPLEMENT_INLINES)
DSK_INLINE_FUNC  void      *dsk_object_new   (const void *object_class)
{
  const DskObjectClass *c = object_class;
  DskObject *rv;
  unsigned i, n;
  DskObjectInitFunc *funcs;
  _dsk_inline_assert (c->object_class_magic == DSK_OBJECT_CLASS_MAGIC);
  if (!c->cache_data->instantiated)
    _dsk_object_class_first_instance (c);
  rv = dsk_malloc (c->sizeof_instance);
  rv->object_class = object_class;
  rv->ref_count = 1;
  rv->finalizer_list = NULL;
  rv->weak_pointer = NULL;
  dsk_bzero_pointers (rv + 1, c->pointers_after_base);
  n = c->cache_data->n_init_funcs;
  funcs = c->cache_data->init_funcs;
  for (i = 0; i < n; i++)
    funcs[i] (rv);
  return rv;
}

DSK_INLINE_FUNC  void       dsk_object_unref (void *object)
{
  DskObject *o = object;
  _dsk_inline_assert (o != NULL);
  _dsk_inline_assert (o->ref_count > 0);
  _dsk_inline_assert (o->object_class->object_class_magic == DSK_OBJECT_CLASS_MAGIC);
  if (--(o->ref_count) == 0)
    dsk_object_handle_last_unref (o);
}

DSK_INLINE_FUNC  void      *dsk_object_ref   (void *object)
{
  DskObject *o = object;
  _dsk_inline_assert (o->ref_count > 0);
  _dsk_inline_assert (o->object_class->object_class_magic == DSK_OBJECT_CLASS_MAGIC);
  o->ref_count += 1;
  return o;
}
#endif

extern DskObjectClass dsk_object_class;
