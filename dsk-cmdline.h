
typedef enum
{
  DSK_CMDLINE_PERMIT_UNKNOWN_OPTIONS = (1<<0),
  DSK_CMDLINE_PERMIT_ARGUMENTS = (1<<1),
  DSK_CMDLINE_DO_NOT_MODIFY_ARGV = (1<<2),
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

typedef bool (*DskCmdlineCallback) (const char *arg_name,
                                    const char *arg_value,
                                    void       *callback_data,
                                    DskError  **error);
#define DSK_CMDLINE_CALLBACK_DECLARE(name) \
        bool          name         (const char *arg_name, \
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
void dsk_cmdline_add_int64   (const char     *static_option_name,
                              const char     *static_description,
			      const char     *static_arg_description,
                              DskCmdlineFlags flags,
                              int64_t        *value_out);
void dsk_cmdline_add_uint64  (const char     *static_option_name,
                              const char     *static_description,
			      const char     *static_arg_description,
                              DskCmdlineFlags flags,
                              uint64_t       *value_out);
void dsk_cmdline_add_double  (const char     *static_option_name,
                              const char     *static_description,
			      const char     *static_arg_description,
                              DskCmdlineFlags flags,
                              double         *value_out);
void dsk_cmdline_add_boolean (const char     *static_option_name,
                              const char     *static_description,
                              const char     *static_arg_description,
                              DskCmdlineFlags flags,
                              bool    *value_out);
void dsk_cmdline_add_string  (const char     *static_option_name,
                              const char     *static_description,
			      const char     *static_arg_description,
                              DskCmdlineFlags flags,
                              const char    **value_out);
void dsk_cmdline_add_func    (const char     *static_option_name,
                              const char     *static_description,
			      const char     *static_arg_description,
                              DskCmdlineFlags flags,
                              DskCmdlineCallback callback,
                              void           *callback_data);

void dsk_cmdline_add_shortcut(char            shortcut,
                              const char     *option_name);


/* Handling of extra arguments and options.  (Here, _arguments_
   are command-line elements that DO NOT begin with "-";
   _options_ DO begin with "-".) */
typedef bool (*DskCmdlineArgumentHandler) (const char *argument,
                                                  DskError  **error);
void dsk_cmdline_set_argument_handler   (DskCmdlineArgumentHandler handler);
void dsk_cmdline_permit_unknown_options (bool     permit);
void dsk_cmdline_permit_extra_arguments (bool     permit);


/* Functions to make it easy to warn user about incompatible options.  */
void dsk_cmdline_mutually_exclusive     (bool     one_required,
                                         const char     *arg_1,
                                         ...) DSK_GNUC_NULL_TERMINATED();
void dsk_cmdline_mutually_exclusive_v   (bool     one_required,
                                         unsigned        n_excl,
                                         char          **excl);

/* Modal program support. */
extern void *dsk_cmdline_mode_user_data;
void dsk_cmdline_begin_mode             (const char     *mode,
                                         const char     *static_short_desc,
                                         const char     *static_long_desc,
                                         const char     *non_option_arg_desc,
                                         DskVoidFunc     mode_callback,
                                         void           *mode_user_data);
void dsk_cmdline_add_mode_alias         (const char     *alias);
void dsk_cmdline_end_mode               (void);

/* Wrapper program support. After the first non-option given to a
   wrapper program, command-line processing ceases. */
void dsk_cmdline_program_wrapper        (bool     is_wrapper);

/* --- Processing the Command-line --- */

//
// Process arguments.
//

// This terminates with usage-message on error.
void        dsk_cmdline_process_args     (int            *argc_inout,
                                          char         ***argv_inout);

// This requires caller to handle errors.
bool        dsk_cmdline_try_process_args (int *argc_inout,
                                          char ***argv_inout,
                                          DskError **error);

void dsk_cmdline_print_usage(void);

void _dsk_cmdline_cleanup (void);
