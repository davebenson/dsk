
/* true configuration */
#define DSK_HAVE_IPV6  1


#include <assert.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdarg.h>
#include <errno.h>

typedef int dsk_boolean;
#define DSK_FALSE		0
#define DSK_TRUE		1

#define DSK_N_ELEMENTS(array)   ( sizeof(array) / sizeof((array)[0]) )
#define DSK_MIN(a,b)            (((a) < (b)) ? (a) : (b))
#define DSK_MAX(a,b)            (((a) > (b)) ? (a) : (b))

/* branch-hinting macros */
#define DSK_LIKELY(condition)     (condition)
#define DSK_UNLIKELY(condition)   (condition)
#define DSK_UNUSED(var)           ((void)(var))

#ifndef DSK_DEBUG
#define DSK_DEBUG 0
#endif
#define DSK_ALIGN(offset, alignment) \
  (   ((offset) + (alignment) - 1) & (~((alignment) - 1))    )

/* Seconds since 1970 GMT (aka the epoch).
   Note that many platforms define time_t as a 32-bit quantity--
   it is always 64-bit in dsk. */
typedef int64_t dsk_time_t;
dsk_time_t dsk_get_current_time ();
#define dsk_get_current_time() ((dsk_time_t) (dsk_dispatch_default ()->last_dispatch_secs))

typedef void (*DskDestroyNotify) (void *data);

typedef enum
{
  DSK_IO_RESULT_SUCCESS,
  DSK_IO_RESULT_AGAIN,
  DSK_IO_RESULT_EOF,            /* only for read operations */
  DSK_IO_RESULT_ERROR
} DskIOResult;

#define dsk_assert(x)  assert(x)
#define dsk_assert_not_reached()   \
  dsk_error("should not get here: %s:%u", __FILE__, __LINE__)

#define DSK_CAN_INLINE  1
#define DSK_INLINE_FUNC static inline
#define _dsk_inline_assert(condition)  dsk_assert(condition)

/* DSK_GNUC_PRINTF(format_idx,arg_idx): Advise the compiler
 * that the arguments should be like printf(3); it may
 * optionally print type warnings.  */
#ifdef __GNUC__
#if     __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ > 4)
#define DSK_GNUC_PRINTF( format_idx, arg_idx )    \
  __attribute__((__format__ (__printf__, format_idx, arg_idx)))
#endif
#endif
#ifndef DSK_GNUC_PRINTF                /* fallback: no compiler hint */
# define DSK_GNUC_PRINTF( format_idx, arg_idx )
#endif

/* logging */
/* fatal user level error:  exit with status 1 */
void dsk_error(const char *format, ...) DSK_GNUC_PRINTF(1,2);

/* non-fatal warning message */
void dsk_warning(const char *format, ...) DSK_GNUC_PRINTF(1,2);

char *dsk_stpcpy (char *dst, const char *src);

/* --- assertions */
#ifdef DSK_DISABLE_ASSERTIONS
#define dsk_warn_if_fail(condition, blurb) do{}while(0)
#define dsk_error_if_fail(condition, blurb) do{}while(0)
#define dsk_goto_if_fail(condition, blurb, label) do{}while(0)
#define dsk_return_if_fail(condition, blurb) do{}while(0)
#define dsk_return_val_if_fail(condition, blurb, value) do{}while(0)
#define dsk_warn_if_reached(blurb)
#define dsk_return_if_reached(blurb)     return
#define dsk_return_val_if_reached(blurb, value)     return (value)

#else
#define dsk_warn_if_fail(condition, blurb) \
  do{ if (!(condition)) \
        dsk_warning ("%s:%u: condition '%s' failed: %s (%s)", \
                     __FILE__, __LINE__, #condition, \
                     (blurb) ? (blurb) : "dsk_warn_if_fail failed", \
                     __func__); \
  }while (0)
#define dsk_die_if_fail(condition, blurb) \
  do{ if (!(condition)) \
        dsk_die ("%s:%u: condition '%s' failed: %s (%s)", \
                  __FILE__, __LINE__, #condition, \
                  (blurb) ? (blurb) : "dsk_error_if_fail failed", \
                  __func__); } while (0)
#define dsk_goto_if_fail(condition, blurb, label) \
  do{ if (!(condition)) \
        { \
          dsk_warning ("%s:%u: condition '%s' failed: %s (%s)", \
                       __FILE__, __LINE__, #condition, \
                       (blurb) ? (blurb) : "dsk_goto_if_fail failed", \
                       __func__); \
          goto label; \
        } \
  }while (0)
#define dsk_return_if_fail(condition, blurb) \
  do{ if (!(condition)) \
        { \
          dsk_warning ("%s:%u: condition '%s' failed: %s (%s)", \
                       __FILE__, __LINE__, #condition, \
                       (blurb) ? (blurb) : "dsk_return_if_fail failed", \
                       __func__); \
          return; \
        } \
  }while (0)
#define dsk_return_val_if_fail(condition, blurb, value) \
  do{ if (!(condition)) \
        { \
          dsk_warning ("%s:%u: condition '%s' failed: %s (%s)", \
                       __FILE__, __LINE__, #condition, \
                       (blurb) ? (blurb) : "dsk_return_val_if_fail failed", \
                       __func__); \
          return (value); \
        } \
  }while (0)
#define dsk_warn_if_reached(blurb) \
  dsk_warning ("%s:%u: should not be reached: %s (%s)", \
                       __FILE__, __LINE__, \
                       (blurb) ? (blurb) : "dsk_warn_if_reached", \
                       __func__)
#define dsk_die_if_reached(blurb) \
  dsk_die ("%s:%u: should not be reached: %s (%s)", \
                       __FILE__, __LINE__, \
                       (blurb) ? (blurb) : "dsk_die_if_reached", \
                       __func__)
#define dsk_return_if_reached(blurb) \
  do{ \
    dsk_warning ("%s:%u: should not be reached: %s (%s)", \
                         __FILE__, __LINE__, \
                         (blurb) ? (blurb) : "dsk_return_if_reached", \
                         __func__); \
    return; \
  } while(0)
#define dsk_return_val_if_reached(blurb, value) \
  do{ \
    dsk_warning ("%s:%u: should not be reached: %s (%s)", \
                         __FILE__, __LINE__, \
                         (blurb) ? (blurb) : "dsk_return_val_if_reached", \
                         __func__); \
    return (value); \
  } while(0)
#endif

/* programmer error:  only needed during development, hopefully.. */
void dsk_die(const char *format, ...) DSK_GNUC_PRINTF(1,2);


/* allocators (and utilities) which behave fatally */
void *dsk_malloc (size_t);
void *dsk_malloc0 (size_t);
void  dsk_free (void *);
void *dsk_realloc (void *, size_t);
char *dsk_strdup (const char *str);
char *dsk_strndup (size_t len, const char *str);
void *dsk_memdup (size_t, const void *);

/* copy the string starting at 'str', and ending with the last-character
   at end_str-1 (so end_str would point to the NUL on a NUL-terminated string */
char *dsk_strdup_slice (const char *str, const char *end_str);

/* utility for initializing objects */
void dsk_bzero_pointers (void *ptrs, unsigned n_ptrs);


dsk_boolean dsk_parse_boolean (const char *str, dsk_boolean *out);
