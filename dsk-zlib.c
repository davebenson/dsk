#include "dsk.h"
#include <zlib.h>

/* helper function: is there space for more data at the end of this fragment? */
static dsk_boolean
fragment_has_empty_space (DskBufferFragment *fragment)
{
  if (fragment->is_foreign)
    return DSK_FALSE;
  return (fragment->buf_max_size > fragment->buf_start + fragment->buf_length);
}


typedef struct _DskZlibCompressorClass DskZlibCompressorClass;
struct _DskZlibCompressorClass
{
  DskOctetFilterClass base_class;
};
typedef struct _DskZlibCompressor DskZlibCompressor;
struct _DskZlibCompressor
{
  DskOctetFilter base_instance;
  z_stream zlib;
  dsk_boolean initialized;
};

static dsk_boolean
dsk_zlib_compressor_process(DskOctetFilter *filter,
                            DskBuffer      *out,
                            unsigned        in_length,
                            const uint8_t  *in_data,
                            DskError      **error)
{
  DskZlibCompressor *compressor = (DskZlibCompressor *) filter;
  DskBufferFragment *prev_last_frag;
  dsk_boolean added_fragment = DSK_FALSE;
  while (in_length > 0)
    {
      DskBufferFragment *f;
      uint8_t *out_start;
      int zrv;
      if (out->last_frag == NULL
       || !fragment_has_empty_space (out->last_frag))
        {
          added_fragment = DSK_TRUE;
          prev_last_frag = out->last_frag;
          dsk_buffer_append_empty_fragment (out);
        }

      compressor->zlib.next_in = (uint8_t*)in_data;
      compressor->zlib.avail_in = in_length;

      f = out->last_frag;
      out_start = f->buf + f->buf_start + f->buf_length;
      compressor->zlib.next_out = out_start;
      compressor->zlib.avail_out = f->buf_max_size - f->buf_start - f->buf_length;
      zrv = deflate (&compressor->zlib, Z_NO_FLUSH);
      if (zrv == Z_OK)
        {
          unsigned amt_in = compressor->zlib.next_in - in_data;
          unsigned amt_out = compressor->zlib.next_out - out_start;
          in_length -= amt_in;
          in_data += amt_in;
          f->buf_length += amt_out;
          out->size += amt_out;
        }
      else
        {
          dsk_set_error (error, "error compressing: %s",
                         compressor->zlib.msg);
          dsk_buffer_maybe_remove_empty_fragment (out);
          return DSK_FALSE;
        }
    }

  /* If we added a fragment that we didn't use,
     remove it. */
  if (added_fragment && out->last_frag->buf_length == 0)
    {
      dsk_buffer_fragment_free (out->last_frag);
      out->last_frag = prev_last_frag;
      if (out->last_frag == NULL)
        out->first_frag = NULL;
    }
  return DSK_TRUE;
}

static dsk_boolean
dsk_zlib_compressor_finish (DskOctetFilter *filter,
                            DskBuffer      *out,
                            DskError      **error)
{
  DskZlibCompressor *compressor = (DskZlibCompressor *) filter;
  DskBufferFragment *prev_last_frag;
  dsk_boolean added_fragment = DSK_FALSE;
  for (;;)
    {
      DskBufferFragment *f;
      uint8_t *out_start;
      int zrv;
      if (out->last_frag == NULL
       || !fragment_has_empty_space (out->last_frag))
        {
          added_fragment = DSK_TRUE;
          prev_last_frag = out->last_frag;
          dsk_buffer_append_empty_fragment (out);
        }

      compressor->zlib.next_in = NULL;
      compressor->zlib.avail_in = 0;
      f = out->last_frag;
      out_start = f->buf + f->buf_start + f->buf_length;
      compressor->zlib.next_out = out_start;
      compressor->zlib.avail_out = f->buf_max_size - f->buf_start - f->buf_length;
      zrv = deflate (&compressor->zlib, Z_FINISH);
      if (zrv == Z_OK || zrv == Z_STREAM_END)
        {
          unsigned amt_out = compressor->zlib.next_out - out_start;
          f->buf_length += amt_out;
          out->size += amt_out;
          if (zrv == Z_STREAM_END)
            break;
        }
      else
        {
          dsk_set_error (error, "error finishing compression: %s",
                         compressor->zlib.msg);
          dsk_buffer_maybe_remove_empty_fragment (out);
          return DSK_FALSE;
        }
    }

  /* If we added a fragment that we didn't use,
     remove it. */
  if (added_fragment && out->last_frag->buf_length == 0)
    {
      dsk_buffer_fragment_free (out->last_frag);
      out->last_frag = prev_last_frag;
      if (out->last_frag == NULL)
        out->first_frag = NULL;
    }
  return DSK_TRUE;
}

