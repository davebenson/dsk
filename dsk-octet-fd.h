/* TODO: dsk_octet_stream_new_fd() must somehow return refs to
 * the sink/source (optionally, at least)  but since the DskOctetStreamFd
 * does not hold refs to the sink/source, it isn't an adequate of handling this
 */

typedef struct _DskOctetStreamFdSourceClass DskOctetStreamFdSourceClass;
typedef struct _DskOctetStreamFdSource DskOctetStreamFdSource;
typedef struct _DskOctetStreamFdSinkClass DskOctetStreamFdSinkClass;
typedef struct _DskOctetStreamFdSink DskOctetStreamFdSink;
typedef struct _DskOctetStreamFdClass DskOctetStreamFdClass;
typedef struct _DskOctetStreamFd DskOctetStreamFd;

DskOctetSource *dsk_octet_source_new_stdin (void);
DskOctetSink *dsk_octet_sink_new_stdout (void);

struct _DskOctetStreamFdSourceClass
{
  DskOctetSourceClass base_class;
};
struct _DskOctetStreamFdSource
{
  DskOctetSource base_instance;
};

struct _DskOctetStreamFdSinkClass
{
  DskOctetSinkClass base_class;
};
struct _DskOctetStreamFdSink
{
  DskOctetSink base_instance;
};

struct _DskOctetStreamFdClass
{
  DskOctetStreamClass base_class;
};
struct _DskOctetStreamFd
{
  DskOctetStream base_instance;
  DskFileDescriptor fd;
  unsigned do_not_close : 1;
  unsigned is_pollable : 1;
};
typedef enum
{
  DSK_FILE_DESCRIPTOR_IS_READABLE              = (1<<0),
  DSK_FILE_DESCRIPTOR_IS_NOT_READABLE          = (1<<1),
  DSK_FILE_DESCRIPTOR_IS_WRITABLE              = (1<<2),
  DSK_FILE_DESCRIPTOR_IS_NOT_WRITABLE          = (1<<3),
  DSK_FILE_DESCRIPTOR_IS_POLLABLE              = (1<<4),
  DSK_FILE_DESCRIPTOR_IS_NOT_POLLABLE          = (1<<5),
  DSK_FILE_DESCRIPTOR_DO_NOT_CLOSE             = (1<<16)
} DskFileDescriptorStreamFlags;

dsk_boolean     dsk_octet_stream_new_fd (DskFileDescriptor fd,
                                         DskFileDescriptorStreamFlags flags,
                                         DskOctetStreamFd **stream_out,
                                         DskOctetStreamFdSource **source_out,
                                         DskOctetStreamFdSink **sink_out,
                                         DskError        **error);

extern const DskOctetStreamFdSinkClass dsk_octet_stream_fd_sink_class;
extern const DskOctetStreamFdSourceClass dsk_octet_stream_fd_source_class;
extern const DskOctetStreamFdClass dsk_octet_stream_fd_class;

#define DSK_OCTET_STREAM_FD_SOURCE(object) DSK_OBJECT_CAST(DskOctetStreamFdSource, object, &dsk_octet_stream_fd_source_class)
#define DSK_OCTET_STREAM_FD_SINK(object) DSK_OBJECT_CAST(DskOctetStreamFdSink, object, &dsk_octet_stream_fd_sink_class)
#define DSK_OCTET_STREAM_FD(object) DSK_OBJECT_CAST(DskOctetStreamFd, object, &dsk_octet_stream_fd_class)
