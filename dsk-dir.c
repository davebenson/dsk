/* TODO: deprecate dsk-file-utils since these have equiv functionality if dir==NULL. */   

#include <alloca.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/file.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "dsk.h"




/* openat() and friends implemented in a portable way.
   Systems without "atfile" support use normal system calls. */


struct _DskDir
{
  int openat_fd;
  char *openat_dir;
  unsigned openat_dir_len;

  unsigned ref_count;

  unsigned buf_alloced;
  char *buf;

  /* alt_buf_alloced, alt_buf are used for rename() which requires
     two filename slots. */
  unsigned alt_buf_alloced;
  char *alt_buf;

  dsk_boolean locked;
};

DskDir      *dsk_dir_new                     (DskDir       *parent,
                                              const char   *dir,
                                              DskDirNewFlags flags,
                                              DskError    **error)
{
  int fd = dsk_dir_sys_open (parent, dir, O_RDONLY, 0);
  if (fd < 0)
    {
      if (errno == ENOENT && (flags & DSK_DIR_NEW_MAYBE_CREATE) != 0)
        {
          if (! dsk_dir_mkdir (parent, dir, 0, NULL, error))
            {
              return NULL;
            }
          fd = dsk_dir_sys_open (parent, dir, O_RDONLY, 0);
        }

      if (fd < 0)
        {
          /* XXX: should check out-of-fd errors! */
          dsk_set_error (error, "error opening directory %s: %s",
                         dir, strerror (errno));
          return NULL;
        }
    }
  dsk_fd_set_close_on_exec (fd);
  if ((flags & DSK_DIR_NEW_SKIP_LOCKING) != 0)
    {
      if (flock (fd, LOCK_EX|((flags & DSK_DIR_NEW_BLOCK_LOCKING) ? 0 : LOCK_NB)) < 0)
        {
          dsk_set_error (error, "error locking directory %s: %s",
                         dir, strerror (errno));
          close (fd);
          return NULL;
        }
    }

  DskDir *rv;
  rv = DSK_NEW (DskDir);
  rv->openat_fd = fd;
  rv->openat_dir = dsk_strdup (dir);
  rv->openat_dir_len = strlen (dir);
  rv->ref_count = 1;
#if DSK_HAS_ATFILE_SUPPORT
  rv->buf_alloced = 0;
  rv->buf = NULL;
#else
  rv->buf_alloced = rv->openat_dir_len + 64;
  rv->buf = dsk_malloc (rv->buf_alloced);
  memcpy (rv->buf, dir, rv->openat_dir_len);
  rv->buf[rv->openat_dir_len] = '/';
#endif
  return rv;
}

void
dsk_dir_unref (DskDir *dir)
{
  if (dir == NULL)
    return;
  dsk_assert (dir->ref_count > 0);
  if (--(dir->ref_count) != 0)
    return;
  close (dir->openat_fd);
  dsk_free (dir->alt_buf);
  dsk_free (dir->buf);
  dsk_free (dir->openat_dir);
  dsk_free (dir);
}

DskDir *
dsk_dir_ref (DskDir *dir)
{
  if (dir)
    dir->ref_count += 1;
  return dir;
}

/* note: 'alloc' must be long enough to hold openat_dir_len+1 */
#if !DSK_HAS_ATFILE_SUPPORT
static void
resize_buf (DskDir *dir,
            unsigned alloc)
{
  dir->buf_alloced = alloc;
  dsk_free (dir->buf);
  dir->buf = dsk_malloc (alloc);
  memcpy (dir->buf, dir->openat_dir, dir->openat_dir_len);
  dir->buf[dir->openat_dir_len] = DSK_DIR_SEPARATOR;
}

static const char *
prep_buf_with_path (DskDir *dir,
                    const char *path)
{
  unsigned len = strlen (path);
  unsigned min_alloced = dir->openat_dir_len + 1 + len + 1;
  if (min_alloced > dir->buf_alloced)
    resize_buf (dir, (min_alloced + 128) & ~63);
  memcpy (dir->buf + dir->openat_dir_len + 1, path, len + 1);
  return dir->buf;
}

static void
swap_alt_buf (DskDir *dir)
{
  char *tmp = dir->alt_buf;
  unsigned tmp_alloced = dir->alt_buf_alloced;
  dir->alt_buf = dir->buf;
  dir->alt_buf_alloced = dir->buf_alloced;
  dir->buf = tmp;
  dir->buf_alloced = tmp_alloced;
}
#endif

