#include "../dsk.h"
#include <string.h>
#include <alloca.h>

static unsigned get_n_leading_zeros (DskASN1Value *v)
{
  unsigned n_leading_zeros;
  unsigned L = v->value_end - v->value_start;
  for (n_leading_zeros = 0; n_leading_zeros < 8 * L; n_leading_zeros++)
    if (v->value_start[(L - 1) - (n_leading_zeros / 8)] & (1<<(7-n_leading_zeros%8)))
      break;
  return n_leading_zeros;
}

static unsigned integer_word_count (DskASN1Value *v)
{
  unsigned L = v->value_end - v->value_start;
  unsigned n_leading_zeros = get_n_leading_zeros (v);
  unsigned n_bits = L * 8 - n_leading_zeros;
  return (n_bits + 31) / 32;
}

static unsigned integer_byte_count (DskASN1Value *v)
{
  unsigned L = v->value_end - v->value_start;
  unsigned n_leading_zeros = get_n_leading_zeros (v);
  unsigned n_bits = L * 8 - n_leading_zeros;
  return (n_bits + 7) / 8;
}

static unsigned pack_bigint (uint32_t *out, DskASN1Value *v)
{
  unsigned L = v->value_end - v->value_start;
  unsigned n_leading_zeros = get_n_leading_zeros (v);
  unsigned n_bits = L * 8 - n_leading_zeros;
  unsigned rv = (n_bits + 31) / 32;
  const uint8_t *in = v->value_start + L;
  unsigned in_rem = L;
  unsigned out_rem = rv;
  while (out_rem > 0 && in_rem >= 4)
    {
      in -= 4;
      *out++ = dsk_uint32be_parse (in);
      in_rem -= 4;
      out_rem--;
    }
  if (out_rem > 0 && in_rem > 0)
    {
      switch (out_rem)
        {
        case 1:
          in--;
          *out++ = *in;
          in_rem -= 1;
          out_rem--;
          break;
        case 2:
          in -= 2;
          *out++ = dsk_uint16be_parse(in);
          in_rem -= 2;
          out_rem--;
          break;
        case 3:
          in -= 3;
          *out++ = dsk_uint24be_parse(in);
          in_rem -= 3;
          out_rem--;
          break;
        }
    }
  while (out_rem > 0)
    {
      *out++ = 0;
      out_rem--;
    }
  return rv;
}

DskRSAPrivateKey *
dsk_rsa_private_key_new_from_asn1 (DskASN1Value *value,
                                   DskError    **error)
{
  if (value->type != DSK_ASN1_TYPE_SEQUENCE
   || value->v_sequence.n_children < 9
   || value->v_sequence.children[0]->type != DSK_ASN1_TYPE_INTEGER
   || value->v_sequence.children[1]->type != DSK_ASN1_TYPE_INTEGER
   || value->v_sequence.children[2]->type != DSK_ASN1_TYPE_INTEGER
   || value->v_sequence.children[3]->type != DSK_ASN1_TYPE_INTEGER
   || value->v_sequence.children[4]->type != DSK_ASN1_TYPE_INTEGER
   || value->v_sequence.children[5]->type != DSK_ASN1_TYPE_INTEGER
   || value->v_sequence.children[6]->type != DSK_ASN1_TYPE_INTEGER
   || value->v_sequence.children[7]->type != DSK_ASN1_TYPE_INTEGER
   || value->v_sequence.children[8]->type != DSK_ASN1_TYPE_INTEGER)
    {
      *error = dsk_error_new ("bad RSAPrivateKey format");
      return NULL;
    }

  unsigned n_words = 0;
  DskASN1Value **children = value->v_sequence.children;
  for (unsigned i = 0; i < 9; i++)
    n_words += integer_word_count(children[i]);
  
  DskRSAPrivateKey *rv = dsk_malloc (sizeof (DskRSAPrivateKey) + n_words * 4);
  uint32_t *at = (uint32_t*) (rv + 1);

  rv->version = children[0]->v_integer;
  rv->modulus_length_bytes = integer_byte_count (children[1]);
  rv->modulus = at;
  at += rv->modulus_len = pack_bigint (at, children[1]);
  rv->public_exp = at;
  at += rv->public_exp_len = pack_bigint (at, children[2]);
  rv->private_exp = at;
  at += rv->private_exp_len = pack_bigint (at, children[3]);
  rv->p = at;
  at += rv->p_len = pack_bigint (at, children[4]);
  rv->q = at;
  at += rv->q_len = pack_bigint (at, children[5]);
  rv->exp1 = at;
  at += rv->exp1_len = pack_bigint (at, children[6]);
  rv->exp2 = at;
  at += rv->exp2_len = pack_bigint (at, children[7]);
  rv->coefficient = at;
  at += rv->coefficient_len = pack_bigint (at, children[8]);

  // TODO: I'm pretty sure the primes must be in descending order,
  // but i need some test code to prove that!
  if (rv->p_len != rv->q_len)
    {
      *error = dsk_error_new ("RSAPrivateKey: primes must be the same size");
      dsk_free (rv);
      return NULL;
    }

#if 0
  for (unsigned i = 0; i < rv->n_other_primes; i++)
    if (rv->other_primes.ri_len != rv->p_len)
      {
        *error = dsk_error_new ("RSAPrivateKey: primes must be the same size");
        dsk_free (rv);
        return NULL;
      }
#endif

  return rv;
}

