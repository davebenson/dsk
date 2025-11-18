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


bool   dsk_tls_key_pair_supports_scheme (DskTlsKeyPair     *kp,
                                         DskTlsSignatureScheme algorithm)
{
  return DSK_TLS_KEY_PAIR_GET_CLASS (kp)->supports_scheme (kp, algorithm);
}

//size_t dsk_tls_key_pair_get_hash_length (DskTlsKeyPair     *kp,
//                                         DskTlsSignatureScheme scheme)
//{
//  return DSK_TLS_KEY_PAIR_GET_CLASS (kp)->get_hash_length (kp, scheme);
//}
//
size_t dsk_tls_key_pair_get_signature_length
                                        (DskTlsKeyPair     *kp,
                                         DskTlsSignatureScheme scheme)
{
  return DSK_TLS_KEY_PAIR_GET_CLASS (kp)->get_signature_length (kp, scheme);
}
bool   dsk_tls_key_pair_has_private_key (DskTlsKeyPair     *kp)
{
  return DSK_TLS_KEY_PAIR_GET_CLASS (kp)->has_private_key (kp);
}
void   dsk_tls_key_pair_sign            (DskTlsKeyPair     *kp,
                                         DskTlsSignatureScheme scheme,
                                         size_t             content_len,
                                         const uint8_t     *content_data,
                                         uint8_t           *signature_out);
void   dsk_tls_key_pair_verify          (DskTlsKeyPair     *kp,
                                         DskTlsSignatureScheme scheme,
                                         size_t             content_len,
                                         const uint8_t     *content_data,
                                         const uint8_t     *sig);
