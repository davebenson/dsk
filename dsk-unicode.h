
DSK_INLINE_FUNC dsk_boolean dsk_unicode_is_surrogate (uint32_t value);
DSK_INLINE_FUNC dsk_boolean dsk_unicode_is_invalid   (uint32_t value);

/* UTF-16 and UTF-32 Byte-Order Mark (abbrev 'bom').
   Used to tell the endianness of the content.  */
#define DSK_UNICODE_BOM 0xfeff

#if DSK_CAN_INLINE || defined(DSK_IMPLEMENT_INLINES)
DSK_INLINE_FUNC dsk_boolean
dsk_unicode_is_surrogate (uint32_t value)
{
  return (0xd800 <= value && value <= 0xdbff)   /* high surrogate */
      || (0xdc00 <= value && value <= 0xdfff);  /* low surrogate */
}
DSK_INLINE_FUNC dsk_boolean dsk_unicode_is_invalid   (uint32_t value)
{
  return (value & 0xfffe) == 0xfffe
      || (0xfdd0 <= value && value <= 0xfdef);
}
#endif