void
dsk_rsa_private_key_free (DskRSAPrivateKey *pkey)
{
  dsk_free (pkey);
}



/* --- EMSA-PKCS1 --- */
#define EMCS_PKCS1_ENCODED_MESSAGE_LEN_LOWER_BOUND     (19+64+3)

static inline unsigned
min_emcs_pkcs1_encoded_message_len (DskChecksumType *type)
{
  if (type == &dsk_checksum_type_sha1)
    return 15 + 20 + 3;

  if (type == &dsk_checksum_type_sha256)
    return 19 + 32 + 3;

  if (type == &dsk_checksum_type_sha384)
    return 19 + 48 + 3;

  if (type == &dsk_checksum_type_sha512)
    return 19 + 64 + 3;

  return 0;
}



#if 0
//
// RFC 8017 Section 5.1.2, Page 12:  This is algorithm
//
//      RSADP(K=(p,q,dP,dQ,qInv,triplets), c)
//
// Where the first argument is the quintuple-based private-key,
// and 'c' is the ciphertext.
// 
bool
dsk_rsa_private_key_decrypt (const DskRSAPrivateKey *key,
                             const uint32_t *ciphertext,
                             uint32_t *message_out)
{
  //
  // Step 2.b.i: Compute m_1 and m_2.
  //     m_1 = ciphertext^dP (mod p)
  //     m_2 = ciphertext^dQ (mod q)
  //
  unsigned factor_len = key->factor_len;
  uint32_t *m_1 = alloca (factor_len * 4);
  uint32_t *m_2 = alloca (factor_len * 4);
  dsk_tls_bignum_modular_exponent (key->modulus_len, ciphertext,
                                   key->dP_len, key->dP,
                                   key->p_len, key->p,
                                   m_1);
  dsk_tls_bignum_modular_exponent (key->modulus_len, ciphertext,
                                   key->dQ_len, key->dQ,
                                   key->q_len, key->q,
                                   m_2);
 
  // Step 2.b.ii: postponed until the final loop 2.b.v.

  // Step 2.b.iii: Let h = (m_1 - m_2) * qInv mod p.
  if (dsk_tls_bignum_subtract_with_borrow (factor_len, m_1, m_2, 0, tmp))
    dsk_tls_bignum_add (factor_len, tmp, key->p);
  dsk_tls_bignum_multiply (factor_len, tmp, qInv, bigtmp);
  dsk_tls_bignum_modulus (factor_len * 2, bigtmp, factor_len, key->p, h);

  // Step 2.b.iv: m = m_2 + q * h
  ...

  // Step 2.b.v: for moduli composed of more than two primes.
  // [I'm not sure that this is used.]
  uint32_t *R = alloca(4 * factor_len * (2 + key->n_other_primes));
  memcpy (R, key->p, 4 * factor_len);
  unsigned R_len = factor_len;
  uint32_t *R_tmp = alloca(4 * factor_len * (2 + key->n_other_primes));
  uint32_t *c_mod_ri = alloca(4 * factor_len);
  for (unsigned i = 0; i < key->n_other_primes; i++)
    {
      //
      // Step 2.b.v.1.  R := R * r_{i-1}.
      //
      const uint32_t *R_im1 = i == 0 ? key->q : key->other_primes[i-1].r;
      dsk_tls_bignum_multiply (R_len, R, factor_len, R_im1, R_tmp);
      uint32_t *R_swap_tmp = R;
      R = R_tmp;
      R_tmp = R_swap_tmp;

      //
      // Step 2.b.ii (postponed): Compute m_i = c^{d_i} mod r_i.
      //
      const uint32_t *r_i = key->other_primes[i].r;
      dsk_tls_bignum_modulus (key->modulus_len, ciphertext,
                              factor_len, r_i,
                              c_mod_ri);
      dsk_tls_bignum_modular_exponent (factor_len, c_mod_ri,
                                       key->other_primes[i].private_exp_len,
                                       key->other_primes[i].private_exp,
                                       factor_len, r_i,
                                       m_i);

      //
      // Step 2.b.v.2.  h = (m_i - m) * t_i mod r_i
      // 
      compute mi_mod_ri and m_mod_ri
      if (dsk_tls_bignum_subtract_with_borrow (factor_len,
                                               mi_mod_ri,
                                               m_mod_ri,
                                               0,
                                               mim_mod_ri))
        {
          dsk_tls_bignum_add (factor_len, mim_mod_ri, r_i, mim_mod_ri);
        }
      dsk_tls_bignum_multiply (factor_len, mim_mod_ri, factor_len, t_i,
                               tmp);
      dsk_tls_bignum_modulus (factor_len * 2, tmp, factor_len, r_i, h);

      //
      // Step 2.b.v.3.  m += R * h.
      //
      ...
    }
  memcpy (message_out, m, 4 * key->modulus_len);
  return true;
}
#endif

