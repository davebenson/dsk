#include "../dsk.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *dsk_tls_object_id_to_string(const DskTlsObjectID *oid)
{
  unsigned tmp_len = DSK_TLS_OBJECT_ID_MAX_SUBID_STRING_SIZE * oid->n_subids;
  char *tmp = alloca(tmp_len);
  char *at = tmp;
  char *end = tmp + tmp_len;
  for (unsigned i = 0; i + 1 < oid->n_subids; i++)
    {
      snprintf (at, end - at, "%u.", oid->subids[i]);
      at = strchr (at, 0);
    }
  snprintf (at, end - at, "%u", oid->subids[oid->n_subids-1]);
  return dsk_strdup (tmp);
}
