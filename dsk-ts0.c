#include <alloca.h>
#include <string.h>
#define DSK_INCLUDE_TS0
#include "dsk.h"
#include "dsk-rbtree-macros.h"
#include "dsk-ts0-builtins.h"

#define MAX_TAG_DEPTH           64

/* Forward-declarations of expression code */
typedef struct _DskTs0Filename DskTs0Filename;
static DskTs0Expr *dsk_ts0_expr_parse    (const char *str,
                                   const char **end_str_out,
                                   DskTs0Filename *filename,
                                   unsigned   *line_no_inout,
                                   DskError  **error);

/* note: takes ownership of 'data'! */
static DskTs0Expr *dsk_ts0_expr_new_literal (DskTs0Filename *filename,
                                               unsigned        line_no,
                                               unsigned        len,
                                               uint8_t        *data);
static DskTs0Expr *dsk_ts0_expr_new_variable (DskTs0Filename *filename,
                                              unsigned        line_no,
                                              unsigned name_len,
                                              const char *name);
static DskTs0Expr *dsk_ts0_expr_new_function (DskTs0Filename *filename,
                                              unsigned        line_no,
                                              unsigned name_len,
                                              const char *name,
                                              unsigned    n_args,
                                              DskTs0Expr **args);
static DskTs0Expr *dsk_ts0_expr_new_function_anon (DskTs0Filename *filename,
                                                   unsigned        line_no,
                                                   DskTs0Function *function,
                                                   unsigned    n_args,
                                                   DskTs0Expr **args);
static void        dsk_ts0_expr_free     (DskTs0Expr *expr);
       dsk_boolean dsk_ts0_expr_evaluate (DskTs0Expr *expr,
                                          DskTs0Namespace *ns,
                                          DskBuffer  *target,
                                          DskError  **error);
static void        dsk_ts0_expr_to_buffer (DskTs0Expr *expr,
                                           DskBuffer  *out);

struct _DskTs0Filename
{
  unsigned ref_count;
  /* filename follows */
};
static inline const char *
dsk_ts0_filename_get_str(const DskTs0Filename *fname)
{
  return (const char*)(fname + 1);
}
/* shorthand */
#define FILENAME_STR(fname) dsk_ts0_filename_get_str(fname)

typedef struct
{
  char *name;
  DskTs0Expr *expr;
} DskTs0NamedExpr;

struct _DskTs0NamespaceNode
{
  DskTs0NamespaceNode *left, *right, *parent;
  dsk_boolean is_red;
  void *value;
  /* key immediately follows structure */
};
#define COMPARE_SCOPE_NODES(a,b, rv) \
  rv = strcmp ((char*)((a)+1), (char*)((b)+1))
#define COMPARE_STRING_TO_SCOPE_NODE(a,b, rv) \
  rv = strcmp ((a), (char*)((b)+1))
#define GET_SCOPE_NODE_TREE(top) \
  top, DskTs0NamespaceNode *, \
  DSK_STD_GET_IS_RED, DSK_STD_SET_IS_RED, \
  parent, left, right, COMPARE_SCOPE_NODES

static DskTs0NamespaceNode *
lookup_namespace_node (DskTs0NamespaceNode *top,
                   const char      *key)
{
  DskTs0NamespaceNode *rv;
  DSK_RBTREE_LOOKUP_COMPARATOR (GET_SCOPE_NODE_TREE (top), key,
                                COMPARE_STRING_TO_SCOPE_NODE, rv);
  return rv;
}
static DskTs0NamespaceNode *
lookup_namespace_node_len (DskTs0NamespaceNode *top,
                           unsigned         key_len,
                           const char      *key)
{
  DskTs0NamespaceNode *rv;
#define COMPARE_KEY_KEYLEN_TO_SCOPE_NODE(unused,b, rv) \
  rv = strncmp (key, (char*)(b+1), key_len); \
  if (rv == 0 && ((char*)(b+1))[key_len] != 0) \
    rv = -1;
  DSK_RBTREE_LOOKUP_COMPARATOR (GET_SCOPE_NODE_TREE (top), unused,
                                COMPARE_KEY_KEYLEN_TO_SCOPE_NODE, rv);
  return rv;
}
static DskTs0NamespaceNode *
force_namespace_node (DskTs0NamespaceNode **top_ptr,
                  const char      *key)
{
  unsigned key_length = strlen (key);
  DskTs0NamespaceNode *rv = dsk_malloc (sizeof (DskTs0NamespaceNode) + key_length + 1);
  DskTs0NamespaceNode *conflict;
  memcpy (rv + 1, key, key_length + 1);
  DSK_RBTREE_INSERT (GET_SCOPE_NODE_TREE ((*top_ptr)), rv, conflict);
  if (conflict != NULL)
    {
      dsk_free (rv);
      return conflict;
    }
  else
    {
      rv->value = NULL;
      return rv;
    }
}

DskTs0Namespace *dsk_ts0_namespace_new(DskTs0Namespace *parent_namespace)
{
  DskTs0Namespace *rv = DSK_NEW (DskTs0Namespace);
  rv->ref_count = 1;
  rv->up = parent_namespace;
  if (parent_namespace)
    dsk_ts0_namespace_ref (parent_namespace);
  rv->top_variable = NULL;
  rv->top_subnamespace = NULL;
  rv->top_function = NULL;
  rv->top_tag = NULL;
  rv->namespace_class = NULL;
  rv->object_data = NULL;
  rv->object_data_destroy = NULL;
  return rv;
}


void               dsk_ts0_namespace_set_object (DskTs0Namespace *namespace,
                                             DskTs0Class *namespace_class,
                                             void *object_data,
                                             DskDestroyNotify object_data_destroy)
{
  dsk_assert (namespace->object_data == NULL);
  namespace->namespace_class = namespace_class;
  namespace->object_data = object_data;
  namespace->object_data_destroy = object_data_destroy;
}
void
dsk_ts0_namespace_set_variable (DskTs0Namespace *namespace,
                            const char *key,
                            const char *value)
{
  DskTs0NamespaceNode *node = force_namespace_node (&namespace->top_variable, key);
  char *vdup = dsk_strdup (value);
  if (node->value)
    dsk_free (node->value);
  node->value = vdup;
}

char *
dsk_ts0_namespace_set_variable_slot (DskTs0Namespace *namespace,
                                     const char      *key,
                                     unsigned         value_length)
{
  DskTs0NamespaceNode *node = force_namespace_node (&namespace->top_variable, key);
  /* TODO: could save malloc/free sometimes if they value_length will fit
     in existing value... */
  char *rv = dsk_malloc (value_length + 1);
  if (node->value)
    dsk_free (node->value);
  node->value = rv;
  return rv;
}

void
dsk_ts0_namespace_add_subnamespace (DskTs0Namespace *namespace,
                            const char *name,
                            DskTs0Namespace *subnamespace)
{
  DskTs0NamespaceNode *node = force_namespace_node (&namespace->top_subnamespace, name);
  dsk_ts0_namespace_ref (subnamespace);
  if (node->value)
    dsk_ts0_namespace_unref (node->value);
  node->value = subnamespace;
}

void
dsk_ts0_namespace_add_function (DskTs0Namespace *namespace,
                            const char *name,
                            DskTs0Function *function)
{
  DskTs0NamespaceNode *node = force_namespace_node (&namespace->top_function, name);
  dsk_ts0_function_ref (function);
  if (node->value)
    dsk_ts0_function_unref (node->value);
  node->value = function;
}

static void
dsk_ts0_namespace_take_function (DskTs0Namespace *namespace,
                            const char *name,
                            DskTs0Function *function)
{
  DskTs0NamespaceNode *node = force_namespace_node (&namespace->top_function, name);
  if (node->value)
    dsk_ts0_function_unref (node->value);
  node->value = function;
}

void
dsk_ts0_namespace_add_tag (DskTs0Namespace *namespace,
                       const char *name,
                       DskTs0Tag *tag)
{
  DskTs0NamespaceNode *node = force_namespace_node (&namespace->top_tag, name);
  dsk_ts0_tag_ref (tag);
  if (node->value)
    dsk_ts0_tag_unref (node->value);
  node->value = tag;
}

static DskTs0Namespace *
resolve_ns (DskTs0Namespace *top_ns,
            const char      *dotted_name,
            const char     **name_out,
            DskError       **error)
{
  const char *at = dotted_name;
  DskTs0Namespace *ns = top_ns;
  for (;;)
    {
      const char *dot = strchr (at, '.');
      if (dot == NULL)
        break;
      DskTs0NamespaceNode *node = lookup_namespace_node_len (ns->top_subnamespace, dot-at, at);
      if (node == NULL)
        {
          dsk_set_error (error, "no subnamespace %.*s", (int)(dot-at), at);
          return NULL;
        }
      ns = node->value;
    }
  *name_out = at;
  return ns;
}

typedef void*(*GetEntityFunc) (DskTs0Class *class,
                               void        *object_data,
                               const char  *name);
static void *
get_entity (DskTs0Namespace *ns,
            const char      *dotted_name,
            unsigned         top_offset,
            unsigned         class_offset,
            DskError       **error)
{
  const char *name;
  DskTs0Namespace *subns = resolve_ns (ns, dotted_name, &name, error);
  if (subns == NULL)
    return NULL;
  DskTs0NamespaceNode **ptop = (DskTs0NamespaceNode **) ((char*)subns + top_offset);
  DskTs0NamespaceNode *node = lookup_namespace_node (*ptop, name);
  if (node == NULL
   && subns->namespace_class != NULL)
    {
      GetEntityFunc func = * (GetEntityFunc *) ((char*)subns->namespace_class + class_offset);
      if (func != NULL)
        {
          void *entity = func (subns->namespace_class, subns->object_data, name);
          if (entity != NULL)
            {
              node = force_namespace_node (ptop, name);
              node->value = entity;
            }
        }
    }
  if (node == NULL)
    {
      dsk_set_error (error, "%s not found in namespace (full-name %s)",
                     name, dotted_name);
      return NULL;
    }
  dsk_assert (node->value != NULL);
  return node->value;
}

