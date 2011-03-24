#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "dsk-common.h"
#include "dsk-fd.h"

static inline void
set_flag_generic (DskFileDescriptor fd,
                  int               get_op,
                  int               set_op,
                  dsk_boolean       set,
                  long              flag)
{
  long flags;
retry_get:
  flags = fcntl (fd, get_op);
  if (flags == -1)
    {
      if (errno == EINTR)
        goto retry_get;
      dsk_die ("error getting flags from file-descriptor %u: %s",
               (unsigned) fd, strerror (errno));
    }
  if (set)
    flags |= flag;
  else
    flags &= ~flag;
retry_set:
  if (fcntl (fd, set_op, flags) == -1)
    {
      if (errno == EINTR)
        goto retry_set;
      dsk_die ("error setting flags for file-descriptor %u: %s",
               (unsigned) fd, strerror (errno));
    }
}

void
dsk_fd_set_nonblocking (DskFileDescriptor fd)
{
  set_flag_generic (fd, F_GETFL, F_SETFL, DSK_TRUE, O_NONBLOCK);
}

void
dsk_fd_set_close_on_exec (DskFileDescriptor fd)
{
  set_flag_generic (fd, F_GETFD, F_SETFD, DSK_TRUE, FD_CLOEXEC);
}

void
dsk_fd_set_blocking (DskFileDescriptor fd)
{
  set_flag_generic (fd, F_GETFL, F_SETFL, DSK_FALSE, O_NONBLOCK);
}

void
dsk_fd_set_no_close_on_exec (DskFileDescriptor fd)
{
  set_flag_generic (fd, F_GETFD, F_SETFD, DSK_FALSE, FD_CLOEXEC);
}

static void
default_too_many_fd_handler (void)
{
  fprintf (stderr, "ERROR: out of file-descriptors!\n");
  abort ();
}

void (*dsk_too_many_fds) (void) = default_too_many_fd_handler;