int          dsk_dir_sys_open                (DskDir       *dir,
                                              const char   *path,
                                              unsigned      flags,
			                      unsigned      mode)
{
  if (dir == NULL)
    return open (path, flags, mode);
#if DSK_HAS_ATFILE_SUPPORT
  /* XXX: should check out-of-fd errors! */
  return openat (dir->openat_fd, path, flags, mode);
#else
  
  return open (prep_buf_with_path (dir, path), flags, mode);
#endif
}


int          dsk_dir_sys_rename              (DskDir       *dir,
                                              const char   *old_path,
                                              const char   *new_path)
{
  if (dir == NULL)
    return rename (old_path, new_path);
#if DSK_HAS_ATFILE_SUPPORT
  return renameat (dir->openat_fd, old_path, dir->openat_fd, new_path);
#else
  const char *o = prep_buf_with_path (dir, old_path);
  swap_alt_buf (dir);
  const char *n = prep_buf_with_path (dir, new_path);
  int rv = rename (o, n);
  swap_alt_buf (dir);
  return rv;
#endif
}

int          dsk_dir_sys_rename2             (DskDir       *old_dir,
                                              const char   *old_path,
                                              DskDir       *new_dir,
                                              const char   *new_path)
{
#if DSK_HAS_ATFILE_SUPPORT
  return renameat (old_dir ? old_dir->openat_fd : AT_FDCWD,
                   old_path,
                   new_dir ? new_dir->openat_fd : AT_FDCWD,
                   new_path);
#else
  if (old_dir == new_dir)
    return dsk_dir_sys_rename (old_dir, old_path, new_path);
  else
    {
      const char *o = old_dir ? prep_buf_with_path (old_dir, old_path) : old_path;
      const char *n = new_dir ? prep_buf_with_path (new_dir, new_path) : new_path;
      return rename (o, n);
    }
#endif
}

int          dsk_dir_sys_unlink              (DskDir       *dir,
                                              const char   *path)
{
  if (dir == NULL)
    return unlink (path);
#if DSK_HAS_ATFILE_SUPPORT
  return unlinkat (dir->openat_fd, path, 0);
#else
  
  return unlink (prep_buf_with_path (dir, path));
#endif
}

int          dsk_dir_sys_rmdir               (DskDir       *dir,
                                              const char   *path)
{
  if (dir == NULL)
    return rmdir (path);
#if DSK_HAS_ATFILE_SUPPORT
  return unlinkat (dir->openat_fd, path, AT_REMOVEDIR);
#else
  return rmdir (prep_buf_with_path (dir, path));
#endif
}

#if 0
//xxx
typedef enum
{
  DSK_DIR_RM_FORCE = (1<<0),
  DSK_DIR_RM_RECURSIVE = (1<<1),

  DSK_DIR_RM_RF = DSK_DIR_RM_FORCE|DSK_DIR_RM_RECURSIVE,
} DskDirRmFlags;

int          dsk_dir_sys_rm                  (DskDir       *dir,
                                              const char   *path,
                                              DskDirRmFlags flags)
{
  ...
}

typedef enum
{
  DSK_DIR_CP_FORCE = (1<<0),
  DSK_DIR_CP_RECURSIVE = (1<<1),

  DSK_DIR_CP_RF = DSK_DIR_CP_FORCE|DSK_DIR_CP_RECURSIVE,
} DskDirCpFlags;
int          dsk_dir_sys_cp                  (DskDir       *dir,
                                              const char   *src,
                                              const char   *dst,
                                              DskDirCpFlags flags);
#endif

