

typedef struct DskTlsKeyShareMethod DskTlsKeyShareMethod;
struct DskTlsKeyShareMethod
{
  DskTlsNamedGroup named_group;
  const char *key_share_method_name;

  unsigned private_key_bytes;
  unsigned public_key_bytes;
  unsigned shared_key_bytes;

  //
  // On input, private_key_inout must be initialized with
  // private_key_bytes bytes of cryptographically random data.
  //
  // If the input is insufficient (extremely unlikely), this returns false.
  // Callers should refill private_key_inout with random data
  // and re-invoke make_private_key.
  //
  bool (*make_key_pair)    (DskTlsKeyShareMethod *method,
                            uint8_t              *private_key_inout,
                            uint8_t              *public_key_out);


  //
  // Take your private key and the other user's public key,
  // and generate a shared key.
  //
  //    MAKE_SHARED_KEY(private_key, peer.public_key)
  //                          ==
  //    MAKE_SHARED_KEY(peer.private_key, public_key)
  //
  //
  // Because the private_key cannot be calculated from the public_key,
  // and b/c MAKE_SHARED_KEY() cannot be calculated
  // without the private_key, the computed shared_key
  // cannot be calculated from the public keys.
  //
  bool (*make_shared_key)  (DskTlsKeyShareMethod *method,
                            const uint8_t        *private_key,
                            const uint8_t        *peer_public_key,
                            uint8_t              *shared_key_out);

};

DskTlsKeyShareMethod *
dsk_tls_key_share_method_by_group (DskTlsNamedGroup group);


// Returns the index of our preferred group,
// or -1 if none matches.
int
dsk_tls_key_share_method_get_best (size_t      n_groups,
                                   const DskTlsNamedGroup *groups);
