#include "../dsk.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

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

  // pull next 52 bits for mantissa
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

  // if exponent is more than 1<<16 return overflow
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
  rv->type = data[0] & 0x1f;

  if (rv->is_constructed)
    {
      //... assemble pieces into a large slab in a mem-pool
      assert (false);                   // is constructed allowed in DER anyways?
    }
  else
    {
      if ((data[1] & 0x80) == 0)
        {
          rv->value_start = data + 2;
          rv->value_end = rv->value_start + data[1];
          if (rv->value_end > data + length)
            {
              *error = dsk_error_new ("data too short after 1-byte length");
            }
        }
      else
        {
          size_t value_len = data[1] & 0x7f;
          const uint8_t *at = data + 2;
          while (at < data + 6 && at < data + length)
            {
              value_len <<= 7;
              value_len |= *at & 0x7f;
              if ((*at++ & 0x80) == 0)
                goto primitive_got_long_length;
            }
          *error = dsk_error_new ("too short parsing length");
          return false;
primitive_got_long_length:
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
        rv->v_object_identifier.n_subidentifiers = n_subids;
        rv->v_object_identifier.subidentifiers = dsk_mem_pool_alloc (pool, sizeof(unsigned) * n_subids);
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
                    rv->v_object_identifier.subidentifiers[s_index++] = first;
                    rv->v_object_identifier.subidentifiers[s_index++] = under_construction;
                  }
                else
                  rv->v_object_identifier.subidentifiers[s_index++] = under_construction;
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
        rv->v_object_identifier.n_subidentifiers = n_subids;
        rv->v_object_identifier.subidentifiers = dsk_mem_pool_alloc (pool, sizeof(unsigned) * n_subids);
        unsigned s_index = 0;
        unsigned under_construction = 0;
        for (const uint8_t *at = rv->value_start; at < rv->value_end; at++)
          {
            under_construction <<= 7;
            under_construction |= *at & 0x7f;
            if ((*at & 0x80) == 0)
              {
                rv->v_object_identifier.subidentifiers[s_index++] = under_construction;
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
success:
  return rv;
}