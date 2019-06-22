#include "../dsk.h"


/*
 * Finite-Field Diffie-Hellman Exchange.
 *
 * Use the intractability of the discrete-log problem
 * to implement a key-sharing protocol.
 */
typedef struct DskTlsKeyShareMethod_FFDHE DskTlsKeyShareMethod_FFDHE;
struct DskTlsKeyShareMethod_FFDHE
{
  DskTlsKeyShareMethod base;
  DskTlsFFDHE *ffdhe;
};

static bool 
ffdhe__make_key_pair  (DskTlsKeyShareMethod *method,
                       uint8_t              *private_key_inout,
                       uint8_t              *public_key_out)
{
  DskTlsKeyShareMethod_FFDHE *f = (DskTlsKeyShareMethod_FFDHE *) method;
  dsk_tls_ffdhe_gen_private_key (f->ffdhe, private_key_inout);
  dsk_tls_ffdhe_compute_public_key (f->ffdhe, private_key_inout, public_key_out);
  return true;
}

static bool
ffdhe__make_shared_key   (DskTlsKeyShareMethod *method,
                          const uint8_t        *private_key,
                          const uint8_t        *peer_public_key,
                          uint8_t              *shared_key_out)
{
  DskTlsKeyShareMethod_FFDHE *f = (DskTlsKeyShareMethod_FFDHE *) method;
  dsk_tls_ffdhe_compute_shared_key (f->ffdhe, private_key, peer_public_key, shared_key_out);
  return true;
}

//
// NOTES on FFDHE:
//   - RFC 7919 gives the key-lengths in bits L;
//     we round up to bits using the formula (L+7)/8.
//
const static DskTlsKeyShareMethod_FFDHE key_share_method__ffdhe2048 =
{
  {
    DSK_TLS_NAMED_GROUP_FFDHE2048,
    "FFDHE-2048",
    (225+7)/8,
    256,
    256,
    ffdhe__make_key_pair,
    ffdhe__make_shared_key
  },
  &dsk_tls_ffdhe2048
};

const static DskTlsKeyShareMethod_FFDHE key_share_method__ffdhe3072 =
{
  {
    DSK_TLS_NAMED_GROUP_FFDHE3072,
    "FFDHE-3072",
    (275+7)/8,
    384,
    384,
    ffdhe__make_key_pair,
    ffdhe__make_shared_key
  },
  &dsk_tls_ffdhe3072
};
const static DskTlsKeyShareMethod_FFDHE key_share_method__ffdhe4096 =
{
  {
    DSK_TLS_NAMED_GROUP_FFDHE4096,
    "FFDHE-4096",
    (325+7)/8,
    512,
    512,
    ffdhe__make_key_pair,
    ffdhe__make_shared_key
  },
  &dsk_tls_ffdhe4096
};
const static DskTlsKeyShareMethod_FFDHE key_share_method__ffdhe6144 =
{
  {
    DSK_TLS_NAMED_GROUP_FFDHE6144,
    "FFDHE-6144",
    (375+7)/8,
    768,
    768,
    ffdhe__make_key_pair,
    ffdhe__make_shared_key
  },
  &dsk_tls_ffdhe6144
};
const static DskTlsKeyShareMethod_FFDHE key_share_method__ffdhe8192 =
{
  {
    DSK_TLS_NAMED_GROUP_FFDHE8192,
    "FFDHE-8192",
    (400+7)/8,
    1024,
    1024,
    ffdhe__make_key_pair,
    ffdhe__make_shared_key
  },
  &dsk_tls_ffdhe8192
};

//
// Curve 25519 support.
//
static bool 
curve25519__make_key_pair     (DskTlsKeyShareMethod *method,
                               uint8_t              *private_key_inout,
                               uint8_t              *public_key_out)
{
  (void) method;
  dsk_curve25519_random_to_private (private_key_inout);
  dsk_curve25519_private_to_public (private_key_inout, public_key_out);
  return true;
}

static bool
curve25519__make_shared_key   (DskTlsKeyShareMethod *method,
                               const uint8_t        *private_key,
                               const uint8_t        *peer_public_key,
                               uint8_t              *shared_key_out)
{
  (void) method;
  dsk_curve25519_private_to_shared (private_key, peer_public_key, shared_key_out);
  return true;
}

const static DskTlsKeyShareMethod key_share_method__curve25519 =
{
  DSK_TLS_NAMED_GROUP_X25519,
  "CURVE-25519",
  32,
  32,
  32,
  curve25519__make_key_pair,
  curve25519__make_shared_key
};

static bool 
curve448__make_key_pair     (DskTlsKeyShareMethod *method,
                               uint8_t              *private_key_inout,
                               uint8_t              *public_key_out)
{
  (void) method;
  dsk_curve448_random_to_private (private_key_inout);
  dsk_curve448_private_to_public (private_key_inout, public_key_out);
  return true;
}

static bool
curve448__make_shared_key   (DskTlsKeyShareMethod *method,
                             const uint8_t        *private_key,
                             const uint8_t        *peer_public_key,
                             uint8_t              *shared_key_out)
{
  (void) method;
  dsk_curve448_private_to_shared (private_key, peer_public_key, shared_key_out);
  return true;
}

const static DskTlsKeyShareMethod key_share_method__curve448 =
{
  DSK_TLS_NAMED_GROUP_X448,
  "CURVE-448",
  56,
  56,
  56,
  curve448__make_key_pair,
  curve448__make_shared_key
};


const DskTlsKeyShareMethod *
dsk_tls_key_share_method_by_group (DskTlsNamedGroup group)
{ 
  switch (group)
    {
      case DSK_TLS_NAMED_GROUP_FFDHE2048:
        return (DskTlsKeyShareMethod *) &key_share_method__ffdhe2048;

      case DSK_TLS_NAMED_GROUP_FFDHE3072:
        return (DskTlsKeyShareMethod *) &key_share_method__ffdhe3072;

      case DSK_TLS_NAMED_GROUP_FFDHE4096:
        return (DskTlsKeyShareMethod *) &key_share_method__ffdhe4096;

      case DSK_TLS_NAMED_GROUP_FFDHE6144:
        return (DskTlsKeyShareMethod *) &key_share_method__ffdhe6144;

      case DSK_TLS_NAMED_GROUP_FFDHE8192:
        return (DskTlsKeyShareMethod *) &key_share_method__ffdhe8192;

      case DSK_TLS_NAMED_GROUP_X25519:
        return &key_share_method__curve25519;

      case DSK_TLS_NAMED_GROUP_X448:
        return &key_share_method__curve448;

      case DSK_TLS_NAMED_GROUP_SECP256R1:
        return dsk_tls_key_share_secp256r1;

      case DSK_TLS_NAMED_GROUP_SECP384R1:
        return dsk_tls_key_share_secp384r1;

      case DSK_TLS_NAMED_GROUP_SECP521R1:
        return dsk_tls_key_share_secp521r1;

      default:
        return NULL;
    }
}

