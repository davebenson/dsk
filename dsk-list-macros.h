
/* We define four data structures:
 * 1.  a Stack.  a singly-ended, singly-linked list.
 * 2.  a Queue.  a doubly-ended, singly-linked list.
 * 3.  a List.  a doubly-ended, doubly-linked list.
 * 4.  a Ring.  a singly-ended, doubly-linked, circular list.
 *
 * Stack operations:
 *    PUSH(stack, node)                                        [O(1)]
 *    POP(stack, rv_node)                                      [O(1)]
 *    INSERT_AFTER(stack, above_node, new_node)                [O(1)]
 *    IS_EMPTY(stack)                                          [O(1)]
 *    REVERSE(stack)                                           [O(N)]
 *    SORT(stack, comparator)                                  [O(NlogN)]
 *    FOREACH(stack, variable, code)                           [O(N)]
 *    GET_BOTTOM(stack, rv_node)                               [O(N)]
 * Queue operations:
 *    ENQUEUE(queue, node)                                     [O(1)]
 *    DEQUEUE(queue, rv_node)                                  [O(1)]
 *    PREPEND(queue, node)                                     [O(1)]
 *    IS_EMPTY(queue)                                          [O(1)]
 *    REVERSE(queue)                                           [O(N)]
 *    FOREACH(queue, variable, code)                           [O(N)]
 *    SORT(queue, comparator)                                  [O(NlogN)] 
 * List operations:
 *    PREPEND(list, node)                                      [O(1)]
 *    APPEND(list, node)                                       [O(1)]
 *    REMOVE_FIRST(list)                                       [O(1)]
 *    REMOVE_LAST(list)                                        [O(1)]
 *    REMOVE(list, node)                                       [O(1)]
 *    INSERT_AFTER(list, position_node, new_node)              [O(1)]
 *    INSERT_BEFORE(list, position_node, new_node)             [O(1)]
 *    IS_EMPTY(list)                                           [O(1)]
 *    REVERSE(list)                                            [O(N)]
 *    FOREACH(list, variable, code)                            [O(N)]
 *    SORT(list)                                               [O(NlogN)]
 * Ring operations:
 *    PREPEND(list, node)                                      [O(1)]
 *    APPEND(list, node)                                       [O(1)]
 *    REMOVE_FIRST(list)                                       [O(1)]
 *    REMOVE_LAST(list)                                        [O(1)]
 *    REMOVE(list, node)                                       [O(1)]
 *    INSERT_AFTER(list, position_node, new_node)              [O(1)]
 *    INSERT_BEFORE(list, position_node, new_node)             [O(1)]
 *    IS_EMPTY(list)                                           [O(1)]
 *    REVERSE(list)                                            [O(N)]
 *    FOREACH(list, variable, code)                            [O(N)]
 *    SORT(list)                                               [O(NlogN)]
 *
 * note: the SORT operation is stable, i.e.  if two
 * elements are equal according to the comparator,
 * then their relative order in the list
 * will be preserved.
 *
 * In the above, 'stack', 'queue', and 'list' are
 * a comma-separated list of arguments, actually.
 * In particular:
 *   stack => NodeType*, top, next_member_name
 *   queue => NodeType*, head, tail, next_member_name
 *   list => NodeType*, first, last, prev_member_name, next_member_name
 *   ring => NodeType*, first, prev_member_name, next_member_name
 * We recommend making macros that end in GET_STACK_ARGS, GET_QUEUE_ARGS,
 * and GET_LIST_ARGS that return the relevant N-tuples.
 */

#define DSK_LOG2_MAX_LIST_SIZE          (DSK_SIZEOF_POINTER*8 - DSK_LOG2_SIZEOF_POINTER)

/* --- Stacks --- */
#define DSK_STACK_PUSH(stack, node) DSK_STACK_PUSH_(stack, node)
#define DSK_STACK_POP(stack, rv_node) DSK_STACK_POP_(stack, rv_node)
#define DSK_STACK_INSERT_AFTER(stack, above_node, new_node) \
        DSK_STACK_INSERT_AFTER_(stack, above_node, new_node)
