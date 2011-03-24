#include "../dsk.h"

int main()
{
  /* TEST: dsk_ascii_isupper */
  dsk_assert (dsk_ascii_isupper ('A'));
  dsk_assert (dsk_ascii_isupper ('Z'));
  dsk_assert (!dsk_ascii_isupper ('a'));
  dsk_assert (!dsk_ascii_isupper ('z'));
  dsk_assert (!dsk_ascii_isupper ('0'));

  return 0;
}
