#include <string.h>
#define DSK_INCLUDE_TS0
#include "dsk.h"
#include "gskrbtreemacros.h"

#define MAX_TAG_DEPTH           64

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
                DskBuffer buffer = DSK_BUFFER_STATIC_INIT;
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
  char *contents = dsk_file_get_contents (filename, NULL, error);
  if (contents == NULL)
    return NULL;
  rv = dsk_ts0_stanza_parse_str (contents, filename, 1, error);
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
  *n_inout += 2;
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

static DskTs0Expr *_dsk_ts0_expr_make_var (unsigned name_len, const char *name);


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
     Control with +/-strip_leading_ws, +/-strip_trailing_ws
     or +/-strip_ws to set both at once. */
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
              if (!dsk_ts0_expr_parse (str, &end, filename, &line_no, error))
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
              piece.info.expression = _dsk_ts0_expr_make_var (str - start, str);
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
                  named_expr.expr = dsk_ts0_expr_parse (str, &end, filename, &line_no, error);
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
          new_substanza = dsk_malloc (sizeof (DskTs0Stanza));
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
          expr = dsk_ts0_expr_parse (str, &end, filename, &line_no, error);
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
              else if (slice_matches (name_start, name_end, "strip_ws"))
                strip_leading_whitespace = strip_trailing_whitespace = enable;
              else if (slice_matches (name_start, name_end, "strip_trailing_ws"))
                strip_trailing_whitespace = enable;
              else if (slice_matches (name_start, name_end, "strip_leading_ws"))
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

  DskTs0Stanza *rv = dsk_malloc (sizeof (DskTs0Stanza));
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

DskTs0Expr *_dsk_ts0_expr_make_var (unsigned length, const char *name)
{
  DskTs0Expr *rv = dsk_malloc (sizeof (DskTs0Expr));
  rv->type = DSK_TS0_EXPR_VARIABLE;
  rv->info.variable.name = dsk_malloc (length + 1);
  memcpy (rv->info.variable.name, name, length);
  rv->info.variable.name[length] = 0;
  return rv;
}


DskTs0Expr *dsk_ts0_expr_parse (const char *str,
                                const char **end_str_out,
                                const char *filename,
                                unsigned   *line_no_inout,
                                DskError  **error)
{
...
}
