
#define DSK_UTF16_HI_SURROGATE_START    0xd800
#define DSK_UTF16_HI_SURROGATE_END      (0xd800 + 2047 - 64)
#define DSK_UTF16_LO_SURROGATE_START    0xdc00
#define DSK_UTF16_LO_SURROGATE_END      (0xdc00 + 1023)

DSK_INLINE_FUNC void dsk_utf16_to_surrogate_pair (uint32_t  unicode,
                                                  uint16_t *pair_out);


#if DSK_CAN_INLINE || defined(DSK_IMPLEMENT_INLINES)
DSK_INLINE_FUNC void
dsk_utf16_to_surrogate_pair (uint32_t  unicode,
                             uint16_t *pair_out)
{
  _dsk_inline_assert (unicode >= 0x10000);
  pair_out[0] = DSK_UTF16_HI_SURROGATE_START
              | ((unicode - 0x10000) >> 10);
  pair_out[1] = DSK_UTF16_LO_SURROGATE_START
              | (unicode & 0x3ff);
}
#endif
