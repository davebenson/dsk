#include <string.h>
#include "dsk.h"

/* --- decoder --- */
typedef struct _DskByteUndoublerClass DskByteUndoublerClass;
struct _DskByteUndoublerClass
{
  DskSyncFilterClass base_class;
};
typedef struct _DskByteUndoubler DskByteUndoubler;
struct _DskByteUndoubler
{
  DskSyncFilter base_instance;
  uint8_t byte;
  uint8_t has_one_byte;
  uint8_t ignore_errors;
};

#define dsk_byte_undoubler_init NULL
#define dsk_byte_undoubler_finalize NULL

static bool
dsk_byte_undoubler_process  (DskSyncFilter *filter,
                             DskBuffer      *out,
                             unsigned        in_length,
                             const uint8_t  *in_data,
                             DskError      **error)
{
  uint8_t byte = ((DskByteUndoubler *)filter)->byte;
  bool ignore_errors = ((DskByteUndoubler *)filter)->ignore_errors;
  if (in_length > 0 && ((DskByteUndoubler*)filter)->has_one_byte)
    {
      if (*in_data == byte)
        {
          in_length--;
          in_data++;
        }
      else if (!ignore_errors)
        goto ERROR;
      ((DskByteUndoubler*)filter)->has_one_byte = 0;
    }
  while (in_length > 0)
    {
      const uint8_t *end = memchr (in_data, byte, in_length);
      if (end == NULL)
	{
	  dsk_buffer_append (out, in_length, in_data);
	  return true;
	}
      dsk_buffer_append (out, end - in_data, in_data);
      in_length -= (end - in_data);
      in_data = end;
      if (in_length == 1)
        {
          ((DskByteUndoubler*)filter)->has_one_byte = true;
          dsk_buffer_append_byte (out, byte);
          return true;
        }
      if (in_data[1] == byte)
        {
          dsk_buffer_append_byte (out, byte);
          in_length -= 2;
          in_data += 2;
        }
      else if (ignore_errors)
        {
          dsk_buffer_append_byte (out, byte);
          in_length -= 1;
          in_data += 1;
        }
      else
        goto ERROR;
    }
  return true;

ERROR:
  dsk_set_error (error, "undoubled byte %s detected", dsk_ascii_byte_name (byte));
  return false;
}
static bool
dsk_byte_undoubler_finish (DskSyncFilter *filter,
                           DskBuffer *out,
                           DskError **error)
{
  DSK_UNUSED (out);
  if (((DskByteUndoubler*)filter)->has_one_byte
   && !((DskByteUndoubler*)filter)->ignore_errors)
    {
      dsk_set_error (error, "terminated with unmatched byte %s",
                     dsk_ascii_byte_name (((DskByteUndoubler*)filter)->byte));
      return false;
    }
  return true;
}

DSK_SYNC_FILTER_SUBCLASS_DEFINE(static, DskByteUndoubler, dsk_byte_undoubler);

DskSyncFilter *
dsk_byte_undoubler_new (uint8_t byte,
                        bool ignore_errors)
{
  DskByteUndoubler *d = dsk_object_new (&dsk_byte_undoubler_class);
  d->byte = byte;
  d->ignore_errors = ignore_errors ? 1 : 0;
  return (DskSyncFilter *) d;
}
