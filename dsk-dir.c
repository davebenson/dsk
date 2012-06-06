

/* openat() and friends implemented in a portable way.
   Systems without "atfile" support use normal system calls. */


struct _DskDir
{
  int openat_fd;
  char *openat_dir;

  unsigned ref_count;

  unsigned buf_alloced;
  char *buf;

  /* alt_buf_alloced, alt_buf are used for rename() which requires
     two filename slots. */
  unsigned alt_buf_alloced;
  char *alt_buf;

  dsk_boolean locked;
};

DskDir      *dsk_dir_open                    (DskDir       *parent,
                                              const char   *dir,
                                              DskDirOpenFlags flags,
                                              DskError    **error)
{
  int fd = dsk_dir_sys_open (parent, dir, O_RDONLY);
  if (fd < 0)
    {
      if (errno == ENOENT && (flags & DSK_DIR_OPEN_MAYBE_CREATE) != 0)
        {
          if (! dsk_dir_mkdir (parent, dir, error))
            {
              return NULL;
            }
          fd = dsk_dir_sys_open (parent, dir, O_RDONLY);
        }

      if (fd < 0)
        {
          /* XXX: should check out-of-fd errors! */
          dsk_set_error (error, "error opening directory %s: %s",
                         dir, strerror (errno));
          return NULL;
        }
    }
  dsk_fd_set_close_on_exec (fd, TRUE);
  if ((flags & DSK_DIR_OPEN_SKIP_LOCKING) != 0)
    {
      if (flock (fd, LOCK_EX|((flags & DSK_DIR_OPEN_BLOCK_LOCKING) ? 0 : LOCK_NB)) < 0)
        {
          dsk_set_error (error, "error locking directory %s: %s",
                         dir, strerror (errno));
          close (fd);
          return NULL;
        }
      *fd_out = fd;
    }

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
  close (dir->fd);
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
static void
resize_buf (DskDir *dir,
            unsigned alloc)
{
  dir->alloc = (min_alloced + 128) & ~63;
  dsk_free (dir->buf);
  dir->buf = dsk_malloc (dir->buf_alloced);
  memcpy (dir->buf, dir->openat_dir, dir->openat_dir_len);
  dir->buf[dir->openat_dir_len] = '/';
}

static const char *
prep_buf_with_path (DskDir *dir,
                    const char *path)
{
  unsigned len = strlen (path);
  unsigned min_alloced = dir->openat_dir_len + 1 + len + 1
  if (min_alloced > dir->buf_alloced)
    resize_buf (dir, (min_alloced + 128) & ~63);
  memcpy (rv->buf + dir->openat_dir_len + 1, path, len + 1);
  return rv->buf;
}

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
    return unlink (path, mode);
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
