
/* --- the low-level aspects of base-64 inclusion --- */

/* Map from 0..63 as characters. */
extern char dsk_base64_value_to_char[64];

/* Map from character to 0..63
 * or -1 for bad byte; -2 for whitespace; -3 for equals */
extern int8_t dsk_base64_char_to_value[256];

