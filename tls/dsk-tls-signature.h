

  // RFC 8848, Section 4.4.3:
  //
  // The digital signature is then
  //     computed over the concatenation of:
  //     -  A string that consists of octet 32 (0x20) repeated 64 times
  //     -  The context string
  //     -  A single 0 byte which serves as the separator
  //     -  The content to be signed ==
  //                Transcript-Hash(Handshake Context, Certificate)
  //
  // The context string for a server signature is
  //           TLS 1.3, server CertificateVerify
void dsk_tls_signature_compute (DskTlsSignatureScheme scheme,
                                bool                  is_server,
                                const uint8_t        *transcript_hash,
                                uint8_t              *signature_out);

