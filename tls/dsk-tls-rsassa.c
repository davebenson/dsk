
// start of rfc3447, rfc4055.

// From RFC 3447 Section B.2.1.
static void
mgf1 (unsigned seed_len,
      const uint8_t *seed,
      DskChecksumType *hash_type,
      unsigned mask_len,
      uint8_t *mask_out)
{
  uint32_t counter;
  unsigned whole = mask_len / hash_type->hash_size;
  void *seeded_instance = alloca (hash_type->instance_size);
  hash_type->init (seeded_instance);
  hash_type->feed (seeded_instance, seed_len, seed);
  void *instance = alloca (hash_type->instance_size);
  unsigned hash_size = hash_type->hash_size;
  uint8_t *hash = alloca (hash_size);
  for (counter = 0; counter < whole; counter++)
    {
      uint8_t c[4];
      dsk_uint32be_pack (counter, c);
      memcpy (instance, seeded_instance, hash_size->instance_size);
      hash_type->feed (instance, 4, c);
      hash_type->end (instance, hash);
      memcpy (mask_out + counter * hash_size, hash, hash_size);
    }
  if (counter * hash_size < mask_len)
    {
      uint8_t c[4];
      dsk_uint32be_pack (counter, c);
      memcpy (instance, seeded_instance, hash_size->instance_size);
      hash_type->feed (instance, 4, c);
      hash_type->end (instance, hash);
      memcpy (mask_out + counter * hash_size, hash, mask_len - counter * hash_size);
   }
}

// From RFC 3447 Section 9.1.1
static void
emsa_pss_encode (unsigned message_len,
                 const uint8_t *message,
                 DskChecksumType *hash_type,
                 unsigned salt_len,
                 unsigned em_len,
                 uint8_t *em_out)
{
  uint8_t *message_hash = alloca (hash_type->hash_size);
  void *hash_instance = alloca (hash_type->instance_size);

  // Step 2:  Compute mHash.
  hash_type->init (hash_instance);
  hash_type->feed (hash_instance, message_len, message);
  hash_type->end (hash_instance, message_hash);

  // Step 3: Check output length for validity.
  assert (em_len >= hash_type->hash_size + salt_len + 2);

  uint8_t *salt = alloca (salt_len);
  if (salt_len > 0)
    dsk_get_cryptorandom_data (salt_len, salt);

  // Step 6: Compute H = hash(00 00 00 00 00 00 00 00 || message_hash || salt)
  uint8_t zeroes[8] = {0,0,0,0,0,0,0,0};
  hash_type->init (hash_instance);
  hash_type->feed (hash_instance, 8, zeroes);
  hash_type->feed (hash_instance, hash_type->hash_size, message_hash);
  hash_type->feed (hash_instance, salt_len, salt);
  uint8_t *H = alloca (hash_type->hash_size);
  hash_type->end (hash_instance, H);

  // Step 7 + 8: Compute DB == 0 x (emLen-saltlen-hash_len-2) || 0x01 || salt
  unsigned db_len = em_len - hash_type->hash_size - 1;
  uint8_t *db = alloca (db_len);
  memset (db, 0, em_len - salt_len - hash_type->hash_size);
  db[em_len - salt_len - hash_type->hash_size] = 1;
  memcpy (db + em_len-salt_len-hash_type->hash_size + 1, salt, salt_len);

  // Step 9: Compute db_mask
  uint8_t *db_mask = alloca (db_len);
  mgf1 (hash_type->hash_size, H, hash_type, db_len, db_mask);

  // Step 10: Compute masked_db as first bytes of output.
  for (unsigned i = 0; i < db_len; i++)
    em_out[i] = db[i] ^ db_mask[i];

  // Step 12: add H and 0xBC.
  memcpy (em_out + db_len, H, hash_type->hash_size);
  em_out[db_len + hash_type->hash_size] = 0xbc;
}

// RFC 3447 Section 5.2.1.
static void
rsasp1 (DskRSAPrivateKey *private_key,
        uint32_t         *m,
        uint32_t         *s_out)
{
  m^dP mod p
  m^dQ mod q
...
}

// RFC 3447 Section 8.1.1.
void dsk_tls_rsassa_pss_sign (DskRSAPrivateKey    *private_key,
                              size_t               message_length,
                              const uint8_t       *message,
                              DskChecksumType     *hash_type,
                              unsigned             salt_len,
                              uint8_t             *signature_out)
{
  //
  // Step 1: Compute EM.
  //
  unsigned em_len = private_key->modulus_len * 4;
  uint8_t *em = alloca (em_len);
  emsa_pss_encode (message_length, message,
                   hash_type,
                   salt_len,
                   em_len,
                   em);

  //
  // Step 2.  RSA Signature
  //
  uint32_t *m = ...;
  rsasp1 (private_key, m, s);
  ... copy s to signature_out (big-endian)
}
