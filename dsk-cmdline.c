#include <string.h>
#include <stdlib.h>
#include <alloca.h>
#include <stdio.h>
#include "dsk-rbtree-macros.h"
#include "dsk-common.h"
#include "dsk-object.h"
#include "dsk-error.h"
#include "dsk-cmdline.h"
#include "dsk-buffer.h"

typedef struct _DskCmdlineMode DskCmdlineMode;
typedef struct _DskCmdlineArg DskCmdlineArg;
typedef struct _ExclNode ExclNode;

#define FIRST_SINGLE_CHAR_CMDLINE_ARG   '!'
#define LAST_SINGLE_CHAR_CMDLINE_ARG    '~'
#define N_SINGLE_CHAR_CMDLINE_ARGS  (LAST_SINGLE_CHAR_CMDLINE_ARG-FIRST_SINGLE_CHAR_CMDLINE_ARG+1)

/* bah, it just seems unnecessary to support an
   arbitrary number of aliases. */
#define DSK_CMDLINE_MAX_ALIASES 5

struct _DskCmdlineMode
{
  const char *mode_name;                /* NULL for top-level */
  const char *short_desc;
  const char *long_desc;
  const char *non_option_arg_desc;
  dsk_boolean permit_unknown_options;
  dsk_boolean permit_extra_arguments;
  dsk_boolean permit_help;
  DskCmdlineArgumentHandler argument_handler;

  unsigned n_aliases;
  const char *aliases[DSK_CMDLINE_MAX_ALIASES];

  DskCmdlineMode *parent;
  DskCmdlineMode *next_sibling;
  DskCmdlineMode *first_child;
  DskCmdlineMode *last_child;

  DskCmdlineArg *arg_tree;
  DskCmdlineArg *args_single_char[N_SINGLE_CHAR_CMDLINE_ARGS];

  void *user_data;
  DskVoidFunc callback;

  ExclNode *first_excl_node, *last_excl_node;
};

static DskCmdlineMode toplevel_mode = {
  .permit_help = DSK_TRUE
};

static DskCmdlineMode *configuring = &toplevel_mode;

void *dsk_cmdline_mode_user_data = NULL;        /* public */

static dsk_boolean swallow_arguments = DSK_TRUE;

#define _DSK_CMDLINE_OPTION_USED                (1<<31)

struct _DskCmdlineArg
{
  const char *option_name;
  char c;
  const char *description;
  const char *arg_description;
  DskCmdlineFlags flags;
  void *value_ptr;
  DskCmdlineCallback callback;
  dsk_boolean (*func)(DskCmdlineArg *arg,
                      const char    *str,
                      DskError     **error);


  DskCmdlineArg *left, *right, *parent;
  dsk_boolean is_red;
};

#define COMPARE_STR_TO_ARG_NODE(a,b, rv)  rv = strcmp(a, b->option_name)
#define COMPARE_CMDLINE_ARG_NODES(a,b, rv)  COMPARE_STR_TO_ARG_NODE(a->option_name,b,rv)

#define CMDLINE_ARG_GET_TREE(mode) \
  (mode)->arg_tree, DskCmdlineArg*,  \
  DSK_STD_GET_IS_RED, DSK_STD_SET_IS_RED, \
  parent, left, right, \
  COMPARE_CMDLINE_ARG_NODES

static int
compare_equal_terminated_str (const char *option,
                              DskCmdlineArg *arg)
{
  const char *at = arg->option_name;
  char a,b;
  while (*option && *option != '=' && *at && *option == *at)
    {
      option++;
      at++;
    }
  a = (*option && *option != '=') ? *option : 0;
  b = *at;
  return a < b ? -1 : a > b ? 1 : 0;
}

#define COMPARE_STR_EQUAL_TO_ARG_NODE(a,b, rv)  rv = compare_equal_terminated_str(a,b)


/* For mutually-exclusive argument handling */
struct _ExclNode
{
  dsk_boolean one_required;
  unsigned n_args;
  DskCmdlineArg **args;
  ExclNode *next;
};


