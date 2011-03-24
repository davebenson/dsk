
/* --- Decoding --- */
typedef struct _DskMimeMultipartDecoder DskMimeMultipartDecoder;
  
/* 
   For "multipart/form-data" (RFC XXXX) 
 */
DskMimeMultipartDecoder *dsk_mime_multipart_decoder_new (char **kv_pairs,
                                                         DskError **error);
dsk_boolean dsk_mime_multipart_decoder_feed (DskMimeMultipartDecoder *decoder,
                                             size_t                   length,
					     const uint8_t           *data,
					     size_t                  *n_parts_ready_out,
					     DskError               **error);
dsk_boolean dsk_mime_multipart_decoder_done (DskMimeMultipartDecoder *decoder,
					     size_t                  *n_parts_ready_out,
					     DskError               **error);

void dsk_mime_multipart_decoder_dequeue_all (DskMimeMultipartDecoder *decoder,
                                             DskCgiVariable       *out);

/* --- Encoding --- */
void dsk_mime_multipart_encode_1            (const DskCgiVariable *variable,
                                             DskBuffer            *output);
