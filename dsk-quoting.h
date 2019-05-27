/* functions mostly intended to imitate strace() in terms of log output format */

bool dsk_c_quote_to_buffer (unsigned length,
                                   const uint8_t *data,
                                   bool write_quotes,
                                   DskBuffer *output);
