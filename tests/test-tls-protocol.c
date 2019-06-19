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

static const char rfc8448_sec3_server_encrypted_extensions_data[] =
  "\x08\x00\x00\x24"                  // handshake header
  "\x00\x22"                          // length in bytes of all extensions
  "\x00\x0a\x00\x14"                  // extension header for supported-groups
  "\x00\x12\x00\x1d\x00\x17\x00\x18\x00\x19\x01\x00\x01\x01\x01\x02\x01\x03\x01\x04" // supported groups
  "\x00\x1c\x00\x02"                  // extension header, type=28 length=2
  "\x40\x01"                          // payload for type=28 extension
  "\x00\x00\x00\x00"                  // extension 0???
  ;

static const char rfc8448_sec3_server_certificate_data[] =
  "\x0b\x00\x01\xb9"
  "\x00"
  "\x00\x01\xb5"          //certificate_list length
  "\x00\x01\xb0"          //certificate data length
  "\x30\x82\x01\xac\x30\x82\x01\x15\xa0\x03\x02\x01\x02\x02\x01\x02"  // 0x1b (==27) lines of cert data
  "\x30\x0d\x06\x09\x2a\x86\x48\x86\xf7\x0d\x01\x01\x0b\x05\x00\x30"
  "\x0e\x31\x0c\x30\x0a\x06\x03\x55\x04\x03\x13\x03\x72\x73\x61\x30"
  "\x1e\x17\x0d\x31\x36\x30\x37\x33\x30\x30\x31\x32\x33\x35\x39\x5a"
  "\x17\x0d\x32\x36\x30\x37\x33\x30\x30\x31\x32\x33\x35\x39\x5a\x30"
  "\x0e\x31\x0c\x30\x0a\x06\x03\x55\x04\x03\x13\x03\x72\x73\x61\x30"
  "\x81\x9f\x30\x0d\x06\x09\x2a\x86\x48\x86\xf7\x0d\x01\x01\x01\x05"
  "\x00\x03\x81\x8d\x00\x30\x81\x89\x02\x81\x81\x00\xb4\xbb\x49\x8f"
  "\x82\x79\x30\x3d\x98\x08\x36\x39\x9b\x36\xc6\x98\x8c\x0c\x68\xde"
  "\x55\xe1\xbd\xb8\x26\xd3\x90\x1a\x24\x61\xea\xfd\x2d\xe4\x9a\x91"
  "\xd0\x15\xab\xbc\x9a\x95\x13\x7a\xce\x6c\x1a\xf1\x9e\xaa\x6a\xf9"
  "\x8c\x7c\xed\x43\x12\x09\x98\xe1\x87\xa8\x0e\xe0\xcc\xb0\x52\x4b"
  "\x1b\x01\x8c\x3e\x0b\x63\x26\x4d\x44\x9a\x6d\x38\xe2\x2a\x5f\xda"
  "\x43\x08\x46\x74\x80\x30\x53\x0e\xf0\x46\x1c\x8c\xa9\xd9\xef\xbf"
  "\xae\x8e\xa6\xd1\xd0\x3e\x2b\xd1\x93\xef\xf0\xab\x9a\x80\x02\xc4"
  "\x74\x28\xa6\xd3\x5a\x8d\x88\xd7\x9f\x7f\x1e\x3f\x02\x03\x01\x00"
  "\x01\xa3\x1a\x30\x18\x30\x09\x06\x03\x55\x1d\x13\x04\x02\x30\x00"
  "\x30\x0b\x06\x03\x55\x1d\x0f\x04\x04\x03\x02\x05\xa0\x30\x0d\x06"
  "\x09\x2a\x86\x48\x86\xf7\x0d\x01\x01\x0b\x05\x00\x03\x81\x81\x00"
  "\x85\xaa\xd2\xa0\xe5\xb9\x27\x6b\x90\x8c\x65\xf7\x3a\x72\x67\x17"
  "\x06\x18\xa5\x4c\x5f\x8a\x7b\x33\x7d\x2d\xf7\xa5\x94\x36\x54\x17"
  "\xf2\xea\xe8\xf8\xa5\x8c\x8f\x81\x72\xf9\x31\x9c\xf3\x6b\x7f\xd6"
  "\xc5\x5b\x80\xf2\x1a\x03\x01\x51\x56\x72\x60\x96\xfd\x33\x5e\x5e"
  "\x67\xf2\xdb\xf1\x02\x70\x2e\x60\x8c\xca\xe6\xbe\xc1\xfc\x63\xa4"
  "\x2a\x99\xbe\x5c\x3e\xb7\x10\x7c\x3c\x54\xe9\xb9\xeb\x2b\xd5\x20"
  "\x3b\x1c\x3b\x84\xe0\xa8\xb2\xf7\x59\x40\x9b\xa3\xea\xc9\xd9\x1d"
  "\x40\x2d\xcc\x0c\xc8\xf8\x96\x12\x29\xac\x91\x87\xb4\x2b\x4d\xe1"
  "\x00\x00"                                                            // extensions
  ;

