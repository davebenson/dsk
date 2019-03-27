#ifndef __DSK_RBTREE_MACROS_H_
#define __DSK_RBTREE_MACROS_H_

/* Macros for construction of a red-black tree.
 * Like our other macro-based data-structures,
 * this doesn't allocate memory, and it doesn't
 * use any helper functions.
 *
 * It supports "invasive" tree structures,
 * for example, you can have nodes that are members
 * of two different trees.
 *
 * A rbtree is a tuple:
 *    top
 *    type*
 *    is_red
 *    set_is_red
 *    parent
 *    left
 *    right
 *    comparator
 * that should be defined by a macro like "GET_TREE()".
 * See tests/test-rbtree-macros.
 *
 * The methods are:
 *   INSERT(tree, node, collision_node) 
 *         Try to insert 'node' into tree.
 *         If an equivalent node exists,
 *         return it in collision_node, and do nothing else.
 *         Otherwise, node is added to the tree
 *         (and collision_node is set to NULL).
 *   INSERT_AT(tree, parent, is_right, new_node) 
 *         Insert node into tree at the appropriate location;
 *         call must ensure that the relevant child of 'parent' is NULL,
 *         and that the usual sorting invariants hold.
 *   REMOVE(tree, node)
 *         Remove a node from the tree.
 *   LOOKUP(tree, node, out)
 *         Find a node in the tree.
 *         Finds the node equivalent with 
 *         'node', and returns it in 'out',
 *         or sets out to NULL if no node exists.
 *   LOOKUP_COMPARATOR(tree, key, key_comparator, out)
 *         Find a node in the tree, based on a key
 *         which may not be in the same format as the node.
 *   FIRST(tree, out)
 *         Set 'out' to the first node in the tree.
 *   LAST(tree, out)
 *         Set 'out' to the last node in the tree.
 *   PREV(tree, cur, out)
 *         Set 'out' to the previous node in the tree before cur.
 *   NEXT(tree, cur, out)
 *         Set 'out' to the next node in the tree after cur.
 *   LOOKUP_OR_PREV_COMPARATOR(tree, key, key_comparator, out)
 *         Find the last node in the tree which is before or equal to 'key'.
 *   LOOKUP_OR_NEXT_COMPARATOR(tree, key, key_comparator, out)
 *         Find the first node in the tree which is after or equal to 'key'.
 *   LOOKUP_OR_PREV(tree, key, out)
 *         Find the last node in the tree which is before or equal to 'key'.
 *   LOOKUP_OR_NEXT(tree, key, out)
 *         Find the first node in the tree which is after or equal to 'key'.
 *   LOOKUP_PREV_COMPARATOR(tree, key, key_comparator, out)
 *         Find the last node in the tree which is before or equal to 'key'.
 *   LOOKUP_NEXT_COMPARATOR(tree, key, key_comparator, out)
 *         Find the first node in the tree which is after or equal to 'key'.
 *   LOOKUP_PREV(tree, key, out)
 *         Find the last node in the tree which is before or equal to 'key'.
 *   LOOKUP_NEXT(tree, key, out)
 *         Find the first node in the tree which is after or equal to 'key'.
 *   REPLACE_NODE(tree, old, replacement)
 *         Replace 'old' with 'replacement'; ensuring that the replacement
 *         is equal to key (or at least PREV(old) < replacement < NEXT(old).)
 *
 * XXX (cont): or better, rename LOOKUP_COMPARATOR to FIND and rename
 * LOOKUP_OR_PREV_COMPARATOR, LOOKUP_OR_NEXT_COMPARATOR to FIND_PREV_EQ, FIND_NEXT_EL resp.
 * Ditch LOOKUP_OR_PREV, LOOKUP_OR_NEXT (since it's trivial to pass the comparator in
 * in the worst case).
 *
 *
 * Occasionally, you may need the "RBCTREE", which has the is_red flag,
 * but also keeps a "count" field at every node telling the size of that subtree.
 * This then has all the methods of RBTREE, plus:
 *   GET_BY_INDEX(tree, n, out)
 *         Return the n-th element in the tree.
 *   GET_BY_INDEX_UNCHECKED(tree, n, out)
 *         Return the n-th element in the tree;
 *         n MUST be less than the number of
 *         nodes in the tree.
 *   GET_NODE_INDEX(tree, node, n_out)
 *        Sets n_out to the index of node in the tree.
 *   
 * An rbctree is a tuple:
 *    top
 *    type*
 *    is_red
 *    set_is_red
 *    get_count
 *    set_count
 *    parent
 *    left
 *    right
 *    comparator
 */

#define DSK_STD_GET_IS_RED(n)     n->is_red
#define DSK_STD_SET_IS_RED(n,v)   n->is_red = v

/*
 * By and large, the red-black tree algorithms used here are from
 * the classic text:
 *     _Introduction to Algorithms_. Thomas Cormen, Charles Leiserson,
 *     and Donald Rivest.  MIT Press.  1990.
 * Citations appears as Algorithms:300 indicating page 300 (for example).
 * The "rbctree" is my name for this idea (daveb),
 * which i suspect has been thought of and implemented before.
 */
#define DSK_RBTREE_INSERT(tree, node, collision_node)                         \
  DSK_RBTREE_INSERT_(tree, node, collision_node)
#define DSK_RBTREE_INSERT_AT(tree, parent, is_red, node)                      \
  DSK_RBTREE_INSERT_AT_(tree, parent, is_right, node)
#define DSK_RBTREE_REPLACE_NODE(tree, old, replacement)                       \
  DSK_RBTREE_REPLACE_NODE_(tree, old, replacement)
#define DSK_RBTREE_REMOVE(tree, node)                                         \
  DSK_RBTREE_REMOVE_(tree, node)
#define DSK_RBTREE_LOOKUP(tree, key, out)                                     \
  DSK_RBTREE_LOOKUP_(tree, key, out)
#define DSK_RBTREE_LOOKUP_COMPARATOR(tree, key, key_comparator, out)          \
  DSK_RBTREE_LOOKUP_COMPARATOR_(tree, key, key_comparator, out)

#define DSK_RBTREE_FIRST(tree, out)                                           \
  DSK_RBTREE_FIRST_(tree, out)
#define DSK_RBTREE_LAST(tree, out)                                            \
  DSK_RBTREE_LAST_(tree, out)
#define DSK_RBTREE_NEXT(tree, in, out)                                        \
  DSK_RBTREE_NEXT_(tree, in, out)
#define DSK_RBTREE_PREV(tree, in, out)                                        \
  DSK_RBTREE_PREV_(tree, in, out)

#define DSK_RBTREE_LOOKUP_OR_NEXT(tree, key, out)                                   \
  DSK_RBTREE_LOOKUP_OR_NEXT_(tree, key, out)
#define DSK_RBTREE_LOOKUP_OR_NEXT_COMPARATOR(tree, key, key_comparator, out)        \
  DSK_RBTREE_LOOKUP_OR_NEXT_COMPARATOR_(tree, key, key_comparator, out)
#define DSK_RBTREE_LOOKUP_OR_PREV(tree, key, out)                                    \
  DSK_RBTREE_LOOKUP_OR_PREV_(tree, key, out)
#define DSK_RBTREE_LOOKUP_OR_PREV_COMPARATOR(tree, key, key_comparator, out)         \
  DSK_RBTREE_LOOKUP_OR_PREV_COMPARATOR_(tree, key, key_comparator, out)

#if 1
#undef DSK_STMT_START
#define DSK_STMT_START do
#undef DSK_STMT_END
#define DSK_STMT_END while(0)
#endif

