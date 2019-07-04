

void dsk_error_set_tls_alert (DskError *error,
                              DskTlsAlertLevel level,
                              DskTlsAlertDescription description);
bool dsk_error_get_tls_alert (DskError *error,
                              DskTlsAlertLevel *level_out,
                              DskTlsAlertDescription *description_out);
