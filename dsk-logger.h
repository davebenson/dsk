

typedef struct _DskLogger DskLogger;
typedef struct _DskLoggerOptions DskLoggerOptions;
struct _DskLoggerOptions
{
  DskDir *dir;
  const char *format;
  unsigned period;
  int tz_offset;
  unsigned max_buffer;
  dsk_boolean autonewline;
};
#define DSK_LOGGER_OPTIONS_INIT \
{ NULL,                         \
  "%Y%m%d-%H.log",              \
  3600,                         \
  0,                            \
  8192,                         \
  DSK_TRUE }

DskLogger *dsk_logger_new         (DskLoggerOptions *options,
                                   DskError        **error);
DskBuffer *dsk_logger_peek_buffer (DskLogger        *logger);
void       dsk_logger_done_buffer (DskLogger        *logger);
void       dsk_logger_destroy     (DskLogger        *logger);

/* helper functions */
void       dsk_logger_printf      (DskLogger        *logger,
                                   const char       *format,
                                   ...) DSK_GNUC_PRINTF(2,3);



#if 0
/* this can be a misleading function:
 * it's better to use peek_buffer/done_buffer and dsk_buffer_vprintf().
 * the problem is that you shouldn't call dsk_logger_printf()
 * multiply times to concatenate messages. */
void       dsk_logger_vprintf     (DskLogger        *logger,
                                   const char       *format,
                                   va_list           args);
#endif
