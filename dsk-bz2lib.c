#include "dsk.h"
#include <bzlib.h>

/* helper function: is there space for more data at the end of this fragment? */
static dsk_boolean
fragment_has_empty_space (DskBufferFragment *fragment)
{
  if (fragment->is_foreign)
    return DSK_FALSE;
  return (fragment->buf_max_size > fragment->buf_start + fragment->buf_length);
}


typedef struct _DskBz2libCompressorClass DskBz2libCompressorClass;
struct _DskBz2libCompressorClass
{
  DskOctetFilterClass base_class;
};
typedef struct _DskBz2libCompressor DskBz2libCompressor;
struct _DskBz2libCompressor
{
  DskOctetFilter base_instance;
  bz_stream bz2lib;
  dsk_boolean initialized;
};

static const char *
bzrv_to_string (int rv_code)
{
  switch (rv_code)
    {
    case BZ_SEQUENCE_ERROR:   return "sequence error";
    case BZ_PARAM_ERROR:      return "parameter error";
    case BZ_MEM_ERROR:        return "out-of-memory";
    case BZ_DATA_ERROR:       return "data corrupt";
    case BZ_DATA_ERROR_MAGIC: return "data corrupt- bad magic";
    case BZ_IO_ERROR:         return "I/O error";
    case BZ_UNEXPECTED_EOF:   return "unexpected end-of-file";
    case BZ_OUTBUFF_FULL:     return "output buffer full";
    case BZ_CONFIG_ERROR:     return "configuration error";
    default:                  return "unknown error";
    }
}