#define DSK_RBTREE_INSERT_(top,type,is_red,set_is_red,parent,left,right,comparator, node,collision_node) \
DSK_STMT_START{                                                                 \
  type _dsk_last = NULL;                                                      \
  type _dsk_at = (top);                                                       \
  int _dsk_last_was_left = 0;                                        \
  collision_node = NULL;                                                      \
  while (_dsk_at != NULL)                                                     \
    {                                                                         \
      int _dsk_compare_rv;                                                    \
      _dsk_last = _dsk_at;                                                    \
      comparator(_dsk_at, (node), _dsk_compare_rv);                           \
      if (_dsk_compare_rv > 0)                                                \
        {                                                                     \
          _dsk_last_was_left = 1;                                          \
          _dsk_at = _dsk_at->left;                                            \
        }                                                                     \
      else if (_dsk_compare_rv < 0)                                           \
        {                                                                     \
          _dsk_last_was_left = 0;                                         \
          _dsk_at = _dsk_at->right;                                           \
        }                                                                     \
      else                                                                    \
        break;                                                                \
   }                                                                          \
  if (_dsk_at != NULL)                                                        \
    {                                                                         \
      /* collision */                                                         \
      collision_node = _dsk_at;                                               \
    }                                                                         \
  else if (_dsk_last == NULL)                                                 \
    {                                                                         \
      /* only node in tree */                                                 \
      top = (node);                                                           \
      set_is_red ((node), 0);                                                 \
      (node)->left = (node)->right = (node)->parent = NULL;                   \
    }                                                                         \
  else                                                                        \
    {                                                                         \
      (node)->parent = _dsk_last;                                             \
      (node)->left = (node)->right = NULL;                                    \
      if (_dsk_last_was_left)                                                 \
        _dsk_last->left = (node);                                             \
      else                                                                    \
        _dsk_last->right = (node);                                            \
                                                                              \
      /* fixup */                                                             \
      _dsk_at = (node);                                                       \
      set_is_red (_dsk_at, 1);                                                \
      while (top != _dsk_at && is_red(_dsk_at->parent))                       \
        {                                                                     \
          if (_dsk_at->parent == _dsk_at->parent->parent->left)               \
            {                                                                 \
              type _dsk_y = _dsk_at->parent->parent->right;                   \
              if (_dsk_y != NULL && is_red (_dsk_y))                          \
                {                                                             \
                  set_is_red (_dsk_at->parent, 0);                            \
                  set_is_red (_dsk_y, 0);                                     \
                  set_is_red (_dsk_at->parent->parent, 1);                    \
                  _dsk_at = _dsk_at->parent->parent;                          \
                }                                                             \
              else                                                            \
                {                                                             \
                  if (_dsk_at == _dsk_at->parent->right)                      \
                    {                                                         \
                      _dsk_at = _dsk_at->parent;                              \
                      DSK_RBTREE_ROTATE_LEFT (top,type,parent,left,right, _dsk_at);\
                    }                                                         \
                  set_is_red(_dsk_at->parent, 0);                             \
                  set_is_red(_dsk_at->parent->parent, 1);                     \
                  DSK_RBTREE_ROTATE_RIGHT (top,type,parent,left,right,        \
                                           _dsk_at->parent->parent);          \
                }                                                             \
            }                                                                 \
          else                                                                \
            {                                                                 \
              type _dsk_y = _dsk_at->parent->parent->left;                    \
              if (_dsk_y != NULL && is_red (_dsk_y))                          \
                {                                                             \
                  set_is_red (_dsk_at->parent, 0);                            \
                  set_is_red (_dsk_y, 0);                                     \
                  set_is_red (_dsk_at->parent->parent, 1);                    \
                  _dsk_at = _dsk_at->parent->parent;                          \
                }                                                             \
              else                                                            \
                {                                                             \
                  if (_dsk_at == _dsk_at->parent->left)                       \
                    {                                                         \
                      _dsk_at = _dsk_at->parent;                              \
                      DSK_RBTREE_ROTATE_RIGHT (top,type,parent,left,right,    \
                                               _dsk_at);                      \
                    }                                                         \
                  set_is_red(_dsk_at->parent, 0);                             \
                  set_is_red(_dsk_at->parent->parent, 1);                     \
                  DSK_RBTREE_ROTATE_LEFT (top,type,parent,left,right,         \
                                          _dsk_at->parent->parent);           \
                }                                                             \
            }                                                                 \
        }                                                                     \
      set_is_red((top), 0);                                                   \
    }                                                                         \
}DSK_STMT_END

#define DSK_RBTREE_INSERT_AT_(top,type,is_red,set_is_red,parent,left,right,comparator, parent_node,is_right,node) \
DSK_STMT_START{                                                                 \
    {                                                                         \
      type _dsk_at;                                                           \
      (node)->parent = (parent_node);                                         \
      (node)->left = (node)->right = NULL;                                    \
      if (is_right)                                                           \
        (parent_node)->right = (node);                                        \
      else                                                                    \
        (parent_node)->left = (node);                                         \
                                                                              \
      /* fixup */                                                             \
      _dsk_at = (node);                                                       \
      set_is_red (_dsk_at, 1);                                                \
      while (top != _dsk_at && is_red(_dsk_at->parent))                       \
        {                                                                     \
          if (_dsk_at->parent == _dsk_at->parent->parent->left)               \
            {                                                                 \
              type _dsk_y = _dsk_at->parent->parent->right;                   \
              if (_dsk_y != NULL && is_red (_dsk_y))                          \
                {                                                             \
                  set_is_red (_dsk_at->parent, 0);                            \
                  set_is_red (_dsk_y, 0);                                     \
                  set_is_red (_dsk_at->parent->parent, 1);                    \
                  _dsk_at = _dsk_at->parent->parent;                          \
                }                                                             \
              else                                                            \
                {                                                             \
                  if (_dsk_at == _dsk_at->parent->right)                      \
                    {                                                         \
                      _dsk_at = _dsk_at->parent;                              \
                      DSK_RBTREE_ROTATE_LEFT (top,type,parent,left,right, _dsk_at);\
                    }                                                         \
                  set_is_red(_dsk_at->parent, 0);                             \
                  set_is_red(_dsk_at->parent->parent, 1);                     \
                  DSK_RBTREE_ROTATE_RIGHT (top,type,parent,left,right,        \
                                           _dsk_at->parent->parent);          \
                }                                                             \
            }                                                                 \
          else                                                                \
            {                                                                 \
              type _dsk_y = _dsk_at->parent->parent->left;                    \
              if (_dsk_y != NULL && is_red (_dsk_y))                          \
                {                                                             \
                  set_is_red (_dsk_at->parent, 0);                            \
                  set_is_red (_dsk_y, 0);                                     \
                  set_is_red (_dsk_at->parent->parent, 1);                    \
                  _dsk_at = _dsk_at->parent->parent;                          \
                }                                                             \
              else                                                            \
                {                                                             \
                  if (_dsk_at == _dsk_at->parent->left)                       \
                    {                                                         \
                      _dsk_at = _dsk_at->parent;                              \
                      DSK_RBTREE_ROTATE_RIGHT (top,type,parent,left,right,    \
                                               _dsk_at);                      \
                    }                                                         \
                  set_is_red(_dsk_at->parent, 0);                             \
                  set_is_red(_dsk_at->parent->parent, 1);                     \
                  DSK_RBTREE_ROTATE_LEFT (top,type,parent,left,right,         \
                                          _dsk_at->parent->parent);           \
                }                                                             \
            }                                                                 \
        }                                                                     \
      set_is_red((top), 0);                                                   \
    }                                                                         \
}DSK_STMT_END
#define DSK_RBTREE_REPLACE_NODE_(top,type,is_red,set_is_red,parent,left,right,comparator, old_node,replacement_node) \
DSK_STMT_START{                                                               \
    int _dsk_old_is_red = is_red (old_node);                                  \
    set_is_red (replacement_node, _dsk_old_is_red);                           \
    if ((old_node)->parent)                                                   \
      {                                                                       \
        if ((old_node)->parent->left == (old_node))                           \
          (old_node)->parent->left = replacement_node;                        \
        else                                                                  \
          (old_node)->parent->right = replacement_node;                       \
      }                                                                       \
    else                                                                      \
      top = replacement_node;                                                 \
    (replacement_node)->left = (old_node)->left;                              \
    (replacement_node)->right = (old_node)->right;                            \
    (replacement_node)->parent = (old_node)->parent;                          \
    if ((replacement_node)->left)                                             \
      (replacement_node)->left->parent = replacement_node;                    \
    if ((replacement_node)->right)                                            \
      (replacement_node)->right->parent = replacement_node;                   \
}DSK_STMT_END

