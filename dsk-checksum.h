
typedef struct DskChecksum DskChecksum;
typedef struct DskChecksumType DskChecksumType;
struct DskChecksumType
{
  const char *name;
  size_t instance_size;
  size_t hash_size;
  size_t block_size_in_bits;
  const uint8_t *empty_hash;
  void (*init)(void *instance);
  void (*feed)(void *instance, size_t len, const uint8_t *data);
  void (*end)(void *instance, uint8_t *hash_out);
};

extern DskChecksumType dsk_checksum_type_md5;
extern DskChecksumType dsk_checksum_type_sha1;

extern DskChecksumType dsk_checksum_type_sha224;
extern DskChecksumType dsk_checksum_type_sha256;

extern DskChecksumType dsk_checksum_type_sha512_224;
extern DskChecksumType dsk_checksum_type_sha512_256;
extern DskChecksumType dsk_checksum_type_sha384;
extern DskChecksumType dsk_checksum_type_sha512;

extern DskChecksumType dsk_checksum_type_crc32;      // always big-endian(!)

/* --- public interface --- */
DskChecksum   *dsk_checksum_new        (DskChecksumType *type);
void           dsk_checksum_destroy    (DskChecksum    *checksum);

void           dsk_checksum_feed       (DskChecksum    *hash,
                                        size_t          length,
                                        const uint8_t  *data);
void           dsk_checksum_feed_str   (DskChecksum    *checksum,
                                        const char     *str);
void           dsk_checksum_done       (DskChecksum    *checksum);
size_t         dsk_checksum_get_size   (DskChecksum    *checksum);
void           dsk_checksum_get        (DskChecksum    *checksum,
                                        uint8_t        *data_out);
void           dsk_checksum_get_hex    (DskChecksum    *checksum,
                                        char           *hex_out);

void           dsk_checksum_get_current (DskChecksum *csum,
                                        uint8_t        *data_out);

/* misc */
size_t         dsk_checksum_type_get_size(DskChecksumType  checksum);

void           dsk_checksum            (DskChecksumType*type,
                                        size_t          len,
                                        const void     *data,
                                        uint8_t        *checksum_out);

/* Size of blocks (in bytes) in which this algorithm operates.
 *
 * This is used for computing HMACs (see RFC 2104, RFC 4868).
 */
unsigned       dsk_checksum_type_get_block_size (DskChecksumType type);
