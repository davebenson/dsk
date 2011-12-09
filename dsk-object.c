#include "dsk-common.h"
#include "dsk-object.h"
#include "dsk-mem-pool.h"
#include "debug.h"

#if DSK_ENABLE_DEBUGGING
dsk_boolean dsk_debug_object_lifetimes;
#endif

#define ASSERT_OBJECT_CLASS_MAGIC(class) \
  dsk_assert ((class)->object_class_magic == DSK_OBJECT_CLASS_MAGIC)

static DskMemPoolFixed weak_pointer_pool = DSK_MEM_POOL_FIXED_STATIC_INIT(sizeof (DskWeakPointer));

static void
dsk_object_init (DskObject *object)
{
  if (dsk_debug_object_lifetimes)
    dsk_warning ("creating object %s at %p", object->object_class->name, object);
  ASSERT_OBJECT_CLASS_MAGIC (object->object_class);
}

static void
dsk_object_finalize (DskObject *object)
{
  if (dsk_debug_object_lifetimes)
    dsk_warning ("finalizing object %s at %p", object->object_class->name, object);
  ASSERT_OBJECT_CLASS_MAGIC (object->object_class);
  dsk_assert (object->ref_count == 0);
}

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskObject);
DskObjectClass dsk_object_class =
DSK_OBJECT_CLASS_DEFINE(DskObject, NULL, dsk_object_init, dsk_object_finalize);

dsk_boolean
dsk_object_is_a (void *object,
                 const void *isa_class)
{
  DskObject *o = object;
  const DskObjectClass *c;
  const DskObjectClass *ic = isa_class;
  if (o == NULL)
    return DSK_FALSE;
  ASSERT_OBJECT_CLASS_MAGIC (ic);
  /* Deliberately ignore ref_count, since we want to be able to cast during
     finalization, i guess */
  c = o->object_class;
  ASSERT_OBJECT_CLASS_MAGIC (c);
  while (c != NULL)
    {
      if (c == ic)
        return DSK_TRUE;
      c = c->parent_class;
    }
  return DSK_FALSE;
}

dsk_boolean
dsk_object_class_is_a (const void *object_class,
                       const void *isa_class)
{
  const DskObjectClass *c = object_class;
  const DskObjectClass *ic = isa_class;
  ASSERT_OBJECT_CLASS_MAGIC (ic);
  ASSERT_OBJECT_CLASS_MAGIC (c);
  while (c != NULL)
    {
      if (c == ic)
        return DSK_TRUE;
      c = c->parent_class;
    }
  return DSK_FALSE;
}

static inline const char *
dsk_object_get_class_name (void *object)
{
  DskObject *o = object;
  if (o == NULL)
    return "*null*";
  if (o->object_class == NULL
   || o->object_class->object_class_magic != DSK_OBJECT_CLASS_MAGIC)
    return "invalid-object";
  return o->object_class->name;
}

static const char *
dsk_object_class_get_name (const DskObjectClass *class)
{
  if (class->object_class_magic == DSK_OBJECT_CLASS_MAGIC)
    return class->name;
  else
    return "*invalid-object-class*";
}

void *
dsk_object_cast (void       *object,
                 const void *isa_class,
                 const char *filename,
                 unsigned    line)
{
  if (!dsk_object_is_a (object, isa_class))
    {
      dsk_warning ("attempt to cast object %p (type %s) to %s invalid (%s:%u)",
                   object, dsk_object_get_class_name (object),
                   dsk_object_class_get_name (isa_class),
                   filename, line);
    }
  return object;
}

void *
dsk_object_class_cast (void       *object_class,
                       const void *isa_class,
                       const char *filename,
                       unsigned    line)
{
  if (!dsk_object_class_is_a (object_class, isa_class))
    {
      dsk_warning ("attempt to cast class %p (type %s) to %s invalid (%s:%u)",
                   object_class, ((DskObjectClass*)(object_class))->name,
                   dsk_object_class_get_name (isa_class),
                   filename, line);
    }
  return object_class;
}

const void *
dsk_object_cast_get_class (void       *object,
                           const void *isa_class,
                           const char *filename,
                           unsigned    line)
{
  if (!dsk_object_is_a (object, isa_class))
    {
      dsk_warning ("attempt to get-class for object %p (type %s) to %s invalid (%s:%u)",
                   object, dsk_object_get_class_name (object),
                   dsk_object_class_get_name (isa_class),
                   filename, line);
    }
  return ((DskObject*)object)->object_class;
}

static const DskObjectClass *all_instantiated_classes = NULL;

