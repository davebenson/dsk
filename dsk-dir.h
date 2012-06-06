
/* openat() and friends implemented in a portable way.
   Systems without "atfile" support use normal system calls. */


typedef enum
{
  DSK_DIR_OPEN_SKIP_LOCKING = (1<<0),
  DSK_DIR_OPEN_BLOCK_LOCKING = (1<<1),
  DSK_DIR_OPEN_MAYBE_CREATE = (1<<2)
} DskDirOpenFlags;

DskDir      *dsk_dir_open                    (DskDir         *parent,
                                              const char     *dir,
                                              DskDirOpenFlags flags,
                                              DskError      **error);

/* note: these functions passthru NULL safely (as does all this API) */
DskDir      *dsk_dir_ref                     (DskDir         *dir);
void         dsk_dir_unref                   (DskDir         *dir);

int          dsk_dir_sys_open                (DskDir         *dir,
                                              const char     *path,
                                              unsigned        flags,
			                      unsigned        mode);
int          dsk_dir_sys_rename              (DskDir         *dir,
                                              const char     *old_path,
                                              const char     *new_path);
int          dsk_dir_sys_rename2             (DskDir         *old_dir,
                                              const char     *old_path,
                                              DskDir         *new_dir,
                                              const char     *new_path);
int          dsk_dir_sys_unlink              (DskDir         *dir,
                                              const char     *path);
int          dsk_dir_sys_rmdir               (DskDir         *dir,
                                              const char     *path);

//xxx
typedef enum
{
  DSK_DIR_RM_RECURSIVE = (1<<0),
  DSK_DIR_RM_IGNORE_MISSING = (1<<1)
} DskDirRmFlags;
int          dsk_dir_rm                      (DskDir       *dir,
                                              const char   *path,
                                              DskDirRmFlags flags,
                                              DskError    **error);
typedef enum
{
  DSK_DIR_CP_RECURSIVE = (1<<0)
} DskDirCpFlags;
int          dsk_dir_cp                      (DskDir       *dir,
                                              const char   *src,
                                              const char   *dst,
                                              DskDirCpFlags flags,
                                              DskError    **error);

typedef enum
{
  DSK_DIR_MKDIR_RECURSIVE = (1<<0)
} DskDirMkdirFlags;
int          dsk_dir_mkdir                   (DskDir       *dir,
                                              const char   *src,
                                              DskDirMkdirFlags flags,
                                              DskError    **error);
