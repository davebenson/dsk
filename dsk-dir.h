
/* openat() and friends implemented in a portable way.
   Systems without "atfile" support use normal system calls. */

#include <dirent.h>

typedef struct _DskDir DskDir;

typedef enum
{
  DSK_DIR_NEW_SKIP_LOCKING = (1<<0),
  DSK_DIR_NEW_BLOCK_LOCKING = (1<<1),
  DSK_DIR_NEW_MAYBE_CREATE = (1<<2)
} DskDirNewFlags;

DskDir      *dsk_dir_new                     (DskDir         *parent,
                                              const char     *dir,
                                              DskDirNewFlags flags,
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
int          dsk_dir_sys_mkdir               (DskDir         *dir,
                                              const char     *path,
                                              unsigned        mode);
DIR         *dsk_dir_sys_opendir             (DskDir         *dir,
                                              const char     *path);

const char  *dsk_dir_get_str                 (DskDir         *dir);
dsk_boolean  dsk_dir_did_create              (DskDir         *dir);

typedef enum
{
  DSK_DIR_OPENFD_MUST_CREATE = (1<<0),
  DSK_DIR_OPENFD_MAY_CREATE = (1<<1),
  DSK_DIR_OPENFD_MUST_EXIST = 0,        /* the default */

  DSK_DIR_OPENFD_WRITABLE = (1<<3),
  DSK_DIR_OPENFD_TRUNCATE = (1<<4),
  DSK_DIR_OPENFD_APPEND = (1<<5),

  DSK_DIR_OPENFD_NO_MKDIR = (1<<6),

  /* opening readonly is the default,
     so you can just pass in 0 
     to get simple reader behavior. */
  DSK_DIR_OPENFD_READONLY = DSK_DIR_OPENFD_MUST_EXIST
} DskDirOpenfdFlags;

int          dsk_dir_openfd (DskDir            *dir,
                             const char        *path,
                             DskDirOpenfdFlags  flags,
                             unsigned           mode,
                             DskError         **error);

typedef enum
{
  DSK_DIR_SET_CONTENTS_NO_MKDIR = (1<<0),
  DSK_DIR_SET_CONTENTS_NO_TMP_FILE = (1<<1)
} DskDirSetContentsFlags;
dsk_boolean  dsk_dir_set_contents (DskDir                 *dir,
                                   const char             *path,
                                   DskDirSetContentsFlags  flags,
                                   unsigned                mode,
                                   size_t                  data_length,
                                   const uint8_t          *data,
                                   DskError              **error);

//xxx
typedef enum
{
  DSK_DIR_RM_RECURSIVE = (1<<0),

  /* in theory, this is redundant, and you could do something like 
       if (dsk_error_get_errno (error, &tmp_errno) && tmp_errno == ENOENT)
         ... ignore error
       else
         ... error
     but it's far nicer to just treat "not-found" failure as success
     in many cases, for 'rm', at least. */
  DSK_DIR_RM_IGNORE_MISSING = (1<<1)
} DskDirRmFlags;
typedef struct
{
  unsigned n_dirs_deleted;
  unsigned n_files_deleted;
} DskDirRmStats;

dsk_boolean  dsk_dir_rm                      (DskDir       *dir,
                                              const char   *path,
                                              DskDirRmFlags flags,
                                              DskDirRmStats*stats_out_optional,
                                              DskError    **error);
typedef enum
{
  DSK_DIR_CP_RECURSIVE = (1<<0)
} DskDirCpFlags;
typedef struct 
{
  uint64_t bytes_copied;
  unsigned files_copied;
  unsigned dirs_copied;
} DskDirCpStats;
int          dsk_dir_cp                      (DskDir       *dir,
                                              const char   *src,
                                              const char   *dst,
                                              DskDirCpFlags flags,
                                              DskDirCpStats*stats_out_optional,
                                              DskError    **error);

typedef enum
{
  DSK_DIR_MKDIR_NONRECURSIVE = (1<<0), /* cf dsk_dir_sys_mkdir() */
  DSK_DIR_MKDIR_MUST_CREATE = (1<<1)
} DskDirMkdirFlags;
typedef struct
{
  unsigned dirs_made;
} DskDirMkdirStats;
int          dsk_dir_mkdir                   (DskDir       *dir,
                                              const char   *src,
                                              DskDirMkdirFlags flags,
                                              DskDirMkdirStats *stats_out_optional,
                                              DskError    **error);
