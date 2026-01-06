
//
// The shortest encoding of any characters is 5 bits.
//
#define DSK_HPACK_HUFFMAN_MAX_DECODED_SIZE_FOR_ENCODED_SIZE(size) \
  ((size) * 8 / 5)

//
// On the other hand, the longest encoded byte will weigh in at 30 bits.
// Plus we need to round up to the nearest full byte.
//
#define DSK_HPACK_HUFFMAN_MAX_ENCODED_SIZE_FOR_DECODED_SIZE(size) \
  (((size) * 30 + 7) / 8)


//
// If the above macro seems to big for you (it requires 4x the input size),
// and if you are in control of the keys and values, you could consider
// this bound on the encoded size.
//
// The longest code, if you stay in ASCII
// and avoid control characters (chars < 32 and char == 127 (DEL)),
// is 19 bits.
//
// We add 1 to the length to make certain optimization possible.
//
#define DSK_HPACK_HUFFMAN_MAX_ENCODED_SIZE_FOR_DECODED_ASCII_SIZE(size) \
  (((size) * 19 + 7) / 8 + 1)
  



bool dsk_hpack_huffman_decode_bitwise  (size_t bytes_length,
                                        const uint8_t *bytes,
                                        size_t *out_length_out,
                                        uint8_t *out,
                                        DskError **error);

bool dsk_hpack_huffman_decode          (size_t bytes_length,
                                        const uint8_t *bytes,
                                        size_t *out_length_out,
                                        uint8_t *out,
                                        DskError **error);

void dsk_hpack_huffman_encode          (size_t data_length,
                                        const uint8_t *data,
                                        size_t *encoded_length_out,
                                        uint8_t *out);
