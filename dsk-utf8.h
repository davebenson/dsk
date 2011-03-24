

/* --- utf-8 string handling --- */
void dsk_utf8_skip_whitespace (const char **p_str);

unsigned    dsk_utf8_encode_unichar (char *buf_out,
                                     uint32_t unicode_value);
dsk_boolean dsk_utf8_decode_unichar (unsigned    buf_len,
                                     const char *buf,
                                     unsigned   *bytes_used_out,
                                     uint32_t   *unicode_value_out);
