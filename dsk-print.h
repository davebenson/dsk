// EXPERIMENT: vararg-free printing

/* DskPrint *codegen = dsk_print_new_fp (stdout, NULL);
   dsk_print_set_string (codegen, "key", "value");
   dsk_print (codegen, "this is $key!\n");
   dsk_print_context_free (codegen);
 */

typedef dsk_boolean (*DskPrintAppendFunc) (unsigned   length,
                                           const uint8_t *data,
                                           void      *append_data,
					   DskError **error);

typedef struct _DskPrint DskPrint;

DskPrint *dsk_print_new    (DskPrintAppendFunc append,
                            void              *append_data,
			    DskDestroyNotify   append_data_destroy);
void      dsk_print_free   (DskPrint *print);

/* stdio.h support */
DskPrint *dsk_print_new_fp (void *file_pointer);
DskPrint *dsk_print_new_fp_fclose (void *file_pointer);
DskPrint *dsk_print_new_fp_pclose (void *file_pointer);

/* buffer support */
DskPrint *dsk_print_new_buffer    (DskBuffer *buffer);

/* Setting variables for interpolation */
void dsk_print_set_string          (DskPrint    *context,
                                    const char  *variable_name,
			            const char  *value);
void dsk_print_set_int             (DskPrint    *context,
                                    const char  *variable_name,
			            int          value);
void dsk_print_set_uint            (DskPrint    *context,
                                    const char  *variable_name,
			            unsigned     value);
void dsk_print_set_int64           (DskPrint    *context,
                                    const char  *variable_name,
			            int64_t      value);
void dsk_print_set_uint64          (DskPrint    *context,
                                    const char  *variable_name,
			            uint64_t     value);
void dsk_print_set_template_string (DskPrint    *context,
                                    const char  *variable_name,
			            const char  *template_string);
void dsk_print_set_buffer          (DskPrint    *context,
                                    const char  *variable_name,
                                    DskBuffer   *buffer);

/* Somewhat like Local Variables:  Pop will undo all variable assignments
   since the last call to push. */
void dsk_print_push                (DskPrint *context);
void dsk_print_pop                 (DskPrint *context);

void dsk_print                     (DskPrint    *context,
                                    const char  *template_string);


/* Making a context of variables that can be popped in one quick go;
   these take ownership of the filter */
void dsk_print_set_filtered_string   (DskPrint    *context,
                                      const char  *variable_name,
			              const char  *raw_string,
                                      DskOctetFilter *filter);
void dsk_print_set_filtered_binary   (DskPrint    *context,
                                      const char  *variable_name,
                                      size_t       raw_string_length,
			              const char  *raw_string,
                                      DskOctetFilter *filter);
void dsk_print_set_filtered_buffer   (DskPrint    *context,
                                      const char  *variable_name,
			              const DskBuffer *buffer,
                                      DskOctetFilter *filter);