// RSADP(K=(n,d), c) = m
bool
dsk_rsa_private_key_decrypt (const DskRSAPrivateKey *key,
                             const uint32_t *ciphertext,
                             uint32_t *message_out)
{
  // 2.a.  Compute
  //   message_out = ciphertext^key->private_exp (mod key->modulus)
  dsk_tls_bignum_modular_exponent (key->modulus_len, ciphertext,
                                   key->private_exp_len, key->private_exp,
                                   key->modulus_len, key->modulus,
                                   message_out);
  return true;
}


// In RFC 8017 Section 5.1.1 this is routine RSAEP.
bool
dsk_rsa_public_key_encrypt  (const DskRSAPublicKey *key,
                             const uint32_t *plaintext,
                             uint32_t *ciphertext_out)
{
  // If the message representative m is not between 0 and n - 1,
  // output "message representative out of range" and stop.
  if (dsk_tls_bignum_compare (key->modulus_len,
                              plaintext,
                              key->modulus) >= 0)
    return false;

  dsk_tls_bignum_modular_exponent (key->modulus_len, plaintext,
                                   key->public_exp_len, key->public_exp,
                                   key->modulus_len, key->modulus,
                                   ciphertext_out);
  return true;
}

// Both message and signature_out are of length key->modulus_len.
void
dsk_rsa_private_key_sign (DskRSAPrivateKey *key,
                         const uint32_t         *message,
                         uint32_t               *signature_out)
{
  dsk_tls_bignum_modular_exponent (key->modulus_len, message,
                                   key->private_exp_len, key->private_exp,
                                   key->modulus_len, key->modulus,
                                   signature_out);
}

bool
dsk_rsa_public_key_verify (DskRSAPublicKey *key,
                           const uint32_t         *message,
                           const uint32_t         *sig)
{
  uint32_t *msg_out = alloca(4 * key->modulus_len);
  dsk_tls_bignum_modular_exponent (key->modulus_len, sig,
                                   key->public_exp_len, key->public_exp,
                                   key->modulus_len, key->modulus,
                                   msg_out);
  return dsk_tls_bignums_equal (key->modulus_len, msg_out, message);
}

