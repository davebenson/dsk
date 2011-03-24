
#define DSK_ALIGNOF_INT        __alignof(int)
#define DSK_ALIGNOF_FLOAT      __alignof(float)
#define DSK_ALIGNOF_DOUBLE     __alignof(double)
#define DSK_ALIGNOF_INT64      __alignof(int64_t)
#define DSK_ALIGNOF_POINTER    __alignof(void*)
#define DSK_ALIGNOF_STRUCTURE  1
#define DSK_SIZEOF_POINTER     4

#define DSK_IS_LITTLE_ENDIAN    1
#define DSK_IS_BIG_ENDIAN    (!DSK_IS_LITTLE_ENDIAN)


/* TODO: actually, this should be TRUE for all known versions
   of linux... that would speed up rm_rf by eliminating an
   extra lstat(2) call. */
#define UNLINK_DIR_RETURNS_EISDIR       DSK_FALSE

