#include "../dsk.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

const char * dsk_asn1_type_name (DskASN1Type type)
{
  switch (type)
    {
#define WRITE_CASE(shortname) case DSK_ASN1_TYPE_##shortname: return #shortname
    WRITE_CASE(BOOLEAN);
    WRITE_CASE(INTEGER);
    WRITE_CASE(BIT_STRING);
    WRITE_CASE(OCTET_STRING);
    WRITE_CASE(NULL);
    WRITE_CASE(OBJECT_IDENTIFIER);
    WRITE_CASE(OBJECT_DESCRIPTOR);
    WRITE_CASE(EXTERNAL);
    WRITE_CASE(REAL);
    WRITE_CASE(ENUMERATED);
    WRITE_CASE(ENUMERATED_PDV);
    WRITE_CASE(UTF8_STRING);
    WRITE_CASE(RELATIVE_OID);
    WRITE_CASE(SEQUENCE);
    WRITE_CASE(SET);
    WRITE_CASE(NUMERIC_STRING);
    WRITE_CASE(PRINTABLE_STRING);
    WRITE_CASE(TELETEX_STRING);
    WRITE_CASE(VIDEOTEXT_STRING);
    WRITE_CASE(ASCII_STRING);
    WRITE_CASE(UTC_TIME);
    WRITE_CASE(GENERALIZED_TIME);
    WRITE_CASE(GRAPHIC_STRING);
    WRITE_CASE(VISIBLE_STRING);
    WRITE_CASE(GENERAL_STRING);
    WRITE_CASE(UNIVERSAL_STRING);
    WRITE_CASE(CHARACTER_STRING);
    WRITE_CASE(BMP_STRING);
#undef WRITE_CASE
    default: return "*unknown-asn1-type*";
    }
}

const char * dsk_asn1_type_name_lowercase (DskASN1Type type)
{
  switch (type)
    {
#define WRITE_CASE(shortname, str) case DSK_ASN1_TYPE_##shortname: return str
    WRITE_CASE(BOOLEAN, "boolean");
    WRITE_CASE(INTEGER, "integer");
    WRITE_CASE(BIT_STRING, "bit-string");
    WRITE_CASE(OCTET_STRING, "octet-string");
    WRITE_CASE(NULL, "null");
    WRITE_CASE(OBJECT_IDENTIFIER, "object-identifier");
    WRITE_CASE(OBJECT_DESCRIPTOR, "object-descriptor");
    WRITE_CASE(EXTERNAL, "external");
    WRITE_CASE(REAL, "real");
    WRITE_CASE(ENUMERATED, "enumerated");
    WRITE_CASE(ENUMERATED_PDV, "enumerated-pdv");
    WRITE_CASE(UTF8_STRING, "utf8-string");
    WRITE_CASE(RELATIVE_OID, "relative-oid");
    WRITE_CASE(SEQUENCE, "sequence");
    WRITE_CASE(SET, "set");
    WRITE_CASE(NUMERIC_STRING, "numeric-string");
    WRITE_CASE(PRINTABLE_STRING, "printable-string");
    WRITE_CASE(TELETEX_STRING, "teletex-string");
    WRITE_CASE(VIDEOTEXT_STRING, "videotext-string");
    WRITE_CASE(ASCII_STRING, "ascii-string");
    WRITE_CASE(UTC_TIME, "utc-time");
    WRITE_CASE(GENERALIZED_TIME, "generalized-time");
    WRITE_CASE(GRAPHIC_STRING, "graphic-string");
    WRITE_CASE(VISIBLE_STRING, "visible-string");
    WRITE_CASE(GENERAL_STRING, "general-string");
    WRITE_CASE(UNIVERSAL_STRING, "universal-string");
    WRITE_CASE(CHARACTER_STRING, "character-string");
    WRITE_CASE(BMP_STRING, "bmp-string");
#undef WRITE_CASE
    default: return "*unknown-asn1-type*";
    }
}

const char *dsk_asn1_tag_class_name (uint8_t v)
{
  switch (v)
    {
    case DSK_ASN1_TAG_CLASS_UNIVERSAL: return "UNIVERSAL";
    case DSK_ASN1_TAG_CLASS_APPLICATION: return "APPLICATION";
    case DSK_ASN1_TAG_CLASS_PRIVATE: return "PRIVATE";
    case DSK_ASN1_TAG_CLASS_CONTEXT_SPECIFIC: return "CONTEXT_SPECIFIC";
    default: return "*unknown-tag-class*";
    }
}

