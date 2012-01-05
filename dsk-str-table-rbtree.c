#include "dsk-str-table.h"

typedef struct _DskStrTableRbtree DskStrTableRbtree;
struct _DskStrTableRbtree
{
  DskStrtableRbtreeNode *top;
  size_t value_size;
  void *default_value;
};
typedef struct _DskStrTableRbtreeNode DskStrTableRbtreeNode;
struct _DskStrTableRbtreeNode
{
  DskStrtableRbtreeNode *parent, *left, *right;
  dsk_boolean is_red;
  /* value follows, then string follows that */
};

#define GET_NODE_STRING(node) \
  ((char*)(node) + sizeof (DskStrtableRbtreeNode) + rbtree->value_size)

#define COMPARE_RBTREE_NODES(a,b, rv) \
  rv = strcmp (GET_NODE_STRING (a), GET_NODE_STRING (b))
#define COMPARE_STR_TO_RBTREE_NODE(a,b, rv) \
  rv = strcmp (a, GET_NODE_STRING (b))

#define GET_RBTREE()   \
  rbtree->top, DskStrtableRbtreeNode *, DSK_STD_GET_IS_RED, \
  DSK_STD_SET_IS_RED, parent, left, right, \
  COMPARE_RBTREE_NODES

static void 
str_table_rbtree__init(size_t         value_size,
                       const void    *value_default,
                       DskStrTable   *to_init)
{
  DskStrTableRbtree *rbtree = (DskStrTableRbtree *) to_init;
  rbtree->top = NULL;
  rbtree->value_size = value_size;
  if (value_default)
    rbtree->default_value = dsk_memdup (value_size, value_default);
  else
    rbtree->default_value = NULL;
}

static void *
str_table_rbtree__get (DskStrTable   *table,
                       const char    *str)
{
  DskStrTableRbtree *rbtree = (DskStrTableRbtree *) to_init;
  DskStrtableRbtreeNode *node;
  DSK_RBTREE_LOOKUP_COMPARATOR (GET_RBTREE (), str, COMPARE_STR_TO_RBTREE_NODE,
                                node);
  if (node == NULL)
    return NULL;
  else
    return node + 1;
}

static void *
str_table_rbtree__get_prev     (DskStrTable   *table,
                     const char    *str,
                     const char   **key_out)
{
  DSK_RBTREE_INFIMUM_COMPARATOR (GET_RBTREE (), str, COMPARE_STR_TO_RBTREE_NODE,
                                 node);
  if (node == NULL)
    return NULL;
  if (strcmp (GET_NODE_STRING (node), str) == 0)
    DSK_RBTREE_PREV (GET_RBTREE (), node, node);
  if (node == NULL)
    return NULL;
  if (key_out != NULL)
    key_out = GET_NODE_STRING (node);
  return node + 1;
}

static void *
str_table_rbtree__get_next     (DskStrTable   *table,
                     const char    *str,
                     const char   **key_out)
{
  DSK_RBTREE_SUPREMUM_COMPARATOR (GET_RBTREE (),
                                  str, COMPARE_STR_TO_RBTREE_NODE,
                                  node);
  if (node == NULL)
    return NULL;
  if (strcmp (GET_NODE_STRING (node), str) == 0)
    DSK_RBTREE_NEXT (GET_RBTREE (), node, node);
  if (node == NULL)
    return NULL;
  if (key_out != NULL)
    key_out = GET_NODE_STRING (node);
  return node + 1;
}

static void *
str_table_rbtree__force        (DskStrTable   *table,
                                const char    *str)
{
  DskStrTableRbtree *rbtree = (DskStrTableRbtree *) table;
  DskStrtableRbtreeNode *node;
  DSK_RBTREE_LOOKUP_COMPARATOR (GET_RBTREE (),
                                str, COMPARE_STR_TO_RBTREE_NODE,
                                node);
  if (node != NULL)
    return node + 1;
  len = strlen (str);
  node = dsk_malloc (sizeof (DskStrtableRbtreeNode) + rbtree->value_size
                     + len + 1);
  memcpy (GET_NODE_STRING (node), str, len + 1);
  DSK_RBTREE_INSERT (GET_RBTREE (), node, conflict);
  dsk_assert (conflict == NULL);
  if (rbtree->default_value)
    memcpy (node + 1, rbtree->default_value, rbtree->value_size);
  else
    memset (node + 1, 0, rbtree->value_size);
  return node + 1;
}

static void *
str_table_rbtree__force_noinit (DskStrTable   *table,
                                const char    *str,
                                dsk_boolean *created_opt_out)
{
  DskStrTableRbtree *rbtree = (DskStrTableRbtree *) table;
  DskStrtableRbtreeNode *node;
  DSK_RBTREE_LOOKUP_COMPARATOR (GET_RBTREE (),
                                str, COMPARE_STR_TO_RBTREE_NODE,
                                node);
  if (node != NULL)
    {
      if (created_opt_out)
        *created_opt_out = DSK_FALSE;
      return node + 1;
    }
  len = strlen (str);
  node = dsk_malloc (sizeof (DskStrtableRbtreeNode) + rbtree->value_size
                     + len + 1);
  memcpy (GET_NODE_STRING (node), str, len + 1);
  DSK_RBTREE_INSERT (GET_RBTREE (), node, conflict);
  dsk_assert (conflict == NULL);
  if (created_opt_out)
    *created_opt_out = DSK_TRUE;
  return node + 1;
}

