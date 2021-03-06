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
  SCHEDULE_MEMBER_OFFSET(client_verify_data),
  SCHEDULE_MEMBER_OFFSET(server_verify_data),
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
  rv->has_client_finish_data = 0;
  rv->has_server_finish_data = 0;
  rv->has_resumption_secret = 0;
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
                                            size_t             opt_pre_shared_key_size,
                                            const uint8_t     *opt_pre_shared_key)
{
  unsigned hash_size = schedule->cipher->hash_size;
  uint8_t *zero_salt = alloca (hash_size);
  memset (zero_salt, 0, hash_size);
  unsigned psk_size = hash_size;
  const uint8_t *psk = zero_salt;
  if (opt_pre_shared_key != NULL)
    {
      psk_size = opt_pre_shared_key_size;
      psk = opt_pre_shared_key;
    }
  dsk_hkdf_extract (schedule->cipher->hash_type,
                    hash_size, zero_salt,
                    psk_size, psk,
                    schedule->early_secret);

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

  //
  // Client and Server Keys and IVs.
  //
  dsk_tls_hkdf_expand_label(schedule->cipher->hash_type,
                            schedule->client_handshake_traffic_secret,
                            3, (const uint8_t *) "key",
                            0, NULL,
                            schedule->cipher->key_size,
                            schedule->client_handshake_write_key);
  dsk_tls_hkdf_expand_label(&dsk_checksum_type_sha256,
                            schedule->client_handshake_traffic_secret,
                            2, (const uint8_t *) "iv",
                            0, NULL,
                            schedule->cipher->iv_size,
                            schedule->client_handshake_write_iv);
  dsk_tls_hkdf_expand_label(schedule->cipher->hash_type,
                            schedule->server_handshake_traffic_secret,
                            3, (const uint8_t *) "key",
                            0, NULL,
                            schedule->cipher->key_size,
                            schedule->server_handshake_write_key);
  dsk_tls_hkdf_expand_label(&dsk_checksum_type_sha256,
                            schedule->server_handshake_traffic_secret,
                            2, (const uint8_t *) "iv",
                            0, NULL,
                            schedule->cipher->iv_size,
                            schedule->server_handshake_write_iv);

  schedule->has_handshake_secrets = true;
}

void
dsk_tls_key_schedule_server_verify_data (DskTlsKeySchedule *schedule,
                                      const uint8_t *cv_hash)
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
                   hash_size, cv_hash,
                   schedule->server_verify_data);
  schedule->has_server_finish_data = true;
}

void
dsk_tls_key_schedule_compute_master_secrets (DskTlsKeySchedule *schedule,
                                             const uint8_t     *s_finished_hash)
{
  assert(schedule->has_server_finish_data && schedule->has_handshake_secrets);
  unsigned hash_size = schedule->cipher->hash_size;
  uint8_t *zero_salt = alloca (hash_size);
  memset (zero_salt, 0, hash_size);
  dsk_hkdf_extract (schedule->cipher->hash_type,
                    hash_size, schedule->handshake_derived_secret,
                    hash_size, zero_salt,
                    schedule->master_secret);
  dsk_tls_derive_secret (schedule->cipher->hash_type,
                         schedule->master_secret,
                         12, (uint8_t *) "c ap traffic",
                         s_finished_hash,
                         schedule->client_application_traffic_secret);
  dsk_tls_derive_secret (schedule->cipher->hash_type,
                         schedule->master_secret,
                         12, (uint8_t *) "s ap traffic",
                         s_finished_hash,
                         schedule->server_application_traffic_secret);
  dsk_tls_derive_secret (schedule->cipher->hash_type,
                         schedule->master_secret,
                         10, (uint8_t *) "exp master",
                         s_finished_hash,
                         schedule->exporter_master_secret);

  //
  // Server and Client Keys and IVs (for application traffic).
  //
  dsk_tls_hkdf_expand_label(schedule->cipher->hash_type,
                            schedule->client_application_traffic_secret,
                            3, (const uint8_t *) "key",
                            0, NULL,
                            schedule->cipher->key_size,
                            schedule->client_application_write_key);
  dsk_tls_hkdf_expand_label(&dsk_checksum_type_sha256,
                            schedule->client_application_traffic_secret,
                            2, (const uint8_t *) "iv",
                            0, NULL,
                            schedule->cipher->iv_size,
                            schedule->client_application_write_iv);
  dsk_tls_hkdf_expand_label(schedule->cipher->hash_type,
                            schedule->server_application_traffic_secret,
                            3, (const uint8_t *) "key",
                            0, NULL,
                            schedule->cipher->key_size,
                            schedule->server_application_write_key);
  dsk_tls_hkdf_expand_label(&dsk_checksum_type_sha256,
                            schedule->server_application_traffic_secret,
                            2, (const uint8_t *) "iv",
                            0, NULL,
                            schedule->cipher->iv_size,
                            schedule->server_application_write_iv);
  schedule->has_master_secrets = true;
}


void
dsk_tls_key_schedule_compute_resumption_secret (DskTlsKeySchedule *schedule,
                                             const uint8_t     *c_finished_hash)
{
  dsk_tls_derive_secret (schedule->cipher->hash_type,
                         schedule->master_secret,
                         10, (uint8_t *) "res master",
                         c_finished_hash,
                         schedule->resumption_master_secret);
}
