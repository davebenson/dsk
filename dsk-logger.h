

typedef struct _DskLogger DskLogger;
typedef struct _DskLoggerOptions DskLoggerOptions;
struct _DskLoggerOptions
{
  DskDir *dir;
  const char *format;
  unsigned period;
  int tz_offset;
  unsigned max_buffer;
};
#define DSK_LOGGER_OPTIONS_INIT \
{ NULL,                         \
  "%Y%m%d-%H.log",              \
  3600,                         \
  0,                            \
  8192 }

DskLogger *dsk_logger_new         (DskLoggerOptions *options,
                                   DskError        **error);
DskBuffer *dsk_logger_peek_buffer (DskLogger        *logger);
void       dsk_logger_done_buffer (DskLogger        *logger);
void       dsk_logger_destroy     (DskLogger        *logger);


