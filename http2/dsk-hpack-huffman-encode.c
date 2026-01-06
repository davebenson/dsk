#include "http2/dsk-hpack-huffman.h"
#include "dsk-hpack-huffman-encode-generated.inc"

void dsk_hpack_huffman_encode          (size_t         data_length,
                                        const uint8_t *data,
                                        size_t        *encoded_length_out,
                                        uint8_t       *out)
{
  uint8_t *initial_out = out;
  unsigned bit_index = 0;

  for (unsigned i = 0; i < data_length; i++)
    {
      const uint8_t *encoded = byte_encodings[i];
      unsigned rem = *encoded++;        // grab length from encoded
      while (rem >= 8)
        {
          *out |= *encoded >> (8 - bit_index);
          out++;
          encoded++;
          *out = (uint8_t) ((unsigned)(*encoded) << bit_index);
          rem -= 8;
        }
      if (rem >= (8 - bit_index))
        {
          *out |= (*encoded++ >> bit_index)
          out++;
          *out = (*encoded << ...);
        }
      else if (rem > 0)
        {
          *out |= (*encoded >> bit_index);
          bit_index += rem;
        }
    }
  if (bit_index > 0)
    {
      // fill lowest 8-bit_index bits with 1
      *out |= (1 << (8 - bit_index)) - 1;
      out++;
    }
  *encoded_length_out = out - initial_out;
  return true;
}

#if 0
void dsk_hpack_huffman_encode_optimized(size_t data_length,
                                        const uint8_t *data,
                                        size_t *encoded_length_out,
                                        uint8_t *out)
{
  uint8_t *initial_out = out;
  const uint8_t *data_end = data + data_length;
  unsigned bit_index;     /* in big-endian bit notation, so bit 0 refers
                             to the high bit. But it only applies
                             to unusual_bytes */

bit_index_0:
  if (data == data_end)
    goto done;
  switch (byte_encodings[*data][0])
    {
      case 5:
        *out = byte_encodings[*data][1];
        data++;
        goto bit_index_5;
      case 6:
        *out = byte_encodings[*data][1];
        data++;
        goto bit_index_6;
      case 7:
        *out = byte_encodings[*data][1];
        data++;
        goto bit_index_7;
      case 8:
        *out = byte_encodings[*data][1];
        data++;
        out++;
        goto bit_index_0;
      default:
        bit_index = 0;
        goto unusual_bytes;
    }

bit_index_1:
  if (data == data_end)
    goto done;
  switch (byte_encodings[*data][0])
    {
      case 5:
        *out |= byte_encodings[*data][1] >> 1;
        data++;
        goto bit_index_6;
      case 6:
        *out |= byte_encodings[*data][1] >> 1;
        data++;
        goto bit_index_7;
      case 7:
        *out |= byte_encodings[*data][1] >> 1;
        data++;
        out++;
        goto bit_index_0;
      case 8:
        *out |= byte_encodings[*data][1] >> 1;
        out++;
        data++;
        out = byte_encodings[data][1] << 7;
        goto bit_index_1;
      default:
        bit_index = 0;
        goto unusual_bytes;
    }





  // encode data[-1] (we already advanced data
unusual_bytes:
  {
    unsigned len = byte_encodings[*data][0];
    // ASSERT: len > 8
    // Possible lengths: 10 11 12 13 14 15 19 20 21 22 23 24 25 26 27 28 30
#endif
