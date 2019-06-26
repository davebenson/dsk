#include "../dsk.h"
#include <string.h>
#include <stdio.h>

#define ADDROUNDKEY(state, w, round) \
    do{ for (unsigned i = 0; i < 16; i++) \
          state[i] ^= w[16*round + i]; } while(0)

#define SUBBYTES(state) \
    do{ for (unsigned i = 0; i < 16; i++) \
          state[i] = subbytes_table[state[i]]; }while(0)

#define SHIFTROWS(state) \
    do{ uint8_t tmp; \
        tmp = state[1]; \
        state[1] = state[5]; \
        state[5] = state[9]; \
        state[9] = state[13]; \
        state[13] = tmp; \
        tmp = state[2]; \
        state[2] = state[10]; \
        state[10] = tmp; \
        tmp = state[6]; \
        state[6] = state[14]; \
        state[14] = tmp; \
        tmp = state[3]; \
        state[3] = state[15]; \
        state[15] = state[11]; \
        state[11] = state[7]; \
        state[7] = tmp; }while(0)

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

#define HEX_FMT "%02x"
#define HEX2    HEX_FMT " " HEX_FMT
#define HEX4    HEX2 " " HEX2
#define HEX8    HEX4 " " HEX4
#define HEX4x4   "    " HEX4 "\n" \
                 "    " HEX4 "\n" \
                 "    " HEX4 "\n" \
                 "    " HEX4 "\n"

#define PRINT_HEX16(label, arr) \
  fprintf(stderr, "%s:\n" HEX4x4 "\n", \
    (label), \
    (arr)[0], (arr)[4], (arr)[8], (arr)[12], \
    (arr)[1], (arr)[5], (arr)[9], (arr)[13], \
    (arr)[2], (arr)[6], (arr)[10], (arr)[14], \
    (arr)[3], (arr)[7], (arr)[11], (arr)[15]);


//
// This function is hard-coded into dsk_aes{128,192,224,256}_init().
//
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

static uint8_t subbytes_table[256] =
{
  //0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
  0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76,
  0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,
  0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,
  0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,
  0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,
  0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,
  0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,
  0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,
  0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,
  0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,
  0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,
  0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,
  0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,
  0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,
  0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,
  0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16
};
static const uint8_t inv_subbytes_table[256] = {
  0x52,0x09,0x6a,0xd5,0x30,0x36,0xa5,0x38,0xbf,0x40,0xa3,0x9e,0x81,0xf3,0xd7,0xfb,
  0x7c,0xe3,0x39,0x82,0x9b,0x2f,0xff,0x87,0x34,0x8e,0x43,0x44,0xc4,0xde,0xe9,0xcb,
  0x54,0x7b,0x94,0x32,0xa6,0xc2,0x23,0x3d,0xee,0x4c,0x95,0x0b,0x42,0xfa,0xc3,0x4e,
  0x08,0x2e,0xa1,0x66,0x28,0xd9,0x24,0xb2,0x76,0x5b,0xa2,0x49,0x6d,0x8b,0xd1,0x25,
  0x72,0xf8,0xf6,0x64,0x86,0x68,0x98,0x16,0xd4,0xa4,0x5c,0xcc,0x5d,0x65,0xb6,0x92,
  0x6c,0x70,0x48,0x50,0xfd,0xed,0xb9,0xda,0x5e,0x15,0x46,0x57,0xa7,0x8d,0x9d,0x84,
  0x90,0xd8,0xab,0x00,0x8c,0xbc,0xd3,0x0a,0xf7,0xe4,0x58,0x05,0xb8,0xb3,0x45,0x06,
  0xd0,0x2c,0x1e,0x8f,0xca,0x3f,0x0f,0x02,0xc1,0xaf,0xbd,0x03,0x01,0x13,0x8a,0x6b,
  0x3a,0x91,0x11,0x41,0x4f,0x67,0xdc,0xea,0x97,0xf2,0xcf,0xce,0xf0,0xb4,0xe6,0x73,
  0x96,0xac,0x74,0x22,0xe7,0xad,0x35,0x85,0xe2,0xf9,0x37,0xe8,0x1c,0x75,0xdf,0x6e,
  0x47,0xf1,0x1a,0x71,0x1d,0x29,0xc5,0x89,0x6f,0xb7,0x62,0x0e,0xaa,0x18,0xbe,0x1b,
  0xfc,0x56,0x3e,0x4b,0xc6,0xd2,0x79,0x20,0x9a,0xdb,0xc0,0xfe,0x78,0xcd,0x5a,0xf4,
  0x1f,0xdd,0xa8,0x33,0x88,0x07,0xc7,0x31,0xb1,0x12,0x10,0x59,0x27,0x80,0xec,0x5f,
  0x60,0x51,0x7f,0xa9,0x19,0xb5,0x4a,0x0d,0x2d,0xe5,0x7a,0x9f,0x93,0xc9,0x9c,0xef,
  0xa0,0xe0,0x3b,0x4d,0xae,0x2a,0xf5,0xb0,0xc8,0xeb,0xbb,0x3c,0x83,0x53,0x99,0x61,
  0x17,0x2b,0x04,0x7e,0xba,0x77,0xd6,0x26,0xe1,0x69,0x14,0x63,0x55,0x21,0x0c,0x7d
};