//
// Compute v_real.double_value and v_real.overflowed based
// on sign, base, exponent, binary_scaling_factor, mantissa.
// 
static bool
compute_real_parts (DskASN1Value *value,
                    DskError    **error)
{
  unsigned n_leading_zeros = 0;
  const uint8_t *x;

  x = value->v_real.num_start;
  while (x < value->value_end && *x == 0)
    {
      x++;
      n_leading_zeros += 8;
    }
  if (x == value->value_end)
    {
      *error = dsk_error_new ("zero not allowed in this encoding");
      return false;
    }
  for (unsigned i = 0; i < 8; i++)
    {
      if (*x & (128 >> i))
        break;
      else
        n_leading_zeros++;
    }

  x = value->v_real.num_start + (n_leading_zeros+1) / 8;
  uint8_t mask = 128 >> ((n_leading_zeros+1) % 8);

  // pull next 52 bits for mantissa;
  // we do it bit-by-bit, even though we could
  // probably do it byte-by-byte.
  unsigned shift = 52;
  uint64_t mant = 0;
  while (shift != 0 && x < value->value_end)
    {
      shift--;
      if (*x & mask)
        mant |= 1ULL << shift;
      mask >>= 1;
      if (mask == 0)
        {
          mask = 0x80;
          x++;
        }
    }

  //
  // if exponent is more than 1<<16 return overflow
  //
  int32_t stated_exp;
  switch (value->v_real.num_start - value->v_real.exp_start)
    {
    case 1:
      stated_exp = (int8_t) value->v_real.exp_start[0];
      break;
    case 2:
      stated_exp = (int16_t) dsk_uint16be_parse (value->v_real.exp_start);
      break;
    case 3:
      {
        uint32_t tmp = dsk_uint24be_parse (value->v_real.exp_start);
        stated_exp = (int32_t) tmp >> 8;
      }
      break;
    default:
      value->v_real.overflowed = true;
      return true;
    }
  stated_exp *= value->v_real.log2_base;
  if (stated_exp < -(1<<16) || stated_exp > (1<<16))
    {
      value->v_real.overflowed = true;
      return true;
    }

  // compute fp exponent
  int exp_adjust = (value->value_end - value->v_real.num_start) * 8 - n_leading_zeros - 1;
  int exponent = stated_exp + exp_adjust;
  if (exponent < -1023 || exponent > 1024)
    {
      value->v_real.overflowed = true;
      return true;
    }

  // finally, put together the pieces into a double.
  union {
    uint64_t i;
    double d;
  } u;
  u.i = dsk_double_as_uint64 (value->v_real.is_signed,
                              exponent,
                              mant);
  value->v_real.double_value = u.d;
  return true;
}

static inline unsigned
parse_2digit (const uint8_t *x)
{
  unsigned a = x[0] - '0';
  unsigned b = x[1] - '0';
  return a * 10 + b;
}

static inline unsigned
parse_4digit (const uint8_t *x)
{
  unsigned a = x[0] - '0';
  unsigned b = x[1] - '0';
  unsigned c = x[2] - '0';
  unsigned d = x[3] - '0';
  return a * 1000 + b * 100 + c * 10 + d;
}