void dsk_cmdline_init        (const char     *short_desc,
                              const char     *long_desc,
                              const char     *non_option_arg_desc,
                              DskCmdlineInitFlags flags)
{
  configuring->short_desc = short_desc;
  configuring->long_desc = long_desc;
  configuring->non_option_arg_desc = non_option_arg_desc;
  if (flags & DSK_CMDLINE_PERMIT_ARGUMENTS)
    configuring->permit_extra_arguments = DSK_TRUE;
  if (flags & DSK_CMDLINE_PERMIT_UNKNOWN_OPTIONS)
    configuring->permit_unknown_options = DSK_TRUE;

  /* By default, modify argc/argv */
  swallow_arguments = (flags & DSK_CMDLINE_DO_NOT_MODIFY_ARGV) == 0;
}

static DskCmdlineArg *
try_option (const char *option_name)
{
  DskCmdlineArg *rv;
  DSK_RBTREE_LOOKUP_COMPARATOR (CMDLINE_ARG_GET_TREE (configuring),
                                option_name,
                                COMPARE_STR_TO_ARG_NODE, rv);
  return rv;
}

static DskCmdlineArg *
add_option (const char *option_name)
{
  DskCmdlineArg *rv = try_option (option_name);
  DskCmdlineArg *conflict;
  if (rv != NULL)
    dsk_die ("option %s added twice", option_name);
  rv = DSK_NEW (DskCmdlineArg);
  rv->option_name = option_name;
  rv->c = 0;
  rv->description = rv->arg_description = NULL;
  rv->flags = 0;
  rv->value_ptr = NULL;
  rv->func = NULL;
  DSK_RBTREE_INSERT (CMDLINE_ARG_GET_TREE (configuring), rv, conflict);
  dsk_assert (conflict == NULL);
  return rv;
}

static dsk_boolean
cmdline_handle_int (DskCmdlineArg *arg,
                    const char    *str,
                    DskError     **error)
{
  char *end;
  long v = strtol (str, &end, 0);
  if (str == end)
    {
      dsk_set_error (error, "error parsing integer");
      return DSK_FALSE;
    }
  if (*end != '\0')
    {
      dsk_set_error (error, "garbage at end of integer");
      return DSK_FALSE;
    }
  * (int *) arg->value_ptr = v;
  return DSK_TRUE;
}

void dsk_cmdline_add_int     (const char     *option_name,
                              const char     *description,
			      const char     *arg_description,
                              DskCmdlineFlags flags,
                              int            *value_out)
{
  DskCmdlineArg *arg = add_option (option_name);
  dsk_assert (description != NULL);
  dsk_assert (arg_description != NULL);
  dsk_assert ((flags & DSK_CMDLINE_OPTIONAL) == 0);
  arg->description = description;
  arg->arg_description = arg_description;
  arg->flags = flags | DSK_CMDLINE_TAKES_ARGUMENT;
  arg->value_ptr = value_out;
  arg->func = cmdline_handle_int;
}

static dsk_boolean
cmdline_handle_uint (DskCmdlineArg *arg,
                    const char    *str,
                    DskError     **error)
{
  char *end;
  long v = strtoul (str, &end, 0);
  if (str == end)
    {
      dsk_set_error (error, "error parsing unsigned integer");
      return DSK_FALSE;
    }
  if (*end != '\0')
    {
      dsk_set_error (error, "garbage at end of unsigned integer");
      return DSK_FALSE;
    }
  * (int *) arg->value_ptr = v;
  return DSK_TRUE;
}

void dsk_cmdline_add_uint    (const char     *option_name,
                              const char     *description,
			      const char     *arg_description,
                              DskCmdlineFlags flags,
                              unsigned       *value_out)
{
  DskCmdlineArg *arg = add_option (option_name);
  dsk_assert (description != NULL);
  dsk_assert (arg_description != NULL);
  dsk_assert ((flags & DSK_CMDLINE_OPTIONAL) == 0);
  arg->description = description;
  arg->arg_description = arg_description;
  arg->flags = flags | DSK_CMDLINE_TAKES_ARGUMENT;
  arg->value_ptr = value_out;
  arg->func = cmdline_handle_uint;
}

static dsk_boolean
cmdline_handle_int64 (DskCmdlineArg *arg,
                      const char    *str,
                      DskError     **error)
{
  char *end;
  // TODO: use strtoq if necessary.
  long v = strtoll (str, &end, 0);
  if (str == end)
    {
      dsk_set_error (error, "error parsing integer");
      return DSK_FALSE;
    }
  if (*end != '\0')
    {
      dsk_set_error (error, "garbage at end of integer");
      return DSK_FALSE;
    }
  * (int *) arg->value_ptr = v;
  return DSK_TRUE;
}