static const uint8_t Rcon[11] = {
  0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36 };



// Nr=num_rounds=10;  Nk=4 (4 32-bit words = 128-bits)
void
dsk_aes128_encryptor_init(DskAES128Encryptor   *s,
                          const uint8_t        *key)
{
  unsigned i;

  // The first round key is the key itself.
  memcpy (s->w, key, 16);

  // 'i' is in 4-byte units, same as in Figure 11.
  for (i = 4; i < 4 * 11; i++)
    {
      uint8_t tmp[4];
      //temp = w[i-1] ... we inline the extraction of temp
      if (i % 4 == 0)
        {
          //temp = SubWord(RotWord(out+4*(i-1)) xor Rcon(i/Nk)

          // rotted := RotWord(out + 4 * (i-1))
          const uint8_t rotted[4] = { s->w[4*(i-1)+1],
                                      s->w[4*(i-1)+2],
                                      s->w[4*(i-1)+3],
                                      s->w[4*(i-1)+0] };
          tmp[0] = subbytes_table[rotted[0]] ^ Rcon[i/4];
          tmp[1] = subbytes_table[rotted[1]];
          tmp[2] = subbytes_table[rotted[2]];
          tmp[3] = subbytes_table[rotted[3]];
        }
      else
        {
          memcpy (tmp, s->w + 4 * (i-1), 4);
        }
      XOR4(s->w + i * 4, s->w + (i - 4) * 4, tmp);

    }
}

#if DSK_ASM_MODE != DSK_ASM_AMD64
void
dsk_aes128_encrypt_inplace(const DskAES128Encryptor *s,
                           uint8_t         *in_out)      /* length 16 */
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
#endif

#define INVSHIFTROWS(state) \
    do{ uint8_t tmp; \
        tmp = state[1]; \
        state[1] = state[13]; \
        state[13] = state[9]; \
        state[9] = state[5]; \
        state[5] = tmp; \
        tmp = state[2]; \
        state[2] = state[10]; \
        state[10] = tmp; \
        tmp = state[6]; \
        state[6] = state[14]; \
        state[14] = tmp; \
        tmp = state[3]; \
        state[3] = state[7]; \
        state[7] = state[11]; \
        state[11] = state[15]; \
        state[15] = tmp; \
         }while(0)
#define INVSUBBYTES(state) \
    do{ for (unsigned i = 0; i < 16; i++) \
          state[i] = inv_subbytes_table[state[i]]; }while(0)