// ---------------------------------------------------------------------
//                          PKCS1 v1.5
// ---------------------------------------------------------------------


//
// Compute Encoded Message (EM) which, despite its name,
// is more of a hash than an encoding.
//
// The formal name of this protocol is EMSA-PKCS1-v1_5
// and is described in RFC 8017 Section 9.2.
//
// This is an implementation of EMSA-PKCS1-v1_5-ENCODE(M, emLen).
//
// Requirement: out_len >= min_emcs_pkcs1_encoded_message_len(checksum).

static bool
compute_emsa_pkcs1_v1_5_encoded_message (DskChecksumType  *type,
                                         size_t            message_len,
                                         const uint8_t    *message_data,
                                         size_t            out_len,
                                         uint8_t          *data_out,
                                         DskError        **error)
{

// T is computed as these bytes, plus the hash-code itself.
/////      SHA-1:   30 21 30 09 06 05 2b 0e 03 02 1a 05 00 04 14 || H.
/////      SHA-256: 30 31 30 0d 06 09 60 86 48 01 65 03 04 02 01 05 00
/////               04 20 || H.
/////      SHA-384: 30 41 30 0d 06 09 60 86 48 01 65 03 04 02 02 05 00
/////               04 30 || H.
/////      SHA-512: 30 51 30 0d 06 09 60 86 48 01 65 03 04 02 03 05 00
/////                  04 40 || H.

  // These computations of T
  // borrow from Note 1 at RFC 3447 9.2.
#define MAX_T_START_LEN      19
  const uint8_t *T_start;
  unsigned T_start_len;
  if (type == &dsk_checksum_type_sha1)
    {
      T_start = (const uint8_t *)
              "\x30\x21\x30\x09\x06\x05\x2b\x0e"
              "\x03\x02\x1a\x05\x00\x04\x14";
      T_start_len = 15;
    }
  else if (type == &dsk_checksum_type_sha256)
    {
      T_start = (const uint8_t *)
              "\x30\x31\x30\x0d\x06\x09\x60\x86"
              "\x48\x01\x65\x03\x04\x02\x01\x05"
              "\x00\x04\x20";
      T_start_len = 19;
    }
  else if (type == &dsk_checksum_type_sha384)
    {
      T_start = (const uint8_t *)
              "\x30\x41\x30\x0d\x06\x09\x60\x86"
              "\x48\x01\x65\x03\x04\x02\x02\x05"
              "\x00\x04\x30";
      T_start_len = 19;
    }
  else if (type == &dsk_checksum_type_sha512)
    {
      T_start = (const uint8_t *)
              "\x30\x51\x30\x0d\x06\x09\x60\x86"
              "\x48\x01\x65\x03\x04\x02\x03\x05"
              "\x00\x04\x40";
      T_start_len = 19;
    }
  else 
    {
      *error = dsk_error_new ("EMSA PKCS-1 not compatible with given hash");
      return false;
    }


  // Step 5.
  //    EM = 0x00 || 0x01 || PS || 0x00 || T
  //       = 0x00 || 0x01 || PS || 0x00 || T_start || H
  data_out[0] = 0x00;
  data_out[1] = 0x01;
  unsigned T_len = T_start_len + type->hash_size;
  if (out_len < T_len + 11)                     // from step 3
    {
      *error = dsk_error_new ("Encoded Message too short");
      return false;
    }
  unsigned ps_len = out_len - T_len - 3;        // from Step 4
  memset (data_out + 2, 0xff, ps_len);
  data_out[2 + ps_len] = 0;
  memcpy (data_out + 3 + ps_len, T_start, T_start_len);
  dsk_checksum (type, message_len, message_data,
                data_out + out_len - type->hash_size);

  return true;
}

static void
big_endian_to_int (unsigned EM_size, const uint8_t *EM, uint32_t *out)
{
  EM += EM_size;
  while (EM_size >= 4)
    {
      EM -= 4;
      *out++ = dsk_uint32be_parse(EM);
      EM_size -= 4;
    }
  if (EM_size > 0)
    {
      EM -= EM_size;
      switch (EM_size)
        {
        case 1: *out++ = *EM; break;
        case 2: *out++ = dsk_uint16be_parse(EM); break;
        case 3: *out++ = dsk_uint24be_parse(EM); break;
        }
    }
}

