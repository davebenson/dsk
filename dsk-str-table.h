
DskStrTable *dsk_str_table_new (size_t value_size,
                                const void *value_default);

/* Includes NUL. */
unsigned     dsk_str_table_get_key_size(DskStrTable *table);

void        *dsk_str_table_get     (DskStrTable *,
                                    const char *str);
void        *dsk_str_table_get_prev(DskStrTable *,
                                    const char *str,
				    char       *key_out);
void        *dsk_str_table_get_next(DskStrTable *,
                                    const char *str,
				    char       *key_out);
void         dsk_str_table_set     (DskStrTable *,
                                    const char *str,
                                    const void *value);

/* Only sets if the entry is not found.
   Returns TRUE if the 'set'-operation worked
   (meaning that the key was created by this operation) */
dsk_boolean  dsk_str_table_maybe_set(DskStrTable *str_table,
                                     const char  *str,
                                     const void  *value);

void       dsk_str_table_foreach   (DskStrTable *...,
                                    const char *prefix,
                                    ...);

/* dsk_str_table_foreach_find is an arguably weak name choice.
   it is intended to mimick a linear-searching "find" function
   which returns TRUE as soon as a value is found that matches
   the criteria implemented in 'func'.  (And returns FALSE otherwise) */
dsk_boolean dsk_str_table_foreach_find (DskStrTable *...,
                                        const char *prefix,
                                        ...);

/* --- general interface - handles queries and sets --- */
typedef enum
{
/* can be passed if you want to initialize the slot yourself. */
  DSK_STR_TABLE_SLOT_NO_INIT          = (1<<0),

  /* Exactly one if these options should be given! */
#define _DSK_STR_TABLE_SLOT_MODE(n) ((n)<<8)
  DSK_STR_TABLE_SLOT_CREATE           = _DSK_STR_TABLE_SLOT_MODE(0),
  DSK_STR_TABLE_SLOT_MUST_CREATE      = _DSK_STR_TABLE_SLOT_MODE(1),
  DSK_STR_TABLE_SLOT_DELETE_IF_EXISTS = _DSK_STR_TABLE_SLOT_MODE(2),
  DSK_STR_TABLE_SLOT_DELETE_OR_CREATE = _DSK_STR_TABLE_SLOT_MODE(3),
  DSK_STR_TABLE_SLOT_MUST_EXIST       = _DSK_STR_TABLE_SLOT_MODE(4),
  DSK_STR_TABLE_SLOT_PREDECESSOR      = _DSK_STR_TABLE_SLOT_MODE(5),
  DSK_STR_TABLE_SLOT_SUCCESSOR        = _DSK_STR_TABLE_SLOT_MODE(6),
} DskStrTableSlotFlags;

#define _DSK_STR_TABLE_SLOT_MODE_MASK    _DSK_STR_TABLE_SLOT_MODE_MASK(0xf)

void        *dsk_str_table_slot(DskStrTable         *table,
                                DskStrTableSlotFlags flags,
                                const char         **key_opt_out,
				dsk_boolean         *created_opt_out,
				const char          *str);