#if 0
static inline uint8_t multiply(uint8_t x, uint8_t y)
{
  uint8_t rv = ((y & 1) ? x : 0);
  x = xtime(x);
  if (y & 2) rv ^= x;
  x = xtime(x);
  if (y & 4) rv ^= x;
  x = xtime(x);
  if (y & 8) rv ^= x;
  x = xtime(x);
  if (y & 16) rv ^= x;
  return rv;
}
static void InvMixColumns(state_t* state)
{
  int i;
  uint8_t a, b, c, d;
  for (i = 0; i < 4; ++i)
  {
    a = (*state)[i][0];
    b = (*state)[i][1];
    c = (*state)[i][2];
    d = (*state)[i][3];

    (*state)[i][0] = Multiply(a, 0x0e) ^ Multiply(b, 0x0b) ^ Multiply(c, 0x0d) ^ Multiply(d, 0x09);
    (*state)[i][1] = Multiply(a, 0x09) ^ Multiply(b, 0x0e) ^ Multiply(c, 0x0b) ^ Multiply(d, 0x0d);
    (*state)[i][2] = Multiply(a, 0x0d) ^ Multiply(b, 0x09) ^ Multiply(c, 0x0e) ^ Multiply(d, 0x0b);
    (*state)[i][3] = Multiply(a, 0x0b) ^ Multiply(b, 0x0d) ^ Multiply(c, 0x09) ^ Multiply(d, 0x0e);
  }
}


#endif

#define INVMIXCOLUMNS_1(state)                     \
do{                                                \
                                                   \
 uint8_t a = (state)[0];                           \
 uint8_t out0 = 0, out1 = a, out2 = a, out3 = a;   \
 XTIME(a);                                         \
 out0 ^= a; out3 ^= a;                             \
 XTIME(a);                                         \
 out0 ^= a; out2 ^= a;                             \
 XTIME(a);                                         \
 out0 ^= a; out1 ^= a; out2 ^= a; out3 ^= a;       \
                                                   \
 uint8_t b = (state)[1];                           \
 out0 ^= b; out2 ^= b; out3 ^= b;                  \
 XTIME(b);                                         \
 out0 ^= b; out1 ^= b;                             \
 XTIME(b);                                         \
 out1 ^= b; out3 ^= b;                             \
 XTIME(b);                                         \
 out0 ^= b; out1 ^= b; out2 ^= b; out3 ^= b;       \
                                                   \
 uint8_t c = (state)[2];                           \
 out0 ^= c; out1 ^= c; out3 ^= c;                  \
 XTIME(c);                                         \
 out1 ^= c; out2 ^= c;                             \
 XTIME(c);                                         \
 out0 ^= c; out2 ^= c;                             \
 XTIME(c);                                         \
 out0 ^= c; out1 ^= c; out2 ^= c; out3 ^= c;       \
                                                   \
 uint8_t d = (state)[3];                           \
 out0 ^= d; out1 ^= d; out2 ^= d;                  \
 XTIME(d);                                         \
 out2 ^= d; out3 ^= d;                             \
 XTIME(d);                                         \
 out1 ^= d; out3 ^= d;                             \
 XTIME(d);                                         \
 out0 ^= d; out1 ^= d; out2 ^= d; out3 ^= d;       \
                                                   \
 (state)[0] = out0;                                \
 (state)[1] = out1;                                \
 (state)[2] = out2;                                \
 (state)[3] = out3;                                \
}while(0)

#define INVMIXCOLUMNS(state)                       \
  do{                                              \
    INVMIXCOLUMNS_1(state + 0);                    \
    INVMIXCOLUMNS_1(state + 4);                    \
    INVMIXCOLUMNS_1(state + 8);                    \
    INVMIXCOLUMNS_1(state + 12);                   \
  }while(0)

void
dsk_aes128_decryptor_init(DskAES128Decryptor   *s,
                          const uint8_t        *key)
{
  DskAES128Encryptor tmp;
  dsk_aes128_encryptor_init (&tmp, key);
  memcpy (s->w, tmp.w + 16*10, 16);
  memcpy (s->w + 16 * 10, tmp.w, 16);
  for (unsigned i = 1; i <= 9; i++)
    {
      uint8_t buf[16];
      memcpy (buf, tmp.w + 16*i, 16);
      INVMIXCOLUMNS(buf);
      memcpy (s->w + 16 * (10-i), buf, 16);
    }
}