#define DSK_RBTREE_REMOVE_(top,type,is_red,set_is_red,parent,left,right,comparator, node) \
/* Algorithms:273. */                                                         \
DSK_STMT_START{                                                                 \
  type _dsk_rb_del_z = (node);                                                \
  type _dsk_rb_del_x;                                                         \
  type _dsk_rb_del_y;                                                         \
  type _dsk_rb_del_nullpar = NULL;	/* Used to emulate sentinel nodes */  \
  int _dsk_rb_del_fixup;                                                 \
  if (_dsk_rb_del_z->left == NULL || _dsk_rb_del_z->right == NULL)            \
    _dsk_rb_del_y = _dsk_rb_del_z;                                            \
  else                                                                        \
    {                                                                         \
      DSK_RBTREE_NEXT_ (top,type,is_red,set_is_red,parent,left,right,comparator,\
                        _dsk_rb_del_z, _dsk_rb_del_y);                        \
    }                                                                         \
  _dsk_rb_del_x = _dsk_rb_del_y->left ? _dsk_rb_del_y->left                   \
                                            : _dsk_rb_del_y->right;           \
  if (_dsk_rb_del_x)                                                          \
    _dsk_rb_del_x->parent = _dsk_rb_del_y->parent;                            \
  else                                                                        \
    _dsk_rb_del_nullpar = _dsk_rb_del_y->parent;                              \
  if (!_dsk_rb_del_y->parent)                                                 \
    top = _dsk_rb_del_x;                                                      \
  else                                                                        \
    {                                                                         \
      if (_dsk_rb_del_y == _dsk_rb_del_y->parent->left)                       \
	_dsk_rb_del_y->parent->left = _dsk_rb_del_x;                          \
      else                                                                    \
	_dsk_rb_del_y->parent->right = _dsk_rb_del_x;                         \
    }                                                                         \
  _dsk_rb_del_fixup = !is_red(_dsk_rb_del_y);                                 \
  if (_dsk_rb_del_y != _dsk_rb_del_z)                                         \
    {                                                                         \
      set_is_red(_dsk_rb_del_y, is_red(_dsk_rb_del_z));                       \
      _dsk_rb_del_y->left = _dsk_rb_del_z->left;                              \
      _dsk_rb_del_y->right = _dsk_rb_del_z->right;                            \
      _dsk_rb_del_y->parent = _dsk_rb_del_z->parent;                          \
      if (_dsk_rb_del_y->parent)                                              \
	{                                                                     \
	  if (_dsk_rb_del_y->parent->left == _dsk_rb_del_z)                   \
	    _dsk_rb_del_y->parent->left = _dsk_rb_del_y;                      \
	  else                                                                \
	    _dsk_rb_del_y->parent->right = _dsk_rb_del_y;                     \
	}                                                                     \
      else                                                                    \
	top = _dsk_rb_del_y;                                                  \
                                                                              \
      if (_dsk_rb_del_y->left)                                                \
	_dsk_rb_del_y->left->parent = _dsk_rb_del_y;                          \
      if (_dsk_rb_del_y->right)                                               \
	_dsk_rb_del_y->right->parent = _dsk_rb_del_y;                         \
      if (_dsk_rb_del_nullpar == _dsk_rb_del_z)                               \
	_dsk_rb_del_nullpar = _dsk_rb_del_y;                                  \
    }                                                                         \
  if (_dsk_rb_del_fixup)                                                      \
    {                                                                         \
      /* delete fixup (Algorithms, p 274) */                                  \
      while (_dsk_rb_del_x != top                                             \
         && !(_dsk_rb_del_x != NULL && is_red (_dsk_rb_del_x)))               \
        {                                                                     \
          type _dsk_rb_del_xparent = _dsk_rb_del_x ? _dsk_rb_del_x->parent    \
                                                   : _dsk_rb_del_nullpar;     \
          if (_dsk_rb_del_x == _dsk_rb_del_xparent->left)                     \
            {                                                                 \
              type _dsk_rb_del_w = _dsk_rb_del_xparent->right;                \
              if (_dsk_rb_del_w != NULL && is_red (_dsk_rb_del_w))            \
                {                                                             \
                  set_is_red (_dsk_rb_del_w, 0);                              \
                  set_is_red (_dsk_rb_del_xparent, 1);                        \
                  DSK_RBTREE_ROTATE_LEFT (top,type,parent,left,right,         \
                                          _dsk_rb_del_xparent);               \
                  _dsk_rb_del_w = _dsk_rb_del_xparent->right;                 \
                }                                                             \
              if (!(_dsk_rb_del_w->left && is_red (_dsk_rb_del_w->left))      \
               && !(_dsk_rb_del_w->right && is_red (_dsk_rb_del_w->right)))   \
                {                                                             \
                  set_is_red (_dsk_rb_del_w, 1);                              \
                  _dsk_rb_del_x = _dsk_rb_del_xparent;                        \
                }                                                             \
              else                                                            \
                {                                                             \
                  if (!(_dsk_rb_del_w->right && is_red (_dsk_rb_del_w->right)))\
                    {                                                         \
                      if (_dsk_rb_del_w->left)                                \
                        set_is_red (_dsk_rb_del_w->left, 0);                  \
                      set_is_red (_dsk_rb_del_w, 1);                          \
                      DSK_RBTREE_ROTATE_RIGHT (top,type,parent,left,right,    \
                                               _dsk_rb_del_w);                \
                      _dsk_rb_del_w = _dsk_rb_del_xparent->right;             \
                    }                                                         \
                  set_is_red (_dsk_rb_del_w, is_red (_dsk_rb_del_xparent));   \
                  set_is_red (_dsk_rb_del_xparent, 0);                        \
                  set_is_red (_dsk_rb_del_w->right, 0);                       \
                  DSK_RBTREE_ROTATE_LEFT (top,type,parent,left,right,         \
                                          _dsk_rb_del_xparent);               \
                  _dsk_rb_del_x = top;                                        \
                }                                                             \
            }                                                                 \
          else                                                                \
            {                                                                 \
              type _dsk_rb_del_w = _dsk_rb_del_xparent->left;                 \
              if (_dsk_rb_del_w && is_red (_dsk_rb_del_w))                    \
                {                                                             \
                  set_is_red (_dsk_rb_del_w, 0);                              \
                  set_is_red (_dsk_rb_del_xparent, 1);                        \
                  DSK_RBTREE_ROTATE_RIGHT (top,type,parent,left,right,        \
                                           _dsk_rb_del_xparent);              \
                  _dsk_rb_del_w = _dsk_rb_del_xparent->left;                  \
                }                                                             \
              if (!(_dsk_rb_del_w->right && is_red (_dsk_rb_del_w->right))    \
               && !(_dsk_rb_del_w->left && is_red (_dsk_rb_del_w->left)))     \
                {                                                             \
                  set_is_red (_dsk_rb_del_w, 1);                              \
                  _dsk_rb_del_x = _dsk_rb_del_xparent;                        \
                }                                                             \
              else                                                            \
                {                                                             \
                  if (!(_dsk_rb_del_w->left && is_red (_dsk_rb_del_w->left))) \
                    {                                                         \
                      set_is_red (_dsk_rb_del_w->right, 0);                   \
                      set_is_red (_dsk_rb_del_w, 1);                          \
                      DSK_RBTREE_ROTATE_LEFT (top,type,parent,left,right,     \
                                              _dsk_rb_del_w);                 \
                      _dsk_rb_del_w = _dsk_rb_del_xparent->left;              \
                    }                                                         \
                  set_is_red (_dsk_rb_del_w, is_red (_dsk_rb_del_xparent));   \
                  set_is_red (_dsk_rb_del_xparent, 0);                        \
                  if (_dsk_rb_del_w->left)                                    \
                    set_is_red (_dsk_rb_del_w->left, 0);                      \
                  DSK_RBTREE_ROTATE_RIGHT (top,type,parent,left,right,        \
                                           _dsk_rb_del_xparent);              \
                  _dsk_rb_del_x = top;                                        \
                }                                                             \
            }                                                                 \
        }                                                                     \
      if (_dsk_rb_del_x != NULL)                                              \
        set_is_red(_dsk_rb_del_x, 0);                                         \
    }                                                                         \
  _dsk_rb_del_z->left = NULL;                                                 \
  _dsk_rb_del_z->right = NULL;                                                \
  _dsk_rb_del_z->parent = NULL;                                               \
}DSK_STMT_END

