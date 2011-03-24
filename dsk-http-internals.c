#include "dsk.h"
#include "dsk-http-internals.h"

dsk_boolean
_dsk_http_scan_for_end_of_header (DskBuffer *buffer,
                                  unsigned *checked_inout,
                                  dsk_boolean permit_empty)
{
  unsigned start = *checked_inout;
  DskBufferFragment *fragment;
  unsigned frag_offset;
  unsigned state;
  uint8_t *at;
  fragment = dsk_buffer_find_fragment (buffer, start, &frag_offset);
  if (fragment == NULL)
    return DSK_FALSE;           /* no new data */

  /* state 0:  non-\n
     state 1:  \n
     state 2:  \n \r
   */
  state = (permit_empty && *checked_inout == 0) ? 1 : 0;
  at = fragment->buf + (start - frag_offset) + fragment->buf_start;
  while (fragment != NULL)
    {
      uint8_t *end = fragment->buf + fragment->buf_start + fragment->buf_length;

      while (at < end)
        {
          if (*at == '\n')
            {
              if (state == 0)
                state = 1;
              else
                {
                  at++;
                  start++;
                  *checked_inout = start;
                  return DSK_TRUE;
                }
            }
          else if (*at == '\r')
            {
              if (state == 1)
                state = 2;
              else
                state = 0;
            }
          else
            state = 0;
          at++;
          start++;
        }

      fragment = fragment->next;
      if (fragment != NULL)
        at = fragment->buf + fragment->buf_start;
    }

  /* hmm. this could obviously get condensed,
     but that would be some pretty magik stuff. */
  if (start < 2)
    start = 0;
  else if (state == 1)
    start -= 1;
  else if (state == 2)
    start -= 2;
  *checked_inout = start;
  return DSK_FALSE;
}

