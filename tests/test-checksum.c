#include "../dsk.h"
#include <string.h>

typedef struct _HashSumTest HashSumTest;
struct _HashSumTest
{
  const char *str;
  char md5[16];
  char sha1[20];
  char sha256[32];
};

static HashSumTest tests[] =
{
  {
    "a",
    { 0x0c, 0xc1, 0x75, 0xb9, 0xc0, 0xf1, 0xb6, 0xa8,
      0x31, 0xc3, 0x99, 0xe2, 0x69, 0x77, 0x26, 0x61 },
    { 0x86, 0xf7, 0xe4, 0x37, 0xfa, 0xa5, 0xa7, 0xfc,
      0xe1, 0x5d, 0x1d, 0xdc, 0xb9, 0xea, 0xea, 0xea,
      0x37, 0x76, 0x67, 0xb8 },
    { 0xca, 0x97, 0x81, 0x12, 0xca, 0x1b, 0xbd, 0xca,
      0xfa, 0xc2, 0x31, 0xb3, 0x9a, 0x23, 0xdc, 0x4d,
      0xa7, 0x86, 0xef, 0xf8, 0x14, 0x7c, 0x4e, 0x72,
      0xb9, 0x80, 0x77, 0x85, 0xaf, 0xee, 0x48, 0xbb }
  }
};
#define TEST_COUNT		DSK_N_ELEMENTS(tests)

int main ()
{
  DskChecksum *h;
  unsigned i;
  for (i = 0; i < TEST_COUNT; i++)
    {
      uint8_t buf[32];

      h = dsk_checksum_new (DSK_CHECKSUM_MD5);
      dsk_checksum_feed_str (h, tests[i].str);
      dsk_checksum_done (h);
      dsk_checksum_get (h, buf);
      dsk_assert (memcmp (buf, tests[i].md5, 16) == 0);
      dsk_checksum_destroy (h);

      h = dsk_checksum_new (DSK_CHECKSUM_SHA1);
      dsk_checksum_feed_str (h, tests[i].str);
      dsk_checksum_done (h);
      dsk_checksum_get (h, buf);
      dsk_assert (memcmp (buf, tests[i].sha1, 20) == 0);
      dsk_checksum_destroy (h);

      h = dsk_checksum_new (DSK_CHECKSUM_SHA256);
      dsk_checksum_feed_str (h, tests[i].str);
      dsk_checksum_done (h);
      dsk_checksum_get (h, buf);
      dsk_assert (memcmp (buf, tests[i].sha256, 32) == 0);
      dsk_checksum_destroy (h);
    }
  return 0;
}

