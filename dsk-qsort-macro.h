
#include "dsk-config.h"

typedef struct _DskQsortStackNode DskQsortStackNode;
typedef struct _DskQsortStack DskQsortStack;

/* Amount of stack to allocate: should be log2(max_array_size)+1.
   on 32-bit, this uses 33*8=264 bytes;
   on 64-bit it uses 65*16=1040 bytes. */
#if (DSK_SIZEOF_SIZE_T == 8)
#define DSK_QSORT_STACK_MAX_SIZE  (65)
#elif (DSK_SIZEOF_SIZE_T == 4)
#define DSK_QSORT_STACK_MAX_SIZE  (33)
#else
#error "sizeof(size_t) is neither 4 nor 8: need DSK_QSORT_STACK_MAX_SIZE def"
#endif

/* Maximum number of elements to sort with insertion sort instead of qsort */
#define DSK_INSERTION_SORT_THRESHOLD	4

struct _DskQsortStackNode
{
  size_t start, len;
};


#define DSK_QSORT(array, type, n_elements, compare)		             \
  DSK_QSORT_FULL(array, type, n_elements, compare,                           \
                 DSK_INSERTION_SORT_THRESHOLD,                               \
                 DSK_QSORT_STACK_MAX_SIZE,                                   \
                 /* no stack guard assertion */)

#define DSK_QSORT_DEBUG(array, type, n_elements, compare)		     \
  DSK_QSORT_FULL(array, type, n_elements, compare,                           \
                 DSK_INSERTION_SORT_THRESHOLD,                               \
                 DSK_QSORT_STACK_MAX_SIZE,                                   \
                 DSK_QSORT_ASSERT_STACK_SIZE)

/* Return the top n_select elements.  (They are sorted)
   The remaining elements are at the end of the array, not
   in any particular order. */
#define DSK_QSELECT(array, type, n_elements, n_select, compare)		     \
  DSK_QSELECT_FULL(array, type, n_elements, n_select, compare,               \
                   DSK_INSERTION_SORT_THRESHOLD,                             \
                   DSK_QSORT_STACK_MAX_SIZE,                                 \
                   /* no stack guard assertion */)

