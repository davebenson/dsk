
typedef enum
{
  DSK_ZLIB_DEFAULT,
  DSK_ZLIB_RAW,
  DSK_ZLIB_GZIP,
} DskZlibMode;

DskOctetFilter *dsk_zlib_compressor_new   (DskZlibMode mode,
                                           unsigned    level);  /* 0 .. 9 */
DskOctetFilter *dsk_zlib_decompressor_new (DskZlibMode mode);

