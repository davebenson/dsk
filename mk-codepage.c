#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <iconv.h>
#include <errno.h>
#include <assert.h>

int main(int argc, char **argv)
{
  char out[1000];
  uint8_t lengths[128];
  unsigned i;

  if (argc != 2)
    {
      fprintf (stderr, "usage: %s codepage\n", argv[0]);
      return 1;
    }
  const char *codepage = argv[1];
  char *c_codepage = strdup (codepage);
  for (i = 0; c_codepage[i]; i++)
    if (c_codepage[i] == '-')
      c_codepage[i] = '_';

  iconv_t ic = iconv_open ("UTF-8", codepage);
  if (ic == NULL || ic == (iconv_t)(-1))
    {
      fprintf (stderr, "iconv_open failed (%s): %s\n",
               codepage, strerror (errno));
      return 1;
    }
  
  printf ("#include \"../dsk.h\"\n");
  printf ("static const uint8_t %s_heap[] = {", c_codepage);
  for (i = 128; i < 256; i++)
    {
      char c;
      c = i;
      char *in = &c;
      size_t in_size = 1;
      char *o = (char*) out;
      size_t out_rem = 1000;
      size_t s = iconv (ic, &in, &in_size, &o, &out_rem);
      if (s == (size_t)-1)
        {
          fprintf (stderr, "iconv failed for char 0x%02x: %s\n",
                   i, strerror (errno));
          lengths[i-128] = 0;
          iconv (ic, NULL,NULL,NULL,NULL);
          continue;
        }
      assert (in_size == 0);
      unsigned out_size = 1000 - out_rem;
      assert (out_size > 0);

      unsigned cc;
      for (cc = 0; cc < out_size; cc++)
        {
	  printf ("0x%02x,", (uint8_t)out[cc]);
	}
      lengths[i-128] = cc;
    }
  printf ("};\n");
  printf ("const DskCodepage dsk_codepage_%s = {\n  {", c_codepage);
  unsigned cum = 0;
  for (i = 0; i < 128; i++)
    {
      printf ("%u,", cum);
      cum += lengths[i];
    }
  printf ("%u}, %s_heap};\n", cum, c_codepage);

  return 0;
}
