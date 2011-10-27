/* functions mostly intended to imitate strace() in terms of log output format */

dsk_boolean dsk_c_quote_to_buffer (unsigned length,
                                   const uint8_t *data,
                                   dsk_boolean write_quotes,
                                   DskBuffer *output);
