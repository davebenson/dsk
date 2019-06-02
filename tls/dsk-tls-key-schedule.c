#include "../dsk.h"
#include <stdlib.h>
#include <alloca.h>
#include <string.h>

#define SCHEDULE_MEMBER_OFFSET(member) \
  offsetof(DskTlsKeySchedule, member)

static size_t hash_len_offset[] =
{
  SCHEDULE_MEMBER_OFFSET(early_secret),
  SCHEDULE_MEMBER_OFFSET(binder_key),
  SCHEDULE_MEMBER_OFFSET(client_early_traffic_secret),
  SCHEDULE_MEMBER_OFFSET(early_exporter_master_secret),
  SCHEDULE_MEMBER_OFFSET(early_derived_secret),
  SCHEDULE_MEMBER_OFFSET(handshake_secret),
  SCHEDULE_MEMBER_OFFSET(client_handshake_traffic_secret),
  SCHEDULE_MEMBER_OFFSET(server_handshake_traffic_secret),
  SCHEDULE_MEMBER_OFFSET(handshake_derived_secret),
  SCHEDULE_MEMBER_OFFSET(verify_data),
  SCHEDULE_MEMBER_OFFSET(master_secret),
  SCHEDULE_MEMBER_OFFSET(client_application_traffic_secret),
  SCHEDULE_MEMBER_OFFSET(server_application_traffic_secret),
  SCHEDULE_MEMBER_OFFSET(exporter_master_secret),
  SCHEDULE_MEMBER_OFFSET(resumption_master_secret),
};
static size_t key_len_offset[] =
{
  SCHEDULE_MEMBER_OFFSET(server_handshake_write_key),
  SCHEDULE_MEMBER_OFFSET(client_handshake_write_key),
  SCHEDULE_MEMBER_OFFSET(server_application_write_key),
  SCHEDULE_MEMBER_OFFSET(client_application_write_key),
};
static size_t iv_len_offset[] =
{
  SCHEDULE_MEMBER_OFFSET(server_handshake_write_iv),
  SCHEDULE_MEMBER_OFFSET(client_handshake_write_iv),
  SCHEDULE_MEMBER_OFFSET(server_application_write_iv),
  SCHEDULE_MEMBER_OFFSET(client_application_write_iv),
};

DskTlsKeySchedule *
dsk_tls_key_schedule_new   (DskTlsCipherSuite *suite)
{
  unsigned total_size = sizeof (DskTlsKeySchedule)
                      + DSK_N_ELEMENTS (hash_len_offset) * suite->hash_size
                      + DSK_N_ELEMENTS (key_len_offset) * suite->key_size
                      + DSK_N_ELEMENTS (iv_len_offset) * suite->iv_size;

  DskTlsKeySchedule *rv = dsk_malloc(total_size);
  rv->cipher = suite;
  rv->has_early_secrets = 0;
  rv->has_handshake_secrets = 0;
  rv->has_master_secrets = 0;
  rv->has_finish_data = 0;
  uint8_t *at = (uint8_t *) (rv + 1);
  for (unsigned i = 0; i < DSK_N_ELEMENTS (hash_len_offset); i++)
    {
      * (uint8_t **) ((char *) rv + hash_len_offset[i]) = at;
      at += suite->hash_size;
    }
  for (unsigned i = 0; i < DSK_N_ELEMENTS (key_len_offset); i++)
    {
      * (uint8_t **) ((char *) rv + key_len_offset[i]) = at;
      at += suite->key_size;
    }
  for (unsigned i = 0; i < DSK_N_ELEMENTS (iv_len_offset); i++)
    {
      * (uint8_t **) ((char *) rv + iv_len_offset[i]) = at;
      at += suite->iv_size;
    }
  return rv;
}

void
dsk_tls_key_schedule_compute_early_secrets (DskTlsKeySchedule *schedule,
                                            bool               externally_shared,
                                            const uint8_t     *client_hello_hash,
                                            const uint8_t     *opt_pre_shared_key)
{
  unsigned hash_size = schedule->cipher->hash_size;
  uint8_t *zero_salt = alloca (hash_size);
  memset (zero_salt, 0, hash_size);
  const uint8_t *psk = opt_pre_shared_key != NULL ? opt_pre_shared_key : zero_salt;
  dsk_hkdf_extract (schedule->cipher->hash_type,
                    hash_size, zero_salt,
                    hash_size, psk,
                    schedule->early_secret);
printf("early secret:");for(unsigned i = 0; i < 32; i++)printf(" %02x", schedule->early_secret[i]);printf("\n");

  dsk_tls_derive_secret (schedule->cipher->hash_type,
                         schedule->early_secret,
                         10, (uint8_t *) (externally_shared ? "ext binder" : "res binder"),
                         schedule->cipher->hash_type->empty_hash,
                         schedule->binder_key);
  dsk_tls_derive_secret (schedule->cipher->hash_type,
                         schedule->early_secret,
                         11, (uint8_t *) "c e traffic",
                         client_hello_hash,
                         schedule->client_early_traffic_secret);
  dsk_tls_derive_secret (schedule->cipher->hash_type,
                         schedule->early_secret,
                         12, (uint8_t *) "e exp master",
                         client_hello_hash,
                         schedule->early_exporter_master_secret);
  dsk_tls_derive_secret (schedule->cipher->hash_type,
                         schedule->early_secret,
                         7, (uint8_t *) "derived",
                         schedule->cipher->hash_type->empty_hash,
                         schedule->early_derived_secret);
  schedule->has_early_secrets = true;
}

