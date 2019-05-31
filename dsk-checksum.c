#include "dsk.h"
#include <string.h>
#include <stdlib.h>
#include <alloca.h>

/* --- for implementing new types of hash functions --- */
struct DskChecksum
{
  DskChecksumType *type;

  // hash follows
  // ctx follows
};

static inline uint8_t *
get_hash (DskChecksum *c)
{
  return (uint8_t *)(c + 1);
}

static inline void *
get_instance (DskChecksum *c)
{
  return get_hash(c) + c->type->hash_size;
}

/**
 * dsk_checksum_feed:
 * @checksum: the checksum to feed data.
 * @data: binary data to accumulate in the checksum.
 * @length: length of the binary data.
 *
 * Affect the checksum incrementally;
 * checksum the given binary data.
 * 
 * You may call this function on little bits of data
 * and it must have exactly the same effect
 * is if you called it once with a larger
 * slab of data.
 */
void
dsk_checksum_feed       (DskChecksum        *checksum,
                         size_t              length,
                         const uint8_t      *data)
{
  void *instance = get_instance(checksum);
  checksum->type->feed (instance, length, data);
}

/**
 * dsk_checksum_feed_str:
 * @checksum: the checksum to feed data.
 * @str: a NUL-terminated string to feed to the checksum.
 *
 * Checksum the given binary data (incrementally).
 *
 * You may mix calls to dsk_checksum_feed() and dsk_checksum_feed_str().
 */
void
dsk_checksum_feed_str   (DskChecksum        *checksum,
		     const char     *str)
{
  void *instance = get_instance(checksum);
  checksum->type->feed (instance, strlen (str), (const uint8_t *) str);
}

/**
 * dsk_checksum_done:
 * @checksum: the checksum to finish.
 *
 * Finish processing loose data for the checksum.
 * This may only be called once in
 * the lifetime of the checksum.
 */
void
dsk_checksum_done       (DskChecksum        *checksum)
{
  void *instance = get_instance(checksum);
  checksum->type->end (instance, get_hash(checksum));
}

/**
 * dsk_checksum_get_size:
 * @checksum: the checksum to query.
 *
 * Get the number of binary bytes that this
 * function maps to.
 * 
 * returns: the number of bytes of binary data in this checksum.
 */
size_t
dsk_checksum_get_size   (DskChecksum        *checksum)
{
  return checksum->type->hash_size;
}

/**
 * dsk_checksum_get:
 * @checksum: the checksum to query.
 * @data_out: binary buffer to fill with the checksum value.
 *
 * Get a binary checksum value.  This should be of the
 * size returned by dsk_checksum_get_size().
 */
void
dsk_checksum_get       (DskChecksum        *checksum,
		        uint8_t            *data_out)
{
  memcpy (data_out, get_hash(checksum), checksum->type->hash_size);
}

/**
 * dsk_checksum_get_hex:
 * @checksum: the checksum to query.
 * @hex_out: buffer to fill with a NUL-terminated hex checksum value.
 *
 * Get a hex checksum value.  This should be of the
 * size returned by (dsk_checksum_get_size() * 2 + 1).
 */
void
dsk_checksum_get_hex   (DskChecksum        *checksum,
		        char               *hex_out)
{
  static const char hex_digits[] = "0123456789abcdef";
  unsigned i;
  const uint8_t *checksum_value = get_hash(checksum);
  unsigned size = checksum->type->hash_size;
  for (i = 0; i < size; i++)
    {
      uint8_t h = checksum_value[i];
      *hex_out++ = hex_digits[h >> 4];
      *hex_out++ = hex_digits[h & 15];
    }
  *hex_out = 0;
}
void           dsk_checksum_get_current (DskChecksum *csum,
                                        uint8_t        *data_out)
{
  void *tmp_instance = alloca (csum->type->instance_size);
  memcpy (tmp_instance, get_instance(csum), csum->type->instance_size);
  csum->type->end(tmp_instance, data_out);
}

/**
 * dsk_checksum_destroy:
 * @checksum: the checksum function.
 *
 * Free memory used by the checksum object.
 */
void
dsk_checksum_destroy    (DskChecksum        *checksum)
{
  dsk_free (checksum);
}

void           dsk_checksum            (DskChecksumType *type,
                                        size_t          len,
                                        const void     *data,
                                        uint8_t        *checksum_out)
{
  void *sum = alloca (type->instance_size);
  type->init(sum);
  type->feed(sum, len, data);
  type->end(sum, checksum_out);
}


DskChecksum *
dsk_checksum_new (DskChecksumType *type)
{
  DskChecksum *rv = dsk_malloc (sizeof (DskChecksum) + type->instance_size + type->hash_size);
  rv->type = type;
  type->init (get_instance (rv));
  return rv;
}
