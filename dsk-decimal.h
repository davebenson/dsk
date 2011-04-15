
/* includes '-' and NUL */
#define DSK_DECIMAL_INT32_SIZE		12

/* includes NUL */
#define DSK_DECIMAL_UINT32_SIZE		11

/* includes '-' and NUL */
#define DSK_DECIMAL_INT64_SIZE		21

/* includes NUL */
#define DSK_DECIMAL_UINT64_SIZE		21

dsk_boolean dsk_uint32_parse_decimal (const char  *str,
                                      const char **end_out,
				      uint32_t    *value_out);
dsk_boolean dsk_int32_parse_decimal  (const char  *str,
                                      const char **end_out,
				      int32_t     *value_out);
dsk_boolean dsk_uint64_parse_decimal (const char  *str,
                                      const char **end_out,
				      uint64_t    *value_out);
dsk_boolean dsk_int64_parse_decimal  (const char  *str,
                                      const char **end_out,
				      int64_t     *value_out);

void        dsk_uint32_print_decimal (uint32_t     in,
                                      char        *out);
void        dsk_int32_print_decimal  (int32_t      in,
                                      char        *out);
void        dsk_uint64_print_decimal (uint64_t     in,
                                      char        *out);
void        dsk_int64_print_decimal  (int64_t      in,
                                      char        *out);