static const char rfc8448_sec3_server_certificate_verify_data[] =
  "\x0f\x00\x00\x84"                        // handshake header (type+length)
  "\x08\x04\x00\x80\x5a\x74\x7c\x5d\x88\xfa\x9b\xd2\xe5\x5a\xb0\x85"
  "\xa6\x10\x15\xb7\x21\x1f\x82\x4c\xd4\x84\x14\x5a\xb3\xff\x52\xf1"
  "\xfd\xa8\x47\x7b\x0b\x7a\xbc\x90\xdb\x78\xe2\xd3\x3a\x5c\x14\x1a"
  "\x07\x86\x53\xfa\x6b\xef\x78\x0c\x5e\xa2\x48\xee\xaa\xa7\x85\xc4"
  "\xf3\x94\xca\xb6\xd3\x0b\xbe\x8d\x48\x59\xee\x51\x1f\x60\x29\x57"
  "\xb1\x54\x11\xac\x02\x76\x71\x45\x9e\x46\x44\x5c\x9e\xa5\x8c\x18"
  "\x1e\x81\x8e\x95\xb8\xc3\xfb\x0b\xf3\x27\x84\x09\xd3\xbe\x15\x2a"
  "\x3d\xa5\x04\x3e\x06\x3d\xda\x65\xcd\xf5\xae\xa2\x0d\x53\xdf\xac"
  "\xd4\x2f\x74\xf3";

static const char rfc8448_sec3_server_finished_data[] =
  "\x14\x00\x00\x20\x9b\x9b\x14\x1d\x90\x63\x37\xfb\xd2\xcb\xdc\xe7"
  "\x1d\xf4\xde\xda\x4a\xb4\x2c\x30\x95\x72\xcb\x7f\xff\xee\x54\x54"
  "\xb7\x8f\x07\x18";