void dsk_cmdline_add_int64   (const char     *option_name,
                              const char     *description,
			      const char     *arg_description,
                              DskCmdlineFlags flags,
                              int            *value_out)
{
  DskCmdlineArg *arg = add_option (option_name);
  dsk_assert (description != NULL);
  dsk_assert (arg_description != NULL);
  dsk_assert ((flags & DSK_CMDLINE_OPTIONAL) == 0);
  arg->description = description;
  arg->arg_description = arg_description;
  arg->flags = flags | DSK_CMDLINE_TAKES_ARGUMENT;
  arg->value_ptr = value_out;
  arg->func = cmdline_handle_int64;
}

static dsk_boolean
cmdline_handle_uint64 (DskCmdlineArg *arg,
                       const char    *str,
                       DskError     **error)
{
  char *end;
  // TODO: use strtouq if necessary.
  long v = strtoull (str, &end, 0);
  if (str == end)
    {
      dsk_set_error (error, "error parsing unsigned integer");
      return DSK_FALSE;
    }
  if (*end != '\0')
    {
      dsk_set_error (error, "garbage at end of unsigned integer");
      return DSK_FALSE;
    }
  * (int *) arg->value_ptr = v;
  return DSK_TRUE;
}

void dsk_cmdline_add_uint64  (const char     *option_name,
                              const char     *description,
			      const char     *arg_description,
                              DskCmdlineFlags flags,
                              unsigned       *value_out)
{
  DskCmdlineArg *arg = add_option (option_name);
  dsk_assert (description != NULL);
  dsk_assert (arg_description != NULL);
  dsk_assert ((flags & DSK_CMDLINE_OPTIONAL) == 0);
  arg->description = description;
  arg->arg_description = arg_description;
  arg->flags = flags | DSK_CMDLINE_TAKES_ARGUMENT;
  arg->value_ptr = value_out;
  arg->func = cmdline_handle_uint64;
}


static dsk_boolean
cmdline_handle_double (DskCmdlineArg *arg,
                       const char    *str,
                       DskError     **error)
{
  char *end;
  double v = strtod (str, &end);
  if (str == end)
    {
      dsk_set_error (error, "error parsing floating-point number");
      return DSK_FALSE;
    }
  if (*end != '\0')
    {
      dsk_set_error (error, "garbage at end of floating-point number");
      return DSK_FALSE;
    }
  * (int *) arg->value_ptr = v;
  return DSK_TRUE;
}

void dsk_cmdline_add_double  (const char     *option_name,
                              const char     *description,
			      const char     *arg_description,
                              DskCmdlineFlags flags,
                              double         *value_out)
{
  DskCmdlineArg *arg = add_option (option_name);
  dsk_assert (description != NULL);
  dsk_assert (arg_description != NULL);
  dsk_assert ((flags & DSK_CMDLINE_OPTIONAL) == 0);
  arg->description = description;
  arg->arg_description = arg_description;
  arg->flags = flags | DSK_CMDLINE_TAKES_ARGUMENT;
  arg->value_ptr = value_out;
  arg->func = cmdline_handle_double;
}

static dsk_boolean
cmdline_handle_boolean (DskCmdlineArg *arg,
                        const char    *str,
                        DskError     **error)
{
  if (str == NULL)
    {
      * (dsk_boolean *) arg->value_ptr = DSK_TRUE;
    }
  else
    {
      if (!dsk_parse_boolean (str, arg->value_ptr))
        {
          dsk_set_error (error, "error parsing boolean from string");
          return DSK_FALSE;
        }
    }
  if (arg->flags & DSK_CMDLINE_REVERSED)
    * (dsk_boolean *) arg->value_ptr = ! (* (dsk_boolean *) arg->value_ptr);
  return DSK_TRUE;
}

void dsk_cmdline_add_boolean (const char     *option_name,
                              const char     *description,
                              const char     *arg_description,
                              DskCmdlineFlags flags,
                              dsk_boolean    *value_out)
{
  DskCmdlineArg *arg = add_option (option_name);
  dsk_assert (description != NULL);
  arg->description = description;
  arg->arg_description = arg_description;
  dsk_assert ((flags & DSK_CMDLINE_OPTIONAL) == 0);
  if (arg_description)
    arg->flags = flags | DSK_CMDLINE_TAKES_ARGUMENT;
  else
    arg->flags = flags & ~DSK_CMDLINE_TAKES_ARGUMENT;
  arg->value_ptr = value_out;
  arg->func = cmdline_handle_boolean;
}

