#include "../dsk.h"

static unsigned get_n_leading_zeros (DskASN1Value *v)
{
  unsigned n_leading_zeros;
  unsigned L = v->value_end - v->value_start;
  for (n_leading_zeros = 0; n_leading_zeros < 8 * L; n_leading_zeros++)
    if (value_start[(L - 1) - (n_leading_zeros / 8)] & (1<<(7-n_leading_zeros%8)))
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
  DskASN1Value **children = value->v_sequence.children[i];
  for (unsigned i = 0; i < 9; i++)
    n_words += integer_word_count(children[i]);
  
  DskRSAPrivateKey *rv = dsk_malloc (sizeof (DskRSAPrivateKey) + n_words * 4);
  uint32_t *at = (uint32_t*) (rv + 1);

  rv->version = value->v_sequence.children[0].v_integer;
  rv->signature_length = integer_byte_count (value->v_sequence.children[1]);
  rv->modulus = at;
  at += rv->modulus_len = pack_bigint (at, value->v_sequence.children[1]);
  rv->public_exp = at;
  at += rv->public_exp_len = pack_bigint (at, value->v_sequence.children[2]);
  rv->private_exp = at;
  at += rv->private_exp_len = pack_bigint (at, value->v_sequence.children[3]);
  rv->p = at;
  at += rv->p_len = pack_bigint (at, value->v_sequence.children[4]);
  rv->q = at;
  at += rv->q_len = pack_bigint (at, value->v_sequence.children[5]);
  rv->exp1 = at;
  at += rv->exp1_len = pack_bigint (at, value->v_sequence.children[6]);
  rv->exp2 = at;
  at += rv->exp2_len = pack_bigint (at, value->v_sequence.children[7]);
  rv->coefficient = at;
  at += rv->coefficient_len = pack_bigint (at, value->v_sequence.children[8]);
  return rv;
}

void
dsk_rsa_private_key_free (DskRSAPrivateKey *pkey)
{
  dsk_free (pkey);
}

#if 0
struct DskRSAPrivateKey
{
  unsigned version;
  unsigned signature_length;            // in bytes
  unsigned modulus_len;
  uint32_t *modulus;
  unsigned public_exp_len;
  uint32_t *public_exp;
  unsigned private_exp_len;
  uint32_t *private_exp;
  unsigned p_len;
  uint32_t *p;
  unsigned q_len;
  uint32_t *q;
  unsigned exp1_len;
  uint32_t *exp1;
  unsigned exp2_len;
  uint32_t *exp2;
  unsigned coefficient_len;
  uint32_t *coefficient;

#if 0
  unsigned n_other_primes;
  DskRSAPrivateKeyOtherPrime *other_primes;
#endif
};
#endif



/* --- EMSA-PKCS1 --- */
#define EMCS_PKCS1_ENCODED_MESSAGE_LEN_LOWER_BOUND     (19+64+3)

static inline unsigned
min_emcs_pkcs1_encoded_message_len (DskChecksumType type)
{
  switch (checksum)
    {
    case DSK_CHECKSUM_SHA1:
      return 15 + 20 + 3;

    case DSK_CHECKSUM_SHA256:
      return 19 + 32 + 3;

    case DSK_CHECKSUM_SHA384:
      return 19 + 48 + 3;
      break;

    case DSK_CHECKSUM_SHA512:
      return 19 + 64 + 3;

    default:
      return 0;
    }
}


//
// Compute Encoded Message (EM) which, despite its name,
// is more of a hash than an encoding.
//
// The formal name of this protocol is EMSA-PKCS1-v1_5
// and is described in RFC 3447 Section 9.2.
//
// Requirement: out_len >= min_emcs_pkcs1_encoded_message_len(checksum).

static bool
compute_emsa_pkcs1_v1_5_encoded_message (DskChecksumType checksum,
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
#define MAX_T_LEN      (19 + 512 / 8)
  uint8_t T[MAX_T_LEN];
  unsigned T_start_len;
  switch (checksum)
    {
    case DSK_CHECKSUM_SHA1:
      memcpy (T, "\x30\x21\x30\x09\x06\x05\x2b\x0e"
                 "\x03\x02\x1a\x05\x00\x04\x14", 15);
      T_start_len = 15;
      break;

    case DSK_CHECKSUM_SHA256:
      memcpy (T, "\x30\x31\x30\x0d\x06\x09\x60\x86"
                 "\x48\x01\x65\x03\x04\x02\x01\x05"
                 "\x00\x04\x20", 19);
      T_start_len = 19;
      break;

    case DSK_CHECKSUM_SHA384:
      memcpy (T, "\x30\x41\x30\x0d\x06\x09\x60\x86"
                 "\x48\x01\x65\x03\x04\x02\x02\x05"
                 "\x00\x04\x30", 19);
      T_start_len = 19;
      break;

    case DSK_CHECKSUM_SHA512:
      memcpy (T, "\x30\x51\x30\x0d\x06\x09\x60\x86"
                 "\x48\x01\x65\x03\x04\x02\x03\x05"
                 "\x00\x04\x40", 19);
      T_start_len = 19;
      break;

    default:
      *error = dsk_error_new ("EMSA PKCS-1 not compatible with given hash");
      return false;
    }


  data_out[0] = 0;
  data_out[1] = 1;
  unsigned ps_len = out_len - T_start_len - 3 - hash_len;
  memset (data_out + 2, 0xff, ps_len);
  data_out[2 + ps_len] = 0;
  memcpy (data_out + 3 + ps_len, T, T_start_len);

  ... finish with hash
  return true;
}



//
// From RFC 3447 Section 8.2.1.
//
void dsk_rsassa_pkcs1_sign  (DskRSAPrivateKey *key,
                             size_t            message_len,
                             const uint8_t    *message_data,
                             uint8_t          *signature_out)
{
...
}
