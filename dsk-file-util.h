// XXX: i think these are all deprecated by the DskDir API.

#define DSK_DIR_SEPARATOR  '/'
#define DSK_DIR_SEPARATOR_S  "/"

uint8_t    *dsk_file_get_contents (const char    *filename,
                                   size_t        *size_out,
			           DskError     **error);
dsk_boolean dsk_file_set_contents (const char    *filename,
                                   size_t         size,
                                   const uint8_t *contents,
			           DskError     **error);

dsk_boolean dsk_file_test_exists  (const char    *filename);

dsk_boolean dsk_mkdir_recursive (const char *dir,
                                 unsigned    permissions,
                                 DskError  **error);
dsk_boolean dsk_mkdir_recursive_at (int         at_file,
                                    const char *dir,
                                    unsigned    permissions,
                                    DskError  **error);

const char *dsk_get_tmp_dir (void);

dsk_boolean dsk_rm_rf   (const char *dir_or_file,
                         DskError    **error);
dsk_boolean dsk_remove_dir_recursive (const char *dir,
                                      DskError  **error);
