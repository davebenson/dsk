/* The remaining interface is relatively low-level;
 * generic key-share implementations should
 * probably use DskKeyShareMethod to handle
 * a range of key-sharing mechanisms instead.
 */



typedef struct DskTlsFFDHE DskTlsFFDHE;
struct DskTlsFFDHE
{
  const char *name;
  DskTlsMontgomeryInfo mont;
  unsigned key_bits;
};

extern DskTlsFFDHE dsk_tls_ffdhe2048;
extern DskTlsFFDHE dsk_tls_ffdhe3072;
extern DskTlsFFDHE dsk_tls_ffdhe4096;
extern DskTlsFFDHE dsk_tls_ffdhe6144;
extern DskTlsFFDHE dsk_tls_ffdhe8192;


//
// key_inout must be filled with (key_bits+7)/8 bytes of
// cryptographically secure random data,
// and it will be modified to be an acceptable key.
//
void dsk_tls_ffdhe_gen_private_key    (DskTlsFFDHE   *ffdhe,
                                       uint8_t       *key_inout);

//
// Generate a public key from a private key.
//
void dsk_tls_ffdhe_compute_public_key (DskTlsFFDHE   *ffdhe,
                                       const uint8_t *private_key,
                                       uint8_t       *public_key_out);

//
// Generate a shared key from my private key
// and someone else's public key.
// (They will get the same key from their private
// and my public key)
//
// It is a cryptographically-hard problem to get
// the shared key from both public keys, however.
//
void dsk_tls_ffdhe_compute_shared_key (DskTlsFFDHE   *ffdhe,
                                       const uint8_t *private_key,
                                       const uint8_t *other_public_key,
                                       uint8_t       *shared_key_out);

