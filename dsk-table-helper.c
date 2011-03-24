#define _ATFILE_SOURCE
#define _XOPEN_SOURCE 700
#define _ATFILE_SOURCE

#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "dsk.h"
#include "dsk-table-helper.h"

int dsk_table_helper_openat (const char *openat_dir,
                             int         openat_fd,
                             const char *base_filename,
                             const char *suffix,
                             unsigned    open_flags,
                             unsigned    open_mode,
                             DskError  **error)
{
  unsigned base_fname_len = strlen (base_filename);
  unsigned suffix_len = strlen (suffix);
  char slab[1024];
  char *buf;
  int fd;
#if defined(__USE_ATFILE)
  DSK_UNUSED (openat_dir);
#else
  unsigned openat_dir_len = strlen (openat_dir);
  DSK_UNUSED (openat_fd);
  base_fname_len += openat_dir_len + 1;
#endif
  if (base_fname_len + suffix_len < sizeof (slab) - 1)
    buf = slab;
  else
    buf = dsk_malloc (base_fname_len + suffix_len + 1);
#if defined(__USE_ATFILE)
  memcpy (buf + 0, base_filename, base_fname_len);
#else
  memcpy (buf + 0, openat_dir, openat_dir_len);
  buf[openat_dir_len] = '/';
  strcpy (buf + openat_dir_len + 1, base_filename);
#endif
  memcpy (buf + base_fname_len, suffix, suffix_len + 1);

#if defined(__USE_ATFILE)
  fd = openat (openat_fd, buf, open_flags, open_mode);
#define OPEN_SYSTEM_CALL "openat"
#else
  fd = open (buf, open_flags, open_mode);
#define OPEN_SYSTEM_CALL "open"
#endif
  if (fd < 0)
    {
      dsk_set_error (error, "error running %s %s: %s",
                     OPEN_SYSTEM_CALL, buf, strerror (errno));
      if (buf != slab)
        dsk_free (buf);
      return -1;
    }

  if (buf != slab)
    dsk_free (buf);
  return fd;
}

dsk_boolean dsk_table_helper_renameat (const char *openat_dir,
                                       int openat_fd,
                                       const char *old_name,
                                       const char *new_name,
                                       DskError  **error)
{
#if defined(__USE_ATFILE)
  DSK_UNUSED (openat_dir);
  if (renameat (openat_fd, old_name, openat_fd, new_name) < 0)
    {
      dsk_set_error (error, "error renameat()ing file: %s -> %s: %s",
                     old_name, new_name, strerror (errno));
      return DSK_FALSE;
    }
  return DSK_TRUE;

#else
  unsigned base_old_len = strlen (old_name);
  unsigned base_new_len = strlen (new_name);
  unsigned openat_dir_len = strlen (openat_dir);
  char old_slab[1024], new_slab[1024];
  char *old_buf, *new_buf;
  dsk_boolean rv = DSK_TRUE;
  DSK_UNUSED (openat_fd);
  if (openat_dir_len + base_new_len + 2 > sizeof (new_slab))
    new_buf = dsk_malloc (openat_dir_len + base_new_len + 2);
  else
    new_buf = new_slab;
  snprintf (new_buf, openat_dir_len + base_new_len + 2, "%s/%s",
            openat_dir, new_name);
  if (openat_dir_len + base_old_len + 2 > sizeof (old_slab))
    old_buf = dsk_malloc (openat_dir_len + base_old_len + 2);
  else
    old_buf = old_slab;
  snprintf (old_buf, openat_dir_len + base_old_len + 2, "%s/%s",
            openat_dir, old_name);
  if (rename (old_buf, new_buf) < 0)
    {
      dsk_set_error (error, "error rename()ing file: %s -> %s: %s",
                     old_buf, new_buf, strerror (errno));
      rv = DSK_FALSE;
    }
  if (new_buf != new_slab)
    dsk_free (new_slab);
  if (old_buf != old_slab)
    dsk_free (old_slab);
  return rv;
#endif
}

dsk_boolean
dsk_table_helper_unlinkat (const char *openat_dir,
                           int openat_fd,
                           const char *to_delete,
                           DskError  **error)
{
#if defined(__USE_ATFILE)
  DSK_UNUSED (openat_dir);
  if (unlinkat (openat_fd, to_delete, 0) < 0)
    {
      dsk_set_error (error, "error unlinking(at) file: %s: %s",
                     to_delete, strerror (errno));
      return DSK_FALSE;
    }
  return DSK_TRUE;
#else
  char buf[1024];
  unsigned base_new_len = strlen (to_delete);
  unsigned openat_dir_len = strlen (openat_dir);
  unsigned needed = base_new_len + 1 + openat_dir_len + 1;
  char *fname = needed <= sizeof (buf) ? buf : dsk_malloc (needed);
  snprintf (fname, needed, "%s/%s", openat_dir, to_delete);
  DSK_UNUSED (openat_fd);
  
  if (unlink (fname) < 0)
    {
      dsk_set_error (error, "error unlinking %s: %s",
                     fname, strerror (errno));
      if (fname != buf)
        dsk_free (fname);
      return DSK_FALSE;
    }
  if (fname != buf)
    dsk_free (fname);
  return DSK_TRUE;
#endif
}

int dsk_table_helper_pread  (int fd,
                             void *buf,
                             size_t len,
                             off_t offset)
{
  return pread (fd, buf, len, offset);
}

