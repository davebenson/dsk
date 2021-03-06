#include "../dsk.h"
#include <string.h>
#include <stdio.h>

static bool cmdline_verbose = false;
static void
test_cert (void)
{
  const uint8_t t1[] = {
    0x30,0x82,0x02,0x12,0x30,0x82,0x01,0x7b,0x02,0x02,0x0d,0xfa,0x30,0x0d,0x06,0x09,
    0x2a,0x86,0x48,0x86,0xf7,0x0d,0x01,0x01,0x05,0x05,0x00,0x30,0x81,0x9b,0x31,0x0b,
    0x30,0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x4a,0x50,0x31,0x0e,0x30,0x0c,0x06,
    0x03,0x55,0x04,0x08,0x13,0x05,0x54,0x6f,0x6b,0x79,0x6f,0x31,0x10,0x30,0x0e,0x06,
    0x03,0x55,0x04,0x07,0x13,0x07,0x43,0x68,0x75,0x6f,0x2d,0x6b,0x75,0x31,0x11,0x30,
    0x0f,0x06,0x03,0x55,0x04,0x0a,0x13,0x08,0x46,0x72,0x61,0x6e,0x6b,0x34,0x44,0x44,
    0x31,0x18,0x30,0x16,0x06,0x03,0x55,0x04,0x0b,0x13,0x0f,0x57,0x65,0x62,0x43,0x65,
    0x72,0x74,0x20,0x53,0x75,0x70,0x70,0x6f,0x72,0x74,0x31,0x18,0x30,0x16,0x06,0x03,
    0x55,0x04,0x03,0x13,0x0f,0x46,0x72,0x61,0x6e,0x6b,0x34,0x44,0x44,0x20,0x57,0x65,
    0x62,0x20,0x43,0x41,0x31,0x23,0x30,0x21,0x06,0x09,0x2a,0x86,0x48,0x86,0xf7,0x0d,
    0x01,0x09,0x01,0x16,0x14,0x73,0x75,0x70,0x70,0x6f,0x72,0x74,0x40,0x66,0x72,0x61,
    0x6e,0x6b,0x34,0x64,0x64,0x2e,0x63,0x6f,0x6d,0x30,0x1e,0x17,0x0d,0x31,0x32,0x30,
    0x38,0x32,0x32,0x30,0x35,0x32,0x36,0x35,0x34,0x5a,0x17,0x0d,0x31,0x37,0x30,0x38,
    0x32,0x31,0x30,0x35,0x32,0x36,0x35,0x34,0x5a,0x30,0x4a,0x31,0x0b,0x30,0x09,0x06,
    0x03,0x55,0x04,0x06,0x13,0x02,0x4a,0x50,0x31,0x0e,0x30,0x0c,0x06,0x03,0x55,0x04,
    0x08,0x0c,0x05,0x54,0x6f,0x6b,0x79,0x6f,0x31,0x11,0x30,0x0f,0x06,0x03,0x55,0x04,
    0x0a,0x0c,0x08,0x46,0x72,0x61,0x6e,0x6b,0x34,0x44,0x44,0x31,0x18,0x30,0x16,0x06,
    0x03,0x55,0x04,0x03,0x0c,0x0f,0x77,0x77,0x77,0x2e,0x65,0x78,0x61,0x6d,0x70,0x6c,
    0x65,0x2e,0x63,0x6f,0x6d,0x30,0x5c,0x30,0x0d,0x06,0x09,0x2a,0x86,0x48,0x86,0xf7,
    0x0d,0x01,0x01,0x01,0x05,0x00,0x03,0x4b,0x00,0x30,0x48,0x02,0x41,0x00,0x9b,0xfc,
    0x66,0x90,0x79,0x84,0x42,0xbb,0xab,0x13,0xfd,0x2b,0x7b,0xf8,0xde,0x15,0x12,0xe5,
    0xf1,0x93,0xe3,0x06,0x8a,0x7b,0xb8,0xb1,0xe1,0x9e,0x26,0xbb,0x95,0x01,0xbf,0xe7,
    0x30,0xed,0x64,0x85,0x02,0xdd,0x15,0x69,0xa8,0x34,0xb0,0x06,0xec,0x3f,0x35,0x3c,
    0x1e,0x1b,0x2b,0x8f,0xfa,0x8f,0x00,0x1b,0xdf,0x07,0xc6,0xac,0x53,0x07,0x02,0x03,
    0x01,0x00,0x01,0x30,0x0d,0x06,0x09,0x2a,0x86,0x48,0x86,0xf7,0x0d,0x01,0x01,0x05,
    0x05,0x00,0x03,0x81,0x81,0x00,0x14,0xb6,0x4c,0xbb,0x81,0x79,0x33,0xe6,0x71,0xa4,
    0xda,0x51,0x6f,0xcb,0x08,0x1d,0x8d,0x60,0xec,0xbc,0x18,0xc7,0x73,0x47,0x59,0xb1,
    0xf2,0x20,0x48,0xbb,0x61,0xfa,0xfc,0x4d,0xad,0x89,0x8d,0xd1,0x21,0xeb,0xd5,0xd8,
    0xe5,0xba,0xd6,0xa6,0x36,0xfd,0x74,0x50,0x83,0xb6,0x0f,0xc7,0x1d,0xdf,0x7d,0xe5,
    0x2e,0x81,0x7f,0x45,0xe0,0x9f,0xe2,0x3e,0x79,0xee,0xd7,0x30,0x31,0xc7,0x20,0x72,
    0xd9,0x58,0x2e,0x2a,0xfe,0x12,0x5a,0x34,0x45,0xa1,0x19,0x08,0x7c,0x89,0x47,0x5f,
    0x4a,0x95,0xbe,0x23,0x21,0x4a,0x53,0x72,0xda,0x2a,0x05,0x2f,0x2e,0xc9,0x70,0xf6,
    0x5b,0xfa,0xfd,0xdf,0xb4,0x31,0xb2,0xc1,0x4a,0x9c,0x06,0x25,0x43,0xa1,0xe6,0xb4,
    0x1e,0x7f,0x86,0x9b,0x16,0x40,
  };
  DskASN1Value *v;
  size_t used;
  DskError *err = NULL;
  DskMemPool pool;
  
  dsk_mem_pool_init (&pool);
  v = dsk_asn1_value_parse_der (sizeof(t1), t1, &used, &pool, &err);
  assert(used == sizeof(t1));

#if 0
  DskBuffer buf = DSK_BUFFER_INIT;
  dsk_asn1_value_dump_to_buffer (v, &buf);
  dsk_buffer_writev (&buf, 1);
  dsk_buffer_clear (&buf);
#endif

  DskTlsX509Certificate *cert = dsk_tls_x509_certificate_from_asn1 (v, &pool, &err);
  if (cert == NULL)
    dsk_die ("error parsing X509 Cert: %s", err->message);
  assert (strcmp (dsk_tls_x509_name_get_component (&cert->issuer, DSK_TLS_X509_DN_COUNTRY), "JP") == 0);
  assert (strcmp (dsk_tls_x509_name_get_component (&cert->issuer, DSK_TLS_X509_DN_STATE_OR_PROVINCE), "Tokyo") == 0);
  assert (strcmp (dsk_tls_x509_name_get_component (&cert->issuer, DSK_TLS_X509_DN_LOCALITY), "Chuo-ku") == 0);
  assert (strcmp (dsk_tls_x509_name_get_component (&cert->issuer, DSK_TLS_X509_DN_ORGANIZATION_NAME), "Frank4DD") == 0);
  assert (strcmp (dsk_tls_x509_name_get_component (&cert->issuer, DSK_TLS_X509_DN_ORGANIZATIONAL_UNIT), "WebCert Support") == 0);
  assert (strcmp (dsk_tls_x509_name_get_component (&cert->issuer, DSK_TLS_X509_DN_COMMON_NAME), "Frank4DD Web CA") == 0);
  assert (strcmp (dsk_tls_x509_name_get_component (&cert->issuer, DSK_TLS_X509_DN_EMAIL_ADDRESS), "support@frank4dd.com") == 0);

  assert (strcmp (dsk_tls_x509_name_get_component (&cert->subject, DSK_TLS_X509_DN_COUNTRY), "JP") == 0);
  assert (strcmp (dsk_tls_x509_name_get_component (&cert->subject, DSK_TLS_X509_DN_STATE_OR_PROVINCE), "Tokyo") == 0);
  assert (strcmp (dsk_tls_x509_name_get_component (&cert->subject, DSK_TLS_X509_DN_ORGANIZATION_NAME), "Frank4DD") == 0);
  assert (strcmp (dsk_tls_x509_name_get_component (&cert->subject, DSK_TLS_X509_DN_COMMON_NAME), "www.example.com") == 0);

  assert (cert->subject_public_key_info.algorithm == DSK_TLS_X509_SIGNATURE_ALGORITHM_RSA_PSS);
  assert (cert->subject_public_key_info.public_key_length == 592 / 8);
  // NOTE Just test the start of the public_key.
  assert (memcmp (cert->subject_public_key_info.public_key,
                  "\x30\x48\x02\x41\x00\x9b\xfc\x66\x90\x79\x84",
		  10) == 0);
  assert (cert->is_signed);
  assert (cert->signature_algorithm == DSK_TLS_X509_SIGNATURE_ALGORITHM_RSA_PKCS1_SHA1);
  assert (cert->signature_length == 1024/8);
  // NOTE Just test the start of the signature.
  assert (memcmp (cert->signature_data,
             "\x14\xb6\x4c\xbb\x81\x79\x33\xe6\x71\xa4\xda\x51\x6f\xcb\x08\x1d",
                  16) == 0);

  dsk_mem_pool_clear (&pool);
  dsk_object_unref (cert);
}


static struct 
{
  const char *name;
  void (*test)(void);
} tests[] =
{
  { "DER 512 x509", test_cert },
};

int main(int argc, char **argv)
{
  dsk_cmdline_init ("test x509 handling",
                    "Test x509 parsing",
                    NULL, 0);
  dsk_cmdline_add_boolean ("verbose", "extra logging", NULL, 0,
                           &cmdline_verbose);
  dsk_cmdline_process_args (&argc, &argv);

  for (unsigned i = 0; i < DSK_N_ELEMENTS (tests); i++)
    {
      fprintf (stderr, "Test: %s... ", tests[i].name);
      tests[i].test ();
      fprintf (stderr, " done.\n");
    }

  dsk_cleanup ();
  
  return 0;
}
