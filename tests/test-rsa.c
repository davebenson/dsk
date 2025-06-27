#include "../dsk.h"
#include <string.h>
#include <alloca.h>
#include <stdio.h>
#include <stdlib.h>

static bool cmdline_verbose = false;

static void
hex_to_bignum (const char *hex,
               unsigned    bignum_len,
               uint32_t   *bignum_out)
{
  const char *at = strchr(hex, 0);
  while (at - 8 >= hex && bignum_len > 0)
    {
      char hbuf[9];
      memcpy (hbuf, at - 8, 8);
      hbuf[8] = 0;
      *bignum_out++ = (uint32_t) strtoul (hbuf, NULL, 16);
      bignum_len--;
      at -= 8;
    }

  if (at > hex && bignum_len > 0)
    {
      char hbuf[9];
      memcpy (hbuf, hex, at - hex);
      hbuf[at - hex] = 0;
      *bignum_out++ = (uint32_t) strtoul (hbuf, NULL, 16);
      bignum_len--;
      at = hex;
    }

  while (bignum_len-- > 0)
    *bignum_out++ = 0;
}

static void
hex_to_binary (const char *hex,
               unsigned    len,
               uint8_t    *data)
{
  while (len-- > 0)
    {
      char h[3] = {hex[0], hex[1], 0};
      *data++ = strtoul (h, NULL, 16);
      hex += 2;
    }
}
static void
perform_raw (unsigned    mod,
             const char *n,
             const char *e,
                 const char *d,
                 const char *msg,
                 const char *ciphertext)
{
  assert (mod % 8 == 0);
  assert (mod % DSK_WORD_BIT_SIZE == 0);
  unsigned len = mod / DSK_WORD_BIT_SIZE;
  uint32_t *rsa_modulus = alloca(DSK_WORD_SIZE * len);
  hex_to_bignum (n, len, rsa_modulus);
  uint32_t *exponent = alloca(DSK_WORD_SIZE * len);
  hex_to_bignum (e, len, exponent);
  uint32_t *private_exponent = alloca(DSK_WORD_SIZE * len);
  hex_to_bignum (d, len, private_exponent);
  uint32_t *input = alloca(DSK_WORD_SIZE * len);
  hex_to_bignum (msg, len, input);
  uint32_t *output = alloca(DSK_WORD_SIZE * len);
  DskRSAPrivateKey privkey;
  memset (&privkey, 0, sizeof(privkey));
  privkey.version = 3;
  privkey.modulus_length_bytes = mod / 8;
  privkey.modulus_len = len;
  privkey.modulus = rsa_modulus;
  privkey.public_exp_len = len;
  privkey.public_exp = exponent;
  privkey.private_exp_len = len;
  privkey.private_exp = private_exponent;
  if (!dsk_rsa_private_key_crypt (&privkey, input, output))
    dsk_die ("private-key-crypt failed");
  uint32_t *expected = alloca(DSK_WORD_SIZE * len);
  hex_to_bignum (ciphertext, len, expected);
  assert (dsk_tls_bignums_equal (len, expected, output));

  DskRSAPublicKey pubkey;
  memset (&pubkey, 0, sizeof (pubkey));
  pubkey.modulus_length_bytes = mod / 8;
  pubkey.modulus_len = len;
  pubkey.modulus = rsa_modulus;
  pubkey.public_exp_len = len;
  pubkey.public_exp = exponent;
  uint32_t *orig = alloca(DSK_WORD_SIZE * len);
  if (!dsk_rsa_public_key_crypt (&pubkey, output, orig))
    dsk_die ("public-key-crypt failed");
  assert (dsk_tls_bignums_equal (len, input, orig));
}


