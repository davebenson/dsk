
/* Attempt to macro-ize a common case where you have an array
 * which you want to be stack allocated for small sizes (where
 * "small" depends on the application), and use the heap for
 * larger sizes.
 */

#define DSK_TMP_ARRAY_DECLARE(type, name, initial_allocation_size) \
  type name##__stack_allocation[initial_allocation_size];          \
  struct {                                                         \
    size_t length;                                                 \
    type *data;                                                    \
    size_t n_allocated;                                            \
  } name = {                                                       \
    0,                                                             \
    name##__stack_allocation,                                      \
    initial_allocation_size                                        \
  }

#define DSK_TMP_ARRAY_APPEND(name, value)                          \
   do{                                                             \
     if (name.length == name.n_allocated)                          \
       {                                                           \
         name.n_allocated *= 2;                                    \
         size_t new_size = sizeof (*(name.data)) * name.n_allocated; \
         if (name.data == name##__stack_allocation)                \
           {                                                       \
             name.data = dsk_malloc (new_size);                    \
             memcpy (name.data, name##__stack_allocation, new_size/2); \
           }                                                       \
         else                                                      \
           {                                                       \
             name.data = dsk_realloc (name.data, new_size);        \
           }                                                       \
       }                                                           \
     name.data[name.length++] = (value);                           \
   }while(0)

#define DSK_TMP_ARRAY_CLEAR(name) 		                   \
  do{                                                              \
    if (name.data != name##__stack_allocation)                     \
      dsk_free (name.data);                                        \
  }while(0)
#define DSK_TMP_ARRAY_CLEAR_TO_ALLOCATION(name, out)               \
  do{                                                              \
    size_t s = sizeof (*(name.data)) * name.length;                \
    if (name.data == name##__stack_allocation)                     \
      out = memcpy (dsk_malloc (s), name.data, s);                 \
    else                                                           \
      out = dsk_realloc (name.data, s);                            \
    name.data = name##__stack_allocation;                          \
    name.length = 0;                                               \
  }while(0)

#define DSK_TMP_ARRAY_QSORT(name, compare_func)                    \
  qsort(name.data, name.length, sizeof (*(name.data)), compare_func)
