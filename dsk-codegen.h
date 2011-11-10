
char * dsk_codegen_mixedcase_normalize       (const char *mixedcase);
char * dsk_codegen_mixedcase_to_uppercase    (const char *mixedcase);
char * dsk_codegen_mixedcase_to_lowercase    (const char *mixedcase);
char * dsk_codegen_lowercase_to_mixedcase    (const char *lowercase);

#if 0
char * dsk_codegen_mixedcase_normalize_n     (unsigned    length,
                                              const char *mixedcase);
char * dsk_codegen_mixedcase_to_uppercase_n  (unsigned    length,
                                              const char *mixedcase);
char * dsk_codegen_mixedcase_to_lowercase_n  (unsigned    length,
                                              const char *mixedcase);
char * dsk_codegen_lowercase_to_mixedcase_n  (unsigned    length,
                                              const char *lowercase);
#endif


/* --- function prototype rendering --- */
typedef struct _DskCodegenArg DskCodegenArg;
typedef struct _DskCodegenFunction DskCodegenFunction;

struct _DskCodegenArg
{
  const char *type;
  const char *name;
};

struct _DskCodegenFunction
{
  const char    *storage;           /* NULL or "static" */
  const char    *return_type;
  const char    *name;
  unsigned       n_args;
  DskCodegenArg *args;
  dsk_boolean    semicolon;
  dsk_boolean    newline;
  dsk_boolean    render_void_args;  /* say "void" if n_args==0 */

  /* --- used to align all functions in a file together --- */

  /* -1 means put return-type and function name on the same line;
     otherwise, they go on separate lines unless they fit. */
  int             function_name_pos;

  /* -1 means just use the length of name; if the name is too long,
     the arguments go on the next line, unless there are 0 args. */
  int             function_name_width;

  /* -1 means use the longest type name */
  int             function_argtypes_width;

  /* -1 means use the longest arg name */
  int             function_argnames_width;
};

#define DSK_CODEGEN_FUNCTION_INIT                       \
{                                                       \
  NULL,                         /* storage */           \
  "void",                       /* return_type */       \
  NULL,                         /* name */              \
  0, NULL,                      /* n_args, args */      \
  DSK_FALSE,                    /* semicolon */         \
  DSK_TRUE,                     /* newline */           \
  DSK_TRUE,                     /* render_void_args */  \
  -1, -1, -1, -1                /* optional alignment args */ \
}



void dsk_codegen_function_render (DskCodegenFunction *function,
                                  DskPrint           *print);
void dsk_codegen_function_render_buffer
                                 (DskCodegenFunction *function,
                                  DskBuffer *buffer);
