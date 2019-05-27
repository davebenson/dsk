#include "../dsk.h"
#include <stdio.h>
#include <string.h>

static const char rfc8448_sec3_client_hello_data[] =
  "\x01\x00\x00\xc0"                            // handshake header
  "\x03\x03\xcb\x34\xec\xb1\xe7\x81\x63\xba\x1c\x38\xc6\xda\xcb\x19\x6a\x6d\xff\xa2"
  "\x1a\x8d\x99\x12\xec\x18\xa2\xef\x62\x83\x02\x4d\xec\xe7\x00\x00\x06\x13\x01\x13"
  "\x03\x13\x02\x01\x00\x00\x91\x00\x00\x00\x0b\x00\x09\x00\x00\x06\x73\x65\x72\x76"
  "\x65\x72\xff\x01\x00\x01\x00\x00\x0a\x00\x14\x00\x12\x00\x1d\x00\x17\x00\x18\x00"
  "\x19\x01\x00\x01\x01\x01\x02\x01\x03\x01\x04\x00\x23\x00\x00\x00\x33\x00\x26\x00"
  "\x24\x00\x1d\x00\x20\x99\x38\x1d\xe5\x60\xe4\xbd\x43\xd2\x3d\x8e\x43\x5a\x7d\xba"
  "\xfe\xb3\xc0\x6e\x51\xc1\x3c\xae\x4d\x54\x13\x69\x1e\x52\x9a\xaf\x2c\x00\x2b\x00"
  "\x03\x02\x03\x04\x00\x0d\x00\x20\x00\x1e\x04\x03\x05\x03\x06\x03\x02\x03\x08\x04"
  "\x08\x05\x08\x06\x04\x01\x05\x01\x06\x01\x02\x01\x04\x02\x05\x02\x06\x02\x02\x02"
  "\x00\x2d\x00\x02\x01\x01\x00\x1c\x00\x02\x40\x01";

static const char rfc8448_sec3_server_hello_data[] =
  "\x02\x00\x00\x56"                  // handshake header
  "\x03\x03"                          // legacy_version
  "\xa6\xaf\x06\xa4\x12\x18\x60\xdc"  // random (32 bytes)
  "\x5e\x6e\x60\x24\x9c\xd3\x4c\x95"
  "\x93\x0c\x8a\xc5\xcb\x14\x34\xda"
  "\xc1\x55\x77\x2e\xd3\xe2\x69\x28"
  "\x00"                              // legacy_session_id_echo
  "\x13\x01"                          // cipher_suite
  "\x00"                              // legacy_compression_method
  "\x00\x2e\x00\x33\x00\x24\x00"
  "\x1d\x00\x20\xc9\x82\x88\x76\x11\x20\x95\xfe\x66\x76\x2b\xdb\xf7\xc6\x72\xe1\x56\xd6"
  "\xcc\x25\x3b\x83\x3d\xf1\xdd\x69\xb1\xb0\x4e\x75\x1f\x0f\x00\x2b\x00\x02\x03\x04";


