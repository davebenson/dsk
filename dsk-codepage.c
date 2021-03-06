#include "dsk.h"

typedef struct _DskCodepageToUtf8Class DskCodepageToUtf8Class;
typedef struct _DskCodepageToUtf8 DskCodepageToUtf8;
struct _DskCodepageToUtf8Class
{
  DskSyncFilterClass base_class;
};
struct _DskCodepageToUtf8
{
  DskSyncFilter base_instance;
  const DskCodepage *codepage;
};

static bool
dsk_codepage_to_utf8_process (DskSyncFilter *filter,
                              DskBuffer      *out,
                              unsigned        in_length,
                              const uint8_t  *in_data,
                              DskError      **error)
{
  const DskCodepage *codepage = ((DskCodepageToUtf8*)filter)->codepage;
  DSK_UNUSED (error);
  while (in_length > 0)
    {
      if (*in_data & 0x80)
        {
          uint16_t offset = codepage->offsets[*in_data - 128];
          uint16_t length = codepage->offsets[*in_data - 128 + 1] - offset;
          dsk_buffer_append_small (out, length, codepage->heap + offset);
          in_data++;
          in_length--;
        }
      else
        {
          unsigned i = 1;
          while (i < in_length && (in_data[i] & 0x80) == 0)
            i++;
          dsk_buffer_append (out, i, in_data);
          in_length -= i;
          in_data += i;
        }
    }
  return true;
}

static bool
dsk_codepage_to_utf8_finish (DskSyncFilter *filter,
                              DskBuffer      *out,
                              DskError      **error)
{
  DSK_UNUSED (filter);
  DSK_UNUSED (out);
  DSK_UNUSED (error);
  return true;
}
#define dsk_codepage_to_utf8_init NULL
#define dsk_codepage_to_utf8_finalize NULL

DSK_SYNC_FILTER_SUBCLASS_DEFINE(static, DskCodepageToUtf8, dsk_codepage_to_utf8);

/**
 * dsk_codepage_to_utf8_new:
 *
 * Create a sync-filter that converts each byte into a character,
 * based on a DskCodepage.
 */
DskSyncFilter *
dsk_codepage_to_utf8_new           (const DskCodepage *codepage)
{
  DskCodepageToUtf8 *cp = dsk_object_new (&dsk_codepage_to_utf8_class);
  cp->codepage = codepage;
  return DSK_SYNC_FILTER (cp);
}
