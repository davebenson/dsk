
/* For more sophisticated unicode handling, see libicu,
 *           http://site.icu-project.org/
 * 
 */

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

DSK_INLINE_FUNC int8_t      dsk_utf8_n_bytes_from_initial (uint8_t initial_byte);

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

#define DSK_UTF8_MAX_LENGTH 4

#define DSK_UTF8_LENGTH_FROM_INIT_BYTE_DIV_8_STR \
  ((int8_t*)"\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\377\377\377\377\377\377\377\377\2\2\2\2\3\3\4\377")



#if DSK_CAN_INLINE || defined(DSK_IMPLEMENT_INLINES)

DSK_INLINE_FUNC int8_t      dsk_utf8_n_bytes_from_initial (uint8_t initial_byte)
{
  return DSK_UTF8_LENGTH_FROM_INIT_BYTE_DIV_8_STR[initial_byte/8];
}
#endif
