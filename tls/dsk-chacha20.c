/*
 * RFC 8439.
 *
 * Stream cipher with authentication from Daniel Bernstein.
 */
#include "../dsk.h"
#include <string.h>

// NOTE: this function may not work if shift==0.
//
// However, it is only used by quarterround,
// which always uses a nonzero constant.
static inline uint32_t
rotate_left32(uint32_t w, unsigned shift)
{
  return (w << shift) | (w >> (32 - shift));
}

static inline void
quarterround (uint32_t *state, unsigned a, unsigned b, unsigned c, unsigned d)
{
  state[a] += state[b];
  state[d] ^= state[a];
  state[d] = rotate_left32(state[d], 16);

  state[c] += state[d];
  state[b] ^= state[c];
  state[b] = rotate_left32(state[b], 12);

  state[a] += state[b];
  state[d] ^= state[a];
  state[d] = rotate_left32(state[d], 8);

  state[c] += state[d];
  state[b] ^= state[c];
  state[b] = rotate_left32(state[b], 7);
}
static void
inner_block (uint32_t *state)
{
   quarterround (state, 0, 4, 8,12);
   quarterround (state, 1, 5, 9,13);
   quarterround (state, 2, 6,10,14);
   quarterround (state, 3, 7,11,15);
   quarterround (state, 0, 5,10,15);
   quarterround (state, 1, 6,11,12);
   quarterround (state, 2, 7, 8,13);
   quarterround (state, 3, 4, 9,14);
}

static void
serialize_state(uint32_t *state,
                uint8_t  *out)
{
#if DSK_IS_LITTLE_ENDIAN
  memcpy (out, state, 64);
#else
  for (unsigned i = 0; i < 16; i++)
    dsk_uint32le_pack (state[i], out + 4*i);
#endif
}

void
dsk_chacha20_block_128 (const uint32_t *key,                  // length 4
                        uint32_t        counter,
                        const uint32_t *nonce,                // length 3
                        uint8_t        *out                   // length 64
                       )
{
  uint32_t state[16] = {
    0x61707865, 0x3120646e, 0x79622d36, 0x6b206574,
    key[0],     key[1],     key[2],     key[3],
    key[0],     key[1],     key[2],     key[3],
    counter,    nonce[0],   nonce[1],   nonce[2]
  };
  uint32_t working_state[16];
  memcpy(working_state, state, sizeof(working_state));
  inner_block(working_state);           // round 1
  inner_block(working_state);           // round 2
  inner_block(working_state);           // round 3
  inner_block(working_state);           // round 4
  inner_block(working_state);           // round 5
  inner_block(working_state);           // round 6
  inner_block(working_state);           // round 7
  inner_block(working_state);           // round 8
  inner_block(working_state);           // round 9
  inner_block(working_state);           // round 10
  for (unsigned i = 0; i < 16; i++)
    state[i] += working_state[i];
  serialize_state(state, out);
}

void
dsk_chacha20_block_256 (const uint32_t *key,                  // length 8
                        uint32_t        counter,
                        const uint32_t *nonce,                // length 3
                        uint8_t        *out                   // length 64
                       )
{
  uint32_t state[16] = {
    0x61707865, 0x3320646e, 0x79622d32, 0x6b206574,
    key[0],     key[1],     key[2],     key[3],
    key[4],     key[5],     key[6],     key[7],
    counter,    nonce[0],   nonce[1],   nonce[2]
  };
  uint32_t working_state[16];
  memcpy(working_state, state, sizeof(working_state));
  inner_block(working_state);           // round 1
  inner_block(working_state);           // round 2
  inner_block(working_state);           // round 3
  inner_block(working_state);           // round 4
  inner_block(working_state);           // round 5
  inner_block(working_state);           // round 6
  inner_block(working_state);           // round 7
  inner_block(working_state);           // round 8
  inner_block(working_state);           // round 9
  inner_block(working_state);           // round 10
  for (unsigned i = 0; i < 16; i++)
    state[i] += working_state[i];

  serialize_state(state, out);
}

void
dsk_chacha20_crypt_128 (const uint32_t *key,                  // length 4
                        uint32_t        counter,
                        const uint32_t *nonce,                // length 3
                        size_t          length,
                        const uint8_t  *in,
                        uint8_t        *out)
{
  uint8_t block[64];
  for (unsigned j = 0; j < length / 64; j++)
    {
      dsk_chacha20_block_128 (key, counter+j, nonce, block);
      for (unsigned k = 0; k < 64; k++)
        *out++ = *in++ ^ block[k];
    }
  if (length % 64)
    {
      unsigned j = length / 64;
      dsk_chacha20_block_128 (key, counter+j, nonce, block);
      unsigned rem = length - j * 64;
      for (unsigned k = 0; k < rem; k++)
        *out++ = *in++ ^ block[k];
    }
}
void
dsk_chacha20_crypt_256 (const uint32_t *key,                  // length 8
                        uint32_t        counter,
                        const uint32_t *nonce,                // length 3
                        size_t          length,
                        const uint8_t  *in,
                        uint8_t        *out)
{
  uint8_t block[64];
  for (unsigned j = 0; j < length / 64; j++)
    {
      dsk_chacha20_block_256 (key, counter+j, nonce, block);
      for (unsigned k = 0; k < 64; k++)
        *out++ = *in++ ^ block[k];
    }
  if (length % 64)
    {
      unsigned j = length / 64;
      dsk_chacha20_block_256 (key, counter+j, nonce, block);
      unsigned rem = length - j * 64;
      for (unsigned k = 0; k < rem; k++)
        *out++ = *in++ ^ block[k];
    }
}