static void
test_client_hello ()
{
  // RFC 8448.  Section 3.
  DskMemPool pool = DSK_MEM_POOL_STATIC_INIT;
  DskError *error = NULL;
  DskTlsHandshakeMessage *shake = dsk_tls_handshake_message_parse (DSK_TLS_HANDSHAKE_MESSAGE_TYPE_CLIENT_HELLO,
                                                    sizeof (rfc8448_sec3_client_hello_data) - 4 - 1,
                                                    (uint8_t*) rfc8448_sec3_client_hello_data + 4,
                                                    &pool,
                                                    &error);
  if (shake == NULL)
    dsk_die ("error parsing client-hello: %s", error->message);
  assert(shake->type == DSK_TLS_HANDSHAKE_MESSAGE_TYPE_CLIENT_HELLO);
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
	    assert(sn->entries[0].type == DSK_TLS_SERVER_NAME_TYPE_HOSTNAME);
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
  uint8_t zero_salt[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  uint8_t zero_ikm[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  uint8_t prk[32];
  dsk_hkdf_extract (&dsk_checksum_type_sha256, 32, zero_salt, 32, zero_ikm, prk);

  assert (memcmp (prk,
              "\x33\xad\x0a\x1c\x60\x7e\xc0\x3b"
              "\x09\xe6\xcd\x98\x93\x68\x0c\xe2"
              "\x10\xad\xf3\x00\xaa\x1f\x26\x60"
              "\xe1\xb2\x2e\x10\xf1\x70\xf9\x2a",
              32) == 0);

  /* {server} derive secret for handshake "tls13 derived" */
  uint8_t transcript_hash[32];
  assert(sizeof(rfc8448_sec3_client_hello_data) - 1 == 196);
  dsk_checksum (&dsk_checksum_type_sha256,
                0, "",
                transcript_hash);
  assert (memcmp (transcript_hash,
                  "\xe3\xb0\xc4\x42\x98\xfc\x1c\x14"
                  "\x9a\xfb\xf4\xc8\x99\x6f\xb9\x24"
                  "\x27\xae\x41\xe4\x64\x9b\x93\x4c"
                  "\xa4\x95\x99\x1b\x78\x52\xb8\x55",
                  32) == 0);

  uint8_t tls13_derived[32];
  dsk_tls_derive_secret (&dsk_checksum_type_sha256,
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
  dsk_curve25519_private_to_shared (
                 client_curve25519_private, server_curve25519_public,
                 curve25519_shared);
  assert(memcmp (expected_curve25519_shared, curve25519_shared, 32) == 0);


  uint8_t hs_secret[32];
  dsk_hkdf_extract (&dsk_checksum_type_sha256, 32, tls13_derived, 32, curve25519_shared, hs_secret);
  assert(memcmp (hs_secret,
                 "\x1d\xc8\x26\xe9\x36\x06\xaa\x6f"
                 "\xdc\x0a\xad\xc1\x2f\x74\x1b\x01"
                 "\x04\x6a\xa6\xb9\x9f\x69\x1e\xd2"
                 "\x21\xa9\xf0\xca\x04\x3f\xbe\xac",
                 32) == 0);

  // compute transcript hash of ClientHello + ServerHello
  DskChecksum *csum = dsk_checksum_new (&dsk_checksum_type_sha256);
  dsk_checksum_feed (csum, sizeof (rfc8448_sec3_client_hello_data) - 1, (uint8_t *)rfc8448_sec3_client_hello_data);
  dsk_checksum_feed (csum, sizeof (rfc8448_sec3_server_hello_data) - 1, (uint8_t *)rfc8448_sec3_server_hello_data);
  dsk_checksum_done (csum);
  dsk_checksum_get (csum, transcript_hash);
  dsk_checksum_destroy (csum);

  // {server} derive secret "tls13 c hs traffic"
  uint8_t tls13_c_hs_traffic[32];
  dsk_tls_derive_secret (&dsk_checksum_type_sha256,
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
  dsk_tls_derive_secret (&dsk_checksum_type_sha256,
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
  dsk_checksum (&dsk_checksum_type_sha256,
                0, "",
                transcript_hash);
  uint8_t derived_for_master[32];
  dsk_tls_derive_secret (&dsk_checksum_type_sha256,
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

  // {server} extract secret "master"
  uint8_t master_secret[32];
  dsk_hkdf_extract (&dsk_checksum_type_sha256,
                    32, derived_for_master,
                    32, zero_ikm,
                    master_secret);
  assert (memcmp (master_secret,
                  "\x18\xdf\x06\x84\x3d\x13\xa0\x8b"
                  "\xf2\xa4\x49\x84\x4c\x5f\x8a\x47"
                  "\x80\x01\xbc\x4d\x4c\x62\x79\x84"
                  "\xd5\xa4\x1d\xa8\xd0\x40\x29\x19",
                  32) == 0);
  
  // {server} derive write traffic keys for handshake data
  uint8_t server_write_traffic_key[16],
          server_write_traffic_iv[12];
  dsk_tls_hkdf_expand_label(&dsk_checksum_type_sha256,
                            tls13_s_hs_traffic,
                            3, (const uint8_t *) "key",
                            0, NULL,
                            sizeof(server_write_traffic_key),
                            server_write_traffic_key);
  assert (memcmp (server_write_traffic_key,
                  "\x3f\xce\x51\x60\x09\xc2\x17\x27\xd0\xf2\xe4\xe8\x6e\xe4\x03\xbc",
                  sizeof(server_write_traffic_key)) == 0);
  dsk_tls_hkdf_expand_label(&dsk_checksum_type_sha256,
                            tls13_s_hs_traffic,
                            2, (const uint8_t *) "iv",
                            0, NULL,
                            sizeof(server_write_traffic_iv),
                            server_write_traffic_iv);
  assert (memcmp (server_write_traffic_iv,
                  "\x5d\x31\x3e\xb2\x67\x12\x76\xee\x13\x00\x0b\x30",
                  sizeof(server_write_traffic_iv)) == 0);


}


static void
test_server_hello (void)
{
  DskMemPool pool = DSK_MEM_POOL_STATIC_INIT;
  DskError *error = NULL;
  DskTlsHandshakeMessage *shake = dsk_tls_handshake_message_parse (DSK_TLS_HANDSHAKE_MESSAGE_TYPE_SERVER_HELLO,
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

static void
test_encrypted_extensions (void)
{
  DskMemPool pool = DSK_MEM_POOL_STATIC_INIT;
  DskError *error = NULL;
  DskTlsHandshakeMessage *shake = dsk_tls_handshake_message_parse (DSK_TLS_HANDSHAKE_MESSAGE_TYPE_ENCRYPTED_EXTENSIONS,
                                                    sizeof (rfc8448_sec3_server_encrypted_extensions_data) - 4 -1,
                                                    (uint8_t*) rfc8448_sec3_server_encrypted_extensions_data + 4,
                                                    &pool,
                                                    &error);
  if (shake == NULL)
    dsk_die ("error parsing EncryptedExtensions: %s", error->message);

  bool has_supported_groups = false;
  bool has_server_name = false;
  for (unsigned i = 0; i < shake->encrypted_extensions.n_extensions; i++)
    {
      DskTlsExtension *ext = shake->encrypted_extensions.extensions[i];
      switch (ext->type)
        {
          case DSK_TLS_EXTENSION_TYPE_SUPPORTED_GROUPS:
            assert (ext->supported_groups.n_supported_groups == 9);
            has_supported_groups = true;
            break;
          case DSK_TLS_EXTENSION_TYPE_SERVER_NAME:
            assert (ext->server_name.n_entries == 0);
            has_server_name = true;
            break;
        }
    }
  assert(has_server_name);
  assert(has_supported_groups);
}

static void
test_certificate_handshake_message_parsing(void)
{
  DskMemPool pool = DSK_MEM_POOL_STATIC_INIT;
  DskError *error = NULL;
  DskTlsHandshakeMessage *shake = dsk_tls_handshake_message_parse (DSK_TLS_HANDSHAKE_MESSAGE_TYPE_CERTIFICATE,
                                                    sizeof (rfc8448_sec3_server_certificate_data) - 4 -1,
                                                    (uint8_t*) rfc8448_sec3_server_certificate_data + 4,
                                                    &pool,
                                                    &error);
  if (shake == NULL)
    dsk_die ("error parsing Certificate: %s", error->message);
  assert(shake->type == DSK_TLS_HANDSHAKE_MESSAGE_TYPE_CERTIFICATE);
  assert(shake->certificate.certificate_request_context_len == 0);
  assert(shake->certificate.n_entries == 1);
  assert(shake->certificate.entries[0].n_extensions == 0);
  assert(shake->certificate.entries[0].cert_data_length == 432);

  dsk_mem_pool_clear (&pool);
}

static void
test_certificate_verify_handshake_message_parsing (void)
{
  DskMemPool pool = DSK_MEM_POOL_STATIC_INIT;
  DskError *error = NULL;
  DskTlsHandshakeMessage *shake = dsk_tls_handshake_message_parse (DSK_TLS_HANDSHAKE_MESSAGE_TYPE_CERTIFICATE_VERIFY,
                                                    sizeof (rfc8448_sec3_server_certificate_verify_data) - 4 -1,
                                                    (uint8_t*) rfc8448_sec3_server_certificate_verify_data + 4,
                                                    &pool,
                                                    &error);
  if (shake == NULL)
    dsk_die ("error parsing CertificateVerify: %s", error->message);
  assert(shake->certificate_verify.algorithm == DSK_TLS_SIGNATURE_SCHEME_RSA_PSS_RSAE_SHA256);
  assert(shake->certificate_verify.signature_length == 128); 
}

static void
test_server_finished_handshake_message_parsing(void)
{
  DskMemPool pool = DSK_MEM_POOL_STATIC_INIT;
  DskError *error = NULL;
  DskTlsHandshakeMessage *shake = dsk_tls_handshake_message_parse (DSK_TLS_HANDSHAKE_MESSAGE_TYPE_FINISHED,
                                                    sizeof (rfc8448_sec3_server_finished_data) - 4 -1,
                                                    (uint8_t*) rfc8448_sec3_server_finished_data + 4,
                                                    &pool,
                                                    &error);
  if (shake == NULL)
    dsk_die ("error parsing Finished: %s", error->message);
  assert(shake->type == DSK_TLS_HANDSHAKE_MESSAGE_TYPE_FINISHED);
  assert(shake->finished.verify_data_length == 32);

  //
  // Calculations from RFC 8446 4.4.4. Finished.
  //
  // compute finished key
  uint8_t finished_key[32];
  dsk_tls_hkdf_expand_label (&dsk_checksum_type_sha256,
                   (uint8_t*)"\xb6\x7b\x7d\x69\x0c\xc1\x6c\x4e"
                             "\x75\xe5\x42\x13\xcb\x2d\x37\xb4"
                             "\xe9\xc9\x12\xbc\xde\xd9\x10\x5d"
                             "\x42\xbe\xfd\x59\xd3\x91\xad\x38",
                             8, (uint8_t *) "finished",
                             0, (uint8_t*) "",
                             32, finished_key);
  assert (memcmp (finished_key,
                  "\x00\x8d\x3b\x66\xf8\x16\xea\x55"
                  "\x9f\x96\xb5\x37\xe8\x85\xc3\x1f"
                  "\xc0\x68\xbf\x49\x2c\x65\x2f\x01"
                  "\xf2\x88\xa1\xd8\xcd\xc1\x9f\xc8", 32) == 0);

  // compute transcript hash of
  // Transcript-Hash(Handshake Context, Certificate, CertificateVerify)
  DskChecksum *csum = dsk_checksum_new (&dsk_checksum_type_sha256);
  dsk_checksum_feed (csum, sizeof (rfc8448_sec3_client_hello_data) - 1, (uint8_t *)rfc8448_sec3_client_hello_data);
  dsk_checksum_feed (csum, sizeof (rfc8448_sec3_server_hello_data) - 1, (uint8_t *)rfc8448_sec3_server_hello_data);
  dsk_checksum_feed (csum, sizeof (rfc8448_sec3_server_encrypted_extensions_data) - 1, (uint8_t *)rfc8448_sec3_server_encrypted_extensions_data);
  dsk_checksum_feed (csum, sizeof (rfc8448_sec3_server_certificate_data) - 1, (uint8_t *)rfc8448_sec3_server_certificate_data);
  dsk_checksum_feed (csum, sizeof (rfc8448_sec3_server_certificate_verify_data) - 1, (uint8_t *)rfc8448_sec3_server_certificate_verify_data);
  uint8_t transcript_hash[32];
  dsk_checksum_done (csum);
  dsk_checksum_get (csum, transcript_hash);
  dsk_checksum_destroy (csum);

  //compute verify-data == HMAC(finished_key, transcript-hash)
  uint8_t verify_data[32];
  dsk_hmac_digest (&dsk_checksum_type_sha256,
                   32, finished_key,
                   32, transcript_hash,
                   verify_data);
   
  assert(memcmp(shake->finished.verify_data, verify_data, 32) == 0);
}

static void
test_server_handshake_encryption (void)
{
  // compute plaintext length
  unsigned payload_len = sizeof(rfc8448_sec3_server_encrypted_extensions_data) - 1
                       + sizeof(rfc8448_sec3_server_certificate_data) - 1
                       + sizeof(rfc8448_sec3_server_certificate_verify_data) - 1
                       + sizeof(rfc8448_sec3_server_finished_data) - 1
                       + 1                      // for trailing content-type
                       ;
  assert(payload_len == 658);                   // includes trailing content-type

  // construct payload of concatenated messages, terminated by ContentType==Handshake
  uint8_t *payload_data = dsk_malloc (payload_len);
  uint8_t *at = payload_data;
  memcpy (at, rfc8448_sec3_server_encrypted_extensions_data,
          sizeof(rfc8448_sec3_server_encrypted_extensions_data) - 1);
  at += sizeof(rfc8448_sec3_server_encrypted_extensions_data) - 1;
  memcpy (at, rfc8448_sec3_server_certificate_data,
          sizeof(rfc8448_sec3_server_certificate_data) - 1);
  at += sizeof(rfc8448_sec3_server_certificate_data) - 1;
  memcpy (at, rfc8448_sec3_server_certificate_verify_data,
          sizeof(rfc8448_sec3_server_certificate_verify_data) - 1);
  at += sizeof(rfc8448_sec3_server_certificate_verify_data) - 1;
  memcpy (at, rfc8448_sec3_server_finished_data,
          sizeof(rfc8448_sec3_server_finished_data) - 1);
  at += sizeof(rfc8448_sec3_server_finished_data) - 1;
  *at++ = DSK_TLS_RECORD_CONTENT_TYPE_HANDSHAKE;
  assert(payload_data + payload_len == at);

  unsigned enc_payload_len = payload_len + 16;

  // Additional data.
  uint8_t additional_data[5] = {
    DSK_TLS_RECORD_CONTENT_TYPE_APPLICATION_DATA,
    3, 3,
    enc_payload_len >> 8, enc_payload_len & 0xff
  };
  unsigned additional_data_length = 5;

  uint8_t iv_data[12];
  memcpy (iv_data, "\x5d\x31\x3e\xb2\x67\x12\x76\xee\x13\x00\x0b\x30", 12);

  DskAES128 aes128;
  dsk_aes128_init (&aes128, 
                  (const uint8_t *) "\x3f\xce\x51\x60\x09\xc2\x17\x27\xd0\xf2\xe4\xe8\x6e\xe4\x03\xbc");
  Dsk_AEAD_GCM_Precomputation precompute;
  dsk_aead_gcm_precompute ((DskBlockCipherInplaceFunc) dsk_aes128_encrypt_inplace, &aes128, &precompute);

  // perform encryption
  uint8_t auth_tag[16];
  uint8_t *ciphertext = dsk_malloc(payload_len);
  dsk_aead_gcm_encrypt (&precompute,
                        payload_len, payload_data,
                        additional_data_length, additional_data,
                        sizeof(iv_data), iv_data,
                        ciphertext,
                        16, auth_tag);
  assert(memcmp (ciphertext,
        "\xd1\xff\x33\x4a\x56\xf5\xbf\xf6\x59\x4a\x07\xcc\x87\xb5\x80\x23"
        "\x3f\x50\x0f\x45\xe4\x89\xe7\xf3\x3a\xf3\x5e\xdf\x78\x69\xfc\xf4"
        "\x0a\xa4\x0a\xa2\xb8\xea\x73\xf8\x48\xa7\xca\x07\x61\x2e\xf9\xf9"
        "\x45\xcb\x96\x0b\x40\x68\x90\x51\x23\xea\x78\xb1\x11\xb4\x29\xba"
        "\x91\x91\xcd\x05\xd2\xa3\x89\x28\x0f\x52\x61\x34\xaa\xdc\x7f\xc7"
        "\x8c\x4b\x72\x9d\xf8\x28\xb5\xec\xf7\xb1\x3b\xd9\xae\xfb\x0e\x57"
        "\xf2\x71\x58\x5b\x8e\xa9\xbb\x35\x5c\x7c\x79\x02\x07\x16\xcf\xb9"
        "\xb1\x18\x3e\xf3\xab\x20\xe3\x7d\x57\xa6\xb9\xd7\x47\x76\x09\xae"
        "\xe6\xe1\x22\xa4\xcf\x51\x42\x73\x25\x25\x0c\x7d\x0e\x50\x92\x89"
        "\x44\x4c\x9b\x3a\x64\x8f\x1d\x71\x03\x5d\x2e\xd6\x5b\x0e\x3c\xdd"
        "\x0c\xba\xe8\xbf\x2d\x0b\x22\x78\x12\xcb\xb3\x60\x98\x72\x55\xcc"
        "\x74\x41\x10\xc4\x53\xba\xa4\xfc\xd6\x10\x92\x8d\x80\x98\x10\xe4"
        "\xb7\xed\x1a\x8f\xd9\x91\xf0\x6a\xa6\x24\x82\x04\x79\x7e\x36\xa6"
        "\xa7\x3b\x70\xa2\x55\x9c\x09\xea\xd6\x86\x94\x5b\xa2\x46\xab\x66"
        "\xe5\xed\xd8\x04\x4b\x4c\x6d\xe3\xfc\xf2\xa8\x94\x41\xac\x66\x27"
        "\x2f\xd8\xfb\x33\x0e\xf8\x19\x05\x79\xb3\x68\x45\x96\xc9\x60\xbd"
        "\x59\x6e\xea\x52\x0a\x56\xa8\xd6\x50\xf5\x63\xaa\xd2\x74\x09\x96"
        "\x0d\xca\x63\xd3\xe6\x88\x61\x1e\xa5\xe2\x2f\x44\x15\xcf\x95\x38"
        "\xd5\x1a\x20\x0c\x27\x03\x42\x72\x96\x8a\x26\x4e\xd6\x54\x0c\x84"
        "\x83\x8d\x89\xf7\x2c\x24\x46\x1a\xad\x6d\x26\xf5\x9e\xca\xba\x9a"
        "\xcb\xbb\x31\x7b\x66\xd9\x02\xf4\xf2\x92\xa3\x6a\xc1\xb6\x39\xc6"
        "\x37\xce\x34\x31\x17\xb6\x59\x62\x22\x45\x31\x7b\x49\xee\xda\x0c"
        "\x62\x58\xf1\x00\xd7\xd9\x61\xff\xb1\x38\x64\x7e\x92\xea\x33\x0f"
        "\xae\xea\x6d\xfa\x31\xc7\xa8\x4d\xc3\xbd\x7e\x1b\x7a\x6c\x71\x78"
        "\xaf\x36\x87\x90\x18\xe3\xf2\x52\x10\x7f\x24\x3d\x24\x3d\xc7\x33"
        "\x9d\x56\x84\xc8\xb0\x37\x8b\xf3\x02\x44\xda\x8c\x87\xc8\x43\xf5"
        "\xe5\x6e\xb4\xc5\xe8\x28\x0a\x2b\x48\x05\x2c\xf9\x3b\x16\x49\x9a"
        "\x66\xdb\x7c\xca\x71\xe4\x59\x94\x26\xf7\xd4\x61\xe6\x6f\x99\x88"
        "\x2b\xd8\x9f\xc5\x08\x00\xbe\xcc\xa6\x2d\x6c\x74\x11\x6d\xbd\x29"
        "\x72\xfd\xa1\xfa\x80\xf8\x5d\xf8\x81\xed\xbe\x5a\x37\x66\x89\x36"
        "\xb3\x35\x58\x3b\x59\x91\x86\xdc\x5c\x69\x18\xa3\x96\xfa\x48\xa1"
        "\x81\xd6\xb6\xfa\x4f\x9d\x62\xd5\x13\xaf\xbb\x99\x2f\x2b\x99\x2f"
        "\x67\xf8\xaf\xe6\x7f\x76\x91\x3f\xa3\x88\xcb\x56\x30\xc8\xca\x01"
        "\xe0\xc6\x5d\x11\xc6\x6a\x1e\x2a\xc4\xc8\x59\x77\xb7\xc7\xa6\x99"
        "\x9b\xbf\x10\xdc\x35\xae\x69\xf5\x51\x56\x14\x63\x6c\x0b\x9b\x68"
        "\xc1\x9e\xd2\xe3\x1c\x0b\x3b\x66\x76\x30\x38\xeb\xba\x42\xf3\xb3"
        "\x8e\xdc\x03\x99\xf3\xa9\xf2\x3f\xaa\x63\x97\x8c\x31\x7f\xc9\xfa"
        "\x66\xa7\x3f\x60\xf0\x50\x4d\xe9\x3b\x5b\x84\x5e\x27\x55\x92\xc1"
        "\x23\x35\xee\x34\x0b\xbc\x4f\xdd\xd5\x02\x78\x40\x16\xe4\xb3\xbe"
        "\x7e\xf0\x4d\xda\x49\xf4\xb4\x40\xa3\x0c\xb5\xd2\xaf\x93\x98\x28"
        "\xfd\x4a\xe3\x79\x4e\x44\xf9\x4d\xf5\xa6\x31\xed\xe4\x2c\x17\x19"
        "\xbf\xda",
        payload_len) == 0);
    assert(memcmp (auth_tag, 
        "\xbf\x02\x53\xfe\x51\x75\xbe\x89\x8e\x75\x0e\xdc\x53\x37\x0d\x2b",
                   16) == 0);

  dsk_free (payload_data);
  dsk_free (ciphertext);
}

static void
test_server_secret_calculations_2 (void)
{
  DskTlsKeySchedule *schedule = dsk_tls_key_schedule_new (&dsk_tls_cipher_suite_aes128_gcm_sha256);
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

  uint8_t client_hello_th[32];
  uint8_t server_hello_th[32];
  uint8_t certverify_th[32];
  uint8_t finished_th[32];

  //
  // Compute Transcript-Hashes.
  //
  DskChecksum *csum = dsk_checksum_new (&dsk_checksum_type_sha256);
  dsk_checksum_feed (csum, sizeof (rfc8448_sec3_client_hello_data) - 1, (uint8_t *)rfc8448_sec3_client_hello_data);
  dsk_checksum_get_current (csum, client_hello_th);
  dsk_checksum_feed (csum, sizeof (rfc8448_sec3_server_hello_data) - 1, (uint8_t *)rfc8448_sec3_server_hello_data);
  dsk_checksum_get_current (csum, server_hello_th);
  dsk_checksum_feed (csum, sizeof (rfc8448_sec3_server_encrypted_extensions_data) - 1, (uint8_t *)rfc8448_sec3_server_encrypted_extensions_data);
  dsk_checksum_feed (csum, sizeof (rfc8448_sec3_server_certificate_data) - 1, (uint8_t *)rfc8448_sec3_server_certificate_data);
  dsk_checksum_feed (csum, sizeof (rfc8448_sec3_server_certificate_verify_data) - 1, (uint8_t *)rfc8448_sec3_server_certificate_verify_data);
  dsk_checksum_get_current (csum, certverify_th);
  dsk_checksum_feed (csum, sizeof (rfc8448_sec3_server_finished_data) - 1, (uint8_t *)rfc8448_sec3_server_finished_data);
  dsk_checksum_get_current (csum, finished_th);
  dsk_checksum_destroy (csum);


  /* {server} extract secret "early". */
  dsk_tls_key_schedule_compute_early_secrets (schedule, false,
                                              client_hello_th,
                                              0, NULL);
  assert (memcmp (schedule->early_secret,
              "\x33\xad\x0a\x1c\x60\x7e\xc0\x3b"
              "\x09\xe6\xcd\x98\x93\x68\x0c\xe2"
              "\x10\xad\xf3\x00\xaa\x1f\x26\x60"
              "\xe1\xb2\x2e\x10\xf1\x70\xf9\x2a",
              32) == 0);

  /* {server} derive secret for handshake "tls13 derived" */
  assert (memcmp (client_hello_th,
                  "\x4d\xb2\x55\xf3\x0d\xa0\x9a\x40"
                  "\x7c\x84\x17\x20\xbe\x83\x1a\x06"
                  "\xa5\xaa\x9b\x36\x62\xa5\xf4\x42"
                  "\x67\xd3\x77\x06\xb7\x3c\x2b\x8c",
                  32) == 0);

  assert (memcmp (schedule->early_derived_secret, 
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
  dsk_curve25519_private_to_shared (
                 client_curve25519_private, server_curve25519_public,
                 curve25519_shared);
  assert(memcmp (expected_curve25519_shared, curve25519_shared, 32) == 0);


  dsk_tls_key_schedule_compute_handshake_secrets (schedule, 
                       server_hello_th,
                       32, curve25519_shared);
  assert(memcmp (schedule->handshake_secret,
                 "\x1d\xc8\x26\xe9\x36\x06\xaa\x6f"
                 "\xdc\x0a\xad\xc1\x2f\x74\x1b\x01"
                 "\x04\x6a\xa6\xb9\x9f\x69\x1e\xd2"
                 "\x21\xa9\xf0\xca\x04\x3f\xbe\xac",
                 32) == 0);

  // {server} derive secret "tls13 c hs traffic"
  assert (memcmp (schedule->client_handshake_traffic_secret,
                  "\xb3\xed\xdb\x12\x6e\x06\x7f\x35"
                  "\xa7\x80\xb3\xab\xf4\x5e\x2d\x8f"
                  "\x3b\x1a\x95\x07\x38\xf5\x2e\x96"
                  "\x00\x74\x6a\x0e\x27\xa5\x5a\x21",
                  32) == 0);

  // {server} derive secret "tls13 s hs traffic"
  assert (memcmp (schedule->server_handshake_traffic_secret,
                  "\xb6\x7b\x7d\x69\x0c\xc1\x6c\x4e"
                  "\x75\xe5\x42\x13\xcb\x2d\x37\xb4"
                  "\xe9\xc9\x12\xbc\xde\xd9\x10\x5d"
                  "\x42\xbe\xfd\x59\xd3\x91\xad\x38",
                  32) == 0);

  // {server} derive secret for master "tls13 derived"
  assert (memcmp (schedule->handshake_derived_secret,
                  "\x43\xde\x77\xe0\xc7\x77\x13\x85"
                  "\x9a\x94\x4d\xb9\xdb\x25\x90\xb5"
                  "\x31\x90\xa6\x5b\x3e\xe2\xe4\xf1"
                  "\x2d\xd7\xa0\xbb\x7c\xe2\x54\xb4",
                  32) == 0);

  dsk_tls_key_schedule_compute_finished_data (schedule, certverify_th);
  // {server} extract secret "master"
  dsk_tls_key_schedule_compute_master_secrets (schedule, 
                       finished_th);
  assert (memcmp (schedule->master_secret,
                  "\x18\xdf\x06\x84\x3d\x13\xa0\x8b"
                  "\xf2\xa4\x49\x84\x4c\x5f\x8a\x47"
                  "\x80\x01\xbc\x4d\x4c\x62\x79\x84"
                  "\xd5\xa4\x1d\xa8\xd0\x40\x29\x19",
                  32) == 0);
  
#if 0   /// TODO fixme traffic keying should be handled by KeyScedule
  // {server} derive write traffic keys for handshake data
  uint8_t server_write_traffic_key[16],
          server_write_traffic_iv[12];
  dsk_tls_hkdf_expand_label(&dsk_checksum_type_sha256,
                            tls13_s_hs_traffic,
                            3, (const uint8_t *) "key",
                            0, NULL,
                            sizeof(server_write_traffic_key),
                            server_write_traffic_key);
  assert (memcmp (server_write_traffic_key,
                  "\x3f\xce\x51\x60\x09\xc2\x17\x27\xd0\xf2\xe4\xe8\x6e\xe4\x03\xbc",
                  sizeof(server_write_traffic_key)) == 0);
  dsk_tls_hkdf_expand_label(&dsk_checksum_type_sha256,
                            tls13_s_hs_traffic,
                            2, (const uint8_t *) "iv",
                            0, NULL,
                            sizeof(server_write_traffic_iv),
                            server_write_traffic_iv);
  assert (memcmp (server_write_traffic_iv,
                  "\x5d\x31\x3e\xb2\x67\x12\x76\xee\x13\x00\x0b\x30",
                  sizeof(server_write_traffic_iv)) == 0);
#endif


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
  { "EncryptedExtensions parsing", test_encrypted_extensions },
  { "Certificate (handshake-message) parsing", test_certificate_handshake_message_parsing },
  { "Certificate Verify (handshake-message) parsing", test_certificate_verify_handshake_message_parsing },
  { "Server Finished (handshake-message)", test_server_finished_handshake_message_parsing },
  { "Server Handshake Encryption", test_server_handshake_encryption },
  { "Server secret calculations (using KeySchedule)", test_server_secret_calculations_2 },
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