DskTs0Tag     *  dsk_ts0_namespace_get_tag      (DskTs0Namespace *ns,
                                                 const char      *dotted_name,
                                                 DskError       **error)
{
  return get_entity (ns, dotted_name, offsetof (DskTs0Namespace, top_tag),
                     offsetof (DskTs0Class, get_tag), error);
}

DskTs0Function * dsk_ts0_namespace_get_function (DskTs0Namespace *ns,
                                                 const char      *dotted_name,
                                                 DskError       **error)
{
  return get_entity (ns, dotted_name, offsetof (DskTs0Namespace, top_function),
                     offsetof (DskTs0Class, get_function), error);
}
DskTs0Namespace *dsk_ts0_namespace_get_namespace(DskTs0Namespace *ns,
                                                 const char      *dotted_name,
                                                 DskError       **error)
{
  return get_entity (ns, dotted_name, offsetof (DskTs0Namespace, top_subnamespace),
                     offsetof (DskTs0Class, get_subnamespace), error);
}
const char    *  dsk_ts0_namespace_get_variable (DskTs0Namespace *ns,
                                                 const char      *dotted_name,
                                                 DskError       **error)
{
  return get_entity (ns, dotted_name, offsetof (DskTs0Namespace, top_variable),
                     offsetof (DskTs0Class, get_variable), error);
}

static void
free_tree_recursive (DskTs0NamespaceNode *top,
                     DskDestroyNotify     destroy_func)
{
  if (top->left)
    free_tree_recursive (top->left, destroy_func);
  if (top->right)
    free_tree_recursive (top->right, destroy_func);
  destroy_func (top->value);
  dsk_free (top);
}

void
_dsk_ts0_namespace_destroy (DskTs0Namespace *ns)
{
  if (ns->top_tag)
    free_tree_recursive (ns->top_tag, (DskDestroyNotify) dsk_ts0_tag_unref);
  if (ns->top_function)
    free_tree_recursive (ns->top_function, (DskDestroyNotify) dsk_ts0_function_unref);
  if (ns->top_subnamespace)
    free_tree_recursive (ns->top_subnamespace, (DskDestroyNotify) dsk_ts0_namespace_unref);
  if (ns->top_variable)
    free_tree_recursive (ns->top_variable, dsk_free);
  dsk_free (ns);
}

static DskTs0Namespace *the_global_namespace = NULL;


/* This can be casted to a DskTs0NamespaceWritable, but usually use the
   dsk_ts0_global_* helper functions instead. */
DskTs0Namespace *dsk_ts0_namespace_global (void)
{
  if (the_global_namespace == NULL)
    the_global_namespace = dsk_ts0_namespace_new (NULL);
  return the_global_namespace;
}

/* === extending the global namespace === */

/* adding global variables and namespaces */
void dsk_ts0_global_set_variable (const char *key,
                                  const char *value)
{
  dsk_ts0_namespace_set_variable (dsk_ts0_namespace_global (), key, value);
}
void dsk_ts0_global_add_subnamespace    (const char *key,
                                     DskTs0Namespace *subnamespace)
{
  dsk_ts0_namespace_add_subnamespace (dsk_ts0_namespace_global (), key, subnamespace);
}

/* simple ways to add various types of functions */
void
dsk_ts0_global_add_lazy_func     (const char *name,
                                  DskTs0LazyFunc lazy_func)
{
  DskTs0Function *function = dsk_ts0_function_new_lazy (lazy_func);
  dsk_ts0_namespace_take_function (dsk_ts0_namespace_global (), name, function);
}

void
dsk_ts0_global_add_lazy_func_data  (const char              *name,
                                    DskTs0LazyDataFunc       lazy_func,
                                    void                    *func_data,
                                    DskDestroyNotify         destroy)
{
  DskTs0Function *function;
  function = dsk_ts0_function_new_lazy_data (lazy_func, func_data, destroy);
  dsk_ts0_namespace_take_function (dsk_ts0_namespace_global (), name, function);
}

void
dsk_ts0_global_add_strict_func(const char *name, DskTs0StrictFunc func)
{
  DskTs0Function *function;
  function = dsk_ts0_function_new_strict (func);
  dsk_ts0_namespace_take_function (dsk_ts0_namespace_global (), name, function);
}

void
dsk_ts0_global_add_strict_func_data          (const char *name,
                                              DskTs0StrictDataFunc func,
                                              void *func_data,
                                              DskDestroyNotify destroy)
{
  DskTs0Function *function;
  function = dsk_ts0_function_new_strict_data (func, func_data, destroy);
  dsk_ts0_namespace_take_function (dsk_ts0_namespace_global (), name, function);
}



#if 0
DskTs0Context *
dsk_ts0_context_new              (DskTs0Namespace *parent_namespace)
{
  DskTs0Namespace *ns = dsk_ts0_namespace_new (parent_namespace);
  ...
}

DskTs0Namespace         *
dsk_ts0_context_peek_namespace       (DskTs0Context *context)
{
  ...
}
#endif