// P = (1<<130) - 5
static uint32_t P[5] = {
  0xfffffffb,
  0xffffffff,
  0xffffffff,
  0xffffffff,
  0x00000003
};
static uint32_t P_barrett_mu[6] = {
  0x00000000,
  0x50000000,
  0x00000000,
  0x00000000,
  0x00000000,
  0x40000000,
};

// "n" includes the added "1" in the byte after the last data byte.
// that's why it is of length 5: because for full blocks the last
// byte is in the next word.  Therefore n[4]==1 for a full block,
// and n[4]==0 for a partial block.
static inline void
poly1305_handle_n (uint32_t *a,    // length 5
                   uint32_t *r,    // clamped key (length 4)
                   uint32_t *n)    // length 5
{
  a[4] += dsk_tls_bignum_add_with_carry (4, a, n, 0, a);
  a[4] += n[4];
  uint32_t tmp[9];
  dsk_tls_bignum_multiply (5, a, 4, r, tmp);

  assert(tmp[8] == 0);

  // XXX: do we need to verify the tmp <= P<<128 ?
  dsk_tls_bignum_modulus_with_barrett_mu (8, tmp, 5, P, P_barrett_mu, a);
}

// Portable version.  Probably want to optimize this?
void
dsk_poly1305_mac     (const uint32_t *key,                    // length 8
                      size_t          message_length,
                      const uint8_t  *message,
                      uint8_t        *mac_out                 // length 16
                     )
{
  uint32_t r[4], s[4], a[5];
  r[0] = key[0] & 0x0fffffff;
  r[1] = key[1] & 0x0ffffffc;
  r[2] = key[2] & 0x0ffffffc;
  r[3] = key[3] & 0x0ffffffc;
  s[0] = key[4];
  s[1] = key[5];
  s[2] = key[6];
  s[3] = key[7];
  a[0] = a[1] = a[2] = a[3] = a[4] = 0;
  unsigned full_blocks = message_length / 16;
  for (unsigned i = 0; i < full_blocks; i++)
    {
      uint32_t n[5] = {
        dsk_uint32le_parse (message + i * 16 + 0),
        dsk_uint32le_parse (message + i * 16 + 4),
        dsk_uint32le_parse (message + i * 16 + 8),
        dsk_uint32le_parse (message + i * 16 + 12),
        1
      };
      poly1305_handle_n(a, r, n);
    }
  if (message_length % 16 != 0)
    {
      union {
        uint32_t n[5];
        uint8_t buf[16];
      } u;
      unsigned rem = message_length & 0xf;
      memcpy(u.buf, message + message_length - rem, rem);
      u.buf[rem] = 1;
      memset(u.buf + rem + 1, 0, 16 - rem - 1);
#if DSK_IS_BIG_ENDIAN
      // TODO: use bswap
      u.n[0] = dsk_uint32le_parse (u.buf+0);
      u.n[1] = dsk_uint32le_parse (u.buf+4);
      u.n[2] = dsk_uint32le_parse (u.buf+8);
      u.n[3] = dsk_uint32le_parse (u.buf+12);
#endif
      u.n[4] = 0;
      poly1305_handle_n(a, r, u.n);
    }
  dsk_tls_bignum_add (4, a, s, a);
  dsk_uint32le_pack (a[0], mac_out + 0);
  dsk_uint32le_pack (a[1], mac_out + 4);
  dsk_uint32le_pack (a[2], mac_out + 8);
  dsk_uint32le_pack (a[3], mac_out + 12);
}

typedef struct DskPoly1305Context DskPoly1305Context;
struct DskPoly1305Context
{
  uint32_t r[4], s[4], a[5];
};

static inline void
dsk_poly1305_context_init (DskPoly1305Context *ctx,
                           const uint32_t     *key)
{
  ctx->r[0] = key[0] & 0x0fffffff;
  ctx->r[1] = key[1] & 0x0ffffffc;
  ctx->r[2] = key[2] & 0x0ffffffc;
  ctx->r[3] = key[3] & 0x0ffffffc;
  ctx->s[0] = key[4];
  ctx->s[1] = key[5];
  ctx->s[2] = key[6];
  ctx->s[3] = key[7];
  ctx->a[0] = ctx->a[1] = ctx->a[2] = ctx->a[3] = ctx->a[4] = 0;
}

