
typedef struct _DskZLangTypeInfo DskZLangTypeInfo;

typedef enum
{
  DSK_ZLANG_NAMESPACE_ENTRY_TYPE,
  DSK_ZLANG_NAMESPACE_ENTRY_GLOBAL,
  DSK_ZLANG_NAMESPACE_ENTRY_FUNCTION
} DskZLangNamespaceEntryType;

typedef struct _DskZLangNamespaceEntry DskZLangNamespaceEntry;
struct _DskZLangNamespaceEntry
{
  DskZLangNamespaceEntryType type;
  char *name;
  union {
    struct {
      void (*clear)(DskZLangTypeInfo        *type_info,
                    DskZLangStackNode       *value);
      void (*copy) (DskZLangTypeInfo        *type_info,
                    DskZLangStackNode       *dst,
                    const DskZLangStackNode *src);
    } type;
    struct {
      DskZLangStackNode value;
    } global;
    struct {
      DskZLangFunction *variadic;
      unsigned n_arities;
      DskZLangFunction **arities;
    } function;
    DskZLangNamespace *ns;
  } u;

  /* tree structure parts */
  DskZLangNamespaceEntry *left, *right, *parent;
  dsk_boolean is_red;
};

struct _DskZLangNamespace
{
  DskZLangNamespaceEntry *tree;
};

struct _DskZLangContext
{
  DskZLangNamespace namespace;
};


DskZLangContext *dsk_zlang_context_new        (void)
{
  DskZLangContext *rv = DSK_NEW (DskZLangContext);
  rv->namespace.tree = NULL;
  dsk_zlang_namespace_register_type (&rv->namespace, "number",
                                     dsk_zlang_stack_node_clear_do_nothing,
                                     dsk_zlang_stack_node_copy_memcpy);
  dsk_zlang_namespace_register_type (&rv->namespace, "string",
                                     dsk_zlang_stack_node_clear_string,
                                     dsk_zlang_stack_node_copy_string);
  return rv;
}

dsk_boolean      dsk_zlang_context_parse_file (DskZLangContext *context,
                                               const char      *filename,
                                               DskZLangInsn   **main_out,
                                               DskError       **error)
{
  size_t size;
  char *contents = dsk_file_get_contents (filename, &size, error);
  DskCTokenScannerConfig config = DSK_CTOKEN_SCANNER_CONFIG_INIT;
  if (content == NULL)
    return DSK_FALSE;
  config.error_filename = filename;
  ctoken = dsk_ctoken_scan_str (contents, contents + size, &config, error);
  if (ctoken == NULL)
    {
      dsk_free (contents);
      return DSK_FALSE;
    }
  ...
}

dsk_boolean      dsk_zlang_context_register   (DskZLangContext *context,
                                               const char      *name,
                                               DskZLangFunction *function,
                                               DskError       **error);

DskZLangStack   *dsk_zlang_stack_new          (DskZLangContext *context,
                                               DskZLangHandler  handler,
                                               DskZLangHandler  exception,
                                               void            *handler_data,
                                               DskDestroyNotify destroy);
void             dsk_zlang_stack_run          (DskZLangStack   *stack,
                                               DskZLangInsn    *insn);


/* one of these functions must be invoked by DskZLangHandler */
void dsk_zlang_handler_return (DskZLangStack     *stack);
void dsk_zlang_handler_throw  (DskZLangStack     *stack,
                               DskZLangStackNode *exception);
