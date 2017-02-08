#define _POSIX_C_SOURCE  2
#include <string.h>
#include <stdio.h>
#include "dsk-rbtree-macros.h"
#include "dsk.h"

typedef struct _VarDef VarDef;
typedef struct _StackNode StackNode;

struct _VarDef
{
  const char *key;		/* value immediately follows structure */
  VarDef *next_in_stack_node;
  VarDef *next_with_same_key;

  /* Only valid if VarDef is in the tree */
  VarDef *left, *right, *parent;
  dsk_boolean is_red;

  unsigned value_length;
  /* value follows */
};
#define COMPARE_VAR_DEFS(a,b, rv) rv = strcmp(a->key, b->key)
#define GET_VARDEF_TREE(context) \
  (context)->tree, VarDef *, DSK_STD_GET_IS_RED, DSK_STD_SET_IS_RED, \
  parent, left, right, COMPARE_VAR_DEFS

struct _StackNode
{
  StackNode *prev;
  VarDef *vars;
};

struct _DskPrint
{
  /* output */
  DskPrintAppendFunc append;
  void              *append_data;
  DskDestroyNotify   append_destroy;

  /* dynamically-scoped variables */
  StackNode *top;
  StackNode bottom;

  /* variables, by name */
  VarDef *tree;
};
DskPrint *dsk_print_new    (DskPrintAppendFunc append,
                            void              *data,
			    DskDestroyNotify   destroy)
{
  DskPrint *rv = DSK_NEW (DskPrint);
  rv->append = append;
  rv->append_data = data;
  rv->append_destroy = destroy;
  rv->top = &rv->bottom;
  rv->bottom.prev = NULL;
  rv->bottom.vars = NULL;
  rv->tree = NULL;
  return rv;
}
void      dsk_print_free   (DskPrint *print)
{
  for (;;)
    {
      /* free all elements in top */
      StackNode *stack = print->top;
      print->top = stack->prev;
      while (stack->vars)
        {
          VarDef *v = stack->vars;
          stack->vars = v->next_in_stack_node;
          dsk_free (v);
        }
      if (print->top == NULL)
        break;
      dsk_free (stack);
    }
  if (print->append_destroy)
    print->append_destroy (print->append_data);
  dsk_free (print);
}

static DskPrint *
get_default_context (void)
{
  static DskPrint *default_context = NULL;
  if (default_context == NULL)
    default_context = dsk_print_new_fp (stderr);
  return default_context;
}

static void
add_var_def (DskPrint *context,
             VarDef   *def)
{
  VarDef *existing;
  if (context == NULL)
    context = get_default_context ();
  def->next_in_stack_node = context->top->vars;
  context->top->vars = def;

  DSK_RBTREE_INSERT (GET_VARDEF_TREE (context), def, existing);
  if (existing)
    DSK_RBTREE_REPLACE_NODE (GET_VARDEF_TREE (context), existing, def);
  def->next_with_same_key = existing;
}


void dsk_print_set_string          (DskPrint    *context,
                                    const char  *variable_name,
			            const char  *value)
{
  unsigned key_len = strlen (variable_name);
  unsigned value_len = strlen (value);
  VarDef *vd = dsk_malloc (sizeof (VarDef) + key_len + 1 + value_len + 1);
  vd->key = strcpy (dsk_stpcpy ((char*)(vd+1), value) + 1, variable_name);
  vd->value_length = value_len;
  add_var_def (context, vd);
}
void dsk_print_set_string_length   (DskPrint    *context,
                                    const char  *variable_name,
                                    unsigned     value_len,
			            const char  *value)
{
  unsigned key_len = strlen (variable_name);
  VarDef *vd = dsk_malloc (sizeof (VarDef) + key_len + 1 + value_len + 1);
  memcpy (vd + 1, value, value_len);
  ((char*)(vd+1))[value_len] = 0;
  vd->key = strcpy ((char*)(vd+1) + value_len + 1, variable_name);
  vd->value_length = value_len;
  add_var_def (context, vd);
}
void dsk_print_set_dsk_buffer      (DskPrint    *context,
                                    const char  *variable_name,
                                    const DskBuffer *buffer)
{
  unsigned key_len = strlen (variable_name);
  VarDef *vd = dsk_malloc (sizeof (VarDef) + key_len + 1 + buffer->size + 1);
  dsk_buffer_peek (buffer, buffer->size, vd + 1);
  ((char*)(vd+1))[buffer->size] = 0;
  vd->key = strcpy ((char*)(vd+1) + buffer->size + 1, variable_name);
  vd->value_length = buffer->size;
  add_var_def (context, vd);
}