void
dsk_tls_key_schedule_compute_handshake_secrets (DskTlsKeySchedule *schedule,
                                                const uint8_t     *server_hello_hash,
                                                unsigned           key_share_len,
                                                const uint8_t     *key_share)
{
  assert(schedule->has_early_secrets);
  unsigned hash_size = schedule->cipher->hash_size;

  dsk_hkdf_extract (schedule->cipher->hash_type,
                    hash_size, schedule->early_derived_secret,
                    key_share_len, key_share,
                    schedule->handshake_secret);
  dsk_tls_derive_secret (schedule->cipher->hash_type,
                         schedule->handshake_secret,
                         12, (uint8_t *) "c hs traffic",
                         server_hello_hash,
                         schedule->client_handshake_traffic_secret);
  dsk_tls_derive_secret (schedule->cipher->hash_type,
                         schedule->handshake_secret,
                         12, (uint8_t *) "s hs traffic",
                         server_hello_hash,
                         schedule->server_handshake_traffic_secret);
  dsk_tls_derive_secret (schedule->cipher->hash_type,
                         schedule->handshake_secret,
                         7, (uint8_t *) "derived",
                         schedule->cipher->hash_type->empty_hash,
                         schedule->handshake_derived_secret);

  schedule->has_handshake_secrets = true;
}

void
dsk_tls_key_schedule_compute_finished_data (DskTlsKeySchedule *schedule,
                                            const uint8_t     *certificate_verify_hash)
{
  assert(schedule->has_handshake_secrets);

  unsigned hash_size = schedule->cipher->hash_size;
  uint8_t *finished_key = alloca (hash_size);
  dsk_tls_hkdf_expand_label (schedule->cipher->hash_type,
                             schedule->server_handshake_traffic_secret,
                             8, (uint8_t *) "finished",
                             0, (uint8_t*) "",
                             32, finished_key);
  dsk_hmac_digest (schedule->cipher->hash_type,
                   hash_size, finished_key,
                   hash_size, certificate_verify_hash,
                   schedule->verify_data);
  schedule->has_finish_data = true;
}

void
dsk_tls_key_schedule_compute_master_secrets (DskTlsKeySchedule *schedule,
                                             const uint8_t     *finished_hash)
{
  assert(schedule->has_finish_data && schedule->has_handshake_secrets);
  unsigned hash_size = schedule->cipher->hash_size;
  uint8_t *zero_salt = alloca (hash_size);
  memset (zero_salt, 0, hash_size);
  printf("handshake_derived_secret: "); for (unsigned i = 0; i < 32;i++)printf(" %02x",schedule->handshake_derived_secret[i]);printf("\n");
  dsk_hkdf_extract (schedule->cipher->hash_type,
                    hash_size, schedule->handshake_derived_secret,
                    hash_size, zero_salt,
                    schedule->master_secret);
  dsk_tls_derive_secret (schedule->cipher->hash_type,
                         schedule->master_secret,
                         12, (uint8_t *) "c ap traffic",
                         finished_hash,
                         schedule->client_application_traffic_secret);
  dsk_tls_derive_secret (schedule->cipher->hash_type,
                         schedule->master_secret,
                         12, (uint8_t *) "s ap traffic",
                         finished_hash,
                         schedule->server_application_traffic_secret);
  dsk_tls_derive_secret (schedule->cipher->hash_type,
                         schedule->master_secret,
                         10, (uint8_t *) "exp master",
                         finished_hash,
                         schedule->exporter_master_secret);
  dsk_tls_derive_secret (schedule->cipher->hash_type,
                         schedule->master_secret,
                         10, (uint8_t *) "res master",
                         finished_hash,
                         schedule->resumption_master_secret);
  schedule->has_master_secrets = true;
}
