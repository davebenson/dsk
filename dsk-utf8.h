

/* --- utf-8 string handling --- */
void dsk_utf8_skip_whitespace (const char **p_str);
void dsk_utf8_skip_nonwhitespace (const char **p_str);

char **dsk_utf8_split_on_whitespace (const char *str);

unsigned    dsk_utf8_encode_unichar (char *buf_out,
                                     uint32_t unicode_value);
dsk_boolean dsk_utf8_decode_unichar (unsigned    buf_len,
                                     const char *buf,
                                     unsigned   *bytes_used_out,
                                     uint32_t   *unicode_value_out);

typedef enum
{
  DSK_UTF8_VALIDATION_SUCCESS,
  DSK_UTF8_VALIDATION_PARTIAL,
  DSK_UTF8_VALIDATION_INVALID
} DskUtf8ValidationResult;

DskUtf8ValidationResult
           dsk_utf8_validate        (unsigned    length,
                                     const char *data,
                                     unsigned   *length_out);