#define DSK_RBTREE_LOOKUP_COMPARATOR_(top,type,is_red,set_is_red,parent,left,right,comparator, \
                                      key,key_comparator,out)                 \
DSK_STMT_START{                                                                 \
  type _dsk_lookup_at = (top);                                                \
  while (_dsk_lookup_at)                                                      \
    {                                                                         \
      int _dsk_compare_rv;                                                    \
      key_comparator((key),_dsk_lookup_at,_dsk_compare_rv);                     \
      if (_dsk_compare_rv < 0)                                                \
        _dsk_lookup_at = _dsk_lookup_at->left;                                \
      else if (_dsk_compare_rv > 0)                                           \
        _dsk_lookup_at = _dsk_lookup_at->right;                               \
      else                                                                    \
        break;                                                                \
    }                                                                         \
  out = _dsk_lookup_at;                                                       \
}DSK_STMT_END
 /* see comments for 'LOOKUP_OR_NEXT'; it is the same with the sense of the comparators
  * and left,right reversed. */
#define DSK_RBTREE_LOOKUP_OR_PREV_COMPARATOR_(top,type,is_red,set_is_red,parent,left,right,comparator, \
                                      key,key_comparator,out)                 \
DSK_STMT_START{                                                                 \
  type _dsk_lookup_at = (top);                                                \
  type _dsk_lookup_rv = NULL;                                                 \
  while (_dsk_lookup_at)                                                      \
    {                                                                         \
      int _dsk_compare_rv;                                                    \
      key_comparator((key),_dsk_lookup_at,_dsk_compare_rv);                     \
      if (_dsk_compare_rv >= 0)                                               \
        {                                                                     \
          _dsk_lookup_rv = _dsk_lookup_at;                                    \
          _dsk_lookup_at = _dsk_lookup_at->right;                             \
        }                                                                     \
      else                                                                    \
        _dsk_lookup_at = _dsk_lookup_at->left;                                \
    }                                                                         \
  out = _dsk_lookup_rv;                                                       \
}DSK_STMT_END
/* see introductory comments for a less mathematical
 * definition.  but what 'supremum' computes is:
 * sup(tree, key) = min S(tree,key) or NULL if S(tree, key)
 * is empty, where S(tree,key) = { t | t \in tree && t >= key }.
 * The 'min' is determined by the 'comparator',
 * and the 't >= key' is determined by the 'key_comparator'.
 * But they must be consistent.
 *
 * so, here's a recursive description.  suppose we wish to compute
 * the supremum sup(a, key), where 'a' is the node in the tree shown:
 *                    a       
 *                   / \      
 *                  b   c     
 * Is 'a >= key'?  Then 'a' is in S(a, key),                
 * and hence sup(a,key) exists.  But a "better" supremum,   
 * in terms of being the 'min' in the tree,                 
 * may lie in 'b'. Nothing better may lie in 'c', since it
 * is larger, and we are computing a minimum of S.
 * 
 * But if 'a < key', then 'a' is not in S.  hence 'b' and its children
 * are not in S.  Hence sup(a) = sup(c), including the possibility that
 * sup(C) = NULL.
 *
 * Therefore,
 *    
 *              sup(b)     if 'a >= key', and sub(b) exists,         [case0]
 *     sup(a) = a          if 'a >= key', and sub(b) does not exist, [case1]
 *              sup(c)     if 'a < key' and sub(c) exists,           [case2]
 *              NULL       if 'a < key' and sub(c) does not exist.   [case3]
 *
 * the non-recursive algo follows once you realize that it's just
 * a tree descent, keeping track of the best candidate you've found.
 * TODO: there's got to be a better way to describe it.
 */
#define DSK_RBTREE_LOOKUP_OR_NEXT_COMPARATOR_(top,type,is_red,set_is_red,parent,left,right,comparator, \
                                      key,key_comparator,out)                 \
DSK_STMT_START{                                                                 \
  type _dsk_lookup_at = (top);                                                \
  type _dsk_lookup_rv = NULL;                                                 \
  while (_dsk_lookup_at)                                                      \
    {                                                                         \
      int _dsk_compare_rv;                                                    \
      key_comparator((key),_dsk_lookup_at,_dsk_compare_rv);                     \
      if (_dsk_compare_rv <= 0)                                               \
        {                                                                     \
          _dsk_lookup_rv = _dsk_lookup_at;                                    \
          _dsk_lookup_at = _dsk_lookup_at->left;                              \
        }                                                                     \
      else                                                                    \
        _dsk_lookup_at = _dsk_lookup_at->right;                               \
    }                                                                         \
  out = _dsk_lookup_rv;                                                       \
}DSK_STMT_END
#define DSK_RBTREE_LOOKUP_(top,type,is_red,set_is_red,parent,left,right,comparator, key,out) \
  DSK_RBTREE_LOOKUP_COMPARATOR_(top,type,is_red,set_is_red,parent,left,right,comparator, key,comparator,out)
#define DSK_RBTREE_LOOKUP_OR_NEXT_(top,type,is_red,set_is_red,parent,left,right,comparator, key,out) \
  DSK_RBTREE_LOOKUP_OR_NEXT_COMPARATOR_(top,type,is_red,set_is_red,parent,left,right,comparator, key,comparator,out)