#define dsk_zlib_compressor_init NULL
static void
dsk_zlib_compressor_finalize (DskZlibCompressor *compressor)
{
  if (compressor->initialized)
    deflateEnd (&compressor->zlib);
}
  
DSK_OCTET_FILTER_SUBCLASS_DEFINE(static, DskZlibCompressor, dsk_zlib_compressor);

DskOctetFilter *dsk_zlib_compressor_new   (DskZlibMode mode,
                                           unsigned    level)
{
  DskZlibCompressor *rv = dsk_object_new (&dsk_zlib_compressor_class);
  int zrv;
  int window_bits = mode == DSK_ZLIB_DEFAULT ? 15
                  : mode == DSK_ZLIB_RAW     ? -15
                  : (16+15);
  zrv = deflateInit2 (&rv->zlib, level, Z_DEFLATED, window_bits, 8,
                      Z_DEFAULT_STRATEGY);
  if (zrv != Z_OK)
    {
      dsk_warning ("deflateInit2 returned error");
      dsk_object_unref (rv);
      return NULL;
    }
  rv->initialized = DSK_TRUE;
  return DSK_OCTET_FILTER (rv);
}




typedef struct _DskZlibDecompressorClass DskZlibDecompressorClass;
typedef struct _DskZlibDecompressor DskZlibDecompressor;
struct _DskZlibDecompressorClass
{
  DskOctetFilterClass base_class;
};
struct _DskZlibDecompressor
{
  DskOctetFilter base_instance;
  z_stream zlib;
  unsigned initialized : 1;
  unsigned input_ended : 1;
};

static dsk_boolean
dsk_zlib_decompressor_process(DskOctetFilter *filter,
                            DskBuffer      *out,
                            unsigned        in_length,
                            const uint8_t  *in_data,
                            DskError      **error)
{
  DskZlibDecompressor *decompressor = (DskZlibDecompressor *) filter;
  DskBufferFragment *prev_last_frag;
  dsk_boolean added_fragment = DSK_FALSE;
  if (decompressor->input_ended)
    {
      if (in_length == 0)
        return DSK_TRUE;
      dsk_set_error (error, "garbage after compressed data");
      return DSK_FALSE;
    }
  while (in_length > 0)
    {
      DskBufferFragment *f;
      uint8_t *out_start;
      int zrv;
      if (out->last_frag == NULL
       || !fragment_has_empty_space (out->last_frag))
        {
          added_fragment = DSK_TRUE;
          prev_last_frag = out->last_frag;
          dsk_buffer_append_empty_fragment (out);
        }

      decompressor->zlib.next_in = (uint8_t *) in_data;
      decompressor->zlib.avail_in = in_length;

      f = out->last_frag;
      out_start = f->buf + f->buf_start + f->buf_length;
      decompressor->zlib.next_out = out_start;
      decompressor->zlib.avail_out = f->buf_max_size - f->buf_start - f->buf_length;
      zrv = inflate (&decompressor->zlib, Z_NO_FLUSH);
      if (zrv == Z_OK || zrv == Z_STREAM_END)
        {
          unsigned amt_in = decompressor->zlib.next_in - in_data;
          unsigned amt_out = decompressor->zlib.next_out - out_start;
          in_data += amt_in;
          in_length -= amt_in;
          f->buf_length += amt_out;
          out->size += amt_out;
          if (zrv == Z_STREAM_END)
            {
              decompressor->input_ended = DSK_TRUE;
              if (in_length > 0)
                {
                  dsk_set_error (error, "garbage after compressed data");
                  dsk_buffer_maybe_remove_empty_fragment (out);
                  return DSK_FALSE;
                }
            }
        }
      else
        {
          dsk_set_error (error, "error decompressing: %s [%d]",
                         decompressor->zlib.msg, zrv);
          dsk_buffer_maybe_remove_empty_fragment (out);
          return DSK_FALSE;
        }
    }

  /* If we added a fragment that we didn't use,
     remove it. */
  if (added_fragment && out->last_frag->buf_length == 0)
    {
      dsk_buffer_fragment_free (out->last_frag);
      out->last_frag = prev_last_frag;
      if (out->last_frag == NULL)
        out->first_frag = NULL;
    }
  return DSK_TRUE;
}