DskASN1Value *
dsk_asn1_value_parse_der (size_t         length,
                          const uint8_t *data,
                          size_t        *used_out,
                          DskMemPool    *pool,
                          DskError     **error)
{
  DskASN1Value *rv;

  if (length < 2)
    {
      *error = dsk_error_new ("value must be at least 2 bytes");
      return NULL;
    }
     
  rv = dsk_mem_pool_alloc (pool, sizeof(DskASN1Value));
  rv->tag_class = data[0] >> 6;
  rv->is_constructed = (data[0] >> 5) & 1;
  rv->type = data[0] & 0xdf;            // remove "constructed" bit

  bool length_is_definite = data[1] != 0x80;
  if (!rv->is_constructed && !length_is_definite)
    {
      *error = dsk_error_new ("definite length required with primitive value");
      return NULL;
    }
  if (length_is_definite)
    {
      if ((data[1] & 0x80) == 0)
        {
          rv->value_start = data + 2;
          rv->value_end = rv->value_start + data[1];
          if (rv->value_end > data + length)
            {
              *error = dsk_error_new ("data too short after 1-byte length");
              return false;
            }
        }
      else
        {
          size_t value_len_len = data[1] & 0x7f;
          const uint8_t *at = data + 2;
          if (2 + value_len_len > length)
            {
              *error = dsk_error_new ("too short in definite length");
              return false;
            }
          size_t value_len = 0;
          at = data + 2;
          for (unsigned i = 0; i < value_len_len; i++)
            {
              value_len <<= 8;
              value_len |= *at++;
            }
          rv->value_start = at;
          rv->value_end = rv->value_start + value_len;
          if (rv->value_end > data + length)
            {
              *error = dsk_error_new ("too short after multibyte length");
              return false;
            }
        }
      *used_out = rv->value_end - data;
    }
  else
    {
      rv->value_start = data + 2;
      rv->value_end = memchr (data + 2, 0, length - 2);
      if (rv->value_end == NULL)
        {
          *error = dsk_error_new ("no NUL in indefinite value encoding");
          return false;
        }
      *used_out = rv->value_end + 1 - data;
    }

  if (rv->tag_class != DSK_ASN1_TAG_CLASS_UNIVERSAL)
    {
      rv->v_tagged.subvalue = NULL;
      return rv;
    }

  switch (rv->type)
    {
    case DSK_ASN1_TYPE_BOOLEAN:
      if (rv->value_start + 1 != rv->value_end)
        {
          *error = dsk_error_new ("boolean must be exactly a byte long");
          return false;
        }
      rv->v_boolean = rv->value_start[0] ? true : false;
      break;

    case DSK_ASN1_TYPE_ENUMERATED:
    case DSK_ASN1_TYPE_INTEGER:
      {
        uint8_t u64be_bytes[8];
        if (rv->value_start == rv->value_end)
          {
            *error = dsk_error_new ("integer must be non-empty");
            return NULL;
          }
        if (rv->value_start + 8 <= rv->value_end)
          memcpy (u64be_bytes, rv->value_end - 8, 8);
        else
          {
            unsigned vlen = rv->value_end - rv->value_start;
            memcpy (u64be_bytes + 8 - vlen, rv->value_start, vlen);
            uint8_t fill = (rv->value_start[0] & 0x80) ? 0xff : 0x00;
            memset (u64be_bytes, fill, 8 - vlen);
          }
        rv->v_integer = dsk_int64be_parse (u64be_bytes);
      }
      break;

    case DSK_ASN1_TYPE_BIT_STRING:
      if (rv->value_start == rv->value_end)
        {
          *error = dsk_error_new ("too short in bitstring");
          return NULL;
        }
      uint8_t len_mod_8 = rv->value_start[0];
      if (len_mod_8 > 0 && rv->value_start + 1 == rv->value_end)
        {
          *error = dsk_error_new ("too short in bitstring");
          return NULL;
        }
      size_t full_bytes = rv->value_end - rv->value_start - 1 - (len_mod_8 > 0);
      rv->v_bit_string.length = full_bytes * 8 + len_mod_8;
      rv->v_bit_string.bits = rv->value_start + 1;
      break;

    case DSK_ASN1_TYPE_NULL:
      if (rv->value_start != rv->value_end)
        {
          *error = dsk_error_new ("NULL value must be empty");
          return NULL;
        }
      break;

    case DSK_ASN1_TYPE_OBJECT_IDENTIFIER:
      {
        unsigned n_subids = 0;
        for (const uint8_t *at = rv->value_start; at < rv->value_end; at++)
          if ((*at & 0x80) == 0)
            n_subids++;
        if (n_subids == 0)
          {
            *error = dsk_error_new ("object-id must have at least one subidentifier");
            return NULL;
          }
        n_subids++;             // first two IDs are actually combined.
        size_t oid_size = dsk_oid_size_by_n_subids (n_subids);
        DskOID *oid = dsk_mem_pool_alloc (pool, oid_size);
        rv->v_object_identifier = oid;
        oid->n_subids = n_subids;
        unsigned s_index = 0;
        unsigned under_construction = 0;
        for (const uint8_t *at = rv->value_start; at < rv->value_end; at++)
          {
            under_construction <<= 7;
            under_construction |= *at & 0x7f;
            if ((*at & 0x80) == 0)
              {
                if (s_index == 0)
                  {
                    unsigned first = under_construction / 40;
                    if (first > 2)
                      first = 2;
                    under_construction -= first * 40;
                    oid->subids[s_index++] = first;
                    oid->subids[s_index++] = under_construction;
                  }
                else
                  oid->subids[s_index++] = under_construction;
                under_construction = 0;
              }
          }
        break;
      }
    case DSK_ASN1_TYPE_RELATIVE_OID:
      {
        unsigned n_subids = 0;
        for (const uint8_t *at = rv->value_start; at < rv->value_end; at++)
          if ((*at & 0x80) == 0)
            n_subids++;
        size_t oid_size = dsk_oid_size_by_n_subids (n_subids);
        DskOID *oid = dsk_mem_pool_alloc (pool, oid_size);
        rv->v_object_identifier = oid;
        oid->n_subids = n_subids;
        unsigned s_index = 0;
        unsigned under_construction = 0;
        for (const uint8_t *at = rv->value_start; at < rv->value_end; at++)
          {
            under_construction <<= 7;
            under_construction |= *at & 0x7f;
            if ((*at & 0x80) == 0)
              {
                oid->subids[s_index++] = under_construction;
                under_construction = 0;
              }
          }
        break;
      }

    case DSK_ASN1_TYPE_EXTERNAL:
      *error = dsk_error_new ("external type not supported");
      return NULL;

    case DSK_ASN1_TYPE_REAL:
      rv->v_real.overflowed = false;
      if (rv->value_start == rv->value_end)
        {
          rv->v_real.is_signed = false;
          rv->v_real.double_value = 0.0;
          rv->v_real.real_type = DSK_ASN1_REAL_ZERO;
        }
      else if ((rv->value_start[0] & 0x80) == 0x80)
        {
          // binary encoding
          rv->v_real.is_signed = (rv->value_start[0] & 0x40) == 0x40;

          static const uint8_t log2_bases[4] = {1,3,4,0};
          rv->v_real.log2_base = log2_bases[(rv->value_start[0] & 0x30) >> 4];
          if (rv->v_real.log2_base == 0)
            {
              *error = dsk_error_new ("bad binary-formatted real");
              return NULL;
            }
          rv->v_real.binary_scaling_factor = (rv->value_start[0] >> 2) & 3;

          static const uint8_t min_length[4] = {3, 4, 5, 3 };
          if ((rv->value_end - rv->value_start) < min_length[rv->value_start[0] & 3])
            {
              *error = dsk_error_new ("too short for real encoding");
              return NULL;
            }
          switch (rv->value_start[0] & 3)
            {
              case 0:
                rv->v_real.exp_start = rv->value_start + 1;
                rv->v_real.num_start = rv->value_start + 2;
                break;
              case 1:
                rv->v_real.exp_start = rv->value_start + 1;
                rv->v_real.num_start = rv->value_start + 3;
                break;
              case 2:
                rv->v_real.exp_start = rv->value_start + 1;
                rv->v_real.num_start = rv->value_start + 4;
                break;
              case 3:
                {
                  uint8_t exp_size = rv->value_start[1];
                  if ((rv->value_end - rv->value_start) < (2 + exp_size + 1))
                    {
                      *error = dsk_error_new ("too short for real encoding");
                      return NULL;
                    }
                  rv->v_real.exp_start = rv->value_start + 2;
                  rv->v_real.num_start = rv->value_start + 2 + exp_size;
                  break;
                }
            }

          if (!compute_real_parts (rv, error))
            return NULL;
        }
      else if ((rv->value_start[0] & 0xc0) == 0x00)
        {
          switch (rv->value_start[0] & 0x3f)
            {
              case 1:
              case 2:
              case 3:
                // ISO 6093 NR1 form "3", "+1", "-1000" (only integers allowed)
                // ISO 6093 NR2 form "3.14", "3.0", "-.3"
                // ISO 6093 NR3 form "3.0E1", "123E+100"
                {
                  const uint8_t *at = rv->value_start + 1;
                  while (at < rv->value_end && *at == ' ')
                    at++;
                  if (at == rv->value_end)
                    {
                      *error = dsk_error_new ("too short in decimal representation");
                      return NULL;
                    }
                  char *txt = dsk_mem_pool_alloc_unaligned (pool, rv->value_end + 1 - at);
                  memcpy (txt, at, rv->value_end - at);
                  txt[rv->value_end - at] = 0;
                  rv->v_real.real_type = DSK_ASN1_REAL_DECIMAL;
                  char *end;
                  rv->v_real.double_value = strtod (txt, &end);
                  // TODO: check 'end'
                  rv->v_real.as_string = txt;
                  break;
                }

              default:
                *error = dsk_error_new ("unknown format for decimal real encoding");
                return NULL;
            }
        }
      else if ((rv->value_start[0] & 0xc0) == 0x40)
        {
          if (rv->value_start + 2 > rv->value_end)
            {
              *error = dsk_error_new ("too short for real special value encoding");
              return NULL;
            }
          switch (rv->value_start[1])
            {
            case 0x40:
              rv->v_real.double_value = INFINITY;
              rv->v_real.real_type = DSK_ASN1_REAL_INFINITY;
              rv->v_real.is_signed = false;
              break;
            case 0x41:
              rv->v_real.double_value = -INFINITY;
              rv->v_real.real_type = DSK_ASN1_REAL_INFINITY;
              rv->v_real.is_signed = true;
              break;
            default:
              *error = dsk_error_new ("bad real special value");
              return NULL;
            }
        }
      else
        assert(false);
      break;

    case DSK_ASN1_TYPE_SET:
    case DSK_ASN1_TYPE_SEQUENCE:
      {
        DskASN1Value *tmp[16];
        DskASN1Value **values = tmp;
        unsigned values_alloced = DSK_N_ELEMENTS (tmp);
        unsigned n_sub = 0;
        for (const uint8_t *at = rv->value_start; at < rv->value_end; )
          {
            if (n_sub == values_alloced)
              {
                if (values == tmp)
                  {
                    values = malloc (sizeof (DskASN1Value*) * values_alloced * 2);
                    memcpy (values, tmp, n_sub * sizeof(DskASN1Value*));
                  }
                else
                  values = realloc (values, sizeof (DskASN1Value*) * values_alloced * 2);
                values_alloced *= 2;
              }
            size_t used;
            values[n_sub] = dsk_asn1_value_parse_der (rv->value_end - at, at, &used, pool, error);
            if (values[n_sub] == NULL)
              {
                if (values != tmp)
                  free (values);
                return NULL;
              }
            n_sub++;
            at += used;
          }

        rv->v_sequence.n_children = n_sub;
        rv->v_sequence.children = dsk_mem_pool_alloc (pool, n_sub * sizeof(DskASN1Value*));
        memcpy (rv->v_sequence.children, values, n_sub * sizeof(DskASN1Value*));
        if (values != tmp)
          free (values);
        break;
      }

    case DSK_ASN1_TYPE_UTF8_STRING:
    case DSK_ASN1_TYPE_OCTET_STRING:
    case DSK_ASN1_TYPE_NUMERIC_STRING:
    case DSK_ASN1_TYPE_PRINTABLE_STRING:
    case DSK_ASN1_TYPE_TELETEX_STRING:
    case DSK_ASN1_TYPE_VIDEOTEXT_STRING:
    case DSK_ASN1_TYPE_ASCII_STRING:
    case DSK_ASN1_TYPE_GRAPHIC_STRING:
    case DSK_ASN1_TYPE_VISIBLE_STRING:
    case DSK_ASN1_TYPE_GENERAL_STRING:
    case DSK_ASN1_TYPE_UNIVERSAL_STRING:
    case DSK_ASN1_TYPE_CHARACTER_STRING:
    case DSK_ASN1_TYPE_BMP_STRING:
    case DSK_ASN1_TYPE_OBJECT_DESCRIPTOR:
      rv->v_str.encoded_length = rv->value_end - rv->value_start;
      rv->v_str.encoded_data = rv->value_start;
      break;
    case DSK_ASN1_TYPE_UTC_TIME:
      {
        // note 2-digit year: "YYMMDDHHMMSSZ"
        if (!(rv->value_end - rv->value_start == 13
                  && dsk_ascii_isdigit(rv->value_start[0])
                  && dsk_ascii_isdigit(rv->value_start[1])
                  && dsk_ascii_isdigit(rv->value_start[2])
                  && dsk_ascii_isdigit(rv->value_start[3])
                  && dsk_ascii_isdigit(rv->value_start[4])
                  && dsk_ascii_isdigit(rv->value_start[5])
                  && dsk_ascii_isdigit(rv->value_start[6])
                  && dsk_ascii_isdigit(rv->value_start[7])
                  && dsk_ascii_isdigit(rv->value_start[8])
                  && dsk_ascii_isdigit(rv->value_start[9])
                  && dsk_ascii_isdigit(rv->value_start[10])
                  && dsk_ascii_isdigit(rv->value_start[11])
                  && rv->value_start[12] == 'Z'))
          {
            *error = dsk_error_new ("UTC Time format invalid");
            return NULL;
          }
        DskDate date;
        date.year = parse_2digit(rv->value_start + 0) + 2000;
        date.month = parse_2digit(rv->value_start + 2);
        date.day = parse_2digit(rv->value_start + 4);
        date.hour = parse_2digit(rv->value_start + 6);
        date.minute = parse_2digit(rv->value_start + 8);
        date.second = parse_2digit(rv->value_start + 10);
        date.zone_offset = 0;
        if (!dsk_date_is_valid (&date))
          {
            *error = dsk_error_new ("UTC Time field invalid");
            return NULL;
          }
        rv->v_time.unixtime = dsk_date_to_unixtime (&date);
        rv->v_time.microseconds = 0;
        break;
      }
    case DSK_ASN1_TYPE_GENERALIZED_TIME:
      {
        // "YYYYMMDDHHMMSS[.ssssss]Z"
        if (!(rv->value_end - rv->value_start >= 15
                  && dsk_ascii_isdigit(rv->value_start[0])
                  && dsk_ascii_isdigit(rv->value_start[1])
                  && dsk_ascii_isdigit(rv->value_start[2])
                  && dsk_ascii_isdigit(rv->value_start[3])
                  && dsk_ascii_isdigit(rv->value_start[4])
                  && dsk_ascii_isdigit(rv->value_start[5])
                  && dsk_ascii_isdigit(rv->value_start[6])
                  && dsk_ascii_isdigit(rv->value_start[7])
                  && dsk_ascii_isdigit(rv->value_start[8])
                  && dsk_ascii_isdigit(rv->value_start[9])
                  && dsk_ascii_isdigit(rv->value_start[10])
                  && dsk_ascii_isdigit(rv->value_start[11])
                  && dsk_ascii_isdigit(rv->value_start[12])
                  && dsk_ascii_isdigit(rv->value_start[13])
                  && (rv->value_start[12] == 'Z' || rv->value_start[12] == '.')
                  && *(rv->value_end-1) == 'Z'))
          {
            *error = dsk_error_new ("Generalized Time format invalid");
            return NULL;
          }
        unsigned microsecs = 0;
        if (rv->value_end - rv->value_start > 15)
          {
            if (rv->value_start[12] != '.')
              {
                *error = dsk_error_new ("Generalized Time format: expected '.'");
                return NULL;
              }
            unsigned n_subsec_digits = 0;
            for (const uint8_t *at = rv->value_start + 13; at + 1 < rv->value_end; at++)
              if (!dsk_ascii_isdigit (*at))
                {
                  *error = dsk_error_new ("Generalized Time format: expected subseconds to be digits");
                  return NULL;
                }
              else
                n_subsec_digits++;
            unsigned ssd = 0;
            for (ssd = 0; ssd < DSK_MIN(6, n_subsec_digits); ssd++)
              {
                microsecs *= 10;
                microsecs += rv->value_start[13 + ssd] - '0';
              }
            for (; ssd < 6; ssd++)
              microsecs *= 10;
          }

        DskDate date;
        date.year = parse_4digit(rv->value_start + 0);
        date.month = parse_2digit(rv->value_start + 4);
        date.day = parse_2digit(rv->value_start + 6);
        date.hour = parse_2digit(rv->value_start + 8);
        date.minute = parse_2digit(rv->value_start + 10);
        date.second = parse_2digit(rv->value_start + 12);
        date.zone_offset = 0;
        if (!dsk_date_is_valid (&date))
          {
            *error = dsk_error_new ("UTC Time field invalid");
            return NULL;
          }
        rv->v_time.unixtime = dsk_date_to_unixtime (&date);
        rv->v_time.microseconds = microsecs;
        break;
      }
    default:
      *error = dsk_error_new ("unhandled value tag");
      return NULL;
    }
  return rv;
}

