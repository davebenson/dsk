
#define DSK_INCLUDE_TS0
#include "dsk.h"



DskTs0Scope *dsk_ts0_scope_new(DskTs0Scope *parent_scope)
{
  DskTs0Scope *rv = dsk_malloc (sizeof (DskTs0Scope));
  rv->ref_count = 1;
  rv->up = parent_scope;
  if (parent_scope)
    dsk_ts0_scope_ref (parent_scope);
  rv->top_variable = NULL;
  rv->top_subscope = NULL;
  rv->top_function = NULL;
  rv->top_tag = NULL;
  rv->scope_class = NULL;
  rv->object_data = NULL;
  rv->object_data_destroy = NULL;
  return rv;
}


/* This can be casted to a DskTs0ScopeWritable, but usually use the
   dsk_ts0_global_* helper functions instead. */
DskTs0Scope *dsk_ts0_scope_global (void)
{
  ...
}

/* === extending the global namespace === */

/* adding global variables and scopes */
void dsk_ts0_global_set_variable (const char *key,
                                  const char *value)
{
  ...
}
void dsk_ts0_global_add_scope    (const char *key,
                                  DskTs0Scope *scope)
{
  ...
}

/* simple ways to add various types of functions */
void
dsk_ts0_global_add_lazy_func     (const char *name,
                                  DskTs0GlobalLazyFunc lazy_func)
{
  ...
}

void
dsk_ts0_global_add_lazy_func_data       (const char *name,
                                              DskTs0GlobalLazyDataFunc lazy_func,
                                              void *func_data,
                                              DskDestroyNotify destroy)
{
  ...
}

void
dsk_ts0_global_add_func(const char *name,
                                  DskTs0GlobalFunc func)
{
  ...
}

void
dsk_ts0_global_add_func_data            (const char *name,
                                              DskTs0GlobalDataFunc func,
                                              void *func_data,
                                              DskDestroyNotify destroy)
{
  ...
}



/* protected, b/c scopes should protect a type-safe wrapper 
   around "object_data" (and "destroy"). */
DskTs0Scope *dsk_ts0_scope_object_new_protected (DskTs0Class *scope_class,
                                                 void        *object_data,
                                                 DskDestroyNotify destroy)
{
  ...
}



DskTs0Context       *
dsk_ts0_context_new              (DskTs0Scope *parent_scope)
{
  ...
}
DskTs0Scope         *
dsk_ts0_context_peek_scope       (DskTs0Context *context)
{
  ...
}

/* --- end-user api --- */
dsk_boolean dsk_ts0_evaluate (DskTs0Context *context,
                              const char  *filename,
                              DskBuffer   *output,
                              DskError   **error)
{
  ...
}



dsk_boolean   dsk_ts0_stanza_evaluate   (DskTs0Context *context,
                                         DskTs0Stanza *stanza,
			                 DskBuffer    *target,
                                         DskError    **error)
{
  ...
}

DskTs0Stanza *dsk_ts0_stanza_parse_file (const char   *filename,
                                         DskError    **error)
{
  ...
}
void          dsk_ts0_stanza_free       (DskTs0Stanza *stanza)
{
  ...
}
