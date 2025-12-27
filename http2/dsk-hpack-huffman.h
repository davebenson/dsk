




bool dsk_hpack_huffman_decode_slow     (size_t bytes_length,
                                        const uint8_t *bytes,
                                        size_t *out_length_out,
                                        uint8_t *out,
                                        DskError **error);

bool dsk_hpack_huffman_decode          (size_t bytes_length,
                                        const uint8_t *bytes,
                                        size_t *out_length_out,
                                        uint8_t *out,
                                        DskError **error);
