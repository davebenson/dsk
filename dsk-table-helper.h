
int dsk_table_helper_openat (DskTableLocation *location,
                             const char *base_filename,
                             const char *suffix,
                             unsigned    open_flags,
                             unsigned    open_mode,
                             DskError  **error);
dsk_boolean dsk_table_helper_renameat (DskTableLocation *location,
                                       const char *old_name,
                                       const char *new_name,
                                       DskError  **error);
dsk_boolean dsk_table_helper_unlinkat (DskTableLocation *location,
                                       const char *to_delete,
                                       DskError  **error);
int dsk_table_helper_pread  (int fd,
                             void *buf,
                             size_t len,
                             off_t offset);
