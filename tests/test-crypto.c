#include "../dsk.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static bool cmdline_verbose = false;

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
  DskRand *rng = dsk_rand_new (&dsk_rand_type_xorshift1024);
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

  // From RFC 7748, Section 6.1.
  uint8_t pub[32];
  dsk_curve25519_private_to_public(
    (uint8_t *) "\x77\x07\x6d\x0a\x73\x18\xa5\x7d\x3c\x16\xc1\x72\x51\xb2\x66\x45\xdf\x4c\x2f\x87\xeb\xc0\x99\x2a\xb1\x77\xfb\xa5\x1d\xb9\x2c\x2a",
    pub);
  assert(memcmp (pub, "\x85\x20\xf0\x09\x89\x30\xa7\x54\x74\x8b\x7d\xdc\xb4\x3e\xf7\x5a\x0d\xbf\x3a\x0d\x26\x38\x1a\xf4\xeb\xa4\xa9\x8e\xaa\x9b\x4e\x6a", 32) == 0);
  uint8_t shared[32];
  dsk_curve25519_private_to_shared(
    (uint8_t *)"\x77\x07\x6d\x0a\x73\x18\xa5\x7d\x3c\x16\xc1\x72\x51\xb2\x66\x45\xdf\x4c\x2f\x87\xeb\xc0\x99\x2a\xb1\x77\xfb\xa5\x1d\xb9\x2c\x2a",
    (uint8_t *)"\xde\x9e\xdb\x7d\x7b\x7d\xc1\xb4\xd3\x5b\x61\xc2\xec\xe4\x35\x37\x3f\x83\x43\xc8\x5b\x78\x67\x4d\xad\xfc\x7e\x14\x6f\x88\x2b\x4f",
    shared);
  assert(memcmp (shared, "\x4a\x5d\x9d\x5b\xa4\xce\x2d\xe1\x72\x8e\x3b\xf4\x80\x35\x0f\x25\xe0\x7e\x21\xc9\x47\xd1\x9e\x33\x76\xf0\x9b\x3c\x1e\x16\x17\x42", 32) == 0);
}
static void
test_curve448 (void)
{
  // From RFC 7748, Section 5.2, Page 11.
  uint8_t out[56];
  uint8_t priv[56];
  memcpy (priv,  "\x3d\x26\x2f\xdd\xf9\xec\x8e\x88\x49\x52\x66\xfe\xa1\x9a"
                 "\x34\xd2\x88\x82\xac\xef\x04\x51\x04\xd0\xd1\xaa\xe1\x21"
                 "\x70\x0a\x77\x9c\x98\x4c\x24\xf8\xcd\xd7\x8f\xbf\xf4\x49"
                 "\x43\xeb\xa3\x68\xf5\x4b\x29\x25\x9a\x4f\x1c\x60\x0a\xd3", 56);
  dsk_curve448_random_to_private (priv);
  dsk_curve448_private_to_shared (
     priv,
     (uint8_t *) "\x06\xfc\xe6\x40\xfa\x34\x87\xbf\xda\x5f\x6c\xf2\xd5\x26"
                 "\x3f\x8a\xad\x88\x33\x4c\xbd\x07\x43\x7f\x02\x0f\x08\xf9"
                 "\x81\x4d\xc0\x31\xdd\xbd\xc3\x8c\x19\xc6\xda\x25\x83\xfa"
                 "\x54\x29\xdb\x94\xad\xa1\x8a\xa7\xa7\xfb\x4e\xf8\xa0\x86",
     out
  );
  assert (memcmp (out,
                  "\xce\x3e\x4f\xf9\x5a\x60\xdc\x66\x97\xda\x1d\xb1\xd8\x5e"
                  "\x6a\xfb\xdf\x79\xb5\x0a\x24\x12\xd7\x54\x6d\x5f\x23\x9f"
                  "\xe1\x4f\xba\xad\xeb\x44\x5f\xc6\x6a\x01\xb0\x77\x9d\x98"
                  "\x22\x39\x61\x11\x1e\x21\x76\x62\x82\xf7\x3d\xd9\x6b\x6f",
                  56) == 0);

  memcpy (priv, "\x20\x3d\x49\x44\x28\xb8\x39\x93\x52\x66\x5d\xdc\xa4\x2f"
                "\x9d\xe8\xfe\xf6\x00\x90\x8e\x0d\x46\x1c\xb0\x21\xf8\xc5"
                "\x38\x34\x5d\xd7\x7c\x3e\x48\x06\xe2\x5f\x46\xd3\x31\x5c"
                "\x44\xe0\xa5\xb4\x37\x12\x82\xdd\x2c\x8d\x5b\xe3\x09\x5f", 56);
  dsk_curve448_random_to_private (priv);
  dsk_curve448_private_to_shared (
     priv,
     (uint8_t *) "\x0f\xbc\xc2\xf9\x93\xcd\x56\xd3\x30\x5b\x0b\x7d\x9e\x55"
                 "\xd4\xc1\xa8\xfb\x5d\xbb\x52\xf8\xe9\xa1\xe9\xb6\x20\x1b"
                 "\x16\x5d\x01\x58\x94\xe5\x6c\x4d\x35\x70\xbe\xe5\x2f\xe2"
                 "\x05\xe2\x8a\x78\xb9\x1c\xdf\xbd\xe7\x1c\xe8\xd1\x57\xdb",
     out
  );
  assert(memcmp(out,
                "\x88\x4a\x02\x57\x62\x39\xff\x7a\x2f\x2f\x63\xb2\xdb\x6a"
                "\x9f\xf3\x70\x47\xac\x13\x56\x8e\x1e\x30\xfe\x63\xc4\xa7"
                "\xad\x1b\x3e\xe3\xa5\x70\x0d\xf3\x43\x21\xd6\x20\x77\xe6"
                "\x36\x33\xc5\x75\xc1\xc9\x54\x51\x4e\x99\xda\x7c\x17\x9d",
                56) == 0);

  const uint8_t *alice_private_unmasked =
          (uint8_t *) "\x9a\x8f\x49\x25\xd1\x51\x9f\x57\x75\xcf\x46\xb0\x4b\x58"
                      "\x00\xd4\xee\x9e\xe8\xba\xe8\xbc\x55\x65\xd4\x98\xc2\x8d"
                      "\xd9\xc9\xba\xf5\x74\xa9\x41\x97\x44\x89\x73\x91\x00\x63"
                      "\x82\xa6\xf1\x27\xab\x1d\x9a\xc2\xd8\xc0\xa5\x98\x72\x6b";
  uint8_t alice_private[56];
  const uint8_t *alice_public =
          (uint8_t *) "\x9b\x08\xf7\xcc\x31\xb7\xe3\xe6\x7d\x22\xd5\xae\xa1\x21"
                      "\x07\x4a\x27\x3b\xd2\xb8\x3d\xe0\x9c\x63\xfa\xa7\x3d\x2c"
                      "\x22\xc5\xd9\xbb\xc8\x36\x64\x72\x41\xd9\x53\xd4\x0c\x5b"
                      "\x12\xda\x88\x12\x0d\x53\x17\x7f\x80\xe5\x32\xc4\x1f\xa0";

  uint8_t test[56];
  memcpy (alice_private, alice_private_unmasked, 56);
  dsk_curve448_random_to_private (alice_private);
  dsk_curve448_private_to_public (alice_private, test);
  assert(memcmp (test, alice_public, 56) == 0);
  const uint8_t *bob_private_unmasked =
          (uint8_t *) "\x1c\x30\x6a\x7a\xc2\xa0\xe2\xe0\x99\x0b\x29\x44\x70\xcb"
                      "\xa3\x39\xe6\x45\x37\x72\xb0\x75\x81\x1d\x8f\xad\x0d\x1d"
                      "\x69\x27\xc1\x20\xbb\x5e\xe8\x97\x2b\x0d\x3e\x21\x37\x4c"
                      "\x9c\x92\x1b\x09\xd1\xb0\x36\x6f\x10\xb6\x51\x73\x99\x2d";
  uint8_t bob_private[56];
  const uint8_t *bob_public =
          (uint8_t *) "\x3e\xb7\xa8\x29\xb0\xcd\x20\xf5\xbc\xfc\x0b\x59\x9b\x6f"
                      "\xec\xcf\x6d\xa4\x62\x71\x07\xbd\xb0\xd4\xf3\x45\xb4\x30"
                      "\x27\xd8\xb9\x72\xfc\x3e\x34\xfb\x42\x32\xa1\x3c\xa7\x06"
                      "\xdc\xb5\x7a\xec\x3d\xae\x07\xbd\xc1\xc6\x7b\xf3\x36\x09";
  memcpy (bob_private, bob_private_unmasked, 56);
  dsk_curve448_random_to_private (bob_private);
  dsk_curve448_private_to_public (bob_private, test);
  assert(memcmp (test, bob_public, 56) == 0);

  const uint8_t *shared_secret =
          (uint8_t *) "\x07\xff\xf4\x18\x1a\xc6\xcc\x95\xec\x1c\x16\xa9\x4a\x0f"
                      "\x74\xd1\x2d\xa2\x32\xce\x40\xa7\x75\x52\x28\x1d\x28\x2b"
                      "\xb6\x0c\x0b\x56\xfd\x24\x64\xc3\x35\x54\x39\x36\x52\x1c"
                      "\x24\x40\x30\x85\xd5\x9a\x44\x9a\x50\x37\x51\x4a\x87\x9d";
  dsk_curve448_private_to_shared (bob_private, alice_public, test);
  assert (memcmp (test, shared_secret, 56) == 0);
  dsk_curve448_private_to_shared (alice_private, bob_public, test);
  assert (memcmp (test, shared_secret, 56) == 0);
}

