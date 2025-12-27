

#include "dsk-hpack-huffman-generated.inc"

bool dsk_hpack_huffman_decode_bitwise  (size_t bytes_length,
                                        const uint8_t *bytes,
                                        size_t *out_length_out,
                                        uint8_t *out,
                                        DskError **error)
{
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

bool dsk_hpack_huffman_decode          (size_t bytes_length,
                                        const uint8_t *bytes,
                                        size_t *out_length_out,
                                        uint8_t *out,
                                        DskError **error)
{
  unsigned bit_index = 0;
  unsigned bits_remaining = bytes_length * 8;

  while (bits_remaining > 0)
    {
      if (bits_remaining < 8)
        goto less_than_8_bits_remaining;

      uint8_t byte = (bit_index == 0)
                   ? *bytes
                   : (*bytes << (8 - bit_index)) | (bytes[1] >> (bit_index))
      BitCountByte lookup = bal_entries[byte];
      if (lookup.length == 0)
        {
          bit_index += 7;
          if (bit_index >= 8)
            {
              bytes++;
              bytes_length--;
              bit_index -= 8;
            }
          int node_index = NODE_1111111;
          ... process bit-by-bit
        }
      else
        {
          *out++ = lookup.byte;
          bit_index += lookup.length;
          if (bit_index >= 8)
            {
              bytes++;
              bytes_length--;
              bit_index -= 8;
            }
        }
    }
  *out_length_out = out - init_out;
  return true;

less_than_8_bits_remaining:
  // because even the shortest encoded byte is 5 bits,
  // there is no way the we can get more than one byte 
  // out of the remaining data
  {
    uint8_t byte = *bytes << bit_index;
    BitCountByte lookup = bal_entries[byte];
    if (lookup.length == 0)
      {
        ....
      }
    if (lookup.length < (8 - bit_index))
      {
        ...
      }
  }

ensure_remaining_bits_1:
  // all remaining bits must be set to 1
}
