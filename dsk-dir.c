/* TODO: deprecate dsk-file-utils since these have equiv functionality if dir==NULL. */   
/// 'cept tmpdir()

#include <alloca.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/file.h>
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

  unsigned locked : 1;
  unsigned erase_on_destroy : 1;
  unsigned did_create : 1;

  unsigned ref_count;

  unsigned buf_alloced;
  char *buf;

  /* alt_buf_alloced, alt_buf are used for rename() which requires
     two filename slots. */
  unsigned alt_buf_alloced;
  char *alt_buf;
};

DskDir      *dsk_dir_new                     (DskDir       *parent,
                                              const char   *dir,
                                              DskDirNewFlags flags,
                                              DskError    **error)
{
  int fd = dsk_dir_sys_open (parent, dir, O_RDONLY, 0);
  dsk_boolean did_create = DSK_FALSE;
  if (fd < 0)
    {
      // TODO: dsk_fd_creation_failed()
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
      did_create = DSK_TRUE;
    }
  dsk_fd_set_close_on_exec (fd);
  dsk_boolean do_lock = (flags & DSK_DIR_NEW_SKIP_LOCKING) == 0;
  if (do_lock)
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
  rv->alt_buf_alloced = 0;
  rv->alt_buf = NULL;
#endif
  rv->locked = do_lock;
  rv->erase_on_destroy = DSK_FALSE;
  rv->did_create = did_create;
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
  if (dir->erase_on_destroy)
    {
      struct stat stat_buf, stat_buf2;
      if (stat (dir->openat_dir, &stat_buf) >= 0
       && fstat (dir->openat_fd, &stat_buf2) >= 0
       && stat_buf.st_dev == stat_buf2.st_dev
       && stat_buf.st_ino == stat_buf2.st_ino)
        {
          (void) dsk_remove_dir_recursive (dir->openat_dir, NULL);
        }
    }
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

void
dsk_dir_set_erase_on_destroy (DskDir *dir)
{
  dir->erase_on_destroy = DSK_TRUE;
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

int          dsk_dir_sys_mkdir               (DskDir         *dir,
                                              const char     *path,
                                              unsigned        mode)
{
  if (dir == NULL)
    return mkdir (path, mode);
#if DSK_HAS_ATFILE_SUPPORT
  return mkdirat (dir->openat_fd, path, mode);
#else
  return mkdir (prep_buf_with_path (dir, path), mode);
#endif
}

const char *dsk_dir_get_str (DskDir *dir)
{
  return dir ? dir->openat_dir : ".";
}
dsk_boolean dsk_dir_did_create (DskDir *dir)
{
  return dir ? dir->did_create : DSK_FALSE;
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

  char *sbuf = buf;
  if (sbuf[0] == DSK_DIR_SEPARATOR)
    sbuf++;

  /* trim trailing slash */
  {
    char *end = strchr (buf, 0);
    if (end > buf && *(end-1) == DSK_DIR_SEPARATOR)
      *(end-1) = 0;
  }

  /* count slashes: result is 1 less than # of components */
  unsigned n_comp = 1;
  {
    const char *at = sbuf;
    while (*at)
      {
        at = strchr (at, DSK_DIR_SEPARATOR);
        if (at != NULL)
          {
            n_comp++;
            at++;
          }
        else
          break;
      }
  }

  /* init components[] */
  char **slashes = alloca (sizeof (char*) * n_comp);
  {
    unsigned i = 0;
    char *at = sbuf;
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

static const char *
filepath_to_directory_end (const char *path)
{
  const char *end = strrchr (path, '/');
  if (end == NULL)
    return NULL;
  const char *init_end;
  do {
    init_end = end;
    if (end >= path + 2 && *(end-1) == '.' && *(end-2) == '/')
      {
        end -= 2;
      }
    else if (end >= path + 3 && *(end-1) == '.' && *(end-2) == '.' && *(end-3) == '/')
      {
        const char *ne = dsk_memrchr(path, '/', end - path - 3);
        if (ne == NULL)
          return NULL;
        end = ne;
      }
  } while (end != init_end);
  if (end == path)
    return NULL;
  return end;
}
    

int
dsk_dir_openfd (DskDir            *dir,
                const char        *path,
                DskDirOpenfdFlags  flags,
                unsigned           mode,
                DskError         **error)
{
  unsigned sys_flags = 0;

  if (flags & DSK_DIR_OPENFD_MUST_CREATE)
    sys_flags |= O_EXCL | O_CREAT;
  if (flags & DSK_DIR_OPENFD_MAY_CREATE)
    sys_flags |= O_CREAT;

  // XXX: no way to force non-readable
  if (flags & DSK_DIR_OPENFD_WRITABLE)
    sys_flags |= O_RDWR;
  else
    sys_flags |= O_RDONLY;

  if (flags & DSK_DIR_OPENFD_TRUNCATE)
    sys_flags |= O_TRUNC;
  if (flags & DSK_DIR_OPENFD_APPEND)
    sys_flags |= O_APPEND;

  dsk_boolean tried_mkdir = DSK_FALSE;
  int fd;
do_try_open_fd:
  fd = dsk_dir_sys_open(dir, path, sys_flags, mode);
  if (fd < 0)
    {
      const char *end;
      int e = errno;
      if (dsk_fd_creation_failed(e))
        goto do_try_open_fd;
      if (e == ENOENT
       && !tried_mkdir
       && (flags & DSK_DIR_OPENFD_MAY_CREATE) != 0
       && (flags & DSK_DIR_OPENFD_NO_MKDIR) == 0
       && (end = filepath_to_directory_end (path)) != NULL)
        {
          char *dirpath = dsk_strdup_slice (path, end);
          if (!dsk_dir_mkdir (dir, dirpath, 0, NULL, error))
            {
              dsk_free (dirpath);
              return -1;
            }
          tried_mkdir = DSK_TRUE;
          goto do_try_open_fd;
        }
      else
        {
           dsk_set_error (error, "error opening file %s: %s", path, strerror (errno));
           return -1;
        }
    }
  else
    return fd;
}

#define MAX_RETRIES 128

dsk_boolean
dsk_dir_set_contents (DskDir                 *dir,
                      const char             *path,
                      DskDirSetContentsFlags  flags,
                      unsigned                mode,
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
          fd = dsk_dir_openfd (dir, tmp_path, openfd_flags, mode, &er);
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
      fd = dsk_dir_openfd (dir, path, openfd_flags, mode, error);
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

DIR *dsk_dir_sys_opendir (DskDir *dir, const char *path)
{
#if DSK_HAS_ATFILE_SUPPORT
  int flags = 0;
# if __APPLE__ && __MACH__
  flags = O_DIRECTORY;
# endif
  int fd;
retry_sys_open:
  fd = dsk_dir_sys_open (dir, path, flags, 0);
  if (fd < 0)
    {
      if (dsk_fd_creation_failed ())
        goto retry_sys_open;
      return NULL;
    }
  return fdopendir (fd);
#else
  return opendir (prep_buf_with_path (dir, path));
#endif
}
  

// TODO: handle EINTR
static dsk_boolean
rmdir_recursive (DskDir *dir, const char *path, DskDirRmStats *stats, DskError**error)
{
  DIR *d = dsk_dir_sys_opendir (dir, path);
  if (d == NULL)
    {
      dsk_set_error (error, "fdopendir failed: %s", strerror (errno));
      if (error)
        dsk_error_set_errno (*error, errno);
      return DSK_FALSE;
    }
  const struct dirent *n;
  while ((n=readdir(d)) != NULL)
    {
      // Skip . and ..
      const char *name = n->d_name;
      if (name[0] == '.')
        {
          if (name[1] == 0)
            continue;
          if (name[1] == '.' && name[2] == 0)
            continue;
        }

      // TODO: could optimize
      char *subpath = dsk_strdup_printf ("%s/%s", path, name);

      if (dsk_dir_sys_unlink (dir, subpath) < 0)
        {
          if (errno == EISDIR)
            {
              if (!rmdir_recursive (dir, subpath, stats, error))
                {
                  dsk_free (subpath);
                  return DSK_FALSE;
                }
            }
          else
            {
              dsk_set_error (error, "unlink %s: %s", subpath, strerror (errno));
              dsk_free (subpath);
              return DSK_FALSE;
            }
        }
      else
        stats->n_files_deleted += 1;
      dsk_free (subpath);
    }
  if (dsk_dir_sys_rmdir (dir, path) < 0)
    {
      dsk_set_error (error, "rmdir %s: %s", path, strerror (errno));
      return DSK_FALSE;
    }
  stats->n_dirs_deleted += 1;
  return DSK_TRUE;
}

dsk_boolean  dsk_dir_rm                      (DskDir       *dir,
                                              const char   *path,
                                              DskDirRmFlags flags,
                                              DskDirRmStats*stats_out_optional,
                                              DskError    **error)
{
  DskDirRmStats stats = {0,0};
  dsk_boolean rv = DSK_TRUE;
retry_unlink:
  if (dsk_dir_sys_unlink (dir, path) < 0)
    {
      int e = errno;
      if (e == EINTR)
        goto retry_unlink;
      if (e == EISDIR && (flags & DSK_DIR_RM_RECURSIVE) != 0)
        {
          rv = rmdir_recursive (dir, path, &stats, error);
          goto do_return;
        }
      if (e == ENOENT && (flags & DSK_DIR_RM_IGNORE_MISSING) != 0)
        {
          goto do_return;
        }
      dsk_set_error (error, "error removing '%s': %s", path, strerror (errno));
      if (error)
        dsk_error_set_errno (*error, e);
      rv = DSK_FALSE;
      goto do_return;
    }
  stats.n_files_deleted += 1;
do_return:
  if (stats_out_optional)
    *stats_out_optional = stats;
  return rv;
}
  
