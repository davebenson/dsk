/*
 * A PQ (Priority Queue) is a tuple:
 *     type, count, array, compare
 * where compare is a macro that takes (type*, type*, rv) and sets rv to -1, 0, 1 as usual
 */
#define DSK_PRIORITY_QUEUE_REMOVE_MIN(pq) \
        DSK_PRIORITY_QUEUE_REMOVE_(pq, 0)
#define DSK_PRIORITY_QUEUE_ADD(pq, value) \
        DSK_PRIORITY_QUEUE_ADD_(pq, value)


#define DSK_PRIORITY_QUEUE_REMOVE_(type,count,array,compare,  index)           \
  do{                                                                          \
    type _dsk_tmp_swap;                                                        \
    (array)[0] = (array)[(count) - 1];                                         \
    --(count);                                                                 \
    size_t _dsk_fixup = 0;                                                     \
    for (;;)                                                                   \
      {                                                                        \
        size_t _dsk_first_child = (_dsk_fixup) * 2 + 1;                        \
        if (_dsk_first_child >= (count))                                       \
          break;                                                               \
                                                                               \
        /* maintain heap invariant for _dsk_fixup[0], _dsk_first_child[0,1]; */\
        /* if a child changes, keep looping; otherwise stop. */                \
        int _dsk_rv;                                                           \
        size_t _dsk_min_idx = _dsk_fixup;                                      \
        compare (&(array)[_dsk_fixup], &(array)[_dsk_first_child], _dsk_rv);   \
        if (_dsk_rv > 0)                                                       \
          _dsk_min_idx = _dsk_first_child;                                     \
                                                                               \
        if (_dsk_first_child + 1 < (count))                                    \
          {                                                                    \
            compare (&(array)[_dsk_min_idx], &(array)[_dsk_first_child + 1], _dsk_rv); \
            if (_dsk_rv > 0)                                                   \
              _dsk_min_idx = _dsk_first_child + 1;                             \
          }                                                                    \
                                                                               \
        /* invariant holds without further modifying tree */                   \
        if (_dsk_min_idx == _dsk_fixup)                                        \
          break;                                                               \
                                                                               \
        /* swap so invariant holds at fixup, */                                \
        /* but now the invariant may be broken at min_idx, */                  \
        /* whose value has become larger. */                                   \
        _DSK_PRIORITY_QUEUE_TMP_SWAP(array, _dsk_fixup, _dsk_min_idx);         \
        _dsk_fixup = _dsk_min_idx;                                             \
      }                                                                        \
  }while(0)
#define DSK_PRIORITY_QUEUE_ADD_(type,count,array,compare, value)               \
  do{                                                                          \
    size_t _dsk_fixup_at = (count)++;                                          \
    type _dsk_tmp_swap;                                                        \
    (array)[_dsk_fixup_at] = (value);                                          \
    while (_dsk_fixup_at > 0)                                                  \
      {                                                                        \
        size_t _dsk_parent = (_dsk_fixup_at - 1) / 2;                          \
        int _dsk_rv;                                                           \
        compare((array)[_dsk_parent], (array)[_dsk_fixup_at], _dsk_rv);        \
        if (_dsk_rv <= 0)                                                      \
          {                                                                    \
            /* invariant holds: no need to continue */                         \
            break;                                                             \
          }                                                                    \
                                                                               \
        _DSK_PRIORITY_QUEUE_TMP_SWAP(array, _dsk_fixup_at, _dsk_parent);       \
        _dsk_fixup_at = _dsk_parent;                                           \
      }                                                                        \
  }while(0)

// internal: for use by macros that define _dsk_tmp_swap
#define _DSK_PRIORITY_QUEUE_TMP_SWAP(array, a, b)                              \
  do{                                                                          \
    _dsk_tmp_swap = (array)[(a)];                                              \
    (array)[(a)] = (array)[(b)];                                               \
    (array)[(b)] = _dsk_tmp_swap;                                              \
  }while(0)
