#include "../dsk.h"

int main()
{
  /* TEST: dsk_ascii_isupper */
  dsk_assert (dsk_ascii_isupper ('A'));
  dsk_assert (dsk_ascii_isupper ('Z'));
  dsk_assert (!dsk_ascii_isupper ('a'));
  dsk_assert (!dsk_ascii_isupper ('z'));
  dsk_assert (!dsk_ascii_isupper ('0'));

  uint16_t pair[2];
  dsk_utf16_to_surrogate_pair (0x10437, pair);
  dsk_assert (pair[0] == 0xd801);
  dsk_assert (pair[1] == 0xdc37);
  dsk_assert (dsk_utf16_surrogate_pair_to_codepoint (0xd801, 0xdc37) == 0x10437);

  dsk_utf16_to_surrogate_pair (0x24b62, pair);
  dsk_assert (pair[0] == 0xd852);
  dsk_assert (pair[1] == 0xdf62);
  dsk_assert (dsk_utf16_surrogate_pair_to_codepoint (0xd852, 0xdf62) == 0x24b62);

  char utf8[6];
  unsigned used;
  uint32_t code;
  dsk_assert (dsk_utf8_encode_unichar (utf8, 0x24) == 1);
  dsk_assert ((uint8_t) utf8[0] == 0x24);
  dsk_assert (dsk_utf8_decode_unichar (6, utf8, &used, &code));
  dsk_assert (used == 1);
  dsk_assert (code == 0x24);
  dsk_assert (dsk_utf8_encode_unichar (utf8, 0xa2) == 2);
  dsk_assert ((uint8_t) utf8[0] == 0xc2);
  dsk_assert ((uint8_t) utf8[1] == 0xa2);
  dsk_assert (dsk_utf8_decode_unichar (6, utf8, &used, &code));
  dsk_assert (used == 2);
  dsk_assert (code == 0xa2);
  dsk_assert (dsk_utf8_encode_unichar (utf8, 0x20ac) == 3);
  dsk_assert ((uint8_t) utf8[0] == 0xe2);
  dsk_assert ((uint8_t) utf8[1] == 0x82);
  dsk_assert ((uint8_t) utf8[2] == 0xac);
  dsk_assert (dsk_utf8_decode_unichar (6, utf8, &used, &code));
  dsk_assert (used == 3);
  dsk_assert (code == 0x20ac);
  dsk_assert (dsk_utf8_encode_unichar (utf8, 0x10348) == 4);
  dsk_assert ((uint8_t) utf8[0] == 0xf0);
  dsk_assert ((uint8_t) utf8[1] == 0x90);
  dsk_assert ((uint8_t) utf8[2] == 0x8d);
  dsk_assert ((uint8_t) utf8[3] == 0x88);
  dsk_assert (dsk_utf8_decode_unichar (6, utf8, &used, &code));
  dsk_assert (used == 4);
  dsk_assert (code == 0x10348);
  return 0;
}
