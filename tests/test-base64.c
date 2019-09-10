#include "../dsk.h"

static void
charbychar_decode()
{
  DskBase64DecodeProcessor proc = DSK_BASE64_DECODE_PROCESSOR_INIT;
  uint8_t in;
  uint8_t out;
  in = 'a';
  assert(dsk_base64_decode (&proc, 1, &in, &out, NULL) == 0);
  in = 'G';
  assert(dsk_base64_decode (&proc, 1, &in, &out, NULL) == 1);
  assert(out == 'h');
  in = 'V';
  assert(dsk_base64_decode (&proc, 1, &in, &out, NULL) == 1);
  assert(out == 'e');
  in = 's';
  assert(dsk_base64_decode (&proc, 1, &in, &out, NULL) == 1);
  assert(out == 'l');
  in = 'b';
  assert(dsk_base64_decode (&proc, 1, &in, &out, NULL) == 0);
  in = 'G';
  assert(dsk_base64_decode (&proc, 1, &in, &out, NULL) == 1);
  assert(out == 'l');
  in = '8';
  assert(dsk_base64_decode (&proc, 1, &in, &out, NULL) == 1);
  assert(out == 'o');
  in = 'K';
  assert(dsk_base64_decode (&proc, 1, &in, &out, NULL) == 1);
  assert(out == '\n');
}


static struct 
{
  const char *name;
  void (*test)(void);
} tests[] =
{
  { "char-by-char base64 decode", charbychar_decode },
};
int main(int argc, char **argv)
{
  unsigned i;
  dsk_cmdline_init ("test base64 routines",
                    "Test routines for lightweight encoder/decoder",
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
