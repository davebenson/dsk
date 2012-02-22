/* DskArray

   This API, which is entirely macro-based, is divided into two parts
     (1) declaration:  declaring static, global, members of objects.
     (2) methods: using such declared arrays.
 */

#include <string.h>             /* for memcpy() */

/* Declarations.
   
   Each of these DSK_ARRAY_DECLARE_*() macros eclares four variables:
       name_init: an array of initial values
                  (on some compilers, 0 is not allowed)
       name:  the array
       n_name:  the length of the array
       name_allocated:  the nubmer of elements allocated in the array

   Can be used in a local context, a global/static context,
   or as members 
 */
#define DSK_ARRAY_DECLARE_VARIABLES(name, type, initial_size)              \
  type name##_init[initial_size];                                          \
  type *name = name##_init;                                                \
  size_t n_##name = 0;                                                     \
  size_t name##_allocated = initial_size;
  
#define DSK_ARRAY_DECLARE_STRUCT_MEMBERS(name, type, initial_size)         \
  ...
  
#define DSK_ARRAY_DECLARE_GLOBALS(static, name, type, initial_stack_size)  \
  ...

/* Methods
   
   All the runtime functions require a "scope":  it refers to
   anything to access the array.  For example, if your array is
   members of a local struct name 'foo', scope should be 'foo.';
   if it is a pointer to 'foo' scope should be 'foo->'.
   If the array is global or local to your function,
   scope should be empty.
   So you will often write DSK_ARRAY_APPEND(,name,type,value)
   (ie [sic] with the missing comma dfw) */


#define DSK_ARRAY_APPEND(scope, name, type, value)                         \
  do{                                                                      \
    if (scope n_##name == scope name##_allocated)                          \
      {                                                                    \
        unsigned daa__new_size = scope name##_allocated * 2;               \
        scope name = DSK_RENEW (type, scope name, daa__new_size);          \
        scope name##_allocated = daa__new_size;                            \
      }                                                                    \
    scope name[(scope n_##name)++] = (value);                              \
  }while(0)

#define DSK_ARRAY_APPEND_N(scope, name, type, n_values, values)            \
  do{                                                                      \
    unsigned _dsk_daan__new_alloced = scope name##_allocated;              \
    unsigned _dsk_daan__n = (n_values);                                    \
    unsigned _dsk_daan__new_size = scope n_##name + _dsk_daan_n;           \
    if (_dsk_daan__new_alloced < _dsk_daan__new_size)                      \
      {                                                                    \
        _dsk_daan__new_alloced *= 2;                                       \
        while (_dsk_daan__new_alloced < _dsk_daan__new_size)               \
          _dsk_daan__new_alloced *= 2;                                     \
        scope name = DSK_RENEW (type, scope name, daa__new_size);          \
        scope name##_allocated = _dsk_daan__new_alloced;                   \
      }                                                                    \
    memcpy (scope name, (values), _dsk_daan__n * sizeof(type));            \
  }while(0)

#define DSK_ARRAY_APPEND_ARRAY(scope, name, type, other_scope, other_name) \
  DSK_ARRAY_APPEND_N (scope, name, type,                                   \
                      other_name n_##other_name, other_name other_name)

#define DSK_ARRAY_REMOVE(scope, name, type, start, count)                  \
  ...

/* A fast remove function that reorders the array as needed */
#define DSK_ARRAY_REMOVE_REORDER(scope, name, type, start, count)          \
  ...

#define DSK_ARRAY_CLEAR_(scope, name)                                      \
  if (name##_init != name)                                                 \
    dsk_free (name);
    
