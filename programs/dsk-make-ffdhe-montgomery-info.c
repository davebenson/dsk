#include "../dsk.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct Num {
  unsigned len;
  uint32_t value[1];
};
struct Num *parse_hex(const char *str)
{
  unsigned len = strlen (str);
  unsigned n_words = (len + 7) / 8;
  struct Num *rv = malloc(sizeof(struct Num) + (n_words-1) * 4);
  char hexbuf[9];
  rv->len = n_words;
  for (unsigned i = 0; i < n_words - 1; i++)
    {
      memcpy(hexbuf, str + len - (8*(i+1)), 8);
      hexbuf[8] = 0;
      rv->value[i] = (uint32_t) strtoul (hexbuf, NULL, 16);
    }
  unsigned last_len = len - 8 * (n_words - 1);
  memcpy (hexbuf, str, last_len);
  hexbuf[last_len] = 0;
  rv->value[n_words - 1] = strtoul (hexbuf, NULL, 16);
  return rv;
}

static struct {
  const char *name;
  const char *p;
} ffdhe_values[] = {
  {
    "ffdhe2048",
    "FFFFFFFFFFFFFFFFADF85458A2BB4A9AAFDC5620273D3CF1"
    "D8B9C583CE2D3695A9E13641146433FBCC939DCE249B3EF9"
    "7D2FE363630C75D8F681B202AEC4617AD3DF1ED5D5FD6561"
    "2433F51F5F066ED0856365553DED1AF3B557135E7F57C935"
    "984F0C70E0E68B77E2A689DAF3EFE8721DF158A136ADE735"
    "30ACCA4F483A797ABC0AB182B324FB61D108A94BB2C8E3FB"
    "B96ADAB760D7F4681D4F42A3DE394DF4AE56EDE76372BB19"
    "0B07A7C8EE0A6D709E02FCE1CDF7E2ECC03404CD28342F61"
    "9172FE9CE98583FF8E4F1232EEF28183C3FE3B1B4C6FAD73"
    "3BB5FCBC2EC22005C58EF1837D1683B2C6F34A26C1B2EFFA"
    "886B423861285C97FFFFFFFFFFFFFFFF"
  }
};

int main()
{
  for (unsigned i = 0; i < DSK_N_ELEMENTS(ffdhe_values); i++)
    {
      struct Num *rv = parse_hex (ffdhe_values[i].p);
      DskTlsMontgomeryInfo m;
      dsk_tls_montgomery_info_init (&m, rv->len, rv->value);

      printf ("static const uint32_t tls_%s_value[] = {\n", ffdhe_values[i].name);
      for (unsigned j = 0; j < rv->len; j++)
        {
          printf ("  0x%08x,\n", m.N[j]);
        }
      printf("};\n\nstatic const uint32_t tls_%s_barrett_mu = {\n", ffdhe_values[i].name);
      for (unsigned j = 0; j < rv->len + 1; j++)
        {
          printf ("  0x%08x,\n", m.barrett_mu[j]);
        }
      printf("};\n\n");
      printf ("DskTlsMontgomeryInfo dsk_tls_%s =\n"
              "{\n"
              "  %u,\n"
              "  tls_%s_value,\n"
              "  0x%x,    /* Nprime */\n"
              "  tls_%s_barrett_mu\n"
              "};\n\n",
              ffdhe_values[i].name, rv->len,
              ffdhe_values[i].name, m.Nprime,
              ffdhe_values[i].name);
    }
  return 0;
}