#define DSK_RBTREE_LOOKUP_OR_PREV_(top,type,is_red,set_is_red,parent,left,right,comparator, key,out) \
  DSK_RBTREE_LOOKUP_OR_PREV_COMPARATOR_(top,type,is_red,set_is_red,parent,left,right,comparator, key,comparator,out)

/* these macros don't need the is_red/set_is_red macros, nor the comparator,
   so omit them, to keep the lines a bit shorter. */
#define DSK_RBTREE_ROTATE_RIGHT(top,type,parent,left,right, node)             \
  DSK_RBTREE_ROTATE_LEFT(top,type,parent,right,left, node)
#define DSK_RBTREE_ROTATE_LEFT(top,type,parent,left,right, node)              \
DSK_STMT_START{                                                                 \
  type _dsk_rot_x = (node);                                                   \
  type _dsk_rot_y = _dsk_rot_x->right;                                        \
                                                                              \
  _dsk_rot_x->right = _dsk_rot_y->left;                                       \
  if (_dsk_rot_y->left)                                                       \
    _dsk_rot_y->left->parent = _dsk_rot_x;                                    \
  _dsk_rot_y->parent = _dsk_rot_x->parent;                                    \
  if (_dsk_rot_x->parent == NULL)                                             \
    top = _dsk_rot_y;                                                         \
  else if (_dsk_rot_x == _dsk_rot_x->parent->left)                            \
    _dsk_rot_x->parent->left = _dsk_rot_y;                                    \
  else                                                                        \
    _dsk_rot_x->parent->right = _dsk_rot_y;                                   \
  _dsk_rot_y->left = _dsk_rot_x;                                              \
  _dsk_rot_x->parent = _dsk_rot_y;                                            \
}DSK_STMT_END

/* iteration */
#define DSK_RBTREE_NEXT_(top,type,is_red,set_is_red,parent,left,right,comparator, in, out)  \
DSK_STMT_START{                                                                 \
  type _dsk_next_at = (in);                                                   \
  /*g_assert (_dsk_next_at != NULL);*/                                        \
  if (_dsk_next_at->right != NULL)                                            \
    {                                                                         \
      _dsk_next_at = _dsk_next_at->right;                                     \
      while (_dsk_next_at->left != NULL)                                      \
        _dsk_next_at = _dsk_next_at->left;                                    \
      out = _dsk_next_at;                                                     \
    }                                                                         \
  else                                                                        \
    {                                                                         \
      type _dsk_next_parent = (in)->parent;                                   \
      while (_dsk_next_parent != NULL                                         \
          && _dsk_next_at == _dsk_next_parent->right)                         \
        {                                                                     \
          _dsk_next_at = _dsk_next_parent;                                    \
          _dsk_next_parent = _dsk_next_parent->parent;                        \
        }                                                                     \
      out = _dsk_next_parent;                                                 \
    }                                                                         \
}DSK_STMT_END

/* prev is just next with left/right child reversed. */
#define DSK_RBTREE_PREV_(top,type,is_red,set_is_red,parent,left,right,comparator, in, out)  \
  DSK_RBTREE_NEXT_(top,type,is_red,set_is_red,parent,right,left,comparator, in, out)

#define DSK_RBTREE_FIRST_(top,type,is_red,set_is_red,parent,left,right,comparator, out)  \
DSK_STMT_START{                                                                 \
  type _dsk_first_at = (top);                                                 \
  if (_dsk_first_at != NULL)                                                  \
    while (_dsk_first_at->left != NULL)                                       \
      _dsk_first_at = _dsk_first_at->left;                                    \
  out = _dsk_first_at;                                                        \
}DSK_STMT_END
#define DSK_RBTREE_LAST_(top,type,is_red,set_is_red,parent,left,right,comparator, out)  \
  DSK_RBTREE_FIRST_(top,type,is_red,set_is_red,parent,right,left,comparator, out)

 /* --- RBC-Tree --- */
#define DSK_RBCTREE_INSERT(tree, node, collision_node)                         \
  DSK_RBCTREE_INSERT_(tree, node, collision_node)
#define DSK_RBCTREE_REMOVE(tree, node)                                         \
  DSK_RBCTREE_REMOVE_(tree, node)
#define DSK_RBCTREE_LOOKUP(tree, key, out)                                     \
  DSK_RBCTREE_LOOKUP_(tree, key, out)
#define DSK_RBCTREE_LOOKUP_COMPARATOR(tree, key, key_comparator, out)          \
  DSK_RBCTREE_LOOKUP_COMPARATOR_(tree, key, key_comparator, out)

#define DSK_RBCTREE_FIRST(tree, out)                                           \
  DSK_RBCTREE_FIRST_(tree, out)
#define DSK_RBCTREE_LAST(tree, out)                                            \
  DSK_RBCTREE_LAST_(tree, out)
#define DSK_RBCTREE_NEXT(tree, in, out)                                        \
  DSK_RBCTREE_NEXT_(tree, in, out)
#define DSK_RBCTREE_PREV(tree, in, out)                                        \
  DSK_RBCTREE_PREV_(tree, in, out)

#define DSK_RBCTREE_LOOKUP_OR_NEXT(tree, key, out)                                   \
  DSK_RBCTREE_LOOKUP_OR_NEXT_(tree, key, out)
#define DSK_RBCTREE_LOOKUP_OR_NEXT_COMPARATOR(tree, key, key_comparator, out)        \
  DSK_RBCTREE_LOOKUP_OR_NEXT_COMPARATOR_(tree, key, key_comparator, out)
#define DSK_RBCTREE_LOOKUP_OR_PREV(tree, key, out)                                    \
  DSK_RBCTREE_LOOKUP_OR_PREV_(tree, key, out)
#define DSK_RBCTREE_LOOKUP_OR_PREV_COMPARATOR(tree, key, key_comparator, out)         \
  DSK_RBCTREE_LOOKUP_OR_PREV_COMPARATOR_(tree, key, key_comparator, out)

#define DSK_RBCTREE_GET_BY_INDEX(tree, index, out)                             \
  DSK_RBCTREE_GET_BY_INDEX_(tree, index, out)
#define DSK_RBCTREE_GET_BY_INDEX_UNCHECKED(tree, index, out)                   \
  DSK_RBCTREE_GET_BY_INDEX_UNCHECKED_(tree, index, out)
#define DSK_RBCTREE_GET_NODE_INDEX(tree, node, index_out)                      \
  DSK_RBCTREE_GET_NODE_INDEX_(tree, node, index_out)

#if 1
#undef DSK_STMT_START
#define DSK_STMT_START do
#undef DSK_STMT_END
#define DSK_STMT_END while(0)
#endif