#define DSK_QSORT_FULL(array, type, n_elements, compare, isort_threshold, stack_size, ss_assertion) \
  do{                                                                        \
    int dsk_rv;                                                              \
    unsigned  dsk_stack_size;                                                \
    DskQsortStackNode dsk_stack[stack_size];                                 \
    type dsk_tmp_swap;                                                       \
    DskQsortStackNode dsk_node;                                              \
    dsk_node.start = 0;                                                      \
    dsk_node.len = (n_elements);                                             \
    dsk_stack_size = 0;                                                      \
    if (n_elements <= isort_threshold)                                       \
      DSK_INSERTION_SORT(array,type,n_elements,compare);                     \
    else                                                                     \
      for(;;)                                                                \
        {                                                                    \
          DskQsortStackNode dsk_stack_nodes[2];                              \
          /* implement median-of-three; sort so that  */                     \
          /* *dsk_a <= *dsk_b <= *dsk_c               */                     \
          type *dsk_lo = array + dsk_node.start;                             \
          type *dsk_hi = dsk_lo + dsk_node.len - 1;                          \
          type *dsk_a = dsk_lo;                                              \
          type *dsk_b = array + dsk_node.start + dsk_node.len / 2;           \
          type *dsk_c = dsk_hi;                                              \
          compare((*dsk_a), (*dsk_b), dsk_rv);                               \
          if (dsk_rv < 0)                                                    \
            {                                                                \
              compare((*dsk_b), (*dsk_c), dsk_rv);                           \
              if (dsk_rv <= 0)                                               \
                {                                                            \
                  /* a <= b <= c: already sorted */                          \
                }                                                            \
              else                                                           \
                {                                                            \
                  compare((*dsk_a), (*dsk_c), dsk_rv);                       \
                  if (dsk_rv <= 0)                                           \
                    {                                                        \
                      /* a <= c <= b */                                      \
                      dsk_tmp_swap = *dsk_b;                                 \
                      *dsk_b = *dsk_c;                                       \
                      *dsk_c = dsk_tmp_swap;                                 \
                    }                                                        \
                  else                                                       \
                    {                                                        \
                      /* c <= a <= b */                                      \
                      dsk_tmp_swap = *dsk_a;                                 \
                      *dsk_a = *dsk_c;                                       \
                      *dsk_c = *dsk_b;                                       \
                      *dsk_b = dsk_tmp_swap;                                 \
                    }                                                        \
                }                                                            \
            }                                                                \
          else                                                               \
            {                                                                \
              /* *b < *a */                                                  \
              compare((*dsk_b), (*dsk_c), dsk_rv);                           \
              if (dsk_rv >= 0)                                               \
                {                                                            \
                  /* *c <= *b < *a */                                        \
                  dsk_tmp_swap = *dsk_c;                                     \
                  *dsk_c = *dsk_a;                                           \
                  *dsk_a = dsk_tmp_swap;                                     \
                }                                                            \
              else                                                           \
                {                                                            \
                  /* b<a, b<c */                                             \
                  compare((*dsk_a), (*dsk_c), dsk_rv);                       \
                  if (dsk_rv >= 0)                                           \
                    {                                                        \
                      /* b < c <= a */                                       \
                      dsk_tmp_swap = *dsk_a;                                 \
                      *dsk_a = *dsk_b;                                       \
                      *dsk_b = *dsk_c;                                       \
                      *dsk_c = dsk_tmp_swap;                                 \
                    }                                                        \
                  else                                                       \
                    {                                                        \
                      /* b < a < c */                                        \
                      dsk_tmp_swap = *dsk_a;                                 \
                      *dsk_a = *dsk_b;                                       \
                      *dsk_b = dsk_tmp_swap;                                 \
                    }                                                        \
                }                                                            \
            }                                                                \
                                                                             \
          /* ok, phew, now *dsk_a <= *dsk_b <= *dsk_c */                     \
                                                                             \
          /* partition this range of the array */                            \
          dsk_a++;                                                           \
          dsk_c--;                                                           \
          do                                                                 \
            {                                                                \
              /* advance dsk_a to an element that violates */                \
              /* partitioning (or it hits dsk_b) */                          \
              for (;;)                                                       \
                {                                                            \
                  compare((*dsk_a), (*dsk_b), dsk_rv);                       \
                  if (dsk_rv >= 0)                                           \
                    break;                                                   \
                  dsk_a++;                                                   \
                }                                                            \
              /* decrease dsk_c to an element that violates */               \
              /* partitioning (or it hits dsk_b) */                          \
              for (;;)                                                       \
                {                                                            \
                  compare((*dsk_b), (*dsk_c), dsk_rv);                       \
                  if (dsk_rv >= 0)                                           \
                    break;                                                   \
                  dsk_c--;                                                   \
                }                                                            \
              if (dsk_a < dsk_c)                                             \
                {                                                            \
                  dsk_tmp_swap = *dsk_a;                                     \
                  *dsk_a = *dsk_c;                                           \
                  *dsk_c = dsk_tmp_swap;                                     \
                  if (dsk_a == dsk_b)                                        \
                    dsk_b = dsk_c;                                           \
                  else if (dsk_b == dsk_c)                                   \
                    dsk_b = dsk_a;                                           \
                  dsk_a++;                                                   \
                  dsk_c--;                                                   \
                }                                                            \
              else if (dsk_a == dsk_c)                                       \
                {                                                            \
                  dsk_a++;                                                   \
                  dsk_c--;                                                   \
                  break;                                                     \
                }                                                            \
            }                                                                \
          while (dsk_a <= dsk_c);                                            \
                                                                             \
          /* the two partitions are [lo,c] and [a,hi], */                    \
          /* which are disjoint since (a > b) by the above loop guard */     \
                                                                             \
          /* These commented-out macros validate that the partitioning worked. */ \
          /*{type*dsk_tmp2; for (dsk_tmp2=dsk_lo;dsk_tmp2<=dsk_c;dsk_tmp2++){ compare(*dsk_tmp2,*dsk_b,dsk_rv); dsk_assert(dsk_rv<=0); }}*/ \
          /*{type*dsk_tmp2; for (dsk_tmp2=dsk_a;dsk_tmp2<=dsk_hi;dsk_tmp2++){ compare(*dsk_tmp2,*dsk_b,dsk_rv); dsk_assert(dsk_rv>=0); }}*/ \
          /*{type*dsk_tmp2; for (dsk_tmp2=dsk_c+1;dsk_tmp2<dsk_a;dsk_tmp2++){ compare(*dsk_tmp2,*dsk_b,dsk_rv); dsk_assert(dsk_rv==0); }}*/ \
                                                                             \
          /* push parts onto stack:  the bigger half must be pushed    */    \
          /* on first to guarantee that the max stack depth is O(log N) */   \
          dsk_stack_nodes[0].start = dsk_node.start;                         \
          dsk_stack_nodes[0].len = dsk_c - dsk_lo + 1;                       \
          dsk_stack_nodes[1].start = dsk_a - (array);                        \
          dsk_stack_nodes[1].len = dsk_hi - dsk_a + 1;                       \
          if (dsk_stack_nodes[0].len < dsk_stack_nodes[1].len)               \
            {                                                                \
              DskQsortStackNode dsk_stack_node_tmp = dsk_stack_nodes[0];     \
              dsk_stack_nodes[0] = dsk_stack_nodes[1];                       \
              dsk_stack_nodes[1] = dsk_stack_node_tmp;                       \
            }                                                                \
          if (dsk_stack_nodes[0].len > isort_threshold)                      \
            {                                                                \
              if (dsk_stack_nodes[1].len > isort_threshold)                  \
                {                                                            \
                  dsk_stack[dsk_stack_size++] = dsk_stack_nodes[0];          \
                  dsk_node = dsk_stack_nodes[1];                             \
                }                                                            \
              else                                                           \
                {                                                            \
                  DSK_INSERTION_SORT ((array) + dsk_stack_nodes[1].start,    \
                                      type, dsk_stack_nodes[1].len, compare);\
                  dsk_node = dsk_stack_nodes[0];                             \
                }                                                            \
            }                                                                \
          else                                                               \
            {                                                                \
              DSK_INSERTION_SORT ((array) + dsk_stack_nodes[0].start,        \
                                  type, dsk_stack_nodes[0].len, compare);    \
              DSK_INSERTION_SORT ((array) + dsk_stack_nodes[1].start,        \
                                  type, dsk_stack_nodes[1].len, compare);    \
              if (dsk_stack_size == 0)                                       \
                break;                                                       \
              dsk_node = dsk_stack[--dsk_stack_size];                        \
            }                                                                \
          ss_assertion;                                                      \
        }                                                                    \
  }while(0)

