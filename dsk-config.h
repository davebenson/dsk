/* TODO: write test program to verify all these macros */

/* SEE:
 *    gcc -dM -E - < /dev/null
 * for the set of gcc preproc macros.
 */

#ifdef __SIZEOF_POINTER__
# define DSK_SIZEOF_POINTER     __SIZEOF_POINTER__
#elif defined(i386) || defined(__i386__)
# define DSK_SIZEOF_POINTER     4
#else
# error "need sizeof(void*) at preprocessor time"
#endif

#if (DSK_SIZEOF_POINTER == 4)
# define DSK_LOG2_SIZEOF_POINTER 2
#elif (DSK_SIZEOF_POINTER == 8)
# define DSK_LOG2_SIZEOF_POINTER 3
#else
# error "unhandled DSK_SIZEOF_POINTER"
#endif

#ifdef __SIZEOF_SIZE_T__
# define DSK_SIZEOF_SIZE_T      __SIZEOF_SIZE_T__
#else
# define DSK_SIZEOF_SIZE_T      DSK_SIZEOF_POINTER
#endif


#if defined(__GNUC__)
# define DSK_ALIGNOF_INT        __alignof(int)
# define DSK_ALIGNOF_FLOAT      __alignof(float)
# define DSK_ALIGNOF_DOUBLE     __alignof(double)
# define DSK_ALIGNOF_INT64      __alignof(int64_t)
# define DSK_ALIGNOF_POINTER    __alignof(void*)
#else
# define DSK_ALIGNOF_FLOAT      4
# warn "Need alignment info for this compiler"
#endif

/* Verify that
       sizeof(struct { char a }) == 1,
       sizeof(struct { char a,b }) == 2,
       sizeof(struct { char a,b,c }) == 3,
   and sizeof(struct { char a,b,c,d,e }) == 5. */
#define DSK_ALIGNOF_STRUCTURE  1

#if defined(__linux__)
# include <endian.h>
# define DSK_IS_LITTLE_ENDIAN    (BYTE_ORDER == LITTLE_ENDIAN)
#elif defined(__APPLE__)
# include <machine/endian.h>
# define DSK_IS_LITTLE_ENDIAN    (BYTE_ORDER == LITTLE_ENDIAN)
#elif defined(i386) || defined(__i386__)
# define DSK_IS_LITTLE_ENDIAN    1
#else
# error "endianness not known here"
#endif

#define DSK_IS_BIG_ENDIAN    (!(DSK_IS_LITTLE_ENDIAN))

/* Miscellaneous features that linux has.
 *  TODO: other OSes. */
#ifdef __linux__
# define DSK_HAS_ATFILE_SUPPORT         DSK_TRUE
#else
# define DSK_HAS_ATFILE_SUPPORT         DSK_FALSE
#endif

/* we should eliminate stdio instead of defining this. */
#ifdef __linux__
# define DSK_HAS_UNLOCKED_STDIO_SUPPORT         DSK_TRUE
#else
# define DSK_HAS_UNLOCKED_STDIO_SUPPORT         DSK_FALSE
#endif

/* Actually, this should be TRUE for all known versions
   of linux... but not mac os/x, and perhaps not BSD.
   If TRUE, this speeds up rm_rf by eliminating an
   extra lstat(2) call. */
#ifdef __linux__
# define UNLINK_DIR_RETURNS_EISDIR       DSK_TRUE
#elif defined(__APPLE__)
# define UNLINK_DIR_RETURNS_EISDIR       DSK_FALSE
#else
# define UNLINK_DIR_RETURNS_EISDIR       DSK_FALSE
# warn "add check that unlink() returns EISDIR"
#endif

#if defined(__linux__)
# define DSK_HAS_EPOLL                   DSK_TRUE
#else
# define DSK_HAS_EPOLL                   DSK_FALSE
#endif

#define DSK_IEEE754                      DSK_TRUE
