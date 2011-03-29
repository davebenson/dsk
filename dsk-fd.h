
#ifdef WIN32
typedef SOCKET DskFileDescriptor;
#else
typedef int DskFileDescriptor;
#endif


void dsk_fd_set_nonblocking (DskFileDescriptor fd);
void dsk_fd_set_close_on_exec (DskFileDescriptor fd);
void dsk_fd_set_blocking (DskFileDescriptor fd);
void dsk_fd_set_no_close_on_exec (DskFileDescriptor fd);

/* A settable handler for when we are out of file-descriptors.
   It must deallocate file-descriptors or terminate. */
typedef void (*DskOutOfFileDescriptorsFunc)(void);
extern DskOutOfFileDescriptorsFunc dsk_too_many_fds;

DSK_INLINE_FUNC dsk_boolean dsk_fd_creation_failed (int e)
{
  if (e == ENFILE || e == EMFILE)
    {
      dsk_too_many_fds ();
      return DSK_TRUE;
    }
  return DSK_FALSE;
}
