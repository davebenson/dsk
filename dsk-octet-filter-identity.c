#include "dsk.h"

typedef struct _DskOctetFilterIdentityClass DskOctetFilterIdentityClass;
struct _DskOctetFilterIdentityClass
{
  DskOctetFilterClass base_class;
};
typedef struct _DskOctetFilterIdentity DskOctetFilterIdentity;
struct _DskOctetFilterIdentity
{
  DskOctetFilter base_instance;
};
static dsk_boolean
dsk_octet_filter_identity_process (DskOctetFilter *filter,
                                   DskBuffer      *out,
                                   unsigned        in_length,
                                   const uint8_t  *in_data,
                                   DskError      **error)
{
  DSK_UNUSED (filter);
  DSK_UNUSED (error);
  dsk_buffer_append (out, in_length, in_data);
  return DSK_TRUE;
}
#define dsk_octet_filter_identity_init NULL
#define dsk_octet_filter_identity_finalize NULL
#define dsk_octet_filter_identity_finish NULL

DSK_OCTET_FILTER_SUBCLASS_DEFINE(static, DskOctetFilterIdentity,
                                 dsk_octet_filter_identity);

DskOctetFilter *dsk_octet_filter_identity_new      (void)
{
  return dsk_object_new (&dsk_octet_filter_identity_class);
}
