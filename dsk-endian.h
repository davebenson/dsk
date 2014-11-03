
/* big-endian parsing */
DSK_INLINE_FUNC uint16_t dsk_uint16be_parse (const uint8_t *rep);
DSK_INLINE_FUNC int16_t  dsk_int16be_parse  (const uint8_t *rep);
DSK_INLINE_FUNC uint32_t dsk_uint24be_parse (const uint8_t *rep);
DSK_INLINE_FUNC int32_t  dsk_int24be_parse  (const uint8_t *rep);
DSK_INLINE_FUNC uint32_t dsk_uint32be_parse (const uint8_t *rep);
DSK_INLINE_FUNC int32_t  dsk_int32be_parse  (const uint8_t *rep);
DSK_INLINE_FUNC uint64_t dsk_uint64be_parse (const uint8_t *rep);
DSK_INLINE_FUNC int64_t  dsk_int64be_parse  (const uint8_t *rep);

/* little-endian parsing */
DSK_INLINE_FUNC uint16_t dsk_uint16le_parse (const uint8_t *rep);
DSK_INLINE_FUNC int16_t  dsk_int16le_parse  (const uint8_t *rep);
DSK_INLINE_FUNC uint32_t dsk_uint24le_parse (const uint8_t *rep);
DSK_INLINE_FUNC int32_t  dsk_int24le_parse  (const uint8_t *rep);
DSK_INLINE_FUNC uint32_t dsk_uint32le_parse (const uint8_t *rep);
DSK_INLINE_FUNC int32_t  dsk_int32le_parse  (const uint8_t *rep);
DSK_INLINE_FUNC uint64_t dsk_uint64le_parse (const uint8_t *rep);
DSK_INLINE_FUNC int64_t  dsk_int64le_parse  (const uint8_t *rep);

/* big-endian packing */
DSK_INLINE_FUNC void     dsk_uint16be_pack  (uint16_t       value,
                                             uint8_t       *rep);
DSK_INLINE_FUNC void     dsk_int16be_pack   (int16_t        value,
                                             uint8_t       *rep);
DSK_INLINE_FUNC void     dsk_uint24be_pack  (uint32_t       value,
                                             uint8_t       *rep);
DSK_INLINE_FUNC void     dsk_int24be_pack   (int32_t        value,
                                             uint8_t       *rep);
DSK_INLINE_FUNC void     dsk_uint32be_pack  (uint32_t       value,
                                             uint8_t       *rep);
DSK_INLINE_FUNC void     dsk_int32be_pack   (int32_t        value,
                                             uint8_t       *rep);
DSK_INLINE_FUNC void     dsk_uint64be_pack  (uint64_t       value,
                                             uint8_t       *rep);
DSK_INLINE_FUNC void     dsk_int64be_pack   (int64_t        value,
                                             uint8_t       *rep);

/* little-endian packing */
DSK_INLINE_FUNC void     dsk_uint16le_pack  (uint16_t       value,
                                             uint8_t       *rep);
DSK_INLINE_FUNC void     dsk_int16le_pack   (int16_t        value,
                                             uint8_t       *rep);
DSK_INLINE_FUNC void     dsk_uint32le_pack  (uint32_t       value,
                                             uint8_t       *rep);
DSK_INLINE_FUNC void     dsk_int32le_pack   (int32_t        value,
                                             uint8_t       *rep);
DSK_INLINE_FUNC void     dsk_uint64le_pack  (uint64_t       value,
                                             uint8_t       *rep);
