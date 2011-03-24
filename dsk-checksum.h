typedef struct _DskChecksum DskChecksum;

/* --- public interface --- */
typedef enum
{
  DSK_CHECKSUM_MD5,
  DSK_CHECKSUM_SHA1,
  DSK_CHECKSUM_SHA256,
  DSK_CHECKSUM_CRC32_BIG_ENDIAN,
  DSK_CHECKSUM_CRC32_LITTLE_ENDIAN,
} DskChecksumType;

DskChecksum   *dsk_checksum_new        (DskChecksumType type);
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

/* misc */
size_t         dsk_checksum_type_get_size(DskChecksumType  checksum);

/* for efficiency: non-allocating versions */
size_t         dsk_checksum_type_get_instance_size (DskChecksumType type);
void           dsk_checksum_init       (void           *checksum_instance,
                                        DskChecksumType type);


