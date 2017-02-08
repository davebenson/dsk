#define _POSIX_C_SOURCE  200809L

#include <alloca.h>
#include "dsk.h"
#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef _O_BINARY
# define MY_O_BINARY _O_BINARY
#elif defined(O_BINARY)
# define MY_O_BINARY  O_BINARY
#else
# define MY_O_BINARY  0
#endif

uint8_t    *dsk_file_get_contents (const char *filename,
                                   size_t     *size_out,
			           DskError  **error)
{
  DskBuffer buffer = DSK_BUFFER_INIT;
  int fd = open (filename, O_RDONLY | MY_O_BINARY);
  int read_rv;
  uint8_t *rv;
  if (fd < 0)
    {
      dsk_set_error (error, "error opening %s: %s",
                     filename, strerror (errno));
      return NULL;
    }
  while ((read_rv = dsk_buffer_readv (&buffer, fd)) > 0)
    ;
  if (read_rv < 0)
    {
      dsk_set_error (error,
                     "error reading file %s: %s",
                     filename, strerror (errno));
      close (fd);
      return NULL;
    }
  if (size_out)
    *size_out = buffer.size;
  rv = dsk_malloc (buffer.size + 1);
  rv[buffer.size] = 0;
  dsk_buffer_read (&buffer, buffer.size, rv);
  close (fd);
  return rv;
}

dsk_boolean dsk_file_set_contents (const char    *filename,
                                   size_t         size,
                                   const uint8_t *contents,
			           DskError     **error)
{
  int fd;
  unsigned rem;
retry_open:
  fd = open (filename, O_WRONLY | O_CREAT | O_TRUNC | MY_O_BINARY, 0666);
  if (fd < 0)
    {
      if (errno == EINTR)
        goto retry_open;
      dsk_set_error (error, "error creating %s: %s",
                     filename, strerror (errno));
      return DSK_FALSE;
    }
  rem = size;
  while (rem > 0)
    {
      int write_rv = write (fd, contents, rem);
      if (write_rv < 0)
        {
          if (errno == EINTR)
            continue;
          dsk_set_error (error, "error writing to %s: %s",
                         filename, strerror (errno));
          close (fd);
          return DSK_FALSE;
        }
      rem -= write_rv;
      contents += write_rv;
    }
  close (fd);
  return DSK_TRUE;
}

dsk_boolean dsk_file_test_exists  (const char *filename)
{
  struct stat stat_buf;
retry_stat:
  if (stat (filename, &stat_buf) < 0)
    {
      if (errno == EINTR)
        goto retry_stat;
      return DSK_FALSE;
    }
  return DSK_TRUE;
}

dsk_boolean dsk_file_test_is_dir  (const char *filename)
{
  struct stat stat_buf;
retry_stat:
  if (stat (filename, &stat_buf) < 0)
    {
      if (errno == EINTR)
        goto retry_stat;
      return DSK_FALSE;
    }
  return S_ISDIR (stat_buf.st_mode);
}

static dsk_boolean
safe_unlink (const char *dir_or_file,
             const char **failed_op_out,
             int *errno_out)
{
#if ! UNLINK_DIR_RETURNS_EISDIR
  struct stat stat_buf;
  if (lstat (dir_or_file, &stat_buf) < 0)
    {
      *errno_out = errno;
      *failed_op_out = "lstat";
      return DSK_FALSE;
    }
  if (S_ISDIR (stat_buf.st_mode))
    {
      *errno_out = EISDIR;
      *failed_op_out = "unlink";
      return DSK_FALSE;
    }
#endif
  if (unlink (dir_or_file) < 0)
    {
      *errno_out = errno;
      *failed_op_out = "unlink";
      return DSK_FALSE;
    }
  return DSK_TRUE;
}

/**
 * dsk_remove_dir_recursive:
 * @dir_or_file: the directory or file to delete.
 * @error: optional error return location.
 *
 * Recursively remove a directory or file,
 * similar to 'rm -rf DIR_OR_FILE' on the unix command-line.
 *
 * returns: whether the removal was successful.
 * This routine fails if there is a permission or i/o problem.
 * (It returns DSK_TRUE if the file does not exist.)
 * If it fails, and error is non-NULL, *error will hold
 * a #DskError object.
 */
dsk_boolean dsk_rm_rf   (const char *dir_or_file, DskError    **error)
{
  int e;
  const char *op;
  if (!safe_unlink (dir_or_file, &op, &e))
    {
      if (strcmp (op, "lstat") == 0 && e == ENOENT)
        return DSK_TRUE;
      if (e == EISDIR)
        {
          /* scan directory, removing contents recursively */
          DIR *dir = opendir (dir_or_file);
          unsigned flen = strlen (dir_or_file);
          struct dirent *dirent;
          if (dir == NULL)
            {
              dsk_set_error (error, "error opening %s: %s",
                             dir_or_file, strerror (errno));
              return DSK_FALSE;
            }
          while ((dirent = readdir (dir)) != NULL)
            {
              const char *base = dirent->d_name;
              char *fname;

              /* skip . and .. */
              if (base[0] == '.'
               && (base[1] == 0 || (base[1] == '.' && base[2] == 0)))
                continue;

              /* recurse */
              fname = dsk_malloc (flen + 1 + strlen (base) + 1);
              strcpy (fname, dir_or_file);
              fname[flen] = '/';
              strcpy (fname + flen + 1, base);
              if (!dsk_rm_rf (fname, error))
                {
                  dsk_free (fname);
                  closedir (dir);
                  return DSK_FALSE;
                }
              dsk_free (fname);
            }
          closedir (dir);

          if (rmdir (dir_or_file) < 0)
            {
              dsk_set_error (error,
                             "error running rmdir(%s): %s", dir_or_file,
                             strerror (errno));
              return DSK_FALSE;
            }
          return DSK_TRUE;
        }
      else
        {
          dsk_set_error (error, "error %s %s: %s",
                         op, dir_or_file, strerror (e));
          return DSK_FALSE;
        }
    }
  return DSK_TRUE;
}

