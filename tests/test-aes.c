#include "../tls/dsk-aes.h"

static void
test_key_expansion_128 (void)
{
  /* FIPS 197, A.1. Expansion of a 128-bit Cipher Key */
  uint8_t cipher_key = {
   0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
   0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
  };
  DskAES128 aes128;
  dsk_aes128_init (&aes128, cipher_key);
  assert (memcmp (aes128.w, 
                            "\x2b\x7e\x15\x16"             //w0
                            "\x28\xae\xd2\xa6"             //w1
                            "\xab\xf7\x15\x88"             //w2
                            "\x09\xcf\x4f\x3c"             //w3
                            "\xa0\xfa\xfe\x17"             //w4
                            "\x88\x54\x2c\xb1"
                            "\x23\xa3\x39\x39"
                            "\x2a\x6c\x76\x05"
                            "\xf2\xc2\x95\xf2"             // w8
                            "\x7a\x96\xb9\x43"
                            "\x59\x35\x80\x7a"
                            "\x73\x59\xf6\x7f"
                            "\x3d\x80\x47\x7d"
                            "\x47\x16\xfe\x3e"
                            "\x1e\x23\x7e\x44"
                            "\x6d\x7a\x88\x3b"
                            "\xef\x44\xa5\x41"             // w16
                            "\xa8\x52\x5b\x7f"
                            "\xb6\x71\x25\x3b"
                            "\xdb\x0b\xad\x00"
                            "\xd4\xd1\xc6\xf8"             // w20
                            "\x7c\x83\x9d\x87"
                            "\xca\xf2\xb8\xbc"
                            "\x11\xf9\x15\xbc"
                            "\x6d\x88\xa3\x7a"             // w24
                            "\x11\x0b\x3e\xfd"
                            "\xdb\xf9\x86\x41"
                            "\xca\x00\x93\xfd"
                            "\x4e\x54\xf7\x0e"
                            "\x5f\x5f\xc9\xf3"
                            "\x84\xa6\x4f\xb2"
                            "\x4e\xa6\xdc\x4f"
                            "\xea\xd2\x73\x21"
                            "\xb5\x8d\xba\xd2"
                            "\x31\x2b\xf5\x60"
                            "\x7f\x8d\x29\x2f"
                            "\xac\x77\x66\xf3"             // w36
                            "\x19\xfa\xdc\x21"
                            "\x28\xd1\x29\x41"
                            "\x57\x5c\x00\x6e"
                            "\xd0\x14\xf9\xa8"             // w40
                            "\xc9\xee\x25\x89"
                            "\xe1\x3f\x0c\xc8"
                            "\xb6\x63\x0c\xa6", (10 + 1) * 4 * 4) == 0);
}

int main()
{
  test_key_expansion_128 ();
  return 0;
}
