
typedef struct _DskErrorClass DskErrorClass;
typedef struct _DskError DskError;

struct _DskErrorClass
{
  DskObjectClass base_class;
};

struct _DskError
{
  DskObject base_instance;
  char *message;
};

extern DskErrorClass dsk_error_class;

DskError *dsk_error_new        (const char *format,
                                ...);
DskError *dsk_error_new_valist (const char *format,
                                va_list     args);
DskError *dsk_error_new_literal(const char *message);
void      dsk_set_error        (DskError  **error,
                                const char *format,
                                ...) DSK_GNUC_PRINTF(2,3);
void      dsk_add_error_prefix (DskError  **error,
                                const char *format,
                                ...) DSK_GNUC_PRINTF(2,3);
void      dsk_add_error_suffix (DskError  **error,
                                const char *format,
                                ...) DSK_GNUC_PRINTF(2,3);
DskError *dsk_error_ref        (DskError   *error);
void      dsk_error_unref      (DskError   *error);
