#include "../dsk.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static dsk_boolean cmdline_verbose = false;

static void
test_aes128 (void)
{
  /* FIPS 197, A.1. Expansion of a 128-bit Cipher Key */
  uint8_t cipher_key[16] = {
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

  const uint8_t plaintext[16] = "\x32\x43\xf6\xa8\x88\x5a\x30\x8d\x31\x31\x98\xa2\xe0\x37\x07\x34";
  const uint8_t ciphertext[16] = "\x39\x25\x84\x1d\x02\xdc\x09\xfb\xdc\x11\x85\x97\x19\x6a\x0b\x32";
  uint8_t tmp[16];

  memcpy (tmp, plaintext, 16);
  dsk_aes128_encrypt_inplace (&aes128, tmp);
  assert (memcmp (tmp, ciphertext, 16) == 0);

  memcpy (tmp, ciphertext, 16);
  dsk_aes128_decrypt_inplace (&aes128, tmp);
  assert (memcmp (tmp, plaintext, 16) == 0);
}

static void
test_aes192 (void)
{
  DskAES192 aes192;
  dsk_aes192_init (&aes192,
        (uint8_t*) "\x00\x01\x02\x03\x04\x05\x06\x07"
                   "\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
                   "\x10\x11\x12\x13\x14\x15\x16\x17");
  const uint8_t plaintext[16] = "\x00\x11\x22\x33\x44\x55\x66\x77"
                                "\x88\x99\xaa\xbb\xcc\xdd\xee\xff";
  const uint8_t ciphertext[16]= "\xdd\xa9\x7c\xa4\x86\x4c\xdf\xe0"
                                "\x6e\xaf\x70\xa0\xec\x0d\x71\x91";
  uint8_t tmp[16];
  memcpy (tmp, plaintext, 16);
  dsk_aes192_encrypt_inplace (&aes192, tmp);
  assert (memcmp (tmp, ciphertext, 16) == 0);
  memcpy (tmp, ciphertext, 16);
  dsk_aes192_decrypt_inplace (&aes192, tmp);
  assert (memcmp (tmp, plaintext, 16) == 0);
}

static void
test_aes256 (void)
{
  DskAES256 aes256;
  dsk_aes256_init (&aes256,
        (uint8_t*) "\x00\x01\x02\x03\x04\x05\x06\x07"
                   "\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
                   "\x10\x11\x12\x13\x14\x15\x16\x17"
                   "\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f");
  const uint8_t plaintext[16] = "\x00\x11\x22\x33\x44\x55\x66\x77"
                                "\x88\x99\xaa\xbb\xcc\xdd\xee\xff";
  const uint8_t ciphertext[16]= "\x8e\xa2\xb7\xca\x51\x67\x45\xbf"
                                "\xea\xfc\x49\x90\x4b\x49\x60\x89";
  uint8_t tmp[16];
  memcpy (tmp, plaintext, 16);
  dsk_aes256_encrypt_inplace (&aes256, tmp);
  assert (memcmp (tmp, ciphertext, 16) == 0);
  memcpy (tmp, ciphertext, 16);
  dsk_aes256_decrypt_inplace (&aes256, tmp);
  assert (memcmp (tmp, plaintext, 16) == 0);
}

/*
https://nvlpubs.nist.gov/nistpubs/Legacy/SP/nistspecialpublication800-38a.pdf
*/
static void
test_aes_from_ecb_examples (void)
{
  DskAES128 aes128;
  dsk_aes128_init (&aes128, (uint8_t *) "\x2b\x7e\x15\x16\x28\xae\xd2\xa6\xab\xf7\x15\x88\x09\xcf\x4f\x3c");
  uint8_t tmp[16];

  //
  // Tests from 880-38a F.1.1 (equiv to F.1.2). [for AES-128]
  //

  static struct {
    const uint8_t p[16];
    const uint8_t c[16];
  } blocks_aes128[4] = {
    {
      "\x6b\xc1\xbe\xe2\x2e\x40\x9f\x96\xe9\x3d\x7e\x11\x73\x93\x17\x2a",
      "\x3a\xd7\x7b\xb4\x0d\x7a\x36\x60\xa8\x9e\xca\xf3\x24\x66\xef\x97"
    },
    {
      "\xae\x2d\x8a\x57\x1e\x03\xac\x9c\x9e\xb7\x6f\xac\x45\xaf\x8e\x51",
      "\xf5\xd3\xd5\x85\x03\xb9\x69\x9d\xe7\x85\x89\x5a\x96\xfd\xba\xaf"
    },
    {
      "\x30\xc8\x1c\x46\xa3\x5c\xe4\x11\xe5\xfb\xc1\x19\x1a\x0a\x52\xef",
      "\x43\xb1\xcd\x7f\x59\x8e\xce\x23\x88\x1b\x00\xe3\xed\x03\x06\x88"
    },
    {
      "\xf6\x9f\x24\x45\xdf\x4f\x9b\x17\xad\x2b\x41\x7b\xe6\x6c\x37\x10",
      "\x7b\x0c\x78\x5e\x27\xe8\xad\x3f\x82\x23\x20\x71\x04\x72\x5d\xd4"
    }
  };

  for (int b = 0; b < 4; b++)
    {
      memcpy (tmp, blocks_aes128[b].p, 16);
      dsk_aes128_encrypt_inplace (&aes128, tmp);
      assert(memcmp(tmp, blocks_aes128[b].c, 16) == 0);
      dsk_aes128_decrypt_inplace (&aes128, tmp);
      assert(memcmp(tmp, blocks_aes128[b].p, 16) == 0);
    }

  //
  // Tests from 880-38a F.1.3 (equiv to F.1.4). [for AES-192]
  //
  DskAES192 aes192;
  dsk_aes192_init (&aes192, (uint8_t *) "\x8e\x73\xb0\xf7\xda\x0e\x64\x52\xc8\x10\xf3\x2b\x80\x90\x79\xe5\x62\xf8\xea\xd2\x52\x2c\x6b\x7b");
    static struct {
    const uint8_t p[16];
    const uint8_t c[16];
  } blocks_aes192[4] = {
    {
      "\x6b\xc1\xbe\xe2\x2e\x40\x9f\x96\xe9\x3d\x7e\x11\x73\x93\x17\x2a",
      "\xbd\x33\x4f\x1d\x6e\x45\xf2\x5f\xf7\x12\xa2\x14\x57\x1f\xa5\xcc",
    },
    {
      "\xae\x2d\x8a\x57\x1e\x03\xac\x9c\x9e\xb7\x6f\xac\x45\xaf\x8e\x51",
      "\x97\x41\x04\x84\x6d\x0a\xd3\xad\x77\x34\xec\xb3\xec\xee\x4e\xef",
    },
    { 
      "\x30\xc8\x1c\x46\xa3\x5c\xe4\x11\xe5\xfb\xc1\x19\x1a\x0a\x52\xef",
      "\xef\x7a\xfd\x22\x70\xe2\xe6\x0a\xdc\xe0\xba\x2f\xac\xe6\x44\x4e"
    },
    {
      "\xf6\x9f\x24\x45\xdf\x4f\x9b\x17\xad\x2b\x41\x7b\xe6\x6c\x37\x10",
      "\x9a\x4b\x41\xba\x73\x8d\x6c\x72\xfb\x16\x69\x16\x03\xc1\x8e\x0e"
    }
  };
  for (int b = 0; b < 4; b++)
    {
      memcpy (tmp, blocks_aes192[b].p, 16);
      dsk_aes192_encrypt_inplace (&aes192, tmp);
      assert(memcmp(tmp, blocks_aes192[b].c, 16) == 0);
      dsk_aes192_decrypt_inplace (&aes192, tmp);
      assert(memcmp(tmp, blocks_aes192[b].p, 16) == 0);
    }

  //
  // Tests from 880-38a F.1.5 (equiv to F.1.6). [for AES-256]
  //
  DskAES256 aes256;
  dsk_aes256_init (&aes256,
         (uint8_t *) "\x60\x3d\xeb\x10\x15\xca\x71\xbe"
                     "\x2b\x73\xae\xf0\x85\x7d\x77\x81"
                     "\x1f\x35\x2c\x07\x3b\x61\x08\xd7"
                     "\x2d\x98\x10\xa3\x09\x14\xdf\xf4");
  static struct {
    const uint8_t p[16];
    const uint8_t c[16];
  } blocks_aes256[4] = {
    {
      "\x6b\xc1\xbe\xe2\x2e\x40\x9f\x96\xe9\x3d\x7e\x11\x73\x93\x17\x2a",
      "\xf3\xee\xd1\xbd\xb5\xd2\xa0\x3c\x06\x4b\x5a\x7e\x3d\xb1\x81\xf8"
    },
    {
      "\xae\x2d\x8a\x57\x1e\x03\xac\x9c\x9e\xb7\x6f\xac\x45\xaf\x8e\x51",
      "\x59\x1c\xcb\x10\xd4\x10\xed\x26\xdc\x5b\xa7\x4a\x31\x36\x28\x70"
    },
    {
      "\x30\xc8\x1c\x46\xa3\x5c\xe4\x11\xe5\xfb\xc1\x19\x1a\x0a\x52\xef",
      "\xb6\xed\x21\xb9\x9c\xa6\xf4\xf9\xf1\x53\xe7\xb1\xbe\xaf\xed\x1d"
    },
    {
      "\xf6\x9f\x24\x45\xdf\x4f\x9b\x17\xad\x2b\x41\x7b\xe6\x6c\x37\x10",
      "\x23\x30\x4b\x7a\x39\xf9\xf3\xff\x06\x7d\x8d\x8f\x9e\x24\xec\xc7"
    }
  };
  for (int b = 0; b < 4; b++)
    {
      memcpy (tmp, blocks_aes256[b].p, 16);
      dsk_aes256_encrypt_inplace (&aes256, tmp);
      assert(memcmp(tmp, blocks_aes256[b].c, 16) == 0);
      dsk_aes256_decrypt_inplace (&aes256, tmp);
      assert(memcmp(tmp, blocks_aes256[b].p, 16) == 0);
    }
}

/* Test vectors from:
 *    David A McGrew and John Viega, "The Galois/Counter Mode of Operation (GCM)".
 *    http://luca-giuzzi.unibs.it/corsi/Support/papers-cryptography/gcm-spec.pdf
 *
 * These tests are from Appendix B, AES Test Vectors.
 */
static void
test_aes_gcm (void)
{
  DskAES128 aes128;
  Dsk_AEAD_GCM_Precomputation precompute;
  uint8_t auth_tag[16];
  uint8_t ciphertext_out[16*4];
  uint8_t plaintext_out[16*4];


  //
  // Test Case 1.
  //
  dsk_aes128_init (&aes128, (uint8_t *) "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");
  dsk_aead_gcm_precompute ((DskBlockCipherInplaceFunc) dsk_aes128_encrypt_inplace,
                           &aes128,
                           &precompute);
  dsk_aead_gcm_encrypt (&precompute,
                        0, (uint8_t *) "",              // plaintext
                        0, (uint8_t *) "",              // assoc data
                        12, (uint8_t *) "\0\0\0\0\0\0\0\0\0\0\0\0",             // iv
                        ciphertext_out,
                        16, auth_tag);
  assert (memcmp (auth_tag, "\x58\xe2\xfc\xce\xfa\x7e\x30\x61\x36\x7f\x1d\x57\xa4\xe7\x45\x5a", 16) == 0);
  if (!dsk_aead_gcm_decrypt (&precompute,
                        0, ciphertext_out,              // ciphertext
                        0, (uint8_t *) "",              // assoc data
                        12, (uint8_t *) "\0\0\0\0\0\0\0\0\0\0\0\0",             // iv
                        plaintext_out,
                        16, auth_tag))
    assert(false);

  //
  // Test Case 2. 
  //
  dsk_aead_gcm_encrypt (&precompute,
                        16, (uint8_t *) "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",    // plaintext
                        0, (uint8_t *) "",              // assoc data
                        12, (uint8_t *) "\0\0\0\0\0\0\0\0\0\0\0\0",             // iv
                        ciphertext_out,
                        16, auth_tag);
  assert (memcmp (ciphertext_out, "\x03\x88\xda\xce\x60\xb6\xa3\x92\xf3\x28\xc2\xb9\x71\xb2\xfe\x78", 16) == 0);
  assert (memcmp (auth_tag, "\xab\x6e\x47\xd4\x2c\xec\x13\xbd\xf5\x3a\x67\xb2\x12\x57\xbd\xdf", 16) == 0);
  if (!dsk_aead_gcm_decrypt (&precompute,
                        16, ciphertext_out,              // ciphertext
                        0, (uint8_t *) "",              // assoc data
                        12, (uint8_t *) "\0\0\0\0\0\0\0\0\0\0\0\0",             // iv
                        plaintext_out,
                        16, auth_tag))
    assert(false);
  assert (memcmp (plaintext_out, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 16) == 0);

  //
  // Test Case 3.
  //
  dsk_aes128_init (&aes128, (uint8_t *) "\xfe\xff\xe9\x92\x86\x65\x73\x1c\x6d\x6a\x8f\x94\x67\x30\x83\x08");
  dsk_aead_gcm_precompute ((DskBlockCipherInplaceFunc) dsk_aes128_encrypt_inplace,
                           &aes128,
                           &precompute);
  const uint8_t *plaintext_case_3 = (uint8_t *) 
                           "\xd9\x31\x32\x25\xf8\x84\x06\xe5\xa5\x59\x09\xc5\xaf\xf5\x26\x9a"
                           "\x86\xa7\xa9\x53\x15\x34\xf7\xda\x2e\x4c\x30\x3d\x8a\x31\x8a\x72"
                           "\x1c\x3c\x0c\x95\x95\x68\x09\x53\x2f\xcf\x0e\x24\x49\xa6\xb5\x25"
                           "\xb1\x6a\xed\xf5\xaa\x0d\xe6\x57\xba\x63\x7b\x39\x1a\xaf\xd2\x55";
  const uint8_t *iv_case_3 = (uint8_t *) "\xca\xfe\xba\xbe\xfa\xce\xdb\xad\xde\xca\xf8\x88";
  dsk_aead_gcm_encrypt (&precompute,
                        64, plaintext_case_3,
                        0, (uint8_t *) "",
                        12, iv_case_3,
                        ciphertext_out,
                        16, auth_tag);
  assert (memcmp (ciphertext_out,
                  "\x42\x83\x1e\xc2\x21\x77\x74\x24\x4b\x72\x21\xb7\x84\xd0\xd4\x9c"
                  "\xe3\xaa\x21\x2f\x2c\x02\xa4\xe0\x35\xc1\x7e\x23\x29\xac\xa1\x2e"
                  "\x21\xd5\x14\xb2\x54\x66\x93\x1c\x7d\x8f\x6a\x5a\xac\x84\xaa\x05"
                  "\x1b\xa3\x0b\x39\x6a\x0a\xac\x97\x3d\x58\xe0\x91\x47\x3f\x59\x85",
                  64) == 0);
  assert (memcmp (auth_tag,
                  "\x4d\x5c\x2a\xf3\x27\xcd\x64\xa6\x2c\xf3\x5a\xbd\x2b\xa6\xfa\xb4",
                  16) == 0);
  if (!dsk_aead_gcm_decrypt (&precompute,
                        64, ciphertext_out,              // ciphertext
                        0, (uint8_t *) "",              // assoc data
                        12, iv_case_3,
                        plaintext_out,
                        16, auth_tag))
    assert(false);
  assert (memcmp (plaintext_out, plaintext_case_3, 64) == 0);


  //
  // Test Case 4.  Adds associated_data.
  //
  dsk_aes128_init (&aes128, (uint8_t *) "\xfe\xff\xe9\x92\x86\x65\x73\x1c\x6d\x6a\x8f\x94\x67\x30\x83\x08");
  dsk_aead_gcm_precompute ((DskBlockCipherInplaceFunc) dsk_aes128_encrypt_inplace,
                           &aes128,
                           &precompute);
  const uint8_t *plaintext_case_4 = (uint8_t *)
                           "\xd9\x31\x32\x25\xf8\x84\x06\xe5\xa5\x59\x09\xc5\xaf\xf5\x26\x9a"
                           "\x86\xa7\xa9\x53\x15\x34\xf7\xda\x2e\x4c\x30\x3d\x8a\x31\x8a\x72"
                           "\x1c\x3c\x0c\x95\x95\x68\x09\x53\x2f\xcf\x0e\x24\x49\xa6\xb5\x25"
                           "\xb1\x6a\xed\xf5\xaa\x0d\xe6\x57\xba\x63\x7b\x39";   // 60 bytes long
  const uint8_t *assoc_data_case_4 = (uint8_t *)
                           "\xfe\xed\xfa\xce\xde\xad\xbe\xef\xfe\xed\xfa\xce\xde\xad\xbe\xef"
                           "\xab\xad\xda\xd2";
  const uint8_t *iv_case_4 =  (uint8_t *) "\xca\xfe\xba\xbe\xfa\xce\xdb\xad\xde\xca\xf8\x88";
  dsk_aead_gcm_encrypt (&precompute,
                        60, plaintext_case_4,
                        20, assoc_data_case_4,
                        12, iv_case_4,
                        ciphertext_out,
                        16, auth_tag);
  const uint8_t *ciphertext_case_4 = (uint8_t *)
                           "\x42\x83\x1e\xc2\x21\x77\x74\x24\x4b\x72\x21\xb7\x84\xd0\xd4\x9c"
                           "\xe3\xaa\x21\x2f\x2c\x02\xa4\xe0\x35\xc1\x7e\x23\x29\xac\xa1\x2e"
                           "\x21\xd5\x14\xb2\x54\x66\x93\x1c\x7d\x8f\x6a\x5a\xac\x84\xaa\x05"
                           "\x1b\xa3\x0b\x39\x6a\x0a\xac\x97\x3d\x58\xe0\x91";
  assert (memcmp (ciphertext_out, ciphertext_case_4, 60) == 0);
  assert (memcmp (auth_tag, "\x5b\xc9\x4f\xbc\x32\x21\xa5\xdb\x94\xfa\xe9\x5a\xe7\x12\x1a\x47", 16) == 0);
  if (!dsk_aead_gcm_decrypt (&precompute,
                        60, ciphertext_out,              // ciphertext
                        20, assoc_data_case_4,
                        12, iv_case_4,
                        plaintext_out,
                        16, auth_tag))
    assert(false);
  assert (memcmp (plaintext_out, plaintext_case_4, 60) == 0);

  //
  // Test Case 5.
  //
  const uint8_t *plaintext_case_5 = plaintext_case_4;
  const uint8_t *assoc_data_case_5 = assoc_data_case_4;
  const uint8_t *iv_case_5 = (uint8_t *) "\xca\xfe\xba\xbe\xfa\xce\xdb\xad"; // length=8
  // key is same as test-case 4.
  dsk_aead_gcm_encrypt (&precompute,
                        60, plaintext_case_5,
                        20, assoc_data_case_5,
                        8, iv_case_5,
                        ciphertext_out,
                        16, auth_tag);
  const uint8_t *ciphertext_case_5 = (uint8_t *)
                                     "\x61\x35\x3b\x4c\x28\x06\x93\x4a\x77\x7f\xf5\x1f\xa2\x2a\x47\x55"
                                     "\x69\x9b\x2a\x71\x4f\xcd\xc6\xf8\x37\x66\xe5\xf9\x7b\x6c\x74\x23"
                                     "\x73\x80\x69\x00\xe4\x9f\x24\xb2\x2b\x09\x75\x44\xd4\x89\x6b\x42"
                                     "\x49\x89\xb5\xe1\xeb\xac\x0f\x07\xc2\x3f\x45\x98";
  assert (memcmp (ciphertext_out, ciphertext_case_5, 60) == 0);
  assert (memcmp (auth_tag, "\x36\x12\xd2\xe7\x9e\x3b\x07\x85\x56\x1b\xe1\x4a\xac\xa2\xfc\xcb", 16) == 0);
  if (!dsk_aead_gcm_decrypt (&precompute,
                        60, ciphertext_out,              // ciphertext
                        20, assoc_data_case_5,
                        8, iv_case_5,
                        plaintext_out,
                        16, auth_tag))
    assert(false);
  assert (memcmp (plaintext_out, plaintext_case_5, 60) == 0);

  //
  // Test Case 6
  //
  // key is same as test 4, 5
  const uint8_t *plaintext_case_6 = plaintext_case_5;
  const uint8_t *assoc_data_case_6 = assoc_data_case_5;
  const uint8_t *iv_case_6 = (uint8_t *)
                             "\x93\x13\x22\x5d\xf8\x84\x06\xe5\x55\x90\x9c\x5a\xff\x52\x69\xaa"
                             "\x6a\x7a\x95\x38\x53\x4f\x7d\xa1\xe4\xc3\x03\xd2\xa3\x18\xa7\x28"
                             "\xc3\xc0\xc9\x51\x56\x80\x95\x39\xfc\xf0\xe2\x42\x9a\x6b\x52\x54"
                             "\x16\xae\xdb\xf5\xa0\xde\x6a\x57\xa6\x37\xb3\x9b";
  dsk_aead_gcm_encrypt (&precompute,
                        60, plaintext_case_6,
                        20, assoc_data_case_6,
                        60, iv_case_6,
                        ciphertext_out,
                        16, auth_tag);
  const uint8_t *ciphertext_case_6 = (uint8_t *)
                                     "\x8c\xe2\x49\x98\x62\x56\x15\xb6\x03\xa0\x33\xac\xa1\x3f\xb8\x94"
                                     "\xbe\x91\x12\xa5\xc3\xa2\x11\xa8\xba\x26\x2a\x3c\xca\x7e\x2c\xa7"
                                     "\x01\xe4\xa9\xa4\xfb\xa4\x3c\x90\xcc\xdc\xb2\x81\xd4\x8c\x7c\x6f"
                                     "\xd6\x28\x75\xd2\xac\xa4\x17\x03\x4c\x34\xae\xe5";
  assert (memcmp (ciphertext_out, ciphertext_case_6, 60) == 0);
  assert (memcmp (auth_tag, "\x61\x9c\xc5\xae\xff\xfe\x0b\xfa\x46\x2a\xf4\x3c\x16\x99\xd0\x50", 16) == 0);
  if (!dsk_aead_gcm_decrypt (&precompute,
                        60, ciphertext_out,              // ciphertext
                        20, assoc_data_case_6,
                        60, iv_case_6,
                        plaintext_out,
                        16, auth_tag))
    assert(false);
  assert (memcmp (plaintext_out, plaintext_case_6, 60) == 0);

  //
  // Test Case 7:   192-bit keys
  //
  DskAES192 aes192;
  dsk_aes192_init (&aes192, (uint8_t *) "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");
  dsk_aead_gcm_precompute ((DskBlockCipherInplaceFunc) dsk_aes192_encrypt_inplace,
                           &aes192,
                           &precompute);
  dsk_aead_gcm_encrypt (&precompute, 
                        0, (uint8_t *) "",
                        0, (uint8_t *) "",
                        12, (uint8_t *) "\0\0\0\0\0\0\0\0\0\0\0\0",
                        ciphertext_out,
                        16, auth_tag);
  assert (memcmp (auth_tag, (uint8_t *) "\xcd\x33\xb2\x8a\xc7\x73\xf7\x4b\xa0\x0e\xd1\xf3\x12\x57\x24\x35", 16) == 0);
  if (!dsk_aead_gcm_decrypt (&precompute,
                        0, (uint8_t *) "",              // ciphertext
                        0, (uint8_t *) "",
                        12, (uint8_t *) "\0\0\0\0\0\0\0\0\0\0\0\0",
                        plaintext_out,
                        16, auth_tag))
    assert(false);

  //
  // Test case 8:  same but with 16-nul plaintext
  dsk_aead_gcm_encrypt (&precompute, 
                        16, (uint8_t *) "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
                        0, (uint8_t *) "",
                        12, (uint8_t *) "\0\0\0\0\0\0\0\0\0\0\0\0",
                        ciphertext_out,
                        16, auth_tag);
  const uint8_t *ciphertext_case_8 = (uint8_t *)
                                     "\x98\xe7\x24\x7c\x07\xf0\xfe\x41\x1c\x26\x7e\x43\x84\xb0\xf6\x00";
  assert (memcmp (auth_tag, (uint8_t *) "\x2f\xf5\x8d\x80\x03\x39\x27\xab\x8e\xf4\xd4\x58\x75\x14\xf0\xfb", 16) == 0);
  assert (memcmp (ciphertext_out, ciphertext_case_8, 16) == 0);
  if (!dsk_aead_gcm_decrypt (&precompute,
                        16, ciphertext_out,
                        0, (uint8_t *) "",
                        12, (uint8_t *) "\0\0\0\0\0\0\0\0\0\0\0\0",
                        plaintext_out,
                        16, auth_tag))
    assert(false);
  assert(memcmp(plaintext_out, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 16) == 0);

  //
  // Test case 9: typical 192-bit invocation; no associated data
  //
  const uint8_t *plaintext_case_9 = (uint8_t *) 
                                    "\xd9\x31\x32\x25\xf8\x84\x06\xe5\xa5\x59\x09\xc5\xaf\xf5\x26\x9a"
                                    "\x86\xa7\xa9\x53\x15\x34\xf7\xda\x2e\x4c\x30\x3d\x8a\x31\x8a\x72"
                                    "\x1c\x3c\x0c\x95\x95\x68\x09\x53\x2f\xcf\x0e\x24\x49\xa6\xb5\x25"
                                    "\xb1\x6a\xed\xf5\xaa\x0d\xe6\x57\xba\x63\x7b\x39\x1a\xaf\xd2\x55";
  const uint8_t *key_case_9 = (uint8_t *)
                                    "\xfe\xff\xe9\x92\x86\x65\x73\x1c\x6d\x6a\x8f\x94\x67\x30\x83\x08"
                                    "\xfe\xff\xe9\x92\x86\x65\x73\x1c";
  const uint8_t *iv_case_9 = iv_case_4;

  dsk_aes192_init (&aes192, key_case_9);
  dsk_aead_gcm_precompute ((DskBlockCipherInplaceFunc) dsk_aes192_encrypt_inplace,
                           &aes192, &precompute);
  dsk_aead_gcm_encrypt (&precompute, 
                        64, plaintext_case_9,
                        0, (uint8_t *) "",
                        12, iv_case_9,
                        ciphertext_out,
                        16, auth_tag);
  const uint8_t *ciphertext_case_9 = (uint8_t *)
                                     "\x39\x80\xca\x0b\x3c\x00\xe8\x41\xeb\x06\xfa\xc4\x87\x2a\x27\x57"
                                     "\x85\x9e\x1c\xea\xa6\xef\xd9\x84\x62\x85\x93\xb4\x0c\xa1\xe1\x9c"
                                     "\x7d\x77\x3d\x00\xc1\x44\xc5\x25\xac\x61\x9d\x18\xc8\x4a\x3f\x47"
                                     "\x18\xe2\x44\x8b\x2f\xe3\x24\xd9\xcc\xda\x27\x10\xac\xad\xe2\x56";
  assert (memcmp (ciphertext_out, ciphertext_case_9, 64) == 0);
  assert (memcmp (auth_tag, (uint8_t *) "\x99\x24\xa7\xc8\x58\x73\x36\xbf\xb1\x18\x02\x4d\xb8\x67\x4a\x14", 16) == 0);
  if (!dsk_aead_gcm_decrypt (&precompute,
                        64, ciphertext_out,
                        0, (uint8_t *) "",
                        12, iv_case_9,
                        plaintext_out,
                        16, auth_tag))
    assert(false);
  assert(memcmp(plaintext_out, plaintext_case_9, 64) == 0);

  //
  // Test case 10: oddly sized plaintext, assoc data
  //
  const uint8_t *plaintext_case_10 = plaintext_case_4;
  const uint8_t *assoc_data_case_10 = assoc_data_case_4;
  const uint8_t *iv_case_10 = iv_case_4;
  dsk_aead_gcm_encrypt (&precompute, 
                        60, plaintext_case_10,
                        20, assoc_data_case_10,
                        12, iv_case_10,
                        ciphertext_out,
                        16, auth_tag);

  const uint8_t *ciphertext_case_10 = (uint8_t *)
                                      "\x39\x80\xca\x0b\x3c\x00\xe8\x41\xeb\x06\xfa\xc4\x87\x2a\x27\x57"
                                      "\x85\x9e\x1c\xea\xa6\xef\xd9\x84\x62\x85\x93\xb4\x0c\xa1\xe1\x9c"
                                      "\x7d\x77\x3d\x00\xc1\x44\xc5\x25\xac\x61\x9d\x18\xc8\x4a\x3f\x47"
                                      "\x18\xe2\x44\x8b\x2f\xe3\x24\xd9\xcc\xda\x27\x10";
  assert (memcmp (ciphertext_out, ciphertext_case_10, 60) == 0);
  assert (memcmp (auth_tag, "\x25\x19\x49\x8e\x80\xf1\x47\x8f\x37\xba\x55\xbd\x6d\x27\x61\x8c", 16) == 0);
  if (!dsk_aead_gcm_decrypt (&precompute,
                        60, ciphertext_out,
                        20, assoc_data_case_10,
                        12, iv_case_10,
                        plaintext_out,
                        16, auth_tag))
    assert(false);
  assert(memcmp(plaintext_out, plaintext_case_10, 60) == 0);

  //
  // Test case 11: same as 10 with short iv
  //
  const uint8_t *plaintext_case_11 = plaintext_case_10;
  const uint8_t *assoc_data_case_11 = assoc_data_case_10;
  const uint8_t *iv_case_11 = iv_case_5;
  dsk_aead_gcm_encrypt (&precompute, 
                        60, plaintext_case_11,
                        20, assoc_data_case_11,
                        8, iv_case_11,
                        ciphertext_out,
                        16, auth_tag);

  const uint8_t *ciphertext_case_11 = (uint8_t *)
                                      "\x0f\x10\xf5\x99\xae\x14\xa1\x54\xed\x24\xb3\x6e\x25\x32\x4d\xb8"
                                      "\xc5\x66\x63\x2e\xf2\xbb\xb3\x4f\x83\x47\x28\x0f\xc4\x50\x70\x57"
                                      "\xfd\xdc\x29\xdf\x9a\x47\x1f\x75\xc6\x65\x41\xd4\xd4\xda\xd1\xc9"
                                      "\xe9\x3a\x19\xa5\x8e\x8b\x47\x3f\xa0\xf0\x62\xf7";
  assert (memcmp (ciphertext_out, ciphertext_case_11, 60) == 0);
  assert (memcmp (auth_tag, "\x65\xdc\xc5\x7f\xcf\x62\x3a\x24\x09\x4f\xcc\xa4\x0d\x35\x33\xf8", 16) == 0);
  if (!dsk_aead_gcm_decrypt (&precompute,
                        60, ciphertext_out,
                        20, assoc_data_case_11,
                        8, iv_case_11,
                        plaintext_out,
                        16, auth_tag))
    assert(false);
  assert(memcmp(plaintext_out, plaintext_case_11, 60) == 0);

  //
  // Test case 12: same as 10,11 with long iv
  //
  const uint8_t *plaintext_case_12 = plaintext_case_10;
  const uint8_t *assoc_data_case_12 = assoc_data_case_10;
  const uint8_t *iv_case_12 = iv_case_6;
  dsk_aead_gcm_encrypt (&precompute, 
                        60, plaintext_case_12,
                        20, assoc_data_case_12,
                        60, iv_case_12,
                        ciphertext_out,
                        16, auth_tag);

  const uint8_t *ciphertext_case_12 = (uint8_t *)
                                      "\xd2\x7e\x88\x68\x1c\xe3\x24\x3c\x48\x30\x16\x5a\x8f\xdc\xf9\xff"
                                      "\x1d\xe9\xa1\xd8\xe6\xb4\x47\xef\x6e\xf7\xb7\x98\x28\x66\x6e\x45"
                                      "\x81\xe7\x90\x12\xaf\x34\xdd\xd9\xe2\xf0\x37\x58\x9b\x29\x2d\xb3"
                                      "\xe6\x7c\x03\x67\x45\xfa\x22\xe7\xe9\xb7\x37\x3b";
  assert (memcmp (ciphertext_out, ciphertext_case_12, 60) == 0);
  assert (memcmp (auth_tag, "\xdc\xf5\x66\xff\x29\x1c\x25\xbb\xb8\x56\x8f\xc3\xd3\x76\xa6\xd9", 16) == 0);
  if (!dsk_aead_gcm_decrypt (&precompute,
                        60, ciphertext_out,
                        20, assoc_data_case_12,
                        60, iv_case_12,
                        plaintext_out,
                        16, auth_tag))
    assert(false);
  assert(memcmp(plaintext_out, plaintext_case_12, 60) == 0);

  //
  // Test case 13: trivial 256-bit key
  //
  DskAES256 aes256;
  dsk_aes256_init (&aes256, (uint8_t*) "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");
  dsk_aead_gcm_precompute ((DskBlockCipherInplaceFunc) dsk_aes256_encrypt_inplace,
                           &aes256, &precompute);
  dsk_aead_gcm_encrypt (&precompute, 
                        0, (uint8_t *) "",
                        0, (uint8_t *) "",
                        12, (uint8_t *) "\0\0\0\0\0\0\0\0\0\0\0\0",
                        ciphertext_out,
                        16, auth_tag);
  assert (memcmp (auth_tag, "\x53\x0f\x8a\xfb\xc7\x45\x36\xb9\xa9\x63\xb4\xf1\xc4\xcb\x73\x8b", 16) == 0);
  if (!dsk_aead_gcm_decrypt (&precompute,
                        0, (uint8_t *) "",
                        0, (uint8_t *) "",
                        12, (uint8_t *) "\0\0\0\0\0\0\0\0\0\0\0\0",
                        plaintext_out,
                        16, auth_tag))
    assert(false);

  //
  // Test case 14: 0 plaintext
  //
  dsk_aead_gcm_encrypt (&precompute, 
                        16, (uint8_t *) "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
                        0, (uint8_t *) "",
                        12, (uint8_t *) "\0\0\0\0\0\0\0\0\0\0\0\0",
                        ciphertext_out,
                        16, auth_tag);
  const uint8_t *ciphertext_case_14 = (uint8_t *)
                                      "\xce\xa7\x40\x3d\x4d\x60\x6b\x6e\x07\x4e\xc5\xd3\xba\xf3\x9d\x18";
  assert (memcmp (ciphertext_out, ciphertext_case_14, 16) == 0);
  assert (memcmp (auth_tag, "\xd0\xd1\xc8\xa7\x99\x99\x6b\xf0\x26\x5b\x98\xb5\xd4\x8a\xb9\x19", 16) == 0);
  if (!dsk_aead_gcm_decrypt (&precompute,
                        16, ciphertext_out,
                        0, (uint8_t *) "",
                        12, (uint8_t *) "\0\0\0\0\0\0\0\0\0\0\0\0",
                        plaintext_out,
                        16, auth_tag))
    assert(false);
  assert (memcmp (plaintext_out, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 16) == 0);

  //
  // Test case 15: standard invocation
  //
  const uint8_t *key_case_15 = (uint8_t *)
                                    "\xfe\xff\xe9\x92\x86\x65\x73\x1c\x6d\x6a\x8f\x94\x67\x30\x83\x08"
                                    "\xfe\xff\xe9\x92\x86\x65\x73\x1c\x6d\x6a\x8f\x94\x67\x30\x83\x08";
  dsk_aes256_init (&aes256, key_case_15);
  dsk_aead_gcm_precompute ((DskBlockCipherInplaceFunc) dsk_aes256_encrypt_inplace,
                           &aes256, &precompute);
  const uint8_t *plaintext_case_15 = plaintext_case_3;
  const uint8_t *iv_case_15 = iv_case_3;
  dsk_aead_gcm_encrypt (&precompute, 
                        64, plaintext_case_15,
                        0, (uint8_t *) "",
                        12, iv_case_15,
                        ciphertext_out,
                        16, auth_tag);
  const uint8_t *ciphertext_case_15 = (uint8_t *)
                                      "\x52\x2d\xc1\xf0\x99\x56\x7d\x07\xf4\x7f\x37\xa3\x2a\x84\x42\x7d"
                                      "\x64\x3a\x8c\xdc\xbf\xe5\xc0\xc9\x75\x98\xa2\xbd\x25\x55\xd1\xaa"
                                      "\x8c\xb0\x8e\x48\x59\x0d\xbb\x3d\xa7\xb0\x8b\x10\x56\x82\x88\x38"
                                      "\xc5\xf6\x1e\x63\x93\xba\x7a\x0a\xbc\xc9\xf6\x62\x89\x80\x15\xad";
  assert (memcmp (ciphertext_out, ciphertext_case_15, 64) == 0);
  assert (memcmp (auth_tag, "\xb0\x94\xda\xc5\xd9\x34\x71\xbd\xec\x1a\x50\x22\x70\xe3\xcc\x6c", 16) == 0);
  if (!dsk_aead_gcm_decrypt (&precompute,
                        64, ciphertext_out,
                        0, (uint8_t *) "",
                        12, iv_case_15,
                        plaintext_out,
                        16, auth_tag))
    assert(false);
  assert (memcmp (plaintext_out, plaintext_case_15, 64) == 0);

  //
  // Test case 16.
  //
  const uint8_t *plaintext_case_16 = plaintext_case_4;
  const uint8_t *iv_case_16 = iv_case_4;
  const uint8_t *assoc_data_case_16 = assoc_data_case_4;
  dsk_aead_gcm_encrypt (&precompute, 
                        60, plaintext_case_16,
                        20, assoc_data_case_16,
                        12, iv_case_16,
                        ciphertext_out,
                        16, auth_tag);
  const uint8_t *ciphertext_case_16 = (uint8_t *)
                                      "\x52\x2d\xc1\xf0\x99\x56\x7d\x07\xf4\x7f\x37\xa3\x2a\x84\x42\x7d"
                                      "\x64\x3a\x8c\xdc\xbf\xe5\xc0\xc9\x75\x98\xa2\xbd\x25\x55\xd1\xaa"
                                      "\x8c\xb0\x8e\x48\x59\x0d\xbb\x3d\xa7\xb0\x8b\x10\x56\x82\x88\x38"
                                      "\xc5\xf6\x1e\x63\x93\xba\x7a\x0a\xbc\xc9\xf6\x62";
  assert (memcmp (ciphertext_out, ciphertext_case_16, 60) == 0);
  assert (memcmp (auth_tag, "\x76\xfc\x6e\xce\x0f\x4e\x17\x68\xcd\xdf\x88\x53\xbb\x2d\x55\x1b", 16) == 0);
  if (!dsk_aead_gcm_decrypt (&precompute,
                        60, ciphertext_out,
                        20, assoc_data_case_16,
                        12, iv_case_16,
                        plaintext_out,
                        16, auth_tag))
    assert(false);
  assert (memcmp (plaintext_out, plaintext_case_16, 60) == 0);

  //
  // Test case 17.  short iv.
  //
  const uint8_t *plaintext_case_17 = plaintext_case_16;
  const uint8_t *iv_case_17 = iv_case_5;
  const uint8_t *assoc_data_case_17 = assoc_data_case_16;
  dsk_aead_gcm_encrypt (&precompute, 
                        60, plaintext_case_17,
                        20, assoc_data_case_17,
                        8, iv_case_17,
                        ciphertext_out,
                        16, auth_tag);
  const uint8_t *ciphertext_case_17 = (uint8_t *)
                                      "\xc3\x76\x2d\xf1\xca\x78\x7d\x32\xae\x47\xc1\x3b\xf1\x98\x44\xcb"
                                      "\xaf\x1a\xe1\x4d\x0b\x97\x6a\xfa\xc5\x2f\xf7\xd7\x9b\xba\x9d\xe0"
                                      "\xfe\xb5\x82\xd3\x39\x34\xa4\xf0\x95\x4c\xc2\x36\x3b\xc7\x3f\x78"
                                      "\x62\xac\x43\x0e\x64\xab\xe4\x99\xf4\x7c\x9b\x1f";
  assert (memcmp (ciphertext_out, ciphertext_case_17, 60) == 0);
  assert (memcmp (auth_tag, "\x3a\x33\x7d\xbf\x46\xa7\x92\xc4\x5e\x45\x49\x13\xfe\x2e\xa8\xf2" , 16) == 0);
  if (!dsk_aead_gcm_decrypt (&precompute,
                        60, ciphertext_out,
                        20, assoc_data_case_17,
                        8, iv_case_17,
                        plaintext_out,
                        16, auth_tag))
    assert(false);
  assert (memcmp (plaintext_out, plaintext_case_17, 60) == 0);

  //
  // Test case 18: long iv.
  //
  const uint8_t *plaintext_case_18 = plaintext_case_17;
  const uint8_t *iv_case_18 = iv_case_6;
  const uint8_t *assoc_data_case_18 = assoc_data_case_17;
  dsk_aead_gcm_encrypt (&precompute, 
                        60, plaintext_case_18,
                        20, assoc_data_case_18,
                        60, iv_case_18,
                        ciphertext_out,
                        16, auth_tag);
  const uint8_t *ciphertext_case_18 = (uint8_t *)
                                      "\x5a\x8d\xef\x2f\x0c\x9e\x53\xf1\xf7\x5d\x78\x53\x65\x9e\x2a\x20"
                                      "\xee\xb2\xb2\x2a\xaf\xde\x64\x19\xa0\x58\xab\x4f\x6f\x74\x6b\xf4"
                                      "\x0f\xc0\xc3\xb7\x80\xf2\x44\x45\x2d\xa3\xeb\xf1\xc5\xd8\x2c\xde"
                                      "\xa2\x41\x89\x97\x20\x0e\xf8\x2e\x44\xae\x7e\x3f";
  assert (memcmp (ciphertext_out, ciphertext_case_18, 60) == 0);
  assert (memcmp (auth_tag, "\xa4\x4a\x82\x66\xee\x1c\x8e\xb0\xc8\xb5\xd4\xcf\x5a\xe9\xf1\x9a" , 16) == 0);
  if (!dsk_aead_gcm_decrypt (&precompute,
                        60, ciphertext_out,
                        20, assoc_data_case_18,
                        60, iv_case_18,
                        plaintext_out,
                        16, auth_tag))
    assert(false);
  assert (memcmp (plaintext_out, plaintext_case_18, 60) == 0);


}

// From Appendix C of 
// https://nvlpubs.nist.gov/nistpubs/Legacy/SP/nistspecialpublication800-38c.pdf
static void
test_aes_ccm (void)
{
  // All examples use the same key.
  DskAES128 aes128;
  dsk_aes128_init (&aes128, (uint8_t *) "\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f");

  //
  // Example 1.
  //
  uint8_t *iv_ex1 = (uint8_t *) "\x10\x11\x12\x13\x14\x15\x16";
  uint8_t *assoc_data_ex1 = (uint8_t *) "\x00\x01\x02\x03\x04\x05\x06\x07";
  uint8_t *plaintext_ex1 = (uint8_t *) "\x20\x21\x22\x23";
  uint8_t ciphertext[64], plaintext[64];
  uint8_t auth_tag[16];
  dsk_aead_ccm_encrypt ((DskBlockCipher128InplaceFunc) dsk_aes128_encrypt_inplace, &aes128,
                          4, plaintext_ex1,
                          8, assoc_data_ex1,
                          7, iv_ex1,
                          ciphertext,
                          4, auth_tag);
  assert (memcmp (ciphertext, "\x71\x62\x01\x5b", 4) == 0);
  assert (memcmp (auth_tag, "\x4d\xac\x25\x5d", 4) == 0);

  if (!dsk_aead_ccm_decrypt ((DskBlockCipher128InplaceFunc) dsk_aes128_encrypt_inplace, &aes128,
                             4, ciphertext,
                             8, assoc_data_ex1,
                             7, iv_ex1,
                             plaintext, 
                             4, auth_tag))
    assert (false);
  assert (memcmp (plaintext, plaintext_ex1, 4) == 0);

  //
  // Example 2.
  //
  uint8_t *iv_ex2 = (uint8_t *) "\x10\x11\x12\x13\x14\x15\x16\x17";
  uint8_t *assoc_data_ex2 = (uint8_t *)"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f";
  uint8_t *plaintext_ex2 = (uint8_t *)"\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f";
  dsk_aead_ccm_encrypt ((DskBlockCipher128InplaceFunc) dsk_aes128_encrypt_inplace, &aes128,
                          16, plaintext_ex2,
                          16, assoc_data_ex2,
                          8, iv_ex2,
                          ciphertext,
                          6, auth_tag);
  assert (memcmp (ciphertext, "\xd2\xa1\xf0\xe0\x51\xea\x5f\x62\x08\x1a\x77\x92\x07\x3d\x59\x3d", 16) == 0);
  assert (memcmp (auth_tag, "\x1f\xc6\x4f\xbf\xac\xcd", 6) == 0);
  if (!dsk_aead_ccm_decrypt ((DskBlockCipher128InplaceFunc) dsk_aes128_encrypt_inplace, &aes128,
                             16, ciphertext,
                             16, assoc_data_ex2,
                             8, iv_ex2,
                             plaintext, 
                             6, auth_tag))
    assert (false);
  assert (memcmp (plaintext, plaintext_ex2, 16) == 0);

  //
  // Example 3.
  //
  uint8_t *iv_ex3 = (uint8_t *) "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b";
  uint8_t *assoc_data_ex3 = (uint8_t *)"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
                                        "\x10\x11\x12\x13";
  uint8_t *plaintext_ex3 = (uint8_t *)"\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f"
                                      "\x30\x31\x32\x33\x34\x35\x36\x37";
  dsk_aead_ccm_encrypt ((DskBlockCipher128InplaceFunc) dsk_aes128_encrypt_inplace, &aes128,
                          24, plaintext_ex3,
                          20, assoc_data_ex3,
                          12, iv_ex3,
                          ciphertext,
                          8, auth_tag);
  assert (memcmp (ciphertext, "\xe3\xb2\x01\xa9\xf5\xb7\x1a\x7a"
                              "\x9b\x1c\xea\xec\xcd\x97\xe7\x0b"
                              "\x61\x76\xaa\xd9\xa4\x42\x8a\xa5", 24) == 0);
  assert (memcmp (auth_tag, "\x48\x43\x92\xfb\xc1\xb0\x99\x51", 8) == 0);
  if (!dsk_aead_ccm_decrypt ((DskBlockCipher128InplaceFunc) dsk_aes128_encrypt_inplace, &aes128,
                             24, ciphertext,
                             20, assoc_data_ex3,
                             12, iv_ex3,
                             plaintext, 
                             8, auth_tag))
    assert (false);
  assert (memcmp (plaintext, plaintext_ex3, 24) == 0);

  //
  // Example 4.
  //
  uint8_t *iv_ex4 = (uint8_t *) "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c";
  size_t assoc_data_ex4_len = 524288 / 8;
  uint8_t *assoc_data_ex4 = malloc (assoc_data_ex4_len);
  for (unsigned i = 0; i < assoc_data_ex4_len; i++)
    assoc_data_ex4[i] = i & 0xff;
  uint8_t *plaintext_ex4 = (uint8_t *)"\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f"
                                      "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f";
  dsk_aead_ccm_encrypt ((DskBlockCipher128InplaceFunc) dsk_aes128_encrypt_inplace, &aes128,
                          32, plaintext_ex4,
                          assoc_data_ex4_len, assoc_data_ex4,
                          13, iv_ex4,
                          ciphertext,
                          14, auth_tag);
  assert (memcmp (ciphertext, "\x69\x91\x5d\xad\x1e\x84\xc6\x37"
                              "\x6a\x68\xc2\x96\x7e\x4d\xab\x61"
                              "\x5a\xe0\xfd\x1f\xae\xc4\x4c\xc4"
                              "\x84\x82\x85\x29\x46\x3c\xcf\x72", 32) == 0);
  assert (memcmp (auth_tag, "\xb4\xac\x6b\xec\x93\xe8\x59\x8e\x7f\x0d\xad\xbc\xea\x5b", 14) == 0);
  if (!dsk_aead_ccm_decrypt ((DskBlockCipher128InplaceFunc) dsk_aes128_encrypt_inplace, &aes128,
                             32, ciphertext,
                             assoc_data_ex4_len, assoc_data_ex4,
                             13, iv_ex4,
                             plaintext, 
                             14, auth_tag))
    assert (false);
  assert (memcmp (plaintext, plaintext_ex4, 32) == 0);
  free (assoc_data_ex4);
}

static void
test_curve25519 (void)
{
  uint8_t alice_private[32], alice_public[32], alice_shared[32];
  uint8_t bob_private[32], bob_public[32], bob_shared[32];
  DskRand *rng = dsk_rand_new_xorshift1024();
  dsk_rand_seed (rng);
  for (unsigned i = 0; i < 32; i++)
    {
      alice_private[i] = dsk_rand_int_range(rng, 0,256);
      bob_private[i] = dsk_rand_int_range(rng,0,256);
    }
  dsk_curve25519_random_to_private (alice_private);
  dsk_curve25519_random_to_private (bob_private);
  dsk_curve25519_private_to_public (alice_private, alice_public);
  dsk_curve25519_private_to_public (bob_private, bob_public);
  dsk_curve25519_private_to_shared (alice_private, bob_public, alice_shared);
  dsk_curve25519_private_to_shared (bob_private, alice_public, bob_shared);
  assert (memcmp (alice_shared, bob_shared, 32) == 0);
}

// Appendix from rfc2104
static void
test_hmac (void)
{
  uint8_t digest[16];
  dsk_hmac_digest (DSK_CHECKSUM_MD5,
                   16, (uint8_t *) "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b",
                   8, (uint8_t *) "Hi There",
                   digest);
  assert (memcmp (digest, "\x92\x94\x72\x7a\x36\x38\xbb\x1c\x13\xf4\x8e\xf8\x15\x8b\xfc\x9d", 16) == 0);


  dsk_hmac_digest (DSK_CHECKSUM_MD5,
                   4, (uint8_t *) "Jefe",
                   28, (uint8_t *) "what do ya want for nothing?",
                   digest);
  assert (memcmp (digest, "\x75\x0c\x78\x3e\x6a\xb0\xb5\x03\xea\xa8\x6e\x31\x0a\x5d\xb7\x38", 16) == 0);

  dsk_hmac_digest (DSK_CHECKSUM_MD5,
                   16, (uint8_t *) "\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa",
                   50, (uint8_t *)
                   "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
                   "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
                   "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
                   "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD"
                   "\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD\xDD",
                   digest);
  assert (memcmp (digest, "\x56\xbe\x34\x52\x1d\x14\x4c\x88\xdb\xb8\xc7\x33\xf0\xe8\xb3\xf6", 16) == 0);
} 

static void
test_hmac_rfc4231 (void)
{
  static struct TestCase_HMAC_RFC4231 {
    const char *name;
    size_t key_len;
    const uint8_t *key;
    size_t data_len;
    const uint8_t *data;
    const uint8_t *hmac_sha256;
  } test_cases[] = {
    {
      "Test Case #1",
      20,
      (uint8_t *) "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b"
                  "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b",
      8,
      (uint8_t *) "Hi There",
      (uint8_t *) "\xb0\x34\x4c\x61\xd8\xdb\x38\x53\x5c\xa8\xaf\xce\xaf\x0b\xf1\x2b"
                  "\x88\x1d\xc2\x00\xc9\x83\x3d\xa7\x26\xe9\x37\x6c\x2e\x32\xcf\xf7"
    },
    {
      "Test Case #2",
      4,
      (uint8_t *) "Jefe",
      28,
      (uint8_t *) "what do ya want for nothing?",
      (uint8_t *) "\x5b\xdc\xc1\x46\xbf\x60\x75\x4e\x6a\x04\x24\x26\x08\x95\x75\xc7"
                  "\x5a\x00\x3f\x08\x9d\x27\x39\x83\x9d\xec\x58\xb9\x64\xec\x38\x43"
    },
    {
      "Test Case #3",
      20,
      (uint8_t *) "\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
                  "\xaa\xaa\xaa\xaa",
      50,
      (uint8_t *) "\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
                  "\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
                  "\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd"
                  "\xdd\xdd",
      (uint8_t *) "\x77\x3e\xa9\x1e\x36\x80\x0e\x46\x85\x4d\xb8\xeb\xd0\x91\x81\xa7"
                  "\x29\x59\x09\x8b\x3e\xf8\xc1\x22\xd9\x63\x55\x14\xce\xd5\x65\xfe"
    },
    {
      "Test Case #4",
      25,
      (uint8_t *) "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10"
                  "\x11\x12\x13\x14\x15\x16\x17\x18\x19",
      50,
      (uint8_t *) "\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd"
                  "\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd"
                  "\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd\xcd"
                  "\xcd\xcd",
      (uint8_t *) "\x82\x55\x8a\x38\x9a\x44\x3c\x0e\xa4\xcc\x81\x98\x99\xf2\x08\x3a"
                  "\x85\xf0\xfa\xa3\xe5\x78\xf8\x07\x7a\x2e\x3f\xf4\x67\x29\x66\x5b"
    },
  #if 0
    {
      "Test Case #5",
      20,
      (uint8_t *) "\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c\x0c"
                  "\x0c\x0c\x0c\x0c",
      20,
      (uint8_t *) "Test With Truncation",
      (uint8_t *) "\xa3\xb6\x16\x74\x73\x10\x0e\xe0\x6e\x0c\x79\x6c\x29\x55\x55\x2b",
    },
  #endif
    {
      "Test Case #6",
      131,
      (uint8_t *) "\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
                  "\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
                  "\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
                  "\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
                  "\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
                  "\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
                  "\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
                  "\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
                  "\xaa\xaa\xaa",
      54,
      (uint8_t *) "Test Using Larger Than Block-Size Key - Hash Key First",
      (uint8_t *) "\x60\xe4\x31\x59\x1e\xe0\xb6\x7f\x0d\x8a\x26\xaa\xcb\xf5\xb7\x7f"
                  "\x8e\x0b\xc6\x21\x37\x28\xc5\x14\x05\x46\x04\x0f\x0e\xe3\x7f\x54"
    },
    {
      "Test Case #7",
      131,
      (uint8_t *) "\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
                  "\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
                  "\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
                  "\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
                  "\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
                  "\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
                  "\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
                  "\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa"
                  "\xaa\xaa\xaa",
       
      152,
      (uint8_t *) "This is a test using a larger than block-size key and a larger than block-size data. The key needs to be hashed before being used by the HMAC algorithm.",
      (uint8_t *) "\x9b\x09\xff\xa7\x1b\x94\x2f\xcb\x27\x63\x5f\xbc\xd5\xb0\xe9\x44"
                  "\xbf\xdc\x63\x64\x4f\x07\x13\x93\x8a\x7f\x51\x53\x5c\x3a\x35\xe2"
    }
  };

  for (unsigned i = 0; i < DSK_N_ELEMENTS (test_cases); i++)
    {
       uint8_t hmac_sha256[32];
       dsk_hmac_digest (DSK_CHECKSUM_SHA256, 
                        test_cases[i].key_len,
                        test_cases[i].key,
                        test_cases[i].data_len,
                        test_cases[i].data,
                        hmac_sha256);
#if 0
       fprintf(stderr, "\n\nRFC 4231, %s\n", test_cases[i].name);
       fprintf(stderr, "Key:   ");
       for (unsigned j = 0; j < test_cases[i].key_len; j++)
         fprintf(stderr, "%c%02x", (j != 0 && j % 16 == 0) ? '\n' : ' ', test_cases[i].key[j]);
       fprintf(stderr, "\nData:  ");
       for (unsigned j = 0; j < test_cases[i].data_len; j++)
         fprintf(stderr, "%c%02x", (j != 0 && j % 16 == 0) ? '\n' : ' ', test_cases[i].data[j]);
       fprintf(stderr, "\nExpected SHA-256:  ");
       for (unsigned j = 0; j < 32; j++)
         fprintf(stderr, "%c%02x", (j != 0 && j % 16 == 0) ? '\n' : ' ', test_cases[i].hmac_sha256[j]);
       fprintf(stderr, "\nActual SHA-256:  ");
       for (unsigned j = 0; j < 32; j++)
         fprintf(stderr, "%c%02x", (j != 0 && j % 16 == 0) ? '\n' : ' ', hmac_sha256[j]);
#endif
       assert (memcmp (test_cases[i].hmac_sha256, hmac_sha256, 32) == 0);
     }
}
static void
test_hkdf (void)
{
  uint8_t prk[32], okm[100];

  // Test Case 1.
  dsk_hkdf_extract (DSK_CHECKSUM_SHA256,
                    13, (uint8_t *) "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c",
                    22, (uint8_t *) "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b",
                    prk);
  assert (memcmp (prk, "\x07\x77\x09\x36\x2c\x2e\x32\xdf\x0d\xdc\x3f\x0d\xc4\x7b\xba\x63"
                       "\x90\xb6\xc7\x3b\xb5\x0f\x9c\x31\x22\xec\x84\x4a\xd7\xc2\xb3\xe5", 32) == 0);
  dsk_hkdf_expand (DSK_CHECKSUM_SHA256, prk,
                   10, (uint8_t *) "\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9",
                   42, okm);
  assert (memcmp (okm, "\x3c\xb2\x5f\x25\xfa\xac\xd5\x7a\x90\x43\x4f\x64\xd0\x36\x2f\x2a"
                       "\x2d\x2d\x0a\x90\xcf\x1a\x5a\x4c\x5d\xb0\x2d\x56\xec\xc4\xc5\xbf"
                       "\x34\x00\x72\x08\xd5\xb8\x87\x18\x58\x65", 42) == 0);

  // Test Case 2.
  dsk_hkdf_extract (DSK_CHECKSUM_SHA256,
                    80,
                    (uint8_t *)
                    "\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f"
                    "\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b\x7c\x7d\x7e\x7f"
                    "\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f"
                    "\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f"
                    "\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf",
                    80,
                    (uint8_t *)
                    "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f"
                    "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f"
                    "\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f"
                    "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f"
                    "\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f",
                    prk);
  assert (memcmp (prk, "\x06\xa6\xb8\x8c\x58\x53\x36\x1a\x06\x10\x4c\x9c\xeb\x35\xb4\x5c"
                       "\xef\x76\x00\x14\x90\x46\x71\x01\x4a\x19\x3f\x40\xc1\x5f\xc2\x44", 32) == 0);
  dsk_hkdf_expand (DSK_CHECKSUM_SHA256,
                   prk,
                   80,
                   (uint8_t *)
                   "\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf"
                   "\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf"
                   "\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf"
                   "\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef"
                   "\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff",
                   82,
                   okm);
  assert (memcmp (okm,
                  "\xb1\x1e\x39\x8d\xc8\x03\x27\xa1\xc8\xe7\xf7\x8c\x59\x6a\x49\x34"
                  "\x4f\x01\x2e\xda\x2d\x4e\xfa\xd8\xa0\x50\xcc\x4c\x19\xaf\xa9\x7c"
                  "\x59\x04\x5a\x99\xca\xc7\x82\x72\x71\xcb\x41\xc6\x5e\x59\x0e\x09"
                  "\xda\x32\x75\x60\x0c\x2f\x09\xb8\x36\x77\x93\xa9\xac\xa3\xdb\x71"
                  "\xcc\x30\xc5\x81\x79\xec\x3e\x87\xc1\x4c\x01\xd5\xc1\xf3\x43\x4f"
                  "\x1d\x87", 82) == 0);

}

static struct 
{
  const char *name;
  void (*test)(void);
} tests[] =
{
  { "AES 128 Tests (from FIPS Appendix)", test_aes128 },
  { "AES 192 Tests (from FIPS Appendix)", test_aes192 },
  { "AES 256 Tests (from FIPS Appendix)", test_aes256 },
  { "AES Tests (from NIST 800-38a Appendix F)", test_aes_from_ecb_examples },
  { "AES GCM Tests (McGrew & Viega)", test_aes_gcm  },
  { "AES CCM Tests (from FIPS Appendix)", test_aes_ccm  },
  { "Curve 25519", test_curve25519 },
  { "HMAC Tests (from RFC 2104 Appendix)", test_hmac },
  { "HMAC Tests (SHA-256, from RFC 4231)", test_hmac_rfc4231 },
  { "HKDF Tests (from RFC 5869 Appendix A)", test_hkdf },
};


int main(int argc, char **argv)
{
  unsigned i;

  dsk_cmdline_init ("test various crypto routines",
                    "Test Cryptographic functions",
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
