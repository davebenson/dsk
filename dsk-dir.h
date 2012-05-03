
/* openat() and friends implemented in a portable way.
   Systems without "atfile" support use normal system calls. */


struct _DskDir
{
  int openat_fd;
  char *openat_dir;

  unsigned ref_count;

  unsigned buf_alloced;
  char *buf;
  unsigned buf_length;
  dsk_boolean locked;

  DskDir *alias;
};

DskDir      *dsk_dir_open                    (DskDir *parent,
                                              const char   *dir,
                                              dsk_boolean   lock,
                                              DskError    **error);

int          dsk_dir_sys_open                (DskDir       *dir,
                                              const char   *path,
                                              unsigned      flags,
			                      unsigned      mode);
int          dsk_dir_sys_rename              (DskDir       *dir,
                                              const char   *old_path,
                                              const char   *new_path);
int          dsk_dir_sys_rename2             (DskDir       *old_dir,
                                              const char   *old_path,
                                              DskDir       *new_dir,
                                              const char   *new_path);
int          dsk_dir_sys_unlink              (DskDir       *dir,
                                              const char   *path);
int          dsk_dir_sys_rmdir               (DskDir       *dir,
                                              const char   *path);

//xxx
typedef enum
{
  DSK_DIR_RM_FORCE = (1<<0),
  DSK_DIR_RM_RECURSIVE = (1<<1),

  DSK_DIR_RM_RF = DSK_DIR_RM_FORCE|DSK_DIR_RM_RECURSIVE,
} DskDirRmFlags;
int          dsk_dir_sys_rm                  (DskDir       *dir,
                                              const char   *path,
                                              DskDirRmFlags flags);
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