static dsk_boolean
dsk_bz2lib_compressor_process(DskOctetFilter *filter,
                            DskBuffer      *out,
                            unsigned        in_length,
                            const uint8_t  *in_data,
                            DskError      **error)
{
  DskBz2libCompressor *compressor = (DskBz2libCompressor *) filter;
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

      compressor->bz2lib.next_in = (char*)in_data;
      compressor->bz2lib.avail_in = in_length;

      f = out->last_frag;
      out_start = f->buf + f->buf_start + f->buf_length;
      compressor->bz2lib.next_out = (char *) out_start;
      compressor->bz2lib.avail_out = f->buf_max_size - f->buf_start - f->buf_length;
      zrv = BZ2_bzCompress (&compressor->bz2lib, BZ_RUN);
      if (zrv == BZ_OK)
        {
          unsigned amt_in = compressor->bz2lib.next_in - (char*)in_data;
          unsigned amt_out = compressor->bz2lib.next_out - (char*)out_start;
          in_length -= amt_in;
          in_data += amt_in;
          f->buf_length += amt_out;
          out->size += amt_out;
        }
      else
        {
          dsk_set_error (error, "error compressing: %s", bzrv_to_string (zrv));
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
dsk_bz2lib_compressor_finish (DskOctetFilter *filter,
                            DskBuffer      *out,
                            DskError      **error)
{
  DskBz2libCompressor *compressor = (DskBz2libCompressor *) filter;
  DskBufferFragment *prev_last_frag;
  dsk_boolean added_fragment = DSK_FALSE;
  prev_last_frag = NULL;                // silence GCC
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

      compressor->bz2lib.next_in = NULL;
      compressor->bz2lib.avail_in = 0;
      f = out->last_frag;
      out_start = f->buf + f->buf_start + f->buf_length;
      compressor->bz2lib.next_out = (char*) out_start;
      compressor->bz2lib.avail_out = f->buf_max_size - f->buf_start - f->buf_length;
      zrv = BZ2_bzCompress (&compressor->bz2lib, BZ_FINISH);
      if (zrv == BZ_OK || zrv == BZ_STREAM_END)
        {
          unsigned amt_out = compressor->bz2lib.next_out - (char*)out_start;
          f->buf_length += amt_out;
          out->size += amt_out;
          if (zrv == BZ_STREAM_END)
            break;
        }
      else
        {
          dsk_set_error (error, "error finishing compression: %s",
                         bzrv_to_string (zrv));
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

static void
dsk_bz2lib_compressor_finalize (DskBz2libCompressor *compressor)
{
  if (compressor->initialized)
    BZ2_bzCompressEnd (&compressor->bz2lib);
}
  
DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskBz2libCompressor);
static DskBz2libCompressorClass dsk_bz2lib_compressor_class =
{ {
  DSK_OBJECT_CLASS_DEFINE(DskBz2libCompressor,
                          &dsk_octet_filter_class,
			  NULL,
			  dsk_bz2lib_compressor_finalize),
  dsk_bz2lib_compressor_process,
  dsk_bz2lib_compressor_finish
} };

DskOctetFilter *dsk_bz2lib_compressor_new   (unsigned    level)
{
  DskBz2libCompressor *rv = dsk_object_new (&dsk_bz2lib_compressor_class);
  int zrv;
  zrv = BZ2_bzCompressInit (&rv->bz2lib, level, DSK_FALSE, 0);
  if (zrv != BZ_OK)
    {
      dsk_warning ("deflateInit2 returned error: %s", bzrv_to_string (zrv));
      dsk_object_unref (rv);
      return NULL;
    }
  rv->initialized = DSK_TRUE;
  return DSK_OCTET_FILTER (rv);
}




typedef struct _DskBz2libDecompressorClass DskBz2libDecompressorClass;
typedef struct _DskBz2libDecompressor DskBz2libDecompressor;
struct _DskBz2libDecompressorClass
{
  DskOctetFilterClass base_class;
};
struct _DskBz2libDecompressor
{
  DskOctetFilter base_instance;
  bz_stream bz2lib;
  unsigned initialized : 1;
  unsigned input_ended : 1;
};

static dsk_boolean
dsk_bz2lib_decompressor_process(DskOctetFilter *filter,
                            DskBuffer      *out,
                            unsigned        in_length,
                            const uint8_t  *in_data,
                            DskError      **error)
{
  DskBz2libDecompressor *decompressor = (DskBz2libDecompressor *) filter;
  DskBufferFragment *prev_last_frag;
  dsk_boolean added_fragment = DSK_FALSE;
  if (decompressor->input_ended)
    {
      if (in_length == 0)
        return DSK_TRUE;
      dsk_set_error (error, "garbage after compressed data");
      return DSK_FALSE;
    }
  prev_last_frag = NULL;                // silence GCC
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

      decompressor->bz2lib.next_in = (char *) in_data;
      decompressor->bz2lib.avail_in = in_length;

      f = out->last_frag;
      out_start = f->buf + f->buf_start + f->buf_length;
      decompressor->bz2lib.next_out = (char*) out_start;
      decompressor->bz2lib.avail_out = f->buf_max_size - f->buf_start - f->buf_length;
      zrv = BZ2_bzDecompress (&decompressor->bz2lib);
      if (zrv == BZ_OK || zrv == BZ_STREAM_END)
        {
          unsigned amt_in = decompressor->bz2lib.next_in - (char*) in_data;
          unsigned amt_out = decompressor->bz2lib.next_out - (char*) out_start;
          in_data += amt_in;
          in_length -= amt_in;
          f->buf_length += amt_out;
          out->size += amt_out;
          if (zrv == BZ_STREAM_END)
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
                         bzrv_to_string (zrv), zrv);
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
dsk_bz2lib_decompressor_finish (DskOctetFilter *filter,
                            DskBuffer      *out,
                            DskError      **error)
{
  DskBz2libDecompressor *decompressor = (DskBz2libDecompressor *) filter;
  DskBufferFragment *prev_last_frag;
  dsk_boolean added_fragment = DSK_FALSE;
  if (decompressor->input_ended)
    return DSK_TRUE;
  prev_last_frag = NULL;                // silence GCC
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

      decompressor->bz2lib.next_in = NULL;
      decompressor->bz2lib.avail_in = 0;
      f = out->last_frag;
      out_start = f->buf + f->buf_start + f->buf_length;
      decompressor->bz2lib.next_out = (char*) out_start;
      decompressor->bz2lib.avail_out = f->buf_max_size - f->buf_start - f->buf_length;
      zrv = BZ2_bzDecompress (&decompressor->bz2lib);
      if (zrv == BZ_OK || zrv == BZ_STREAM_END)
        {
          unsigned amt_out = decompressor->bz2lib.next_out - (char*) out_start;
          f->buf_length += amt_out;
          out->size += amt_out;
          if (zrv == BZ_STREAM_END)
            break;
        }
      else
        {
          dsk_set_error (error, "error finishing decompression: %s [%d]",
                         bzrv_to_string (zrv), zrv);
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

static void
dsk_bz2lib_decompressor_finalize (DskBz2libDecompressor *compressor)
{
  if (compressor->initialized)
    BZ2_bzDecompressEnd (&compressor->bz2lib);
}
  
DSK_OBJECT_CLASS_DEFINE_CACHE_DATA(DskBz2libDecompressor);
static DskBz2libDecompressorClass dsk_bz2lib_decompressor_class =
{ {
  DSK_OBJECT_CLASS_DEFINE(DskBz2libDecompressor,
                          &dsk_octet_filter_class,
			  NULL,
			  dsk_bz2lib_decompressor_finalize),
  dsk_bz2lib_decompressor_process,
  dsk_bz2lib_decompressor_finish
} };

DskOctetFilter *dsk_bz2lib_decompressor_new   (void)
{
  DskBz2libDecompressor *rv = dsk_object_new (&dsk_bz2lib_decompressor_class);
  int zrv;
  zrv = BZ2_bzDecompressInit (&rv->bz2lib, DSK_FALSE, DSK_FALSE);
  if (zrv != BZ_OK)
    {
      dsk_warning ("BZ2_bzDecompressInit returned error");
      dsk_object_unref (rv);
      return NULL;
    }
  rv->initialized = DSK_TRUE;
  return DSK_OCTET_FILTER (rv);
}