/* TODO: do we want DSK_INSERTION_SELECT for use here internally? */
#define DSK_QSELECT_FULL(array, type, n_elements, n_select, compare, isort_threshold, stack_size, ss_assertion)    \
  do{                                                                        \
    int dsk_rv;                                                              \
    unsigned dsk_stack_size;                                                 \
    DskQsortStackNode dsk_stack[stack_size];                                 \
    type dsk_tmp_swap;                                                       \
    DskQsortStackNode dsk_node;                                              \
    dsk_node.start = 0;                                                      \
    dsk_node.len = (n_elements);                                             \
    dsk_stack_size = 0;                                                      \
    if (n_elements <= isort_threshold)                                       \
      DSK_INSERTION_SORT(array,type,n_elements,compare);                     \
    else                                                                     \
      for(;;)                                                                \
        {                                                                    \
          DskQsortStackNode dsk_stack_nodes[2];                              \
          /* implement median-of-three; sort so that  */                     \
          /* *dsk_a <= *dsk_b <= *dsk_c               */                     \
          type *dsk_lo = array + dsk_node.start;                             \
          type *dsk_hi = dsk_lo + dsk_node.len - 1;                          \
          type *dsk_a = dsk_lo;                                              \
          type *dsk_b = array + dsk_node.start + dsk_node.len / 2;           \
          type *dsk_c = dsk_hi;                                              \
          compare((*dsk_a), (*dsk_b), dsk_rv);                               \
          if (dsk_rv < 0)                                                    \
            {                                                                \
              compare((*dsk_b), (*dsk_c), dsk_rv);                           \
              if (dsk_rv <= 0)                                               \
                {                                                            \
                  /* a <= b <= c: already sorted */                          \
                }                                                            \
              else                                                           \
                {                                                            \
                  compare((*dsk_a), (*dsk_c), dsk_rv);                       \
                  if (dsk_rv <= 0)                                           \
                    {                                                        \
                      /* a <= c <= b */                                      \
                      dsk_tmp_swap = *dsk_b;                                 \
                      *dsk_b = *dsk_c;                                       \
                      *dsk_c = dsk_tmp_swap;                                 \
                    }                                                        \
                  else                                                       \
                    {                                                        \
                      /* c <= a <= b */                                      \
                      dsk_tmp_swap = *dsk_a;                                 \
                      *dsk_a = *dsk_c;                                       \
                      *dsk_c = *dsk_b;                                       \
                      *dsk_b = dsk_tmp_swap;                                 \
                    }                                                        \
                }                                                            \
            }                                                                \
          else                                                               \
            {                                                                \
              /* *b < *a */                                                  \
              compare((*dsk_b), (*dsk_c), dsk_rv);                           \
              if (dsk_rv >= 0)                                               \
                {                                                            \
                  /* *c <= *b < *a */                                        \
                  dsk_tmp_swap = *dsk_c;                                     \
                  *dsk_c = *dsk_a;                                           \
                  *dsk_a = dsk_tmp_swap;                                     \
                }                                                            \
              else                                                           \
                {                                                            \
                  /* b<a, b<c */                                             \
                  compare((*dsk_a), (*dsk_c), dsk_rv);                       \
                  if (dsk_rv >= 0)                                           \
                    {                                                        \
                      /* b < c <= a */                                       \
                      dsk_tmp_swap = *dsk_a;                                 \
                      *dsk_a = *dsk_b;                                       \
                      *dsk_b = *dsk_c;                                       \
                      *dsk_c = dsk_tmp_swap;                                 \
                    }                                                        \
                  else                                                       \
                    {                                                        \
                      /* b < a < c */                                        \
                      dsk_tmp_swap = *dsk_a;                                 \
                      *dsk_a = *dsk_b;                                       \
                      *dsk_b = dsk_tmp_swap;                                 \
                    }                                                        \
                }                                                            \
            }                                                                \
                                                                             \
          /* ok, phew, now *dsk_a <= *dsk_b <= *dsk_c */                     \
                                                                             \
          /* partition this range of the array */                            \
          dsk_a++;                                                           \
          dsk_c--;                                                           \
          do                                                                 \
            {                                                                \
              /* advance dsk_a to a element that violates */                 \
              /* partitioning (or it hits dsk_b) */                          \
              for (;;)                                                       \
                {                                                            \
                  compare((*dsk_a), (*dsk_b), dsk_rv);                       \
                  if (dsk_rv >= 0)                                           \
                    break;                                                   \
                  dsk_a++;                                                   \
                }                                                            \
              /* advance dsk_c to a element that violates */                 \
              /* partitioning (or it hits dsk_b) */                          \
              for (;;)                                                       \
                {                                                            \
                  compare((*dsk_b), (*dsk_c), dsk_rv);                       \
                  if (dsk_rv >= 0)                                           \
                    break;                                                   \
                  dsk_c--;                                                   \
                }                                                            \
              if (dsk_a < dsk_c)                                             \
                {                                                            \
                  dsk_tmp_swap = *dsk_a;                                     \
                  *dsk_a = *dsk_c;                                           \
                  *dsk_c = dsk_tmp_swap;                                     \
                  if (dsk_a == dsk_b)                                        \
                    dsk_b = dsk_c;                                           \
                  else if (dsk_b == dsk_c)                                   \
                    dsk_b = dsk_a;                                           \
                  dsk_a++;                                                   \
                  dsk_c--;                                                   \
                }                                                            \
              else if (dsk_a == dsk_c)                                       \
                {                                                            \
                  dsk_a++;                                                   \
                  dsk_c--;                                                   \
                  break;                                                     \
                }                                                            \
            }                                                                \
          while (dsk_a <= dsk_c);                                            \
                                                                             \
          /* the two partitions are [lo,c] and [a,hi], */                    \
          /* which are disjoint since (a > b) by the above loop guard */     \
                                                                             \
          /* push parts onto stack:  the bigger half must be pushed    */    \
          /* on first to guarantee that the max stack depth is O(log N) */   \
          dsk_stack_nodes[0].start = dsk_node.start;                         \
          dsk_stack_nodes[0].len = dsk_c - dsk_lo + 1;                       \
          dsk_stack_nodes[1].start = dsk_a - (array);                        \
          dsk_stack_nodes[1].len = dsk_hi - dsk_a + 1;                       \
          if (dsk_stack_nodes[1].start >= n_select)                          \
            {                                                                \
              if (dsk_stack_nodes[0].len > isort_threshold)                  \
                {                                                            \
                  dsk_node = dsk_stack_nodes[0];                             \
                }                                                            \
              else                                                           \
                {                                                            \
                  DSK_INSERTION_SORT ((array) + dsk_stack_nodes[0].start,    \
                                      type, dsk_stack_nodes[0].len, compare);\
                  if (dsk_stack_size == 0)                                   \
                    break;                                                   \
                  dsk_node = dsk_stack[--dsk_stack_size];                    \
                }                                                            \
            }                                                                \
          else                                                               \
            {                                                                \
              if (dsk_stack_nodes[0].len < dsk_stack_nodes[1].len)           \
                {                                                            \
                  DskQsortStackNode dsk_stack_node_tmp = dsk_stack_nodes[0]; \
                  dsk_stack_nodes[0] = dsk_stack_nodes[1];                   \
                  dsk_stack_nodes[1] = dsk_stack_node_tmp;                   \
                }                                                            \
              if (dsk_stack_nodes[0].len > isort_threshold)                  \
                {                                                            \
                  if (dsk_stack_nodes[1].len > isort_threshold)              \
                    {                                                        \
                      dsk_stack[dsk_stack_size++] = dsk_stack_nodes[0];      \
                      dsk_node = dsk_stack_nodes[1];                         \
                      ss_assertion;                                          \
                    }                                                        \
                  else                                                       \
                    {                                                        \
                      DSK_INSERTION_SORT ((array) + dsk_stack_nodes[1].start,\
                                          type, dsk_stack_nodes[1].len, compare);\
                      dsk_node = dsk_stack_nodes[0];                         \
                    }                                                        \
                }                                                            \
              else                                                           \
                {                                                            \
                  DSK_INSERTION_SORT ((array) + dsk_stack_nodes[0].start,    \
                                      type, dsk_stack_nodes[0].len, compare);\
                  DSK_INSERTION_SORT ((array) + dsk_stack_nodes[1].start,    \
                                  type, dsk_stack_nodes[1].len, compare);    \
                  if (dsk_stack_size == 0)                                   \
                    break;                                                   \
                  dsk_node = dsk_stack[--dsk_stack_size];                    \
                }                                                            \
            }                                                                \
        }                                                                    \
  }while(0)