static void
test_client_hello ()
{
  // RFC 8448.  Section 3.
  DskMemPool pool = DSK_MEM_POOL_STATIC_INIT;
  DskError *error = NULL;
  DskTlsHandshake *shake = dsk_tls_handshake_parse (DSK_TLS_HANDSHAKE_TYPE_CLIENT_HELLO,
                                                    sizeof (rfc8448_sec3_client_hello_data) - 4 - 1,
                                                    (uint8_t*) rfc8448_sec3_client_hello_data + 4,
                                                    &pool,
                                                    &error);
  if (shake == NULL)
    dsk_die ("error parsing client-hello: %s", error->message);
  assert(shake->type == DSK_TLS_HANDSHAKE_TYPE_CLIENT_HELLO);
  bool has_supported_groups = false;
  bool has_server_name = false;
  bool has_supported_versions = false;
  bool has_key_share = false;
  for (unsigned i = 0; i < shake->client_hello.n_extensions; i++)
    {
      DskTlsExtension *ext = shake->client_hello.extensions[i];
      switch (ext->type)
        {
        case DSK_TLS_EXTENSION_TYPE_SUPPORTED_GROUPS:
          {
            DskTlsExtension_SupportedGroups *sg = (DskTlsExtension_SupportedGroups *) ext;
            assert (sg->n_supported_groups == 9);
            assert(!has_supported_groups);

            // ensure x25519 is in the list.
            unsigned j;
            for (j = 0; j < sg->n_supported_groups; j++)
              if (sg->supported_groups[j] == DSK_TLS_NAMED_GROUP_X25519)
                break;
            assert (j < sg->n_supported_groups);
          
            has_supported_groups = true;
            break;
          }
        case DSK_TLS_EXTENSION_TYPE_SERVER_NAME:
          {
	    DskTlsExtension_ServerNameList *sn = (DskTlsExtension_ServerNameList *) ext;
            assert(!has_server_name);
	    assert(sn->n_entries == 1);
	    assert(sn->entries[0].type == DSK_TLS_EXTENSION_SERVER_NAME_TYPE_HOSTNAME);
	    assert(sn->entries[0].name_length == 6);
	    assert(strcmp (sn->entries[0].name, "server") == 0);
            has_server_name = true;
	    break;
          }
        case DSK_TLS_EXTENSION_TYPE_KEY_SHARE:
          {
            assert(!has_key_share);
	    DskTlsExtension_KeyShare *ks = (DskTlsExtension_KeyShare *) ext;
	    assert(ks->n_key_shares == 1);
	    assert(ks->key_shares[0].named_group == DSK_TLS_NAMED_GROUP_X25519);
	    assert(ks->key_shares[0].key_exchange_length == 32);
	    assert(memcmp(ks->key_shares[0].key_exchange_data,
                          "\x99\x38\x1d\xe5\x60\xe4\xbd\x43"
                          "\xd2\x3d\x8e\x43\x5a\x7d\xba\xfe"
                          "\xb3\xc0\x6e\x51\xc1\x3c\xae\x4d"
                          "\x54\x13\x69\x1e\x52\x9a\xaf\x2c", 32) == 0);
            has_key_share = true;
            break;
          }
        case DSK_TLS_EXTENSION_TYPE_SUPPORTED_VERSIONS:
          {
            assert(!has_supported_versions);
	    DskTlsExtension_SupportedVersions *sv = (DskTlsExtension_SupportedVersions *) ext;
            assert(sv->n_supported_versions == 1);
            assert(sv->supported_versions[0] == DSK_TLS_PROTOCOL_VERSION_1_3);
            has_supported_versions = true;
            break;
          }
        case DSK_TLS_EXTENSION_TYPE_SIGNATURE_ALGORITHMS:
          {
            DskTlsExtension_SignatureAlgorithms *sa = (DskTlsExtension_SignatureAlgorithms *) ext;
            assert(sa->n_schemes == 15);
            //for (unsigned i = 0; i < 15; i++)
              //printf("sig[%u] = 0x%04x... %s\n", i, sa->schemes[i], dsk_tls_signature_scheme_name (sa->schemes[i]));
            break;
          }

        case DSK_TLS_EXTENSION_TYPE_PSK_KEY_EXCHANGE_MODES:
          ///... TODO
          break;

          // optional TODO: RECORD_SIZE_LIMIT

        }
    }
  assert(has_supported_groups);
  assert(has_server_name);
  assert(has_key_share);
  assert(has_supported_versions);
}

