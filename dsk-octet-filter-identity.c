#include "dsk.h"

typedef struct _DskSyncFilterIdentityClass DskSyncFilterIdentityClass;
struct _DskSyncFilterIdentityClass
{
  DskSyncFilterClass base_class;
};
typedef struct _DskSyncFilterIdentity DskSyncFilterIdentity;
struct _DskSyncFilterIdentity
{
  DskSyncFilter base_instance;
};
static dsk_boolean
dsk_sync_filter_identity_process (DskSyncFilter *filter,
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
#define dsk_sync_filter_identity_init NULL
#define dsk_sync_filter_identity_finalize NULL
#define dsk_sync_filter_identity_finish NULL

DSK_OCTET_FILTER_SUBCLASS_DEFINE(static, DskSyncFilterIdentity,
                                 dsk_sync_filter_identity);

DskSyncFilter *dsk_sync_filter_identity_new      (void)
{
  return dsk_object_new (&dsk_sync_filter_identity_class);
}