void _dsk_object_class_first_instance (const DskObjectClass *c)
{
  const DskObjectClass *at;
  DskObjectInitFunc *iat, *fat;
  DskObjectClassCacheData *cd = (DskObjectClassCacheData *) c->cache_data;
  unsigned n_init = 0, n_finalize = 0;
  dsk_assert (!cd->instantiated);
  for (at = c; at; at = at->parent_class)
    {
      if (at->init)
        n_init++;
      if (at->finalize)
        n_finalize++;
    }
  cd->init_funcs = dsk_malloc (sizeof (DskObjectFinalizeFunc)
                               * (n_init+n_finalize));
  cd->finalizer_funcs = cd->init_funcs + n_init;
  cd->n_init_funcs = n_init;
  cd->n_finalizer_funcs = n_finalize;
  iat = fat = cd->finalizer_funcs;
  for (at = c; at; at = at->parent_class)
    {
      if (at->init)
        *(--iat) = at->init;
      if (at->finalize)
        *fat++ = at->finalize;
    }
  cd->instantiated = DSK_TRUE;

  cd->prev_instantiated = all_instantiated_classes;
  all_instantiated_classes = c;
}


void
_dsk_object_cleanup_classes (void)
{
  while (all_instantiated_classes)
    {
      const DskObjectClass *c = all_instantiated_classes;
      DskObjectClassCacheData *cd = (DskObjectClassCacheData*) c->cache_data;
      all_instantiated_classes = cd->prev_instantiated;

      dsk_free (cd->init_funcs);
      cd->instantiated = DSK_FALSE;
    }
}

void
dsk_object_handle_last_unref (DskObject *o)
{
  const DskObjectClass *c = o->object_class;
  unsigned i, n;
  DskObjectFinalizeFunc *funcs;
  ASSERT_OBJECT_CLASS_MAGIC (c);

  if (o->weak_pointer)
    {
      DskWeakPointer *wp = o->weak_pointer;
      wp->object = NULL;
      o->weak_pointer = NULL;
      dsk_weak_pointer_unref (wp);
    }
  while (o->finalizer_list)
    {
      DskObjectFinalizeHandler *h = o->finalizer_list;
      o->finalizer_list = h->next;
      h->destroy (h->destroy_data);
      dsk_free (h);
    }

  n = c->cache_data->n_finalizer_funcs;
  funcs = c->cache_data->finalizer_funcs;
  for (i = 0; i < n; i++)
    funcs[i] (o);
  dsk_free (o);
}


void *
dsk_object_ref_f (void *object)
{
  return dsk_object_ref (object);
}

void
dsk_object_unref_f (void *object)
{
  dsk_object_unref (object);
}

void
dsk_object_trap_finalize (DskObject *object,
                     DskDestroyNotify destroy,
                     void            *destroy_data)
{
  DskObjectFinalizeHandler *f = DSK_NEW (DskObjectFinalizeHandler);
  f->destroy = destroy;
  f->destroy_data = destroy_data;
  f->next = object->finalizer_list;
  object->finalizer_list = f;
}

void
dsk_object_untrap_finalize (DskObject      *object,
                            DskDestroyNotify destroy,
                            void            *destroy_data)
{
  DskObjectFinalizeHandler **pf = &object->finalizer_list;
  while (*pf)
    {
      if ((*pf)->destroy == destroy && (*pf)->destroy_data == destroy_data)
        {
          DskObjectFinalizeHandler *die = *pf;
          *pf = die->next;
          dsk_free (die);
          return;
        }
      pf = &((*pf)->next);
    }
  dsk_return_if_reached ("no matching finalizer");
}

void dsk_weak_pointer_unref (DskWeakPointer *weak_pointer)
{
  if (--(weak_pointer->ref_count) == 0)
    {
      dsk_assert (weak_pointer->object == NULL);
      dsk_mem_pool_fixed_free (&weak_pointer_pool, weak_pointer);
    }
}
DskWeakPointer * dsk_weak_pointer_ref (DskWeakPointer *weak_pointer)
{
  ++(weak_pointer->ref_count);
  return weak_pointer;
}
DskWeakPointer * dsk_object_get_weak_pointer (DskObject *object)
{
  if (object->weak_pointer)
    ++(object->weak_pointer->ref_count);
  else
    {
      object->weak_pointer = dsk_mem_pool_fixed_alloc (&weak_pointer_pool);
      object->weak_pointer->ref_count = 2;
      object->weak_pointer->object = object;
    }
  return object->weak_pointer;
}
