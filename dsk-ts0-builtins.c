#define DSK_INCLUDE_TS0
#include "dsk.h"
#include "dsk-ts0-builtins.h"


static dsk_boolean
function_invoke__concat (DskTs0Function  *function,
                         DskTs0Namespace *ns,
                         unsigned         n_args,
                         DskTs0Expr     **args,
                         DskBuffer       *output,
                         DskError       **error)
{
  unsigned i;
  (void) function;
  for (i = 0; i < n_args; i++)
    if (!dsk_ts0_expr_evaluate (args[i], ns, output, error))
      return DSK_FALSE;
  return DSK_TRUE;
}

DskTs0Function dsk_ts0_function_concat =
{
  function_invoke__concat,
  NULL,
  1,
  1             /* cachable */
};

#if 0
#define STRICT_FUNCTION_ARGS DskTs0Namespace *ns, \
                             unsigned        n_args, \
                             char          **args, \
                             DskBuffer      *output, \
                             DskError      **error)

static dsk_boolean
tr__strict (STRICT_FUNCTION_ARGS)
{
  DSK_UNUSED (ns);
  ENSURE_N_ARGS_EXACT(3);

  uint8_t map[256];
  for (i = 0; i < 256; i++)
    map[i] = i;
  
  ...
}

#define STRICT_FUNCTION_ENTRY(basename) \
{ #basename, basename ## __strict }

const DskTs0BuiltinStrictFunc dsk_ts0_builtin_strict_funcs[] =
{
  STRICT_FUNCTION_ENTRY (tr),
};

const unsigned dsk_ts0_builtin_n_strict_funcs = DSK_N_ELEMENTS (dsk_ts0_builtin_strict_funcs);

const DskTs0BuiltinLazyFunc dsk_ts0_builtin_lazy_funcs[] =
{
  LAZY_FUNCTION_ENTRY (catch),
  LAZY_FUNCTION_ENTRY (catch),
const unsigned dsk_ts0_builtin_n_lazy_funcs = DSK_N_ELEMENTS (dsk_ts0_builtin_lazy_funcs);
#endif
