#include "../dsk.h"

#include "../dsk.h"
#include <string.h>
#include <stdio.h>

static dsk_boolean cmdline_verbose = DSK_FALSE;

#define TRAILER_5       "\1\2\3\4\5"

static void
test_integer (void)
{
  const uint8_t t1[] = "\2\1\x2a" TRAILER_5;
  DskASN1Value *v;
  size_t used;
  DskError *err = NULL;
  DskMemPool pool;
  
  dsk_mem_pool_init (&pool);
  v = dsk_asn1_value_parse_der (sizeof(t1), t1, &used, &pool, &err);
  assert(v != NULL);
  assert(v->type == DSK_ASN1_TYPE_INTEGER);
  assert(v->v_integer == 42);
  assert(used == 3);
  assert(used == sizeof(t1) - 6);
  dsk_mem_pool_clear (&pool);

  const uint8_t t2[] = "\2\2\x03\xe8" TRAILER_5;
  
  dsk_mem_pool_init (&pool);
  v = dsk_asn1_value_parse_der (sizeof(t2), t2, &used, &pool, &err);
  assert(v != NULL);
  assert(v->type == DSK_ASN1_TYPE_INTEGER);
  assert(v->v_integer == 1000);
  assert(used == 4);
  assert(used == sizeof(t2) - 6);
  dsk_mem_pool_clear (&pool);

}

static void
test_boolean (void)
{
  const uint8_t t1[] = "\1\1\x00" TRAILER_5;
  DskASN1Value *v;
  size_t used;
  DskError *err = NULL;
  DskMemPool pool;
  
  dsk_mem_pool_init (&pool);
  v = dsk_asn1_value_parse_der (sizeof(t1), t1, &used, &pool, &err);
  assert(v != NULL);
  assert(v->type == DSK_ASN1_TYPE_BOOLEAN);
  assert(v->v_boolean == false);
  assert(used == sizeof(t1) - 6);
  dsk_mem_pool_clear (&pool);

  const uint8_t t2[] = "\1\1\xff" TRAILER_5;

  dsk_mem_pool_init (&pool);
  v = dsk_asn1_value_parse_der (sizeof(t2), t2, &used, &pool, &err);
  assert(v != NULL);
  assert(v->type == DSK_ASN1_TYPE_BOOLEAN);
  assert(v->v_boolean == true);
  assert(used == sizeof(t2) - 6);
  dsk_mem_pool_clear (&pool);
}

static void
test_bitstring (void)
{
  const uint8_t t1[] = "\3\7\x04\x0A\x3B\x5F\x29\x1C\xD0" TRAILER_5;
  DskASN1Value *v;
  size_t used;
  DskError *err = NULL;
  DskMemPool pool;
  
  dsk_mem_pool_init (&pool);
  v = dsk_asn1_value_parse_der (sizeof(t1), t1, &used, &pool, &err);
  assert(v != NULL);
  assert(v->type == DSK_ASN1_TYPE_BIT_STRING);
  assert(v->v_bit_string.length == 44);
  assert(memcmp(v->v_bit_string.bits, "\x0A\x3B\x5F\x29\x1C\xD0", 6) == 0);
  assert(used == sizeof(t1) - 6);
  dsk_mem_pool_clear (&pool);
}


static struct 
{
  const char *name;
  void (*test)(void);
} tests[] =
{
  { "ASN1 integer handling", test_integer },
  { "ASN1 boolean handling", test_boolean },
  { "ASN1 bitstring handling", test_bitstring },
};

int main(int argc, char **argv)
{
  dsk_cmdline_init ("test ASN.1 handling",
                    "Test ASN.1 parsing",
                    NULL, 0);
  dsk_cmdline_add_boolean ("verbose", "extra logging", NULL, 0,
                           &cmdline_verbose);
  dsk_cmdline_process_args (&argc, &argv);

  for (unsigned i = 0; i < DSK_N_ELEMENTS (tests); i++)
    {
      fprintf (stderr, "Test: %s... ", tests[i].name);
      tests[i].test ();
      fprintf (stderr, " done.\n");
    }

  dsk_cleanup ();
  
  return 0;
}