static dsk_boolean
cmdline_handle_string (DskCmdlineArg *arg,
                       const char    *str,
                       DskError     **error)
{
  DSK_UNUSED (error);
  * (const char **) arg->value_ptr = str;
  return DSK_TRUE;
}

void dsk_cmdline_add_string  (const char     *option_name,
                              const char     *description,
			      const char     *arg_description,
                              DskCmdlineFlags flags,
                              const char    **value_out)
{
  DskCmdlineArg *arg = add_option (option_name);
  dsk_assert (description != NULL);
  dsk_assert (arg_description != NULL);
  dsk_assert ((flags & DSK_CMDLINE_OPTIONAL) == 0);
  arg->description = description;
  arg->arg_description = arg_description;
  arg->flags = flags | DSK_CMDLINE_TAKES_ARGUMENT;
  arg->value_ptr = value_out;
  arg->func = cmdline_handle_string;
}

static dsk_boolean
cmdline_handle_callback (DskCmdlineArg *arg,
                        const char    *str,
                        DskError     **error)
{
  return arg->callback (arg->option_name, str, arg->value_ptr, error);
}

void
dsk_cmdline_add_func    (const char        *option_name,
                         const char        *description,
                         const char        *arg_description,
                         DskCmdlineFlags    flags,
                         DskCmdlineCallback callback,
                         void              *callback_data)
{
  DskCmdlineArg *arg = add_option (option_name);
  dsk_assert (description != NULL);
  arg->description = description;
  arg->arg_description = arg_description;
  if (arg_description)
    arg->flags = flags | DSK_CMDLINE_TAKES_ARGUMENT;
  arg->value_ptr = callback_data;
  arg->callback = callback;
  arg->func = cmdline_handle_callback;
}

void dsk_cmdline_add_shortcut(char            shortcut,
                              const char     *option_name)
{
  DskCmdlineArg *arg = try_option (option_name);
  dsk_return_if_fail (arg != NULL, "option did not exist");
  dsk_return_if_fail (FIRST_SINGLE_CHAR_CMDLINE_ARG <= shortcut
                      && shortcut <= LAST_SINGLE_CHAR_CMDLINE_ARG,
                      "invalid shortcut letter");
  dsk_return_if_fail (configuring->args_single_char[shortcut - FIRST_SINGLE_CHAR_CMDLINE_ARG] == NULL,
                      "shortcut already defined");
  arg->c = shortcut;
  configuring->args_single_char[shortcut - FIRST_SINGLE_CHAR_CMDLINE_ARG] = arg;
}

void dsk_cmdline_mutually_exclusive (dsk_boolean     one_required,
                                     const char     *arg_1,
                                     ...)
{
  va_list args;
  unsigned n = 1;
  char **arg_array;
  char *a;
  va_start (args, arg_1);
  while (va_arg (args, char*))
    n++;
  va_end (args);

  arg_array = alloca (sizeof(char*) * n);
  arg_array[0] = (char*) arg_1;
  va_start (args, arg_1);
  n=1;
  while ((a=va_arg (args, char*)) != NULL)
    arg_array[n++] = a;
  va_end (args);
  dsk_cmdline_mutually_exclusive_v (one_required, n, arg_array);
}

void dsk_cmdline_mutually_exclusive_v (dsk_boolean     one_required,
                                       unsigned        n_excl,
                                       char          **excl)
{
  DskCmdlineArg **args = DSK_NEW_ARRAY (n_excl, DskCmdlineArg*);
  ExclNode *node = DSK_NEW (ExclNode);
  unsigned i;
  node->args = args;
  for (i = 0; i < n_excl; i++)
    if ((args[i] = try_option (excl[i])) == NULL)
      dsk_die ("dsk_cmdline_mutually_exclusive_v: bad option %s", excl[i]);
  node->n_args = n_excl;
  node->one_required = one_required;
  node->next = NULL;

  if (configuring->first_excl_node == NULL)
    configuring->first_excl_node = node;
  else
    configuring->last_excl_node->next = node;
  configuring->last_excl_node = node;
}

void dsk_cmdline_permit_unknown_options (dsk_boolean permit)
{
  configuring->permit_unknown_options = permit;
}
void dsk_cmdline_permit_extra_arguments (dsk_boolean permit)
{
  configuring->permit_extra_arguments = permit;
}
void dsk_cmdline_set_argument_handler (DskCmdlineArgumentHandler handler)
{
  dsk_assert (configuring->argument_handler == NULL);
  configuring->argument_handler = handler;
}

