



/* TS0:  Attack v0 of a lightweight Templating System. */
typedef struct _DskTs0Function DskTs0Function;
typedef struct _DskTs0Tag DskTs0Tag;
typedef struct _DskTs0Scope DskTs0Scope;
typedef struct _DskTs0Class DskTs0Class;
typedef struct _DskTs0Context DskTs0Context;
typedef struct _DskTs0Expr DskTs0Expr;
typedef struct _DskTs0Stanza DskTs0Stanza;


/* --- string-valued functions --- */
struct _DskTs0Function
{
  dsk_boolean (*invoke) (DskTs0Function *function,
                         DskTs0Context *context,
                         unsigned        n_args,
                         DskTs0Expr    **args,
                         DskBuffer      *output,
                         DskError      **error);
  void        (*destroy)(DskTs0Function *function);
  unsigned ref_count;
};

DSK_INLINE_FUNC DskTs0Function *dsk_ts0_function_ref   (DskTs0Function *);
DSK_INLINE_FUNC void            dsk_ts0_function_unref (DskTs0Function *);

/* --- tag-type operators --- */
struct _DskTs0Tag
{
  dsk_boolean (*invoke) (DskTs0Tag         *tag,
                         DskTs0Context *context,
                         DskTs0Stanza      *body,
                         DskBuffer         *output,
                         DskError         **error);
  void        (*destroy)(DskTs0Tag *tag);
  unsigned ref_count;
};

DSK_INLINE_FUNC DskTs0Tag      *dsk_ts0_tag_ref   (DskTs0Tag *);
DSK_INLINE_FUNC void            dsk_ts0_tag_unref (DskTs0Tag *);



/* --- a lexical-scope, instantiated at evaluation time --- */
typedef struct _DskTs0ScopeVariable DskTs0ScopeVariable;
typedef struct _DskTs0ScopeSubscope DskTs0ScopeSubscope;
typedef struct _DskTs0ScopeFunction DskTs0ScopeFunction;
typedef struct _DskTs0ScopeTag DskTs0ScopeTag;
struct _DskTs0Scope
{
  unsigned ref_count;
  DskTs0Scope *up;
  DskTs0ScopeVariable *top_variable;
  DskTs0ScopeSubscope *top_subscope;
  DskTs0ScopeFunction *top_function;
  DskTs0ScopeTag *top_tag;

  /* for scopes with objects */
  DskTs0Class *scope_class;
  void *object_data;
  DskDestroyNotify object_data_destroy;
};

DskTs0Scope       *dsk_ts0_scope_new        (DskTs0Scope *parent_scope);
void               dsk_ts0_scope_set_object (DskTs0Scope *scope,
                                             DskTs0Class *scope_class,
                                             void *object_data,
                                             DskDestroyNotify object_data_destroy);
void               dsk_ts0_scope_set_variable (DskTs0Scope *scope,
                                               const char *key,
                                               const char *value);
void               dsk_ts0_scope_add_subscope (DskTs0Scope *scope,
                                               const char *name,
                                               DskTs0Scope *subscope);
void               dsk_ts0_scope_add_function (DskTs0Scope *scope,
                                               const char *name,
                                               DskTs0Function *function);
void               dsk_ts0_scope_add_tag (DskTs0Scope *scope,
                                               const char *name,
                                               DskTs0Tag *tag);

DSK_INLINE_FUNC void            dsk_ts0_scope_unref (DskTs0Scope *);
DSK_INLINE_FUNC DskTs0Scope   * dsk_ts0_scope_ref   (DskTs0Scope *);

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


/* --- for scopes that always have the same name, optionally operating on
   different data --- */
typedef char *(*DskTs0ClassLookupVarFunc) (void     *object_data,
                                           const char *name);
typedef DskTs0Scope *(*DskTs0ClassLookupScopeFunc) (void   *object_data,
                                                    const char *name);


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
  void       (*destroy_object)    (DskTs0Class *ts0_class,
                                   void        *object_data);
} DskTs0Class;

#if 0
typedef struct _DskTs0ClassFunctionNode DskTs0ClassFunctionNode;
typedef struct _DskTs0ClassTagNode DskTs0ClassTagNode;
struct _DskTs0Class
{
  unsigned ref_count;
  DskTs0ClassLookupVarFunc var_func;
  DskTs0ClassLookupScopeFunc scope_func;
  DskTs0ClassFunctionNode *functions;
  DskTs0ClassTagNode *tags;
};

typedef DskTs0Function *(*DskTs0ClassFunctionFunc) (void     *object_data,
                                                    const char *name);
typedef DskTs0Tag      *(*DskTs0ClassTagFunc)      (void     *object_data,
                                                    const char *name);

DskTs0Class *dsk_ts0_class_new          (DskTs0ClassLookupVarFunc   var_func,
                                         DskTs0ClassLookupScopeFunc scope_func);
void         dsk_ts0_class_add_function (DskTs0Class            *class_,
                                         const char             *func_name,
                                         DskTs0ClassFunctionFunc func);
void         dsk_ts0_class_add_tag      (DskTs0Class            *class_,
                                         const char             *func_name,
                                         DskTs0ClassTagFunc      func);
DSK_INLINE_FUNC void            dsk_ts0_class_unref (DskTs0Class *);
DSK_INLINE_FUNC DskTs0Class   * dsk_ts0_class_ref   (DskTs0Class *);
#endif

struct _DskTs0ScopeObject
{
  DskTs0Scope base_scope;
  DskTs0Class *scope_class;
  void *object_data;
  DskDestroyNotify destroy_object_data;
};

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
DskTs0Scope         *dsk_ts0_context_peek_scope       (DskTs0Context *);

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

/* --- inlines --- */
DSK_INLINE_FUNC DskTs0Function *dsk_ts0_function_ref   (DskTs0Function *);
DSK_INLINE_FUNC void            dsk_ts0_function_unref (DskTs0Function *);
DSK_INLINE_FUNC DskTs0Tag      *dsk_ts0_tag_ref   (DskTs0Tag *);
DSK_INLINE_FUNC void            dsk_ts0_tag_unref (DskTs0Tag *);
