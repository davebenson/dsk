/* UNTESTED */

#include <string.h>
#include "dsk.h"
#include "dsk-config.h"

typedef struct _DskUtf8ToUtf16Class DskUtf8ToUtf16Class;
typedef struct _DskUtf8ToUtf16 DskUtf8ToUtf16;

struct _DskUtf8ToUtf16Class
{
  DskSyncFilterClass base_class;
};

struct _DskUtf8ToUtf16
{
  DskSyncFilter base_instance;
  uint8_t n_utf8_bytes : 4;
  uint8_t swap : 1;
  uint8_t must_emit_bom : 1;
  uint8_t utf8_bytes[DSK_UTF8_MAX_LENGTH];
};

static void
uint16_array_swap (unsigned count,
                   uint16_t *inout)
{
  while (count--)
    {
      uint16_t v = *inout;
      v = (v<<8) | (v>>8);
      *inout++ = v;
    }
}

static bool
dsk_utf8_to_utf16_process (DskSyncFilter *filter,
                           DskBuffer      *out,
                           unsigned        in_length,
                           const uint8_t  *in_data,
                           DskError      **error)
{
  DskUtf8ToUtf16 *u8to16 = (DskUtf8ToUtf16 *) filter;
  if (DSK_UNLIKELY (u8to16->must_emit_bom))
    {
      uint16_t bom = DSK_UNICODE_BOM;
      if (u8to16->swap)
        uint16_array_swap (1, &bom);
      dsk_buffer_append (out, 2, &bom);
      u8to16->must_emit_bom = false;
    }
  unsigned in_used = 0;
  if (u8to16->n_utf8_bytes)
    {
      unsigned size = dsk_utf8_n_bytes_from_initial (in_data[0]);
      unsigned needed = size - u8to16->n_utf8_bytes;
      if (in_length < needed)
        {
          memcpy (u8to16->utf8_bytes + u8to16->n_utf8_bytes, in_data, in_length);
          u8to16->n_utf8_bytes += in_length;
          return true;
        }
      memcpy (u8to16->utf8_bytes, in_data, needed);
      in_used = needed;

      unsigned used;
      uint32_t v;
      if (!dsk_utf8_decode_unichar (size, (char*)u8to16->utf8_bytes, &used, &v))
        {
          dsk_set_error (error, "bad UTF-8");
          return false;
        }
      unsigned count;
      uint16_t b[2];
      if (v < 0x10000)
        {
          /* no surrogate pair */
          count = 1;
          b[0] = v;
        }
      else
        {
          /* use surrogate pair */
          count = 2;
          dsk_utf16_to_surrogate_pair (v, b);
        }
      if (u8to16->swap)
        uint16_array_swap (count, b);
      dsk_buffer_append (out, 2 * count, b);
    }
#define UTF16_BUF_SIZE  128
  uint16_t buf[UTF16_BUF_SIZE];
  unsigned n_buf = 0;
  while (in_used < in_length)
    {
      while (in_used < in_length && n_buf < UTF16_BUF_SIZE - 1)
        {
          switch (dsk_utf8_n_bytes_from_initial (in_data[in_used]))
            {
            case 1:
              buf[n_buf++] = in_data[in_used++];
              break;
            case 2:
              if (in_used + 2 > in_length)
                goto partial;
              buf[n_buf++] = ((in_data[in_used] & 0x1f) << 6) | (in_data[in_used+1] & 0x3f);
              in_used += 2;
              break;

            case 3:
              if (in_used + 3 > in_length)
                goto partial;
              buf[n_buf]   = ((in_data[in_used] & 0x0f) << 12)
                           | ((in_data[in_used+1] & 0x3f) << 6)
                           | ((in_data[in_used+2] & 0x3f) << 0);
              if (dsk_unicode_is_surrogate (buf[n_buf]))
                {
                  dsk_set_error (error, "surrogate-pair found in UTF-8 input");
                  return false;
                }
              n_buf++;
              in_used += 3;
              break;
            case 4:
              if (in_used + 4 > in_length)
                goto partial;
              {
                uint32_t v = ((in_data[in_used+0] & 0x07) << 18)
                           | ((in_data[in_used+1] & 0x3f) << 12)
                           | ((in_data[in_used+2] & 0x3f) << 6)
                           | ((in_data[in_used+3] & 0x3f) << 0);
                if (v < 0x10000)
                  {
                    dsk_set_error (error, "bad UTF-8");
                    return false;
                  }
                dsk_utf16_to_surrogate_pair (v, buf + n_buf);
                n_buf += 2;
              }
              break;
            }
        }
      if (u8to16->swap)
        uint16_array_swap (n_buf, buf);
      dsk_buffer_append (out, n_buf * 2, buf);
    }
  u8to16->n_utf8_bytes = 0;
  return true;

partial:
  if (u8to16->swap)
    uint16_array_swap (n_buf, buf);
  dsk_buffer_append (out, n_buf * 2, buf);
  u8to16->n_utf8_bytes = in_length - in_used;
  memcpy (u8to16->utf8_bytes, in_data + in_used, in_length - in_used);
  return true;
}

bool
dsk_utf8_to_utf16_finish (DskSyncFilter *filter,
                          DskBuffer      *out,
                          DskError      **error)
{
  DskUtf8ToUtf16 *u8to16 = (DskUtf8ToUtf16 *) filter;
  if (DSK_UNLIKELY (u8to16->n_utf8_bytes))
    {
      dsk_set_error (error, "partial UTF-8 sequence at end-of-file");
      return false;
    }
  if (u8to16->must_emit_bom)
    {
      uint16_t bom = DSK_UNICODE_BOM;
      if (u8to16->swap)
        uint16_array_swap (1, &bom);
      dsk_buffer_append (out, 2, &bom);
    }
  return true;
}

#define dsk_utf8_to_utf16_init NULL
#define dsk_utf8_to_utf16_finalize NULL

DSK_SYNC_FILTER_SUBCLASS_DEFINE(static, DskUtf8ToUtf16, dsk_utf8_to_utf16);

DskSyncFilter *
dsk_utf8_to_utf16_converter_new    (DskUtf16WriterFlags flags)
{
  DskUtf8ToUtf16 *u8to16 = dsk_object_new (&dsk_utf8_to_utf16_class);
  DskUtf16WriterFlags e = flags & (DSK_UTF16_WRITER_BIG_ENDIAN|DSK_UTF16_WRITER_LITTLE_ENDIAN);
  if (e == 0)
    {
      /* native endian */
    }
  else if (e == DSK_UTF16_WRITER_LITTLE_ENDIAN)
    {
      u8to16->swap = DSK_IS_BIG_ENDIAN;
    }
  else if (e == DSK_UTF16_WRITER_BIG_ENDIAN)
    {
      u8to16->swap = DSK_IS_LITTLE_ENDIAN;
    }
  else
    {
      dsk_warning ("cannot specify little and big endian at the same time");
      return NULL;
    }
  if (flags & DSK_UTF16_WRITER_EMIT_MARKER)
    u8to16->must_emit_bom = 1;
  return DSK_SYNC_FILTER (u8to16);
}