void dsk_print_set_int             (DskPrint    *context,
                                    const char  *variable_name,
                                    int          value)
{
  char buf[32];
  snprintf (buf, sizeof (buf), "%d", value);
  dsk_print_set_string (context, variable_name, buf);
}

void dsk_print_set_uint (DskPrint *context,
                         const char *variable_name,
                         unsigned value)
{
  char buf[32];
  snprintf (buf, sizeof (buf), "%u", value);
  dsk_print_set_string (context, variable_name, buf);
}

void dsk_print_set_int64           (DskPrint    *context,
                                    const char  *variable_name,
                                    int64_t      value)
{
  char buf[64];
  snprintf (buf, sizeof (buf), "%lld", (long long) value);
  dsk_print_set_string (context, variable_name, buf);
}

void dsk_print_set_uint64 (DskPrint *context,
                         const char *variable_name,
                         uint64_t value)
{
  char buf[64];
  snprintf (buf, sizeof (buf), "%llu", (unsigned long long) value);
  dsk_print_set_string (context, variable_name, buf);
}

void dsk_print_set_double          (DskPrint        *context,
                                    const char      *name,
                                    DskPrintFloatFlags flags,
                                    double           value)
{
  char buf[256];
  if (flags & DSK_PRINT_FLOAT_FLAG_EXACT_DIGITS)
    {
      snprintf (buf, sizeof (buf), "%.*f",
                (int) (DSK_PRINT_FLOAT_MAX_PRECISION & flags),
                value);
      dsk_assert ((flags & (DSK_PRINT_FLOAT_FLAG_SCIENTIFIC)) == 0);
    }
  else if (flags & DSK_PRINT_FLOAT_FLAG_SCIENTIFIC)
    {
      snprintf (buf, sizeof (buf), "%.*e",
                (int) (DSK_PRINT_FLOAT_MAX_PRECISION & flags),
                value);
      dsk_assert ((flags & (DSK_PRINT_FLOAT_FLAG_EXACT_DIGITS)) == 0);
    }
  else
    {
      dsk_assert_not_reached ();
    }
  dsk_print_set_string (context, name, buf);
}

void dsk_print_push (DskPrint *context)
{
  StackNode *new_node = DSK_NEW (StackNode);
  if (context == NULL)
    context = get_default_context ();
  new_node->prev = context->top;
  context->top = new_node;
  new_node->vars = NULL;
}

void dsk_print_pop (DskPrint *context)
{
  StackNode *old;
  if (context == NULL)
    context = get_default_context ();
  if (context->top == &context->bottom)
    {
      dsk_warning ("dsk_print_pop: stack empty");
      return;
    }
  old = context->top;
  context->top = old->prev;

  while (old->vars)
    {
      VarDef *vd = old->vars;
      VarDef *next = vd->next_with_same_key;
      old->vars = vd->next_in_stack_node;
      if (next == NULL)
        DSK_RBTREE_REMOVE (GET_VARDEF_TREE (context), vd);
      else
        DSK_RBTREE_REPLACE_NODE (GET_VARDEF_TREE (context), vd, next);
      dsk_free (vd);
    }
  dsk_free (old);
}

