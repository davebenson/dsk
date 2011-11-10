
void dsk_printf (const char *format,
                 ...) DSK_GNUC_PRINT(1,2);


typedef void (*DskPrintfAppendFunc) (void          *append_data,
                                     unsigned       length,
				     const uint8_t *data);



void dsk_printf_valist (const char *format,
                        ...) DSK_GNUC_PRINT(1,2);


/* defining new printf-like mechanisms */
typedef enum
{
  DSK_PRINTF_COLLECT_INT      = 'i',
  DSK_PRINTF_COLLECT_POINTER  = 'p',
  DSK_PRINTF_COLLECT_SIZE_T   = 'z',
  DSK_PRINTF_COLLECT_INT64    = 'q',
  DSK_PRINTF_COLLECT_DOUBLE   = 'f'
} DskPrintfCollectType;

typedef struct _DskPrintfCollectArg DskPrintfCollectArg;
struct _DskPrintfCollectArg
{
  DskPrintfCollectType type;
  union {
    int i;
    void *p;
    size_t z;
    int64_t q;
    double d;
  } info;
};
typedef void (*DskPrintfFunc) (unsigned n_args,
                               DskPrintfCollectArg *args,
			       const char *format,
			       DskPrintfAppendFunc append,
			       void *append_data,
			       void *print_func_data);

