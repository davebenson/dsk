#include <string.h>
#include "dsk.h"

typedef struct _DskStrTableCritbitInnerNode DskStrTableCritbitInnerNode;
typedef struct _DskStrTableCritbit DskStrTableCritbit;
typedef struct _DskStrTableCritbitLeafNode DskStrTableCritbitLeafNode;

struct _DskStrTableCritbitLeafNode
{
  unsigned n_remaining_bits;
  /* bits follow, value precedes node */
};

struct _DskStrTableCritbitInnerNode
{
  intptr_t children[2];		/* may be inner or leaf node;
                                   both children always exist */
  unsigned has_value : 1;
  unsigned n_common_bits : 31;

  /* bits follow, value precedes node */
};

struct _DskStrTableCritbit
{
  DskStrTable base;
  unsigned max_depth;
  unsigned buf_alloced;
  char *buf;
  unsigned value_size_aligned;
  void *default_value;

  intptr_t top;         /* leaf or inner */
};


#define IS_CHILD_LEAF(child_intptr)  ((child_intptr) & 1)

#if DEBUG_CHILD_HANDLING
static inline void *debug__check_and_zero_lowest_bit(intptr_t p, unsigned v)
{
  dsk_assert ((p & 1) == v);
  return (void*)((child_intptr) & (~(intptr_t)1))
}
# define CHILD_LEAF(child_intptr)     debug__check_and_zero_lowest_bit(child_intptr,1)
# define CHILD_INNER(child_intptr)    debug__check_and_zero_lowest_bit(child_intptr,0)
#else
# define CHILD_LEAF(child_intptr)     (void*)((child_intptr) & (~(intptr_t)1))
# define CHILD_INNER(child_intptr)    (void*)((child_intptr))
#endif


static void 
str_table_critbit__init(size_t         value_size,
                        const void    *value_default,
                        DskStrTable   *to_init)
{
  DskStrTableCritbit *critbit = (DskStrTableCritbit *) to_init;
  critbit->top = 0;
  critbit->value_size_aligned = DSK_ALIGN (value_size, 8);
  if (value_default)
    critbit->default_value = memcpy (dsk_malloc (critbit->value_size_aligned),
                                     value_default, value_size);
  else
    critbit->default_value = NULL;
  critbit->max_depth = 0;
  critbit->buf_alloced = 0;
  critbit->buf = NULL;
}

static int
compare_aligned_bits (unsigned       bit_offset,
                      unsigned       n_bits,
                      const char    *str,
                      const uint8_t *node_bits)
{
  str += bit_offset / 8;
  if (bit_offset & 7)
    {
      /* handle initial partial bits */
      unsigned part = 8 - (bit_offset & 7);
      uint8_t mask = (1<<part) - 1;
      int rv = (int)(uint8_t)(*str & mask)
             - (int)(uint8_t)(*node_bits & mask);
      if (rv)
        return rv;
      str++;
      node_bits++;
      n_bits -= part;
    }

  while (n_bits >= 8)
    {
      int rv = (int)(uint8_t)(*str) - (int)(uint8_t)(*node_bits);
      if (rv)
        return rv;
      n_bits -= 8;
      str++;
      node_bits++;
    }
  if (n_bits == 0)
    return 0;
  
  /* compare partial byte */
  {
    uint8_t mask = (0xff << (8 - n_bits));
    return (int)(uint8_t)(*str & mask)
         - (int)(uint8_t)(*node_bits & mask);
  }
}


static void *
str_table_critbit__get (DskStrTable   *table,
                       const char    *str)
{
  DskStrTableCritbit *critbit = (DskStrTableCritbit *) table;
  unsigned bit_at = 0;
  unsigned n_bits = strlen (str) * 8;

  DskStrTableCritbitInnerNode *inner;
  DskStrTableCritbitLeafNode *leaf;

  if (critbit->top == 0)
    return NULL;
  else if (IS_CHILD_LEAF (critbit->top))
    {
      leaf = CHILD_LEAF (critbit->top);
      goto at_leaf;
    }

  inner = CHILD_INNER (critbit->top);
  for (;;)   /* While processing inner nodes: */
    {
      /* scan common bits */
      int cmp = compare_aligned_bits (bit_at, inner->n_common_bits, str, (const uint8_t*)(inner+1));
      if (cmp != 0)
        return NULL;
      bit_at += inner->n_common_bits;
      if (bit_at == n_bits)
        {
          return inner->has_value
               ? ((char*)(inner) - critbit->value_size_aligned)
               : NULL;
        }

      /* jump as appropriate */
      intptr_t child = inner->children[(str[bit_at/8] >> (7-(bit_at%8))) & 1];
      bit_at++;
      if (child == 0)
        return NULL;
      if (IS_CHILD_LEAF (child))
        {
          leaf = CHILD_LEAF (child);
          goto at_leaf;
        }
      inner = CHILD_INNER (child);
    }

at_leaf:
  /* scan remaining bits */
  if (compare_aligned_bits (bit_at, leaf->n_remaining_bits,
                            str, (const uint8_t*)(leaf + 1)) != 0)
    return NULL;
  return ((char*)(leaf) - critbit->value_size_aligned);
}

