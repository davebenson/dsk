#include "../dsk.h"
#include <stdio.h>
#include <string.h>


static void
test_client_hello ()
{
  // RFC 8448.  Section 3.
  const char hs_data[] = 
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
  DskMemPool pool = DSK_MEM_POOL_STATIC_INIT;
  DskError *error = NULL;
  DskTlsHandshake *shake = dsk_tls_handshake_parse (DSK_TLS_HANDSHAKE_TYPE_CLIENT_HELLO,
                                                    sizeof (hs_data), (uint8_t*) hs_data, &pool,
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

static struct 
{
  const char *name;
  void (*test)(void);
} tests[] =
{
  { "Client Hello parsing", test_client_hello },
};


int main(int argc, char **argv)
{
  unsigned i;
  dsk_boolean cmdline_verbose = false;

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
