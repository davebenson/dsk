// HMAC is a system for computing a digest.
//
// See RFC 2104. "HMAC: Keyed-Hashing for Message Authentication"
//

void dsk_hmac_digest (DskChecksumType type,
                      size_t          key_len,
                      const uint8_t  *key,
                      size_t          text_len,
                      const uint8_t  *text,
                      uint8_t        *digest_out);