dsk_boolean
dsk_remove_dir_recursive   (const char *dir,
                            DskError    **error)
{
  struct stat stat_buf;
  if (lstat (dir, &stat_buf) < 0)
    {
      dsk_set_error (error, "error %s not found", dir);
      return DSK_FALSE;
    }
  if (!S_ISDIR (stat_buf.st_mode))
    {
      dsk_set_error (error, "dsk_remove_dir_recursive called on non-directory");
      return DSK_FALSE;
    }
  return dsk_rm_rf (dir, error);
}

dsk_boolean dsk_mkdir_recursive (const char *dir,
                                 unsigned    permissions,
                                 DskError  **error)
{
  unsigned dir_len = strlen (dir);
  char *dir_buf = alloca (dir_len + 1);
  unsigned cur_len = 0;

  if (dsk_file_test_is_dir (dir))
    return DSK_TRUE;

  /* append to dir_buf any number of consecutive '/'
     characters. */
#define SCAN_THROUGH_SLASHES()  \
  do{                 \
    while (cur_len < dir_len && dir[cur_len] == DSK_DIR_SEPARATOR) \
      dir_buf[cur_len++] = DSK_DIR_SEPARATOR; \
  }while(0)

  SCAN_THROUGH_SLASHES();
  while (cur_len < dir_len)
    {
      const char *slash = strchr (dir + cur_len, DSK_DIR_SEPARATOR);    /* not UTF8 */
      unsigned new_cur_len;
      if (slash == NULL)
        new_cur_len = dir_len;
      else
        new_cur_len = slash - dir;

      memcpy (dir_buf + cur_len, dir + cur_len, new_cur_len - cur_len);
      dir_buf[new_cur_len] = 0;
      cur_len = new_cur_len;

      if (dsk_file_test_is_dir (dir_buf))
        ;
      else
        {
          if (mkdir (dir_buf, permissions) < 0)
            {
              if (errno != EEXIST)
                {
                  dsk_set_error (error, "error making directory %s: %s",
                               dir_buf, strerror (errno));
                  return DSK_FALSE;
                }
            }
        }

      SCAN_THROUGH_SLASHES();
    }
  return DSK_TRUE;
#undef SCAN_THROUGH_SLASHES
}

#if 0
#if DSK_HAS_ATFILE_SUPPORT
dsk_boolean dsk_mkdir_recursive_at (int openat_fd,
                                    const char *dir,
                                    unsigned    permissions,
                                    DskError  **error)
{
  unsigned dir_len = strlen (dir);
  char *dir_buf = alloca (dir_len + 1);
  unsigned cur_len = 0;

  if (dsk_file_test_is_dir (dir))
    return DSK_TRUE;

  /* append to dir_buf any number of consecutive '/'
     characters. */
#define SCAN_THROUGH_SLASHES()  \
  do{                 \
    while (cur_len < dir_len && dir[cur_len] == DSK_DIR_SEPARATOR) \
      dir_buf[cur_len++] = DSK_DIR_SEPARATOR; \
  }while(0)

  SCAN_THROUGH_SLASHES();
  while (cur_len < dir_len)
    {
      const char *slash = strchr (dir + cur_len, DSK_DIR_SEPARATOR);    /* not UTF8 */
      unsigned new_cur_len;
      if (slash == NULL)
        new_cur_len = dir_len;
      else
        new_cur_len = slash - dir;

      memcpy (dir_buf + cur_len, dir + cur_len, new_cur_len - cur_len);
      dir_buf[new_cur_len] = 0;
      cur_len = new_cur_len;

      if (dsk_file_test_is_dir...at (openat_fd, dir_buf))
        ;
      else
        {
          if (mkdirat (openat_fd, dir_buf, permissions) < 0)
            {
              if (errno != EEXIST)
                {
                  dsk_set_error (error, "error making directory %s: %s",
                               dir_buf, strerror (errno));
                  return DSK_FALSE;
                }
            }
        }

      SCAN_THROUGH_SLASHES();
    }
  return DSK_TRUE;
#undef SCAN_THROUGH_SLASHES
}
#endif
#endif


static const char default_tmpdir[] = "/tmp";
static char *tmp_dir = NULL;

const char *dsk_get_tmp_dir (void)
{
  if (tmp_dir == NULL)
    {
      /* Use strdup in case someone calls setenv().
         TODO: need a cleanup hook for this... */
      tmp_dir = dsk_strdup (getenv ("TMPDIR"));
      if (tmp_dir == NULL)
        tmp_dir = dsk_strdup (getenv ("TMP"));
      if (tmp_dir == NULL)
        tmp_dir = dsk_strdup (getenv ("TEMP"));
      if (tmp_dir == NULL)
        tmp_dir = (char *) "/tmp";
    }
  return tmp_dir;
}
