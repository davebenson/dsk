#include <string.h>
#define DSK_INCLUDE_TS0
#include "dsk.h"
#include "gskrbtreemacros.h"

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
  GSK_STD_GET_IS_RED, GSK_STD_SET_IS_RED, \
  parent, left, right, COMPARE_SCOPE_NODES

static DskTs0NamespaceNode *
lookup_namespace_node (DskTs0NamespaceNode *top,
                   const char      *key)
{
  DskTs0NamespaceNode *rv;
  GSK_RBTREE_LOOKUP_COMPARATOR (GET_SCOPE_NODE_TREE (top), key,
                                COMPARE_STRING_TO_SCOPE_NODE, rv);
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
  GSK_RBTREE_INSERT (GET_SCOPE_NODE_TREE ((*top_ptr)), rv, conflict);
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
  DskTs0Namespace *rv = dsk_malloc (sizeof (DskTs0Namespace));
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
                                  DskTs0GlobalLazyFunc lazy_func)
{
  DskTs0Function *function = dsk_ts0_function_new_global_lazy (lazy_func);
  dsk_ts0_global_take_function (name, function);
}

void
dsk_ts0_global_add_lazy_func_data  (const char              *name,
                                    DskTs0GlobalLazyDataFunc lazy_func,
                                    void                    *func_data,
                                    DskDestroyNotify         destroy)
{
  DskTs0Function *function;
  function = dsk_ts0_function_new_global_lazy_data (lazy_func, func_data, destroy);
  dsk_ts0_global_take_function (name, function);
}

void
dsk_ts0_global_add_func(const char *name, DskTs0GlobalFunc func)
{
  DskTs0Function *function;
  function = dsk_ts0_function_new_global (func);
  dsk_ts0_global_take_function (name, function);
}

void
dsk_ts0_global_add_func_data            (const char *name,
                                              DskTs0GlobalDataFunc func,
                                              void *func_data,
                                              DskDestroyNotify destroy)
{
  DskTs0Function *function;
  function = dsk_ts0_function_new_global_data (func, func_data, destroy);
  dsk_ts0_global_take_function (name, function);
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
    switch (stanza->pieces[i].type)
      {
      case DSK_TS0_STANZA_PIECE_LITERAL:
        dsk_buffer_append (target,
                           stanza->pieces[i].info.literal.length,
                           stanza->pieces[i].info.literal.data);
        break;
      case DSK_TS0_STANZA_PIECE_EXPRESSION:
        if (!dsk_ts0_expr_evaluate (stanza->pieces[i].info.expression,
                                    ns, target, error))
          {
            ...
          }
        break;
      case DSK_TS0_STANZA_PIECE_TAG:
        ...
        break;
      }
}

DskTs0Stanza *dsk_ts0_stanza_parse_file (const char   *filename,
                                         DskError    **error)
{
  DskTs0Stanza *rv;
  char *contents = dsk_file_get_contents (filename, NULL, error);
  if (contents == NULL)
    return NULL;
  rv = dsk_ts0_stanza_parse_str (str, filename, 1, error);
  dsk_free (contents);
  return rv;
}

DskTs0Stanza *dsk_ts0_stanza_parse_str  (const char   *str,
                                         const char   *filename,
                                         unsigned      line_no,
                                         DskError    **error)
{
#if 0
  <% %> <%/ %>
  <%( $foo )%>
#endif
  for (;;)
    {
      /* try making a literal */
      const char *start = str;
      unsigned n_double_dollars = 0;
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
      if (str > start)
        {
          if (n_double_dollars > 0)
            {
              /* literal with dollar characters removed */
              ...
            }
          else
            {
              /* simple literal */
              ...
            }
        }
      
      if (*str == 0)
        {
          break;
        }
      if (*str == '$')
        {
          /* variable expression */
          ...
        }

      /* at <% */
      str += 2;
      skip_whitespace (&str, &line_no);

      if (dsk_ascii_isalnum (*str) || *str == '_')
        {
          /* tag opening */
          ...
        }
      else if (*str == '/')
        {
          /* tag closing */
          ...
        }
      else if (*str == '(')
        {
          /* expression */
          ...
        }
      else if (*str == '+' || *str == '-')
        {
          /* tokenizer / parser flags */
          ...
        }
      else
        {
          dsk_set_error (error, "unexpected <%% %c at %s:%u",
                         *str, filename, line_no);
          return NULL;
        }
    }

  return 
}

void          dsk_ts0_stanza_free       (DskTs0Stanza *stanza)
{
  ...
}

/* --- expressions --- */
struct _DskTs0Expr
{
  DskTs0ExprType type;
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
      DskTs0Function *function; /* optional */
      unsigned n_args;
      DskTs0Expr **args;
    } function_call;
  } info;
};

DskTs0Expr *dsk_ts0_expr_parse (const char *str,
                                const char **end_str_out,
                                const char *filename,
                                unsigned   *line_no_inout,
                                DskError  **error)
{
...
};