static dsk_boolean
dsk_zlib_decompressor_finish (DskOctetFilter *filter,
                            DskBuffer      *out,
                            DskError      **error)
{
  DskZlibDecompressor *decompressor = (DskZlibDecompressor *) filter;
  DskBufferFragment *prev_last_frag;
  dsk_boolean added_fragment = DSK_FALSE;
  if (decompressor->input_ended)
    return DSK_TRUE;
  for (;;)
    {
      DskBufferFragment *f;
      uint8_t *out_start;
      int zrv;
      if (out->last_frag == NULL
       || !fragment_has_empty_space (out->last_frag))
        {
          added_fragment = DSK_TRUE;
          prev_last_frag = out->last_frag;
          dsk_buffer_append_empty_fragment (out);
        }

      decompressor->zlib.next_in = NULL;
      decompressor->zlib.avail_in = 0;
      f = out->last_frag;
      out_start = f->buf + f->buf_start + f->buf_length;
      decompressor->zlib.next_out = out_start;
      decompressor->zlib.avail_out = f->buf_max_size - f->buf_start - f->buf_length;
      zrv = inflate (&decompressor->zlib, Z_FINISH);
      if (zrv == Z_OK || zrv == Z_STREAM_END)
        {
          unsigned amt_out = decompressor->zlib.next_out - out_start;
          f->buf_length += amt_out;
          out->size += amt_out;
          if (zrv == Z_STREAM_END)
            break;
        }
      else
        {
          dsk_set_error (error, "error finishing decompression: %s [%d]",
                         decompressor->zlib.msg, zrv);
          dsk_buffer_maybe_remove_empty_fragment (out);
          return DSK_FALSE;
        }
    }

  /* If we added a fragment that we didn't use,
     remove it. */
  if (added_fragment && out->last_frag->buf_length == 0)
    {
      dsk_buffer_fragment_free (out->last_frag);
      out->last_frag = prev_last_frag;
      if (out->last_frag == NULL)
        out->first_frag = NULL;
    }
  return DSK_TRUE;
}

#define dsk_zlib_decompressor_init NULL
static void
dsk_zlib_decompressor_finalize (DskZlibDecompressor *compressor)
{
  if (compressor->initialized)
    inflateEnd (&compressor->zlib);
}
  
DSK_OCTET_FILTER_SUBCLASS_DEFINE(static, DskZlibDecompressor, dsk_zlib_decompressor);

DskOctetFilter *dsk_zlib_decompressor_new   (DskZlibMode mode)
{
  DskZlibDecompressor *rv = dsk_object_new (&dsk_zlib_decompressor_class);
  int zrv;
  int window_bits = mode == DSK_ZLIB_DEFAULT ? 15
                  : mode == DSK_ZLIB_RAW     ? -15
                  : (32+15);
  zrv = inflateInit2 (&rv->zlib, window_bits);
  if (zrv != Z_OK)
    {
      dsk_warning ("inflateInit2 returned error");
      dsk_object_unref (rv);
      return NULL;
    }
  rv->initialized = DSK_TRUE;
  return DSK_OCTET_FILTER (rv);
}
