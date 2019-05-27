

DskSyncFilter *dsk_sync_filter_identity_new      (void);
DskSyncFilter *dsk_base64_encoder_new             (bool break_lines);
DskSyncFilter *dsk_base64_decoder_new             (void);
DskSyncFilter *dsk_hex_encoder_new                (bool break_lines,
                                                    bool include_spaces);
DskSyncFilter *dsk_hex_decoder_new                (void);
DskSyncFilter *dsk_url_encoder_new                (void);
DskSyncFilter *dsk_url_decoder_new                (void);
DskSyncFilter *dsk_c_quoter_new                   (bool add_quotes,
                                                    bool protect_trigraphs);
DskSyncFilter *dsk_c_unquoter_new                 (bool remove_quotes);
DskSyncFilter *dsk_quote_printable_new            (void);
DskSyncFilter *dsk_unquote_printable_new          (void);
DskSyncFilter *dsk_xml_escaper_new                (void);
DskSyncFilter *dsk_xml_unescaper_new              (void);
DskSyncFilter *dsk_json_prettier_new              (void);
#define dsk_html_escaper_new() dsk_xml_escaper_new()
#define dsk_html_unescaper_new() dsk_xml_unescaper_new()

typedef enum
{
  DSK_UTF16_WRITER_BIG_ENDIAN = (1<<0),
  DSK_UTF16_WRITER_LITTLE_ENDIAN = (1<<1),
  DSK_UTF16_WRITER_EMIT_MARKER = (1<<2)
} DskUtf16WriterFlags;

DskSyncFilter *dsk_utf8_to_utf16_converter_new    (DskUtf16WriterFlags flags);
typedef enum
{
  DSK_UTF16_PARSER_LITTLE_ENDIAN,
  DSK_UTF16_PARSER_BIG_ENDIAN,
  DSK_UTF16_PARSER_REQUIRE_MARKER
} DskUtf16ParserMode;
DskSyncFilter *dsk_utf16_to_utf8_converter_new    (DskUtf16ParserMode mode);
DskSyncFilter *dsk_byte_doubler_new               (uint8_t c);
DskSyncFilter *dsk_byte_undoubler_new             (uint8_t c,
                                                    bool ignore_errors);

typedef enum
{
  DSK_UTF8_FIXER_DROP,
  DSK_UTF8_FIXER_LATIN1
} DskUtf8FixerMode;
DskSyncFilter *dsk_utf8_fixer_new                 (DskUtf8FixerMode mode);

typedef struct _DskCodepage DskCodepage;
struct _DskCodepage
{
  /* gives mappings for characters above 127. */
  uint16_t offsets[129];
  const uint8_t *heap;
};
extern const DskCodepage dsk_codepage_latin1;

DskSyncFilter *dsk_codepage_to_utf8_new           (const DskCodepage *codepage);


DskSyncFilter *dsk_whitespace_trimmer_new         (void);


/* The "_take" suffix implies the reference-count is passed on all the filters,
 * since it's needed, and their caller can't use them anyway.
 * However, the slab of memory at 'filters' is not taken over (the pointers
 * are copied)
 */
DskSyncFilter *dsk_sync_filter_chain_new_take    (unsigned         n_filters,
                                                    DskSyncFilter **filters);
DskSyncFilter *dsk_sync_filter_chain_new_take_list(DskSyncFilter *first,
                                                     ...);