static void
int_to_big_endian(const uint32_t *num,
                  unsigned sig_len,
                  uint8_t *signature_out)
{
  signature_out += sig_len;
  while (sig_len >= 4)
    {
      signature_out -= 4;
      dsk_uint32be_pack (*num++, signature_out);
      sig_len -= 4;
    }
  if (sig_len > 0)
    {
      signature_out -= sig_len;
      switch (sig_len)
        {
        case 1:
          *signature_out = *num;
          break;
        case 2:
          dsk_uint16be_pack (*num, signature_out);
          break;
        case 3:
          dsk_uint24be_pack (*num, signature_out);
          break;
        }
    }
}

//
// From RFC 3447 Section 8.2.1.
//
void dsk_rsassa_pkcs1_5_sign  (DskRSAPrivateKey *key,
                               DskChecksumType  *checksum_type,
                               size_t            message_length,
                               const uint8_t    *message_data,
                               uint8_t          *signature_out)
{
  DskError *error = NULL;
  unsigned EM_size = key->modulus_length_bytes;
  uint8_t *EM = alloca(EM_size);
  if (!compute_emsa_pkcs1_v1_5_encoded_message
             (checksum_type,
              message_length, message_data,
              EM_size, EM,
              &error))
    dsk_die("error making PKCS1.5 Encoded Message: %s", error->message);

  // convert EM to m
  assert ((EM_size + 3) / 4 == key->modulus_len);
  uint32_t *m = alloca((EM_size + 3) / 4 * 4);
  big_endian_to_int(EM_size, EM, m);

  // RSASP1(key, m);
  uint32_t *s = alloca ((EM_size + 3) / 4 * 4);
  dsk_rsa_private_key_sign (key, m, s);
  int_to_big_endian(s, EM_size, signature_out);
}

bool dsk_rsassa_pkcs1_5_verify(DskRSAPublicKey  *key,
                               DskChecksumType  *checksum_type,
                               size_t            message_length,
                               const uint8_t    *message_data,
                               const uint8_t    *signature)
{
  // 2.a. Compute signature from big-endian octets to a number.
  uint32_t *s = alloca (4 * key->modulus_len);
  big_endian_to_int (key->modulus_length_bytes, signature, s);

  // 2.b. m = RSAVP1((n,e), s).
  uint32_t *EM0 = alloca (key->modulus_len * 4);
  dsk_rsa_public_key_verify(key, s, EM0);

  // 3. compute EM
  uint8_t *EM1bytes = alloca (key->modulus_length_bytes);
  DskError *error = NULL;
  if (!compute_emsa_pkcs1_v1_5_encoded_message
             (checksum_type,
              message_length, message_data,
              key->modulus_length_bytes, EM1bytes,
              &error))
    {
      dsk_warning ("error making PKCS1.5 Encoded Message: %s", error->message);
      dsk_error_unref (error);
      return false;
    }
  uint32_t *EM1 = alloca(4 * key->modulus_len);
  big_endian_to_int (key->modulus_length_bytes, EM1bytes, EM1);

  return dsk_tls_bignums_equal (key->modulus_len, EM0, EM1);
}

// This is MGF1 From RFC 8017 B.2.1.
static void
mask_generation_function_1 (DskChecksumType *checksum_type,
                            unsigned         seed_len,
                            const uint8_t   *seed,
                            unsigned         mask_len,
                            uint8_t         *mask_out)
{
  unsigned hash_size = checksum_type->hash_size;
  unsigned N = (mask_len + hash_size - 1) / hash_size;
  void *hash_instance_seeded = alloca(checksum_type->instance_size);
  uint8_t *hash_tmp = alloca(hash_size);
  void *hash_instance = alloca(checksum_type->instance_size);
  checksum_type->init(hash_instance_seeded);
  checksum_type->feed(hash_instance_seeded, seed_len, seed);
  for (uint32_t counter = 0; counter < N; counter++)
    {
      memcpy(hash_instance,
             hash_instance_seeded,
             checksum_type->instance_size);
      uint8_t C[4];
      dsk_uint32be_pack (counter, C);
      checksum_type->feed(hash_instance_seeded, 4, C);
      if (mask_len < hash_size)
        {
          checksum_type->end(hash_instance, hash_tmp);
          memcpy (mask_out, hash_tmp, mask_len);
        }
      else
        {
          checksum_type->end(hash_instance, mask_out);
          mask_out += hash_size;
          mask_len -= hash_size;
        }
    }
}