static void
dump_hex (DskBuffer     *buffer,
          unsigned       indent,
          size_t         length,
          const uint8_t *data)
{
  for (unsigned i = 0; i < length; i++)
    {
      if (i % 16 == 0)
        dsk_buffer_append_repeated_byte (buffer, indent, ' ');
      dsk_buffer_printf (buffer, "%02x%c", data[i], (i == length - 1 || i % 16 == 15) ? '\n' : ' ');
    }
  if (length == 0)
    dsk_buffer_printf (buffer, "%.*s[empty]\n", (int) length, "");
}

static void
dump_hex_oneline (DskBuffer *buffer, const uint8_t *start, const uint8_t *end)
{
  for (const uint8_t *at = start; at < end; at++)
    dsk_buffer_printf (buffer, "%02x", *at);
}

static bool
is_ascii_printable (size_t len, const uint8_t *data)
{
  for (size_t i = 0; i < len; i++)
    if (data[i] < 32 || data[i] > 126)
      return false;
  return true;
}

void
dump_to_buffer (const DskASN1Value *value,
                DskBuffer *buffer,
                unsigned indent)
{
  if (value->tag_class != DSK_ASN1_TAG_CLASS_UNIVERSAL)
    {
      dsk_buffer_printf (buffer, "%*sTag Class: %s    Tag: 0x%02x\n",
                         indent, "",
                         dsk_asn1_tag_class_name (value->tag_class),
                         value->type);
      dump_hex (buffer, indent + 4,
                value->value_end - value->value_start,
                value->value_start);
      return;
    }

  switch (value->type)
    {
    case DSK_ASN1_TYPE_BOOLEAN:
      dsk_buffer_printf (buffer, "%*sboolean: %s\n",
                         indent, "",
                         value->v_boolean ? "true" : "false");
      return;
    case DSK_ASN1_TYPE_INTEGER:
      dsk_buffer_printf (buffer, "%*sinteger: %llu\n",
                         indent, "",
                         value->v_integer);
      dump_hex (buffer, indent + 4,
                value->value_end - value->value_start,
                value->value_start);
      return;
    case DSK_ASN1_TYPE_BIT_STRING:
      dsk_buffer_printf (buffer, "%*sbit-string: length=%u\n",
                         indent, "",
                         (unsigned) value->v_bit_string.length);
      dump_hex (buffer, indent + 4,
                (value->v_bit_string.length + 7) / 8,
                value->v_bit_string.bits);
      return;
    case DSK_ASN1_TYPE_NULL:
      dsk_buffer_printf (buffer, "%*snull\n",
                         indent, "");
      break;
    case DSK_ASN1_TYPE_OBJECT_IDENTIFIER:
      {
        const DskOID *oid = value->v_object_identifier;
        dsk_buffer_printf (buffer, "%*sobject-identifier: ", indent, "");
        for (unsigned i = 0; i < oid->n_subids; i++)
          dsk_buffer_printf(buffer,
                            "%u%c",
                            oid->subids[i],
                            i + 1 < oid->n_subids ? '.' : '\n');
      }
      break;
    case DSK_ASN1_TYPE_RELATIVE_OID:
      {
        const DskOID *oid = value->v_relative_oid;
        dsk_buffer_printf (buffer, "%*srelative-object-identifier:", indent, "");
        for (unsigned i = 0; i < oid->n_subids; i++)
          dsk_buffer_printf(buffer, "%c%u", i == 0 ? ' ' : '.', oid->subids[i]);
        dsk_buffer_append_byte (buffer, '\n');
      }
      break;
    case DSK_ASN1_TYPE_OBJECT_DESCRIPTOR:
      break;
    case DSK_ASN1_TYPE_EXTERNAL:
      assert(false);
      break;
    case DSK_ASN1_TYPE_REAL:
      switch (value->v_real.real_type)
        {
        case DSK_ASN1_REAL_ZERO:
          dsk_buffer_printf (buffer, "%*sreal: zero\n", indent, "");
          break;
        case DSK_ASN1_REAL_INFINITY:
          dsk_buffer_printf (buffer, "%*sreal: %c%s\n",
                             indent, "", value->v_real.is_signed ? '-' : '+',
                             DSK_HTML_ENTITY_UTF8_infin);
          break;
        case DSK_ASN1_REAL_BINARY:
          if (value->v_real.overflowed)
            dsk_buffer_printf (buffer, "%*s: real: binary: overflowed\n", 
                               indent, "");
          else
            dsk_buffer_printf (buffer, "%*s: real: binary: double-value=%.22g\n", 
                               indent, "", value->v_real.double_value);
          dsk_buffer_printf (buffer, "%*s sign=%c base_log2=%u base=%u binary_scaling_factor=%u\n",
                             indent + 4, "",
                             value->v_real.is_signed ? '-' : '+',
                             value->v_real.log2_base,
                             1 << value->v_real.log2_base,
                             value->v_real.binary_scaling_factor);
          dsk_buffer_printf (buffer, "%*s exp=",
                             indent + 4, "");
          dump_hex_oneline (buffer, value->v_real.exp_start, value->v_real.num_start);
          dsk_buffer_printf (buffer, "\"\n%*s num=",
                             indent + 4, "");
          dump_hex_oneline (buffer, value->v_real.num_start, value->value_end);
          dsk_buffer_append_string (buffer, "\"\n");
          break;
              
        case DSK_ASN1_REAL_DECIMAL:
          dsk_buffer_printf (buffer, "%*s: real: decimal: double-value=%.22g\n", 
                             indent, "", value->v_real.double_value);
          dsk_buffer_printf (buffer, "%*s\"%s\"\n", 
                             indent + 4, "", value->v_real.as_string);
          break;
        }
      break;
    case DSK_ASN1_TYPE_ENUMERATED:
      dsk_buffer_printf (buffer, "%*s: enumerated: value=%lld [0x%llx]\n",
                         indent, "", value->v_enumerated, value->v_enumerated);
      break;
    case DSK_ASN1_TYPE_ENUMERATED_PDV:
      assert(false);   //TODO
      break;

    case DSK_ASN1_TYPE_SEQUENCE:
      dsk_buffer_printf (buffer, "%*ssequence: length=%u\n",
                         indent, "", (unsigned) value->v_sequence.n_children);
      for (unsigned i = 0; i < value->v_sequence.n_children; i++)
        dump_to_buffer (value->v_sequence.children[i], buffer, indent + 2);
      break;
    case DSK_ASN1_TYPE_SET:
      dsk_buffer_printf (buffer, "%*sset: length=%u\n",
                         indent, "", (unsigned) value->v_set.n_children);
      for (unsigned i = 0; i < value->v_set.n_children; i++)
        dump_to_buffer (value->v_set.children[i], buffer, indent + 2);
      break;
        
    case DSK_ASN1_TYPE_UTF8_STRING:
    case DSK_ASN1_TYPE_OCTET_STRING:
    case DSK_ASN1_TYPE_NUMERIC_STRING:
    case DSK_ASN1_TYPE_PRINTABLE_STRING:
    case DSK_ASN1_TYPE_TELETEX_STRING:
    case DSK_ASN1_TYPE_VIDEOTEXT_STRING:
    case DSK_ASN1_TYPE_ASCII_STRING:
    case DSK_ASN1_TYPE_UTC_TIME:
    case DSK_ASN1_TYPE_GENERALIZED_TIME:
    case DSK_ASN1_TYPE_GRAPHIC_STRING:
    case DSK_ASN1_TYPE_VISIBLE_STRING:
    case DSK_ASN1_TYPE_GENERAL_STRING:
    case DSK_ASN1_TYPE_UNIVERSAL_STRING:
    case DSK_ASN1_TYPE_CHARACTER_STRING:
    case DSK_ASN1_TYPE_BMP_STRING:
      {
        unsigned len = value->value_end - value->value_start;
        dsk_buffer_printf (buffer, "%*s%s:\n", indent, "", dsk_asn1_type_name_lowercase (value->type));
        if (is_ascii_printable (len, value->value_start))
          {
            for (unsigned i = 0; i < len; i++)
              {
                if (i % 32 == 0)
                  dsk_buffer_printf (buffer, "%*s %c", indent, "", i == 0 ? '[' : ' ');
                dsk_buffer_append_byte (buffer, value->value_start[i]);
                if (i + 1 == len)
                  dsk_buffer_append_byte (buffer, ']');
                if (i % 32 == 31 || i + 1 == len)
                  dsk_buffer_append_byte(buffer, '\n');
              }
          }
        else
          {
            unsigned i = 0;
            while (i < len)
              {
                unsigned n = len - i;
                if (n > 8)
                  n = 8;

                dsk_buffer_printf (buffer, "%*s %8x:", indent, "", i);
                for (unsigned j = 0; j < n; j++)
                  dsk_buffer_printf (buffer, "  %02x ", value->value_start[i+j]);
                dsk_buffer_printf (buffer, "\n%*s %8x:", indent, "", i);
                for (unsigned j = 0; j < n; j++)
                  dsk_buffer_printf (buffer, " %s", dsk_ascii_fixed4_byte_name (value->value_start[i+j]));
                dsk_buffer_append_byte (buffer, '\n');
                i += n;
                if (i < len)
                  dsk_buffer_append_byte (buffer, '\n');
              }
          }
      }
      break;
    default:
      break;
    }
}

