#include <string.h>
#include "dsk.h"

/* --- decoder --- */
typedef struct _DskByteDoublerClass DskByteDoublerClass;
struct _DskByteDoublerClass
{
  DskSyncFilterClass base_class;
};
typedef struct _DskByteDoubler DskByteDoubler;
struct _DskByteDoubler
{
  DskSyncFilter base_instance;
  uint8_t byte;
};

#define dsk_byte_doubler_init NULL
#define dsk_byte_doubler_finalize NULL

static bool
dsk_byte_doubler_process    (DskSyncFilter *filter,
                             DskBuffer      *out,
                             unsigned        in_length,
                             const uint8_t  *in_data,
                             DskError      **error)
{
  uint8_t byte = ((DskByteDoubler *)filter)->byte;
  DSK_UNUSED (error);
  while (in_length > 0)
    {
      const uint8_t *end = memchr (in_data, byte, in_length);
      unsigned c;
      if (end == NULL)
	{
	  dsk_buffer_append (out, in_length, in_data);
	  return true;
	}
      dsk_buffer_append (out, end - in_data, in_data);
      in_length -= (end - in_data);
      in_data = end;
      for (c = 1; c < in_length; c++)
        if (in_data[c] != byte)
	  break;
      dsk_buffer_append_repeated_byte (out, c*2, byte);
      in_length -= c;
      in_data += c;
    }
  return true;
}
#define dsk_byte_doubler_finish NULL

DSK_SYNC_FILTER_SUBCLASS_DEFINE(static, DskByteDoubler, dsk_byte_doubler);

DskSyncFilter *
dsk_byte_doubler_new (uint8_t byte)
{
  DskByteDoubler *d = dsk_object_new (&dsk_byte_doubler_class);
  d->byte = byte;
  return (DskSyncFilter *) d;
}
