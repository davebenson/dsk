

//
// RFC 5869:  HMAC-based Key Derivation Function
//
// Build a secure key of any length
// from various keying materials,
// and a cryptographic hash function (an HMAC).
//

void dsk_hkdf_extract   (DskChecksumType*hash_type,
                         size_t          salt_len,
                         const uint8_t  *salt,
                         size_t          initial_key_len,
                         const uint8_t  *initial_key,
                         uint8_t        *pseudorandom_key_out);


void dsk_hkdf_expand    (DskChecksumType*hash_type,
                         const uint8_t  *pseudorandom_key,
                         size_t          info_len,
                         const uint8_t  *info,
                         unsigned        output_length,
                         uint8_t        *output_keying_material);

 