static inline void
dsk_poly1305_context_feed_block (DskPoly1305Context *ctx,
                                 const uint8_t      *input)
{
  uint32_t n[5] = {
    dsk_uint32le_parse (input + 0),
    dsk_uint32le_parse (input + 4),
    dsk_uint32le_parse (input + 8),
    dsk_uint32le_parse (input + 12),
    1
  };
  poly1305_handle_n(ctx->a, ctx->r, n);
}
static void
dsk_poly1305_context_feed_padded (DskPoly1305Context *ctx,
                                  size_t              input_len,
                                  const uint8_t      *input)
{
  for (unsigned i = 0; i + 16 <= input_len; i += 16)
    dsk_poly1305_context_feed_block(ctx, input + i);
  if (input_len & 0xf)
    {
      uint8_t tmp[16];
      unsigned rem = input_len & 0xf;
      memcpy (tmp, input + input_len - rem, rem);
      memset (tmp + rem, 0, 16 - rem);
      dsk_poly1305_context_feed_block (ctx, tmp);
    }
}
static void
dsk_poly1305_context_feed_2lengths (DskPoly1305Context *ctx,
                                    size_t              len1,
                                    size_t              len2)
{
#if DSK_SIZEOF_SIZE_T == 4
  uint32_t n[5] = {
    len1,
    0,
    len2,
    0,
    1
  };
#else
  uint32_t n[5] = {
    len1,
    len1 >> 32,
    len2,
    len2 >> 32,
    1
  };
#endif
  poly1305_handle_n(ctx->a, ctx->r, n);
}

static void
dsk_poly1305_context_finish (DskPoly1305Context *ctx,
                             uint8_t            *mac_out)
{
  dsk_tls_bignum_add (4, ctx->a, ctx->s, ctx->a);
  dsk_uint32le_pack (ctx->a[0], mac_out + 0);
  dsk_uint32le_pack (ctx->a[1], mac_out + 4);
  dsk_uint32le_pack (ctx->a[2], mac_out + 8);
  dsk_uint32le_pack (ctx->a[3], mac_out + 12);
}

void
dsk_poly1305_key_gen (const uint32_t *key,                    // length 8
                      const uint32_t *nonce,                  // length 3
                      uint32_t       *poly1305_key_out        // length 4
                     )
{
  uint8_t block_out[64];
  dsk_chacha20_block_256 (key, 0, nonce, block_out);
#if DSK_IS_LITTLE_ENDIAN
  memcpy (poly1305_key_out, block_out, 32);
#else
  poly1305_key_out[0] = dsk_uint32le_parse (block_out + 0);
  poly1305_key_out[1] = dsk_uint32le_parse (block_out + 4);
  poly1305_key_out[2] = dsk_uint32le_parse (block_out + 8);
  poly1305_key_out[3] = dsk_uint32le_parse (block_out + 12);
#endif
}

void dsk_aead_chacha20_poly1305_encrypt (const uint32_t            *key,
                                         size_t                     plaintext_len,
                                         const uint8_t             *plaintext,
                                         size_t                     associated_data_len,
                                         const uint8_t             *associated_data,
                                         const uint32_t            *iv,
                                         uint8_t                   *ciphertext,
                                         uint8_t                   *authentication_tag)
{
  dsk_chacha20_crypt_256 (key, 1, iv, plaintext_len, plaintext, ciphertext);
  
  uint32_t one_time_key[8];
  DskPoly1305Context poly1305_ctx;
  dsk_poly1305_key_gen (key, iv, one_time_key);
  dsk_poly1305_context_init (&poly1305_ctx, one_time_key);
  dsk_poly1305_context_feed_padded (&poly1305_ctx, associated_data_len, associated_data);
  dsk_poly1305_context_feed_padded (&poly1305_ctx, plaintext_len, ciphertext);
  dsk_poly1305_context_feed_2lengths (&poly1305_ctx, associated_data_len, plaintext_len);
  dsk_poly1305_context_finish (&poly1305_ctx, authentication_tag);
}

bool dsk_aead_chacha20_poly1305_decrypt (const uint32_t            *key,
                                         size_t                     ciphertext_len,
                                         const uint8_t             *ciphertext,
                                         size_t                     associated_data_len,
                                         const uint8_t             *associated_data,
                                         const uint32_t             *iv,
                                         uint8_t                   *plaintext,
                                         const uint8_t             *authentication_tag)
{
  uint32_t one_time_key[8];
  dsk_poly1305_key_gen (key, iv, one_time_key);
  DskPoly1305Context poly1305_ctx;
  dsk_poly1305_context_init (&poly1305_ctx, one_time_key);
  dsk_poly1305_context_feed_padded (&poly1305_ctx, associated_data_len, associated_data);
  dsk_poly1305_context_feed_padded (&poly1305_ctx, ciphertext_len, ciphertext);
  dsk_poly1305_context_feed_2lengths (&poly1305_ctx, associated_data_len, ciphertext_len);
  uint8_t tmp_authentication_tag[16];
  dsk_poly1305_context_finish (&poly1305_ctx, tmp_authentication_tag);

  // TODO: replace with constant-time (ie data-independent) memcmp()
  if (memcmp (tmp_authentication_tag, authentication_tag, 16) != 0)
    return false;

  dsk_chacha20_crypt_256 (key, 1, iv, ciphertext_len, ciphertext, plaintext);
  return true;
}

