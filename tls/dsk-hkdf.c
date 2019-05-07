#include "../dsk.h"
#include <alloca.h>
#include <string.h>

void dsk_hkdf_extract   (DskChecksumType hash_type,
                         size_t          salt_len,
                         const uint8_t  *salt,
                         size_t          initial_key_len,
                         const uint8_t  *initial_key,
                         uint8_t        *pseudorandom_key_out)
{
  void *H = alloca (dsk_checksum_sizeof_instance (hash_type));
  dsk_checksum_init (H, hash_type);
  dsk_hmac_digest (hash_type, salt_len, salt, initial_key_len, initial_key, pseudorandom_key_out);
}


void dsk_hkdf_expand    (DskChecksumType hash_type,
                         const uint8_t  *pseudorandom_key,
                         size_t          info_len,
                         const uint8_t  *info,
                         unsigned        output_length,
                         uint8_t        *output_keying_material)
{
  void *H = alloca (dsk_checksum_sizeof_instance (hash_type));
  size_t len = dsk_checksum_type_get_size (hash_type);
  uint8_t *T = alloca (len + info_len + 1);
  memcpy (T + len, info, info_len);
  T[len + info_len] = 1;
  dsk_hmac_digest (hash_type, len, pseudorandom_key, info_len + 1, T + len, T);


  if (output_length <= len)
    {
      memcpy (output_keying_material, T, output_length);
      return;
    }
  memcpy (output_keying_material, T, len);
  output_keying_material += len;
  output_length -= len;

  while (output_length >= len)
    {
      T[len + info_len] += 1;
      dsk_hmac_digest (hash_type, len, pseudorandom_key, len + info_len + 1, T, T);
      memcpy (output_keying_material, T, len);
      output_keying_material += len;
      output_length -= len;
    }
  if (output_length > 0)
    {
      T[len + info_len] += 1;
      dsk_hmac_digest (hash_type, len, pseudorandom_key, len + info_len + 1, T, T);
      memcpy (output_keying_material, T, output_length);
    }
}

 
