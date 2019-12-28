
typedef struct DskChecksumType DskChecksumType;
typedef struct DskChecksum DskChecksum;

// Direct use of DskChecksumType is recommended
// for performance-critical code.
//
// To use it, allocate an aligned block B of size DskChecksumType.instance_size.
// Call checksum_type->init(B).
// Feed it data incrementally using checksum_type->feed(B, length, data).
//
// Checksum "instances" (the thing initialized by init)
// must have several properties:
//   * it must be of size instance_size and aligned per instance_alignment
//   * they are movable (ie you can replace "init" with
//     a memcpy of a valid instance (of the same type, of course)).
//   * they have no destructor - they must not do memory allocation, etc
//
struct DskChecksumType
{
  const char *name;
  size_t instance_size;
  size_t hash_size;
  size_t block_size_in_bits;
  const uint8_t *empty_hash;
  void (*init)(void          *instance);
  void (*feed)(void          *instance,
               size_t         len,
               const uint8_t *data);
  void (*end) (void          *instance,
               uint8_t       *hash_out);
};

extern DskChecksumType dsk_checksum_type_md5;
extern DskChecksumType dsk_checksum_type_sha1;

extern DskChecksumType dsk_checksum_type_sha224;
extern DskChecksumType dsk_checksum_type_sha256;

extern DskChecksumType dsk_checksum_type_sha512_224;
extern DskChecksumType dsk_checksum_type_sha512_256;
extern DskChecksumType dsk_checksum_type_sha384;
extern DskChecksumType dsk_checksum_type_sha512;

// TODO: BLAKE hash.

extern DskChecksumType dsk_checksum_type_crc32;      // always big-endian(!)

/* --- easy-to-interface which is performant enough for most users --- */
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

// Return the result of calling done+get without altering the csum state.
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
unsigned       dsk_checksum_type_get_block_size (DskChecksumType *type);


DskChecksumType *dsk_checksum_type_by_name (const char *str);
