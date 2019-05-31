#include "../dsk.h"
#include <stdlib.h>
#include <alloca.h>
#include <string.h>

static size_t hash_len_offset[] =
{
  offsetof(...),
  ...
};
static size_t key_len_offset[] =
{
  offsetof(...),
  ...
};
static size_t iv_len_offset[] =
{
  offsetof(...),
  ...
};

DskTlsKeySchedule *
dsk_tls_key_schedule_alloc (DskTlsKeySchedule *schedule,
                            DskTlsCipherSuite *suite)
{
  unsigned total_size = sizeof (DskTlsKeySchedule)
                      + DSK_N_ELEMENTS (hash_len_offset) * schedule->hash_type->hash_size
                      + DSK_N_ELEMENTS (key_len_offset) * suite->tls_key_length
                      + DSK_N_ELEMENTS (iv_len_offset) * suite->tls_iv_length;
  ...
}

void
dsk_tls_key_schedule_compute_early_secrets (DskTlsKeySchedule *schedule,
                                            bool               externally_shared,
                                            const uint8_t     *client_hello_hash,
                                            const uint8_t     *opt_pre_shared_key)
{
  unsigned hash_length = schedule->hash_length;
  uint8_t *zero_salt = alloca (hash_length);
  memset (zero_salt, 0, hash_length);
  const uint8_t *psk = opt_pre_shared_key != NULL ? opt_pre_shared_key : zero_salt;
  dsk_hkdf_extract (schedule->hash_type, 32, zero_salt, 32, psk, schedule->early_secret);

  dsk_tls_derive_secret (schedule->hash_type,
                         schedule->early_secret,
                         10, (uint8_t *) (externally_shared ? "ext binder" : "res binder"),
                         0,
                         schedule->binder_key);
  dsk_tls_derive_secret (schedule->hash_type,
                         schedule->early_secret,
                         11, (uint8_t *) "c e traffic",
                         client_hello_hash,
                         schedule->client_early_traffic_secret);
  dsk_tls_derive_secret (schedule->hash_type,
                         schedule->early_secret,
                         12, (uint8_t *) "e exp master",
                         client_hello_hash,
                         schedule->early_exporter_master_secret);
  dsk_tls_derive_secret (schedule->hash_type,
                         schedule->early_secret,
                         7, (uint8_t *) "derived",
                         0,
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
  unsigned hash_length = schedule->hash_length;

  dsk_hkdf_extract (schedule->hash_type,
                    hash_length, schedule->early_derived_secret,
                    key_share_len, key_share,
                    schedule->handshake_secret);
  dsk_tls_derive_secret (schedule->hash_type,
                         schedule->handshake_secret,
                         12, (uint8_t *) "c hs traffic",
                         server_hello_hash,
                         schedule->client_handshake_traffic_secret);
  dsk_tls_derive_secret (schedule->hash_type,
                         schedule->handshake_secret,
                         12, (uint8_t *) "s hs traffic",
                         server_hello_hash,
                         schedule->server_handshake_traffic_secret);
  dsk_tls_derive_secret (schedule->hash_type,
                         schedule->handshake_secret,
                         7, (uint8_t *) "derived",
                         0,
                         schedule->handshake_derived_secret);

  schedule->has_handshake_secrets = true;
}

void
dsk_tls_key_schedule_compute_finished_data (DskTlsKeySchedule *schedule,
                                            const uint8_t     *certificate_verify_hash)
{
  assert(schedule->has_handshake_secrets);

  unsigned hash_length = schedule->hash_length;
  uint8_t *finished_key = alloca (hash_length);
  dsk_tls_hkdf_expand_label (schedule->hash_type,
                             schedule->server_handshake_traffic_secret,
                             8, (uint8_t *) "finished",
                             0, (uint8_t*) "",
                             32, finished_key);
  dsk_hmac_digest (schedule->hash_type,
                   hash_length, finished_key,
                   hash_length, certificate_verify_hash,
                   schedule->verify_data);
  schedule->has_finish_data = true;
}

void
dsk_tls_key_schedule_compute_master_secrets (DskTlsKeySchedule *schedule,
                                             const uint8_t     *finished_hash)
{
  assert(schedule->has_finish_data && schedule->has_handshake_secrets);
  unsigned hash_length = schedule->hash_length;
  uint8_t *zero_salt = alloca (hash_length);
  memset (zero_salt, 0, hash_length);
  dsk_hkdf_extract (schedule->hash_type,
                    hash_length, schedule->handshake_derived_secret,
                    hash_length, zero_salt,
                    schedule->master_secret);
  dsk_tls_derive_secret (schedule->hash_type,
                         schedule->master_secret,
                         12, (uint8_t *) "c ap traffic",
                         finished_hash,
                         schedule->client_application_traffic_secret_0);
  dsk_tls_derive_secret (schedule->hash_type,
                         schedule->master_secret,
                         12, (uint8_t *) "s ap traffic",
                         finished_hash,
                         schedule->server_application_traffic_secret_0);
  dsk_tls_derive_secret (schedule->hash_type,
                         schedule->master_secret,
                         10, (uint8_t *) "exp master",
                         finished_hash,
                         schedule->exporter_master_secret);
  dsk_tls_derive_secret (schedule->hash_type,
                         schedule->master_secret,
                         10, (uint8_t *) "res master",
                         finished_hash,
                         schedule->resumption_master_secret);
  schedule->has_master_secrets = true;
}
