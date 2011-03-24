
typedef enum
{
  DSK_CMDLINE_PERMIT_UNKNOWN_OPTIONS = (1<<0),
  DSK_CMDLINE_PERMIT_ARGUMENTS = (1<<1)
} DskCmdlineInitFlags;

typedef enum
{
  DSK_CMDLINE_MANDATORY = (1<<0),
  DSK_CMDLINE_REVERSED = (1<<1),        /* only for boolean w/ no arg */
  DSK_CMDLINE_PRINT_DEFAULT = (1<<2),
  DSK_CMDLINE_TAKES_ARGUMENT = (1<<3),  /* only needed for func-ptr */
  DSK_CMDLINE_REPEATABLE = (1<<4),
  DSK_CMDLINE_OPTIONAL = (1<<5),        /* only needed for func-ptr */

  _DSK_CMDLINE_IS_FOUR_BYTES = (1<<16)
} DskCmdlineFlags;

typedef dsk_boolean (*DskCmdlineCallback) (const char *arg_name,
                                           const char *arg_value,
                                           void       *callback_data,
                                           DskError  **error);
#define DSK_CMDLINE_CALLBACK_DECLARE(name) \
        dsk_boolean          name         (const char *arg_name, \
                                           const char *arg_value, \
                                           void       *callback_data, \
                                           DskError  **error)

void dsk_cmdline_init        (const char     *static_short_desc,
                              const char     *long_desc,
                              const char     *non_option_arg_desc,
                              DskCmdlineInitFlags flags);

void dsk_cmdline_add_int     (const char     *static_option_name,
                              const char     *static_description,
			      const char     *static_arg_description,
                              DskCmdlineFlags flags,
                              int            *value_out);
void dsk_cmdline_add_uint    (const char     *static_option_name,
                              const char     *static_description,
			      const char     *static_arg_description,
                              DskCmdlineFlags flags,
                              unsigned       *value_out);
void dsk_cmdline_add_double  (const char     *static_option_name,
                              const char     *static_description,
			      const char     *static_arg_description,
                              DskCmdlineFlags flags,
                              double         *value_out);
void dsk_cmdline_add_boolean (const char     *static_option_name,
                              const char     *static_description,
                              const char     *static_arg_description,
                              DskCmdlineFlags flags,
                              dsk_boolean    *value_out);
void dsk_cmdline_add_string  (const char     *static_option_name,
                              const char     *static_description,
			      const char     *static_arg_description,
                              DskCmdlineFlags flags,
                              char          **value_out);
void dsk_cmdline_add_func    (const char     *static_option_name,
                              const char     *static_description,
			      const char     *static_arg_description,
                              DskCmdlineFlags flags,
                              DskCmdlineCallback callback,
                              void           *callback_data);
void dsk_cmdline_permit_unknown_options (dsk_boolean permit);
void dsk_cmdline_permit_extra_arguments (dsk_boolean permit);

void dsk_cmdline_process_args(int            *argc_inout,
                              char         ***argv_inout);


dsk_boolean dsk_cmdline_try_process_args (int *argc_inout,
                                          char ***argv_inout,
                                          DskError **error);


void _dsk_cmdline_cleanup (void);