#define DSK_STACK_IS_EMPTY(stack) DSK_STACK_IS_EMPTY_(stack)
#define DSK_STACK_REVERSE(stack) DSK_STACK_REVERSE_(stack)
#define DSK_STACK_FOREACH(stack, iter_var, code) DSK_STACK_FOREACH_(stack, iter_var, code)
#define DSK_STACK_SORT(stack, comparator) DSK_STACK_SORT_(stack, comparator)
#define DSK_STACK_GET_BOTTOM(stack, rv_node) DSK_STACK_GET_BOTTOM_(stack, rv_node)

#define DSK_STACK_PUSH_(type, top, next, node) 				\
  do{								        \
    type _dsk_tmp = (node);                                             \
    _dsk_tmp->next = (top);						\
    (top) = _dsk_tmp;							\
  }while(0)
#define DSK_STACK_POP_(type, top, next, rv_node) 	                \
  do{								        \
    rv_node = (top);							\
    (top) = (top)->next;						\
  }while(0)
#define DSK_STACK_INSERT_AFTER_(type, top, next, above_node, new_node)  \
  do{								        \
    type _dsk_tmp = (new_node);                                         \
    type _dsk_above = (above_node);                                     \
    _dsk_tmp->next = _dsk_above->next;				        \
    _dsk_above->next = _dsk_tmp;					\
  }while(0)
#define DSK_STACK_IS_EMPTY_(type, top, next)				\
  ((top) == NULL)
#define DSK_STACK_REVERSE_(type, top, next)				\
  do{								        \
    type _dsk___prev = NULL;                                            \
    type _dsk___at = (top);						\
    while (_dsk___at != NULL)						\
      {									\
	type _dsk__next = _dsk___at->next;				\
        _dsk___at->next = _dsk___prev;					\
	_dsk___prev = _dsk___at;					\
	_dsk___at = _dsk__next;						\
      }									\
    (top) = _dsk___prev;						\
  }while(0)
#define DSK_STACK_FOREACH_(type, top, next, iter_var, code)             \
  for (iter_var = top; iter_var != NULL; )                              \
    {                                                                   \
      type _dsk__next = iter_var->next;                                 \
      code;                                                             \
      iter_var = _dsk__next;                                            \
    }
/* sort algorithm:
 *   in order to implement SORT in a macro, it cannot use recursion.
 *   but that's ok because you can just manually implement a stack,
 *   which is probably faster anyways.
 */
#define DSK_STACK_SORT_(type, top, next, comparator)			\
  do{								        \
    type _dsk_stack[DSK_LOG2_MAX_LIST_SIZE];				\
    unsigned _dsk_stack_size = 0;					\
    unsigned _dsk_i;                                                    \
    type _dsk_at;							\
    for (_dsk_at = top; _dsk_at != NULL; )				\
      {									\
	type _dsk_a = _dsk_at;						\
	type _dsk_b;							\
        type _dsk_cur_list;                                             \
        int _dsk_comparator_rv;                                         \
	_dsk_at = _dsk_at->next;					\
	if (_dsk_at)							\
	  {								\
	    _dsk_b = _dsk_at;						\
	    _dsk_at = _dsk_at->next;					\
	    comparator (_dsk_a, _dsk_b, _dsk_comparator_rv);		\
	    if (_dsk_comparator_rv > 0)					\
	      {								\
                /* sort first two elements */                           \
		type _dsk_swap = _dsk_b;				\
		_dsk_b = _dsk_a;					\
		_dsk_a = _dsk_swap;					\
		_dsk_a->next = _dsk_b;					\
		_dsk_b->next = NULL;					\
	      }								\
	    else							\
              {                                                         \
                /* first two elements already sorted */                 \
	        _dsk_b->next = NULL;					\
              }                                                         \
	  }								\
	else								\
	  {								\
            /* only one element remains */                              \
	    _dsk_a->next = NULL;					\
            _dsk_at = NULL;                                             \
	  }								\
	_dsk_cur_list = _dsk_a;						\
									\
	/* merge _dsk_cur_list up the stack */				\
	for (_dsk_i = 0; ; _dsk_i++)				        \
	  {								\
	    /* expanding the stack is marked unlikely,         */	\
	    /* since in the case it matters (where the number  */	\
	    /* of elements is big), the stack rarely grows.    */	\
	    if (DSK_UNLIKELY (_dsk_i == _dsk_stack_size))               \
	      {                                                         \
		_dsk_stack[_dsk_stack_size++] = _dsk_cur_list;          \
		break;                                                  \
	      }                                                         \
	    else if (_dsk_stack[_dsk_i] == NULL)                        \
	      {                                                         \
		_dsk_stack[_dsk_i] = _dsk_cur_list;                     \
		break;                                                  \
	      }                                                         \
	    else                                                        \
	      {                                                         \
		/* Merge _dsk_stack[_dsk_i] and _dsk_cur_list. */       \
		type _dsk_merge_list = _dsk_stack[_dsk_i];              \
                type _dsk_new_cur_list;                                 \
		_dsk_stack[_dsk_i] = NULL;                              \
                                                                        \
                _DSK_MERGE_NONEMPTY_LISTS (_dsk_merge_list,             \
                                           _dsk_cur_list,               \
                                           _dsk_new_cur_list,           \
                                           type, next, comparator);     \
                _dsk_cur_list = _dsk_new_cur_list;			\
                _dsk_stack[_dsk_i] = NULL;				\
              }                                                         \
	  }								\
      }									\
                                                                        \
    /* combine all the elements on the stack into a final output */     \
    top = NULL;                                                         \
    for (_dsk_i = 0; _dsk_i < _dsk_stack_size; _dsk_i++)		\
      if (_dsk_stack[_dsk_i] != NULL)                                   \
        {                                                               \
          if (top == NULL)                                              \
            top = _dsk_stack[_dsk_i];                                   \
          else                                                          \
            {                                                           \
              type _dsk_new_top;                                        \
              _DSK_MERGE_NONEMPTY_LISTS (_dsk_stack[_dsk_i],            \
                                         top,                           \
                                         _dsk_new_top,                  \
                                         type, next, comparator);       \
              top = _dsk_new_top;                                       \
            }                                                           \
        }                                                               \
  }while(0)

