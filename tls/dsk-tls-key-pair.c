#include "../dsk.h"

DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskTlsKeyPair);
DskTlsKeyPairClass dsk_tls_key_pair_class =
{
  DSK_OBJECT_CLASS_DEFINE(
    DskTlsKeyPair,
    &dsk_object_class,
    NULL,             // init
    NULL              // finalize
  ),
  NULL,   // get_signature_length
  NULL,   // has_private_key
  NULL,   // supports_scheme
  NULL,   // sign
  NULL,   // verify
};
