#include "../dsk.h"
#include <string.h>
#include <stdio.h>

static dsk_boolean cmdline_verbose = DSK_FALSE;

static void test_little_endian (void)
{
  uint8_t t_16[2] = {0x01, 0x02};
  uint8_t t_32[4] = {0x01, 0x02, 0x03, 0x04};
  uint8_t t_64[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  uint8_t buf[8];
  dsk_assert (dsk_uint16le_parse (t_16) == 0x0201);
  dsk_assert (dsk_uint32le_parse (t_32) == 0x04030201);
  dsk_assert (dsk_uint64le_parse (t_64) == 0x0807060504030201ULL);
  dsk_uint16le_pack (0x0201, buf);
  dsk_assert (memcmp (t_16, buf, 2) == 0);
  dsk_uint32le_pack (0x04030201, buf);
  dsk_assert (memcmp (t_32, buf, 4) == 0);
  dsk_uint64le_pack (0x0807060504030201ULL, buf);
  dsk_assert (memcmp (t_64, buf, 8) == 0);
}
static void test_big_endian (void)
{
  uint8_t t_16[2] = {0x01, 0x02};
  uint8_t t_32[4] = {0x01, 0x02, 0x03, 0x04};
  uint8_t t_64[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  uint8_t buf[8];
  dsk_assert (dsk_uint16be_parse (t_16) == 0x0102);
  dsk_assert (dsk_uint32be_parse (t_32) == 0x01020304);
  dsk_assert (dsk_uint64be_parse (t_64) == 0x0102030405060708ULL);
  dsk_uint16be_pack (0x0102, buf);
  dsk_assert (memcmp (t_16, buf, 2) == 0);
  dsk_uint32be_pack (0x01020304, buf);
  dsk_assert (memcmp (t_32, buf, 4) == 0);
  dsk_uint64be_pack (0x0102030405060708ULL, buf);
  dsk_assert (memcmp (t_64, buf, 8) == 0);
}


static struct 
{
  const char *name;
  void (*test)(void);
} tests[] =
{
  { "simple little-endian tests", test_little_endian },
  { "simple big-endian tests", test_big_endian },
};
int main(int argc, char **argv)
{
  unsigned i;
  dsk_cmdline_init ("test endianness parsing/printing",
                    "Test routines to pack/parse various integers in various endianness",
                    NULL, 0);
  dsk_cmdline_add_boolean ("verbose", "extra logging", NULL, 0,
                           &cmdline_verbose);
  dsk_cmdline_process_args (&argc, &argv);

  for (i = 0; i < DSK_N_ELEMENTS (tests); i++)
    {
      fprintf (stderr, "Test: %s... ", tests[i].name);
      tests[i].test ();
      fprintf (stderr, " done.\n");
    }
  dsk_cleanup ();
  return 0;
}