#define DSK_STACK_GET_BOTTOM_(type, top, next, rv_node)                 \
  do{                                                                   \
    rv_node = top;                                                      \
    if (rv_node != NULL)                                                \
      while (rv_node->next)                                             \
        rv_node = rv_node->next;                                        \
  }while(0)

/* --- Queues --- */
#define DSK_QUEUE_ENQUEUE(queue, node) DSK_QUEUE_ENQUEUE_(queue, node)
#define DSK_QUEUE_DEQUEUE(queue, rv_node) DSK_QUEUE_DEQUEUE_(queue, rv_node) 
#define DSK_QUEUE_PREPEND(queue, node) DSK_QUEUE_PREPEND_(queue, node) 
#define DSK_QUEUE_IS_EMPTY(queue) DSK_QUEUE_IS_EMPTY_(queue) 
#define DSK_QUEUE_REVERSE(queue) DSK_QUEUE_REVERSE_(queue) 
#define DSK_QUEUE_SORT(queue, comparator) DSK_QUEUE_SORT_(queue, comparator) 

#define DSK_QUEUE_ENQUEUE_(type, head, tail, next, node)                \
  do{                                                                   \
    type _dsk_tmp = (node);                                             \
    if (tail)                                                           \
      tail->next = _dsk_tmp;                                            \
    else                                                                \
      head = _dsk_tmp;                                                  \
    tail = _dsk_tmp;                                                    \
    node->next = NULL;                                                  \
  }while(0)
#define DSK_QUEUE_DEQUEUE_(type, head, tail, next, rv_node)             \
  do{                                                                   \
    rv_node = head;                                                     \
    if (head)                                                           \
      {                                                                 \
        head = head->next;                                              \
        if (head == NULL)                                               \
          tail = NULL;                                                  \
      }                                                                 \
  }while(0)
#define DSK_QUEUE_PREPEND_(type, head, tail, next, node)                \
  do{                                                                   \
    type _dsk_tmp = (node);                                             \
    _dsk_tmp->next = head;                                              \
    head = _dsk_tmp;                                                    \
    if (tail == NULL)                                                   \
      tail = head;                                                      \
  }while(0)

#define DSK_QUEUE_IS_EMPTY_(type, head, tail, next)                     \
  ((head) == NULL)

