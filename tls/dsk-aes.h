/* A "pure" implementation of FIPS 197, Advanced Encryption Standard.
 *
 * This algorithm is usually known as simply AES,
 * or sometimes as a portmanteau of its inventors names, Rijndael.
 *
 * It is a block cipher that encrypts/decrypts blocks of length 128 bits.
 *
 * The original paper allowed for various key and block widths,
 * but FIPS only standardized three, all with a block-width of 128 bits.
 * The key-lengths are 128, 192 and 256, giving us AES128 AES192 AES256.
 */

/* References:
 *  - FIPS 197:   https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197.pdf
 *  - _The Design of Rijndael: AES - the Advanced Encryption Standard_,
 *    Rijman and Daemen, Springer, 2002.
 */

/* All FIPS/NIST approved versions of AES
 * have a blocksize of 128-bits which is 16 bytes.
 */
#define DSK_AES_BLOCKSIZE_IN_BYTES             16

typedef struct { uint8_t w[16 * 11]; uint32_t align[4 * 11]; } DskAES128;


void
dsk_aes128_init           (DskAES128            *s,
                           const uint8_t        *key);     /* length 16 */
void
dsk_aes128_encrypt_inplace(const DskAES128      *aes_key,
                           uint8_t              *in_out);  /* length 16 */
void
dsk_aes128_decrypt_inplace(const DskAES128      *aes_key,
                           uint8_t              *in_out);  /* length 16 */


typedef struct { uint8_t w[16 * 13]; uint32_t align[4 * 13]; } DskAES192;


void
dsk_aes192_init           (DskAES192            *s,
                           const uint8_t        *key);     /* length 16 (TODO: check) */
void
dsk_aes192_encrypt_inplace(const DskAES192      *aes_key,
                           uint8_t              *in_out);  /* length 16 */
void
dsk_aes192_decrypt_inplace(const DskAES192      *aes_key,
                           uint8_t              *in_out);  /* length 24 */


typedef struct { uint8_t w[16 * 15]; uint32_t align[4 * 15]; } DskAES256;


void
dsk_aes256_init           (DskAES256            *s,
                           const uint8_t        *key);     /* length 32 (TODO: check) */
void
dsk_aes256_encrypt_inplace(const DskAES256      *aes_key,
                           uint8_t              *in_out);  /* length 16 */
void
dsk_aes256_decrypt_inplace(const DskAES256      *aes_key,
                           uint8_t              *in_out);  /* length 16 */

