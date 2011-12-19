
DSK_INLINE_FUNC dsk_boolean dsk_unicode_is_surrogate (uint32_t value);

#if DSK_CAN_INLINE || defined(DSK_IMPLEMENT_INLINES)
DSK_INLINE_FUNC dsk_boolean
dsk_unicode_is_surrogate (uint32_t value)
{
  return (0xd800 <= value && value <= 0xdbff)   /* high surrogate */
      || (0xdc00 <= value && value <= 0xdfff);  /* low surrogate */
}
#endif