#define DSK_QUEUE_REVERSE_(type, head, tail, next)                      \
  do{                                                                   \
    type _dsk_queue_new_tail = head;                                    \
    DSK_STACK_REVERSE_(type, head, next);                               \
    tail = _dsk_queue_new_tail;                                         \
  }while(0)
#define DSK_QUEUE_FOREACH_(type, head, tail, next, variable, code)      \
  DSK_STACK_FOREACH_(type, head, next, variable, code)

#define DSK_QUEUE_SORT_(type, head, tail, next, comparator)             \
  do{                                                                   \
    DSK_STACK_SORT_(type, head, next, comparator);                      \
    DSK_STACK_GET_BOTTOM_(type, head, next, tail);                      \
  }while(0)

/* --- List --- */
#define DSK_LIST_PREPEND(list, node) DSK_LIST_PREPEND_(list, node)
#define DSK_LIST_APPEND(list, node) DSK_LIST_APPEND_(list, node)
#define DSK_LIST_REMOVE_FIRST(list) DSK_LIST_REMOVE_FIRST_(list)
#define DSK_LIST_REMOVE_LAST(list) DSK_LIST_REMOVE_LAST_(list)
#define DSK_LIST_REMOVE(list, node) DSK_LIST_REMOVE_(list, node)
#define DSK_LIST_INSERT_AFTER(list, at, node) DSK_LIST_INSERT_AFTER_(list, at, node)
#define DSK_LIST_INSERT_BEFORE(list, at, node) DSK_LIST_INSERT_BEFORE_(list, at, node)
#define DSK_LIST_IS_EMPTY(list) DSK_LIST_IS_EMPTY_(list)
#define DSK_LIST_REVERSE(list) DSK_LIST_REVERSE_(list)
#define DSK_LIST_FOREACH(list, variable, code) DSK_LIST_FOREACH_(list, variable, code)
#define DSK_LIST_REVERSE_FOREACH(list, variable, code) DSK_LIST_REVERSE_FOREACH_(list, variable, code)
#define DSK_LIST_SORT(list, comparator) DSK_LIST_SORT_(list, comparator)

#define DSK_LIST_PREPEND_(type, first, last, prev, next, node)          \
  do{                                                                   \
    type _dsk_tmp = (node);                                             \
    if (first)                                                          \
      first->prev = (_dsk_tmp);                                         \
    else                                                                \
      last = (_dsk_tmp);                                                \
    node->next = first;                                                 \
    node->prev = NULL;                                                  \
    first = node;                                                       \
  }while(0)
#define DSK_LIST_APPEND_(type, first, last, prev, next, node)           \
  DSK_LIST_PREPEND_(type, last, first, next, prev, node)
#define DSK_LIST_REMOVE_FIRST_(type, first, last, prev, next)           \
  do{                                                                   \
    first = first->next;                                                \
    if (first == NULL)                                                  \
      last = NULL;                                                      \
    else                                                                \
      first->prev = NULL;                                               \
  }while(0)
#define DSK_LIST_REMOVE_LAST_(type, first, last, prev, next)            \
  DSK_LIST_REMOVE_FIRST_(type, last, first, next, prev)
#define DSK_LIST_REMOVE_(type, first, last, prev, next, node)           \
  do{                                                                   \
    type _dsk_tmp = (node);                                             \
    if (_dsk_tmp->prev)                                                 \
      _dsk_tmp->prev->next = _dsk_tmp->next;                            \
    else                                                                \
      first = _dsk_tmp->next;                                           \
    if (_dsk_tmp->next)                                                 \
      _dsk_tmp->next->prev = _dsk_tmp->prev;                            \
    else                                                                \
      last = _dsk_tmp->prev;                                            \
  }while(0)

#define DSK_LIST_INSERT_AFTER_(type, first, last, prev, next, at, node) \
  do{                                                                   \
    type _dsk_at = (at);                                                \
    type _dsk_node = (node);                                            \
    _dsk_node->prev = _dsk_at;                                          \
    _dsk_node->next = _dsk_at->next;                                    \
    if (_dsk_node->next)                                                \
      _dsk_node->next->prev = _dsk_node;                                \
    else                                                                \
      last = _dsk_node;                                                 \
    _dsk_at->next = _dsk_node;                                          \
  }while(0)
