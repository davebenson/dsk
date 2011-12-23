

typedef struct _DskLogger DskLogger;
typedef struct _DskLoggerOptions DskLoggerOptions;
struct _DskLoggerOptions
{
  int openat_fd;
  const char *openat_dir;
  const char *format;
  unsigned period;
  int tz_offset;
  unsigned max_buffer;
};
#define DSK_LOGGER_OPTIONS_INIT \
{ -1,                           \
  NULL,                         \
  "%Y%m%d-%H.log",              \
  3600,                         \
  0,                            \
  8192 }

DskLogger *dsk_logger_new         (DskLoggerOptions *options,
                                   DskError        **error);
DskBuffer *dsk_logger_peek_buffer (DskLogger        *logger);
void       dsk_logger_done_buffer (DskLogger        *logger);
void       dsk_logger_destroy     (DskLogger        *logger);