dsk_boolean dsk_dir_mkdir       (DskDir     *dir,
                                 const char *path,
                                 DskDirMkdirFlags flags,
                                 DskDirMkdirStats *stats,
                                 DskError  **error)
{
  unsigned perms = 0777;
  unsigned dirs_made = 0;

  /* Handle non-recursive case. */
  if ((flags & DSK_DIR_MKDIR_NONRECURSIVE) == DSK_DIR_MKDIR_NONRECURSIVE)
    {
      if (dsk_dir_sys_mkdir (dir, path, perms) < 0)
        {
          dsk_set_error (error, "mkdir %s: %s", path, strerror (errno));
          return DSK_FALSE;
        }
      ++dirs_made;
      if (stats)
        {
          stats->dirs_made = dirs_made;
        }
      return DSK_TRUE;
    }

  /* Normalize path:  remove "." and ".."; remove multiple consecutive slashes. */
  unsigned orig_path_len = strlen (path);
  char *buf = alloca (orig_path_len + 1);
  memcpy (buf, path, orig_path_len + 1);
  if (!dsk_path_normalize_inplace (buf))
    {
      dsk_set_error (error, "bad path");
      return DSK_FALSE;
    }
  if (buf[0] == DSK_DIR_SEPARATOR)
    buf++;

  /* trim trailing slash */
  {
    char *end = strchr (buf, 0);
    if (end > buf && *(end-1) == DSK_DIR_SEPARATOR)
      *(end-1) = 0;
  }

  /* count slashes: result is 1 less than # of components */
  unsigned n_comp = 1;
  {
    const char *at = buf;
    while (*at)
      {
        at = strchr (at, DSK_DIR_SEPARATOR);
        if (at != NULL)
          {
            n_comp++;
            at++;
          }
      }
  }

  /* init components[] */
  char **slashes = alloca (sizeof (char*) * n_comp);
  {
    unsigned i = 0;
    char *at = buf;
    while (at)
      {
        at = strchr (at, DSK_DIR_SEPARATOR);
        if (at != NULL)
          slashes[i++] = at++;
      }
  }

  {
    unsigned i;
    for (i = n_comp; i != 0; )
      {
        if (dsk_dir_sys_mkdir (dir, buf, perms) == 0)
          {
            ++dirs_made;
            break;
          }
        else if (errno == EEXIST)
          break;
        else if (errno == ENOENT)
          {
            /* trim path */
            i--;
            *(slashes[i-1]) = 0;
          }
        else
          {
            dsk_set_error (error, "mkdir %s recursive: error making %s: %s", path, buf, strerror (errno));
            return DSK_FALSE;
          }
      }
    while (i < n_comp)
      {
        *(slashes[i-1]) = DSK_DIR_SEPARATOR;
        i++;
        if (dsk_dir_sys_mkdir (dir, buf, perms) == 0)
          continue;
        dsk_set_error (error, "mkdir %s recursive: error making %s: %s", path, buf, strerror (errno));
        return DSK_FALSE;
      }
      if (stats)
        {
          stats->dirs_made = dirs_made;
        }
  }
  return DSK_TRUE;
}

#define MAX_RETRIES 128

dsk_boolean
dsk_dir_set_contents (DskDir                 *dir,
                      const char             *path,
                      unsigned                mode,
                      DskDirSetContentsFlags  flags,
                      size_t                  data_length,
                      const uint8_t          *data,
                      DskError              **error)
{
  dsk_boolean use_tmp = (flags & DSK_DIR_SET_CONTENTS_NO_TMP_FILE) == 0;
  DskDirOpenfdFlags openfd_flags =
    ((flags & DSK_DIR_SET_CONTENTS_NO_MKDIR) ? DSK_DIR_OPENFD_NO_MKDIR : 0);
  int fd = -1;
  char *tmp_path = NULL;
  unsigned retries = 0;

  /* open a tmp file in the same dir */
  if (use_tmp)
    {
      openfd_flags |= DSK_DIR_OPENFD_MUST_CREATE;
      while (fd < 0)
        {
          DskError *er = NULL;
          tmp_path = dsk_strdup_printf ("%s.%08x", path, dsk_random_uint32 ());
          fd = dsk_dir_openfd (dir, tmp_path, mode, openfd_flags, &er);
          if (fd < 0)
            {
              int e;
              dsk_free (tmp_path);
              if (dsk_error_get_errno (er, &e) && e == EEXIST && retries++ < MAX_RETRIES)
                {
                  dsk_error_unref (er);
                  continue;
                }
              dsk_propagate_error (error, er);
              return DSK_FALSE;
            }
        }
    }
  else
    {
      openfd_flags |= DSK_DIR_OPENFD_MAY_CREATE|DSK_DIR_OPENFD_TRUNCATE;
      fd = dsk_dir_openfd (dir, path, mode, openfd_flags, error);
      if (fd < 0)
        return DSK_FALSE;
    }

  /* write data */
  size_t at = 0;
  while (at < data_length)
    {
      ssize_t write_rv = write (fd, data + at, data_length - at);
      if (write_rv < 0)
        {
          if (errno == EINTR)
            continue;
          dsk_set_error (error, "error writing to %s: %s", path, strerror (errno));
          dsk_dir_sys_unlink (dir, tmp_path);
          return DSK_FALSE;
        }
      at += write_rv;
    }

  close (fd);
  
  /* move tmp file to real file */
  if (use_tmp)
    {
      if (dsk_dir_sys_rename (dir, tmp_path, path) < 0)
        {
          dsk_set_error (error, "error renaming %s to %s: %s", tmp_path, path, strerror (errno));
          dsk_free (tmp_path);
          return DSK_FALSE;
        }
      dsk_free (tmp_path);
    }
  return DSK_TRUE;
}
