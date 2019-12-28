typedef struct _DskFdStreamClass DskFdStreamClass;
typedef struct _DskFdStream DskFdStream;

// Readable stream.
DskStream *dsk_stream_new_stdin (void);
// Writable stream.
DskStream *dsk_stream_new_stdout (void);
// Readable/writable stream.
DskStream *dsk_stream_new_stdio (void);

struct _DskFdStreamClass
{
  DskStreamClass base_class;
};
struct _DskFdStream
{
  DskStream base_instance;
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

DskFdStream *dsk_fd_stream_new          (DskFileDescriptor fd,
                                         DskFileDescriptorStreamFlags flags,
                                         DskError        **error);

extern const DskFdStreamClass dsk_fd_stream_class;

#define DSK_FD_STREAM(object) DSK_OBJECT_CAST(DskFdStream, object, &dsk_fd_stream_class)
