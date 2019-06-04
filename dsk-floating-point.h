

#define DSK_DOUBLE_MANTISSA_BITS   52
#define DSK_DOUBLE_EXPONENT_BITS   11
#define DSK_DOUBLE_EXPONENT_BIAS   1023

// exponent goes from -1023 to +1024.
// mantissa has 52 bits;
// there is an implied one in front of the mantissa
// for a normalized floating point number.
DSK_INLINE uint64_t dsk_double_as_uint64 (uint8_t sign_bit,
                                               int16_t exponent,
                                               uint64_t mantissa);

#define DSK_FLOAT_MANTISSA_BITS   23
#define DSK_FLOAT_EXPONENT_BITS   8
#define DSK_FLOAT_EXPONENT_BIAS   127

// exponent goes from -127 to 128.
// mantissa is 23 bits with an implied 1 in front.
DSK_INLINE uint32_t dsk_float_as_uint32  (uint8_t sign_bit,
                                               int16_t exponent,
                                               uint32_t mantissa);



DSK_INLINE uint64_t dsk_double_as_uint64 (uint8_t sign_bit,
                                               int16_t exponent,
                                               uint64_t mantissa)
{
  uint64_t sign = (uint64_t) sign_bit << 63;
  uint64_t exp = (uint64_t) ((exponent + DSK_DOUBLE_EXPONENT_BIAS) & 0x7ff) << 52;
  uint64_t mant = mantissa & 0xfffffffffffffULL;
  return sign | exp | mant;
}

DSK_INLINE uint32_t dsk_float_as_uint32  (uint8_t sign_bit,
                                               int16_t exponent,
                                               uint32_t mantissa)
{
  uint32_t sign = (uint32_t) sign_bit << 31;
  uint32_t exp = (uint32_t) ((exponent + DSK_FLOAT_EXPONENT_BIAS) & 0xff) << 23;
  uint32_t mant = mantissa & 0x7fffff;
  return sign | exp | mant;
}
