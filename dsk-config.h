/* TODO: write test program to verify all these macros */


#if defined(__GNUC__)
#define DSK_ALIGNOF_INT        __alignof(int)
#define DSK_ALIGNOF_FLOAT      __alignof(float)
#define DSK_ALIGNOF_DOUBLE     __alignof(double)
#define DSK_ALIGNOF_INT64      __alignof(int64_t)
#define DSK_ALIGNOF_POINTER    __alignof(void*)
#else
#warn "Need alignment info for this compiler"
#endif

/* Verify that
       sizeof(struct { char a }) == 1,
       sizeof(struct { char a,b }) == 2,
       sizeof(struct { char a,b,c }) == 3,
   and sizeof(struct { char a,b,c,d,e }) == 5. */
#define DSK_ALIGNOF_STRUCTURE  1

#ifdef __SIZEOF_POINTER__
# define DSK_SIZEOF_POINTER     __SIZEOF_POINTER__
#else
# error "need sizeof(void*) at preprocessor time"
#endif

#ifdef __SIZEOF_SIZE_T__
# define DSK_SIZEOF_SIZE_T      __SIZEOF_SIZE_T__
#else
# error "need sizeof(size_t) at preprocessor time"
#endif

#ifdef __linux__
# include <endian.h>
# define DSK_IS_LITTLE_ENDIAN    (BYTE_ORDER == LITTLE_ENDIAN)
#else
# error "endianness not known here"
#endif

#define DSK_IS_BIG_ENDIAN    (!DSK_IS_LITTLE_ENDIAN)


/* TODO: actually, this should be TRUE for all known versions
   of linux... that would speed up rm_rf by eliminating an
   extra lstat(2) call. */
#ifdef __linux__
# define UNLINK_DIR_RETURNS_EISDIR       DSK_TRUE
#else
# warn "add check that unlink() returns EISDIR"
# define UNLINK_DIR_RETURNS_EISDIR       DSK_FALSE
#endif

