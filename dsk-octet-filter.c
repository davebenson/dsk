#include "dsk.h"

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskOctetFilter);
const DskOctetFilterClass dsk_octet_filter_class =
{
  DSK_OBJECT_CLASS_DEFINE (DskOctetFilter, &dsk_object_class, NULL, NULL),
  NULL, NULL
};
dsk_boolean          dsk_octet_filter_process_buffer (DskOctetFilter *filter,
                                                      DskBuffer      *out,
                                                      unsigned        in_len,
                                                      DskBuffer      *in,
                                                      dsk_boolean     discard,
                                                      DskError      **error)
{
  DskBufferFragment *at = in->first_frag;
  unsigned to_discard;
  if (in_len > in->size)
    in_len = in->size;
  to_discard = discard ? in_len : 0;
  while (in_len != 0)
    {
      unsigned process = at->buf_length;
      if (process > in_len)
        process = in_len;
      if (!dsk_octet_filter_process (filter, out, process, at->buf + at->buf_start, error))
        return DSK_FALSE;
      in_len -= process;
      at = at->next;
    }
  dsk_buffer_discard (in, to_discard);
  return DSK_TRUE;
}

dsk_boolean dsk_filter_to_buffer  (unsigned length,
                                   const uint8_t *data,
                                   DskOctetFilter *filter,
                                   DskBuffer *output,
                                   DskError **error)
{
  if (!dsk_octet_filter_process (filter, output, length, data, error)
   || !dsk_octet_filter_finish (filter, output, error))
    {
      return DSK_FALSE;
    }
  dsk_object_unref (filter);
  return DSK_TRUE;
}