void dsk_cmdline_process_args(int            *argc_inout,
                              char         ***argv_inout)
{
  DskError *error = NULL;
  if (!dsk_cmdline_try_process_args (argc_inout, argv_inout, &error))
    dsk_error ("error processing command-line arguments: %s", error->message);
}

static void
skip_or_swallow (int *argc_inout,
                 char ***argv_inout,
                 int *i_inout,
                 unsigned n)
{
  if (swallow_arguments)
    {
      memmove ((*argv_inout) + (*i_inout),
               (*argv_inout) + (*i_inout) + n,
               ((*argc_inout + 1) - (*i_inout + n)) * sizeof (char*));
      *argc_inout -= n;
    }
  else
    *i_inout += n;
}

static dsk_boolean
check_mandatory_args_recursive (DskCmdlineArg *node,
                                DskError     **error)
{
  if ((node->flags & (DSK_CMDLINE_MANDATORY|_DSK_CMDLINE_OPTION_USED)) == DSK_CMDLINE_MANDATORY)
    {
      dsk_set_error (error, "mandatory option --%s not supplied", node->option_name);
      return DSK_FALSE;
    }
  return (node->left == NULL || check_mandatory_args_recursive (node->left, error))
      && (node->right == NULL || check_mandatory_args_recursive (node->right, error));
}

static dsk_boolean
check_mutually_exclusive_args (DskCmdlineMode *mode,
                               DskError **error)
{
  ExclNode *at;
  for (at = mode->first_excl_node; at; at = at->next)
    {
      unsigned i;
      const char *got_arg_name = NULL;
      for (i = 0; i < at->n_args; i++)
        if (at->args[i]->flags & _DSK_CMDLINE_OPTION_USED)
          {
            if (got_arg_name)
              {
                dsk_set_error (error, "'--%s' and '--%s' are mutually exclusive",
                               got_arg_name, at->args[i]->option_name);
                return DSK_FALSE;
              }
            got_arg_name = at->args[i]->option_name;
          }
      if (got_arg_name == NULL && at->one_required)
        {
          DskBuffer buffer = DSK_BUFFER_INIT;
          for (i = 0; i < at->n_args; i++)
            {
              dsk_buffer_append_string (&buffer, " --");
              dsk_buffer_append_string (&buffer, at->args[i]->option_name);
            }
          char *str = dsk_buffer_empty_to_string (&buffer);
          dsk_set_error (error, "one of the following options is required:%s",
                         str);
          dsk_free (str);
          return DSK_FALSE;
        }
    }
  return DSK_TRUE;
}

static void
print_option (DskCmdlineArg *arg)
{
  fprintf (stderr, "  ");
  if (arg->option_name)
    {
      if ((arg->flags & DSK_CMDLINE_OPTIONAL) != 0 && arg->arg_description != NULL)
        fprintf (stderr, "--%s[=%s]", arg->option_name, arg->arg_description);
      else if (arg->arg_description)
        fprintf (stderr, "--%s=%s", arg->option_name, arg->arg_description);
      else
        fprintf (stderr, "--%s", arg->option_name);
    }
  if (arg->option_name && arg->c)
    fprintf (stderr, ", ");
  if (arg->c)
    {
      fprintf (stderr, "-%c", arg->c);
      if (arg->arg_description)
        fprintf (stderr, " %s", arg->arg_description);
    }
  fprintf (stderr, "\n        %s\n", arg->description);
}

static void
print_options_recursive (DskCmdlineArg *tree)
{
  if (tree == NULL)
    return;
  print_options_recursive (tree->left);
  print_option (tree);
  print_options_recursive (tree->right);
}

static void usage (DskCmdlineMode *mode,
                   const char *prog_name)
{
  const char *base_prog_name = strrchr (prog_name, '/');
  if (base_prog_name)
    base_prog_name++;
  else
    base_prog_name = prog_name;
  if (mode->short_desc)
    fprintf (stderr, "%s - %s\n\n",
             base_prog_name,
             mode->short_desc);
  fprintf (stderr, "usage: %s [OPTIONS] %s\n",
           base_prog_name, mode->non_option_arg_desc ? mode->non_option_arg_desc : "");
  if (mode->long_desc)
    {
      fprintf (stderr, "\n%s\n", mode->long_desc);
    }
  fprintf (stderr, "Options:\n");
  print_options_recursive (mode->arg_tree);
}