#define DSK_RBCTREE_INSERT_(top,type,is_red,set_is_red,get_count,set_count,parent,left,right,comparator, node,collision_node) \
DSK_STMT_START{                                                                 \
  type _dsk_last = NULL;                                                      \
  type _dsk_at = (top);                                                       \
  int _dsk_last_was_left = 0;                                        \
  collision_node = NULL;                                                      \
  while (_dsk_at != NULL)                                                     \
    {                                                                         \
      int _dsk_compare_rv;                                                    \
      _dsk_last = _dsk_at;                                                    \
      comparator(_dsk_at, (node), _dsk_compare_rv);                           \
      if (_dsk_compare_rv > 0)                                                \
        {                                                                     \
          _dsk_last_was_left = 1;                                          \
          _dsk_at = _dsk_at->left;                                            \
        }                                                                     \
      else if (_dsk_compare_rv < 0)                                           \
        {                                                                     \
          _dsk_last_was_left = 0;                                         \
          _dsk_at = _dsk_at->right;                                           \
        }                                                                     \
      else                                                                    \
        break;                                                                \
   }                                                                          \
  if (_dsk_at != NULL)                                                        \
    {                                                                         \
      /* collision */                                                         \
      collision_node = _dsk_at;                                               \
    }                                                                         \
  else if (_dsk_last == NULL)                                                 \
    {                                                                         \
      /* only node in tree */                                                 \
      top = (node);                                                           \
      set_is_red ((node), 0);                                                 \
      (node)->left = (node)->right = (node)->parent = NULL;                   \
      set_count ((node), 1);                                                  \
    }                                                                         \
  else                                                                        \
    {                                                                         \
      (node)->parent = _dsk_last;                                             \
      (node)->left = (node)->right = NULL;                                    \
      if (_dsk_last_was_left)                                                 \
        _dsk_last->left = (node);                                             \
      else                                                                    \
        _dsk_last->right = (node);                                            \
                                                                              \
      /* fixup counts */                                                      \
      set_count ((node), 1);                                                  \
      for (_dsk_at = _dsk_last; _dsk_at; _dsk_at = _dsk_at->parent)           \
        {                                                                     \
          unsigned _dsk_new_count = get_count(_dsk_at) + 1;                   \
          set_count(_dsk_at, _dsk_new_count);                                 \
        }                                                                     \
                                                                              \
      /* fixup */                                                             \
      _dsk_at = (node);                                                       \
      set_is_red (_dsk_at, 1);                                                \
      while (_dsk_at->parent != NULL && is_red(_dsk_at->parent))              \
        {                                                                     \
          if (_dsk_at->parent == _dsk_at->parent->parent->left)               \
            {                                                                 \
              type _dsk_y = _dsk_at->parent->parent->right;                   \
              if (_dsk_y != NULL && is_red (_dsk_y))                          \
                {                                                             \
                  set_is_red (_dsk_at->parent, 0);                            \
                  set_is_red (_dsk_y, 0);                                     \
                  set_is_red (_dsk_at->parent->parent, 1);                    \
                  _dsk_at = _dsk_at->parent->parent;                          \
                }                                                             \
              else                                                            \
                {                                                             \
                  if (_dsk_at == _dsk_at->parent->right)                      \
                    {                                                         \
                      _dsk_at = _dsk_at->parent;                              \
                      DSK_RBCTREE_ROTATE_LEFT (top,type,parent,left,right,get_count,set_count, _dsk_at);\
                    }                                                         \
                  set_is_red(_dsk_at->parent, 0);                             \
                  set_is_red(_dsk_at->parent->parent, 1);                     \
                  DSK_RBCTREE_ROTATE_RIGHT (top,type,parent,left,right,get_count,set_count,\
                                         _dsk_at->parent->parent);            \
                }                                                             \
            }                                                                 \
          else                                                                \
            {                                                                 \
              type _dsk_y = _dsk_at->parent->parent->left;                    \
              if (_dsk_y != NULL && is_red (_dsk_y))                          \
                {                                                             \
                  set_is_red (_dsk_at->parent, 0);                            \
                  set_is_red (_dsk_y, 0);                                     \
                  set_is_red (_dsk_at->parent->parent, 1);                    \
                  _dsk_at = _dsk_at->parent->parent;                          \
                }                                                             \
              else                                                            \
                {                                                             \
                  if (_dsk_at == _dsk_at->parent->left)                       \
                    {                                                         \
                      _dsk_at = _dsk_at->parent;                              \
                      DSK_RBCTREE_ROTATE_RIGHT (top,type,parent,left,right,get_count,set_count,\
                                             _dsk_at);                        \
                    }                                                         \
                  set_is_red(_dsk_at->parent, 0);                             \
                  set_is_red(_dsk_at->parent->parent, 1);                     \
                  DSK_RBCTREE_ROTATE_LEFT (top,type,parent,left,right,get_count,set_count,\
                                          _dsk_at->parent->parent);           \
                }                                                             \
            }                                                                 \
        }                                                                     \
      set_is_red((top), 0);                                                   \
    }                                                                         \
}DSK_STMT_END

