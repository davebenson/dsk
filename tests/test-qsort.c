#include "../dsk.h"
#include "../dsk-qsort-macro.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_SIZE 2048

// Compare Function for stdlib qsort(), with counting.
static unsigned compare_count = 0;
static int compare_int(const void *a, const void *b) {
  int A = * (int *) a;
  int B = * (int *) b;
  compare_count += 1;
  return A<B ? -1 : A>B ? 1 : 0;
}

// Compare macro for DSK_QSORT() comparator, with counting.
#define COUNTING_COMPARATOR(a,b,rv) \
  DSK_QSORT_SIMPLE_COMPARATOR(a,b,rv); \
  compare_count_dsk++

int main(int argc, char **argv)
{
  // Per man page for srand:
  //        If no seed value is provided, the rand() function is automatically
  //        seeded with a value of 1.
  int seed = 1;

  //
  // Parse Command-line arguments.
  //
  dsk_cmdline_add_int("seed", "random seed", "SEED", 0, &seed);
  dsk_cmdline_process_args(&argc, &argv);
  srand(seed);

  //
  // Prepare random array to sort.
  //
  int *arr = DSK_NEW_ARRAY(TEST_SIZE, int);
  for (int i = 0; i < TEST_SIZE; i++)
    arr[i] = rand();

  //
  // run qsort() from stdlib, counting
  // the number of comparisons.
  //
  int *copy_stdlib = dsk_memdup(sizeof(int) * TEST_SIZE, arr);
  qsort(copy_stdlib, TEST_SIZE, sizeof(int), compare_int);

  //
  // run qsort() from dsk, counting
  // the number of comparisons.
  //
  unsigned compare_count_dsk = 0;
  int *copy_dsk = dsk_memdup(sizeof(int) * TEST_SIZE, arr);

  DSK_QSORT (copy_dsk, int, TEST_SIZE, COUNTING_COMPARATOR);

  //
  // Validate that each algorithm got the same results.
  //
  assert(memcmp (copy_stdlib, copy_dsk, sizeof(int) * TEST_SIZE) == 0);

  printf("stdlib qsort used %u compares; dsk qsort used %u compares\n",
         compare_count, compare_count_dsk);

  return 0;
}