static DskCmdlineMode *
find_child_mode (DskCmdlineMode *parent,
                 const char     *name)
{
  DskCmdlineMode *at;
  for (at = parent->first_child; at; at = at->next_sibling)
    {
      unsigned i;
      if (strcmp (at->mode_name, name) == 0)
        return at;
      for (i = 0; i < at->n_aliases; i++)
        if (strcmp (at->aliases[i], name) == 0)
          return at;
    }
  return NULL;
}

dsk_boolean
dsk_cmdline_try_process_args (int *argc_inout,
                              char ***argv_inout,
                              DskError **error)
{
  char **argv = *argv_inout;
  int i;
  DskCmdlineMode *mode = &toplevel_mode;
  DskCmdlineMode *submode;
  for (i = 1; i < *argc_inout; )
    {
      if (argv[i][0] == '-')
        {
          if (argv[i][1] == '-')
            {
              /* long option */
              const char *opt = argv[i] + 2;
              DskCmdlineArg *arg;
              DSK_RBTREE_LOOKUP_COMPARATOR (CMDLINE_ARG_GET_TREE (mode), opt,
                                            COMPARE_STR_EQUAL_TO_ARG_NODE, arg);
              if (arg == NULL)
                {
                  if (mode->permit_unknown_options)
                    {
                      i++;
                      continue;
                    }
                  if (mode->permit_help && strcmp (opt, "help") == 0)
                    {
                      usage (mode, **argv_inout);
                      exit (1);
                    }
                  dsk_set_error (error, "bad option --%s", opt);
                  return DSK_FALSE;
                }
              else
                {
                  const char *eq = strchr (opt, '=');
                  if (eq == NULL
                   && (arg->flags & DSK_CMDLINE_TAKES_ARGUMENT)
                   && (arg->flags & DSK_CMDLINE_OPTIONAL) == 0)
                    {
                      if (i + 1 == *argc_inout)
                        {
                          dsk_set_error (error, "option --%s requires argument", opt);
                          return DSK_FALSE;
                        }
                      if (!arg->func (arg, argv[i+1], error))
                        {
                          dsk_add_error_prefix (error, "processing --%s", opt);
                          return DSK_FALSE;
                        }
                      arg->flags |= _DSK_CMDLINE_OPTION_USED;
                      skip_or_swallow (argc_inout, argv_inout, &i, 2);
                      continue;
                    }
                  if (eq != NULL && !(arg->flags & DSK_CMDLINE_TAKES_ARGUMENT))
                    {
                      dsk_set_error (error, "option --%s has argument: not allowed", opt);
                      return DSK_FALSE;
                    }
                  if (!arg->func (arg, eq ? (eq + 1) : NULL, error))
                    {
                      dsk_add_error_prefix (error, "processing --%s", opt);
                      return DSK_FALSE;
                    }
                  skip_or_swallow (argc_inout, argv_inout, &i, 1);
                  arg->flags |= _DSK_CMDLINE_OPTION_USED;
                }
            }
          else if (argv[i][1] == 0)
            {
              /* just '-':  cease processing options, if allowed */
              if (!mode->permit_extra_arguments)
                {
                  dsk_set_error (error, "got '-', but non-options are forbidding");
                  return DSK_FALSE;
                }
              skip_or_swallow (argc_inout, argv_inout, &i, 1);
              break;
            }
          else
            {
              /* short option */
              if (argv[i][2] == 0)
                {
                  /* a single short-option may take arguments (as in argv[i+1]) */
                  DskCmdlineArg *arg;
                  if ((uint8_t)argv[i][1] < FIRST_SINGLE_CHAR_CMDLINE_ARG
                   || (uint8_t)argv[i][1] > LAST_SINGLE_CHAR_CMDLINE_ARG)
                    {
                      dsk_set_error (error, "invalid single-char option '%c'", argv[i][1]);
                      return DSK_FALSE;
                    }
                  arg = mode->args_single_char[argv[i][1] - FIRST_SINGLE_CHAR_CMDLINE_ARG];
                  if (arg == NULL)
                    {
                      dsk_set_error (error, "invalid single-char option '%c'", argv[i][1]);
                      return DSK_FALSE;
                    }
                  if (arg->flags & DSK_CMDLINE_TAKES_ARGUMENT)
                    {
                      if (i + 1 == *argc_inout)
                        {
                          dsk_set_error (error, "option %s takes argument", argv[i]);
                          return DSK_FALSE;
                        }
                      if (!arg->func (arg, argv[i+1], error))
                        {
                          dsk_add_error_prefix (error, "parsing argument to %s", argv[i]);
                          return DSK_FALSE;
                        }
                      skip_or_swallow (argc_inout, argv_inout, &i, 2);
                      arg->flags |= _DSK_CMDLINE_OPTION_USED;
                    }
                  else
                    {
                      if (!arg->func (arg, NULL, error))
                        {
                          dsk_add_error_prefix (error, "parsing argument to %s", argv[i]);
                          return DSK_FALSE;
                        }
                      skip_or_swallow (argc_inout, argv_inout, &i, 1);
                      arg->flags |= _DSK_CMDLINE_OPTION_USED;
                    }
                }
              else
                {
                  /* any number of no-argument short options */
                  unsigned ci;
                  for (ci = 1; argv[i][ci] != 0; ci++)
                    {
                      if (argv[i][ci] < FIRST_SINGLE_CHAR_CMDLINE_ARG
                       || argv[i][ci] > LAST_SINGLE_CHAR_CMDLINE_ARG
                       || mode->args_single_char[argv[i][ci] - FIRST_SINGLE_CHAR_CMDLINE_ARG] == NULL)
                        {
                          dsk_set_error (error, "invalid single-char option '%c'", argv[i][ci]);
                          return DSK_FALSE;
                        }
                      if (mode->args_single_char[argv[i][ci] - FIRST_SINGLE_CHAR_CMDLINE_ARG]->flags & DSK_CMDLINE_TAKES_ARGUMENT)
                        {
                          dsk_set_error (error, "multiple short-options combined, but '%c' take argument", argv[i][ci]);
                          return DSK_FALSE;
                        }
                    }
                  for (ci = 1; argv[i][ci] != 0; ci++)
                    {
                      DskCmdlineArg *arg = mode->args_single_char[argv[i][ci] - FIRST_SINGLE_CHAR_CMDLINE_ARG];
                      if (!arg->func (arg, NULL, error))
                        {
                          dsk_add_error_prefix (error, "processing -%c", argv[i][ci]);
                          return DSK_FALSE;
                        }
                      arg->flags |= _DSK_CMDLINE_OPTION_USED;
                    }
                  skip_or_swallow (argc_inout, argv_inout, &i, 1);
                }
            }
        }
      else if ((submode=find_child_mode (mode, argv[i])) != NULL)
        {
          mode = submode;
          dsk_cmdline_mode_user_data = mode->user_data;
          if (mode->callback)
            mode->callback ();
          skip_or_swallow (argc_inout, argv_inout, &i, 1);
        }
      else if (mode->argument_handler)
        {
          if (!mode->argument_handler (argv[i], error))
            return DSK_FALSE;
          skip_or_swallow (argc_inout, argv_inout, &i, 1);
        }
      else if (mode->permit_extra_arguments)
        {
          i++;
        }
      else
        {
          dsk_set_error (error, "unexpected command-line argument '%s'", argv[i]);
          return DSK_FALSE;
        }
    }

  /* check that all mandatory arguments are used */
  for (submode = mode; submode; submode = submode->parent)
    {
      if (submode->arg_tree != NULL
       && !check_mandatory_args_recursive (submode->arg_tree, error))
        return DSK_FALSE;
      if (!check_mutually_exclusive_args (submode, error))
        return DSK_FALSE;
    }

  return DSK_TRUE;
}

static void
_dsk_cmdline_arg_tree_free_recursive (DskCmdlineArg *arg)
{
  if (arg == NULL)
    return;
  _dsk_cmdline_arg_tree_free_recursive (arg->left);
  _dsk_cmdline_arg_tree_free_recursive (arg->right);
  dsk_free (arg);
}

static void _dsk_cmdline_mode_clear (DskCmdlineMode *mode)
{
  while (mode->first_child)
    {
      DskCmdlineMode *kill = mode->first_child;
      mode->first_child = kill->next_sibling;
      _dsk_cmdline_mode_clear (kill);
      dsk_free (kill);
    }
  _dsk_cmdline_arg_tree_free_recursive (mode->arg_tree);
}

void _dsk_cmdline_cleanup (void)
{
  _dsk_cmdline_mode_clear (&toplevel_mode);
}