#if 0
static void
read_nonzero_cryptorandom_data(unsigned len, uint8_t *out)
{
  while (len > 0)
    {
      dsk_get_cryptorandom_data (len, out);

      // Find initial non-zero bytes.
      unsigned i;
      for (i = 0; i < len; i++)
        if (out[i] == 0)
          break;

      // Setup output index; increment i,
      // which is either ==len or out[i]==0.
      unsigned o = i++;
      while (i < len)
        {
          if (DSK_UNLIKELY(out[i] != 0))
            out[o++] = out[i];
          i++;
        }
      len -= o;
      out += o;
    }
}
#endif

//
// Implementation of EMSA-PSS-ENCODE(M, emBits),
// RFC 8017 Section 9.1.1.
//
//   M      = message_data, message_len (in bytes)
//   Hash   = ChecksumType
//   MGF    = pss_mask_generator
//   sLen   = salt_len (length in bytes)
//   emBits = out_len * 8
//   EM     = data_out
//   Mhash  = message_hash
//   hLen   = size of hash == hash_size
//
static bool
compute_emsa_pss_encoded_message  (DskChecksumType *checksum_type,
                                   size_t            salt_len,
                                   DskTlsCryptoRNG  *rng,
                                   size_t            message_len,
                                   const uint8_t    *message_data,
                                   size_t            out_len,
                                   uint8_t          *data_out,
                                   DskError        **error)
{
  if (rng == NULL)
    rng = &dsk_tls_crypto_rng_default;

  DskTlsCryptoRNG_Nonzero nonzero_rng =
    DSK_TLS_CRYPTO_RNG_NONZERO_INIT(rng);

  //
  // The numbered Steps below are from RFC 8017, Section 9.1.
  //

  unsigned hash_size = checksum_type->hash_size;

  //
  // Step 1.   If the length of M is greater than the input limitation for
  //           the hash function (2^61 - 1 octets for SHA-1), output
  //           "message too long" and stop.
  //
  // NOTE: there's no limit to the message_len for an non-legacy length.

  //
  // Step 2.   Let mHash = Hash(M), an octet string of length hLen.
  //
  uint8_t *message_hash = alloca (hash_size);
  dsk_checksum (checksum_type,
                message_len, message_data,
                message_hash);

  //
  // Step 3.   If emLen < hLen + sLen + 2, output "encoding error" and stop.
  //
  if (out_len < hash_size + salt_len + 2)
    {
      *error = dsk_error_new ("encoding error");
      return false;
    }

  //
  // Step 4.   Generate a random octet string salt of length sLen; if sLen =
  //           0, then salt is the empty string.
  //
  uint8_t *salt = alloca (salt_len);
  nonzero_rng.base.read((DskTlsCryptoRNG *) &nonzero_rng, salt_len, salt);

  //
  // Steps 5 and 6.
  //    (5)   Let
  //
  //              M' = (0x)00 00 00 00 00 00 00 00 || mHash || salt;
  //
  //           M' is an octet string of length 8 + hLen + sLen with eight
  //           initial zero octets.
  //
  //    (6)    Let H = Hash(M'), an octet string of length hLen.
  //
  void *hash_instance = alloca(checksum_type->instance_size);
  checksum_type->init(hash_instance);
  checksum_type->feed(hash_instance, 8, (uint8_t *) "\0\0\0\0\0\0\0\0");
  checksum_type->feed(hash_instance, hash_size, message_hash);
  checksum_type->feed(hash_instance, salt_len, salt);
  uint8_t *H = alloca(hash_size);
  checksum_type->end(hash_instance, H);

  // Step 7.   Generate an octet string PS consisting of
  //                emLen - sLen - hLen - 2
  //           zero octets.  The length of PS may be 0.
  assert(out_len >= salt_len + hash_size + 2);
  unsigned PS_len = out_len - salt_len - hash_size - 2;

  // Step 8.   Let DB = PS || 0x01 || salt; DB is an octet string of length
  //           emLen - hLen - 1.

  // Step 9.   Let dbMask = MGF(H, emLen - hLen - 1).
  unsigned db_len = PS_len + 1 + salt_len;
  assert(db_len == out_len - hash_size - 1);
  uint8_t *db_mask = alloca(db_len);
  mask_generation_function_1 (checksum_type,
                              hash_size, H,
                              out_len - hash_size - 1,
                              db_mask);

  // Step 10.  Let maskedDB = DB \xor dbMask.
  db_mask[PS_len] ^= 1;
  for (unsigned i = 0; i < salt_len; i++)
    db_mask[PS_len+1+i] ^= salt[i];

  // Step 11.  Set the leftmost 8emLen - emBits bits of the leftmost octet
  //           in maskedDB to zero.
  // NOTE: since we only support emBits as a multiple of 8,
  //       this is always a no-op.

  // Step 12.  Let EM = maskedDB || H || 0xbc.
  memcpy(data_out, db_mask, db_len);
  memcpy(data_out + db_len, H, hash_size);
  data_out[out_len - 1] = 0xbd;

  // Step 13.  Output EM.

  return true;
}

