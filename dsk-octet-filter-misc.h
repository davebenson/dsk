

DskOctetFilter *dsk_octet_filter_identity_new      (void);
DskOctetFilter *dsk_base64_encoder_new             (dsk_boolean break_lines);
DskOctetFilter *dsk_base64_decoder_new             (void);
DskOctetFilter *dsk_hex_encoder_new                (dsk_boolean break_lines,
                                                    dsk_boolean include_spaces);
DskOctetFilter *dsk_hex_decoder_new                (void);
DskOctetFilter *dsk_url_encoder_new                (void);
DskOctetFilter *dsk_url_decoder_new                (void);
DskOctetFilter *dsk_c_quoter_new                   (dsk_boolean add_quotes,
                                                    dsk_boolean protect_trigraphs);
DskOctetFilter *dsk_c_unquoter_new                 (dsk_boolean remove_quotes);
DskOctetFilter *dsk_quote_printable_new            (void);
DskOctetFilter *dsk_unquote_printable_new          (void);
DskOctetFilter *dsk_xml_escaper_new                (void);
DskOctetFilter *dsk_xml_unescaper_new              (void);
DskOctetFilter *dsk_utf8_to_utf16_converter_new    (dsk_boolean big_endian);
DskOctetFilter *dsk_utf16_to_utf8_converter_new    (dsk_boolean initially_big_endian,
                                                    dsk_boolean require_endian_marker);
DskOctetFilter *dsk_byte_doubler_new               (uint8_t c);
DskOctetFilter *dsk_byte_undoubler_new             (uint8_t c,
                                                    dsk_boolean ignore_errors);


DskOctetFilter *dsk_whitespace_trimmer_new         (void);
/* The "_take" suffix implies the reference-count is passed on all the filters,
 * since it's needed, and all there caller can't use them anyway.
 * However, the slab of memory at 'filters' is not taken over (the pointers
 * are copied)
 */
DskOctetFilter *dsk_octet_filter_chain_new_take    (unsigned         n_filters,
                                                    DskOctetFilter **filters);
DskOctetFilter *dsk_octet_filter_chain_new_take_list(DskOctetFilter *first,
                                                     ...);

