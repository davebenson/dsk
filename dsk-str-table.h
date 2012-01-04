


DskStrTable *dsk_str_table_new (size_t value_size,
                                const void *value_default);

/* Includes NUL. */
unsigned     dsk_str_table_get_key_size(DskStrTable *table);

void        *dsk_str_table_get     (DskStrTable *,
                                    const char *str);
void        *dsk_str_table_get_prev(DskStrTable *,
                                    const char *str,
				    const char **key_out);
void        *dsk_str_table_get_next(DskStrTable *,
                                    const char *str,
				    const char **key_out);
void        *dsk_str_table_force   (DskStrTable *,
                                    const char *str);
void        *dsk_str_table_force_noinit 
                                   (DskStrTable *,
                                    const char *str,
                                    dsk_boolean *created_opt_out);
void        *dsk_str_table_create  (DskStrTable *,
                                    const char *str);
dsk_boolean  dsk_str_table_delete  (DskStrTable *,
                                    const char *str,
                                    void       *value_opt_out);
void        *dsk_str_table_create_or_delete
                                   (DskStrTable *,
                                    const char *str);
void        *dsk_str_table_create_or_delete_noinit
                                   (DskStrTable *,
                                    const char *str);
void         dsk_str_table_set     (DskStrTable *,
                                    const char *str,
                                    const void *value);

typedef void (*DskStrTableForeach) (const char *str,
                                    void       *value,
                                    void       *foreach_data);

void       dsk_str_table_foreach   (DskStrTable *str_table,
                                    const char *prefix,
                                    DskStrTableForeach foreach,
                                    void       *foreach_data);

typedef dsk_boolean (*DskStrTableForeachFind) (const char *str,
                                               void       *value,
                                               void       *foreach_data);
/* dsk_str_table_foreach_find is an arguably weak name choice.
   it is intended to mimick a linear-searching "find" function
   which returns TRUE as soon as a value is found that matches
   the criteria implemented in 'func'.  (And returns FALSE otherwise) */
dsk_boolean dsk_str_table_foreach_find (DskStrTable *str_table,
                                        const char *prefix,
                                        DskStrTableForeachFind func,
                                        void       *foreach_data);

struct _DskStrTable
{
  DskStrTableInterface *iface;
};

struct _DskStrTableInterface
{
  size_t sizeof_table;
  void (*init)          (size_t         value_size,
                         const void    *value_default,
                         DskStrTable   *to_init);
  void *(*get)          (DskStrTable   *table,
                         const char    *str);
  void *(*get_prev)     (DskStrTable   *table,
                         const char    *str,
                         const char   **key_out);
  void *(*get_next)     (DskStrTable   *table,
                         const char    *str,
                         const char   **key_out);
  void *(*force)        (DskStrTable   *table,
                         const char    *str);
  void *(*force_noinit) (DskStrTable   *table,
                         const char    *str,
                         dsk_boolean *created_opt_out);
  void *(*create)       (DskStrTable   *table,
                         const char    *str);
  dsk_boolean  (*remove)(DskStrTable   *table,
                         const char    *str,
                         void          *value_opt_out);
  void *(*create_or_remove)
                        (DskStrTable   *table,
                         const char    *str);
  void *(*create_or_remove_noinit)
                        (DskStrTable   *str_table,
                         const char    *str);
  void  (*set)          (DskStrTable   *str_table,
                         const char    *str,
                         const void    *value);
  dsk_boolean (*foreach)(DskStrTable   *str_table,
                         const char    *prefix,
                         DskStrTableForeachFind foreach,
                         void          *foreach_data);
};
