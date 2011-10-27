/* TS0:  Attack v0 of a lightweight Templating System. */

/* There are several means of inserting variable content into
   your document:

   (1) Variables.  Variables are simple string variables.
       They can be accessed using a '$':
           $variable
       Sometimes variables are inside other namespaces or objects.
       To access such a variable use:
           $namespace.variable
           $object.variable

   (2) Expressions.  Expressions are little code snippets
       that evaluate to a string.  There are two syntaxes
       for using expressions at the top-level:
           $( add(2, 4) )
       and
           <%( add(2, 4) )%>

   (3) Tags.  These are for longer functions which operate on a body:
        <% if condition=$support_feature %>
          code implementing feature
        <%/ if %>
        <% repeat count="10" var="i" %>
          iteration $i
        <%/ repeat %>


    Lexing options: (If "!" precedes the name, it defaults to FALSE,)
       tag_chomp                   Remove a single newline following a tag.
       !strip_leading_spaces       Remove all whitespace after an open-tag
                                   or close tag.
       !strip_trailing_spaces      Remove all whitespace before a close-tag
                                   or an open-tag.
       !strip_spaces               Remove all leading and trailing whitespace.
       dollar_var                  Cause $variable to do variable lookup,
                                   $$ becomes a single $.

 */

/* TODO: remove_tag_indent option */


typedef struct _DskTs0Function DskTs0Function;
typedef struct _DskTs0Tag DskTs0Tag;
typedef struct _DskTs0Namespace DskTs0Namespace;
typedef struct _DskTs0Class DskTs0Class;
typedef struct _DskTs0Expr DskTs0Expr;
typedef struct _DskTs0Stanza DskTs0Stanza;


/* --- string-valued functions --- */
struct _DskTs0Function
{
  dsk_boolean (*invoke) (DskTs0Function *function,
                         DskTs0Namespace *ns,
                         unsigned        n_args,
                         DskTs0Expr    **args,
                         DskBuffer      *output,
                         DskError      **error);
  void        (*destroy)(DskTs0Function *function);
  unsigned ref_count;
  unsigned cachable : 1;
};

DSK_INLINE_FUNC DskTs0Function *dsk_ts0_function_ref   (DskTs0Function *);
DSK_INLINE_FUNC void            dsk_ts0_function_unref (DskTs0Function *);

dsk_boolean dsk_ts0_expr_evaluate (DskTs0Expr *expr,
                                   DskTs0Namespace *ns,
                                   DskBuffer  *target,
                                   DskError  **error);

/* --- tag-type operators --- */
struct _DskTs0Tag
{
  dsk_boolean (*invoke) (DskTs0Tag         *tag,
                         DskTs0Namespace   *ns,
                         DskTs0Stanza      *body,
                         DskBuffer         *output,
                         DskError         **error);
  void        (*destroy)(DskTs0Tag *tag);
  unsigned ref_count;
  unsigned cachable : 1;
};

DSK_INLINE_FUNC DskTs0Tag      *dsk_ts0_tag_ref   (DskTs0Tag *);
DSK_INLINE_FUNC void            dsk_ts0_tag_unref (DskTs0Tag *);



/* --- a lexical-namespace, instantiated at evaluation time --- */
typedef struct _DskTs0NamespaceNode DskTs0NamespaceNode;
struct _DskTs0Namespace
{
  unsigned ref_count;
  DskTs0Namespace *up;
  DskTs0NamespaceNode *top_variable;
  DskTs0NamespaceNode *top_subnamespace;
  DskTs0NamespaceNode *top_function;
  DskTs0NamespaceNode *top_tag;

  /* for namespaces with objects */
  DskTs0Class *namespace_class;
  void *object_data;
  DskDestroyNotify object_data_destroy;
};

DskTs0Namespace *dsk_ts0_namespace_new        (DskTs0Namespace *parent_namespace);
/* protected, b/c namespaces should protect a type-safe wrapper 
   around "object_data" (and "destroy"). */
void             dsk_ts0_namespace_set_object   (DskTs0Namespace *ns,
                                                 DskTs0Class *namespace_class,
                                                 void *object_data,
                                                 DskDestroyNotify destroy);