#if DSK_ASM_MODE != DSK_ASM_AMD64
void
dsk_aes128_decrypt_inplace(const DskAES128Decryptor     *s,
                           uint8_t              *in_out)   /* length 16 */
{
  ADDROUNDKEY(in_out, s->w, 0);
  for (unsigned round = 1; round <= 9; round++)
    {
      INVSHIFTROWS(in_out);
      INVSUBBYTES(in_out);
      INVMIXCOLUMNS(in_out);
      ADDROUNDKEY(in_out, s->w, round);
    }
  INVSHIFTROWS(in_out);
  INVSUBBYTES(in_out);
  ADDROUNDKEY(in_out, s->w, 10);
}
#endif

// Nr=num_rounds=12;  Nk=6 (6 32-bit words = 192-bits)
void
dsk_aes192_encryptor_init(DskAES192Encryptor          *s,
                const uint8_t        *key)
{
  unsigned i;

  // The first round key is the key itself.
  memcpy (s->w, key, 24);

  // 'i' is in 4-byte units, same as in Figure 11.
  for (i = 6; i < 4 * 13; i++)
    {
      uint8_t tmp[4];
      //temp = w[i-1] ... we inline the extraction of temp
      if (i % 6 == 0)
        {
          //temp = SubWord(RotWord(out+4*(i-1)) xor Rcon(i/Nk)

          // rotted := RotWord(out + 4 * (i-1))
          const uint8_t rotted[4] = { s->w[4*(i-1)+1],
                                      s->w[4*(i-1)+2],
                                      s->w[4*(i-1)+3],
                                      s->w[4*(i-1)+0] };
          tmp[0] = subbytes_table[rotted[0]] ^ Rcon[i/6];
          tmp[1] = subbytes_table[rotted[1]];
          tmp[2] = subbytes_table[rotted[2]];
          tmp[3] = subbytes_table[rotted[3]];
        }
      else
        {
          memcpy (tmp, s->w + 4 * (i-1), 4);
        }
      XOR4(s->w + i * 4, s->w + (i - 6) * 4, tmp);

    }
}

#if DSK_ASM_MODE != DSK_ASM_AMD64
void
dsk_aes192_encrypt_inplace(const DskAES192 *s,
                           uint8_t         *in_out)      /* length 16 */
{
  ADDROUNDKEY(in_out, s->w, 0);
  for (unsigned round = 1; round < 12; round++)
    {
      SUBBYTES(in_out);
      SHIFTROWS(in_out);
      MIXCOLUMNS(in_out);
      ADDROUNDKEY(in_out, s->w, round);
    }
  SUBBYTES(in_out);
  SHIFTROWS(in_out);
  ADDROUNDKEY(in_out, s->w, 12);
}
#endif

void
dsk_aes192_decryptor_init(DskAES192Decryptor   *s,
                          const uint8_t        *key)
{
  DskAES192Encryptor tmp;
  dsk_aes192_encryptor_init (&tmp, key);
  memcpy (s->w, tmp.w + 16*12, 16);
  memcpy (s->w + 16 * 12, tmp.w, 16);
  for (unsigned i = 1; i <= 11; i++)
    {
      uint8_t buf[16];
      memcpy (buf, tmp.w + 16*i, 16);
      INVMIXCOLUMNS(buf);
      memcpy (s->w + 16 * (12-i), buf, 16);
    }
}