static void *
str_table_rbtree__create       (DskStrTable   *table,
                                const char    *str)
{
  DskStrTableRbtree *rbtree = (DskStrTableRbtree *) table;
  DskStrtableRbtreeNode *node;
  unsigned len;
  DskStrtableRbtreeNode *conflict;
  DSK_RBTREE_LOOKUP_COMPARATOR (GET_RBTREE (),
                                str, COMPARE_STR_TO_RBTREE_NODE,
                                node);
  if (node != NULL)
    return NULL;
  len = strlen (str);
  node = dsk_malloc (sizeof (DskStrtableRbtreeNode) + rbtree->value_size
                     + len + 1);
  memcpy (GET_NODE_STRING (node), str, len + 1);
  DSK_RBTREE_INSERT (GET_RBTREE (), node, conflict);
  dsk_assert (conflict == NULL);
  if (rbtree->default_value)
    memcpy (node + 1, rbtree->default_value, rbtree->value_size);
  else
    memset (node + 1, 0, rbtree->value_size);
  return node + 1;
}

static dsk_boolean  
str_table_rbtree__remove(DskStrTable   *table,
                         const char    *str,
                         void          *value_opt_out)
{
  DskStrtableRbtreeNode *node;
  DSK_RBTREE_LOOKUP_COMPARATOR (GET_RBTREE (),
                                str, COMPARE_STR_TO_RBTREE_NODE,
                                node);
  if (node == NULL)
    return DSK_FALSE;
  if (value_opt_out)
    memcpy (value_opt_out, node + 1, rbtree->value_size);
  DSK_RBTREE_REMOVE (GET_RBTREE (), node);
  dsk_free (node);
  return DSK_TRUE;
}

static void *
str_table_rbtree__create_or_remove_noinit
                    (DskStrTable   *str_table,
                     const char    *str)
{
  ...
  DskStrtableRbtreeNode *node;
  DSK_RBTREE_LOOKUP_COMPARATOR (GET_RBTREE (),
                                str, COMPARE_STR_TO_RBTREE_NODE,
                                node);
  if (node)
    {
      /* remove */
      DSK_RBTREE_REMOVE (GET_RBTREE (), node);
      return NULL;
    }
  else
    {
      /* create */
      unsigned len = strlen (str);
      DskStrtableRbtreeNode *node = dsk_malloc (sizeof (DskStrtableRbtreeNode) + rbtree->value_size
                         + len + 1);
      DskStrtableRbtreeNode *conflict;
      memcpy (GET_NODE_STRING (node), str, len + 1);
      DSK_RBTREE_INSERT (GET_RBTREE (), node, conflict);
      dsk_assert (conflict == NULL);
      return node + 1;
    }
}

static void *
str_table_rbtree__create_or_remove
                    (DskStrTable   *table,
                     const char    *str)
{
  void *rv = str_table_rbtree__create_or_remove_noinit (table, str);
  if (rv != NULL)
    {
      if (rbtree->default_value)
        memcpy (rv, rbtree->default_value, rbtree->value_size);
      else
        memset (rv, 0, rbtree->value_size);
    }
  return rv;
}

static void  
str_table_rbtree__set(DskStrTable   *str_table,
                      const char    *str,
                      const void    *value)
{
  DskStrTableRbtree *rbtree = (DskStrTableRbtree *) table;
  void *nv = str_table_rbtree__force_noinit (str_table, str, NULL);
  memcpy (nv, value, rbtree->value_size);
}

static dsk_boolean
foreach_recursive (DskStrTableRbtree *rbtree,
                   unsigned prefix_length,
                   const char *prefix,
                   DskStrTableRbtreeNode *node,
                   DskStrTableForeachFind foreach,
                   void          *foreach_data)
{
  int cmp = 0;
  char *node_str = GET_NODE_STRING (node);
  for (i = 0; i < prefix_length && cmp == 0; i++)
    {
      if (node_str[i] == 0)
        {
          cmp = 1;
          break;
        }
      cmp = (int)(uint8_t)prefix[i] - (int)(uint8_t)node_str[i];
    }
  if (cmp <= 0 && node->left != NULL)
    {
      if (foreach_recursive (rbtree, prefix_length, prefix, node->left,
                             foreach, foreach_data))
        return DSK_TRUE;
    }
  if (cmp == 0)
    if (foreach (node_str, node + 1, foreach_data))
      return DSK_TRUE;
  if (cmp >= 0 && node->right != NULL)
    {
      if (foreach_recursive (rbtree, prefix_length, prefix, node->right,
                             foreach, foreach_data))
        return DSK_TRUE;
    }
  return DSK_FALSE;
}

static dsk_boolean 
str_table_rbtree__foreach(DskStrTable   *str_table,
                          const char    *prefix,
                          DskStrTableForeachFind foreach,
                          void          *foreach_data)
{
  DskStrTableRbtree *rbtree = (DskStrTableRbtree *) table;
  unsigned prefix_length = strlen (prefix);
  DskStrTableRbtreeNode *at = rbtree->top;
  if (at)
    return foreach_recursive (prefix_length, prefix, at, foreach, foreach_data);
  else
    return DSK_FALSE;
}

DskStrTableInterface dsk_str_table_interface_rbtree =
{
  sizeof (DskStrTableRbtree),
  str_table_rbtree__init,
  str_table_rbtree__get,
  str_table_rbtree__get_prev,
  str_table_rbtree__get_next,
  str_table_rbtree__force,
  str_table_rbtree__force_noinit,
  str_table_rbtree__create,
  str_table_rbtree__remove,
  str_table_rbtree__create_or_remove,
  str_table_rbtree__create_or_remove_noinit,
  str_table_rbtree__set,
  str_table_rbtree__foreach
};
