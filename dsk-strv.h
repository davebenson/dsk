


size_t    dsk_strv_length  (char **strs);
void      dsk_strv_lengths (char **strs,
                            size_t *n_strings_out,            /* optional */
                            size_t *total_string_bytes_out);  /* optional */
char    **dsk_strv_concat  (char **a_strs, char **b_strs);
char    **dsk_strvv_concat (char ***str_arrays);
char    **dsk_strv_copy    (char **strs);
void      dsk_strv_free    (char **strs);

char    **dsk_strsplit     (const char *str,
                            const char *sep);

/* see also:  dsk_utf8_split_on_whitespace() */


char   **dsk_strv_copy_compact (char **strv);
