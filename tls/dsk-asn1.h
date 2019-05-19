// For a quick-start overview, which includes a fairly full example
// with x509 certs:
//
//     http://www.zytrax.com/tech/survival/asn1.html#examples-x509-subset
//
//
// The core spec is fairly readable:
//
//     https://www.itu.int/ITU-T/studygroups/com17/languages/X.690-0207.pdf
//
//
// The specification language itself is defined here:
// this complexity is where things get tricky, and
// we don't attempt to implement anythinng in here:
//
//     https://www.itu.int/ITU-T/studygroups/com17/languages/X.680-0207.pdf
//
//
// A fairly complete book is freely available:
// It contains a history of ASN.1 development
// that would probably explain the quirks.
//
//     http://www.oss.com/asn1/resources/books-whitepapers-pubs/dubuisson-asn1-book.PDF



#define DSK_ASN1_TYPE__TAG_CLASS__UNIVERSAL            0x00
#define DSK_ASN1_TYPE__TAG_CLASS__APPLICATION          0x40
#define DSK_ASN1_TYPE__TAG_CLASS__PRIVATE              0x80
#define DSK_ASN1_TYPE__TAG_CLASS__CONTEXT_SPECIFIC     0xc0
typedef uint8_t DskASN1TagClass;

typedef enum
{
  DSK_ASN1_TYPE_BOOLEAN            =  1,
  DSK_ASN1_TYPE_INTEGER            =  2,
  DSK_ASN1_TYPE_BIT_STRING         =  3,
  DSK_ASN1_TYPE_OCTET_STRING       =  4,
  DSK_ASN1_TYPE_NULL               =  5,
  DSK_ASN1_TYPE_OBJECT_IDENTIFIER  =  6,
  DSK_ASN1_TYPE_OBJECT_DESCRIPTOR  =  7,
  DSK_ASN1_TYPE_EXTERNAL           =  8,
  DSK_ASN1_TYPE_REAL               =  9,
  DSK_ASN1_TYPE_ENUMERATED         = 10,
  DSK_ASN1_TYPE_ENUMERATED_PDV     = 11,
  DSK_ASN1_TYPE_UTF8_STRING        = 12,
  DSK_ASN1_TYPE_RELATIVE_OID       = 13,
  DSK_ASN1_TYPE_SEQUENCE           = 16,
  DSK_ASN1_TYPE_SET                = 17,
  DSK_ASN1_TYPE_NUMERIC_STRING     = 18,
  DSK_ASN1_TYPE_PRINTABLE_STRING   = 19,
  DSK_ASN1_TYPE_TELETEX_STRING     = 20,
  DSK_ASN1_TYPE_VIDEOTEXT_STRING   = 21,
  DSK_ASN1_TYPE_ASCII_STRING       = 22,
  DSK_ASN1_TYPE_UTC_TIME           = 23,
  DSK_ASN1_TYPE_GENERALIZED_TIME   = 24,
  DSK_ASN1_TYPE_GRAPHIC_STRING     = 25,
  DSK_ASN1_TYPE_VISIBLE_STRING     = 26,
  DSK_ASN1_TYPE_GENERAL_STRING     = 27,
  DSK_ASN1_TYPE_UNIVERSAL_STRING   = 28,
  DSK_ASN1_TYPE_CHARACTER_STRING   = 29,
  DSK_ASN1_TYPE_BMP_STRING         = 30
} DskASN1Type;                                   

typedef enum
{
  DSK_ASN1_REAL_ZERO,
  DSK_ASN1_REAL_INFINITY,
  DSK_ASN1_REAL_BINARY,
  DSK_ASN1_REAL_DECIMAL,
} DskASN1RealType;

typedef struct DskASN1Value DskASN1Value;
struct DskASN1Value
{
  DskASN1Type type;
  DskASN1TagClass tag_class;
  bool is_constructed;
  const uint8_t *value_start, *value_end;
  union {
    bool v_boolean;
    int64_t v_integer;
    int64_t v_enumerated;

    struct {
      size_t length;
      const uint8_t *bits;
    } v_bit_string;

    struct {
      size_t n_subidentifiers;
      unsigned *subidentifiers;
    } v_object_identifier, v_relative_oid;

    struct {
      DskASN1RealType real_type;

      // may be approximate.
      double double_value;
      bool overflowed;

      // For REAL_TYPE_BINARY.
      uint8_t log2_base;
      uint8_t binary_scaling_factor;
      const uint8_t *exp_start;         // exp continues to num_start
      const uint8_t *num_start;         // num continues to value_end
      bool is_signed;

      // For REAL_TYPE_DECIMAL
      const char *as_string;
    } v_real;

    // All these remaining types are the same;
    // use ascii_string if your intent is to
    // set generic response.
    struct {
      size_t encoded_length;                    // number of bytes in 'bytes'
      const uint8_t *encoded_data;
    } v_str,
      v_numeric_string, v_printable_string, v_teletex_string, v_ascii_string,
      v_graphic_string, v_visible_string, v_general_string,
      v_universal_string, v_character_string, v_bmp_string,
      v_object_descriptor;

    // At their core, times are represented as strings.
    struct {
      uint64_t unixtime;
      unsigned microseconds;
    } v_time;

    struct {
      unsigned n_children;
      DskASN1Value **children;
    } v_sequence, v_set;
  };
};

DskASN1Value * dsk_asn1_value_parse_der (size_t length,
                                         const uint8_t *data,
                                         size_t *used_out,
                                         DskMemPool *pool,
                                         DskError **error);

