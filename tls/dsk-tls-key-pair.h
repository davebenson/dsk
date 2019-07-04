//
// A Key-Pair is a public-key and a private-key.
//
// A DskTlsKeyPair instance may be just a public-key,
// or the pair.  Without private-key, signing is not possible.
//


typedef struct DskTlsKeyPairClass DskTlsKeyPairClass;
typedef struct DskTlsKeyPair DskTlsKeyPair;

#define DSK_TLS_KEY_PAIR(object) DSK_OBJECT_CAST(DskKeyPair, object, &dsk_tls_key_pair_class)
#define DSK_TLS_KEY_PAIR_GET_CLASS(object) DSK_OBJECT_CAST_GET_CLASS(DskKeyPair, object, &dsk_tls_key_pair_class)

struct DskTlsKeyPairClass
{
  DskObjectClass base_class;
  size_t (*get_signature_length) (DskTlsKeyPair     *kp,
                                  DskTlsSignatureScheme scheme);
  bool   (*has_private_key)      (DskTlsKeyPair     *kp);
  bool   (*supports_scheme)      (DskTlsKeyPair     *kp,
                                  DskTlsSignatureScheme algorithm);
  void   (*sign)                 (DskTlsKeyPair     *kp,
                                  DskTlsSignatureScheme algo,
                                  size_t             content_len,
                                  const uint8_t     *content_data,
                                  uint8_t           *signature_out);
  void   (*verify)               (DskTlsKeyPair     *kp,
                                  DskTlsSignatureScheme algo,
                                  size_t             content_len,
                                  const uint8_t     *content_data,
                                  const uint8_t     *sig);
};
struct DskTlsKeyPair
{
  DskObject base_instance;
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

extern DskTlsKeyPairClass dsk_tls_key_pair_class;