static void
test_server_secret_calculations_1 (void)
{
  // TODO test dsk_curve25519_private_to_public
  const uint8_t *client_curve25519_private =
                 (uint8_t *)  "\x49\xaf\x42\xba\x7f\x79\x94\x85"
                              "\x2d\x71\x3e\xf2\x78\x4b\xcb\xca"
                              "\xa7\x91\x1d\xe2\x6a\xdc\x56\x42"
                              "\xcb\x63\x45\x40\xe7\xea\x50\x05";
  const uint8_t *client_curve25519_public =
                 (uint8_t *)  "\x99\x38\x1d\xe5\x60\xe4\xbd\x43"
                              "\xd2\x3d\x8e\x43\x5a\x7d\xba\xfe"
                              "\xb3\xc0\x6e\x51\xc1\x3c\xae\x4d"
                              "\x54\x13\x69\x1e\x52\x9a\xaf\x2c";
  const uint8_t *server_curve25519_private =
                 (uint8_t *)  "\xb1\x58\x0e\xea\xdf\x6d\xd5\x89"
                              "\xb8\xef\x4f\x2d\x56\x52\x57\x8c"
                              "\xc8\x10\xe9\x98\x01\x91\xec\x8d"
                              "\x05\x83\x08\xce\xa2\x16\xa2\x1e";
  const uint8_t *server_curve25519_public =
                 (uint8_t *)  "\xc9\x82\x88\x76\x11\x20\x95\xfe"
                              "\x66\x76\x2b\xdb\xf7\xc6\x72\xe1"
                              "\x56\xd6\xcc\x25\x3b\x83\x3d\xf1"
                              "\xdd\x69\xb1\xb0\x4e\x75\x1f\x0f";
  const uint8_t *expected_curve25519_shared =
                 (uint8_t *)  "\x8b\xd4\x05\x4f\xb5\x5b\x9d\x63"
                              "\xfd\xfb\xac\xf9\xf0\x4b\x9f\x0d"
                              "\x35\xe6\xd6\x3f\x53\x75\x63\xef"
                              "\xd4\x62\x72\x90\x0f\x89\x49\x2d";

  /* {server} extract secret "early". */
  uint8_t salt[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  uint8_t ikm[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  uint8_t prk[32];
  dsk_hkdf_extract (DSK_CHECKSUM_SHA256, 32, salt, 32, ikm, prk);

  assert (memcmp (prk,
              "\x33\xad\x0a\x1c\x60\x7e\xc0\x3b"
              "\x09\xe6\xcd\x98\x93\x68\x0c\xe2"
              "\x10\xad\xf3\x00\xaa\x1f\x26\x60"
              "\xe1\xb2\x2e\x10\xf1\x70\xf9\x2a",
              32) == 0);

  /* {server} derive secret for handshake "tls13 derived" */
  uint8_t transcript_hash[32];
  assert(sizeof(rfc8448_sec3_client_hello_data) - 1 == 196);
  dsk_checksum (DSK_CHECKSUM_SHA256,
                0, "",
                transcript_hash);
  printf("%02x %02x %02x %02x %02x %02x\n", transcript_hash[0],transcript_hash[1],transcript_hash[2],transcript_hash[3],transcript_hash[4],transcript_hash[5]);
  assert (memcmp (transcript_hash,
                  "\xe3\xb0\xc4\x42\x98\xfc\x1c\x14"
                  "\x9a\xfb\xf4\xc8\x99\x6f\xb9\x24"
                  "\x27\xae\x41\xe4\x64\x9b\x93\x4c"
                  "\xa4\x95\x99\x1b\x78\x52\xb8\x55",
                  32) == 0);

  uint8_t tls13_derived[32];
  dsk_tls_derive_secret (DSK_CHECKSUM_SHA256,
                         prk,
                         7, (uint8_t *) "derived",
                         transcript_hash,
                         tls13_derived);
  assert (memcmp (tls13_derived, 
                  "\x6f\x26\x15\xa1\x08\xc7\x02\xc5"
                  "\x67\x8f\x54\xfc\x9d\xba\xb6\x97"
                  "\x16\xc0\x76\x18\x9c\x48\x25\x0c"
                  "\xeb\xea\xc3\x57\x6c\x36\x11\xba",
                  32) == 0);


  /* curve25519 */
  uint8_t curve25519_shared[32];
  dsk_curve25519_private_to_shared (
                 server_curve25519_private, client_curve25519_public,
                 curve25519_shared);
  assert(memcmp (expected_curve25519_shared, curve25519_shared, 32) == 0);


  uint8_t hs_secret[32];
  dsk_hkdf_extract (DSK_CHECKSUM_SHA256, 32, tls13_derived, 32, curve25519_shared, hs_secret);
  assert(memcmp (hs_secret,
                 "\x1d\xc8\x26\xe9\x36\x06\xaa\x6f"
                 "\xdc\x0a\xad\xc1\x2f\x74\x1b\x01"
                 "\x04\x6a\xa6\xb9\x9f\x69\x1e\xd2"
                 "\x21\xa9\xf0\xca\x04\x3f\xbe\xac",
                 32) == 0);

  // compute transcript hash of ClientHello + ServerHello
  DskChecksum *csum = dsk_checksum_new (DSK_CHECKSUM_SHA256);
  dsk_checksum_feed (csum, sizeof (rfc8448_sec3_client_hello_data) - 1, (uint8_t *)rfc8448_sec3_client_hello_data);
  dsk_checksum_feed (csum, sizeof (rfc8448_sec3_server_hello_data) - 1, (uint8_t *)rfc8448_sec3_server_hello_data);
  dsk_checksum_done (csum);
  dsk_checksum_get (csum, transcript_hash);
  dsk_checksum_destroy (csum);

  // {server} derive secret "tls13 c hs traffic"
  uint8_t tls13_c_hs_traffic[32];
  dsk_tls_derive_secret (DSK_CHECKSUM_SHA256,
                         hs_secret,
                         12, (uint8_t *) "c hs traffic",
                         transcript_hash,
                         tls13_c_hs_traffic);
  assert (memcmp (tls13_c_hs_traffic, 
                  "\xb3\xed\xdb\x12\x6e\x06\x7f\x35"
                  "\xa7\x80\xb3\xab\xf4\x5e\x2d\x8f"
                  "\x3b\x1a\x95\x07\x38\xf5\x2e\x96"
                  "\x00\x74\x6a\x0e\x27\xa5\x5a\x21",
                  32) == 0);

  // {server} derive secret "tls13 s hs traffic"
  uint8_t tls13_s_hs_traffic[32];
  dsk_tls_derive_secret (DSK_CHECKSUM_SHA256,
                         hs_secret,
                         12, (uint8_t *) "s hs traffic",
                         transcript_hash,
                         tls13_s_hs_traffic);
  assert (memcmp (tls13_s_hs_traffic, 
                  "\xb6\x7b\x7d\x69\x0c\xc1\x6c\x4e"
                  "\x75\xe5\x42\x13\xcb\x2d\x37\xb4"
                  "\xe9\xc9\x12\xbc\xde\xd9\x10\x5d"
                  "\x42\xbe\xfd\x59\xd3\x91\xad\x38",
                  32) == 0);

  // {server} derive secret for master "tls13 derived"
  dsk_checksum (DSK_CHECKSUM_SHA256,
                0, "",
                transcript_hash);
  uint8_t derived_for_master[32];
  dsk_tls_derive_secret (DSK_CHECKSUM_SHA256,
                         hs_secret,
                         7, (uint8_t *) "derived",
                         transcript_hash,
                         derived_for_master);
  assert (memcmp (derived_for_master,
                  "\x43\xde\x77\xe0\xc7\x77\x13\x85"
                  "\x9a\x94\x4d\xb9\xdb\x25\x90\xb5"
                  "\x31\x90\xa6\x5b\x3e\xe2\xe4\xf1"
                  "\x2d\xd7\xa0\xbb\x7c\xe2\x54\xb4",
                  32) == 0);


}


static void
test_server_hello (void)
{
  DskMemPool pool = DSK_MEM_POOL_STATIC_INIT;
  DskError *error = NULL;
  DskTlsHandshake *shake = dsk_tls_handshake_parse (DSK_TLS_HANDSHAKE_TYPE_SERVER_HELLO,
                                                    sizeof (rfc8448_sec3_server_hello_data) - 4 -1,
                                                    (uint8_t*) rfc8448_sec3_server_hello_data + 4,
                                                    &pool,
                                                    &error);
  if (shake == NULL)
    dsk_die ("error parsing client-hello: %s", error->message);
  bool has_supported_versions = false;
  bool has_key_share = false;
  assert (shake->server_hello.cipher_suite == DSK_TLS_CIPHER_SUITE_TLS_AES_128_GCM_SHA256);
  for (unsigned i = 0; i < shake->server_hello.n_extensions; i++)
    {
      //dsk_warning ("ext[%u] = 0x%04x [%s]", i, shake->server_hello.extensions[i]->type,
      //     dsk_tls_extension_type_name (shake->server_hello.extensions[i]->type));
      DskTlsExtension *ext = shake->server_hello.extensions[i];
      switch (ext->type)
        {
        case DSK_TLS_EXTENSION_TYPE_KEY_SHARE:
          has_key_share = true;
          DskTlsKeyShareEntry *se = &ext->key_share.server_share;
          assert (se->named_group == DSK_TLS_NAMED_GROUP_X25519);
          assert (se->key_exchange_length == 32);
          break;
        case DSK_TLS_EXTENSION_TYPE_SUPPORTED_VERSIONS:
          has_supported_versions = true;
          assert (ext->supported_versions.n_supported_versions == 1);
          assert (ext->supported_versions.supported_versions[0] == DSK_TLS_PROTOCOL_VERSION_1_3);
          break;
        default:
          assert(false);
        }
    }
  assert(has_supported_versions);
  assert(has_key_share);
}

static struct 
{
  const char *name;
  void (*test)(void);
} tests[] =
{
  { "Client Hello parsing", test_client_hello },
  { "Server secret calculations", test_server_secret_calculations_1 },
  { "Server Hello parsing", test_server_hello },
};


int main(int argc, char **argv)
{
  unsigned i;
  bool cmdline_verbose = false;

  dsk_cmdline_init ("test various TLS parsing routines",
                    "Test TLS parsing functions",
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