#define DSK_LIST_INSERT_BEFORE_(type, first, last, prev, next, at, node)\
  DSK_LIST_INSERT_AFTER_(type, last, first, next, prev, at, node)
#define DSK_LIST_IS_EMPTY_(type, first, last, prev, next)               \
  ((first) == NULL)
#define DSK_LIST_REVERSE_(type, first, last, prev, next)                \
  do{                                                                   \
    type _dsk_at = first;                                               \
    first = last;                                                       \
    last = _dsk_at;                                                     \
    while (_dsk_at)                                                     \
      {                                                                 \
        type _dsk_old_next = _dsk_at->next;                             \
        _dsk_at->next = _dsk_at->prev;                                  \
        _dsk_at->prev = _dsk_old_next;                                  \
        _dsk_at = _dsk_old_next;                                        \
      }                                                                 \
  }while(0)
#define DSK_LIST_SORT_(type, first, last, prev, next, comparator)       \
  do{                                                                   \
    type _dsk_prev = NULL;                                              \
    type _dsk_at;                                                       \
    DSK_STACK_SORT_(type, first, next, comparator);                     \
    for (_dsk_at = first; _dsk_at; _dsk_at = _dsk_at->next)             \
      {                                                                 \
        _dsk_at->prev = _dsk_prev;                                      \
        _dsk_prev = _dsk_at;                                            \
      }                                                                 \
    last = _dsk_prev;                                                   \
  }while(0)
#define DSK_LIST_FOREACH_(type, first, last, prev, next, variable, code)\
  do{                                                                   \
    if (first)                                                          \
      {                                                                 \
        variable = first;                                               \
        while (variable != NULL)                                        \
          {                                                             \
            code;                                                       \
            variable = variable->next;                                  \
          }                                                             \
      }                                                                 \
  }while(0)
#define DSK_LIST_REVERSE_FOREACH_(type, first, last, prev, next, variable, code)\
  DSK_LIST_FOREACH_(type, last, first, next, prev, variable, code)

/* --- rings --- */
#define DSK_RING_PREPEND(ring, node)                                    \
        DSK_RING_PREPEND_(ring, node)
#define DSK_RING_APPEND(ring, node)                                     \
        DSK_RING_APPEND_(ring, node)
#define DSK_RING_REMOVE_FIRST(ring)                                     \
        DSK_RING_REMOVE_FIRST_(ring, node)
#define DSK_RING_REMOVE_LAST(ring)                                      \
        DSK_RING_REMOVE_LAST_(ring, node)
#define DSK_RING_REMOVE(ring, node)                                     \
        DSK_RING_REMOVE_(ring, node)
#define DSK_RING_INSERT_AFTER(ring, position_node, new_node)            \
        DSK_RING_INSERT_AFTER_(ring, node)
#define DSK_RING_INSERT_BEFORE(ring, position_node, new_node)           \
        DSK_RING_INSERT_BEFORE_(ring, node)
#define DSK_RING_IS_EMPTY(ring)                                         \
        DSK_RING_IS_EMPTY_(ring)
#define DSK_RING_REVERSE(ring)                                          \
        DSK_RING_REVERSE_(ring, node)
#define DSK_RING_FOREACH(ring, variable, code)                          \
        DSK_RING_FOREACH_(ring, variable, code)
#define DSK_RING_SORT(ring, comparator)                                 \
        DSK_RING_SORT_(ring, variable, code)
#define DSK_RING_PREPEND_(type, first, prev, next, node)                \
  do{                                                                   \
    if (first == NULL)                                                  \
      (node)->prev = (node)->next = (node);                             \
    else                                                                \
      {                                                                 \
        (node)->prev = (first)->prev;                                   \
        (node)->next = (first);                                         \
        (first)->prev->next = (node);                                   \
        (first)->prev = (node);                                         \
      }                                                                 \
    first = (node);                                                     \
  }while(0)
#define DSK_RING_APPEND_(type, first, prev, next, node)                 \
  do{                                                                   \
    if (first == NULL)                                                  \
      {                                                                 \
        (node)->prev = (node)->next = (node);                           \
        first = node;                                                   \
      }                                                                 \
    else                                                                \
      {                                                                 \
        (node)->prev = (first)->prev;                                   \
        (node)->next = (first);                                         \
        (first)->prev->next = (node);                                   \
        (first)->prev = (node);                                         \
      }                                                                 \
  }while(0)
