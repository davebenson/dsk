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
ffdhe__make_private_key  (DskTlsKeyShareMethod *method,
                          uint8_t              *private_key_inout)
{
  DskTlsKeyShareMethod_FFDHE *f = (DskTlsKeyShareMethod_FFDHE *) method;
  dsk_tls_ffdhe_gen_private_key (f->ffdhe, private_key_inout);
  return true;
}

static void 
ffdhe__make_public_key   (DskTlsKeyShareMethod *method,
                          const uint8_t        *private_key,
                          uint8_t              *public_key_out)
{
  DskTlsKeyShareMethod_FFDHE *f = (DskTlsKeyShareMethod_FFDHE *) method;
  dsk_tls_ffdhe_compute_public_key (f->ffdhe, private_key, public_key_out);
}

static void
ffdhe__make_shared_key   (DskTlsKeyShareMethod *method,
                          const uint8_t        *private_key,
                          const uint8_t        *peer_public_key,
                          uint8_t              *shared_key_out)
{
  DskTlsKeyShareMethod_FFDHE *f = (DskTlsKeyShareMethod_FFDHE *) method;
  dsk_tls_ffdhe_compute_shared_key (f->ffdhe, private_key, peer_public_key, shared_key_out);
}

//
// NOTES on FFDHE:
//   - RFC 7919 gives the key-lengths in bits L;
//     we round up to bits using the formula (L+7)/8.
//
static DskTlsKeyShareMethod_FFDHE key_share_method__ffdhe2048 =
{
  {
    DSK_TLS_NAMED_GROUP_FFDHE2048,
    "FFDHE-2048",
    (225+7)/8,
    256,
    256,
    ffdhe__make_private_key,
    ffdhe__make_public_key,
    ffdhe__make_shared_key
  },
  &dsk_tls_ffdhe2048
};

static DskTlsKeyShareMethod_FFDHE key_share_method__ffdhe3072 =
{
  {
    DSK_TLS_NAMED_GROUP_FFDHE3072,
    "FFDHE-3072",
    (275+7)/8,
    384,
    384,
    ffdhe__make_private_key,
    ffdhe__make_public_key,
    ffdhe__make_shared_key
  },
  &dsk_tls_ffdhe3072
};
static DskTlsKeyShareMethod_FFDHE key_share_method__ffdhe4096 =
{
  {
    DSK_TLS_NAMED_GROUP_FFDHE4096,
    "FFDHE-4096",
    (325+7)/8,
    512,
    512,
    ffdhe__make_private_key,
    ffdhe__make_public_key,
    ffdhe__make_shared_key
  },
  &dsk_tls_ffdhe4096
};
static DskTlsKeyShareMethod_FFDHE key_share_method__ffdhe6144 =
{
  {
    DSK_TLS_NAMED_GROUP_FFDHE6144,
    "FFDHE-6144",
    (375+7)/8,
    768,
    768,
    ffdhe__make_private_key,
    ffdhe__make_public_key,
    ffdhe__make_shared_key
  },
  &dsk_tls_ffdhe6144
};
static DskTlsKeyShareMethod_FFDHE key_share_method__ffdhe8192 =
{
  {
    DSK_TLS_NAMED_GROUP_FFDHE8192,
    "FFDHE-8192",
    (400+7)/8,
    1024,
    1024,
    ffdhe__make_private_key,
    ffdhe__make_public_key,
    ffdhe__make_shared_key
  },
  &dsk_tls_ffdhe8192
};

//
// Curve 25519 support.
//
static bool 
curve25519__make_private_key  (DskTlsKeyShareMethod *method,
                               uint8_t              *private_key_inout)
{
  (void) method;
  dsk_curve25519_random_to_private (private_key_inout);
  return true;
}

static void 
curve25519__make_public_key   (DskTlsKeyShareMethod *method,
                               const uint8_t        *private_key,
                               uint8_t              *public_key_out)
{
  (void) method;
  dsk_curve25519_private_to_public (private_key, public_key_out);
}

static void
curve25519__make_shared_key   (DskTlsKeyShareMethod *method,
                               const uint8_t        *private_key,
                               const uint8_t        *peer_public_key,
                               uint8_t              *shared_key_out)
{
  (void) method;
  dsk_curve25519_private_to_shared (private_key, peer_public_key, shared_key_out);
}

static DskTlsKeyShareMethod key_share_method__curve25519 =
{
  DSK_TLS_NAMED_GROUP_X25519,
  "CURVE-25519",
  32,
  32,
  32,
  curve25519__make_private_key,
  curve25519__make_public_key,
  curve25519__make_shared_key
};


DskTlsKeyShareMethod *
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

      case DSK_TLS_NAMED_GROUP_X448:              // TODO
      default:
        return NULL;
    }
}