#define DSK_RBCTREE_REMOVE_(top,type,is_red,set_is_red,get_count,set_count,parent,left,right,comparator, node) \
/* Algorithms:273. */                                                         \
DSK_STMT_START{                                                                 \
  type _dsk_rb_del_z = (node);                                                \
  type _dsk_rb_del_x;                                                         \
  type _dsk_rb_del_y;                                                         \
  type _dsk_rb_del_nullpar = NULL;	/* Used to emulate sentinel nodes */  \
  int _dsk_rb_del_fixup;                                                 \
  if (_dsk_rb_del_z->left == NULL || _dsk_rb_del_z->right == NULL)            \
    _dsk_rb_del_y = _dsk_rb_del_z;                                            \
  else                                                                        \
    {                                                                         \
      DSK_RBTREE_NEXT_ (top,type,is_red,set_is_red,parent,left,right,comparator,\
                        _dsk_rb_del_z, _dsk_rb_del_y);                        \
    }                                                                         \
  _dsk_rb_del_x = _dsk_rb_del_y->left ? _dsk_rb_del_y->left                   \
                                            : _dsk_rb_del_y->right;           \
  if (_dsk_rb_del_x)                                                          \
    _dsk_rb_del_x->parent = _dsk_rb_del_y->parent;                            \
  else                                                                        \
    _dsk_rb_del_nullpar = _dsk_rb_del_y->parent;                              \
  if (!_dsk_rb_del_y->parent)                                                 \
    top = _dsk_rb_del_x;                                                      \
  else                                                                        \
    {                                                                         \
      if (_dsk_rb_del_y == _dsk_rb_del_y->parent->left)                       \
	_dsk_rb_del_y->parent->left = _dsk_rb_del_x;                          \
      else                                                                    \
	_dsk_rb_del_y->parent->right = _dsk_rb_del_x;                         \
      _DSK_RBCTREE_FIX_COUNT_AND_UP(type,parent,left,right,get_count,set_count, _dsk_rb_del_y->parent);\
    }                                                                         \
  _dsk_rb_del_fixup = !is_red(_dsk_rb_del_y);                                 \
  if (_dsk_rb_del_y != _dsk_rb_del_z)                                         \
    {                                                                         \
      set_is_red(_dsk_rb_del_y, is_red(_dsk_rb_del_z));                       \
      _dsk_rb_del_y->left = _dsk_rb_del_z->left;                              \
      _dsk_rb_del_y->right = _dsk_rb_del_z->right;                            \
      _dsk_rb_del_y->parent = _dsk_rb_del_z->parent;                          \
      if (_dsk_rb_del_y->parent)                                              \
	{                                                                     \
	  if (_dsk_rb_del_y->parent->left == _dsk_rb_del_z)                   \
	    _dsk_rb_del_y->parent->left = _dsk_rb_del_y;                      \
	  else                                                                \
	    _dsk_rb_del_y->parent->right = _dsk_rb_del_y;                     \
	}                                                                     \
      else                                                                    \
        {                                                                     \
          top = _dsk_rb_del_y;                                                \
        }                                                                     \
      /* TODO: look at pictures to see if "_AND_UP" is necessary */           \
      _DSK_RBCTREE_FIX_COUNT_AND_UP(type,parent,left,right,get_count,set_count, _dsk_rb_del_y);\
                                                                              \
      if (_dsk_rb_del_y->left)                                                \
	_dsk_rb_del_y->left->parent = _dsk_rb_del_y;                          \
      if (_dsk_rb_del_y->right)                                               \
	_dsk_rb_del_y->right->parent = _dsk_rb_del_y;                         \
      if (_dsk_rb_del_nullpar == _dsk_rb_del_z)                               \
	_dsk_rb_del_nullpar = _dsk_rb_del_y;                                  \
    }                                                                         \
  if (_dsk_rb_del_fixup)                                                      \
    {                                                                         \
      /* delete fixup (Algorithms, p 274) */                                  \
      while (_dsk_rb_del_x != top                                             \
         && !(_dsk_rb_del_x != NULL && is_red (_dsk_rb_del_x)))               \
        {                                                                     \
          type _dsk_rb_del_xparent = _dsk_rb_del_x ? _dsk_rb_del_x->parent    \
                                                   : _dsk_rb_del_nullpar;     \
          if (_dsk_rb_del_x == _dsk_rb_del_xparent->left)                     \
            {                                                                 \
              type _dsk_rb_del_w = _dsk_rb_del_xparent->right;                \
              if (_dsk_rb_del_w != NULL && is_red (_dsk_rb_del_w))            \
                {                                                             \
                  set_is_red (_dsk_rb_del_w, 0);                              \
                  set_is_red (_dsk_rb_del_xparent, 1);                        \
                  DSK_RBCTREE_ROTATE_LEFT (top,type,parent,left,right,get_count,set_count, \
                                          _dsk_rb_del_xparent);               \
                  _dsk_rb_del_w = _dsk_rb_del_xparent->right;                 \
                }                                                             \
              if (!(_dsk_rb_del_w->left && is_red (_dsk_rb_del_w->left))      \
               && !(_dsk_rb_del_w->right && is_red (_dsk_rb_del_w->right)))   \
                {                                                             \
                  set_is_red (_dsk_rb_del_w, 1);                              \
                  _dsk_rb_del_x = _dsk_rb_del_xparent;                        \
                }                                                             \
              else                                                            \
                {                                                             \
                  if (!(_dsk_rb_del_w->right && is_red (_dsk_rb_del_w->right)))\
                    {                                                         \
                      if (_dsk_rb_del_w->left)                                \
                        set_is_red (_dsk_rb_del_w->left, 0);                  \
                      set_is_red (_dsk_rb_del_w, 1);                          \
                      DSK_RBCTREE_ROTATE_RIGHT (top,type,parent,left,right,get_count,set_count, \
                                               _dsk_rb_del_w);                \
                      _dsk_rb_del_w = _dsk_rb_del_xparent->right;             \
                    }                                                         \
                  set_is_red (_dsk_rb_del_w, is_red (_dsk_rb_del_xparent));   \
                  set_is_red (_dsk_rb_del_xparent, 0);                        \
                  set_is_red (_dsk_rb_del_w->right, 0);                       \
                  DSK_RBCTREE_ROTATE_LEFT (top,type,parent,left,right,get_count,set_count, \
                                          _dsk_rb_del_xparent);               \
                  _dsk_rb_del_x = top;                                        \
                }                                                             \
            }                                                                 \
          else                                                                \
            {                                                                 \
              type _dsk_rb_del_w = _dsk_rb_del_xparent->left;                 \
              if (_dsk_rb_del_w && is_red (_dsk_rb_del_w))                    \
                {                                                             \
                  set_is_red (_dsk_rb_del_w, 0);                              \
                  set_is_red (_dsk_rb_del_xparent, 1);                        \
                  DSK_RBCTREE_ROTATE_RIGHT (top,type,parent,left,right,get_count,set_count, \
                                           _dsk_rb_del_xparent);              \
                  _dsk_rb_del_w = _dsk_rb_del_xparent->left;                  \
                }                                                             \
              if (!(_dsk_rb_del_w->right && is_red (_dsk_rb_del_w->right))    \
               && !(_dsk_rb_del_w->left && is_red (_dsk_rb_del_w->left)))     \
                {                                                             \
                  set_is_red (_dsk_rb_del_w, 1);                              \
                  _dsk_rb_del_x = _dsk_rb_del_xparent;                        \
                }                                                             \
              else                                                            \
                {                                                             \
                  if (!(_dsk_rb_del_w->left && is_red (_dsk_rb_del_w->left))) \
                    {                                                         \
                      set_is_red (_dsk_rb_del_w->right, 0);                   \
                      set_is_red (_dsk_rb_del_w, 1);                          \
                      DSK_RBCTREE_ROTATE_LEFT (top,type,parent,left,right,get_count,set_count, \
                                              _dsk_rb_del_w);                 \
                      _dsk_rb_del_w = _dsk_rb_del_xparent->left;              \
                    }                                                         \
                  set_is_red (_dsk_rb_del_w, is_red (_dsk_rb_del_xparent));   \
                  set_is_red (_dsk_rb_del_xparent, 0);                        \
                  if (_dsk_rb_del_w->left)                                    \
                    set_is_red (_dsk_rb_del_w->left, 0);                      \
                  DSK_RBCTREE_ROTATE_RIGHT (top,type,parent,left,right,get_count,set_count, \
                                           _dsk_rb_del_xparent);              \
                  _dsk_rb_del_x = top;                                        \
                }                                                             \
            }                                                                 \
        }                                                                     \
      if (_dsk_rb_del_x != NULL)                                              \
        set_is_red(_dsk_rb_del_x, 0);                                         \
    }                                                                         \
  _dsk_rb_del_z->left = NULL;                                                 \
  _dsk_rb_del_z->right = NULL;                                                \
  _dsk_rb_del_z->parent = NULL;                                               \
}DSK_STMT_END

#define DSK_RBCTREE_LOOKUP_COMPARATOR_(top,type,is_red,set_is_red,get_count,set_count,parent,left,right,comparator, \
                                      key,key_comparator,out)                 \
 DSK_RBTREE_LOOKUP_COMPARATOR_(top,type,is_red,set_count,parent,left,right,comparator, key,key_comparator,out)
#define DSK_RBCTREE_LOOKUP_OR_PREV_COMPARATOR_(top,type,is_red,set_is_red,get_count,set_count,parent,left,right,comparator, \
                                      key,key_comparator,out)                 \
 DSK_RBTREE_LOOKUP_OR_PREV_COMPARATOR_(top,type,is_red,set_is_red,parent,left,right,comparator, key,key_comparator,out)
#define DSK_RBCTREE_LOOKUP_OR_NEXT_COMPARATOR_(top,type,is_red,set_is_red,get_count,set_count,parent,left,right,comparator, \
                                      key,key_comparator,out)                 \
 DSK_RBTREE_LOOKUP_OR_NEXT_COMPARATOR_(top,type,is_red,set_is_red,parent,left,right,comparator, key,key_comparator,out)
#define DSK_RBCTREE_LOOKUP_(top,type,is_red,set_is_red,get_count,set_count,parent,left,right,comparator, key,out) \
  DSK_RBTREE_LOOKUP_COMPARATOR_(top,type,is_red,set_is_red,parent,left,right,comparator, key,comparator,out)
#define DSK_RBCTREE_LOOKUP_OR_NEXT_(top,type,is_red,set_is_red,get_count,set_count,parent,left,right,comparator, key,out) \
  DSK_RBTREE_LOOKUP_OR_NEXT_COMPARATOR_(top,type,is_red,set_is_red,parent,left,right,comparator, key,comparator,out)
#define DSK_RBCTREE_LOOKUP_OR_PREV_(top,type,is_red,set_is_red,get_count,set_count,parent,left,right,comparator, key,out) \
  DSK_RBTREE_LOOKUP_OR_PREV_COMPARATOR_(top,type,is_red,set_is_red,parent,left,right,comparator, key,comparator,out)
