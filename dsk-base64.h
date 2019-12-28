
/* --- the low-level aspects of base-64 inclusion --- */

/* Map from 0..63 as characters. */
extern char dsk_base64_value_to_char[64];

/* Map from character to 0..63
 * or -1 for bad byte; -2 for whitespace; -3 for equals */
extern int8_t dsk_base64_char_to_value[256];


typedef struct DskBase64DecodeProcessor DskBase64DecodeProcessor;
struct DskBase64DecodeProcessor
{
  uint8_t state;               // 0,1,2,3 
  uint8_t partial;
};

#define DSK_BASE64_DECODE_OUTPUT_FOR_INPUT(inlen) \
  (((inlen) * 6 + 4) / 8)
#define DSK_BASE64_DECODE_PROCESSOR_INIT {0,0}
ptrdiff_t dsk_base64_decode (DskBase64DecodeProcessor *p,
                             size_t len,
                             const uint8_t *in_data,
                             uint8_t *out_data,
                             DskError **error);


       