#if DSK_ASM_MODE != DSK_ASM_AMD64
void
dsk_aes192_decrypt_inplace(const DskAES192Decryptor *s,
                           uint8_t         *in_out)      /* length 16 */
{
  ADDROUNDKEY(in_out, s->w, 0);
  for (unsigned round = 1; round <= 11; round++)
    {
      INVSHIFTROWS(in_out);
      INVSUBBYTES(in_out);
      INVMIXCOLUMNS(in_out);
      ADDROUNDKEY(in_out, s->w, round);
    }
  INVSHIFTROWS(in_out);
  INVSUBBYTES(in_out);
  ADDROUNDKEY(in_out, s->w, 12);
}
#endif

// Nr=num_rounds=14;  Nk=8 (8 32-bit words = 256-bits)
void
dsk_aes256_encryptor_init(DskAES256Encryptor *s,
                const uint8_t        *key)
{
  unsigned i;

  // The first round key is the key itself.
  memcpy (s->w, key, 32);

  // 'i' is in 4-byte units, same as in Figure 11.
  for (i = 8; i < 4 * 15; i++)
    {
      uint8_t tmp[4];
      //temp = w[i-1] ... we inline the extraction of temp
      if (i % 8 == 0)
        {
          //temp = SubWord(RotWord(out+4*(i-1)) xor Rcon(i/Nk)

          // rotted := RotWord(out + 4 * (i-1))
          const uint8_t rotted[4] = { s->w[4*(i-1)+1],
                                      s->w[4*(i-1)+2],
                                      s->w[4*(i-1)+3],
                                      s->w[4*(i-1)+0] };
          tmp[0] = subbytes_table[rotted[0]] ^ Rcon[i/8];
          tmp[1] = subbytes_table[rotted[1]];
          tmp[2] = subbytes_table[rotted[2]];
          tmp[3] = subbytes_table[rotted[3]];
        }
      else if (i % 8 == 4)
        {
          tmp[0] = subbytes_table[s->w[4*(i-1)+0]];
          tmp[1] = subbytes_table[s->w[4*(i-1)+1]];
          tmp[2] = subbytes_table[s->w[4*(i-1)+2]];
          tmp[3] = subbytes_table[s->w[4*(i-1)+3]];
        }
      else
        {
          memcpy (tmp, s->w + 4 * (i-1), 4);
        }
      XOR4(s->w + i * 4, s->w + (i - 8) * 4, tmp);

    }
}

void
dsk_aes256_encrypt_inplace(const DskAES256Encryptor *s,
                           uint8_t         *in_out)      /* length 16 */
{
  ADDROUNDKEY(in_out, s->w, 0);
  for (unsigned round = 1; round < 14; round++)
    {
      SUBBYTES(in_out);
      SHIFTROWS(in_out);
      MIXCOLUMNS(in_out);
      ADDROUNDKEY(in_out, s->w, round);
    }
  SUBBYTES(in_out);
  SHIFTROWS(in_out);
  ADDROUNDKEY(in_out, s->w, 14);
}

void
dsk_aes256_decryptor_init(DskAES256Decryptor   *s,
                          const uint8_t        *key)
{
  DskAES256Encryptor tmp;
  dsk_aes256_encryptor_init (&tmp, key);
  memcpy (s->w, tmp.w + 16*14, 16);
  memcpy (s->w + 16 * 14, tmp.w, 16);
  for (unsigned i = 1; i <= 13; i++)
    {
      uint8_t buf[16];
      memcpy (buf, tmp.w + 16*i, 16);
      INVMIXCOLUMNS(buf);
      memcpy (s->w + 16 * (14-i), buf, 16);
    }
}
void
dsk_aes256_decrypt_inplace(const DskAES256Decryptor *s,
                           uint8_t         *in_out)      /* length 16 */
{
  ADDROUNDKEY(in_out, s->w, 0);
  for (unsigned round = 1; round <= 13; round++)
    {
      INVSHIFTROWS(in_out);
      INVSUBBYTES(in_out);
      INVMIXCOLUMNS(in_out);
      ADDROUNDKEY(in_out, s->w, round);
    }
  INVSHIFTROWS(in_out);
  INVSUBBYTES(in_out);
  ADDROUNDKEY(in_out, s->w, 14);
}

