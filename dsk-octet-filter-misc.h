

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
DskOctetFilter *dsk_json_prettier_new              (void);
#define dsk_html_escaper_new() dsk_xml_escaper_new()
#define dsk_html_unescaper_new() dsk_xml_unescaper_new()

typedef enum
{
  DSK_UTF16_WRITER_BIG_ENDIAN = (1<<0),
  DSK_UTF16_WRITER_LITTLE_ENDIAN = (1<<1),
  DSK_UTF16_WRITER_EMIT_MARKER = (1<<2)
} DskUtf16WriterFlags;

DskOctetFilter *dsk_utf8_to_utf16_converter_new    (DskUtf16WriterFlags flags);
typedef enum
{
  DSK_UTF16_PARSER_LITTLE_ENDIAN,
  DSK_UTF16_PARSER_BIG_ENDIAN,
  DSK_UTF16_PARSER_REQUIRE_MARKER
} DskUtf16ParserMode;
DskOctetFilter *dsk_utf16_to_utf8_converter_new    (DskUtf16ParserMode mode);
DskOctetFilter *dsk_byte_doubler_new               (uint8_t c);
DskOctetFilter *dsk_byte_undoubler_new             (uint8_t c,
                                                    dsk_boolean ignore_errors);

typedef enum
{
  DSK_UTF8_FIXER_DROP,
  DSK_UTF8_FIXER_LATIN1
} DskUtf8FixerMode;
DskOctetFilter *dsk_utf8_fixer_new                 (DskUtf8FixerMode mode);

typedef struct _DskCodepage DskCodepage;
struct _DskCodepage
{
  /* gives mappings for characters above 127. */
  uint16_t offsets[129];
  const uint8_t *heap;
};
extern const DskCodepage dsk_codepage_latin1;

DskOctetFilter *dsk_codepage_to_utf8_new           (const DskCodepage *codepage);


DskOctetFilter *dsk_whitespace_trimmer_new         (void);


/* The "_take" suffix implies the reference-count is passed on all the filters,
 * since it's needed, and their caller can't use them anyway.
 * However, the slab of memory at 'filters' is not taken over (the pointers
 * are copied)
 */
DskOctetFilter *dsk_octet_filter_chain_new_take    (unsigned         n_filters,
                                                    DskOctetFilter **filters);
DskOctetFilter *dsk_octet_filter_chain_new_take_list(DskOctetFilter *first,
                                                     ...);

