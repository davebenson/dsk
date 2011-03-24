#include "dsk-common.h"
#include "dsk-base64.h"

#define BASE64_LINE_LENGTH  64    /* TODO: look this up!!!!!! */

/* Map from 0..63 as characters. */
char dsk_base64_value_to_char[64] = {
#include "dsk-base64-char-table.inc"
};


/* Map from character to 0..63
 * or -1 for bad byte; -2 for whitespace; -3 for equals */
int8_t dsk_base64_char_to_value[256] = {
#include "dsk-base64-value-table.inc"
};
