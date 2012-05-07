
typedef enum
{
  JM_VALUE_BOOLEAN,            /* NOTE: matches DSK_JSON_VALUE_* */
  JM_VALUE_NULL,
  JM_VALUE_OBJECT,
  JM_VALUE_ARRAY,
  JM_VALUE_STRING,
  JM_VALUE_NUMBER,

  /* Our custom types */
  JM_VALUE_FUNCTION = DSK_N_JSON_VALUE_TYPES,
  JM_VALUE_CONTINUATION        /* only found on stack */
} JmValueType;


union _JmValue
{
  JmValueType type;
  DskJsonValueBoolean v_boolean;
  DskJsonValueObject v_object;
  DskJsonValueArray v_array;
  DskJsonValueString v_string;
  DskJsonValueNumber v_number;
  JmJsonValueFunction v_function;
};

struct _JmStackNode
{
  JmStackVar *variables;
  JmScopeStack *stack;
};

struct _JmScript
{
...
};

JmScript   *jm_script_parse     (const char      *str,
                                 DskError       **error);
JmScript   *jm_script_parse_str (const char      *str,
                                 const char      *filename,
			         unsigned         starting_line_no,
                                 DskError       **error);
typedef enum
{
  JM_RUN_RESULT_SUCCESS,
  JM_RUN_RESULT_ERROR,
  JM_RUN_RESULT_BLOCKED
} JmRunResultType;

struct _JmRunResult
{
  JmRunResultType type;
  union {
    DskJsonValue *success;
    DskError *error;
  } info;
};

void         jm_script_run       (JmScript       *script,
                                  JmContext      *eval_context,
				  JmRunResult    *result);

typedef enum
{
  JM_CONTEXT_READY,
  JM_CONTEXT_RUNNING,
  JM_CONTEXT_BLOCKED
} JmContextState;

struct _JmBlockage
{
  unsigned ns;
  void *data;
};

/* For most contexts, the parent context is the global one,
   but it can chain.
struct _JmContext
{
  JmContext *parent;
  JmContextState state;
  JmBlockage blockage;
};

void       jm_context_continue   (JmContext    *context,
                                  JmRunResult  *result);


/* --- for implementing function --- */
void      *jm_context_alloc      (JmContext    *context,
                                  size_t        size);