void dsk_rsassa_pss_sign  (DskRSAPrivateKey *key,
                           DskChecksumType  *checksum_type,
                           size_t            salt_len,
                           DskRand          *rng,
                           size_t            message_length,
                           const uint8_t    *message_data,
                           uint8_t          *signature_out)
{
  DskError *error = NULL;
  unsigned EM_size = key->modulus_length_bytes;
  uint8_t *EM = alloca(EM_size);
  if (!compute_emsa_pss_encoded_message
             (checksum_type,
              salt_len,
              rng,
              message_length, message_data,
              EM_size, EM,
              &error))
    dsk_die("error making PKCS1.5 Encoded Message: %s", error->message);

  // convert EM to m
  assert ((EM_size + 3) / 4 == key->modulus_len);
  uint32_t *m = alloca((EM_size + 3) / 4 * 4);
  big_endian_to_int(EM_size, EM, m);

  // RSASP1(key, m);
  uint32_t *s = alloca(4 * key->modulus_len);
  dsk_rsa_private_key_sign (key, m, s);
  int_to_big_endian(s, key->modulus_length_bytes, signature_out);
}

bool dsk_rsassa_pss_verify(DskRSAPublicKey  *key,
                           DskChecksumType  *checksum_type,
                           size_t            salt_len,
                           size_t            message_length,
                           const uint8_t    *message_data,
                           const uint8_t    *sig)
{
  // 2.a. Compute signature from big-endian octets to a number.
  uint32_t *s = alloca (4 * key->modulus_len);
  big_endian_to_int (key->modulus_length_bytes, sig, s);

  // 2.b. m = RSAVP1((n,e), s).
  uint32_t *EM0 = alloca (key->modulus_len * 4);
  dsk_rsa_public_key_verify(key, s, EM0);

  // 3. compute EM
  uint8_t *EM1bytes = alloca (key->modulus_length_bytes);
  DskError *error = NULL;
  if (!compute_emsa_pss_encoded_message
             (checksum_type,
              salt_len,
              NULL,                     // TODO: support custom RNG
              message_length, message_data,
              key->modulus_length_bytes, EM1bytes,
              &error))
    {
      dsk_warning ("error making PSS Encoded Message: %s", error->message);
      dsk_error_unref (error);
      return false;
    }
  uint32_t *EM1 = alloca(4 * key->modulus_len);
  big_endian_to_int (key->modulus_length_bytes, EM1bytes, EM1);

  return dsk_tls_bignums_equal (key->modulus_len, EM0, EM1);
}