/* --- implementation of print --- */
static dsk_boolean
handle_template_expression (DskPrint *context,
                            const char *expr,
                            unsigned   *expr_len_out,
                            DskError **error)
{
  /* For now, the only type of template expr is $variable or ${variable} */
  unsigned length;
  const char *start;
  VarDef *result;
  dsk_assert (*expr == '$');
  if (expr[1] == '{')
    {
      const char *end;
      start = expr + 2;
      while (dsk_ascii_isspace (*start))
        start++;
#define ascii_is_ident(c)    (dsk_ascii_isalnum(c) || (c)=='_')
      if (!ascii_is_ident (*start))
        {
          dsk_set_error (error, "unexpected character %s after '${' in dsk_print",
                         dsk_ascii_byte_name (*start));
          return DSK_FALSE;
        }
      length = 1;
      while (ascii_is_ident (start[length]))
        length++;
      end = start + length;
      while (dsk_ascii_isspace (*end))
        end++;
      if (*end != '}')
        {
          dsk_set_error (error,
                         "unexpected character %s after '${%.*s' in dsk_print",
                         dsk_ascii_byte_name (*end),
                         (int) length, start);
          return DSK_FALSE;
        }
      *expr_len_out = (end + 1) - expr;
    }
  else if (ascii_is_ident (expr[1]))
    {
      length = 1;
      start = expr + 1;
      while (ascii_is_ident (start[length]))
        length++;
      *expr_len_out = length + 1;
    }
  else
    {
      dsk_set_error (error, "unexpected character %s after '$' in dsk_print",
                     dsk_ascii_byte_name (expr[1]));
      return DSK_FALSE;
    }
#define COMPARE_START_LEN_TO_VAR_DEF(unused,b, rv) \
      rv = memcmp (start, b->key, length);            \
      if (rv == 0 && b->key[length] != '\0')          \
        rv = -1;
  DSK_RBTREE_LOOKUP_COMPARATOR (GET_VARDEF_TREE (context),
                                unused, COMPARE_START_LEN_TO_VAR_DEF,
                                result);
#undef COMPARE_START_LEN_TO_VAR_DEF
  if (result == NULL)
    {
      dsk_set_error (error,
                     "unset variable $%.*s excountered in print template",
                     (int) length, start);
      return DSK_FALSE;
    }
  if (!context->append (result->value_length, (uint8_t*)(result+1),
                        context->append_data, error))
    return DSK_FALSE;
  return DSK_TRUE;
}

static dsk_boolean
print__internal (DskPrint   *context,
                 const char *str,
                 DskError  **error)
{
  const char *at = str;
  const char *last_at = at;
  while (*at)
    {
      if (*at == '$')
        {
          if (at[1] == '$')
            {
              /* call append - include the '$' */
              if (!context->append (at-last_at+1, (uint8_t*) last_at, 
                                    context->append_data, error))
                return DSK_FALSE;

              at += 2;
              last_at = at;
            }
          else
            {
              unsigned expr_len;
              /* call append on any uninterpreted data */
              if (at > last_at
               && !context->append (at-last_at, (uint8_t*) last_at, 
                                    context->append_data, error))
                return DSK_FALSE;

              /* parse template expression */
              if (!handle_template_expression (context, at, &expr_len, error))
                return DSK_FALSE;
              at += expr_len;
              last_at = at;
            }
        }
      else
        at++;
    }
  if (last_at != at)
    {
      if (!context->append (at-last_at, (uint8_t*) last_at, 
                            context->append_data, error))
        return DSK_FALSE;
    }
  return DSK_TRUE;
}

void
dsk_print (DskPrint *context,
           const char *template_string)
{
  DskError *error = NULL;
  if (context == NULL)
    context = get_default_context ();
  if (!print__internal (context, template_string, &error)
   || !context->append (1, (const uint8_t*) "\n", context->append_data, &error))
    {
      dsk_warning ("error in dsk_print: %s", error->message);
      dsk_error_unref (error);
    }
}

dsk_boolean dsk_print_append_data           (DskPrint *print,
                                      unsigned   length,
                                      const uint8_t *data,
                                     DskError **error)
{
  return print->append (length, data, print->append_data, error);
}

static dsk_boolean append_to_buffer (unsigned   length,
                                     const uint8_t *data,
                                     void      *append_data,
                                     DskError **error)
{
  DSK_UNUSED (error);
  dsk_buffer_append (append_data, length, data);
  return DSK_TRUE;
}

void
dsk_print_set_template_string (DskPrint *context,
                               const char *variable_name,
                               const char *template_string)
{
  DskBuffer buffer = DSK_BUFFER_INIT;
  DskError *error = NULL;

  /* save append/append_func */
  DskPrintAppendFunc old_append = context->append;
  void *old_append_data = context->append_data;

  context->append = append_to_buffer;
  context->append_data = &buffer;

  if (!print__internal (context, template_string, &error))
    {
      dsk_warning ("error in dsk_print: %s", error->message);
      dsk_error_unref (error);
    }

  context->append = old_append;
  context->append_data = old_append_data;

  dsk_print_set_buffer (context, variable_name, &buffer);
  dsk_buffer_clear (&buffer);
}