// Appendix from rfc2104
static void
test_hmac (void)
{
  uint8_t digest[16];
  dsk_hmac_digest (&dsk_checksum_type_md5,
                   16, (uint8_t *) "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b",
                   8, (uint8_t *) "Hi There",
                   digest);
  assert (memcmp (digest, "\x92\x94\x72\x7a\x36\x38\xbb\x1c\x13\xf4\x8e\xf8\x15\x8b\xfc\x9d", 16) == 0);


  dsk_hmac_digest (&dsk_checksum_type_md5,
                   4, (uint8_t *) "Jefe",
                   28, (uint8_t *) "what do ya want for nothing?",
                   digest);
  assert (memcmp (digest, "\x75\x0c\x78\x3e\x6a\xb0\xb5\x03\xea\xa8\x6e\x31\x0a\x5d\xb7\x38", 16) == 0);

  dsk_hmac_digest (&dsk_checksum_type_md5,
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
       dsk_hmac_digest (&dsk_checksum_type_sha256,
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
  dsk_hkdf_extract (&dsk_checksum_type_sha256,
                    13, (uint8_t *) "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c",
                    22, (uint8_t *) "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b",
                    prk);
  assert (memcmp (prk, "\x07\x77\x09\x36\x2c\x2e\x32\xdf\x0d\xdc\x3f\x0d\xc4\x7b\xba\x63"
                       "\x90\xb6\xc7\x3b\xb5\x0f\x9c\x31\x22\xec\x84\x4a\xd7\xc2\xb3\xe5", 32) == 0);
  dsk_hkdf_expand (&dsk_checksum_type_sha256, prk,
                   10, (uint8_t *) "\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9",
                   42, okm);
  assert (memcmp (okm, "\x3c\xb2\x5f\x25\xfa\xac\xd5\x7a\x90\x43\x4f\x64\xd0\x36\x2f\x2a"
                       "\x2d\x2d\x0a\x90\xcf\x1a\x5a\x4c\x5d\xb0\x2d\x56\xec\xc4\xc5\xbf"
                       "\x34\x00\x72\x08\xd5\xb8\x87\x18\x58\x65", 42) == 0);

  // Test Case 2.
  dsk_hkdf_extract (&dsk_checksum_type_sha256,
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
  dsk_hkdf_expand (&dsk_checksum_type_sha256,
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

static void
test_chacha20 (void)
{
  static const uint32_t key[8] = {
    0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c,
    0x13121110, 0x17161514, 0x1b1a1918, 0x1f1e1d1c,
  };
  static const uint32_t nonce[3] = {
    0x09000000, 0x4a000000, 0x00000000
  };
  uint8_t out[64];
  dsk_chacha20_block_256 (key, 1, nonce, out);
  assert(memcmp (out,
                 "\x10\xf1\xe7\xe4\xd1\x3b\x59\x15"
                 "\x50\x0f\xdd\x1f\xa3\x20\x71\xc4"
                 "\xc7\xd1\xf4\xc7\x33\xc0\x68\x03"
                 "\x04\x22\xaa\x9a\xc3\xd4\x6c\x4e"
                 "\xd2\x82\x64\x46\x07\x9f\xaa\x09"
                 "\x14\xc2\xd7\x05\xd9\x8b\x02\xa2"
                 "\xb5\x12\x9c\xd1\xde\x16\x4e\xb9"
                 "\xcb\xd0\x83\xe8\xa2\x50\x3c\x4e",
                 64) == 0);

  //
  // Test vector from 2.4.2
  //
  static const uint32_t nonce2[3] = {
    0x00000000, 0x4a000000, 0x00000000
  };
  const char plaintext[] =
    "Ladies and Gentlemen of the class of '99: "
    "If I could offer you only one tip for the future, sunscreen would be it.";
  unsigned plaintext_len = sizeof(plaintext) - 1;
  assert(plaintext_len == 114);
  uint8_t ciphertext[114];
  dsk_chacha20_crypt_256 (key, 1, nonce2, plaintext_len, (uint8_t *) plaintext, ciphertext);
  assert(memcmp(ciphertext,
                "\x6e\x2e\x35\x9a\x25\x68\xf9\x80"
                "\x41\xba\x07\x28\xdd\x0d\x69\x81"
                "\xe9\x7e\x7a\xec\x1d\x43\x60\xc2"
                "\x0a\x27\xaf\xcc\xfd\x9f\xae\x0b"
                "\xf9\x1b\x65\xc5\x52\x47\x33\xab"
                "\x8f\x59\x3d\xab\xcd\x62\xb3\x57"
                "\x16\x39\xd6\x24\xe6\x51\x52\xab"
                "\x8f\x53\x0c\x35\x9f\x08\x61\xd8"
                "\x07\xca\x0d\xbf\x50\x0d\x6a\x61"
                "\x56\xa3\x8e\x08\x8a\x22\xb6\x5e"
                "\x52\xbc\x51\x4d\x16\xcc\xf8\x06"
                "\x81\x8c\xe9\x1a\xb7\x79\x37\x36"
                "\x5a\xf9\x0b\xbf\x74\xa3\x5b\xe6"
                "\xb4\x0b\x8e\xed\xf2\x78\x5e\x42"
                "\x87\x4d",
                114) == 0);
}

static void
test_poly1305_mac (void)
{
  static const uint32_t key[8] = {
    0x78bed685, 0x336d5557, 0xfe52447f, 0xa806d542,
    0x8a800301, 0xfdb20dfb, 0xaff6bf4a, 0x1bf54941,
  };
  static const char message[] = "Cryptographic Forum Research Group";
  unsigned message_len = sizeof(message) - 1;
  assert (message_len == 34);
  uint8_t mac[16];
  dsk_poly1305_mac (key, message_len, (const uint8_t *) message, mac);
  assert (memcmp (mac,
                  "\xa8\x06\x1d\xc1\x30\x51\x36\xc6"
                  "\xc2\x2b\x8b\xaf\x0c\x01\x27\xa9",
                  16) == 0);
}
static void
test_poly1305_keygen (void)
{
  static const uint32_t key[8] = {
    //0xq3q2q1q0, 0xq7q6q5q4, 0xqbqaq9q8, 0xqfqeqdqc,
    0x83828180, 0x87868584, 0x8b8a8988, 0x8f8e8d8c,
    0x93929190, 0x97969594, 0x9b9a9998, 0x9f9e9d9c,
  };
  static const uint32_t nonce[3] = {
    0x00000000, 0x03020100, 0x07060504,
  };
  uint32_t out[8];
  dsk_poly1305_key_gen (key, nonce, out);
  static const uint32_t expected[8] = {
    0x8ba0d58a, 0xcc815f90, 0x27405081, 0x7194b24a,
    0x37b633a8, 0xa50dfde3, 0xe2b8db08, 0x46a6d1fd,
  };
  assert (memcmp (out, expected, 32) == 0);
}

static void
test_chachapoly_aead (void)
{
  static const char plaintext[] =
    "Ladies and Gentlemen of the class of '99: "
    "If I could offer you only one tip for the future, sunscreen would be it.";
  unsigned plaintext_len = sizeof(plaintext) - 1;

  static const uint8_t assoc_data[] = {
    0x50, 0x51, 0x52, 0x53, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7 
  };
  unsigned assoc_data_len = sizeof(assoc_data);

  static const uint32_t key[] = {
    0x83828180, 0x87868584, 0x8b8a8988, 0x8f8e8d8c,
    0x93929190, 0x97969594, 0x9b9a9998, 0x9f9e9d9c

  };
  static const uint32_t iv[] = {
    0x00000007, 0x43424140, 0x47464544
  };
  uint8_t ciphertext[sizeof(plaintext)-1];
  uint8_t tag[16];
  dsk_aead_chacha20_poly1305_encrypt (key, plaintext_len, (uint8_t *) plaintext,
                                      assoc_data_len, assoc_data,
                                      iv,
                                      ciphertext, tag);
  assert (memcmp (tag, 
                  "\x1a\xe1\x0b\x59\x4f\x09\xe2\x6a"
                  "\x7e\x90\x2e\xcb\xd0\x60\x06\x91",
                  16) == 0);
  assert (memcmp (ciphertext, 
                  "\xd3\x1a\x8d\x34\x64\x8e\x60\xdb"
                  "\x7b\x86\xaf\xbc\x53\xef\x7e\xc2"
                  "\xa4\xad\xed\x51\x29\x6e\x08\xfe"
                  "\xa9\xe2\xb5\xa7\x36\xee\x62\xd6"
                  "\x3d\xbe\xa4\x5e\x8c\xa9\x67\x12"
                  "\x82\xfa\xfb\x69\xda\x92\x72\x8b"
                  "\x1a\x71\xde\x0a\x9e\x06\x0b\x29"
                  "\x05\xd6\xa5\xb6\x7e\xcd\x3b\x36"
                  "\x92\xdd\xbd\x7f\x2d\x77\x8b\x8c"
                  "\x98\x03\xae\xe3\x28\x09\x1b\x58"
                  "\xfa\xb3\x24\xe4\xfa\xd6\x75\x94"
                  "\x55\x85\x80\x8b\x48\x31\xd7\xbc"
                  "\x3f\xf4\xde\xf0\x8e\x4b\x7a\x9d"
                  "\xe5\x76\xd2\x65\x86\xce\xc6\x4b"
                  "\x61\x16",
                  plaintext_len) == 0);
  uint8_t *rt_plaintext = alloca (plaintext_len);
  if (!dsk_aead_chacha20_poly1305_decrypt (key, plaintext_len, ciphertext,
                                           assoc_data_len, assoc_data,
                                           iv,
                                           rt_plaintext, tag))
    assert(false);
  assert(memcmp (rt_plaintext, plaintext, plaintext_len) == 0);
}

static void
test_ecprime (void)
{
  //
  // Following Exercise 4.1 from 
  //
  //    Lawrence Washington, Elliptic Curve:  Number Theory and Cryptography, 2nd Ed,  2008.
  //
  //    the elliptic curve's equation is:
  //            y^2=x^3+x+1 and p=5, and base-point=(0,1).
  //    the book gives: 3*(0,1) == (2,1).
  //
  static uint32_t hecc_A[] = {1};
  static uint32_t hecc_B[] = {1};
  static uint32_t hecc_p[] = {23};
  static uint32_t hecc_x[] = {3};
  static uint32_t hecc_y[] = {10};
  static uint32_t hecc_barrett_mu[2] = { 0x8590b216, 0x0b21642c };
  static uint32_t hecc_xy0[] = {0,0};

  static uint32_t xpow[] = {
     7,19,17, 9,12,11,13, 0, 6,18, 5, 1, 4, 1, 5,18, 6, 0,13,11,12, 9,17,19, 7, 3
  };
  static uint32_t ypow[] = {
    12, 5, 3,16, 4, 3,16, 1, 4,20, 4, 7, 0,16,19, 3,19,22, 7,20,19, 7,20,18,11,13
  };

  static DskTls_ECPrime_Group g_hecc= { "t23", 1, hecc_p, hecc_barrett_mu, hecc_A, hecc_B, hecc_x, hecc_y, hecc_xy0, true, true, 22 };
  uint32_t xin[1] = {3}, yin[1] = {10};
  uint32_t xout[1], yout[1];
  for (unsigned i = 0; i < DSK_N_ELEMENTS (xpow); i++)
    {
      uint32_t k = i + 2;
      dsk_tls_ecprime_add (&g_hecc, hecc_x, hecc_y, xin, yin, xout, yout);
      assert(xout[0] == xpow[i]);
      assert(yout[0] == ypow[i]);

      dsk_tls_ecprime_multiply_int (&g_hecc, hecc_x, hecc_y, 1, &k, xout, yout);
      assert(xout[0] == xpow[i]);
      assert(yout[0] == ypow[i]);


      xin[0] = xout[0];
      yin[0] = yout[0];
    }
}

static void
parse_big_decimal (const char *str, unsigned out_len, uint32_t *out)
{
  memset (out, 0, out_len * 4);
  for (unsigned at = 0; str[at] != 0; at++)
    {
      dsk_tls_bignum_multiply_word (out_len, out, 10, out);
      dsk_tls_bignum_add_word_inplace (out_len, out, str[at] - '0');
    }
}
static void
parse_hex_exact (const char *str, unsigned out_len, uint32_t *out)
{
  char hex[9];
  for (unsigned i = 0; i < out_len; i++)
    {
      memcpy (hex, str + (out_len - 1 - i) * 8, 8);
      hex[8] = 0;
      unsigned tmp = strtoul (hex, NULL, 16);
      out[i] = tmp;
    }
}

  struct TestEC {
    const char *k;
    const char *x;
    const char *y;
  };
static void
generic_secp_tests(const DskTls_ECPrime_Group *group,
                   unsigned N,
                   const struct TestEC *tests)
{
  uint32_t *k = alloca(group->len * 4);
  uint32_t *x = alloca(group->len * 4);
  uint32_t *y = alloca(group->len * 4);

  for (unsigned i = 0; i < N; i++)
    {
      //printf("%s %u\n",group->name,i);
      uint32_t *xout = alloca(group->len * 4);
      uint32_t *yout = alloca(group->len * 4);
      parse_big_decimal (tests[i].k, group->len, k);
      unsigned k_len = dsk_tls_bignum_actual_len (group->len, k);
      dsk_tls_ecprime_multiply_int (group,
                                    group->x,
                                    group->y,
                                    k_len, k,
                                    xout, yout);

      parse_hex_exact (tests[i].x, group->len, x);
      parse_hex_exact (tests[i].y, group->len, y);
      #if 0
      printf("act x:");for (unsigned i=0;i<group->len;i++)printf(" %08x", xout[i]);printf("\n");
      printf("act y:");for (unsigned i=0;i<group->len;i++)printf(" %08x", yout[i]);printf("\n");
      printf("exp x:");for (unsigned i=0;i<group->len;i++)printf(" %08x", x[i]);printf("\n");
      printf("exp y:");for (unsigned i=0;i<group->len;i++)printf(" %08x", y[i]);printf("\n");
      #endif
      assert (memcmp (x, xout, group->len*4) == 0);
      assert (memcmp (y, yout, group->len*4) == 0);
    }
}
static void
test_secp256r1 (void)
{
  static struct TestEC tests[] = {
    {
      "1",
      "6B17D1F2E12C4247F8BCE6E563A440F277037D812DEB33A0F4A13945D898C296",
      "4FE342E2FE1A7F9B8EE7EB4A7C0F9E162BCE33576B315ECECBB6406837BF51F5",
    },
    {
      "2",
      "7CF27B188D034F7E8A52380304B51AC3C08969E277F21B35A60B48FC47669978",
      "07775510DB8ED040293D9AC69F7430DBBA7DADE63CE982299E04B79D227873D1",
    },
    {
      "3",
      "5ECBE4D1A6330A44C8F7EF951D4BF165E6C6B721EFADA985FB41661BC6E7FD6C",
       "8734640C4998FF7E374B06CE1A64A2ECD82AB036384FB83D9A79B127A27D5032"
    },

    {
      "4",
      "E2534A3532D08FBBA02DDE659EE62BD0031FE2DB785596EF509302446B030852",
       "E0F1575A4C633CC719DFEE5FDA862D764EFC96C3F30EE0055C42C23F184ED8C6"
    },

    {
      "5",
      "51590B7A515140D2D784C85608668FDFEF8C82FD1F5BE52421554A0DC3D033ED",
       "E0C17DA8904A727D8AE1BF36BF8A79260D012F00D4D80888D1D0BB44FDA16DA4"
    },

    {
      "6",
      "B01A172A76A4602C92D3242CB897DDE3024C740DEBB215B4C6B0AAE93C2291A9",
       "E85C10743237DAD56FEC0E2DFBA703791C00F7701C7E16BDFD7C48538FC77FE2"
    },

    {
      "7",
      "8E533B6FA0BF7B4625BB30667C01FB607EF9F8B8A80FEF5B300628703187B2A3",
       "73EB1DBDE03318366D069F83A6F5900053C73633CB041B21C55E1A86C1F400B4"
    },

    {
      "8",
      "62D9779DBEE9B0534042742D3AB54CADC1D238980FCE97DBB4DD9DC1DB6FB393",
       "AD5ACCBD91E9D8244FF15D771167CEE0A2ED51F6BBE76A78DA540A6A0F09957E"
    },

    {
      "9",
      "EA68D7B6FEDF0B71878938D51D71F8729E0ACB8C2C6DF8B3D79E8A4B90949EE0",
       "2A2744C972C9FCE787014A964A8EA0C84D714FEAA4DE823FE85A224A4DD048FA"
    },

    {
      "10",
      "CEF66D6B2A3A993E591214D1EA223FB545CA6C471C48306E4C36069404C5723F",
       "878662A229AAAE906E123CDD9D3B4C10590DED29FE751EEECA34BBAA44AF0773"
    },

    {
      "11",
      "3ED113B7883B4C590638379DB0C21CDA16742ED0255048BF433391D374BC21D1",
       "9099209ACCC4C8A224C843AFA4F4C68A090D04DA5E9889DAE2F8EEFCE82A3740"
    },

    {
      "12",
      "741DD5BDA817D95E4626537320E5D55179983028B2F82C99D500C5EE8624E3C4",
       "0770B46A9C385FDC567383554887B1548EEB912C35BA5CA71995FF22CD4481D3"
    },

    {
      "13",
      "177C837AE0AC495A61805DF2D85EE2FC792E284B65EAD58A98E15D9D46072C01",
       "63BB58CD4EBEA558A24091ADB40F4E7226EE14C3A1FB4DF39C43BBE2EFC7BFD8"
    },

    {
      "14",
      "54E77A001C3862B97A76647F4336DF3CF126ACBE7A069C5E5709277324D2920B",
       "F599F1BB29F4317542121F8C05A2E7C37171EA77735090081BA7C82F60D0B375"
    },

    {
      "15",
      "F0454DC6971ABAE7ADFB378999888265AE03AF92DE3A0EF163668C63E59B9D5F",
       "B5B93EE3592E2D1F4E6594E51F9643E62A3B21CE75B5FA3F47E59CDE0D034F36"
    },

    {
      "16",
      "76A94D138A6B41858B821C629836315FCD28392EFF6CA038A5EB4787E1277C6E",
       "A985FE61341F260E6CB0A1B5E11E87208599A0040FC78BAA0E9DDD724B8C5110"
    },

    {
      "17",
      "47776904C0F1CC3A9C0984B66F75301A5FA68678F0D64AF8BA1ABCE34738A73E",
       "AA005EE6B5B957286231856577648E8381B2804428D5733F32F787FF71F1FCDC"
    },

    {
      "18",
      "1057E0AB5780F470DEFC9378D1C7C87437BB4C6F9EA55C63D936266DBD781FDA",
       "F6F1645A15CBE5DC9FA9B7DFD96EE5A7DCC11B5C5EF4F1F78D83B3393C6A45A2"
    },

    {
      "19",
      "CB6D2861102C0C25CE39B7C17108C507782C452257884895C1FC7B74AB03ED83",
       "58D7614B24D9EF515C35E7100D6D6CE4A496716E30FA3E03E39150752BCECDAA"
    },

    {
      "20",
      "83A01A9378395BAB9BCD6A0AD03CC56D56E6B19250465A94A234DC4C6B28DA9A",
       "76E49B6DE2F73234AE6A5EB9D612B75C9F2202BB6923F54FF8240AAA86F640B8"
    },

    {
      "112233445566778899",
      "339150844EC15234807FE862A86BE77977DBFB3AE3D96F4C22795513AEAAB82F",
       "B1C14DDFDC8EC1B2583F51E85A5EB3A155840F2034730E9B5ADA38B674336A21"
    },

    {
      "112233445566778899112233445566778899",
      "1B7E046A076CC25E6D7FA5003F6729F665CC3241B5ADAB12B498CD32F2803264",
       "BFEA79BE2B666B073DB69A2A241ADAB0738FE9D2DD28B5604EB8C8CF097C457B"
    },

    {
      "29852220098221261079183923314599206100666902414330245206392788703677545185283",
      "9EACE8F4B071E677C5350B02F2BB2B384AAE89D58AA72CA97A170572E0FB222F",
       "1BBDAEC2430B09B93F7CB08678636CE12EAAFD58390699B5FD2F6E1188FC2A78"
    },

    {
      "57896042899961394862005778464643882389978449576758748073725983489954366354431",
      "878F22CC6DB6048D2B767268F22FFAD8E56AB8E2DC615F7BD89F1E350500DD8D",
       "714A5D7BB901C9C5853400D12341A892EF45D87FC553786756C4F0C9391D763E"
    },

    {
      "1766845392945710151501889105729049882997660004824848915955419660366636031",
      "659A379625AB122F2512B8DADA02C6348D53B54452DFF67AC7ACE4E8856295CA",
       "49D81AB97B648464D0B4A288BD7818FAB41A16426E943527C4FED8736C53D0F6"
    },

    {
      "28948025760307534517734791687894775804466072615242963443097661355606862201087",
      "CBCEAAA8A4DD44BBCE58E8DB7740A5510EC2CB7EA8DA8D8F036B3FB04CDA4DE4",
       "4BD7AA301A80D7F59FD983FEDBE59BB7B2863FE46494935E3745B360E32332FA"
    },

    {
      "113078210460870548944811695960290644973229224625838436424477095834645696384",
      "F0C4A0576154FF3A33A3460D42EAED806E854DFA37125221D37935124BA462A4",
       "5B392FA964434D29EEC6C9DBC261CF116796864AA2FAADB984A2DF38D1AEF7A3"
    },

    {
      "12078056106883488161242983286051341125085761470677906721917479268909056",
      "5E6C8524B6369530B12C62D31EC53E0288173BD662BDF680B53A41ECBCAD00CC",
       "447FE742C2BFEF4D0DB14B5B83A2682309B5618E0064A94804E9282179FE089F"
    },

    {
      "57782969857385448082319957860328652998540760998293976083718804450708503920639",
      "03792E541BC209076A3D7920A915021ECD396A6EB5C3960024BE5575F3223484",
       "FC774AE092403101563B712F68170312304F20C80B40C06282063DB25F268DE4"
    },

    {
      "57896017119460046759583662757090100341435943767777707906455551163257755533312",
      "2379FF85AB693CDF901D6CE6F2473F39C04A2FE3DCD842CE7AAB0E002095BCF8",
       "F8B476530A634589D5129E46F322B02FBC610A703D80875EE70D7CE1877436A1"
    },

    {
      "452312848374287284681282171017647412726433684238464212999305864837160993279",
      "C1E4072C529BF2F44DA769EFC934472848003B3AF2C0F5AA8F8DDBD53E12ED7C",
       "39A6EE77812BB37E8079CD01ED649D3830FCA46F718C1D3993E4A591824ABCDB"
    },

    {
      "904571339174065134293634407946054000774746055866917729876676367558469746684",
      "34DFBC09404C21E250A9B40FA8772897AC63A094877DB65862B61BD1507B34F3",
       "CF6F8A876C6F99CEAEC87148F18C7E1E0DA6E165FFC8ED82ABB65955215F77D3"
    },

    {
      "115792089210356248762697446949407573529996955224135760342422259061068512044349",
      "83A01A9378395BAB9BCD6A0AD03CC56D56E6B19250465A94A234DC4C6B28DA9A",
       "891B64911D08CDCC5195A14629ED48A360DDFD4596DC0AB007DBF5557909BF47"
    },

    {
      "115792089210356248762697446949407573529996955224135760342422259061068512044350",
      "CB6D2861102C0C25CE39B7C17108C507782C452257884895C1FC7B74AB03ED83",
       "A7289EB3DB2610AFA3CA18EFF292931B5B698E92CF05C1FC1C6EAF8AD4313255"
    },

    {
      "115792089210356248762697446949407573529996955224135760342422259061068512044351",
      "1057E0AB5780F470DEFC9378D1C7C87437BB4C6F9EA55C63D936266DBD781FDA",
       "090E9BA4EA341A246056482026911A58233EE4A4A10B0E08727C4CC6C395BA5D"
    },

    {
      "115792089210356248762697446949407573529996955224135760342422259061068512044352",
      "47776904C0F1CC3A9C0984B66F75301A5FA68678F0D64AF8BA1ABCE34738A73E",
       "55FFA1184A46A8D89DCE7A9A889B717C7E4D7FBCD72A8CC0CD0878008E0E0323"
    },

    {
      "115792089210356248762697446949407573529996955224135760342422259061068512044353",
      "76A94D138A6B41858B821C629836315FCD28392EFF6CA038A5EB4787E1277C6E",
       "567A019DCBE0D9F2934F5E4A1EE178DF7A665FFCF0387455F162228DB473AEEF"
    },

    {
      "115792089210356248762697446949407573529996955224135760342422259061068512044354",
      "F0454DC6971ABAE7ADFB378999888265AE03AF92DE3A0EF163668C63E59B9D5F",
       "4A46C11BA6D1D2E1B19A6B1AE069BC19D5C4DE328A4A05C0B81A6321F2FCB0C9"
    },

    {
      "115792089210356248762697446949407573529996955224135760342422259061068512044355",
      "54E77A001C3862B97A76647F4336DF3CF126ACBE7A069C5E5709277324D2920B",
       "0A660E43D60BCE8BBDEDE073FA5D183C8E8E15898CAF6FF7E45837D09F2F4C8A"
    },

    {
      "115792089210356248762697446949407573529996955224135760342422259061068512044356",
      "177C837AE0AC495A61805DF2D85EE2FC792E284B65EAD58A98E15D9D46072C01",
       "9C44A731B1415AA85DBF6E524BF0B18DD911EB3D5E04B20C63BC441D10384027"
    },

    {
      "115792089210356248762697446949407573529996955224135760342422259061068512044357",
      "741DD5BDA817D95E4626537320E5D55179983028B2F82C99D500C5EE8624E3C4",
       "F88F4B9463C7A024A98C7CAAB7784EAB71146ED4CA45A358E66A00DD32BB7E2C"
    },

    {
      "115792089210356248762697446949407573529996955224135760342422259061068512044358",
      "3ED113B7883B4C590638379DB0C21CDA16742ED0255048BF433391D374BC21D1",
       "6F66DF64333B375EDB37BC505B0B3975F6F2FB26A16776251D07110317D5C8BF"
    },

    {
      "115792089210356248762697446949407573529996955224135760342422259061068512044359",
      "CEF66D6B2A3A993E591214D1EA223FB545CA6C471C48306E4C36069404C5723F",
       "78799D5CD655517091EDC32262C4B3EFA6F212D7018AE11135CB4455BB50F88C"
    },

    {
      "115792089210356248762697446949407573529996955224135760342422259061068512044360",
      "EA68D7B6FEDF0B71878938D51D71F8729E0ACB8C2C6DF8B3D79E8A4B90949EE0",
       "D5D8BB358D36031978FEB569B5715F37B28EB0165B217DC017A5DDB5B22FB705"
    },

    {
      "115792089210356248762697446949407573529996955224135760342422259061068512044361",
      "62D9779DBEE9B0534042742D3AB54CADC1D238980FCE97DBB4DD9DC1DB6FB393",
       "52A533416E1627DCB00EA288EE98311F5D12AE0A4418958725ABF595F0F66A81"
    },

    {
      "115792089210356248762697446949407573529996955224135760342422259061068512044362",
      "8E533B6FA0BF7B4625BB30667C01FB607EF9F8B8A80FEF5B300628703187B2A3",
       "8C14E2411FCCE7CA92F9607C590A6FFFAC38C9CD34FBE4DE3AA1E5793E0BFF4B"
    },

    {
      "115792089210356248762697446949407573529996955224135760342422259061068512044363",
      "B01A172A76A4602C92D3242CB897DDE3024C740DEBB215B4C6B0AAE93C2291A9",
       "17A3EF8ACDC8252B9013F1D20458FC86E3FF0890E381E9420283B7AC7038801D"
    },

    {
      "115792089210356248762697446949407573529996955224135760342422259061068512044364",
      "51590B7A515140D2D784C85608668FDFEF8C82FD1F5BE52421554A0DC3D033ED",
       "1F3E82566FB58D83751E40C9407586D9F2FED1002B27F7772E2F44BB025E925B"
    },

    {
      "115792089210356248762697446949407573529996955224135760342422259061068512044365",
      "E2534A3532D08FBBA02DDE659EE62BD0031FE2DB785596EF509302446B030852",
       "1F0EA8A4B39CC339E62011A02579D289B103693D0CF11FFAA3BD3DC0E7B12739"
    },

    {
      "115792089210356248762697446949407573529996955224135760342422259061068512044366",
      "5ECBE4D1A6330A44C8F7EF951D4BF165E6C6B721EFADA985FB41661BC6E7FD6C",
       "78CB9BF2B6670082C8B4F931E59B5D1327D54FCAC7B047C265864ED85D82AFCD"
    },

    {
      "115792089210356248762697446949407573529996955224135760342422259061068512044367",
      "7CF27B188D034F7E8A52380304B51AC3C08969E277F21B35A60B48FC47669978",
       "F888AAEE24712FC0D6C26539608BCF244582521AC3167DD661FB4862DD878C2E"
    },

    {
      "115792089210356248762697446949407573529996955224135760342422259061068512044368",
      "6B17D1F2E12C4247F8BCE6E563A440F277037D812DEB33A0F4A13945D898C296",
      "B01CBD1C01E58065711814B583F061E9D431CCA994CEA1313449BF97C840AE0A",
    }
  };

  generic_secp_tests(&dsk_tls_ecprime_group_secp256r1,
                     DSK_N_ELEMENTS (tests),
                     tests);
}

static void
test_secp384r1 (void)
{
  static struct TestEC tests[] = {
    {
    "1",
    "AA87CA22BE8B05378EB1C71EF320AD746E1D3B628BA79B9859F741E082542A385502F25DBF55296C3A545E3872760AB7",
    "3617DE4A96262C6F5D9E98BF9292DC29F8F41DBD289A147CE9DA3113B5F0B8C00A60B1CE1D7E819D7A431D7C90EA0E5F",
    },
    {
    "2",
    "08D999057BA3D2D969260045C55B97F089025959A6F434D651D207D19FB96E9E4FE0E86EBE0E64F85B96A9C75295DF61",
    "8E80F1FA5B1B3CEDB7BFE8DFFD6DBA74B275D875BC6CC43E904E505F256AB4255FFD43E94D39E22D61501E700A940E80",
    },
    {
    "3",
    "077A41D4606FFA1464793C7E5FDC7D98CB9D3910202DCD06BEA4F240D3566DA6B408BBAE5026580D02D7E5C70500C831",
    "C995F7CA0B0C42837D0BBE9602A9FC998520B41C85115AA5F7684C0EDC111EACC24ABD6BE4B5D298B65F28600A2F1DF1",
    },
    {
    "4",
    "138251CD52AC9298C1C8AAD977321DEB97E709BD0B4CA0ACA55DC8AD51DCFC9D1589A1597E3A5120E1EFD631C63E1835",
    "CACAE29869A62E1631E8A28181AB56616DC45D918ABC09F3AB0E63CF792AA4DCED7387BE37BBA569549F1C02B270ED67",
    },
    {
    "5",
    "11DE24A2C251C777573CAC5EA025E467F208E51DBFF98FC54F6661CBE56583B037882F4A1CA297E60ABCDBC3836D84BC",
    "8FA696C77440F92D0F5837E90A00E7C5284B447754D5DEE88C986533B6901AEB3177686D0AE8FB33184414ABE6C1713A",
    },
    {
    "6",
    "627BE1ACD064D2B2226FE0D26F2D15D3C33EBCBB7F0F5DA51CBD41F26257383021317D7202FF30E50937F0854E35C5DF",
    "09766A4CB3F8B1C21BE6DDA6C14F1575B2C95352644F774C99864F613715441604C45B8D84E165311733A408D3F0F934",
    },
    {
    "7",
    "283C1D7365CE4788F29F8EBF234EDFFEAD6FE997FBEA5FFA2D58CC9DFA7B1C508B05526F55B9EBB2040F05B48FB6D0E1",
    "9475C99061E41B88BA52EFDB8C1690471A61D867ED799729D9C92CD01DBD225630D84EDE32A78F9E64664CDAC512EF8C",
    },
    {
    "8",
    "1692778EA596E0BE75114297A6FA383445BF227FBE58190A900C3C73256F11FB5A3258D6F403D5ECE6E9B269D822C87D",
    "DCD2365700D4106A835388BA3DB8FD0E22554ADC6D521CD4BD1C30C2EC0EEC196BADE1E9CDD1708D6F6ABFA4022B0AD2",
    },
    {
    "9",
    "8F0A39A4049BCB3EF1BF29B8B025B78F2216F7291E6FD3BAC6CB1EE285FB6E21C388528BFEE2B9535C55E4461079118B",
    "62C77E1438B601D6452C4A5322C3A9799A9B3D7CA3C400C6B7678854AED9B3029E743EFEDFD51B68262DA4F9AC664AF8",
    },
    {
    "10",
    "A669C5563BD67EEC678D29D6EF4FDE864F372D90B79B9E88931D5C29291238CCED8E85AB507BF91AA9CB2D13186658FB",
    "A988B72AE7C1279F22D9083DB5F0ECDDF70119550C183C31C502DF78C3B705A8296D8195248288D997784F6AB73A21DD",
    },
    {
    "11",
    "099056E27DA7B998DA1EEEC2904816C57FE935ED5837C37456C9FD14892D3F8C4749B66E3AFB81D626356F3B55B4DDD8",
    "2E4C0C234E30AB96688505544AC5E0396FC4EED8DFC363FD43FF93F41B52A3255466D51263AAFF357D5DBA8138C5E0BB",
    },
    {
    "12",
    "952A7A349BD49289AB3AC421DCF683D08C2ED5E41F6D0E21648AF2691A481406DA4A5E22DA817CB466DA2EA77D2A7022",
    "A0320FAF84B5BC0563052DEAE6F66F2E09FB8036CE18A0EBB9028B096196B50D031AA64589743E229EF6BACCE21BD16E",
    },
    {
    "13",
    "A567BA97B67AEA5BAFDAF5002FFCC6AB9632BFF9F01F873F6267BCD1F0F11C139EE5F441ABD99F1BAAF1CA1E3B5CBCE7",
    "DE1B38B3989F3318644E4147AF164ECC5185595046932EC086329BE057857D66776BCB8272218A7D6423A12736F429CC",
    },
    {
    "14",
    "E8C8F94D44FBC2396BBEAC481B89D2B0877B1DFFD23E7DC95DE541EB651CCA2C41ABA24DBC02DE6637209ACCF0F59EA0",
    "891AE44356FC8AE0932BCBF6DE52C8A933B86191E7728D79C8319413A09D0F48FC468BA05509DE22D7EE5C9E1B67B888",
    },
    {
    "15",
    "B3D13FC8B32B01058CC15C11D813525522A94156FFF01C205B21F9F7DA7C4E9CA849557A10B6383B4B88701A9606860B",
    "152919E7DF9162A61B049B2536164B1BEEBAC4A11D749AF484D1114373DFBFD9838D24F8B284AF50985D588D33F7BD62",
    },
    {
    "16",
    "D5D89C3B5282369C5FBD88E2B231511A6B80DFF0E5152CF6A464FA9428A8583BAC8EBC773D157811A462B892401DAFCF",
    "D815229DE12906D241816D5E9A9448F1D41D4FC40E2A3BDB9CABA57E440A7ABAD1210CB8F49BF2236822B755EBAB3673",
    },
    {
    "17",
    "4099952208B4889600A5EBBCB13E1A32692BEFB0733B41E6DCC614E42E5805F817012A991AF1F486CAF3A9ADD9FFCC03",
    "5ECF94777833059839474594AF603598163AD3F8008AD0CD9B797D277F2388B304DA4D2FAA9680ECFA650EF5E23B09A0",
    },
    {
    "18",
    "DFB1FE3A40F7AC9B64C41D39360A7423828B97CB088A4903315E402A7089FA0F8B6C2355169CC9C99DFB44692A9B93DD",
    "453ACA1243B5EC6B423A68A25587E1613A634C1C42D2EE7E6C57F449A1C91DC89168B7036EC0A7F37A366185233EC522",
    },
    {
    "19",
    "8D481DAB912BC8AB16858A211D750B77E07DBECCA86CD9B012390B430467AABF59C8651060801C0E9599E68713F5D41B",
    "A1592FF0121460857BE99F2A60669050B2291B68A1039AA0594B32FD7ADC0E8C11FFBA5608004E646995B07E75E52245",
    },
    {
    "20",
    "605508EC02C534BCEEE9484C86086D2139849E2B11C1A9CA1E2808DEC2EAF161AC8A105D70D4F85C50599BE5800A623F",
    "5158EE87962AC6B81F00A103B8543A07381B7639A3A65F1353AEF11B733106DDE92E99B78DE367B48E238C38DAD8EEDD",
    },
    {
    "112233445566778899",
    "A499EFE48839BC3ABCD1C5CEDBDD51904F9514DB44F4686DB918983B0C9DC3AEE05A88B72433E9515F91A329F5F4FA60",
    "3B7CA28EF31F809C2F1BA24AAED847D0F8B406A4B8968542DE139DB5828CA410E615D1182E25B91B1131E230B727D36A",
    },
    {
    "112233445566778899112233445566778899",
    "90A0B1CAC601676B083F21E07BC7090A3390FE1B9C7F61D842D27FA315FB38D83667A11A71438773E483F2A114836B24",
    "3197D3C6123F0D6CD65D5F0DE106FEF36656CB16DC7CD1A6817EB1D51510135A8F492F72665CFD1053F75ED03A7D04C9",
    },
    {
    "10158184112867540819754776755819761756724522948540419979637868435924061464745859402573149498125806098880003248619520",
    "F2A066BD332DC59BBC3D01DA1B124C687D8BB44611186422DE94C1DA4ECF150E664D353CCDB5CB2652685F8EB4D2CD49",
    "D6ED0BF75FDD8E53D87765FA746835B673881D6D1907163A2C43990D75B454294F942EC571AD5AAE1806CAF2BB8E9A4A",
    },
    {
    "9850501551105991028245052605056992139810094908912799254115847683881357749738726091734403950439157209401153690566655",
    "5C7F9845D1C4AA44747F9137B6F9C39B36B26B8A62E8AF97290434D5F3B214F5A0131550ADB19058DC4C8780C4165C4A",
    "712F7FCCC86F647E70DB8798228CB16344AF3D00B139B6F8502939C2A965AF0EB4E39E2E16AB8F597B8D5630A50C9D85",
    },
    {
    "9850502723405747097317271194763310482462751455185699630571661657946308788426092983270628740691202018691293898608608",
    "DD5838F7EC3B8ACF1BECFD746F8B668C577107E93548ED93ED0D254C112E76B10F053109EF8428BFCD50D38C4C030C57",
    "33244F479CDAC34F160D9E4CE2D19D2FF0E3305B5BF0EEF29E91E9DE6E28F678C61B773AA7E3C03740E1A49D1AA2493C",
    },
    {
    "1146189371817832990947611400450889406070215735255370280811736587845016396640969656447803207438173695115264",
    "CB8ED893530BFBA04B4CA655923AAAD109A62BC8411D5925316C32D33602459C33057A1FBCB5F70AEB295D90F9165FBC",
    "426AEE3E91B08420F9B357B66D5AFCBCF3956590BF5564DBF9086042EB880493D19DA39AAA6436C6B5FC66CE5596B43F",
    },
    {
    "9619341438217097641865390297189708858938017986426152622639500179774624579127744608993294698873437325090751520764",
    "67F714012B6B070182122DDD435CC1C2262A1AB88939BC6A2906CB2B4137C5E82B4582160F6403CAB887ACDF5786A268",
    "90E31CF398CE2F8C5897C7380BF541075D1B4D3CB70547262B7095731252F181AC0597C66AF8311C7780DB39DEC0BD32",
    },
    {
    "1231307996623833742387400352380172566077927415136813282735641918395585376659282194317590461518639141730493780722175",
    "55A79DF7B53A99D31462C7E1A5ED5623970715BB1021098CB973A7520CBD6365E613E4B2467486FB37E86E01CEE09B8F",
    "B95AEB71693189911661B709A886A1867F056A0EFE401EE11C06030E46F7A87731DA4575863178012208707DD666727C",
    },
    {
    "587118838854683800942906722504810343086699021451906946003274128973058942197377013128840514404789143516741631",
    "9539A968CF819A0E52E10EEA3BACA1B6480D7E4DF69BC07002C568569047110EE4FE72FCA423FDD5179D6E0E19C44844",
    "A7728F37A0AE0DF2716061900D83A4DA149144129F89A214A8260464BAB609BB322E4E67DE5E4C4C6CB8D25983EC19B0",
    },
    {
    "153914077530671739663795070876894766451466019374644150541452557147890542143280855693795882295846834387672681660416",
    "933FC13276672AB360D909161CD02D830B1628935DF0D800C6ED602C59D575A86A8A97E3A2D697E3ED06BE741C0097D6",
    "F35296BD7A6B4C6C025ED6D84338CCCC7522A45C5D4FBDB1442556CAEFB598128FA188793ADA510EB5F44E90A4E4BEF1",
    },
    {
    "75148784606135150476268171850082176256856776750560539466196504390587921789283134009866871754361028131485122560",
    "0CE31E1C4A937071E6EBACA026A93D783848BCC0C1585DAF639518125FCD1F1629D63041ABFB11FFC8F03FA8B6FCF6BF",
    "A69EA55BE4BEAB2D5224050FEBFFBDFCFD614624C3B4F228909EB80012F003756D1C377E52F04FA539237F24DD080E2E",
    },
    {
    "19691383761310193665095292424754807745686799029814707849273381514021788371252213000473497648851202400395528761229312",
    "6842CFE3589AC268818291F31D44177A9168DCBC19F321ED66D81ECF59E31B54CCA0DDFD4C4136780171748D69A91C54",
    "E3A5ECD5AC725F13DBC631F358C6E817EDCF3A613B83832741A9DB591A0BAE767FC714F70C2E7EA891E4312047DECCC0",
    },
    {
    "39402006196394479212279040100143613805079739270465446667946905279627659399113263569398956308152294913554433653942623",
    "605508EC02C534BCEEE9484C86086D2139849E2B11C1A9CA1E2808DEC2EAF161AC8A105D70D4F85C50599BE5800A623F",
    "AEA7117869D53947E0FF5EFC47ABC5F8C7E489C65C59A0ECAC510EE48CCEF92116D16647721C984B71DC73C825271122",
    },
    {
    "39402006196394479212279040100143613805079739270465446667946905279627659399113263569398956308152294913554433653942624",
    "8D481DAB912BC8AB16858A211D750B77E07DBECCA86CD9B012390B430467AABF59C8651060801C0E9599E68713F5D41B",
    "5EA6D00FEDEB9F7A841660D59F996FAF4DD6E4975EFC655FA6B4CD028523F172EE0045A8F7FFB19B966A4F828A1ADDBA",
    },
    {
    "39402006196394479212279040100143613805079739270465446667946905279627659399113263569398956308152294913554433653942625",
    "DFB1FE3A40F7AC9B64C41D39360A7423828B97CB088A4903315E402A7089FA0F8B6C2355169CC9C99DFB44692A9B93DD",
    "BAC535EDBC4A1394BDC5975DAA781E9EC59CB3E3BD2D118193A80BB65E36E2366E9748FB913F580C85C99E7BDCC13ADD",
    },
    {
    "39402006196394479212279040100143613805079739270465446667946905279627659399113263569398956308152294913554433653942626",
    "4099952208B4889600A5EBBCB13E1A32692BEFB0733B41E6DCC614E42E5805F817012A991AF1F486CAF3A9ADD9FFCC03",
    "A1306B8887CCFA67C6B8BA6B509FCA67E9C52C07FF752F32648682D880DC774BFB25B2CF55697F13059AF10B1DC4F65F",
    },
    {
    "39402006196394479212279040100143613805079739270465446667946905279627659399113263569398956308152294913554433653942627",
    "D5D89C3B5282369C5FBD88E2B231511A6B80DFF0E5152CF6A464FA9428A8583BAC8EBC773D157811A462B892401DAFCF",
    "27EADD621ED6F92DBE7E92A1656BB70E2BE2B03BF1D5C42463545A81BBF585442EDEF3460B640DDC97DD48AB1454C98C",
    },
    {
    "39402006196394479212279040100143613805079739270465446667946905279627659399113263569398956308152294913554433653942628",
    "B3D13FC8B32B01058CC15C11D813525522A94156FFF01C205B21F9F7DA7C4E9CA849557A10B6383B4B88701A9606860B",
    "EAD6E618206E9D59E4FB64DAC9E9B4E411453B5EE28B650B7B2EEEBC8C2040257C72DB064D7B50AF67A2A773CC08429D",
    },
    {
    "39402006196394479212279040100143613805079739270465446667946905279627659399113263569398956308152294913554433653942629",
    "E8C8F94D44FBC2396BBEAC481B89D2B0877B1DFFD23E7DC95DE541EB651CCA2C41ABA24DBC02DE6637209ACCF0F59EA0",
    "76E51BBCA903751F6CD4340921AD3756CC479E6E188D728637CE6BEC5F62F0B603B9745EAAF621DD2811A362E4984777",
    },
    {
    "39402006196394479212279040100143613805079739270465446667946905279627659399113263569398956308152294913554433653942630",
    "A567BA97B67AEA5BAFDAF5002FFCC6AB9632BFF9F01F873F6267BCD1F0F11C139EE5F441ABD99F1BAAF1CA1E3B5CBCE7",
    "21E4C74C6760CCE79BB1BEB850E9B133AE7AA6AFB96CD13F79CD641FA87A82988894347C8DDE75829BDC5ED9C90BD633",
    },
    {
    "39402006196394479212279040100143613805079739270465446667946905279627659399113263569398956308152294913554433653942631",
    "952A7A349BD49289AB3AC421DCF683D08C2ED5E41F6D0E21648AF2691A481406DA4A5E22DA817CB466DA2EA77D2A7022",
    "5FCDF0507B4A43FA9CFAD215190990D1F6047FC931E75F1446FD74F69E694AF1FCE559B9768BC1DD610945341DE42E91",
    },
    {
    "39402006196394479212279040100143613805079739270465446667946905279627659399113263569398956308152294913554433653942632",
    "099056E27DA7B998DA1EEEC2904816C57FE935ED5837C37456C9FD14892D3F8C4749B66E3AFB81D626356F3B55B4DDD8",
    "D1B3F3DCB1CF5469977AFAABB53A1FC6903B1127203C9C02BC006C0BE4AD5CD9AB992AEC9C5500CA82A2457FC73A1F44",
    },
    {
    "39402006196394479212279040100143613805079739270465446667946905279627659399113263569398956308152294913554433653942633",
    "A669C5563BD67EEC678D29D6EF4FDE864F372D90B79B9E88931D5C29291238CCED8E85AB507BF91AA9CB2D13186658FB",
    "567748D5183ED860DD26F7C24A0F132208FEE6AAF3E7C3CE3AFD20873C48FA56D6927E69DB7D77266887B09648C5DE22",
    },
    {
    "39402006196394479212279040100143613805079739270465446667946905279627659399113263569398956308152294913554433653942634",
    "8F0A39A4049BCB3EF1BF29B8B025B78F2216F7291E6FD3BAC6CB1EE285FB6E21C388528BFEE2B9535C55E4461079118B",
    "9D3881EBC749FE29BAD3B5ACDD3C56866564C2835C3BFF39489877AB51264CFC618BC100202AE497D9D25B075399B507",
    },
    {
    "39402006196394479212279040100143613805079739270465446667946905279627659399113263569398956308152294913554433653942635",
    "1692778EA596E0BE75114297A6FA383445BF227FBE58190A900C3C73256F11FB5A3258D6F403D5ECE6E9B269D822C87D",
    "232DC9A8FF2BEF957CAC7745C24702F1DDAAB52392ADE32B42E3CF3D13F113E594521E15322E8F729095405CFDD4F52D",
    },
    {
    "39402006196394479212279040100143613805079739270465446667946905279627659399113263569398956308152294913554433653942636",
    "283C1D7365CE4788F29F8EBF234EDFFEAD6FE997FBEA5FFA2D58CC9DFA7B1C508B05526F55B9EBB2040F05B48FB6D0E1",
    "6B8A366F9E1BE47745AD102473E96FB8E59E2798128668D62636D32FE242DDA8CF27B120CD5870619B99B3263AED1073",
    },
    {
    "39402006196394479212279040100143613805079739270465446667946905279627659399113263569398956308152294913554433653942637",
    "627BE1ACD064D2B2226FE0D26F2D15D3C33EBCBB7F0F5DA51CBD41F26257383021317D7202FF30E50937F0854E35C5DF",
    "F68995B34C074E3DE41922593EB0EA8A4D36ACAD9BB088B36679B09EC8EABBE8FB3BA4717B1E9ACEE8CC5BF82C0F06CB",
    },
    {
    "39402006196394479212279040100143613805079739270465446667946905279627659399113263569398956308152294913554433653942638",
    "11DE24A2C251C777573CAC5EA025E467F208E51DBFF98FC54F6661CBE56583B037882F4A1CA297E60ABCDBC3836D84BC",
    "705969388BBF06D2F0A7C816F5FF183AD7B4BB88AB2A211773679ACC496FE513CE889791F51704CCE7BBEB55193E8EC5",
    },
    {
    "39402006196394479212279040100143613805079739270465446667946905279627659399113263569398956308152294913554433653942639",
    "138251CD52AC9298C1C8AAD977321DEB97E709BD0B4CA0ACA55DC8AD51DCFC9D1589A1597E3A5120E1EFD631C63E1835",
    "35351D679659D1E9CE175D7E7E54A99E923BA26E7543F60C54F19C3086D55B22128C7840C8445A96AB60E3FE4D8F1298",
    },
    {
    "39402006196394479212279040100143613805079739270465446667946905279627659399113263569398956308152294913554433653942640",
    "077A41D4606FFA1464793C7E5FDC7D98CB9D3910202DCD06BEA4F240D3566DA6B408BBAE5026580D02D7E5C70500C831",
    "366A0835F4F3BD7C82F44169FD5603667ADF4BE37AEEA55A0897B3F123EEE1523DB542931B4A2D6749A0D7A0F5D0E20E",
    },
    {
    "39402006196394479212279040100143613805079739270465446667946905279627659399113263569398956308152294913554433653942641",
    "08D999057BA3D2D969260045C55B97F089025959A6F434D651D207D19FB96E9E4FE0E86EBE0E64F85B96A9C75295DF61",
    "717F0E05A4E4C312484017200292458B4D8A278A43933BC16FB1AFA0DA954BD9A002BC15B2C61DD29EAFE190F56BF17F",
    },
    {
    "39402006196394479212279040100143613805079739270465446667946905279627659399113263569398956308152294913554433653942642",
    "AA87CA22BE8B05378EB1C71EF320AD746E1D3B628BA79B9859F741E082542A385502F25DBF55296C3A545E3872760AB7",
    "C9E821B569D9D390A26167406D6D23D6070BE242D765EB831625CEEC4A0F473EF59F4E30E2817E6285BCE2846F15F1A0",
    }
  };
  generic_secp_tests(&dsk_tls_ecprime_group_secp384r1,
                     DSK_N_ELEMENTS (tests),
                     tests);
}
static void
test_secp521r1 (void)
{
  static struct TestEC tests[] = {
    {
    "1",
    "000000C6858E06B70404E9CD9E3ECB662395B4429C648139053FB521F828AF606B4D3DBAA14B5E77EFE75928FE1DC127A2FFA8DE3348B3C1856A429BF97E7E31C2E5BD66",
    "0000011839296A789A3BC0045C8A5FB42C7D1BD998F54449579B446817AFBD17273E662C97EE72995EF42640C550B9013FAD0761353C7086A272C24088BE94769FD16650",
    },
    {
    "2",
    "000000433C219024277E7E682FCB288148C282747403279B1CCC06352C6E5505D769BE97B3B204DA6EF55507AA104A3A35C5AF41CF2FA364D60FD967F43E3933BA6D783D",
    "000000F4BB8CC7F86DB26700A7F3ECEEEED3F0B5C6B5107C4DA97740AB21A29906C42DBBB3E377DE9F251F6B93937FA99A3248F4EAFCBE95EDC0F4F71BE356D661F41B02",
    },
    {
    "3",
    "000001A73D352443DE29195DD91D6A64B5959479B52A6E5B123D9AB9E5AD7A112D7A8DD1AD3F164A3A4832051DA6BD16B59FE21BAEB490862C32EA05A5919D2EDE37AD7D",
    "0000013E9B03B97DFA62DDD9979F86C6CAB814F2F1557FA82A9D0317D2F8AB1FA355CEEC2E2DD4CF8DC575B02D5ACED1DEC3C70CF105C9BC93A590425F588CA1EE86C0E5",
    },
    {
    "4",
    "00000035B5DF64AE2AC204C354B483487C9070CDC61C891C5FF39AFC06C5D55541D3CEAC8659E24AFE3D0750E8B88E9F078AF066A1D5025B08E5A5E2FBC87412871902F3",
    "00000082096F84261279D2B673E0178EB0B4ABB65521AEF6E6E32E1B5AE63FE2F19907F279F283E54BA385405224F750A95B85EEBB7FAEF04699D1D9E21F47FC346E4D0D",
    },
    {
    "5",
    "000000652BF3C52927A432C73DBC3391C04EB0BF7A596EFDB53F0D24CF03DAB8F177ACE4383C0C6D5E3014237112FEAF137E79A329D7E1E6D8931738D5AB5096EC8F3078",
    "0000015BE6EF1BDD6601D6EC8A2B73114A8112911CD8FE8E872E0051EDD817C9A0347087BB6897C9072CF374311540211CF5FF79D1F007257354F7F8173CC3E8DEB090CB",
    },
    {
    "6",
    "000001EE4569D6CDB59219532EFF34F94480D195623D30977FD71CF3981506ADE4AB01525FBCCA16153F7394E0727A239531BE8C2F66E95657F380AE23731BEDF79206B9",
    "000001DE0255AD0CC64F586AE2DD270546E3B1112AABBB73DA5A808E7240A926201A8A96CAB72D0E56648C9DF96C984DE274F2203DC7B8B55CA0DADE1EACCD7858D44F17",
    },
    {
    "7",
    "00000056D5D1D99D5B7F6346EEB65FDA0B073A0C5F22E0E8F5483228F018D2C2F7114C5D8C308D0ABFC698D8C9A6DF30DCE3BBC46F953F50FDC2619A01CEAD882816ECD4",
    "0000003D2D1B7D9BAAA2A110D1D8317A39D68478B5C582D02824F0DD71DBD98A26CBDE556BD0F293CDEC9E2B9523A34591CE1A5F9E76712A5DDEFC7B5C6B8BC90525251B",
    },
    {
    "8",
    "0000000822C40FB6301F7262A8348396B010E25BD4E29D8A9B003E0A8B8A3B05F826298F5BFEA5B8579F49F08B598C1BC8D79E1AB56289B5A6F4040586F9EA54AA78CE68",
    "0000016331911D5542FC482048FDAB6E78853B9A44F8EDE9E2C0715B5083DE610677A8F189E9C0AA5911B4BFF0BA0DF065C578699F3BA940094713538AD642F11F17801C",
    },
    {
    "9",
    "000001585389E359E1E21826A2F5BF157156D488ED34541B988746992C4AB145B8C6B6657429E1396134DA35F3C556DF725A318F4F50BABD85CD28661F45627967CBE207",
    "0000002A2E618C9A8AEDF39F0B55557A27AE938E3088A654EE1CEBB6C825BA263DDB446E0D69E5756057AC840FF56ECF4ABFD87D736C2AE928880F343AA0EA86B9AD2A4E",
    },
    {
    "10",
    "00000190EB8F22BDA61F281DFCFE7BB6721EC4CD901D879AC09AC7C34A9246B11ADA8910A2C7C178FCC263299DAA4DA9842093F37C2E411F1A8E819A87FF09A04F2F3320",
    "000001EB5D96B8491614BA9DBAEAB3B0CA2BA760C2EEB2144251B20BA97FD78A62EF62D2BF5349D44D9864BB536F6163DC57EBEFF3689639739FAA172954BC98135EC759",
    },
    {
    "11",
    "0000008A75841259FDEDFF546F1A39573B4315CFED5DC7ED7C17849543EF2C54F2991652F3DBC5332663DA1BD19B1AEBE3191085015C024FA4C9A902ECC0E02DDA0CDB9A",
    "00000096FB303FCBBA2129849D0CA877054FB2293ADD566210BD0493ED2E95D4E0B9B82B1BC8A90E8B42A4AB3892331914A95336DCAC80E3F4819B5D58874F92CE48C808",
    },
    {
    "12",
    "000001C0D9DCEC93F8221C5DE4FAE9749C7FDE1E81874157958457B6107CF7A5967713A644E90B7C3FB81B31477FEE9A60E938013774C75C530928B17BE69571BF842D8C",
    "0000014048B5946A4927C0FE3CE1D103A682CA4763FE65AB71494DA45E404ABF6A17C097D6D18843D86FCDB6CC10A6F951B9B630884BA72224F5AE6C79E7B1A3281B17F0",
    },
    {
    "13",
    "0000007E3E98F984C396AD9CD7865D2B4924861A93F736CDE1B4C2384EEDD2BEAF5B866132C45908E03C996A3550A5E79AB88EE94BEC3B00AB38EFF81887848D32FBCDA7",
    "00000108EE58EB6D781FEDA91A1926DAA3ED5A08CED50A386D5421C69C7A67AE5C1E212AC1BD5D5838BC763F26DFDD351CBFBBC36199EAAF9117E9F7291A01FB022A71C9",
    },
    {
    "14",
    "000001875BC7DC551B1B65A9E1B8CCFAAF84DED1958B401494116A2FD4FB0BABE0B3199974FC06C8B897222D79DF3E4B7BC744AA6767F6B812EFBF5D2C9E682DD3432D74",
    "0000005CA4923575DACB5BD2D66290BBABB4BDFB8470122B8E51826A0847CE9B86D7ED62D07781B1B4F3584C11E89BF1D133DC0D5B690F53A87C84BE41669F852700D54A",
    },
    {
    "15",
    "0000006B6AD89ABCB92465F041558FC546D4300FB8FBCC30B40A0852D697B532DF128E11B91CCE27DBD00FFE7875BD1C8FC0331D9B8D96981E3F92BDE9AFE337BCB8DB55",
    "000001B468DA271571391D6A7CE64D2333EDBF63DF0496A9BAD20CBA4B62106997485ED57E9062C899470A802148E2232C96C99246FD90CC446ABDD956343480A1475465",
    },
    {
    "16",
    "000001D17D10D8A89C8AD05DDA97DA26AC743B0B2A87F66192FD3F3DD632F8D20B188A52943FF18861CA00A0E5965DA7985630DF0DBF5C8007DCDC533A6C508F81A8402F",
    "0000007A37343C582D77001FC714B18D3D3E69721335E4C3B800D50EC7CA30C94B6B82C1C182E1398DB547AA0B3075AC9D9988529E3004D28D18633352E272F89BC73ABE",
    },
    {
    "17",
    "000001B00DDB707F130EDA13A0B874645923906A99EE9E269FA2B3B4D66524F269250858760A69E674FE0287DF4E799B5681380FF8C3042AF0D1A41076F817A853110AE0",
    "00000085683F1D7DB16576DBC111D4E4AEDDD106B799534CF69910A98D68AC2B22A1323DF9DA564EF6DD0BF0D2F6757F16ADF420E6905594C2B755F535B9CB7C70E64647",
    },
    {
    "18",
    "000001BC33425E72A12779EACB2EDCC5B63D1281F7E86DBC7BF99A7ABD0CFE367DE4666D6EDBB8525BFFE5222F0702C3096DEC0884CE572F5A15C423FDF44D01DD99C61D",
    "0000010D06E999885B63535DE3E74D33D9E63D024FB07CE0D196F2552C8E4A00AC84C044234AEB201F7A9133915D1B4B45209B9DA79FE15B19F84FD135D841E2D8F9A86A",
    },
    {
    "19",
    "000000998DCCE486419C3487C0F948C2D5A1A07245B77E0755DF547EFFF0ACDB3790E7F1FA3B3096362669679232557D7A45970DFECF431E725BBDE478FF0B2418D6A19B",
    "00000137D5DA0626A021ED5CC3942497535B245D67D28AEE2B7BCF4ACC50EEE36545772773AD963FF2EB8CF9B0EC39991631C377F5A4D89EA9FBFE44A9091A695BFD0575",
    },
    {
    "20",
    "0000018BDD7F1B889598A4653DEEAE39CC6F8CC2BD767C2AB0D93FB12E968FBED342B51709506339CB1049CB11DD48B9BDB3CD5CAD792E43B74E16D8E2603BFB11B0344F",
    "000000C5AADBE63F68CA5B6B6908296959BF0AF89EE7F52B410B9444546C550952D311204DA3BDDDC6D4EAE7EDFAEC1030DA8EF837CCB22EEE9CFC94DD3287FED0990F94",
    },
    {
    "112233445566778899",
    "000001650048FBD63E8C30B305BF36BD7643B91448EF2206E8A0CA84A140789A99B0423A0A2533EA079CA7E049843E69E5FA2C25A163819110CEC1A30ACBBB3A422A40D8",
    "0000010C9C64A0E0DB6052DBC5646687D06DECE5E9E0703153EFE9CB816FE025E85354D3C5F869D6DB3F4C0C01B5F97919A5E72CEEBE03042E5AA99112691CFFC2724828",
    },
    {
    "112233445566778899112233445566778899",
    "0000017E1370D39C9C63925DAEEAC571E21CAAF60BD169191BAEE8352E0F54674443B29786243564ABB705F6FC0FE5FC5D3F98086B67CA0BE7AC8A9DEC421D9F1BC6B37F",
    "000001CD559605EAD19FBD99E83600A6A81A0489E6F20306EE0789AE00CE16A6EFEA2F42F7534186CF1C60DF230BD9BCF8CB95E5028AD9820B2B1C0E15597EE54C4614A6",
    },
    {
    "1769805277975163035253775930842367129093741786725376786007349332653323812656658291413435033257677579095366632521448854141275926144187294499863933403633025023",
    "000000B45CB84651C9D4F08858B867F82D816E84E94FE4CAE3DA5F65E420B08398D0C5BF019253A6C26D20671BDEF0B8E6C1D348A4B0734687F73AC6A4CBB2E085C68B3F",
    "000001C84942BBF538903062170A4BA8B3410D385719BA2037D29CA5248BFCBC8478220FEC79244DCD45D31885A1764DEE479CE20B12CEAB62F9001C7AA4282CE4BE7F56",
    },
    {
    "104748400337157462316262627929132596317243790506798133267698218707528750292682889221414310155907963824712114916552440160880550666043997030661040721887239",
    "000001CCEF4CDA108CEBE6568820B54A3CA3A3997E4EF0EDA6C350E7ED3DBB1861EDD80181C650CEBE5440FEBA880F9C8A7A86F8B82659794F6F5B88E501E5DD84E65D7E",
    "000001026565F8B195D03C3F6139C3A63EAA1C29F7090AB2A8F75027939EC05109035F1B38E6C508E0C14CE53AB7E2DA33AA28140EDBF3964862FB157119517454E60F07",
    },
    {
    "6703903865078345888141381651430168039496664077350965054288133126549307058741788671148197429777343936466127575938031786147409472627479702469884214509568000",
    "000000C1002DC2884EEDADB3F9B468BBEBD55980799852C506D37271FFCD006919DB3A96DF8FE91EF6ED4B9081B1809E8F2C2B28AF5FCBF524147C73CB0B913D6FAB0995",
    "000001614E8A62C8293DD2AA6EF27D30974A4FD185019FA8EF4F982DA48698CECF706581F69EE9ED67A9C231EC9D0934D0F674646153273BCBB345E923B1EC1386A1A4AD",
    },
    {
    "1675925643682395305404517165643562251880026958780896531698856737024179880343339878336382412050263431942974939646683480906434632963478257639757341102436352",
    "0000010ED3E085ECDE1E66874286B5D5642B9D37853A026A0A025C7B84936E2ECEEC5F342E14C80C79CCF814D5AD085C5303F2823251F2B9276F88C9D7A43E387EBD87AC",
    "000001BE399A7666B29E79BBF3D277531A97CE05CAC0B49BECE4781E7AEE0D6E80FEE883C76E9F08453DC1ADE4E49300F3D56FEE6A1510DA1B1F12EEAA39A05AA0508119",
    },
    {
    "12785133382149415221402495202586701798620696169446772599038235721862338692190156163951558963856959059232381602864743924427451786769515154396810706943",
    "0000013070A29B059D317AF37089E40FCB135868F52290EFF3E9F3E32CDADCA18EA234D8589C665A4B8E3D0714DE004A419DEA7091A3BBA97263C438FE9413AA598FD4A5",
    "000000238A27FD9E5E7324C8B538EF2E334B71AC2611A95F42F4F2544D8C4A65D2A32A8BAFA15EFD4FC2BD8AB2B0C51F65B680879589F4D5FE8A84CEB17A2E8D3587F011",
    },
    {
    "214524875832249255872206855495734426889477529336261655255492425273322727861341825677722947375406711676372335314043071600934941615185418540320233184489636351",
    "000001A3D88799878EC74E66FF1AD8C7DFA9A9B4445A17F0810FF8189DD27AE3B6C580D352476DBDAEB08D7DA0DE3866F7C7FDBEBB8418E19710F1F7AFA88C22280B1404",
    "000000B39703D2053EC7B8812BDFEBFD81B4CB76F245FE535A1F1E46801C35DE03C15063A99A203981529C146132863CA0E68544D0F0A638D8A2859D82B4DD266F27C3AE",
    },
    {
    "51140486275567859131139077890835526884648461857823088348651153840508287621366854506831244746531272246620295123104269565867055949378266395604768784399",
    "000001D16B4365DEFE6FD356DC1F31727AF2A32C7E86C5AE87ED2950A08BC8653F203C7F7860E80F95AA27C93EA76E8CD094127B15ED42CC5F96DC0A0F9A1C1E31D0D526",
    "0000006E3710A0F9366E0BB8A14FFE8EBC2722EECF4A123EC9BA98DCCCA335D6FAFD289DC69FD90903C9AC982FEB46DF93F03A7C8C9549D32C1C386D17F37340E63822A8",
    },
    {
    "6651529716025206881035279952881520627841152247212784520914425039312606120198879080839643311347169019249080198239408356563413447402270445462102068592377843",
    "000001B1220F67C985E9FC9C588C0C86BB16E6FE4CC11E168A98D701AE4670724B3D030ED9965FADF4207C7A1BE9BE0F40DEF2BBFFF0C7EABCB5B42526CE1D3CAA468F52",
    "0000006CDAD2860F6D2C37159A5A866D11605F2E7D87430DCFE6E6816AB6423CD9003CA6F2527B9C2A2483C541D456C963D18A0D2A46E158CB2A44C0BF42D562881FB748",
    },
    {
    "3224551824613232232537680077946818660156835288778087344805370397811379731631671254853846826682273677870214778462237171365140390183770226853329363961324241919",
    "000000F25E545213C8C074BE38A0612EA9B66336B14A874372548D9716392DFA31CD0D13E94F86CD48B8D43B80B5299144E01245C873B39F6AC6C4FB397746AF034AD67C",
    "000001733ABB21147CC27E35F41FAF40290AFD1EEB221D983FFABBD88E5DC8776450A409EACDC1BCA2B9F517289C68645BB96781808FEAE42573C2BB289F16E2AECECE17",
    },
    {
    "12486613128442885430380874043991285080254917488396284953815149251315412600634581539066663092297612040669978017623587752845409653167277021864132608",
    "00000172CD22CBE0634B6BFEE24BB1D350F384A945ED618ECAD48AADC6C1BC0DCC107F0FFE9FE14DC929F90153F390C25BE5D3A73A56F9ACCB0C72C768753869732D0DC4",
    "000000D249CFB570DA4CC48FB5426A928B43D7922F787373B6182408FBC71706E7527E8414C79167F3C999FF58DE352D238F1FE7168C658D338F72696F2F889A97DE23C5",
    },
    {
    "6864797660130609714981900799081393217269435300143305409394463459185543183397655394245057746333217197532963996371363321113864768612440380340372808892707005429",
    "0000018BDD7F1B889598A4653DEEAE39CC6F8CC2BD767C2AB0D93FB12E968FBED342B51709506339CB1049CB11DD48B9BDB3CD5CAD792E43B74E16D8E2603BFB11B0344F",
    "0000013A552419C09735A49496F7D696A640F50761180AD4BEF46BBBAB93AAF6AD2CEEDFB25C4222392B1518120513EFCF257107C8334DD11163036B22CD78012F66F06B",
    },
    {
    "6864797660130609714981900799081393217269435300143305409394463459185543183397655394245057746333217197532963996371363321113864768612440380340372808892707005430",
    "000000998DCCE486419C3487C0F948C2D5A1A07245B77E0755DF547EFFF0ACDB3790E7F1FA3B3096362669679232557D7A45970DFECF431E725BBDE478FF0B2418D6A19B",
    "000000C82A25F9D95FDE12A33C6BDB68ACA4DBA2982D7511D48430B533AF111C9ABA88D88C5269C00D1473064F13C666E9CE3C880A5B2761560401BB56F6E596A402FA8A",
    },
    {
    "6864797660130609714981900799081393217269435300143305409394463459185543183397655394245057746333217197532963996371363321113864768612440380340372808892707005431",
    "000001BC33425E72A12779EACB2EDCC5B63D1281F7E86DBC7BF99A7ABD0CFE367DE4666D6EDBB8525BFFE5222F0702C3096DEC0884CE572F5A15C423FDF44D01DD99C61D",
    "000000F2F9166677A49CACA21C18B2CC2619C2FDB04F831F2E690DAAD371B5FF537B3FBBDCB514DFE0856ECC6EA2E4B4BADF646258601EA4E607B02ECA27BE1D27065795",
    },
    {
    "6864797660130609714981900799081393217269435300143305409394463459185543183397655394245057746333217197532963996371363321113864768612440380340372808892707005432",
    "000001B00DDB707F130EDA13A0B874645923906A99EE9E269FA2B3B4D66524F269250858760A69E674FE0287DF4E799B5681380FF8C3042AF0D1A41076F817A853110AE0",
    "0000017A97C0E2824E9A89243EEE2B1B51222EF94866ACB30966EF56729753D4DD5ECDC20625A9B10922F40F2D098A80E9520BDF196FAA6B3D48AA0ACA4634838F19B9B8",
    },
    {
    "6864797660130609714981900799081393217269435300143305409394463459185543183397655394245057746333217197532963996371363321113864768612440380340372808892707005433",
    "000001D17D10D8A89C8AD05DDA97DA26AC743B0B2A87F66192FD3F3DD632F8D20B188A52943FF18861CA00A0E5965DA7985630DF0DBF5C8007DCDC533A6C508F81A8402F",
    "00000185C8CBC3A7D288FFE038EB4E72C2C1968DECCA1B3C47FF2AF13835CF36B4947D3E3E7D1EC6724AB855F4CF8A53626677AD61CFFB2D72E79CCCAD1D8D076438C541",
    },
    {
    "6864797660130609714981900799081393217269435300143305409394463459185543183397655394245057746333217197532963996371363321113864768612440380340372808892707005434",
    "0000006B6AD89ABCB92465F041558FC546D4300FB8FBCC30B40A0852D697B532DF128E11B91CCE27DBD00FFE7875BD1C8FC0331D9B8D96981E3F92BDE9AFE337BCB8DB55",
    "0000004B9725D8EA8EC6E2958319B2DCCC12409C20FB6956452DF345B49DEF9668B7A12A816F9D3766B8F57FDEB71DDCD369366DB9026F33BB954226A9CBCB7F5EB8AB9A",
    },
    {
    "6864797660130609714981900799081393217269435300143305409394463459185543183397655394245057746333217197532963996371363321113864768612440380340372808892707005435",
    "000001875BC7DC551B1B65A9E1B8CCFAAF84DED1958B401494116A2FD4FB0BABE0B3199974FC06C8B897222D79DF3E4B7BC744AA6767F6B812EFBF5D2C9E682DD3432D74",
    "000001A35B6DCA8A2534A42D299D6F44544B42047B8FEDD471AE7D95F7B831647928129D2F887E4E4B0CA7B3EE17640E2ECC23F2A496F0AC57837B41BE99607AD8FF2AB5",
    },
    {
    "6864797660130609714981900799081393217269435300143305409394463459185543183397655394245057746333217197532963996371363321113864768612440380340372808892707005436",
    "0000007E3E98F984C396AD9CD7865D2B4924861A93F736CDE1B4C2384EEDD2BEAF5B866132C45908E03C996A3550A5E79AB88EE94BEC3B00AB38EFF81887848D32FBCDA7",
    "000000F711A7149287E01256E5E6D9255C12A5F7312AF5C792ABDE3963859851A3E1DED53E42A2A7C74389C0D92022CAE340443C9E6615506EE81608D6E5FE04FDD58E36",
    },
    {
    "6864797660130609714981900799081393217269435300143305409394463459185543183397655394245057746333217197532963996371363321113864768612440380340372808892707005437",
    "000001C0D9DCEC93F8221C5DE4FAE9749C7FDE1E81874157958457B6107CF7A5967713A644E90B7C3FB81B31477FEE9A60E938013774C75C530928B17BE69571BF842D8C",
    "000000BFB74A6B95B6D83F01C31E2EFC597D35B89C019A548EB6B25BA1BFB54095E83F68292E77BC2790324933EF5906AE4649CF77B458DDDB0A519386184E5CD7E4E80F",
    },
    {
    "6864797660130609714981900799081393217269435300143305409394463459185543183397655394245057746333217197532963996371363321113864768612440380340372808892707005438",
    "0000008A75841259FDEDFF546F1A39573B4315CFED5DC7ED7C17849543EF2C54F2991652F3DBC5332663DA1BD19B1AEBE3191085015C024FA4C9A902ECC0E02DDA0CDB9A",
    "0000016904CFC03445DED67B62F35788FAB04DD6C522A99DEF42FB6C12D16A2B1F4647D4E43756F174BD5B54C76DCCE6EB56ACC923537F1C0B7E64A2A778B06D31B737F7",
    },
    {
    "6864797660130609714981900799081393217269435300143305409394463459185543183397655394245057746333217197532963996371363321113864768612440380340372808892707005439",
    "00000190EB8F22BDA61F281DFCFE7BB6721EC4CD901D879AC09AC7C34A9246B11ADA8910A2C7C178FCC263299DAA4DA9842093F37C2E411F1A8E819A87FF09A04F2F3320",
    "00000014A26947B6E9EB456245154C4F35D4589F3D114DEBBDAE4DF4568028759D109D2D40ACB62BB2679B44AC909E9C23A814100C9769C68C6055E8D6AB4367ECA138A6",
    },
    {
    "6864797660130609714981900799081393217269435300143305409394463459185543183397655394245057746333217197532963996371363321113864768612440380340372808892707005440",
    "000001585389E359E1E21826A2F5BF157156D488ED34541B988746992C4AB145B8C6B6657429E1396134DA35F3C556DF725A318F4F50BABD85CD28661F45627967CBE207",
    "000001D5D19E736575120C60F4AAAA85D8516C71CF7759AB11E3144937DA45D9C224BB91F2961A8A9FA8537BF00A9130B54027828C93D516D777F0CBC55F15794652D5B1",
    },
    {
    "6864797660130609714981900799081393217269435300143305409394463459185543183397655394245057746333217197532963996371363321113864768612440380340372808892707005441",
    "0000000822C40FB6301F7262A8348396B010E25BD4E29D8A9B003E0A8B8A3B05F826298F5BFEA5B8579F49F08B598C1BC8D79E1AB56289B5A6F4040586F9EA54AA78CE68",
    "0000009CCE6EE2AABD03B7DFB7025491877AC465BB0712161D3F8EA4AF7C219EF988570E76163F55A6EE4B400F45F20F9A3A879660C456BFF6B8ECAC7529BD0EE0E87FE3",
    },
    {
    "6864797660130609714981900799081393217269435300143305409394463459185543183397655394245057746333217197532963996371363321113864768612440380340372808892707005442",
    "00000056D5D1D99D5B7F6346EEB65FDA0B073A0C5F22E0E8F5483228F018D2C2F7114C5D8C308D0ABFC698D8C9A6DF30DCE3BBC46F953F50FDC2619A01CEAD882816ECD4",
    "000001C2D2E48264555D5EEF2E27CE85C6297B874A3A7D2FD7DB0F228E242675D93421AA942F0D6C321361D46ADC5CBA6E31E5A061898ED5A2210384A3947436FADADAE4",
    },
    {
    "6864797660130609714981900799081393217269435300143305409394463459185543183397655394245057746333217197532963996371363321113864768612440380340372808892707005443",
    "000001EE4569D6CDB59219532EFF34F94480D195623D30977FD71CF3981506ADE4AB01525FBCCA16153F7394E0727A239531BE8C2F66E95657F380AE23731BEDF79206B9",
    "00000021FDAA52F339B0A7951D22D8FAB91C4EEED554448C25A57F718DBF56D9DFE575693548D2F1A99B7362069367B21D8B0DDFC238474AA35F2521E1533287A72BB0E8",
    },
    {
    "6864797660130609714981900799081393217269435300143305409394463459185543183397655394245057746333217197532963996371363321113864768612440380340372808892707005444",
    "000000652BF3C52927A432C73DBC3391C04EB0BF7A596EFDB53F0D24CF03DAB8F177ACE4383C0C6D5E3014237112FEAF137E79A329D7E1E6D8931738D5AB5096EC8F3078",
    "000000A41910E42299FE291375D48CEEB57EED6EE327017178D1FFAE1227E8365FCB8F7844976836F8D30C8BCEEABFDEE30A00862E0FF8DA8CAB0807E8C33C17214F6F34",
    },
    {
    "6864797660130609714981900799081393217269435300143305409394463459185543183397655394245057746333217197532963996371363321113864768612440380340372808892707005445",
    "00000035B5DF64AE2AC204C354B483487C9070CDC61C891C5FF39AFC06C5D55541D3CEAC8659E24AFE3D0750E8B88E9F078AF066A1D5025B08E5A5E2FBC87412871902F3",
    "0000017DF6907BD9ED862D498C1FE8714F4B5449AADE5109191CD1E4A519C01D0E66F80D860D7C1AB45C7ABFADDB08AF56A47A114480510FB9662E261DE0B803CB91B2F2",
    },
    {
    "6864797660130609714981900799081393217269435300143305409394463459185543183397655394245057746333217197532963996371363321113864768612440380340372808892707005446",
    "000001A73D352443DE29195DD91D6A64B5959479B52A6E5B123D9AB9E5AD7A112D7A8DD1AD3F164A3A4832051DA6BD16B59FE21BAEB490862C32EA05A5919D2EDE37AD7D",
    "000000C164FC4682059D2226686079393547EB0D0EAA8057D562FCE82D0754E05CAA3113D1D22B30723A8A4FD2A5312E213C38F30EFA36436C5A6FBDA0A7735E11793F1A",
    },
    {
    "6864797660130609714981900799081393217269435300143305409394463459185543183397655394245057746333217197532963996371363321113864768612440380340372808892707005447",
    "000000433C219024277E7E682FCB288148C282747403279B1CCC06352C6E5505D769BE97B3B204DA6EF55507AA104A3A35C5AF41CF2FA364D60FD967F43E3933BA6D783D",
    "0000010B44733807924D98FF580C1311112C0F4A394AEF83B25688BF54DE5D66F93BD2444C1C882160DAE0946C6C805665CDB70B1503416A123F0B08E41CA9299E0BE4FD",
    },
    {
    "6864797660130609714981900799081393217269435300143305409394463459185543183397655394245057746333217197532963996371363321113864768612440380340372808892707005448",
    "000000C6858E06B70404E9CD9E3ECB662395B4429C648139053FB521F828AF606B4D3DBAA14B5E77EFE75928FE1DC127A2FFA8DE3348B3C1856A429BF97E7E31C2E5BD66",
    "000000E7C6D6958765C43FFBA375A04BD382E426670ABBB6A864BB97E85042E8D8C199D368118D66A10BD9BF3AAF46FEC052F89ECAC38F795D8D3DBF77416B89602E99AF",
    },
  };
  generic_secp_tests(&dsk_tls_ecprime_group_secp521r1,
                     DSK_N_ELEMENTS (tests),
                     tests);
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
  { "Curve 448", test_curve448 },
  { "HMAC Tests (from RFC 2104 Appendix)", test_hmac },
  { "HMAC Tests (SHA-256, from RFC 4231)", test_hmac_rfc4231 },
  { "HKDF Tests (from RFC 5869 Appendix A)", test_hkdf },
  { "Chacha-20 Tests (from RFC 8439 2.3.2, 2.4.2)", test_chacha20 },
  { "Poly1305 MAC Tests (from RFC 8439 2.5.2)", test_poly1305_mac },
  { "Poly1305 key-gen Tests (from RFC 8439 2.6.2)", test_poly1305_keygen },
  { "Chacha20-Poly1305 AEAD test (from RFC 8439 2.8.2)", test_chachapoly_aead },
  { "Elliptic-Curves over F_p", test_ecprime },
  { "SECP256R1", test_secp256r1 },
  { "SECP384R1", test_secp384r1 },
  { "SECP521R1", test_secp521r1 },
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