#define DSK_RBCTREE_NEXT_(top,type,is_red,set_is_red,get_count,set_count,parent,left,right,comparator, in, out)  \
   DSK_RBTREE_NEXT_(top,type,is_red,set_is_red,parent,left,right,comparator, in, out)
#define DSK_RBCTREE_PREV_(top,type,is_red,set_is_red,parent,left,right,comparator, in, out)  \
   DSK_RBTREE_PREV_(top,type,is_red,set_is_red,parent,left,right,comparator, in, out)
#define DSK_RBCTREE_FIRST_(top,type,is_red,set_is_red,get_count,set_count,parent,left,right,comparator, out)  \
  DSK_RBTREE_FIRST_(top,type,is_red,set_is_red,parent,left,right,comparator, out)
#define DSK_RBCTREE_LAST_(top,type,is_red,set_is_red,get_count,set_count,parent,left,right,comparator, out)  \
  DSK_RBTREE_LAST_(top,type,is_red,set_is_red,parent,left,right,comparator, out)

#define DSK_RBCTREE_GET_BY_INDEX_(top,type,is_red,set_is_red,get_count,set_count,parent,left,right,comparator, index,out) \
  DSK_STMT_START{                                                                \
    if (top == NULL || (index) >= get_count(top))                              \
      out = NULL;                                                              \
    else                                                                       \
      DSK_RBCTREE_GET_BY_INDEX_UNCHECKED_(top,type,is_red,set_is_red,get_count,set_count,parent,left,right,comparator, index,out);\
  }DSK_STMT_END
#define DSK_RBCTREE_GET_BY_INDEX_UNCHECKED_(top,type,is_red,set_is_red,get_count,set_count,parent,left,right,comparator, index,out) \
   DSK_STMT_START{                                                               \
     type _dsk_at = (top);                                                     \
     size_t _dsk_index = (index);                                              \
     for (;;)                                                                  \
       {                                                                       \
         size_t _dsk_left_size = _dsk_at->left ? get_count(_dsk_at->left) : 0; \
         if (_dsk_index < _dsk_left_size)                                      \
           _dsk_at = _dsk_at->left;                                            \
         else if (_dsk_index == _dsk_left_size)                                \
           break;                                                              \
         else                                                                  \
           {                                                                   \
             _dsk_index -= (_dsk_left_size + 1);                               \
             _dsk_at = _dsk_at->right;                                         \
           }                                                                   \
       }                                                                       \
     out = _dsk_at;                                                            \
   }DSK_STMT_END

#define DSK_RBCTREE_GET_NODE_INDEX_(top,type,is_red,set_is_red,get_count,set_count,parent,left,right,comparator, node,index_out) \
   DSK_STMT_START{                                                               \
     type _dsk_at = (node);                                                    \
     size_t _dsk_rv = _dsk_at->left ? get_count (_dsk_at->left) : 0;           \
     while (_dsk_at->parent != NULL)                                           \
       {                                                                       \
         if (_dsk_at->parent->left == _dsk_at)                                 \
           ;                                                                   \
         else                                                                  \
           {                                                                   \
             if (_dsk_at->parent->left)                                        \
               _dsk_rv += get_count((_dsk_at->parent->left));                  \
             _dsk_rv += 1;                                                     \
           }                                                                   \
         _dsk_at = _dsk_at->parent;                                            \
       }                                                                       \
     index_out = _dsk_rv;                                                      \
   }DSK_STMT_END

#define DSK_RBCTREE_ROTATE_RIGHT(top,type,parent,left,right,get_count,set_count, node) \
  DSK_RBCTREE_ROTATE_LEFT(top,type,parent,right,left,get_count,set_count, node)
#define DSK_RBCTREE_ROTATE_LEFT(top,type,parent,left,right,get_count,set_count, node)  \
DSK_STMT_START{                                                                 \
  type _dsk_rot_x = (node);                                                   \
  type _dsk_rot_y = _dsk_rot_x->right;                                        \
                                                                              \
  _dsk_rot_x->right = _dsk_rot_y->left;                                       \
  if (_dsk_rot_y->left)                                                       \
    _dsk_rot_y->left->parent = _dsk_rot_x;                                    \
  _dsk_rot_y->parent = _dsk_rot_x->parent;                                    \
  if (_dsk_rot_x->parent == NULL)                                             \
    top = _dsk_rot_y;                                                         \
  else if (_dsk_rot_x == _dsk_rot_x->parent->left)                            \
    _dsk_rot_x->parent->left = _dsk_rot_y;                                    \
  else                                                                        \
    _dsk_rot_x->parent->right = _dsk_rot_y;                                   \
  _dsk_rot_y->left = _dsk_rot_x;                                              \
  _dsk_rot_x->parent = _dsk_rot_y;                                            \
                                                                              \
  /* fixup counts. */                                                         \
  /* to re-derive this here's what rotate_left(x) does pictorially: */        \
  /*       x                                 y                      */        \
  /*      / \                               / \                     */        \
  /*     a   y     == rotate_left(x) ==>   x   c                    */        \
  /*        / \                           / \                       */        \
  /*       b   c                         a   b                      */        \
  /*                                                                */        \
  /* so:       n0 = get_count(x) (==a+b+c+2)                        */        \
  /*           n1 = get_count(c)   (c may be null)                  */        \
  /*           n2 = n0 - n1 - 1;                                    */        \
  /*           set_count(x, n2)                                     */        \
  /*           set_count(y, n0)                                     */        \
  /*     c is _dsk_rot_y->right at this point                       */        \
  /*                                                                */        \
  /* equivalently:                                                  */        \
  /*      y->count = x->count;                                      */        \
  /*      x->count -= c->count + 1                                  */        \
  {                                                                           \
    size_t _dsk_rot_n0 = get_count(_dsk_rot_x);                               \
    size_t _dsk_rot_n1 = _dsk_rot_y->right ? get_count(_dsk_rot_y->right) : 0;\
    size_t _dsk_rot_n2 = _dsk_rot_n0 - _dsk_rot_n1 - 1;                       \
    set_count (_dsk_rot_x, _dsk_rot_n2);                                      \
    set_count (_dsk_rot_y, _dsk_rot_n0);                                      \
  }                                                                           \
}DSK_STMT_END

 /* utility: recompute node's count, based on count of its children */
#define _DSK_RBCTREE_FIX_COUNT(type,parent,left,right,get_count,set_count, node)          \
DSK_STMT_START{                                                                 \
  size_t _dsk_fixcount_count = 1;                                             \
  if ((node)->left != NULL)                                                   \
    _dsk_fixcount_count += get_count((node)->left);                           \
  if ((node)->right != NULL)                                                  \
    _dsk_fixcount_count += get_count((node)->right);                          \
  set_count((node), _dsk_fixcount_count);                                     \
}DSK_STMT_END

 /* utility: recompute node's count, based on count of its children */
#define _DSK_RBCTREE_FIX_COUNT_AND_UP(type,parent,left,right,get_count,set_count, node)   \
DSK_STMT_START{                                                                 \
  type _tmp_fix_count_up;                                                     \
  for (_tmp_fix_count_up = (node);                                            \
       _tmp_fix_count_up != NULL;                                             \
       _tmp_fix_count_up = _tmp_fix_count_up->parent)                         \
    _DSK_RBCTREE_FIX_COUNT (type,parent,left,right,get_count,set_count, _tmp_fix_count_up);\
}DSK_STMT_END

#endif
