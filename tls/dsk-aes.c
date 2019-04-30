#include "dsk-aes.h"

#define ADDROUNDKEY(state, w, round) \
    do{ for (unsigned i = 0; i < 16; i++) \
          state[i] ^= w[16*round + i]; } while(0)

#define SUBBYTES(state) \
    do{ for (unsigned i = 0; i < 16; i++) \
          state[i] = subbytes_table[state[i]]; }while(0)

#define SHIFTROWS(state) \
    do{ uint8_t tmp; \
        tmp = state[4]; \
        state[4] = state[5]; \
        state[5] = state[6]; \
        state[6] = state[7]; \
        state[7] = tmp; \
        tmp = state[8]; \
        state[8] = state[10]; \
        state[10] = tmp; \
        tmp = state[9]; \
        state[9] = state[11]; \
        state[11] = tmp; \
        tmp = state[12]; \
        state[12] = state[15]; \
        state[15] = state[14]; \
        state[14] = state[13]; \
        state[13] = tmp; }while(0)

static inline uint8_t xtime(uint8_t x)
{
  return ((x<<1) ^ (((x>>7) & 1) * 0x1b));
}
#define XTIME(t)           t = xtime(t)

#define MIXCOLUMNS(state) \
  do{ \
    for (unsigned i = 0; i < 4; ++i) \
      {   \
        uint8_t s0 = state[4*i+0]; \
        uint8_t s1 = state[4*i+1]; \
        uint8_t s2 = state[4*i+2]; \
        uint8_t s3 = state[4*i+3]; \
        uint8_t x = s0 ^ s1 ^ s2 ^ s3; \
        uint8_t tm; \
        tm = s0 ^ s1; XTIME(tm); state[4*i+0] ^= tm ^ x; \
        tm = s1 ^ s2; XTIME(tm); state[4*i+1] ^= tm ^ x; \
        tm = s2 ^ s3; XTIME(tm); state[4*i+2] ^= tm ^ x; \
        tm = s3 ^ s0; XTIME(tm); state[4*i+3] ^= tm ^ x; \
      } \
  }while(0)

#define XOR4(out, a, b) \
  do{ \
    (out)[0] = (a)[0] ^ (b)[0]; \
    (out)[1] = (a)[1] ^ (b)[1]; \
    (out)[2] = (a)[2] ^ (b)[2]; \
    (out)[3] = (a)[3] ^ (b)[3]; \
  }while(0)

#if 0
/* Figure 11, from FIPS 197. */
static void
dsk_aes_expand_key (unsigned Nk,   // in "words", will be 8,12,16
                    unsigned n_rounds,
                    const uint8_t *cipher_key,
                    uint8_t *out)
{
  unsigned i, j, k;
  uint8_t tempa[4]; // Used for the column/row operations

  // The first round key is the key itself.
  memcpy (out, cipher_key, Nk * 4);

  // 'i' is in 4-byte units, same as in Figure 11.
  for (i = Nk; i < 4 * (n_rounds + 1); i++)
    {
      //temp = w[i-1] ... we inline the extraction of temp
      if (i % Nk == 0)
        {
          //temp = SubWord(RotWord(out+4*(i-1)) xor Rcon(i/Nk)

          // rotted := RotWord(out + 4 * (i-1))
          const uint8_t rotted[4] = { out[4*(i-1)+1],
                                      out[4*(i-1)+2],
                                      out[4*(i-1)+3],
                                      out[4*(i-1)+0] };
          const uint8_t subbed[4] = { subbytes_table[rotted[0]],
                                      subbytes_table[rotted[1]],
                                      subbytes_table[rotted[2]],
                                      subbytes_table[rotted[3]] };
          unsigned rcon_idx = (i / Nk);
          XOR4(tmp, subbed, Rcon + rcon_idx * 4);
        }
      else if (key_size / 4 > 6 && i % (key_size / 4) == 4)
        {
          tmp[0] = subbytes_table[out[4*(i-1)+0]];
          tmp[1] = subbytes_table[out[4*(i-1)+1]];
          tmp[2] = subbytes_table[out[4*(i-1)+2]];
          tmp[3] = subbytes_table[out[4*(i-1)+3]];
        }
      else
        {
          memcpy (tmp, out + 4 * (i-1), 4);
        }
      out[i*4+0] = out[(i - Nk) * 4 + 0] ^ tmp[0];
      out[i*4+1] = out[(i - Nk) * 4 + 1] ^ tmp[1];
      out[i*4+2] = out[(i - Nk) * 4 + 2] ^ tmp[2];
      out[i*4+3] = out[(i - Nk) * 4 + 3] ^ tmp[3];
    }
}
#endif

// Nr=num_rounds=10;  Nk=4 (4 32-bit words = 128-bits)
void
dsk_aes128_encryptor_init(DskAES128Encryptor   *s,
                          const uint8_t        *key)
{
  unsigned i, j, k;
  uint8_t tempa[4]; // Used for the column/row operations

  // The first round key is the key itself.
  memcpy (out, cipher_key, 16);

  // 'i' is in 4-byte units, same as in Figure 11.
  for (i = 4; i < 4 * 11; i++)
    {
      //temp = w[i-1] ... we inline the extraction of temp
      if (i % 4 == 0)
        {
          //temp = SubWord(RotWord(out+4*(i-1)) xor Rcon(i/Nk)

          // rotted := RotWord(out + 4 * (i-1))
          const uint8_t rotted[4] = { out[4*(i-1)+1],
                                      out[4*(i-1)+2],
                                      out[4*(i-1)+3],
                                      out[4*(i-1)+0] };
          const uint8_t subbed[4] = { subbytes_table[rotted[0]],
                                      subbytes_table[rotted[1]],
                                      subbytes_table[rotted[2]],
                                      subbytes_table[rotted[3]] };
          unsigned rcon_idx = (i / Nk);
          XOR4(tmp, subbed, Rcon + rcon_idx * 4);
        }
      else
        {
          memcpy (tmp, out + 4 * (i-1), 4);
        }
      XOR4(out + i * 4, out + (i - 4) * 4, tmp);
    }
}

void
dsk_aes128_encrypt_inplace(const DskAES128Encryptor *s,
                           uint8_t        *in_out)      /* length 16 */
{
  ADDROUNDKEY(in_out, s->w, 0);
  for (unsigned round = 1; round < 10; round++)
    {
      SUBBYTES(in_out);
      SHIFTROWS(in_out);
      MIXCOLUMNS(in_out);
      ADDROUNDKEY(in_out, s->w, round);
    }
  SUBBYTES(in_out);
  SHIFTROWS(in_out);
  ADDROUNDKEY(in_out, s->w, 10);
}