static void *
str_table_critbit__get_prev     (DskStrTable   *table,
                                 const char    *str,
                                 const char   **key_out)
{
  DskStrTableCritbit *critbit = (DskStrTableCritbit *) table;
  DskStrTableCritbitInnerNode *last_right_parent = NULL;
  DskStrTableCritbitLeafNode *leaf = NULL;
  unsigned bit_at = 0;
  if (critbit->top == 0)
    return NULL;
  if (IS_CHILD_LEAF (critbit->top))
    {
      leaf = CHILD_LEAF (critbit->top);
      goto handle_leaf;
    }
  else
    {
      DskStrTableCritbitInnerNode *inner = CHILD_INNER (critbit->top);
      for (;;)
        {
          int cmp = compare_aligned_bits (...);
          if (cmp < 0)  /* str < node */
            {
              ...
            }
          else if (cmp > 0) /* str > node */
            {
              ...
            }
          else
            {
              bit_at += ...;
            }
        }
    }

handle_leaf:
  ...
}

static void *
str_table_critbit__get_next     (DskStrTable   *table,
                                 const char    *str,
                                 const char   **key_out)
{
  DskStrTableCritbit *critbit = (DskStrTableCritbit *) table;
  ...
}

static void *
str_table_critbit__force        (DskStrTable   *table,
                                const char    *str)
{
  DskStrTableCritbit *critbit = (DskStrTableCritbit *) table;
  ...
}

static void *
str_table_critbit__force_noinit (DskStrTable   *table,
                                 const char    *str,
                                 dsk_boolean *created_opt_out)
{
  DskStrTableCritbit *critbit = (DskStrTableCritbit *) table;
  ...
}

static void *
str_table_critbit__create       (DskStrTable   *table,
                                 const char    *str)
{
  DskStrTableCritbit *critbit = (DskStrTableCritbit *) table;
  ...
}

static dsk_boolean  
str_table_critbit__remove(DskStrTable   *table,
                          const char    *str,
                          void          *value_opt_out)
{
  DskStrTableCritbit *critbit = (DskStrTableCritbit *) table;
  ...
}

static void *
str_table_critbit__create_or_remove_noinit
                    (DskStrTable   *str_table,
                     const char    *str)
{
  DskStrTableCritbit *critbit = (DskStrTableCritbit *) table;
  ...
}

static void *
str_table_critbit__create_or_remove
                    (DskStrTable   *table,
                     const char    *str)
{
  DskStrTableCritbit *critbit = (DskStrTableCritbit *) table;
  void *rv = str_table_critbit__create_or_remove_noinit (table, str);
  if (rv != NULL)
    {
      if (critbit->default_value)
        memcpy (rv, critbit->default_value, critbit->value_size);
      else
        memset (rv, 0, critbit->value_size);
    }
  return rv;
}

static void  
str_table_critbit__set(DskStrTable   *str_table,
                      const char    *str,
                      const void    *value)
{
  DskStrTableCritbit *critbit = (DskStrTableCritbit *) table;
  void *nv = str_table_critbit__force_noinit (str_table, str, NULL);
  memcpy (nv, value, critbit->value_size);
}

static dsk_boolean 
str_table_critbit__foreach(DskStrTable   *str_table,
                          const char    *prefix,
                          DskStrTableForeachFind foreach,
                          void          *foreach_data)
{
  DskStrTableCritbit *critbit = (DskStrTableCritbit *) table;
  ...
}

DskStrTableInterface dsk_str_table_interface_critbit =
{
  sizeof (DskStrTableCritbit),
  str_table_critbit__init,
  str_table_critbit__get,
  str_table_critbit__get_prev,
  str_table_critbit__get_next,
  str_table_critbit__force,
  str_table_critbit__force_noinit,
  str_table_critbit__create,
  str_table_critbit__remove,
  str_table_critbit__create_or_remove,
  str_table_critbit__create_or_remove_noinit,
  str_table_critbit__set,
  str_table_critbit__foreach
};
