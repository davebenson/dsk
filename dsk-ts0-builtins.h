
extern DskTs0Function dsk_ts0_function_concat;

typedef struct _DskTs0BuiltinStrictFunc DskTs0BuiltinStrictFunc;
struct _DskTs0BuiltinStrictFunc
{
  const char *name;
  DskTs0StrictFunc func;
};

typedef struct _DskTs0BuiltinLazyFunc DskTs0BuiltinLazyFunc;
struct _DskTs0BuiltinLazyFunc
{
  const char *name;
  DskTs0LazyFunc func;
};
extern const unsigned dsk_ts0_builtin_n_strict_funcs;
extern const DskTs0BuiltinStrictFunc dsk_ts0_builtin_strict_funcs[];

extern const unsigned dsk_ts0_builtin_n_lazy_funcs;
extern const DskTs0BuiltinLazyFunc dsk_ts0_builtin_lazy_funcs[];

