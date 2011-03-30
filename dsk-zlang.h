
typedef enum
{
  DSK_ZLANG_TYPE_NUMBER,
  DSK_ZLANG_TYPE_UNICODE,
  DSK_ZLANG_TYPE_STRING,
  DSK_ZLANG_TYPE_CODE_POINTER,
  DSK_ZLANG_TYPE_CATCH,
  DSK_ZLANG_INSN_FUNCTION
} DskZLangType;

typedef void         (*DskZLangHandler) (DskZLangStack     *stack,
                                         void              *handler_data);
typedef DskZLangInsn (*DskZLangCaseFunc)(DskZLangStackNode *test_val,
                                         void              *case_data);
  
struct _DskZLangStackNode
{
  DskZLangType type;
  union {
    int v_int;
    unsigned v_uint;
    float v_float;
    double v_double;
    struct {
      DskZLangInsn *activation;
      unsigned n_params;
      DskZLangStackNode *params;
    } v_code_pointer;
    struct {
      DskZLangHandler handler;
      void *handler_data;
      DskDestroyNotify destroy;
    } v_catch;
    DskZLangFunction *v_function;
    char *v_string;
  };
};
struct _DskZLangStack
{
  unsigned stack_size;
  unsigned stack_alloced;
  DskZLangStackNode *stack;
};
  
typedef enum
{
  DSK_ZLANG_INSN_PUSH,
  DSK_ZLANG_INSN_DUP,
  DSK_ZLANG_INSN_POP,
  DSK_ZLANG_INSN_CALL,
  DSK_ZLANG_INSN_CASE,
  DSK_ZLANG_INSN_JUMP
} DskZLangInsnType;

struct _DskZLangInsn
{
  DskZLangInsnType type;
  union {
    struct { DskZLangStackNode *value; } v_push;
    struct { unsigned source, dest; } v_dup;
    unsigned v_pop;
    struct {
      DskZLangHandler handler;
      void *handler_data;
      unsigned n_args;
      DskDestroyNotify destroy;
    } v_call;
    struct {
      DskZLangCaseFunc case_func;
      void *case_data;
      DskDestroyNotify destroy;
    } v_case;
    DskZLangInsn *v_jump;
  } u;
};

typedef struct _DskZLangFunction DskZLangFunction;
struct _DskZLangFunction
{
  int n_args;                   /* -1 for any arity */
  DskZLangHandler handler;
  void *handler_data;
  DskDestroyNotify destroy;
  unsigned ref_count;
};
DskZLangFunction *dsk_zlang_function_new_c (int n_args,
                                            DskZLangHandler handler,
                                            void *handler_data,
                                            DskDestroyNotify destroy);
DskZLangFunction *dsk_zlang_function_new   (int           n_args,
                                            unsigned      n_insn,
                                            DskZLangInsn *insn);


void dsk_zlang_stack_push     (DskZLangStack     *stack,
                               DskZLangStackNode *node);
void dsk_zlang_stack_pop      (DskZLangStack     *stack,
                               unsigned           count);

DskZLangContext *dsk_zlang_context_new        (void);
dsk_boolean      dsk_zlang_context_parse_file (DskZLangContext *context,
                                               const char      *filename,
                                               DskZLangInsn   **main_out,
                                               DskError       **error);
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
