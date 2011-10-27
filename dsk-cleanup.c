#include "dsk-common.h"
#include "dsk-fd.h"
#include "dsk-object.h"
#include "dsk-error.h"
#include "dsk-buffer.h"
#include "dsk-cmdline.h"
#include "dsk-dispatch.h"
#include "dsk-cleanup.h"

void dsk_cleanup (void)
{
  _dsk_buffer_cleanup_recycling_bin ();
  dsk_dispatch_destroy_default ();
  _dsk_cmdline_cleanup ();
  _dsk_object_cleanup_classes ();
}