static void
perform_pkcs1_5 (unsigned    mod,
                 const char *n,
                 const char *e,
                 const char *d,
                 const char *msg,
                 DskChecksumType *checksum_type,
                 const char *S)
{
  DskRSAPrivateKey pk;
  memset(&pk, 0, sizeof(DskRSAPrivateKey));
  assert(mod % 8 == 0);
  pk.version = 3;
  pk.modulus_length_bytes = mod / 8;
  unsigned mod_len = (pk.modulus_length_bytes + 3) / 4;
  pk.modulus_len = mod_len;
  pk.modulus = alloca(4 * pk.modulus_len);
  hex_to_bignum (n, pk.modulus_len, pk.modulus);
  pk.public_exp_len = mod_len;
  pk.public_exp = alloca(4 * pk.public_exp_len);
  hex_to_bignum (e, pk.public_exp_len, pk.public_exp);
  pk.private_exp_len = mod_len;
  pk.private_exp = alloca(4 * mod_len);
  hex_to_bignum (e, pk.private_exp_len, pk.private_exp);

  
  unsigned msg_len = strlen(msg);
  assert(msg_len % 2 == 0);
  msg_len /= 2;
  uint8_t *msg_binary = alloca(msg_len / 2);
  hex_to_binary(msg, msg_len, msg_binary);
  
  uint8_t *S_binary = alloca(msg_len);
  hex_to_binary(S, msg_len, S_binary);
  assert (msg_len == pk.modulus_length_bytes);

  uint8_t *S_computed = alloca(pk.modulus_length_bytes);

  dsk_rsassa_pkcs1_5_sign (&pk,
                           checksum_type,
                           msg_len, msg_binary,
                           S_computed);
  assert(memcmp (S_computed, S_binary, msg_len) == 0);

  DskRSAPublicKey pubkey;
  memset(&pubkey, 0, sizeof(DskRSAPublicKey));

  if (!dsk_rsassa_pkcs1_5_verify (&pubkey,
                                  checksum_type,
                                  msg_len, msg_binary,
                                  S_computed))
    dsk_die("dsk_rsassa_pkcs1_5_verify failed");
}

