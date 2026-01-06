

#include "dsk-hpack-huffman-decode-generated.inc"

// We treat the bytes array as an array of bits, most-significant-bit first.
bool dsk_hpack_huffman_decode_bitwise  (size_t bytes_length,
                                        const uint8_t *bytes,
                                        size_t *out_length_out,
                                        uint8_t *out,
                                        DskError **error)
{
  if (bytes_length == 0)
    {
      *out_length_out = 0;
      return true;
    }

  uint8_t bit_index = 0;
  uint8_t bits = *bytes++;
  unsigned out_len = 0;
  unsigned leaf_index = 0;
  bytes_length -= 1;
  for (;;)
    {
      short next = hufftree_leaf_nodes[leaf_index][bits >> 7];
      if (next < 0)
        {
           out[out_len++] = -next - 1;
           leaf_index = 0;
        }
      else
        leaf_index = next;

      if (++bit_index == 8)
        {
          if (bytes_length == 0)
            {
              if (leaf_index > 7)
                {
                  /* Padding must be all ones.
                   *
                   * The first EOS_NUM_ONES==30 entries are for
                   * strings of repeated ones.
                   *
                   * But there should never be more than 7 bits of padding,
                   * so we compare to 7 instead of EOS_NUM_ONES.
                   */
                  dsk_set_error (error, "garbage after last bits or truncated compressed data");
                  return false;
                }
              *out_length_out = out_len;
              return true;
            }
          bits = *bytes++;
          bit_index = 0;
          bytes_length -= 1;
        }
      else
        {
          bits <<= 1;
          bit_index++;
        }
    }
}


//
// Optimized version that gathers a whole byte of data,
// then look it up in a table.
//
// All multibyte sequences start with seven 1s,
// so we begin a bit-by-bit lookup after the first 7 bits.
//
bool dsk_hpack_huffman_decode          (size_t bytes_length,
                                        const uint8_t *bytes,
                                        size_t *out_length_out,
                                        uint8_t *out,
                                        DskError **error)
{
  unsigned bit_index = 0;
  unsigned bits_remaining = bytes_length * 8;
  const uint8_t *initial_out = out;

  while (bits_remaining > 0)
    {
      if (bits_remaining < 8)
        goto less_than_8_bits_remaining;

      uint8_t byte = (bit_index == 0)
                   ? *bytes
                   : (*bytes << (8 - bit_index)) | (bytes[1] >> (bit_index))
      BitCountByte lookup = bal_entries[byte];
      if (DSK_LIKELY (lookup.length != 0))
        {
          // fast case: encoding requires 8 or less bits.
          *out++ = lookup.byte;
          bit_index += lookup.length;
          if (bit_index >= 8)
            {
              bytes++;
              bytes_length--;
              bit_index -= 8;
            }
        }
      else
        {
          // slow case: this happens for rare bytes.
          int node_index = bal_entries[NODE_1111111].children[byte & 1];
          bytes++;
          bytes_length--;
          if (bytes_length == 0)
            {
              dsk_set_error (error, "8 bits of padding found: not allowed");
              return false;
            }

          // process bit-by-bit
          for (;;)
            {
              int bit = (*bytes >> (7 - bit_index)) & 1;
              node_index = bal_entries[node_index].children[bit];
              if (node_index < 0)
                {
                  *out++ = -node_index - 1;
                }
              if (++bit_index == 8)
                {
                  bytes++;
                  bytes_length--;
                  bit_index = 0;
                  if (bytes_length == 0)
                    {
                      if (node_index > 7)
                        {
                          dsk_set_error (error, "ended with more than 8 bits of padding: not allowed");
                          return false;
                        }
                      *out_length_out = out - initial_out;
                      return true;
                    }
                }

              // go back to 8-bit at a time processing.
              if (node_index < 0)
                break;
            }
        }
    }
  goto return_true;

less_than_8_bits_remaining:
  // because even the shortest encoded byte is 5 bits,
  // there is no way the we can get more than one byte 
  // out of the remaining data
  {
    uint8_t byte = *bytes << bit_index;
    BitCountByte lookup = bal_entries[byte];
    if (lookup.length == 0 || lookup.length < bits_remaining)
      {
        if (byte != (0xff << bit_index))
          goto not_padded_with_1s;
          }
        goto return_true;
      }
    *out++ = lookup.byte;
    bits_remaining -= lookup.length;
    bit_index += lookup.length;
    if (bits_remaining > 0)
      {
        if ((*bytes << bit_index) != (0xff << bit_index))
          goto not_padded_with_1s;
      }
    goto return_true;
  }

return_true:
  *out_length_out = out - initial_out;
  return true;

not_padded_with_1s: 
  dsk_set_error (error, "hpack data not padded with 1s");
  return false;
}
