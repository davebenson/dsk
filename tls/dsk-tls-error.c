#include "../dsk.h"

typedef struct _TlsAlertInfo TlsAlertInfo;
struct _TlsAlertInfo {
  DskTlsAlertLevel level;
  DskTlsAlertDescription description;
};

static void tls_alert_info_clear(DskErrorDataType *type, void *data)
{
  DSK_UNUSED(type);
  DSK_UNUSED(data);
}

static void tls_alert_info_copy(DskErrorDataType *type, void *data, const void *src_data)
{
  DSK_UNUSED(type);
  * (TlsAlertInfo *) data = * (const TlsAlertInfo *) src_data;
}

static void tls_alert_info_to_string(DskErrorDataType *type, const void *data, DskBuffer *out)
{
  DSK_UNUSED(type);
  const TlsAlertInfo *alert_info = data;
  dsk_buffer_append_string (out, "TLS Alert: ");
  dsk_buffer_append_string (out, dsk_tls_alert_level_name (alert_info->level));
  dsk_buffer_append_string (out, ": ");
  dsk_buffer_append_string (out, dsk_tls_alert_description_name (alert_info->description));
}

static DskErrorDataType dsk_error_data_tls_alert_info = {
  "DskTlsAlertInfo",
  sizeof(TlsAlertInfo),
  tls_alert_info_clear,
  tls_alert_info_copy,
  tls_alert_info_to_string
};

void dsk_error_set_tls_alert (DskError *error,
                              DskTlsAlertLevel level,
                              DskTlsAlertDescription description)
{
  TlsAlertInfo alert_info = { level, description };
  TlsAlertInfo *dst = dsk_error_force_data (error, &dsk_error_data_tls_alert_info, NULL);
  *dst = alert_info;
}

bool dsk_error_get_tls_alert (DskError *error,
                              DskTlsAlertLevel *level_out,
                              DskTlsAlertDescription *description_out)
{
  TlsAlertInfo *alert_info = dsk_error_find_data (error, &dsk_error_data_tls_alert_info);
  if (alert_info)
    {
      *level_out = alert_info->level;
      *description_out = alert_info->description;
      return true;
    }
  return false;
}