void
dsk_asn1_value_dump_to_buffer (const DskASN1Value *value,
                               DskBuffer *buffer)
{
  dump_to_buffer (value, buffer, 0);
}

char *
dsk_asn1_primitive_value_to_string (const DskASN1Value *value)
{
  switch (value->type)
    {
      case DSK_ASN1_TYPE_BOOLEAN:
        return dsk_strdup (value->v_boolean ? "true" : "false");

      case DSK_ASN1_TYPE_ENUMERATED:
      case DSK_ASN1_TYPE_ENUMERATED_PDV:
      case DSK_ASN1_TYPE_INTEGER:
        /// XXX
        return dsk_strdup_printf ("%lld", (long long) value->v_integer);


      case DSK_ASN1_TYPE_NULL:
        return dsk_strdup ("NULL");

      //case DSK_ASN1_TYPE_BIT_STRING:
      //case DSK_ASN1_TYPE_OBJECT_IDENTIFIER:
      //case DSK_ASN1_TYPE_OBJECT_DESCRIPTOR:
      //case DSK_ASN1_TYPE_EXTERNAL:
      //case DSK_ASN1_TYPE_RELATIVE_OID:

      case DSK_ASN1_TYPE_REAL:
        return dsk_strdup_printf ("%.18g", value->v_real.double_value);
      case DSK_ASN1_TYPE_OCTET_STRING:
      case DSK_ASN1_TYPE_UTF8_STRING:
      case DSK_ASN1_TYPE_NUMERIC_STRING:
      case DSK_ASN1_TYPE_PRINTABLE_STRING:
      case DSK_ASN1_TYPE_TELETEX_STRING:
      case DSK_ASN1_TYPE_VIDEOTEXT_STRING:
      case DSK_ASN1_TYPE_ASCII_STRING:
      case DSK_ASN1_TYPE_UTC_TIME:
      case DSK_ASN1_TYPE_GENERALIZED_TIME:
      case DSK_ASN1_TYPE_GRAPHIC_STRING:
      case DSK_ASN1_TYPE_VISIBLE_STRING:
      case DSK_ASN1_TYPE_GENERAL_STRING:
      case DSK_ASN1_TYPE_UNIVERSAL_STRING:
      case DSK_ASN1_TYPE_CHARACTER_STRING:
      case DSK_ASN1_TYPE_BMP_STRING:
        // TODO: UTF-8 conversion (should be done along with string validation on parse)
        return dsk_strndup (value->value_end - value->value_start, (char*) value->value_start);
      default:
        assert(false);
        return NULL;
    }
}
    
