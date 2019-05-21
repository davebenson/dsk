

struct DskTlsSigner
{
  unsigned signature_length;
  void (*sign)(DskTlsSigner    *signer
               size_t           len,
               const uint8_t   *data,
               uint8_t        *sig_out);
  void (*destroy) (DskTlsSigner *signer);
};


DskTlsSigner *dsk_tls_signer_new (DskTlsSignatureScheme scheme,
                                  size_t                keypair_len,
                                  const uint8_t        *keypair_data,
                                  DskError            **error);


