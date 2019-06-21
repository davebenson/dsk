/*
 * Library for generating public/private/shared keys
 * for use with Diffie-Hellman Key Exchange.
 *
 * The original work was done by Dan Bernstein here: http://cr.yp.to/ecdh.html
 *
 * This implementation is simple, and is based off RFC 7748.
 */

/* This code is relatively low-level,
 * generic key-share implementations should
 * probably use DskKeyShareMethod to handle
 * a range of options instead.
 */
/* inputs and outputs all have length 56 (bytes) */


DSK_INLINE void dsk_curve448_random_to_private (uint8_t *random_to_private);

void dsk_curve448_private_to_public (const uint8_t *private_key,
                                       uint8_t       *public_key_out);
void dsk_curve448_private_to_shared (const uint8_t *private_key,
                                       const uint8_t *other_public_key,
                                       uint8_t       *shared_key_out);

DSK_INLINE void
dsk_curve448_random_to_private (uint8_t *random_to_private)
{
  random_to_private[0] &= 252;
  random_to_private[55] |= 128;
}