DSK_INLINE_FUNC void     dsk_int64le_pack   (int64_t        value,
                                             uint8_t       *rep);




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
DSK_INLINE_FUNC uint32_t dsk_uint24be_parse (const uint8_t *rep)
{
  return (((uint32_t)rep[0]) << 16)
       | (((uint32_t)rep[1]) << 8)
       | (((uint32_t)rep[2]) << 0);
}
DSK_INLINE_FUNC int32_t dsk_int24be_parse (const uint8_t *rep)
{
  return ((rep[0]&0x80) ? 0xff000000 : 0)       /* sign extend */
       | (((uint32_t)rep[0]) << 16)
       | (((uint32_t)rep[1]) << 8)
       | (((uint32_t)rep[2]) << 0);
}
DSK_INLINE_FUNC int32_t  dsk_int32be_parse  (const uint8_t *rep)
{
  return (int32_t) dsk_uint32be_parse (rep);
}
DSK_INLINE_FUNC uint32_t dsk_uint32be_parse (const uint8_t *rep)
{
  return (((uint32_t)rep[0]) << 24)
       | (((uint32_t)rep[1]) << 16)
       | (((uint32_t)rep[2]) << 8)
       | (((uint32_t)rep[3]) << 0);
}
DSK_INLINE_FUNC uint64_t dsk_uint64be_parse (const uint8_t *rep)
{
#if DSK_SIZEOF_SIZE_T == 4
  uint32_t hi = dsk_uint32be_parse (rep + 0);
  uint32_t lo = dsk_uint32be_parse (rep + 4);
  return (((uint64_t)hi) << 32) | (lo);
#else
  return (((uint64_t)rep[0]) << 56)
       | (((uint64_t)rep[1]) << 48)
       | (((uint64_t)rep[2]) << 40)
       | (((uint64_t)rep[3]) << 32)
       | (((uint64_t)rep[4]) << 24)
       | (((uint64_t)rep[5]) << 16)
       | (((uint64_t)rep[6]) << 8)
       | (((uint64_t)rep[7]) << 0);
#endif
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
DSK_INLINE_FUNC uint32_t dsk_uint24le_parse (const uint8_t *rep)
{
  return (((uint32_t)rep[2]) << 16)
       | (((uint32_t)rep[1]) << 8)
       | (((uint32_t)rep[0]) << 0);
}
DSK_INLINE_FUNC int32_t dsk_int24le_parse (const uint8_t *rep)
{
  return ((rep[2]&0x80) ? (int32_t)0xff000000 : 0)       /* sign extend */
       | (((int32_t)rep[2]) << 16)
       | (((int32_t)rep[1]) << 8)
       | (((int32_t)rep[0]) << 0);
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
#if DSK_SIZEOF_SIZE_T == 4
  uint32_t hi = dsk_uint32le_parse (rep + 4);
  uint32_t lo = dsk_uint32le_parse (rep + 0);
  return (((uint64_t)hi) << 32) | (lo);
#else
  return (((uint64_t)rep[0]) << 0 )
       | (((uint64_t)rep[1]) << 8 )
       | (((uint64_t)rep[2]) << 16)
       | (((uint64_t)rep[3]) << 24)
       | (((uint64_t)rep[4]) << 32)
       | (((uint64_t)rep[5]) << 40)
       | (((uint64_t)rep[6]) << 48)
       | (((uint64_t)rep[7]) << 56);
#endif
}
DSK_INLINE_FUNC int64_t  dsk_int64le_parse  (const uint8_t *rep)
{
  return (int64_t) dsk_uint64le_parse (rep);
}

/* --- packing --- */
DSK_INLINE_FUNC void     dsk_uint16be_pack  (uint16_t       value,
                                             uint8_t       *rep)
{
  rep[0] = value>>8;
  rep[1] = value>>0;
}
DSK_INLINE_FUNC void     dsk_int16be_pack   (int16_t        value,
                                             uint8_t       *rep)
{
  dsk_uint16be_pack (value, rep);
}
DSK_INLINE_FUNC void     dsk_uint24be_pack  (uint32_t       value,
                                             uint8_t       *rep)
{
  rep[0] = value>>16;
  rep[1] = value>>8;
  rep[2] = value>>0;
}
DSK_INLINE_FUNC void     dsk_int24be_pack   (int32_t        value,
                                             uint8_t       *rep)
{
  dsk_uint24be_pack (value, rep);
}
DSK_INLINE_FUNC void     dsk_uint32be_pack  (uint32_t       value,
                                             uint8_t       *rep)
{
  rep[0] = value>>24;
  rep[1] = value>>16;
  rep[2] = value>>8;
  rep[3] = value>>0;
}
DSK_INLINE_FUNC void     dsk_int32be_pack   (int32_t        value,
                                             uint8_t       *rep)
{
  dsk_uint32be_pack (value, rep);
}
DSK_INLINE_FUNC void     dsk_uint64be_pack  (uint64_t       value,
                                             uint8_t       *rep)
{
#if DSK_SIZEOF_SIZE_T == 4
  dsk_uint32be_pack (value >> 32, rep);
  dsk_uint32be_pack (value >> 0, rep + 4);
#else
  rep[0] = value>>56;
  rep[1] = value>>48;
  rep[2] = value>>40;
  rep[3] = value>>32;
  rep[4] = value>>24;
  rep[5] = value>>16;
  rep[6] = value>>8;
  rep[7] = value>>0;
#endif
}
DSK_INLINE_FUNC void     dsk_int64be_pack   (int64_t        value,
                                             uint8_t       *rep)
{
  dsk_uint64be_pack (value, rep);
}

/* little-endian packing */
DSK_INLINE_FUNC void     dsk_uint16le_pack  (uint16_t       value,
                                             uint8_t       *rep)
{
  rep[0] = value>>0;
  rep[1] = value>>8;
}
DSK_INLINE_FUNC void     dsk_int16le_pack   (int16_t        value,
                                             uint8_t       *rep)
{
  dsk_uint16le_pack (value, rep);
}
DSK_INLINE_FUNC void     dsk_uint24le_pack  (uint32_t       value,
                                             uint8_t       *rep)
{
  rep[0] = value>>0;
  rep[1] = value>>8;
  rep[2] = value>>16;
}
DSK_INLINE_FUNC void     dsk_int24le_pack   (int32_t        value,
                                             uint8_t       *rep)
{
  dsk_uint24le_pack (value, rep);
}
DSK_INLINE_FUNC void     dsk_uint32le_pack  (uint32_t       value,
                                             uint8_t       *rep)
{
  rep[0] = value>>0;
  rep[1] = value>>8;
  rep[2] = value>>16;
  rep[3] = value>>24;
}
DSK_INLINE_FUNC void     dsk_int32le_pack   (int32_t        value,
                                             uint8_t       *rep)
{
  dsk_uint32le_pack (value, rep);
}
DSK_INLINE_FUNC void     dsk_uint64le_pack  (uint64_t       value,
                                             uint8_t       *rep)
{
#if DSK_SIZEOF_SIZE_T == 4
  dsk_uint32le_pack (value >> 32, rep + 4);
  dsk_uint32le_pack (value >> 0, rep);
#else
  rep[0] = value>>0;
  rep[1] = value>>8;
  rep[2] = value>>16;
  rep[3] = value>>24;
  rep[4] = value>>32;
  rep[5] = value>>40;
  rep[6] = value>>48;
  rep[7] = value>>56;
#endif
}
DSK_INLINE_FUNC void     dsk_int64le_pack   (int64_t        value,
                                             uint8_t       *rep)
{
  dsk_uint64le_pack (value, rep);
}

#endif
