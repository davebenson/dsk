
int dsk_table_helper_openat (const char *openat_dir,
                             int openat_fd,
                             const char *base_filename,
                             const char *suffix,
                             unsigned    open_flags,
                             unsigned    open_mode,
                             DskError  **error);
dsk_boolean dsk_table_helper_renameat (const char *openat_dir,
                                       int openat_fd,
                                       const char *old_name,
                                       const char *new_name,
                                       DskError  **error);
dsk_boolean dsk_table_helper_unlinkat (const char *openat_dir,
                                       int openat_fd,
                                       const char *to_delete,
                                       DskError  **error);
int dsk_table_helper_pread  (int fd,
                             void *buf,
                             size_t len,
                             off_t offset);