DskPrint *dsk_print_new_buffer    (DskBuffer *buffer)
{
  return dsk_print_new (append_to_buffer, buffer, NULL);
}

void dsk_print_set_buffer          (DskPrint    *context,
                                    const char  *variable_name,
                                    const DskBuffer *buffer)
{
  unsigned key_len = strlen (variable_name);
  unsigned value_len = buffer->size;
  VarDef *vd = dsk_malloc (sizeof (VarDef) + key_len + 1 + value_len + 1);
  char *value = (char*) (vd + 1);
  dsk_buffer_peek (buffer, value_len, value);
  value[value_len] = 0;
  vd->key = value + value_len + 1;
  strcpy ((char*) vd->key, variable_name);
  vd->value_length = value_len;
  add_var_def (context, vd);
}

/* --- quoting support --- */
void dsk_print_set_filtered_buffer   (DskPrint    *context,
                                    const char  *variable_name,
			            const DskBuffer *buffer,
                                    DskOctetFilter *filter)
{
  DskBuffer output = DSK_BUFFER_INIT;
  const DskBufferFragment *in = buffer->first_frag;
  DskError *error = NULL;
  while (in)
    {
      if (!dsk_octet_filter_process (filter, &output, in->buf_length,
                                     in->buf + in->buf_start, &error))
        goto failed;
      in = in->next;
    }
  if (!dsk_octet_filter_finish (filter, &output, &error))
    goto failed;

  dsk_print_set_dsk_buffer (context, variable_name, &output);
  dsk_buffer_clear (&output);
  dsk_object_unref (filter);
  return;

failed:
  dsk_warning ("error filtering data in dsk_print_set_filtered: %s (variable name=%s)",
               error->message,
               variable_name);
  dsk_buffer_clear (&output);
  dsk_object_unref (filter);
  return;
}
void dsk_print_set_filtered_string   (DskPrint    *context,
                                    const char  *variable_name,
			            const char  *raw_string,
                                    DskOctetFilter *filter)
{
  dsk_print_set_filtered_binary (context, variable_name,
                               strlen (raw_string),
                               (const uint8_t *) raw_string,
                               filter);
}

void dsk_print_set_filtered_binary   (DskPrint    *context,
                                    const char  *variable_name,
                                    size_t       raw_string_length,
			            const uint8_t*raw_string,
                                    DskOctetFilter *filter)
{
  DskBuffer buf;
  DskBufferFragment fragment;
  fragment.buf_start = 0;
  fragment.buf = (uint8_t*) raw_string;
  fragment.buf_length = raw_string_length;
  fragment.next = NULL;
  buf.first_frag = buf.last_frag = &fragment;
  buf.size = fragment.buf_length;
  dsk_print_set_filtered_buffer (context, variable_name, &buf, filter);
}


static dsk_boolean
append_to_file_pointer (unsigned   length,
                        const uint8_t *data,
                        void      *append_data,
                        DskError **error)
{
  /* TODO: use fwrite_unlocked where available */
  if (length == 0)
    return DSK_TRUE;
  if (fwrite (data, length, 1, append_data) != 1)
    {
      dsk_set_error (error, "error writing to file-pointer");
      return DSK_FALSE;
    }
  return DSK_TRUE;
}

static void
fclose_file_pointer (void *data)
{
  fclose (data);
}
static void
pclose_file_pointer (void *data)
{
  pclose (data);
}

DskPrint *dsk_print_new_fp (void *file_pointer)
{
  return dsk_print_new (append_to_file_pointer,
                        file_pointer,
                        NULL);
}
DskPrint *dsk_print_new_fp_fclose (void *file_pointer)
{
  return dsk_print_new (append_to_file_pointer,
                        file_pointer,
                        fclose_file_pointer);
}
DskPrint *dsk_print_new_fp_pclose (void *file_pointer)
{
  return dsk_print_new (append_to_file_pointer,
                        file_pointer,
                        pclose_file_pointer);
}