void             dsk_ts0_namespace_set_variable (DskTs0Namespace *ns,
                                                 const char *key,
                                                 const char *value);
char            *dsk_ts0_namespace_set_variable_slot (DskTs0Namespace *ns,
                                                 const char *key,
                                                 unsigned    value_length);
void             dsk_ts0_namespace_add_subspace (DskTs0Namespace *ns,
                                                 const char *name,
                                                 DskTs0Namespace *subnamespace);
void             dsk_ts0_namespace_add_function (DskTs0Namespace *ns,
                                                 const char *name,
                                                 DskTs0Function *function);
void             dsk_ts0_namespace_add_tag      (DskTs0Namespace *ns,
                                                 const char *name,
                                                 DskTs0Tag *tag);

/* The 'get' functions handle dotted named.
 * (The 'add' and 'set' functions do not.) */
/* Do not unref() the return-values. */
DskTs0Tag     *  dsk_ts0_namespace_get_tag      (DskTs0Namespace *ns,
                                                 const char      *dotted_name,
                                                 DskError       **error);
DskTs0Function * dsk_ts0_namespace_get_function (DskTs0Namespace *ns,
                                                 const char      *dotted_name,
                                                 DskError       **error);
DskTs0Namespace *dsk_ts0_namespace_get_namespace(DskTs0Namespace *ns,
                                                 const char      *dotted_name,
                                                 DskError       **error);
const char    *  dsk_ts0_namespace_get_variable (DskTs0Namespace *ns,
                                                 const char      *dotted_name,
                                                 DskError       **error);

DSK_INLINE_FUNC void            dsk_ts0_namespace_unref (DskTs0Namespace *);
DSK_INLINE_FUNC DskTs0Namespace*dsk_ts0_namespace_ref   (DskTs0Namespace *);

/* This can be casted to a DskTs0NamespaceWritable, but usually use the
   dsk_ts0_global_* helper functions instead. */
DskTs0Namespace *dsk_ts0_namespace_global (void);

/* === extending the global namespace === */

/* adding global variables and namespaces */
void dsk_ts0_global_set_variable (const char *key,
                                  const char *value);
void dsk_ts0_global_add_subspace (const char *key,
                                  DskTs0Namespace *ns);

/* simple ways to add various types of functions */
typedef dsk_boolean (*DskTs0LazyFunc)  (DskTs0Namespace *ns,
                                              unsigned         n_args,
                                              DskTs0Expr     **args,
                                              DskBuffer       *output,
                                              DskError       **error);
void dsk_ts0_global_add_lazy_func(const char *name,
                                  DskTs0LazyFunc lazy_func);
typedef dsk_boolean (*DskTs0LazyDataFunc) (DskTs0Namespace *ns,
                                                 unsigned        n_args,
                                                 DskTs0Expr    **args,
                                                 DskBuffer      *output,
                                                 char           *func_data,
                                                 DskError      **error);
void dsk_ts0_global_add_lazy_func_data       (const char *name,
                                              DskTs0LazyDataFunc lazy_func,
                                              void *func_data,
                                              DskDestroyNotify destroy);
typedef dsk_boolean (*DskTs0StrictFunc)     (DskTs0Namespace *ns,
                                             unsigned        n_args,
                                             char          **args,
                                             DskBuffer      *output,
                                             DskError      **error);
void dsk_ts0_global_add_func(const char *name,
                                  DskTs0StrictFunc func);
typedef dsk_boolean (*DskTs0StrictDataFunc)     (DskTs0Namespace *ns,
                                             unsigned        n_args,
                                             char          **args,
                                             DskBuffer      *output,
                                              char           *func_data,
                                             DskError      **error);
void dsk_ts0_global_add_func_data            (const char *name,
                                              DskTs0StrictDataFunc func,
                                              void *func_data,
                                              DskDestroyNotify destroy);

void dsk_ts0_global_take_function            (const char    *name,
                                              DskTs0Function *function);

