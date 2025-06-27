
typedef struct _DskErrorClass DskErrorClass;
typedef struct _DskError DskError;
typedef struct _DskErrorData DskErrorData;
typedef struct _DskErrorDataType DskErrorDataType;

struct _DskErrorClass
{
  DskObjectClass base_class;
};

struct _DskError
{
  DskObject base_instance;
  char *message;

  DskErrorData *data_list;
};


// ErrorData are information attached to the DskError
// instance, like causal error, etc.
struct _DskErrorData
{
  DskErrorDataType *type;
  DskErrorData *next;
  /* data follows */
};
struct _DskErrorDataType
{
  const char *name;
  size_t size;
  void (*clear) (DskErrorDataType *type, void *data);
  void (*copy) (DskErrorDataType *type, void *dst_data, const void *src_data);
  void (*to_string)(DskErrorDataType *type, const void *data, DskBuffer *buffer);
};

extern DskErrorClass dsk_error_class;

DskError   *dsk_error_new        (const char   *format,
                                  ...);
DskError   *dsk_error_new_with_cause
                                 (DskError     *cause,
		                  const char   *format,
                                  ...);
DskError   *dsk_error_new_valist (const char   *format,
                                  va_list       args);
DskError   *dsk_error_new_literal(const char   *message);
void        dsk_set_error        (DskError    **error,
                                  const char   *format,
                                  ...) DSK_GNUC_PRINTF(2,3);
void        dsk_add_error_prefix (DskError    **error,
                                  const char   *format,
                                  ...) DSK_GNUC_PRINTF(2,3);
void        dsk_add_error_suffix (DskError    **error,
                                  const char   *format,
                                  ...) DSK_GNUC_PRINTF(2,3);
void       *dsk_error_force_data (DskError     *error,
                                  DskErrorDataType *data_type,
                                  bool         *created_out);
void       *dsk_error_find_data  (DskError     *error,
                                  DskErrorDataType *data_type);
bool        dsk_error_remove_data(DskError     *error,
                                  DskErrorDataType *data_type);
DskError   *dsk_error_ref        (DskError     *error);
void        dsk_error_unref      (DskError     *error);

void        dsk_error_to_buffer  (DskError     *error,
		                  DskBuffer    *buffer);
char      * dsk_error_to_string  (DskError     *error);

// A new DskError object with all data cloned.
DskError   *dsk_error_clone      (DskError     *error);

// Move 'src' to '*dst', or, free 'src' if 'dest' is NULL.
void        dsk_propagate_error  (DskError    **dst,
                                  DskError     *src);

extern DskErrorDataType dsk_error_data_type_errno;
void        dsk_error_set_errno  (DskError     *error,
                                  int           error_no);
bool        dsk_error_get_errno  (DskError     *error,
                                  int          *error_no_out);

extern DskErrorDataType dsk_error_data_type_cause;
void        dsk_error_set_cause  (DskError     *error,
                                  DskError     *cause);
DskError *  dsk_error_get_cause  (DskError     *error);
