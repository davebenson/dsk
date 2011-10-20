#include "dsk.h"
#include "dsk-ts0.h"            /* TEMPORARY */



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
DskTs0Scope *dsk_ts0_scope_global (void);

/* === extending the global namespace === */

/* adding global variables and scopes */
void dsk_ts0_global_set_variable (const char *key,
                                  const char *value);
void dsk_ts0_global_add_scope    (const char *key,
                                  DskTs0Scope *scope);

/* simple ways to add various types of functions */
typedef dsk_boolean (*DskTs0GlobalLazyFunc) (DskTs0Context *context,
                                              unsigned        n_args,
                                              DskTs0Expr    **args,
                                              DskBuffer      *output,
                                              DskError      **error);
void dsk_ts0_global_add_lazy_func(const char *name,
                                  DskTs0GlobalLazyFunc lazy_func);
typedef dsk_boolean (*DskTs0GlobalLazyDataFunc) (DskTs0Context *context,
                                              unsigned        n_args,
                                              DskTs0Expr    **args,
                                              DskBuffer      *output,
                                              char           *func_data,
                                              DskError      **error);
void dsk_ts0_global_add_lazy_func_data       (const char *name,
                                              DskTs0GlobalLazyDataFunc lazy_func,
                                              void *func_data,
                                              DskDestroyNotify destroy);
typedef dsk_boolean (*DskTs0GlobalFunc)     (DskTs0Context *context,
                                             unsigned        n_args,
                                             char          **args,
                                             DskBuffer      *output,
                                              char           *func_data,
                                             DskError      **error);
void dsk_ts0_global_add_func(const char *name,
                                  DskTs0GlobalFunc func);
typedef dsk_boolean (*DskTs0GlobalDataFunc)     (DskTs0Context *context,
                                             unsigned        n_args,
                                             char          **args,
                                             DskBuffer      *output,
                                             DskError      **error);
void dsk_ts0_global_add_func_data            (const char *name,
                                              DskTs0GlobalDataFunc func,
                                              void *func_data,
                                              DskDestroyNotify destroy);

typedef struct {
  char *(*get_variable)           (DskTs0Class *ts0_class,  
                                   void        *object_data,
                                   const char  *var_name);  
  DskTs0Scope *(*get_subscope)    (DskTs0Class *ts0_class,  
                                   void        *object_data,
                                   const char  *object_name);  
  DskTs0Function *(*get_function) (DskTs0Class *ts0_class,  
                                   void        *object_data,
                                   const char  *function_name);  
  DskTs0Tag *(*get_tag)           (DskTs0Class *ts0_class,
                                   void        *object_data,
                                   const char  *tag_name);
} DskTs0Class;


/* protected, b/c scopes should protect a type-safe wrapper 
   around "object_data" (and "destroy"). */
DskTs0Scope *dsk_ts0_scope_object_new_protected (DskTs0Class *scope_class,
                                                 void        *object_data,
                                                 DskDestroyNotify destroy);


#if 0
/* underlying table object with pushable dynamic scopes */
typedef struct _DskTs0Table DskTs0Table;

DskTs0Table   *dsk_ts0_table_new        (void);
void        dsk_ts0_table_free       (DskTs0Table   *table);

void        dsk_ts0_table_set        (DskTs0Table   *table,
                                      const char *key,
		                      const char *value);
void        dsk_ts0_table_set_printf (DskTs0Table   *table,
                                      const char *key,
		                      const char *value_format,
			              ...) DSK_GNUC_PRINTF(3,4);
const char *dsk_ts0_table_get        (DskTs0Table   *table,
                                      const char *key);

void        dsk_ts0_table_push       (DskTs0Table   *table);
void        dsk_ts0_table_pop        (DskTs0Table   *table);
#endif


#if 0
typedef enum
{
  DSK_TS0_ERROR_UNKNOWN_TAG,
  DSK_TS0_ERROR_INVALID_TAG,
  DSK_TS0_ERROR_BAD_VARIABLE_NAME,
  DSK_TS0_ERROR_MISMATCHED_TAG,
  DSK_TS0_ERROR_UNEXPECTED_CLOSE_TAG,
} DskTs0ErrorCode;
#endif

DskTs0Context       *dsk_ts0_context_new              (DskTs0Scope *parent_scope);
DskTs0ScopeWritable *dsk_ts0_context_peek_scope       (DskTs0Context *);
DskTs0Scope         *dsk_ts0_context_peek_parent_scope(DskTs0Context *);

/* --- end-user api --- */
dsk_boolean dsk_ts0_evaluate (DskTs0Context *context,
                              const char  *filename,
                              DskBuffer   *output,
                              DskError   **error);



dsk_boolean   dsk_ts0_stanza_evaluate   (DskTs0Context *context,
                                         DskTs0Stanza *stanza,
			                 DskBuffer    *target,
                                         DskError    **error);

DskTs0Stanza *dsk_ts0_stanza_parse_file (const char   *filename,
                                         DskError    **error);
void          dsk_ts0_stanza_free       (DskTs0Stanza *stanza);


#if 0  /* internal now-a-days */
DskTs0Table * dsk_ts0_parse_tag_line    (DskTs0       *system,
                                         const char   *tag_line_args,
                                         DskTs0Table  *variables,
                                         DskError    **error);
#endif