/* Do not allow equality, since that would make the next push a
   stack overflow, and we might not detect it correctly to stack corruption. */
#define DSK_QSORT_ASSERT_STACK_SIZE(stack_alloced)                          \
  dsk_assert(dsk_stack_size < stack_alloced)

#define DSK_INSERTION_SORT(array, type, length, compare)                     \
  do{                                                                        \
    unsigned dsk_ins_i, dsk_ins_j;                                           \
    type dsk_ins_tmp;                                                        \
    for (dsk_ins_i = 1; dsk_ins_i < length; dsk_ins_i++)                     \
      {                                                                      \
        /* move (dsk_ins_i-1) into place */                                  \
        unsigned dsk_ins_min = dsk_ins_i - 1;                                \
        int dsk_ins_compare_rv;                                              \
        for (dsk_ins_j = dsk_ins_i; dsk_ins_j < length; dsk_ins_j++)         \
          {                                                                  \
            compare(((array)[dsk_ins_min]), ((array)[dsk_ins_j]),            \
                    dsk_ins_compare_rv);                                     \
            if (dsk_ins_compare_rv > 0)                                      \
              dsk_ins_min = dsk_ins_j;                                       \
          }                                                                  \
        /* swap dsk_ins_min and (dsk_ins_i-1) */                             \
        dsk_ins_tmp = (array)[dsk_ins_min];                                  \
        (array)[dsk_ins_min] = (array)[dsk_ins_i - 1];                       \
        (array)[dsk_ins_i - 1] = dsk_ins_tmp;                                \
      }                                                                      \
  }while(0)

#define DSK_QSORT_SIMPLE_COMPARATOR(a,b,compare_rv)                          \
  do{                                                                        \
    if ((a) < (b))                                                           \
      compare_rv = -1;                                                       \
    else if ((a) > (b))                                                      \
      compare_rv = 1;                                                        \
    else                                                                     \
      compare_rv = 0;                                                        \
  }while(0)
