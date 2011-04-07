
/* big-endian parsing */
DSK_INLINE_FUNC uint16_t dsk_uint16be_parse (const uint8_t *rep);
DSK_INLINE_FUNC int16_t  dsk_int16be_parse  (const uint8_t *rep);
DSK_INLINE_FUNC uint32_t dsk_uint32be_parse (const uint8_t *rep);
DSK_INLINE_FUNC int32_t  dsk_int32be_parse  (const uint8_t *rep);
DSK_INLINE_FUNC uint64_t dsk_uint64be_parse (const uint8_t *rep);
DSK_INLINE_FUNC int64_t  dsk_int64be_parse  (const uint8_t *rep);

/* little-endian parsing */
DSK_INLINE_FUNC uint16_t dsk_uint16le_parse (const uint8_t *rep);
DSK_INLINE_FUNC int16_t  dsk_int16le_parse  (const uint8_t *rep);
DSK_INLINE_FUNC uint32_t dsk_uint32le_parse (const uint8_t *rep);
DSK_INLINE_FUNC int32_t  dsk_int32le_parse  (const uint8_t *rep);
DSK_INLINE_FUNC uint64_t dsk_uint64le_parse (const uint8_t *rep);
DSK_INLINE_FUNC int64_t  dsk_int64le_parse  (const uint8_t *rep);




#if DSK_CAN_INLINE || defined(DSK_IMPLEMENT_INLINES)
DSK_INLINE_FUNC uint16_t dsk_uint16be_parse (const uint8_t *rep)
{
  return (((uint16_t)rep[0]) << 8)
       | (((uint16_t)rep[1]) << 0);
}
DSK_INLINE_FUNC int16_t  dsk_int16be_parse  (const uint8_t *rep)
{
  return (int16_t) dsk_uint16be_parse (rep);
}
DSK_INLINE_FUNC uint32_t dsk_uint32be_parse (const uint8_t *rep)
{
  return (((uint32_t)rep[0]) << 24)
       | (((uint32_t)rep[1]) << 16)
       | (((uint32_t)rep[2]) << 8)
       | (((uint32_t)rep[3]) << 0);
}
DSK_INLINE_FUNC int32_t  dsk_int32be_parse  (const uint8_t *rep)
{
  return (int32_t) dsk_uint32be_parse (rep);
}
DSK_INLINE_FUNC uint64_t dsk_uint64be_parse (const uint8_t *rep)
{
  return (((uint64_t)rep[0]) << 56)
       | (((uint64_t)rep[1]) << 48)
       | (((uint64_t)rep[2]) << 40)
       | (((uint64_t)rep[3]) << 32)
       | (((uint64_t)rep[4]) << 24)
       | (((uint64_t)rep[5]) << 16)
       | (((uint64_t)rep[6]) << 8)
       | (((uint64_t)rep[7]) << 0);
}
DSK_INLINE_FUNC int64_t  dsk_int64be_parse  (const uint8_t *rep)
{
  return (int64_t) dsk_uint64be_parse (rep);
}

/* little-endian parsing */
DSK_INLINE_FUNC uint16_t dsk_uint16le_parse (const uint8_t *rep)
{
  return (((uint16_t)rep[0]) << 0)
       | (((uint16_t)rep[1]) << 8);
}
DSK_INLINE_FUNC int16_t  dsk_int16le_parse  (const uint8_t *rep)
{
  return (int16_t) dsk_uint16le_parse (rep);
}
DSK_INLINE_FUNC uint32_t dsk_uint32le_parse (const uint8_t *rep)
{
  return (((uint32_t)rep[0]) << 0)
       | (((uint32_t)rep[1]) << 8)
       | (((uint32_t)rep[2]) << 16)
       | (((uint32_t)rep[3]) << 24);
}
DSK_INLINE_FUNC int32_t  dsk_int32le_parse  (const uint8_t *rep)
{
  return (int32_t) dsk_uint32le_parse (rep);
}
DSK_INLINE_FUNC uint64_t dsk_uint64le_parse (const uint8_t *rep)
{
  return (((uint64_t)rep[0]) << 0 )
       | (((uint64_t)rep[1]) << 8 )
       | (((uint64_t)rep[2]) << 16)
       | (((uint64_t)rep[3]) << 24)
       | (((uint64_t)rep[4]) << 32)
       | (((uint64_t)rep[5]) << 40)
       | (((uint64_t)rep[6]) << 48)
       | (((uint64_t)rep[7]) << 56);
}
DSK_INLINE_FUNC int64_t  dsk_int64le_parse  (const uint8_t *rep)
{
  return (int64_t) dsk_uint64le_parse (rep);
}
#endif
