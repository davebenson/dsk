
/* --- file-descriptor mucking --- */
int      dsk_buffer_writev              (DskBuffer       *read_from,
                                         int              fd);
int      dsk_buffer_writev_len          (DskBuffer *read_from,
		                         int              fd,
		                         unsigned         max_bytes);
/* returns TRUE iff all the data was written.  'read_from' is blank. */
bool dsk_buffer_write_all_to_fd         (DskBuffer       *read_from,
                                         int              fd,
                                         DskError       **error);
int      dsk_buffer_readv               (DskBuffer       *write_to,
                                         int              fd);

typedef enum {
  DSK_BUFFER_DUMP_DRAIN = (1<<0),
  DSK_BUFFER_DUMP_NO_DRAIN = (1<<1),
  DSK_BUFFER_DUMP_FATAL_ERRORS = (1<<2),
  DSK_BUFFER_DUMP_LEAVE_PARTIAL = (1<<3),
  DSK_BUFFER_DUMP_NO_CREATE_DIRS = (1<<4),
  DSK_BUFFER_DUMP_EXECUTABLE = (1<<5),
} DskBufferDumpFlags;

bool dsk_buffer_dump (DskBuffer          *buffer,
                      const char         *filename,
                      DskBufferDumpFlags  flags,
                      DskError          **error);
                          