/* --- functions --- */
#define DSK_TS0_FUNCTION_BASE_INIT(suffix) \
{ function_invoke__##suffix, function_destroy__##suffix, 1, 1 }

#define FUNCTION_INVOKE__ARGS \
  DskTs0Function *function, \
  DskTs0Namespace *ns, \
  unsigned        n_args, \
  DskTs0Expr    **args, \
  DskBuffer      *output, \
  DskError      **error

#define function_destroy__dsk_free ((void(*)(DskTs0Function*)) dsk_free)

/* helper function to invoke strict (ie non-lazy) functions */
typedef dsk_boolean (*StrictMarshal) (DskTs0Function *function,
                                      DskTs0Namespace *ns,
                                      unsigned        n_args,
                                      char          **args,
                                      DskBuffer      *output,
                                      DskError      **error);
static dsk_boolean
function_invoke_strict_generic (DskTs0Function *function,
                                DskTs0Namespace *ns,
                                unsigned        n_args,
                                DskTs0Expr    **args,
                                DskBuffer      *output,
                                StrictMarshal   marshal,
                                DskError      **error)
{
  unsigned *lengths = alloca (sizeof (unsigned) * n_args);
  char **arg_strings = alloca (sizeof (char *) * n_args);
  DskBuffer buffer = DSK_BUFFER_INIT;
  unsigned last_size = 0;
  unsigned i;
  for (i = 0; i < n_args; i++)
    {
      if (!dsk_ts0_expr_evaluate (args[i], ns, &buffer, error))
        {
          dsk_buffer_clear (&buffer);
          return DSK_FALSE;
        }
      lengths[i] = buffer.size - last_size;
      last_size = buffer.size;
    }

  unsigned needed_length = buffer.size + n_args;        /* for NUL characters */
  dsk_boolean must_free = (needed_length >= 512);

  char *slab;
  if (must_free)
    slab = dsk_malloc (needed_length);
  else
    slab = alloca (needed_length);

  char *at = slab;
  for (i = 0; i < n_args; i++)
    {
      arg_strings[i] = at;
      dsk_buffer_read (&buffer, lengths[i], at);
      at += lengths[i];
      *at++ = '\0';
    }
  dsk_boolean rv = marshal (function, ns, n_args, arg_strings, output, error);
  if (must_free)
    dsk_free (slab);
  return rv;
}


/* strict functions w/o data */
typedef struct {
  DskTs0Function base;
  DskTs0StrictFunc func;
} DskTs0Function_Strict;

static dsk_boolean
strict_marshal (DskTs0Function *function,
                DskTs0Namespace *ns,
                unsigned        n_args,
                char          **args,
                DskBuffer      *output,
                DskError      **error)
{
  DskTs0Function_Strict *F = (DskTs0Function_Strict *) function;
  return F->func (ns, n_args, args, output, error);
}

static dsk_boolean
function_invoke__strict (FUNCTION_INVOKE__ARGS)
{
  return function_invoke_strict_generic (function, ns, n_args, args, output, strict_marshal, error);
}
#define function_destroy__strict    function_destroy__dsk_free

DskTs0Function *
dsk_ts0_function_new_strict           (DskTs0StrictFunc         func)
{
  static DskTs0Function base = DSK_TS0_FUNCTION_BASE_INIT(strict);
  DskTs0Function_Strict *rv = DSK_NEW (DskTs0Function_Strict);
  rv->base = base;
  rv->func = func;
  return &rv->base;
}

/* strict functions w/ data */
typedef struct {
  DskTs0Function base;
  DskTs0StrictDataFunc func;
  void *func_data;
  DskDestroyNotify destroy;
} DskTs0Function_StrictData;

static dsk_boolean
strict_marshal__data (DskTs0Function *function,
                      DskTs0Namespace *ns,
                      unsigned        n_args,
                      char          **args,
                      DskBuffer      *output,
                      DskError      **error)
{
  DskTs0Function_StrictData *F = (DskTs0Function_StrictData *) function;
  return F->func (ns, n_args, args, output, F->func_data, error);
}

static dsk_boolean
function_invoke__strict_data (FUNCTION_INVOKE__ARGS)
{
  return function_invoke_strict_generic (function, ns, n_args, args, output, strict_marshal__data, error);
}

static void
function_destroy__strict_data (DskTs0Function *function)
{
  DskTs0Function_StrictData *F = (DskTs0Function_StrictData *) function;
  if (F->destroy)
    F->destroy (F->func_data);
  dsk_free (F);
}

DskTs0Function *
dsk_ts0_function_new_strict_data      (DskTs0StrictDataFunc     func,
                                       void                    *data,
                                       DskDestroyNotify         destroy)
{
  static DskTs0Function base = DSK_TS0_FUNCTION_BASE_INIT(strict_data);
  DskTs0Function_StrictData *rv = DSK_NEW (DskTs0Function_StrictData);
  rv->base = base;
  rv->func = func;
  rv->func_data = data;
  rv->destroy = destroy;
  return &rv->base;
}


/* lazy functions w/o data */
typedef struct {
  DskTs0Function base;
  DskTs0LazyFunc func;
} DskTs0Function_Lazy;
static dsk_boolean
function_invoke__lazy (FUNCTION_INVOKE__ARGS)
{
  DskTs0Function_Lazy *F = (DskTs0Function_Lazy *) function;
  return F->func (ns, n_args, args, output, error);
}
#define function_destroy__lazy    function_destroy__dsk_free

DskTs0Function *
dsk_ts0_function_new_lazy      (DskTs0LazyFunc func)
{
  static DskTs0Function base = DSK_TS0_FUNCTION_BASE_INIT(lazy);
  DskTs0Function_Lazy *rv = DSK_NEW (DskTs0Function_Lazy);
  rv->base = base;
  rv->func = func;
  return &rv->base;
}

/* lazy functions w/ data */
typedef struct {
  DskTs0Function base;
  DskTs0LazyDataFunc func;
  void *func_data;
  DskDestroyNotify destroy;
} DskTs0Function_LazyData;
static dsk_boolean
function_invoke__lazy_data (FUNCTION_INVOKE__ARGS)
{
  DskTs0Function_LazyData *F = (DskTs0Function_LazyData *) function;
  return F->func (ns, n_args, args, output, F->func_data, error);
}
static void
function_destroy__lazy_data (DskTs0Function *function)
{
  DskTs0Function_LazyData *F = (DskTs0Function_LazyData *) function;
  if (F->destroy)
    F->destroy (F->func_data);
  dsk_free (F);
}

DskTs0Function *
dsk_ts0_function_new_lazy_data (DskTs0LazyDataFunc lazy,
                                       void                    *data,
                                       DskDestroyNotify         destroy)
{
  static DskTs0Function base = DSK_TS0_FUNCTION_BASE_INIT(lazy_data);
  DskTs0Function_LazyData *rv = DSK_NEW (DskTs0Function_LazyData);
  rv->base = base;
  rv->func = lazy;
  rv->func_data = data;
  rv->destroy = destroy;
  return &rv->base;
}

/* --- ref-counted filenames --- */
static DskTs0Filename *dsk_ts0_filename_new (const char *filename)
{
  unsigned len = strlen (filename);
  DskTs0Filename *rv = dsk_malloc (sizeof (DskTs0Filename) + len + 1);
  rv->ref_count = 1;
  strcpy ((char*)(rv + 1), filename);
  return rv;
}
static inline DskTs0Filename *dsk_ts0_filename_ref (DskTs0Filename *filename)
{
  filename->ref_count += 1;
  return filename;
}
static inline void dsk_ts0_filename_unref (DskTs0Filename *filename)
{
  filename->ref_count -= 1;
  if (filename->ref_count == 0)
    dsk_free (filename);
}


/* --- end-user api --- */
dsk_boolean dsk_ts0_evaluate (DskTs0Namespace *ns,
                              const char  *filename,
                              DskBuffer   *output,
                              DskError   **error)
{
  DskTs0Stanza *stanza = dsk_ts0_stanza_parse_file (filename, error);
  dsk_boolean rv;
  if (stanza == NULL)
    return DSK_FALSE;
  rv = dsk_ts0_stanza_evaluate (ns, stanza, output, error);
  dsk_ts0_stanza_free (stanza);
  return rv;
}


typedef enum
{
  DSK_TS0_STANZA_PIECE_LITERAL,
  DSK_TS0_STANZA_PIECE_EXPRESSION,
  DSK_TS0_STANZA_PIECE_TAG
} DskTs0StanzaPieceType;
typedef struct _DskTs0StanzaPiece DskTs0StanzaPiece;
struct _DskTs0StanzaPiece
{
  DskTs0StanzaPieceType type;
  union
  {
    struct {
      unsigned length;
      uint8_t *data;
    } literal;
    DskTs0Expr *expression;
    struct {
      unsigned n_args;
      DskTs0NamedExpr *args;
      char *tag_name;
      DskTs0Tag *cached_tag;
      DskTs0Stanza *body;
    } tag;
  } info;
};
struct _DskTs0Stanza
{
  unsigned n_pieces;
  DskTs0StanzaPiece *pieces;
};

dsk_boolean   dsk_ts0_stanza_evaluate   (DskTs0Namespace *ns,
                                         DskTs0Stanza *stanza,
			                 DskBuffer    *target,
                                         DskError    **error)
{
  unsigned i;
  for (i = 0; i < stanza->n_pieces; i++)
    {
      DskTs0StanzaPiece *p = &stanza->pieces[i];
      switch (p->type)
        {
        case DSK_TS0_STANZA_PIECE_LITERAL:
          dsk_buffer_append (target,
                             p->info.literal.length,
                             p->info.literal.data);
          break;
        case DSK_TS0_STANZA_PIECE_EXPRESSION:
          if (!dsk_ts0_expr_evaluate (p->info.expression,
                                      ns, target, error))
            return DSK_FALSE;
          break;
        case DSK_TS0_STANZA_PIECE_TAG:
          {
            DskTs0Tag *tag = p->info.tag.cached_tag;
            if (tag == NULL)
              {
                tag = dsk_ts0_namespace_get_tag (ns, p->info.tag.tag_name, error);
                if (tag == NULL)
                  {
                    //dsk_add_error_suffix (error, " (at %s:%u)", filename, line_no);
                    return DSK_FALSE;
                  }
                if (tag->cachable)
                  {
                    tag->ref_count += 1;
                    p->info.tag.cached_tag = tag;
                  }
              }
            DskTs0Namespace *new_ns = dsk_ts0_namespace_new (ns);
            for (i = 0; i < p->info.tag.n_args; i++)
              {
                DskBuffer buffer = DSK_BUFFER_INIT;
                if (!dsk_ts0_expr_evaluate (p->info.tag.args[i].expr,
                                            ns, &buffer, error))
                  {
                    dsk_add_error_prefix (error, "evaluating arg %s",
                                          p->info.tag.args[i].name);
                    dsk_ts0_namespace_unref (new_ns);
                    return DSK_FALSE;
                  }
                char *buf;
                buf = dsk_ts0_namespace_set_variable_slot (new_ns,
                                                           p->info.tag.args[i].name,
                                                           buffer.size);
                dsk_buffer_read (&buffer, buffer.size, buf);
              }
            if (! tag->invoke (tag, new_ns, p->info.tag.body, target, error))
              {
                /// dsk_add_error_prefix(error, ...);
                dsk_ts0_namespace_unref (new_ns);
                return DSK_FALSE;
              }
            dsk_ts0_namespace_unref (new_ns);
          }
          break;
        }
    }
  return DSK_TRUE;
}

DskTs0Stanza *dsk_ts0_stanza_parse_file (const char   *filename,
                                         DskError    **error)
{
  DskTs0Stanza *rv;
  uint8_t *contents = dsk_file_get_contents (filename, NULL, error);
  if (contents == NULL)
    return NULL;
  rv = dsk_ts0_stanza_parse_str ((char*)contents, filename, 1, error);
  dsk_free (contents);
  return rv;
}

static void
append_piece (unsigned *n_inout,
              DskTs0StanzaPiece **pieces_inout,
              unsigned *pieces_alloced_inout,
              DskTs0StanzaPiece *init_pieces,
              DskTs0StanzaPiece to_append)
{
  if (*n_inout == *pieces_alloced_inout)
    {
      unsigned old_size = *pieces_alloced_inout * sizeof (DskTs0StanzaPiece);
      *pieces_alloced_inout *= 2;
      unsigned new_size = *pieces_alloced_inout * sizeof (DskTs0StanzaPiece);

      if (*pieces_inout == init_pieces)
        {
          *pieces_inout = dsk_malloc (new_size);
          memcpy (*pieces_inout, init_pieces, old_size);
        }
      else
        *pieces_inout = dsk_realloc (*pieces_inout, new_size);
    }
  (*pieces_inout)[*n_inout] = to_append;
  *n_inout += 1;
}

static void
skip_whitespace (const char **str_inout,
                 unsigned    *line_no_inout)
{
  const char *str = *str_inout;
  unsigned line_no = *line_no_inout;
  while (dsk_ascii_isspace (*str))
    {
      if (*str == '\n')
        ++line_no;
      ++str;
    }
  *str_inout = str;
  *line_no_inout = line_no;
}

static inline dsk_boolean
slice_matches (const char *start, const char *end, const char *str)
{
  while (start < end)
    {
      if (*str != *start)
        return DSK_FALSE;
      start++;
      str++;
    }
  return (*str == '\0');
}


static void
dsk_ts0_stanza_piece_clear (DskTs0StanzaPiece *piece)
{
  switch (piece->type)
    {
    case DSK_TS0_STANZA_PIECE_LITERAL:
      dsk_free (piece->info.literal.data);
      break;
    case DSK_TS0_STANZA_PIECE_TAG:
      {
        unsigned i;
        for (i = 0; i < piece->info.tag.n_args; i++)
          {
            dsk_free (piece->info.tag.args[i].name);
            dsk_ts0_expr_free (piece->info.tag.args[i].expr);
          }
      }
      dsk_free (piece->info.tag.tag_name);
      if (piece->info.tag.cached_tag)
        dsk_ts0_tag_unref (piece->info.tag.cached_tag);
      dsk_ts0_stanza_free (piece->info.tag.body);
      break;
    case DSK_TS0_STANZA_PIECE_EXPRESSION:
      dsk_ts0_expr_free (piece->info.expression);
      break;
    }
}



typedef struct _TagStack TagStack;
struct _TagStack
{
  /* index into 'pieces' array where the first argument to this tag occurs. */
  unsigned first_piece;

  /* name of the tag */
  char *name;

  /* arguments to the opening tag */
  unsigned n_args;
  DskTs0NamedExpr *args;
};

DskTs0Stanza *dsk_ts0_stanza_parse_str  (const char   *str,
                                         const char   *filename,
                                         unsigned      line_no,
                                         DskError    **error)
{
  DskTs0Filename *fname = dsk_ts0_filename_new (filename);
#if 0
  <% %> <%/ %>
  <%( $foo )%>
#endif
  /* Whether $variable is a variable substitution in plain text.
   * Control with +/-dollar_var.
   */
  dsk_boolean dollar_variables = DSK_TRUE;

  /* Whether a newline is removed immediately following a tag.
   * Control with +/-tag_chomp.
   */
  dsk_boolean tags_chomp_newline = DSK_TRUE;

  /* Strip whitespace at start of block, end of block.
     Control with +/-strip_leading_spaces, +/-strip_trailing_spaces
     or +/-strip_spaces to set both at once. */
  dsk_boolean strip_leading_whitespace = DSK_FALSE;
  dsk_boolean strip_trailing_whitespace = DSK_FALSE;

  /* stack of open <% tags %> */
  TagStack tag_stack[MAX_TAG_DEPTH];
  unsigned tag_stack_size = 0;

  /* stripping whitespace for the next literal we are going to add */
  dsk_boolean now_strip_first_newline = DSK_FALSE;
  dsk_boolean now_strip_leading_whitespace = DSK_FALSE;
  dsk_boolean now_strip_trailing_whitespace = DSK_FALSE;


  unsigned n_pieces = 0;
  unsigned pieces_alloced = 32;
  DskTs0StanzaPiece init_pieces[32];
  DskTs0StanzaPiece *pieces = init_pieces;
#define APPEND_PIECE(piece) \
      append_piece (&n_pieces, &pieces, &pieces_alloced, init_pieces, piece)
  for (;;)
    {
      /* try making a literal */
      const char *start = str;
      unsigned n_double_dollars = 0;
      if (now_strip_first_newline)
        {
          const char *at = str;
          while (dsk_ascii_isspace (*at))
            {
              if (*at == '\n')
                {
                  str = at + 1;
                  line_no += 1;
                  break;
                }
              at++;
            }
          now_strip_first_newline = DSK_FALSE;
        }
      if (now_strip_leading_whitespace)
        {
          skip_whitespace (&str, &line_no);
          now_strip_leading_whitespace = DSK_FALSE;
        }
      if (dollar_variables)
        {
          while (*str && (str[0] != '<' || str[1] != '%'))
            {
              if (*str == '$')
                {
                  if (str[1] == '$')
                    {
                      n_double_dollars++;
                      str += 2;
                      continue;
                    }
                  else
                    {
                      /* variable expression... which we will handle after
                         the literal-handling code. */
                      break;
                    }
                }
              if (*str == '\n')
                line_no++;
              str++;
            }
        }
      else
        {
          while (*str && (str[0] != '<' || str[1] != '%'))
            {
              if (*str == '\n')
                line_no++;
              str++;
            }
        }
      const char *end_literal = str;
      if (now_strip_trailing_whitespace)
        {
          while (end_literal > start && dsk_ascii_isspace (*(end_literal-1)))
            end_literal--;
          now_strip_trailing_whitespace = DSK_FALSE;
        }
      if (end_literal > start)
        {
          if (n_double_dollars > 0)
            {
              /* literal with dollar characters removed */
              DskTs0StanzaPiece piece;
              uint8_t *out;
              piece.type = DSK_TS0_STANZA_PIECE_LITERAL;
              piece.info.literal.length = (end_literal - start) - n_double_dollars;
              out = dsk_malloc (piece.info.literal.length);
              piece.info.literal.data = out;
              while (start < end_literal)
                {
                  *out++ = *start;
                  if (*start == '$')
                    start += 2;
                  else
                    start += 1;
                }
              APPEND_PIECE (piece);
            }
          else
            {
              /* simple literal */
              DskTs0StanzaPiece piece;
              piece.type = DSK_TS0_STANZA_PIECE_LITERAL;
              piece.info.literal.length = (end_literal - start);
              piece.info.literal.data = dsk_memdup (end_literal - start, start);
              APPEND_PIECE (piece);
            }
        }
      
      if (*str == 0)
        {
          break;
        }
      if (*str == '$')
        {
          /* variable expression */
          DskTs0StanzaPiece piece;
          str++;
          if (*str == '(')
            {
              const char *end;
              if (!dsk_ts0_expr_parse (str, &end, fname, &line_no, error))
                goto error_cleanup;
              str = end;
              continue;
            }
          else
            {
              start = str;
              while (dsk_ascii_isalnum (*str) || *str == '_' || *str == '.')
                str++;
              if (start == str)
                {
                  dsk_set_error (error, "missing variable-name after '$' (%s:%u)",
                                 filename, line_no);
                  goto error_cleanup;
                }
              piece.type = DSK_TS0_STANZA_PIECE_EXPRESSION;
              piece.info.expression = dsk_ts0_expr_new_variable (fname, line_no,
                                                              str - start, start);
              APPEND_PIECE (piece);
              continue;
            }
        }

      dsk_boolean setup_posttag_whitespace_rules = DSK_FALSE;

      /* at <% */
      dsk_assert (str[0] == '<' && str[1] == '%');
      str += 2;
      skip_whitespace (&str, &line_no);

      if (dsk_ascii_isalnum (*str) || *str == '_')
        {
          /* tag opening */
          const char *tag_name_start = str++;
          while (dsk_ascii_isalnum (*str) || *str == '_')
            str++;
          const char *tag_name_end = str;
          unsigned n_args = 0;
          unsigned args_alloced = 8;
          DskTs0NamedExpr init_args[8];
          DskTs0NamedExpr *args = init_args;

          for (;;)
            {
              skip_whitespace (&str, &line_no);
              if (*str == 0)
                {
                  dsk_set_error (error, "missing %%> in opening tag (%s:%u)",
                                 filename, line_no);
                  goto error_cleanup;
                }
              if (*str == '%' && str[1] == '>')
                {
                  str += 2;
                  break;
                }
              else if (dsk_ascii_isalnum (*str) || *str == '_')
                {
                  DskTs0NamedExpr named_expr;
                  const char *arg_name_start = str++;
                  while (dsk_ascii_isalnum (*str) || *str == '_')
                    str++;
                  const char *arg_name_end = str;
                  skip_whitespace (&str, &line_no);
                  if (*str != '=')
                    {
                      dsk_set_error (error, "expected '=' at %s:%u",
                                     filename, line_no);
                      /* TODO: free 'args' */
                      goto error_cleanup;
                    }
                  str++;
                  skip_whitespace (&str, &line_no);
                  const char *end;
                  named_expr.expr = dsk_ts0_expr_parse (str, &end, fname, &line_no, error);
                  if (named_expr.expr == NULL)
                    {
                      dsk_add_error_prefix (error, "in opening-tag %.*s",
                                            (int)(tag_name_end-tag_name_start),
                                            tag_name_start);
                      /* TODO: free 'args' */
                      goto error_cleanup;
                    }
                  str = end;
                  skip_whitespace (&str, &line_no);
                  named_expr.name = dsk_strdup_slice (arg_name_start, arg_name_end);
                  if (args_alloced == n_args)
                    {
                      unsigned old_size = args_alloced * sizeof (DskTs0NamedExpr);
                      unsigned new_size = old_size * 2;
                      args_alloced *= 2;
                      if (args == init_args)
                        {
                          DskTs0NamedExpr *n = dsk_malloc (new_size);
                          memcpy (n, args, old_size);
                          args = n;
                        }
                      else
                        args = dsk_realloc (args, new_size);
                    }
                  args[n_args++] = named_expr;
                }
              else
                {
                  dsk_set_error (error,
                                 "unexpected character %s in open-tag %s:%u",
                                 dsk_ascii_byte_name (*str),
                                 filename, line_no);
                  /* TODO: free 'args' */
                  goto error_cleanup;
                }
            }

          /* Create opening tag in stack */
          if (tag_stack_size == MAX_TAG_DEPTH)
            {
              dsk_set_error (error,
                             "maximum tag-depth %u exceeded, at %s:%u",
                             MAX_TAG_DEPTH, filename, line_no);
              /* TODO: free 'args' */
              goto error_cleanup;
            }
          tag_stack[tag_stack_size].name = dsk_strdup_slice (tag_name_start, tag_name_end);
          tag_stack[tag_stack_size].n_args = n_args;
          tag_stack[tag_stack_size].args = dsk_memdup (sizeof (DskTs0NamedExpr) * n_args, args);
          tag_stack[tag_stack_size].first_piece = n_pieces;
          tag_stack_size++;

          if (args != init_args)
            dsk_free (args);

          /* setup whitespace-swallowing */
          setup_posttag_whitespace_rules = DSK_TRUE;
        }
      else if (*str == '/')
        {
          /* tag closing */

          /* ensure stack non-empty */
          if (tag_stack_size == 0)
            {
              dsk_set_error (error, "got close tag, no open tag %s:%u",
                             filename, line_no);
              goto error_cleanup;
            }

          TagStack *top = tag_stack + tag_stack_size - 1;
          /* verify tag-stack match, if close-tag has name */
          str++;
          skip_whitespace (&str, &line_no);
          if (dsk_ascii_isalnum (*str) || *str == '_')
            {
              const char *name_start = str++;
              while (dsk_ascii_isalnum (*str) || *str == '_')
                str++;
              const char *name_end = str;
              skip_whitespace (&str, &line_no);
              if (str[0] != '%' || str[1] != '>')
                {
                  dsk_set_error (error, "expected %%> at %s:%u", filename, line_no);
                  goto error_cleanup;
                }

              /* ensure tags match */
              const char *top_name = top->name;
              if (slice_matches (name_start, name_end, top->name))
                {
                  dsk_set_error (error,
                                 "close-tag does not match open-tag: got <%%/ %.*s %%>, expected <%%/ %s %%> at %s:%u",
                                 (int)(name_end - name_start), name_start,
                                 top_name,
                                 filename, line_no);
                  goto error_cleanup;
                }
              str += 2;
            }
          else if (*str == '%' && str[1] == '>')
            {
              /* empty close tag */
              str += 2;
            }
          else
            {
              /* syntax error */
              dsk_set_error (error, "unexpected character %s after <%%/, %s:%u",
                             dsk_ascii_byte_name (*str),
                             filename, line_no);
              goto error_cleanup;
            }

          /* create stanza piece */
          DskTs0StanzaPiece piece;
          piece.type = DSK_TS0_STANZA_PIECE_TAG;
          piece.info.tag.n_args = top->n_args;
          piece.info.tag.args = top->args;      /* take ownership */
          piece.info.tag.tag_name = top->name;  /* take ownership */
          piece.info.tag.cached_tag = NULL;
          DskTs0Stanza *new_substanza;
          new_substanza = DSK_NEW (DskTs0Stanza);
          piece.info.tag.body = new_substanza;
          new_substanza->n_pieces = n_pieces - top->first_piece;
          new_substanza->pieces = dsk_memdup (sizeof (DskTs0StanzaPiece) * new_substanza->n_pieces, pieces + top->first_piece);
          n_pieces = top->first_piece;
          APPEND_PIECE (piece);

          /* pop stack */
          tag_stack_size--;

          /* setup whitespace-swallowing */
          setup_posttag_whitespace_rules = DSK_TRUE;
        }
      else if (*str == '(')
        {
          /* expression */
          const char *end;
          DskTs0Expr *expr;
          expr = dsk_ts0_expr_parse (str, &end, fname, &line_no, error);
          if (expr == NULL)
            goto error_cleanup;
          DskTs0StanzaPiece piece;
          piece.type = DSK_TS0_STANZA_PIECE_EXPRESSION;
          piece.info.expression = expr;
          APPEND_PIECE (piece);
          str = end;
          skip_whitespace (&str, &line_no);
          if (str[0] != '%' || str[1] != '>')
            {
              dsk_set_error (error, "after expression, expected %%>, %s:%u",
                             filename, line_no);
              goto error_cleanup;
            }
          str += 2;

          /* setup whitespace-swallowing */
          setup_posttag_whitespace_rules = DSK_TRUE;
        }
      else if (*str == '+' || *str == '-')
        {
          for (;;)
            {
              /* tokenizer / parser flags */
              dsk_boolean enable = *str == '+';
              str++;
              const char *name_start = str;
              while (dsk_ascii_isalnum (*str) || *str == '_')
                str++;
              const char *name_end = str;
              if (name_end == name_start)
                {
                  dsk_set_error (error, "missing option name at %s:%u",
                                 filename, line_no);
                  goto error_cleanup;
                }
              skip_whitespace (&str, &line_no);
              if (slice_matches (name_start, name_end, "tag_chomp"))
                tags_chomp_newline = enable;
              else if (slice_matches (name_start, name_end, "dollar_var"))
                dollar_variables = enable;
              else if (slice_matches (name_start, name_end, "strip_spaces"))
                strip_leading_whitespace = strip_trailing_whitespace = enable;
              else if (slice_matches (name_start, name_end, "strip_trailing_spaces"))
                strip_trailing_whitespace = enable;
              else if (slice_matches (name_start, name_end, "strip_leading_spaces"))
                strip_leading_whitespace = enable;
              else
                {
                  dsk_set_error (error, "unknown option %.*s at %s:%u",
                                 (int)(name_end - name_start), name_start,
                                 filename, line_no);
                  goto error_cleanup;
                }

              skip_whitespace (&str, &line_no);
              if (str[0] == '%' && str[1] == '>')
                {
                  str += 2;
                  break;
                }
              else if (*str != '+' && *str != '-')
                {
                  dsk_set_error (error, "unexpected character %s, %s:%u",
                                 dsk_ascii_byte_name (*str), filename, line_no);
                  goto error_cleanup;
                }
            }

          /* setup whitespace-swallowing */
          setup_posttag_whitespace_rules = DSK_TRUE;
        }
      else
        {
          dsk_set_error (error, "unexpected <%% %c at %s:%u",
                         *str, filename, line_no);
          goto error_cleanup;
        }

      if (setup_posttag_whitespace_rules)
        {
          now_strip_first_newline = tags_chomp_newline;
          now_strip_leading_whitespace = strip_leading_whitespace;
          now_strip_trailing_whitespace = strip_trailing_whitespace;
        }
    }

  DskTs0Stanza *rv = DSK_NEW (DskTs0Stanza);
  rv->n_pieces = n_pieces;
  if (pieces == init_pieces)
    rv->pieces = dsk_memdup (sizeof (DskTs0StanzaPiece) * n_pieces, pieces);
  else
    rv->pieces = pieces;
  return rv;


error_cleanup:
  {
    unsigned i;
    for (i = 0; i < n_pieces; i++)
      dsk_ts0_stanza_piece_clear (pieces + i);
  }
  if (pieces != init_pieces)
    dsk_free (pieces);
  return NULL;
}

void           dsk_ts0_stanza_dump       (DskTs0Stanza *stanza,
                                          unsigned      indent,
                                          DskBuffer    *buffer)
{
  unsigned i;
  for (i = 0; i < stanza->n_pieces; i++)
    {
      dsk_buffer_append_repeated_byte (buffer, indent, ' ');
      dsk_buffer_printf (buffer, "piece %u:\n", i);
      dsk_buffer_append_repeated_byte (buffer, indent+2, ' ');
      switch (stanza->pieces[i].type)
        {
        case DSK_TS0_STANZA_PIECE_LITERAL:
          dsk_filter_to_buffer (stanza->pieces[i].info.literal.length,
                                stanza->pieces[i].info.literal.data,
                                dsk_c_quoter_new (DSK_TRUE, DSK_TRUE),
                                buffer,
                                NULL);
          break;
        case DSK_TS0_STANZA_PIECE_EXPRESSION:
          dsk_ts0_expr_to_buffer (stanza->pieces[i].info.expression, buffer);
          break;
        case DSK_TS0_STANZA_PIECE_TAG:
          dsk_buffer_append_string (buffer, "<% ");
          dsk_buffer_append_string (buffer, stanza->pieces[i].info.tag.tag_name);
          unsigned a;
          for (a = 0; a < stanza->pieces[i].info.tag.n_args; a++)
            {
              dsk_buffer_append_byte (buffer, ' ');
              dsk_buffer_append_string (buffer, stanza->pieces[i].info.tag.args[a].name);
              dsk_buffer_append_byte (buffer, '=');
              dsk_ts0_expr_to_buffer (stanza->pieces[i].info.tag.args[a].expr, buffer);
            }
          dsk_buffer_append_string (buffer, " %>\n");
          dsk_ts0_stanza_dump (stanza->pieces[i].info.tag.body, indent + 4, buffer);
          dsk_buffer_append_repeated_byte (buffer, indent, ' ');
          dsk_buffer_append_string (buffer, "<%/ ");
          dsk_buffer_append_string (buffer, stanza->pieces[i].info.tag.tag_name);
          dsk_buffer_append_string (buffer, " %>");
          break;
        }
      dsk_buffer_append_byte (buffer, '\n');
    }
}

void          dsk_ts0_stanza_free       (DskTs0Stanza *stanza)
{
  unsigned i;
  for (i = 0; i < stanza->n_pieces; i++)
    dsk_ts0_stanza_piece_clear (stanza->pieces + i);
  dsk_free (stanza->pieces);
  dsk_free (stanza);
}

/* --- expressions --- */
typedef enum
{
  DSK_TS0_EXPR_LITERAL,
  DSK_TS0_EXPR_VARIABLE,
  DSK_TS0_EXPR_FUNCTION_CALL
} DskTs0ExprType;
struct _DskTs0Expr
{
  DskTs0ExprType type;
  DskTs0Filename *filename;
  unsigned line_no;
  union {
    struct {
      unsigned length;
      uint8_t *data;
    } literal;
    struct {
      char *name;
    } variable;
    struct {
      char *name;

      /* the 'function' itself is optional (it will be looked up from
         the name in the namespace); it may be set by the caching code,
         or other ways in the future. */
      DskTs0Function *function;

      unsigned n_args;
      DskTs0Expr **args;
    } function_call;
  } info;
};

typedef enum
{
  DSK_TS0_EXPR_TOKEN_TYPE_LPAREN,
  DSK_TS0_EXPR_TOKEN_TYPE_RPAREN,
  DSK_TS0_EXPR_TOKEN_TYPE_COMMA,
  DSK_TS0_EXPR_TOKEN_TYPE_DOLLAR,
  DSK_TS0_EXPR_TOKEN_TYPE_BAREWORD,
  DSK_TS0_EXPR_TOKEN_TYPE_QUOTED_STRING,
  DSK_TS0_EXPR_TOKEN_TYPE_NUMBER,
} DskTs0ExprTokenType;
static const char *_dsk_ts0_expr_token_type_names[] = {
  "left-paren '('",
  "right-paren ')'",
  "comma ','",
  "dollar '$'"
  "bareword",
  "quoted-string",
  "number"
};
static const char *
dsk_ts0_expr_token_type_name (DskTs0ExprTokenType type)
{
  dsk_assert (type <= DSK_TS0_EXPR_TOKEN_TYPE_NUMBER);
  return _dsk_ts0_expr_token_type_names[type];
}


typedef struct _CachedSubexpr CachedSubexpr;
struct _CachedSubexpr
{
  const char *start, *end;
  DskTs0Expr *expr;
  CachedSubexpr *next;
};
static CachedSubexpr *
reverse_cached_subexpr_list (CachedSubexpr *old_list)
{
  CachedSubexpr *head = NULL;
  while (old_list)
    {
      CachedSubexpr *n = old_list->next;
      old_list->next = head;
      head = old_list;
      old_list = n;
    }
  return head;
}

typedef struct _DskTs0ExprToken DskTs0ExprToken;
struct _DskTs0ExprToken
{
  DskTs0ExprTokenType type;
  const char *start, *end;
  unsigned start_line_no;

  /* for subexpressions within quoted-strings */
  CachedSubexpr *cached_subexpr_list;
};

static dsk_boolean
scan_number (const char *start,
             const char **end_out,
             DskTs0Filename *filename,
             unsigned line_no,
             DskError **error)
{
  const char *str = start;
  if (*str == '+' || *str == '-')
    str++;
  while (dsk_ascii_isdigit (*str))
    str++;
  if (*str == '.')
    {
      str++;
      while (dsk_ascii_isdigit (*str))
        str++;
    }
  if (*str == 'e' || *str == 'E')
    {
      str++;
      if (*str == '+' || *str == '-')
        str++;
      if (!dsk_ascii_isdigit (*str))
        {
          dsk_set_error (error,
                         "expected digit after 'e' in floating-point number at %s:%u",
                         FILENAME_STR (filename), line_no);
                         
        }
      str++;
      while (dsk_ascii_isdigit (*str))
        {
          str++;
        }
    }
  if (dsk_ascii_isalnum (*str) || *str == '_')
    {
      dsk_set_error (error,
                     "unexpected character %s after number at %s:%u",
                     dsk_ascii_byte_name (*str),
                     FILENAME_STR (filename), line_no);
      return DSK_FALSE;
    }
  *end_out = str;
  return DSK_TRUE;
}

static DskTs0Expr *parse_expr_from_tokens (DskTs0Filename *filename,
                                           unsigned        start_line_no,
                                           unsigned        n_tokens,
                                           DskTs0ExprToken *tokens,
                                           DskError       **error);

DskTs0Expr *dsk_ts0_expr_parse (const char *str,
                                const char **end_str_out,
                                DskTs0Filename *filename,
                                unsigned   *line_no_inout,
                                DskError  **error)
{
  /* --- lexing the string, scanning for end-of-expression --- */
  unsigned line_no = *line_no_inout;
  unsigned n_tokens = 0;
  unsigned tokens_alloced = 32;
  DskTs0ExprToken init_tokens[32];
  DskTs0ExprToken *tokens = init_tokens;

  dsk_boolean do_dollar_variable_interpolation = DSK_TRUE;      /* TODO: make tunable */

  /* we figure out the end of an expression at
     tokenization time by realizing that an expression
     must be:
         QUOTED_STRING
         NUMBER
         DOLLAR BAREWORD               [variable access]
         BAREWORD LPAREN ... RPAREN    [function call]
         LPAREN ... RPAREN             [parenthesized expr]
    where ... is an list of tokens whose have balanced parentheses.
  */
  int paren_balance = 0;
  for (;;)
    {
      DskTs0ExprToken token;
      skip_whitespace (&str, &line_no);
      if (*str == 0)
        {
          dsk_set_error (error, "expression too short %s:%u",
                         FILENAME_STR (filename), line_no);
          return NULL;
        }
      token.start = str;
      token.start_line_no = line_no;
      token.cached_subexpr_list = NULL;
      switch (*str)
        {
        case '(':
          token.type = DSK_TS0_EXPR_TOKEN_TYPE_LPAREN;
          token.end = str + 1;
          break;
        case '$':
          token.type = DSK_TS0_EXPR_TOKEN_TYPE_DOLLAR;
          token.end = str + 1;
          break;
        case ',':
          token.type = DSK_TS0_EXPR_TOKEN_TYPE_COMMA;
          token.end = str + 1;
          break;
        case ')':
          token.type = DSK_TS0_EXPR_TOKEN_TYPE_RPAREN;
          token.end = str + 1;
          break;
        case '"':
          token.type = DSK_TS0_EXPR_TOKEN_TYPE_QUOTED_STRING;
          str++;
          while (*str != '"')
            {
              if (*str == '\0')
                {
                  dsk_set_error (error, "premature end-of-file in quoted-string starting at %s:%u",
                                 FILENAME_STR (filename), token.start_line_no);
                  goto error_cleanup;
                }
              else if (*str == '\\')
                {
                  if (str[1] == '\0')
                    {
                      dsk_set_error (error, "premature end-of-file in quoted-string starting at %s:%u, after backslash",
                                     FILENAME_STR (filename), token.start_line_no);
                      goto error_cleanup;
                    }
                  str++;                /* skip an extra byte */
                }
              else if (*str == '\n')
                line_no++;
              else if (do_dollar_variable_interpolation && *str == '$' && str[1] == '(')
                {
                  const char *end_expr;
                  DskTs0Expr *subexpr = dsk_ts0_expr_parse (str + 1, &end_expr,
                                                            filename, &line_no,
                                                            error);
                  if (subexpr == NULL)
                    {
                      dsk_add_error_suffix (error, " in quoted-string starting at %s:%u",
                                            FILENAME_STR (filename), token.start_line_no);
                      goto error_cleanup;
                    }
                  CachedSubexpr *c = DSK_NEW (CachedSubexpr);
                  c->start = str + 1;
                  c->end = end_expr;
                  c->expr = subexpr;
                  c->next = token.cached_subexpr_list;
                  token.cached_subexpr_list = c;
                  str = end_expr;
                }
              str++;
            }
          str++;                /* skip trailing double-quote */
          token.end = str;
          token.cached_subexpr_list = reverse_cached_subexpr_list (token.cached_subexpr_list);
          break;
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        case '+': case '-': case '.':
          token.type = DSK_TS0_EXPR_TOKEN_TYPE_NUMBER;
          if (!scan_number (str, &token.end, filename, line_no, error))
            goto error_cleanup;
          break;
        default:
          /* handle bareword */
          if (!dsk_ascii_isalnum (*str) && *str != '_')
            {
              dsk_set_error (error, "unexpected character %s in string-expression, %s:%u",
                             dsk_ascii_byte_name (*str),
                             FILENAME_STR (filename), line_no);
              goto error_cleanup;
            }
          token.type = DSK_TS0_EXPR_TOKEN_TYPE_BAREWORD;
          while (dsk_ascii_isalnum (*str) || *str == '_')
            str++;
          token.end = str;
          break;
        }
      str = token.end;

      /* append token to array */
      if (n_tokens == tokens_alloced)
        {
          unsigned old_size = tokens_alloced * sizeof (DskTs0ExprToken);
          tokens_alloced *= 2;
          unsigned new_size = tokens_alloced * sizeof (DskTs0ExprToken);
          if (tokens == init_tokens)
            {
              tokens = dsk_malloc (new_size);
              memcpy (tokens, init_tokens, old_size);
            }
          else
            tokens = dsk_realloc (tokens, new_size);
        }
      tokens[n_tokens++] = token;

      if (n_tokens == 1)
        {
          if (tokens[0].type == DSK_TS0_EXPR_TOKEN_TYPE_QUOTED_STRING
           || tokens[0].type == DSK_TS0_EXPR_TOKEN_TYPE_NUMBER)
            break;
          if (tokens[0].type != DSK_TS0_EXPR_TOKEN_TYPE_DOLLAR
           && tokens[0].type != DSK_TS0_EXPR_TOKEN_TYPE_BAREWORD
           && tokens[0].type != DSK_TS0_EXPR_TOKEN_TYPE_LPAREN)
            {
              dsk_set_error (error, "unexpected token type %s at %s:%u",
                             dsk_ts0_expr_token_type_name (tokens[0].type),
                             FILENAME_STR (filename), line_no);
              goto error_cleanup;
            }
        }
      else if (n_tokens == 2)
        {
          if (tokens[0].type == DSK_TS0_EXPR_TOKEN_TYPE_DOLLAR
           && tokens[1].type == DSK_TS0_EXPR_TOKEN_TYPE_BAREWORD)
            break;
          if (tokens[0].type == DSK_TS0_EXPR_TOKEN_TYPE_BAREWORD
           && tokens[1].type == DSK_TS0_EXPR_TOKEN_TYPE_LPAREN)
            {
              /* beginning of function call */
            }
          else if (tokens[0].type == DSK_TS0_EXPR_TOKEN_TYPE_LPAREN)
            {
              if (paren_balance == 0)
                {
                  dsk_set_error (error, "empty parenthesized expression not allowed (ie encountered \"()\") at %s:%u",
                                 FILENAME_STR (filename), tokens[0].start_line_no);
                  goto error_cleanup;
                }
            }
          else
            {
              dsk_set_error (error, "parsing expression starting at %s:%u (bad tokens %s %s)",
                             FILENAME_STR (filename), tokens[0].start_line_no,
                             dsk_ts0_expr_token_type_name (tokens[0].type),
                             dsk_ts0_expr_token_type_name (tokens[1].type));
              goto error_cleanup;
            }
        }
      else if (paren_balance == 0)
        {
          break;
        }
    }
  *end_str_out = str;
  *line_no_inout = line_no;

  /* --- parse tokens --- */
  DskTs0Expr *rv;
  dsk_assert (n_tokens > 0);
  rv = parse_expr_from_tokens (filename, tokens[0].start_line_no,
                               n_tokens, tokens, error);
  if (tokens != init_tokens)
    dsk_free (tokens);
  return rv;

error_cleanup:
  {
    unsigned i;
    for (i = 0; i < n_tokens; i++)
      if (tokens[i].type == DSK_TS0_EXPR_TOKEN_TYPE_QUOTED_STRING)
        {
          while (tokens[i].cached_subexpr_list != NULL)
            {
              CachedSubexpr *c = tokens[i].cached_subexpr_list;
              tokens[i].cached_subexpr_list = c->next;
              dsk_ts0_expr_free (c->expr);
              dsk_free (c);
            }
        }
  }
  if (tokens != init_tokens)
    dsk_free (tokens);
  return NULL;
}

static void
dsk_ts0_expr__find_next_comma (unsigned n_tokens, 
                               DskTs0ExprToken *tokens,
                               unsigned *at_inout)
{
  unsigned at = *at_inout;
  unsigned paren_balance = 0;
  while (at < n_tokens)
    {
      switch (tokens[at].type)
        {
        case DSK_TS0_EXPR_TOKEN_TYPE_LPAREN:
          paren_balance += 1;
          break;
        case DSK_TS0_EXPR_TOKEN_TYPE_RPAREN:
          dsk_assert (paren_balance != 0);
          paren_balance -= 1;
          break;
        case DSK_TS0_EXPR_TOKEN_TYPE_COMMA:
          *at_inout = at;
          return;
        default:
          break;
        }
    }
  *at_inout = at;
}

static unsigned
scan_backslash_expr (const char *start,
                     const char *max_end,
                     DskBuffer  *dst,
                     DskError  **error)
{
  if (start == max_end)
    {
      dsk_set_error (error, "end of string at \\ sequence");
      return 0;
    }
  switch (*start)
    {
      /* allow any punctuation to be backslashed */
    case '"': case '\\': case '\'': case '?':
    case '!': case '@': case '#': case '$': case '%':
    case '^': case '&': case '*': case '(': case ')':
    case ';': case ':': case '[': case ']': case '~':
    case '`': case '=': case '+': case '-': case '_':
    case '{': case '}': case ',': case '.': case '/':
    case '<': case '>':
      dsk_buffer_append_byte (dst, *start);
      return 1;
    case '0': case '1': case '2': case '3':
    case '4': case '5': case '6': case '7':
      {
        unsigned n_digits = 1;
        unsigned value = *start - '0';
        while (n_digits < 3
            && start + n_digits < max_end
            && ('0' <= start[n_digits] && start[n_digits] <= '7'))
          {
            value <<= 3;
            value += start[n_digits] - '0';
            n_digits++;
          }
        dsk_buffer_append_byte (dst, value);
        return n_digits;
      }
    case 'n':
      dsk_buffer_append_byte (dst, '\n');
      return 1;
    case 't':
      dsk_buffer_append_byte (dst, '\t');
      return 1;
    case 'r':
      dsk_buffer_append_byte (dst, '\r');
      return 1;
    default:
      dsk_set_error (error, "unexpected byte %s after \\",
                     dsk_ascii_byte_name (*start));
      return 0;
    }
}

/* TODO: needs flags passed in for do_dollar_variable_interpolation and permit_multiline_quoted_string */
static DskTs0Expr *
parse_expr_from_tokens (DskTs0Filename *filename,
                        unsigned        start_line_no,
                        unsigned    n_tokens,
                        DskTs0ExprToken *tokens,
                        DskError   **error)
{
  dsk_boolean permit_multiline_quoted_string = DSK_FALSE;               /* TODO: tunable */
restart_parse:
  if (n_tokens == 0)
    {
      dsk_set_error (error, "empty expression not allowed %s:%u",
                     FILENAME_STR (filename), start_line_no);
      return NULL;
    }
  switch (tokens[0].type)
    {
    case DSK_TS0_EXPR_TOKEN_TYPE_LPAREN:
      if (tokens[n_tokens-1].type != DSK_TS0_EXPR_TOKEN_TYPE_RPAREN)
        {
          dsk_set_error (error, "ts0-expr internal error: subexpression paren mismatch? %s:%u (end token %s at line %u)",
                         FILENAME_STR (filename), tokens[0].start_line_no,
                         dsk_ts0_expr_token_type_name (tokens[n_tokens-1].type),
                         tokens[n_tokens-1].start_line_no);
          return NULL;
        }
      n_tokens -= 2;
      tokens += 1;
      goto restart_parse;
    case DSK_TS0_EXPR_TOKEN_TYPE_RPAREN:
      dsk_set_error (error, "did not expect ')': probably a program error (at %s:%u)",
                    FILENAME_STR (filename), tokens[0].start_line_no);
      return NULL;
    case DSK_TS0_EXPR_TOKEN_TYPE_COMMA:
      dsk_set_error (error, "did not expect ',' at %s:%u",
                     FILENAME_STR (filename), start_line_no);
      return NULL;
    case DSK_TS0_EXPR_TOKEN_TYPE_DOLLAR:
      if (n_tokens != 2 || tokens[1].type != DSK_TS0_EXPR_TOKEN_TYPE_BAREWORD)
        {
          dsk_set_error (error, "expected bareword after '$', got %s, at %s:%u",
                         dsk_ts0_expr_token_type_name (tokens[1].type),
                         FILENAME_STR (filename), tokens[1].start_line_no);
          return NULL;
        }
      return dsk_ts0_expr_new_variable (filename, tokens[1].start_line_no,
                                        tokens[1].end - tokens[1].start,
                                        tokens[1].start);
    case DSK_TS0_EXPR_TOKEN_TYPE_BAREWORD:
      if (n_tokens < 2)
        {
          dsk_set_error (error, "naked bareword '%.*s' is not an expression at %s:%u",
                         (int)(tokens[0].end - tokens[0].start),
                         tokens[0].start,
                         FILENAME_STR (filename), tokens[0].start_line_no);
          return NULL;
        }
      if (tokens[1].type != DSK_TS0_EXPR_TOKEN_TYPE_LPAREN)
        {
          dsk_set_error (error, "expected '(' after bareword '%.*s', got %s at %s:%u",
                         (int)(tokens[0].end - tokens[0].start),
                         tokens[0].start,
                         dsk_ts0_expr_token_type_name (tokens[1].type),
                         FILENAME_STR (filename), tokens[0].start_line_no);
          return NULL;
        }
      if (tokens[n_tokens - 1].type != DSK_TS0_EXPR_TOKEN_TYPE_RPAREN)
        {
          dsk_set_error (error, "function call to %.*s beginning at line %u did not end with a ')' (ended with %s) at %s:%u",
                         (int)(tokens[0].end - tokens[0].start),
                         tokens[0].start,
                         tokens[0].start_line_no,
                         dsk_ts0_expr_token_type_name (tokens[n_tokens - 1].type),
                         FILENAME_STR (filename), tokens[0].start_line_no);
          return NULL;
        }

      /* function call */
      {
        /* count arguments */
        unsigned n_commas = 0;
        unsigned at = 0;
        unsigned n_args_tokens = n_tokens - 3;
        DskTs0ExprToken *args_tokens = tokens + 2;
        for (;;)
          {
            dsk_ts0_expr__find_next_comma (n_args_tokens, args_tokens, &at);
            if (at < n_args_tokens)
              {
                n_commas++;
                at++;
              }
            else
              break;
          }
        unsigned n_args = n_tokens == 3 ? 0 : (n_commas + 1);

        dsk_assert (n_args < 4000);             /* since we alloca next line */
        DskTs0Expr **args = alloca (sizeof (DskTs0Expr *) * n_args);

        /* gather arguments */
        at = 0;
        unsigned args_at = 0;
        while (at < n_args_tokens)
          {
            unsigned start_at = at;
            dsk_ts0_expr__find_next_comma (n_args_tokens, args_tokens, &at);
            if (at > start_at)
              {
                DskTs0Expr *arg_expr = parse_expr_from_tokens (filename, args_tokens[start_at].start_line_no,
                                                               at - start_at, args_tokens + start_at,
                                                               error);
                args[args_at] = arg_expr;
                args_at++;
              }
            if (at == n_args_tokens)
              break;
            else if (at < n_args_tokens && args_tokens[at].type == DSK_TS0_EXPR_TOKEN_TYPE_COMMA)
              at++;
            else
              break;            /* can this happpen???? */
          }

        /* create expression */
        DskTs0Expr *rv;
        rv = dsk_ts0_expr_new_function (filename, tokens[0].start_line_no,
                                        tokens[0].end - tokens[0].start,
                                        tokens[0].start,
                                        n_args, args);
        return rv;
      }

    case DSK_TS0_EXPR_TOKEN_TYPE_QUOTED_STRING:
      if (n_tokens != 1)
        {
          dsk_set_error (error, "unexpected token %s after quoted-string %s:%u",
                         dsk_ts0_expr_token_type_name (tokens[1].type),
                         FILENAME_STR (filename), tokens[1].start_line_no);
          return NULL;
        }
      const char *iq_start = tokens[0].start + 1;
      const char *iq_end = tokens[0].end - 1;
      const char *at = iq_start;
      DskBuffer dst = DSK_BUFFER_INIT;
      dsk_boolean do_dollar_variable_interpolation = DSK_TRUE; /* TODO: tunable */
      unsigned line_no = tokens[0].start_line_no;
      unsigned max_pieces = 1;
      if (do_dollar_variable_interpolation)
        {
          unsigned n_dollar = 0;
          while (at < iq_end)
            {
              if (*at == '$')
                {
                  if (at + 1 < iq_end && at[1] == '$')
                    at += 1;
                  else
                    n_dollar++;
                }
              at++;
            }
          max_pieces = n_dollar * 2 + 1;
        }

      /* TODO: what if max_pieces is too large for alloca() !!! */
      DskTs0Expr **pieces = alloca (sizeof (DskTs0Expr*) * max_pieces);
      unsigned n_pieces = 0;

      while (at < iq_end)
        {
          if (do_dollar_variable_interpolation && *at == '$')
            {
              if (at[1] == '$')
                {
                  dsk_buffer_append_byte (&dst, '$');
                  at += 2;
                }
              else
                {
                  if (dst.size > 0)
                    {
                      uint8_t *slab = dsk_malloc (dst.size);
                      unsigned len = dst.size;
                      dsk_buffer_read (&dst, len, slab);
                      pieces[n_pieces] = dsk_ts0_expr_new_literal (filename, line_no, len, slab);
                      n_pieces++;
                    }

                  /* variable (or subexpression) */
                  if (at + 1 == iq_end)
                    {
                      dsk_set_error (error, "unexpected end-of-string after '$' at %s:%u",
                                     FILENAME_STR (filename), line_no);
                      goto quoted_string_error;
                    }
                  else if (at[1] == '(')
                    {
                      /* subexpression */
                      dsk_assert (tokens[0].cached_subexpr_list != NULL);
                      CachedSubexpr *c = tokens[0].cached_subexpr_list;
                      tokens[0].cached_subexpr_list = c->next;
                      dsk_assert (c->start == at + 1);
                      pieces[n_pieces] = c->expr;
                      dsk_free (c);
                      n_pieces++;
                    }
                  else if (dsk_ascii_isalnum (at[1]) || at[1] == '_')
                    {
                      /* variable interpolation */
                      const char *start_name = at + 1;
                      const char *end_name = start_name + 1;
                      while (dsk_ascii_isalnum (*end_name) || *end_name == '_')
                        end_name++;
                      pieces[n_pieces] = dsk_ts0_expr_new_variable (filename, line_no,
                                                                    end_name - start_name,
                                                                    start_name);
                      n_pieces++;
                      at = end_name;
                      dsk_assert (at <= iq_end);
                    }
                  else
                    {
                      dsk_set_error (error, "unexpected character %s after '$' at %s:%u",
                                     dsk_ascii_byte_name (at[1]),
                                     FILENAME_STR (filename), line_no);
                      goto quoted_string_error;
                    }
                }
              continue;
            }
          if (*at == '\n')
            {
              if (!permit_multiline_quoted_string)
                {
                  dsk_set_error (error, "unexpected newline in quoted-string at %s:%u",
                                 FILENAME_STR (filename), line_no);
                  goto quoted_string_error;
                }
              line_no++;
            }

          if (*at == '\\')
            {
              at++;
              unsigned len = scan_backslash_expr (at, iq_end, &dst, error);
              if (len == 0)
                {
                  dsk_add_error_suffix (error, " at %s:%u",
                                        FILENAME_STR (filename), line_no);
                  goto quoted_string_error;
                }
              at += len;
            }
          else
            {
              dsk_buffer_append_byte (&dst, *at);
              at++;
            }
        }
      if (0)
        {
quoted_string_error:
          dsk_buffer_clear (&dst);
          unsigned i;
          for (i = 0; i < n_pieces; i++)
            dsk_ts0_expr_free (pieces[i]);
          return NULL;
        }
      if (dst.size > 0)
        {
          uint8_t *slab = dsk_malloc (dst.size);
          unsigned len = dst.size;
          dsk_buffer_read (&dst, len, slab);
          pieces[n_pieces] = dsk_ts0_expr_new_literal (filename, line_no, len, slab);
          n_pieces++;
        }
      if (n_pieces == 0)
        {
          return dsk_ts0_expr_new_literal (filename, line_no, 0, NULL);
        }
      else if (n_pieces == 1)
        {
          return pieces[0];
        }
      else
        {
          /* create concatenation */
          return dsk_ts0_expr_new_function_anon (filename, tokens[0].start_line_no,
                                                 &dsk_ts0_function_concat, n_pieces, pieces);
        }

    case DSK_TS0_EXPR_TOKEN_TYPE_NUMBER:
      if (n_tokens != 1)
        {
          dsk_set_error (error, "unexpected token %s after number at %s:%u",
                         dsk_ts0_expr_token_type_name (tokens[1].type),
                         FILENAME_STR (filename), tokens[1].start_line_no);
          return NULL;
        }
      unsigned len = tokens[0].end - tokens[0].start;
      return dsk_ts0_expr_new_literal (filename, start_line_no,
                                       len, dsk_memdup (len, tokens[0].start));
    }
  dsk_return_val_if_reached ("unhandled expression-token-type", NULL);
}

static void        dsk_ts0_expr_to_buffer (DskTs0Expr *expr,
                                           DskBuffer  *out)
{
  switch (expr->type)
    {
    case DSK_TS0_EXPR_LITERAL:
      dsk_filter_to_buffer (expr->info.literal.length,
                            expr->info.literal.data,
                            dsk_octet_filter_chain_new_take_list (
                              dsk_byte_doubler_new ('$'),
                              dsk_c_quoter_new (DSK_TRUE, DSK_TRUE),
                              NULL
                            ),
                            out,
                            NULL);
      break;
    case DSK_TS0_EXPR_VARIABLE:
      dsk_buffer_append_byte (out, '$');
      dsk_buffer_append_string (out, expr->info.variable.name);
      break;
    case DSK_TS0_EXPR_FUNCTION_CALL:
      if (expr->info.function_call.name == NULL)
        dsk_buffer_append_string (out, "*anon_function*");
      else
        dsk_buffer_append_string (out, expr->info.function_call.name);
      dsk_buffer_append_byte (out, '(');
      unsigned i;
      for (i = 0; i < expr->info.function_call.n_args; i++)
        {
          if (i > 0)
            dsk_buffer_append_string (out, ", ");
          dsk_ts0_expr_to_buffer (expr->info.function_call.args[i], out);
        }
      dsk_buffer_append_byte (out, ')');
      break;
    }
}

/* note: takes ownership of data */
static DskTs0Expr *
dsk_ts0_expr_new_literal (DskTs0Filename *filename,
                          unsigned        line_no,
                          unsigned        len,
                          uint8_t        *data)
{
  DskTs0Expr *rv = dsk_malloc (sizeof (DskTs0Expr) + len);
  rv->filename = dsk_ts0_filename_ref (filename);
  rv->line_no = line_no;
  rv->type = DSK_TS0_EXPR_LITERAL;
  memcpy (rv + 1, data, len);
  return rv;
}
static DskTs0Expr *
dsk_ts0_expr_new_variable (DskTs0Filename *filename,
                           unsigned        line_no,
                           unsigned        length,
                           const char     *name)
{
  DskTs0Expr *rv = dsk_malloc (sizeof (DskTs0Expr) + length + 1);
  rv->filename = dsk_ts0_filename_ref (filename);
  rv->line_no = line_no;
  rv->type = DSK_TS0_EXPR_VARIABLE;
  rv->info.variable.name = memcpy ((char *) (rv + 1), name, length);
  rv->info.variable.name[length] = 0;
  return rv;
}

static DskTs0Expr *
dsk_ts0_expr_new_function (DskTs0Filename *filename,
                           unsigned        line_no,
                           unsigned name_len,
                           const char *name,
                           unsigned    n_args,
                           DskTs0Expr **args)
{
  unsigned size = sizeof (DskTs0Expr)
                + n_args * sizeof (DskTs0Expr*)
                + name_len + 1;
  DskTs0Expr *rv = dsk_malloc (size);
  rv->filename = dsk_ts0_filename_ref (filename);
  rv->line_no = line_no;
  rv->type = DSK_TS0_EXPR_FUNCTION_CALL;
  rv->info.function_call.n_args = n_args;
  rv->info.function_call.args = memcpy (rv + 1, args, n_args * sizeof (DskTs0Expr*));
  rv->info.function_call.name = (char*)(rv->info.function_call.args + n_args);
  memcpy (rv->info.function_call.name, name, name_len);
  rv->info.function_call.name[name_len] = '\0';
  rv->info.function_call.function = NULL;
  return rv;
}

static DskTs0Expr *
dsk_ts0_expr_new_function_anon (DskTs0Filename *filename,
                           unsigned        line_no,
                           DskTs0Function *function,
                           unsigned    n_args,
                           DskTs0Expr **args)
{
  unsigned size = sizeof (DskTs0Expr)
                + n_args * sizeof (DskTs0Expr*);
  DskTs0Expr *rv = dsk_malloc (size);
  rv->filename = dsk_ts0_filename_ref (filename);
  rv->line_no = line_no;
  rv->type = DSK_TS0_EXPR_FUNCTION_CALL;
  rv->info.function_call.n_args = n_args;
  rv->info.function_call.args = memcpy (rv + 1, args, n_args * sizeof (DskTs0Expr*));
  rv->info.function_call.name = NULL;
  rv->info.function_call.function = dsk_ts0_function_ref (function);
  return rv;
}

static void
dsk_ts0_expr_free (DskTs0Expr *expr)
{
  switch (expr->type)
    {
    case DSK_TS0_EXPR_VARIABLE:
      break;
    case DSK_TS0_EXPR_LITERAL:
      break;
    case DSK_TS0_EXPR_FUNCTION_CALL:
      if (expr->info.function_call.function != NULL)
        dsk_ts0_function_unref (expr->info.function_call.function);
      unsigned i;
      for (i = 0; i < expr->info.function_call.n_args; i++)
        dsk_ts0_expr_free (expr->info.function_call.args[i]);
      break;
    }
  dsk_ts0_filename_unref (expr->filename);
  dsk_free (expr);
}

static void
add_expr_error_suffix (DskError **error,
                       DskTs0Expr *expr)
{
  dsk_add_error_suffix (error,
                        "at %s:%u",
                        FILENAME_STR (expr->filename),
                        expr->line_no);
}

dsk_boolean dsk_ts0_expr_evaluate (DskTs0Expr *expr,
                                   DskTs0Namespace *ns,
                                   DskBuffer  *target,
                                   DskError  **error)
{
  switch (expr->type)
    {
    case DSK_TS0_EXPR_VARIABLE:
      {
        const char *str = dsk_ts0_namespace_get_variable (ns, expr->info.variable.name, error);
        if (str == NULL)
          {
            add_expr_error_suffix (error, expr);
            return DSK_FALSE;
          }
        dsk_buffer_append_string (target, str);
        break;
      }
    case DSK_TS0_EXPR_LITERAL:
      dsk_buffer_append (target, expr->info.literal.length, expr->info.literal.data);
      break;
    case DSK_TS0_EXPR_FUNCTION_CALL:
      {
        DskTs0Function *func = expr->info.function_call.function;
        if (func == NULL)
          {
            func = dsk_ts0_namespace_get_function (ns, expr->info.function_call.name, error);
            if (func == NULL)
              {
                add_expr_error_suffix (error, expr);
                return DSK_FALSE;
              }
            if (func->cachable)
              expr->info.function_call.function = dsk_ts0_function_ref (func);
          }
        if (!func->invoke (func, ns,
                           expr->info.function_call.n_args,
                           expr->info.function_call.args, 
                           target, error))
          {
            add_expr_error_suffix (error, expr);
            return DSK_FALSE;
          }
        break;
      }
    }
  return DSK_TRUE;
}