static void
test_pkcs15_mod1024 (void)
{
  unsigned mod = 1024;
  const char *n = "c8a2069182394a2ab7c3f4190c15589c56a2d4bc42dca675b34cc950e2466304"
                  "8441e8aa593b2bc59e198b8c257e882120c62336e5cc745012c7ffb063eebe53"
                  "f3c6504cba6cfe51baa3b6d1074b2f398171f4b1982f4d65caf882ea4d56f32a"
                  "b57d0c44e6ad4e9cf57a4339eb6962406e350c1b15397183fbf1f0353c9fc991";
  const char *e = "0000000000000000000000000000000000000000000000000000000000000000"
                  "0000000000000000000000000000000000000000000000000000000000000000"
                  "0000000000000000000000000000000000000000000000000000000000000000"
                  "0000000000000000000000000000000000000000000000000000000000010001";
  const char *d = "5dfcb111072d29565ba1db3ec48f57645d9d8804ed598a4d470268a89067a2c9"
                  "21dff24ba2e37a3ce834555000dc868ee6588b7493303528b1b3a94f0b71730c"
                  "f1e86fca5aeedc3afa16f65c0189d810ddcd81049ebbd0391868c50edec958b3"
                  "a2aaeff6a575897e2f20a3ab5455c1bfa55010ac51a7799b1ff8483644a3d425";

  // Test from line 13.
  perform_pkcs1_5(mod, n, e, d, 
                  "e8312742ae23c456ef28a23142c4490895832765dadce02afe5be5d31b0048fb"
                  "eee2cf218b1747ad4fd81a2e17e124e6af17c3888e6d2d40c00807f423a233ca"
                  "d62ce9eaefb709856c94af166dba08e7a06965d7fc0d8e5cb26559c460e47bc0"
                  "88589d2242c9b3e62da4896fab199e144ec136db8d84ab84bcba04ca3b90c8e5",
                  &dsk_checksum_type_sha1,
                  "28928e19eb86f9c00070a59edf6bf8433a45df495cd1c73613c2129840f48c4a"
                  "2c24f11df79bc5c0782bcedde97dbbb2acc6e512d19f085027cd575038453d04"
                  "905413e947e6e1dddbeb3535cdb3d8971fe0200506941056f21243503c83eadd"
                  "e053ed866c0e0250beddd927a08212aa8ac0efd61631ef89d8d049efb36bb35f");

  // Test from line 17.
  perform_pkcs1_5(mod, n, e, d, 
                  "4c95073dac19d0256eaadff3505910e431dd50018136afeaf690b7d18069fcc9"
                  "80f6f54135c30acb769bee23a7a72f6ce6d90cbc858c86dbbd64ba48a07c6d7d"
                  "50c0e9746f97086ad6c68ee38a91bbeeeb2221aa2f2fb4090fd820d4c0ce5ff0"
                  "25ba8adf43ddef89f5f3653de15edcf3aa8038d4686960fc55b2917ec8a8f9a8",
                  &dsk_checksum_type_sha1,
                  "53ab600a41c71393a271b0f32f521963087e56ebd7ad040e4ee8aa7c450ad18a"
                  "c3c6a05d4ae8913e763cfe9623bd9cb1eb4bed1a38200500fa7df3d95dea485f"
                  "032a0ab0c6589678f9e8391b5c2b1392997ac9f82f1d168878916aace9ac7455"
                  "808056af8155231a29f42904b7ab87a5d71ed6395ee0a9d024b0ca3d01fd7150");

  // Test from line 21.
  perform_pkcs1_5(mod, n, e, d, 
                  "e075ad4b0f9b5b20376e467a1a35e308793ba38ed983d03887b8b82eda630e68"
                  "b8618dc45b93de5555d7bcfed23756401e61f5516757de6ec3687a71755fb4a6"
                  "6cfaa3db0c9e69b631485b4c71c762eea229a0469c7357a440950792ba9cd7ae"
                  "022a36b9a923c2ebd2aa69897f4cceba0e7aee97033d03810725a9b731833f27",
                  &dsk_checksum_type_sha1,
                  "642609ce084f479271df596480252e2f892b3e7982dff95994c3eeda787f80f3"
                  "f6198bbce33ec5515378d4b571d7186078b75b43aed11d342547386c5696eb37"
                  "99a0b28475e54cd4ca7d036dcd8a11f5e10806f7d3b8cc4fcb3e93e857be9583"
                  "44a34e126809c15b3d33661cf57bf5c338f07acced60f14019335c152d86b3b2");

  // Test from line 25.
  perform_pkcs1_5(mod, n, e, d, 
                  "18500155d2e0587d152698c07ba44d4f04aa9a900b77ce6678a137b238b73b1a"
                  "ea24a409db563cf635209aea735d3b3c18d7d59fa167e760b85d95e8aa21b388"
                  "1b1b2076f9d15512ae4f3d6e9acc480ec08abbecbffe4abf0531e87d3f66de1f"
                  "13fd1aa41297ca58e87b2a56d6399a4c638df47e4e851c0ec6e6d97addcde366",
                  &dsk_checksum_type_sha1,
                  "42f3c3c75f65ad42057bfac13103837bf9f8427c6ebc22a3adf7b8e47a6857f1"
                  "cb17d2a533c0a913dd9a8bdc1786222360cbd7e64b45fcf54f5da2f34230ab48"
                  "06a087f8be47f35c4e8fee2e6aa2919a56679ce2a528a44bf818620d5b00b9ab"
                  "0e1c8d2d722b53d3a8cca35a990ed25536ea65335e8253a54a68a64a373e0ed7");

  // Test from line 29.
  perform_pkcs1_5(mod, n, e, d, 
                  "f7f79f9df2760fc83c73c7ccea7eae482dcfa5e02acf05e105db48283f440640"
                  "439a24ca3b2a482228c58f3f32c383db3c4847d4bcc615d3cac3eb2b77dd8004"
                  "5f0b7db88225ea7d4fa7e64502b29ce23053726ea00883ea5d80502509a3b2df"
                  "74d2142f6e70de22d9a134a50251e1a531798e747e9d386fe79ae1dea09e851b",
                  &dsk_checksum_type_sha1,
                  "ac2ae66bca1ec12a66a2909fe2148a1d492d1edd00063b8b33af74760dc40567"
                  "18fd5041d4dfee12bec7081ab1bab2d0eb2712f334509f6889b19d75d1fd0fc6"
                  "1bf12976109c3614c46051e2a401b20880d6e64ad6a47f23939803d138aa0a44"
                  "bc41ba63030746622248771431dff97e8a856f0b61d114f813911ee229655155");

  // Test from line 33.
  perform_pkcs1_5(mod, n, e, d, 
                  "099bf17f16bcfd4c19b34fecb4b3233c9f9f98718f67b3065d95a5f864235136"
                  "2b9009534433987f73ce86b513669736b65295350c934fd40663e24f3a103777"
                  "8a0bcd63003cb962fd99eb3393f7b2792f2083697b25f6c682f6110f162fc9f7"
                  "6e35c615148267ddff3d06cffb0e7dee5230e874a5c8adc41b75baa0be280e9c",
                  &dsk_checksum_type_sha1,
                  "3a2b7571619272b81d3562a11c644502894421583e02879f5a7759fb64ec2ab8"
                  "105f7d11947c8e4bfca87219e52987aad3b81cbd483166ed78152af24460c908"
                  "879f34c870573127e3448c8fbf43028350c975bbc3a999196a3e9961228a2bb1"
                  "5b4985e95bba970ac4ad2ac5b42ac51dbc6509effc13396693980fc89ba44c7b");

  // Test from line 37.
  perform_pkcs1_5(mod, n, e, d, 
                  "fb40a73dc82f167f9c2bf98a991ea82fdb0141dbad44871afd70f05a0e0bf9f2"
                  "6dbcbd6226afc6dc373b230445c2baf58ed9e0841fa927c8479577da4b1e61d9"
                  "5b03af31c5ac401d69c8136b6d36a1803221709b8670e55e1b5d5a8a3763700a"
                  "ae5ea6330eee2b4a191cf146784003d8ad2218a94a5f68e3600ebef23ba4cf8c",
                  &dsk_checksum_type_sha1,
                  "b10322602c284f4079e509faf3f40a3d2af3abef9f09171fdd16469d679bb9ad"
                  "c7e2acb1addb0bd5b38b5c4d986b43c79b9724f61e99b5b303630b62d0d8d5f7"
                  "6577fe7ea387710b43789ee1b35b614b691f0a27b73baf6bc3f28ec210b9d3e4"
                  "c5a2729cc1203b74ef70e315cfe5d06e040aee6b3d22d91d6e229f690a966dd9");

  // Test from line 41.
  perform_pkcs1_5(mod, n, e, d, 
                  "97e74960dbd981d46aadc021a6cf181ddde6e4cfcb4b638260c0a519c45faba2"
                  "99d0ca2e80bf50dfde8d6a42e04645dfbcd4740f3a72920e74632851d9e3d01a"
                  "785e9b497ce0b175f2cd373bd3d276d63e1b39f005c676b86b9831352cef9eda"
                  "bef8865ad722ebbe2fd3efb48759f22aea23fb1b333159a9cfc98a6dc46c5b0b",
                  &dsk_checksum_type_sha1,
                  "60ebc9e4e2e2b4fa6d31c57d0b86835e8d201d21c274cf5452cdd7ef2857dc78"
                  "0dde3526f3658c4f2c8710eaae4870d275997e5cbb268e3bd251f543b8828feb"
                  "85c211c858e47a74cb122dc17f26fe92b4afeecbf1e20bea75c794c0482aa653"
                  "2e87955dba249f0fa6562bdf8f4ccd8a63da69d1f337523f65206fb8eb163173");

  // Test from line 45.
  perform_pkcs1_5(mod, n, e, d, 
                  "95d04624b998938dd0a5ba6d7042aa88a2674dad438a0d31abb7979d8de3dea4"
                  "1e7e63587a47b59d436433dd8bb219fdf45abb9015a50b4b201161b9c2a47c30"
                  "4b80c4040fb8d1fa0c623100cded661b8eb52fa0a0d509a70f3cf4bd83047ad9"
                  "64ffee924192f28e73c63b3efd9c99c8b7a13145acc30d2dc063d80f96abe286",
                  &dsk_checksum_type_sha1,
                  "859cc4fcd1b88ccda695b12311cf8bdca3b4c135faa11f9053dc10f4bf12e5f2"
                  "179be6ab5ad90f8d115f5df795a77340e20662809fa732b92560adcffdb0ddb7"
                  "2d33811e94f854330680f2b238300995a9113a469afd9e756f649208d2942feb"
                  "ffb22e832279063ec5b57ab542d9bbc56e82cdc6a03b00d10d45801575e949e1");

  // Test from line 49.
  perform_pkcs1_5(mod, n, e, d, 
                  "207102f598ec280045be67592f5bba25ba2e2b56e0d2397cbe857cde52da8cca"
                  "83ae1e29615c7056af35e8319f2af86fdccc4434cd7707e319c9b2356659d788"
                  "67a6467a154e76b73c81260f3ab443cc039a0d42695076a79bd8ca25ebc8952e"
                  "d443c2103b2900c9f58b6a1c8a6266e43880cda93bc64d714c980cd8688e8e63",
                  &dsk_checksum_type_sha1,
                  "77f0f2a04848fe90a8eb35ab5d94cae843db61024d0167289eea92e5d1e10a52"
                  "6e420f2d334f1bf2aa7ea4e14a93a68dba60fd2ede58b794dcbd37dcb1967877"
                  "d6b67da3fdf2c0c7433e47134dde00c9c4d4072e43361a767a527675d8bda7d5"
                  "921bd483c9551950739e9b2be027df3015b61f751ac1d9f37bea3214d3c8dc96");

/// XXX: SHA224
/// XXX: SHA256
/// XXX: SHA384
/// XXX: SHA512
}

static struct 
{
  const char *name;
  void (*test)(void);
} tests[] =
{
  { "RSA Raw", test_raw_rsa },
  { "RSA PKCS1-1.5 1024-bit", test_pkcs15_mod1024 },
};

int main(int argc, char **argv)
{
  unsigned i;

  dsk_cmdline_init ("test RSA signing",
                    "Test RSA functions which are used by TLS",
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