#define DSK_RING_INSERT_AFTER_(type, first, prev, next, position_node, node)\
  do{                                                                   \
    (node)->prev = (position_node);                                     \
    (node)->next = (position_node)->next;                               \
    (node)->prev->next = (node);                                        \
    (node)->next->prev = (node);                                        \
  }while(0)
#define DSK_RING_INSERT_BEFORE_(type, first, prev, next, position_node, node)\
  do{                                                                   \
    if ((position_node) == (first))                                     \
      DSK_RING_PREPEND_(type, first, prev, next, node);                 \
    else                                                                \
      DSK_RING_INSERT_AFTER_(type, first, prev, next, position_node->prev, node);\
  }while(0)
#define DSK_RING_IS_EMPTY_(type, first, prev, next) ((first) == NULL)
#define DSK_RING_REVERSE_(type, first, prev, next)                      \
  do{                                                                   \
    if (first)                                                          \
      {                                                                 \
        type _dsk_last = (first)->prev;                                 \
        type _dsk_at = (first);                                         \
        for (;;)                                                        \
          {                                                             \
            type _dsk_swap = _dsk_at->prev;                             \
            _dsk_at->prev = _dsk_at->next;                              \
            _dsk_at->next = _dsk_swap;                                  \
            if (_dsk_at == _dsk_last)                                   \
              break;                                                    \
            _dsk_at = _dsk_at->prev;                                    \
          }                                                             \
      }                                                                 \
  }while(0)
#define DSK_RING_FOREACH_(type, first, prev, next, variable, code)      \
  do{                                                                   \
    if (first)                                                          \
      {                                                                 \
        variable = first;                                               \
        for (;;)                                                        \
          {                                                             \
            code;                                                       \
            if (first->prev == variable)                                \
              break;                                                    \
            variable = variable->next;                                  \
          }                                                             \
      }                                                                 \
  }while(0)
#define DSK_RING_SORT_(type, first, prev, next, comparator)             \
  do{                                                                   \
    if (first)                                                          \
      {                                                                 \
        type _dsk_prev;                                                 \
        type _dsk_at;                                                   \
        first->prev->next = NULL;                                       \
        DSK_STACK_SORT_ (type, first, next, comparator);                \
        _dsk_prev = first;                                              \
        for (_dsk_at = first->next; _dsk_at; _dsk_at = _dsk_at->next)   \
          {                                                             \
            _dsk_at->prev = _dsk_prev;                                  \
            _dsk_prev = _dsk_at;                                        \
          }                                                             \
        first->prev = _dsk_prev;                                        \
        _dsk_prev->next = first;                                        \
      }                                                                 \
  }while(0)

/* --- Internals --- */
#define _DSK_MERGE_NONEMPTY_LISTS(a,b,out,type,next,comparator)         \
  do{                                                                   \
    type _dsk_out_at;                                                   \
    int _dsk_comparator_rv;                                             \
    /* merge 'a' and 'b' into 'out' -- in order to make the sort stable,*/\
    /* always put elements if 'a' first in the event of a tie (i.e. */  \
    /* when comparator_rv==0)                                       */  \
    comparator (a, b, _dsk_comparator_rv);                              \
    if (_dsk_comparator_rv <= 0)                                        \
      {                                                                 \
        out = a;                                                        \
        a = a->next;                                                    \
      }                                                                 \
    else                                                                \
      {                                                                 \
        out = b;                                                        \
        b = b->next;                                                    \
      }                                                                 \
    _dsk_out_at = out;			                                \
    while (a && b)                                                      \
      {                                                                 \
        comparator (a, b, _dsk_comparator_rv);                          \
        if (_dsk_comparator_rv <= 0)				        \
          {							        \
            _dsk_out_at->next = a;		                        \
            _dsk_out_at = a;			                        \
            a = a->next;		                                \
          }                                                             \
        else                                                            \
          {							        \
            _dsk_out_at->next = b;		                        \
            _dsk_out_at = b;			                        \
            b = b->next;	                                        \
          }							        \
      }							                \
    _dsk_out_at->next = (a != NULL) ? a : b;                            \
  }while(0)