DskTs0Function *
dsk_ts0_function_new_strict           (DskTs0StrictFunc         func);
DskTs0Function *
dsk_ts0_function_new_strict_data      (DskTs0StrictDataFunc     func,
                                       void                    *data,
                                       DskDestroyNotify         destroy);
DskTs0Function *
dsk_ts0_function_new_lazy      (DskTs0LazyFunc     lazy);
DskTs0Function *
dsk_ts0_function_new_lazy_data (DskTs0LazyDataFunc lazy,
                                       void                    *data,
                                       DskDestroyNotify         destroy);

/* --- classes -- local variables that act like objects or classes.
   (meaning they have variables, functions and tags) --- */
#if 0
typedef char *(*DskTs0ClassLookupVarFunc) (void     *object_data,
                                           const char *name);
typedef DskTs0Namespace *(*DskTs0ClassLookupNamespaceFunc) (void   *object_data,
                                                    const char *name);
#endif


struct _DskTs0Class
{
  char *(*get_variable)           (DskTs0Class *ts0_class,  
                                   void        *object_data,
                                   const char  *var_name);  
  DskTs0Namespace *(*get_subnamespace)    (DskTs0Class *ts0_class,  
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
};




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

/* --- end-user api --- */
dsk_boolean dsk_ts0_evaluate (DskTs0Namespace *ns,
                              const char  *filename,
                              DskBuffer   *output,
                              DskError   **error);



dsk_boolean   dsk_ts0_stanza_evaluate   (DskTs0Namespace *ns,
                                         DskTs0Stanza *stanza,
			                 DskBuffer    *target,
                                         DskError    **error);

DskTs0Stanza *dsk_ts0_stanza_parse_file (const char   *filename,
                                         DskError    **error);
DskTs0Stanza *dsk_ts0_stanza_parse_str  (const char   *str,
                                         const char   *filename,
                                         unsigned      line_no,
                                         DskError    **error);
void          dsk_ts0_stanza_dump       (DskTs0Stanza *stanza,
                                         unsigned      indent,
                                         DskBuffer    *buffer);
void          dsk_ts0_stanza_free       (DskTs0Stanza *stanza);


#if 0  /* internal now-a-days */
DskTs0Table * dsk_ts0_parse_tag_line    (DskTs0       *system,
                                         const char   *tag_line_args,
                                         DskTs0Table  *variables,
                                         DskError    **error);
#endif

/* private (used by inlines + macros) */
void _dsk_ts0_namespace_destroy (DskTs0Namespace *ns);


/* --- inlines --- */
#if DSK_CAN_INLINE || defined(DSK_IMPLEMENT_INLINES)
DSK_INLINE_FUNC DskTs0Function *
dsk_ts0_function_ref   (DskTs0Function *function)
{
  _dsk_inline_assert (function->ref_count > 0);
  function->ref_count += 1;
  return function;
}

DSK_INLINE_FUNC void
dsk_ts0_function_unref (DskTs0Function *function)
{
  _dsk_inline_assert (function->ref_count > 0);
  if (--(function->ref_count) == 0)
    function->destroy (function);
}

DSK_INLINE_FUNC DskTs0Tag *
dsk_ts0_tag_ref   (DskTs0Tag *tag)
{
  _dsk_inline_assert (tag->ref_count > 0);
  tag->ref_count += 1;
  return tag;
}

DSK_INLINE_FUNC void
dsk_ts0_tag_unref (DskTs0Tag *tag)
{
  _dsk_inline_assert (tag->ref_count > 0);
  if (--(tag->ref_count) == 0)
    {
      if (tag->destroy)
        tag->destroy (tag);
    }
}
DSK_INLINE_FUNC void
dsk_ts0_namespace_unref (DskTs0Namespace *ns)
{
  _dsk_inline_assert (ns->ref_count > 0);
  if (--(ns->ref_count) == 0)
    _dsk_ts0_namespace_destroy (ns);
}
DSK_INLINE_FUNC DskTs0Namespace *
dsk_ts0_namespace_ref (DskTs0Namespace *ns)
{
  _dsk_inline_assert (ns->ref_count > 0);
  ns->ref_count += 1;
  return ns;
}
#endif
