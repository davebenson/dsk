


unsigned  dsk_strv_length  (char **strs);
void      dsk_strv_lengths (char **strs,
                            unsigned *n_strings_out,            /* optional */
                            unsigned *total_string_bytes_out);  /* optional */
char    **dsk_strv_concat  (char **a_strs, char **b_strs);
char    **dsk_strvv_concat (char ***str_arrays);
char    **dsk_strv_copy    (char **strs);
void      dsk_strv_free    (char **strs);

/* see also:  dsk_utf8_split_on_whitespace() */

