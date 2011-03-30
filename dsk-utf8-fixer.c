#include "dsk.h"
#include <string.h>

typedef struct _DskUtf8Fixer DskUtf8Fixer;
typedef struct _DskUtf8FixerClass DskUtf8FixerClass;
struct _DskUtf8FixerClass
{
  DskOctetFilterClass base;
};
struct _DskUtf8Fixer
{
  DskOctetFilter base;

  DskUtf8FixerMode mode;

  /* partial character analysis */
  uint8_t spare[6];
  unsigned spare_len;
};

static void
handle_bad_char (DskUtf8Fixer *fixer,
                 DskBuffer    *out,
                 uint8_t       c)
{
  char buf[6];
  switch (fixer->mode)
    {
    case DSK_UTF8_FIXER_DROP:
      break;
    case DSK_UTF8_FIXER_LATIN1:
      dsk_buffer_append (out, dsk_utf8_encode_unichar (buf, c), buf);
      break;
    }
}

static dsk_boolean
dsk_utf8_fixer_process  (DskOctetFilter *filter,
                         DskBuffer      *out,
                         unsigned        in_length,
                         const uint8_t  *in_data,
                         DskError      **error)
{
  DskUtf8Fixer *fixer = (DskUtf8Fixer *) filter;
  DSK_UNUSED (error);
retry_spare:
  if (fixer->spare_len)
    {
      unsigned used = 6 - fixer->spare_len;
      unsigned u;
      if (used < in_length)
        used = in_length;
      memcpy (fixer->spare + fixer->spare_len, in_data, used);
      switch (dsk_utf8_validate (fixer->spare_len + used, (char*) fixer->spare, &u))
        {
        case DSK_UTF8_VALIDATION_SUCCESS:
          dsk_buffer_append (out, fixer->spare_len, fixer->spare);
          in_length -= used;
          in_data += used;
          fixer->spare_len = 0;
          break;
        case DSK_UTF8_VALIDATION_PARTIAL:
          dsk_assert (u > fixer->spare_len);
          dsk_buffer_append (out, u, fixer->spare);
          in_length -= u - fixer->spare_len;
          in_data += u - fixer->spare_len;
          fixer->spare_len = 0;
          break;
        case DSK_UTF8_VALIDATION_INVALID:
          dsk_buffer_append (out, u, fixer->spare);
          if (u < fixer->spare_len)
            {
              handle_bad_char (fixer, out, fixer->spare[u]);
              u++;
              memmove (fixer->spare, fixer->spare + u, fixer->spare_len - u);
              fixer->spare_len -= u;
              goto retry_spare;
            }
          else
            {
              in_length -= u - fixer->spare_len;
              in_data += u - fixer->spare_len;
              fixer->spare_len = 0;
            }
          break;
        }
    }
  while (in_length)
    {
      unsigned used;
      switch (dsk_utf8_validate (in_length, (char*) in_data, &used))
        {
        case DSK_UTF8_VALIDATION_SUCCESS:
          dsk_buffer_append (out, used, in_data);
          return DSK_TRUE;
        case DSK_UTF8_VALIDATION_PARTIAL:
          dsk_assert (in_length - used <= 6);
          dsk_buffer_append (out, used, in_data);
          fixer->spare_len = in_length - used;
          memcpy (fixer->spare, in_data, fixer->spare_len);
          return DSK_TRUE;
        case DSK_UTF8_VALIDATION_INVALID:
          if (used > 0)
            {
              dsk_buffer_append (out, used, in_data);
              in_data += used;
              in_length -= used;
            }
          dsk_assert (in_length > 0);
          dsk_assert (*in_data >= 128);
          handle_bad_char (fixer, out, *in_data);
          in_length--;
          in_data++;
          break;
        }
    }
  return DSK_TRUE;
}

static dsk_boolean
dsk_utf8_fixer_finish  (DskOctetFilter *filter,
                        DskBuffer      *out,
                        DskError      **error)
{
  DskUtf8Fixer *fixer = (DskUtf8Fixer *) filter;
  unsigned i;
  DSK_UNUSED (error);
  for (i = 0; i < fixer->spare_len; i++)
    handle_bad_char (fixer, out, fixer->spare[i]);
  return DSK_TRUE;
}

#define dsk_utf8_fixer_init NULL
#define dsk_utf8_fixer_finalize NULL

DSK_OCTET_FILTER_SUBCLASS_DEFINE(static, DskUtf8Fixer, dsk_utf8_fixer);

DskOctetFilter *dsk_utf8_fixer_new     (DskUtf8FixerMode mode)
{
  DskUtf8Fixer *fixer = dsk_object_new (&dsk_utf8_fixer_class);
  fixer->mode = mode;
  return &fixer->base;
}
