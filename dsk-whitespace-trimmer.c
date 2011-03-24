#include "dsk.h"

typedef struct _DskWhitespaceTrimmerClass DskWhitespaceTrimmerClass;
struct _DskWhitespaceTrimmerClass
{
  DskOctetFilterClass base_class;
};
typedef struct _DskWhitespaceTrimmer DskWhitespaceTrimmer;
struct _DskWhitespaceTrimmer
{
  DskOctetFilter base_instance;
  uint8_t in_initial_space;
  uint8_t in_space;
};
static void dsk_whitespace_trimmer_init (DskWhitespaceTrimmer *trimmer)
{
  trimmer->in_space = trimmer->in_initial_space = DSK_TRUE;
}
#define dsk_whitespace_trimmer_finalize NULL

static dsk_boolean
dsk_whitespace_trimmer_process    (DskOctetFilter *filter,
                                   DskBuffer      *out,
                                   unsigned        in_length,
                                   const uint8_t  *in_data,
                                   DskError      **error)
{
  DskWhitespaceTrimmer *trimmer = (DskWhitespaceTrimmer *) filter;
  uint8_t in_space = trimmer->in_space;
  DSK_UNUSED (error);
  while (in_length > 0)
    {
      uint8_t c = *in_data++;
      if (dsk_ascii_isspace (c))
        {
          in_space = DSK_TRUE;
        }
      else
        {
          if (in_space)
            {
              if (trimmer->in_initial_space)
                trimmer->in_initial_space = DSK_FALSE;
              else
                dsk_buffer_append_byte (out, ' ');
            }
          in_space = DSK_FALSE;
          dsk_buffer_append_byte (out, c);
        }
      in_length--;
    }
  trimmer->in_space = in_space;
  return DSK_TRUE;
}

#define dsk_whitespace_trimmer_finish NULL

DSK_OCTET_FILTER_SUBCLASS_DEFINE(static, DskWhitespaceTrimmer, dsk_whitespace_trimmer);

DskOctetFilter *dsk_whitespace_trimmer_new         (void)
{
  DskWhitespaceTrimmer *trimmer = dsk_object_new (&dsk_whitespace_trimmer_class);
  trimmer->in_initial_space = trimmer->in_space = DSK_TRUE;
  return DSK_OCTET_FILTER (trimmer);
}
  
