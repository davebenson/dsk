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
static inline void *check_and_zero_lowest_bit(intptr_t p, unsigned v)
{
  dsk_assert ((p & 1) == v);
  return (void*)((child_intptr) & (~(intptr_t)1))
}
# define CHILD_LEAF(child_intptr)     check_and_zero_lowest_bit(child_intptr,1)
# define CHILD_INNER(child_intptr)    check_and_zero_lowest_bit(child_intptr,0)
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

static void *
str_table_critbit__get (DskStrTable   *table,
                       const char    *str)
{
  DskStrTableCritbit *critbit = (DskStrTableCritbit *) to_init;
  ...
}

static void *
str_table_critbit__get_prev     (DskStrTable   *table,
                     const char    *str,
                     const char   **key_out)
{
  DskStrTableCritbit *critbit = (DskStrTableCritbit *) to_init;
  ...
}

static void *
str_table_critbit__get_next     (DskStrTable   *table,
                     const char    *str,
                     const char   **key_out)
{
  DskStrTableCritbit *critbit = (DskStrTableCritbit *) to_init;
  ...
}

static void *
str_table_critbit__force        (DskStrTable   *table,
                                const char    *str)
{
  DskStrTableCritbit *critbit = (DskStrTableCritbit *) to_init;
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
