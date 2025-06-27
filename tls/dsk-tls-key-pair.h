//
// A Key-Pair is a public-key and a private-key.
// This is asymmetric crypto in a nutshell.
//
// A DskTlsKeyPair instance may be just a public-key,
// or the pair.
//
// Without a private-key, signing is not possible
// (that's the whole point of the key-pair system).
//

//
// There are also asymmetric encryption schemes
// (where the public key is for encryption
// and the private key is for decryption),
// but they aren't used by TLS, so we omit them
// from our abstraction.
//

typedef struct DskTlsKeyPairClass DskTlsKeyPairClass;
typedef struct DskTlsKeyPair DskTlsKeyPair;

#define DSK_TLS_KEY_PAIR(object) DSK_OBJECT_CAST(DskTlsKeyPair, object, &dsk_tls_key_pair_class)
#define DSK_TLS_KEY_PAIR_GET_CLASS(object) DSK_OBJECT_CAST_GET_CLASS(DskTlsKeyPair, object, &dsk_tls_key_pair_class)

struct DskTlsKeyPairClass
{
  DskObjectClass base_class;
  size_t (*get_signature_length) (DskTlsKeyPair     *kp,
                                  DskTlsSignatureScheme scheme);
  bool   (*has_private_key)      (DskTlsKeyPair     *kp);
  void   (*sign)                 (DskTlsKeyPair     *kp,
                                  DskTlsSignatureScheme algo,
                                  size_t             content_len,
                                  const uint8_t     *content_data,
                                  uint8_t           *signature_out);
  bool   (*verify)               (DskTlsKeyPair     *kp,
                                  DskTlsSignatureScheme algo,
                                  size_t             content_len,
                                  const uint8_t     *content_data,
                                  const uint8_t     *sig);
};
struct DskTlsKeyPair
{
  DskObject base_instance;
  size_t n_supported_schemes;
  const DskTlsSignatureScheme *supported_schemes;
};

bool   dsk_tls_key_pair_supports_scheme (DskTlsKeyPair     *kp,
                                         DskTlsSignatureScheme algorithm);
size_t dsk_tls_key_pair_get_hash_length (DskTlsKeyPair     *kp,
                                         DskTlsSignatureScheme scheme);
size_t dsk_tls_key_pair_get_signature_length
                                        (DskTlsKeyPair     *kp,
                                         DskTlsSignatureScheme scheme);
bool   dsk_tls_key_pair_has_private_key (DskTlsKeyPair     *kp);
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
#define DSK_TLS_KEY_PAIR_DEFINE_CLASS(CN, cn) \
  {                                           \
    DSK_OBJECT_CLASS_DEFINE(                  \
      CN,                                     \
      &dsk_tls_key_pair_class,                \
      cn ## _init,                            \
      cn ## _finalize                         \
    ),                                        \
    cn ## _get_signature_length,              \
    cn ## _has_private_key,                   \
    cn ## _supports_scheme,                   \
    cn ## _sign,                              \
    cn ## _verify                             \
  }

extern DskTlsKeyPairClass dsk_tls_key_pair_class;
