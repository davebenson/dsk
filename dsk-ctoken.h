
typedef struct _DskCToken DskCToken;

typedef enum
{
  DSK_CTOKEN_TYPE_TOPLEVEL,
  DSK_CTOKEN_TYPE_PAREN,
  DSK_CTOKEN_TYPE_BRACE,
  DSK_CTOKEN_TYPE_BRACKET,
  DSK_CTOKEN_TYPE_DOUBLE_QUOTED_STRING,
  DSK_CTOKEN_TYPE_SINGLE_QUOTED_STRING,
  DSK_CTOKEN_TYPE_BACKTICK_STRING,
  DSK_CTOKEN_TYPE_OPERATOR,
  DSK_CTOKEN_TYPE_NUMBER,
  DSK_CTOKEN_TYPE_BAREWORD,
} DskCTokenType;

struct _DskCToken
{
  DskCToken *parent;
  DskCTokenType type;

  unsigned token_id;                    /* for use by caller */
  void *token;                          /* for use by caller */

  unsigned start_byte, n_bytes;		/* including brace/brackets */
  unsigned start_line, n_lines;		/* 1-indexed, per tradition */
  unsigned n_children;
  DskCToken *children;
};

typedef enum
{
  DSK_CTOKEN_SCAN_FLAGS_DEFAULT = 0
} DskCTokenScanFlags;

typedef struct _DskCTokenScannerConfig DskCTokenScannerConfig;
struct _DskCTokenScannerConfig
{
  /* Called for single and double quoted strings.
   * You can tell which it is since *str will be either ' or ".
   */
  dsk_boolean (*scan_quoted)    (const char  *str,
                                 const char  *end,
                                 unsigned    *n_used_out,
                                 unsigned    *token_id_opt_out,
                                 void       **token_opt_out,
                                 DskError   **error);

  /* If !support_backtick_strings, ` is treated as an operator */
  dsk_boolean support_backtick_strings;

  /* Called for non-grouping, non-quoting punctuation. */
  dsk_boolean (*scan_op)        (const char  *str,
                                 const char  *end,
                                 unsigned    *n_used_out,
                                 unsigned    *token_id_opt_out,
                                 void       **token_opt_out,
                                 DskError   **error);

  dsk_boolean (*scan_bareword)  (const char  *str,
                                 const char  *end,
                                 unsigned    *n_used_out,
                                 unsigned    *token_id_opt_out,
                                 void       **token_opt_out,
                                 DskError   **error);

  dsk_boolean (*scan_number)    (const char  *str,
                                 const char  *end,
                                 unsigned    *n_used_out,
                                 unsigned    *token_id_opt_out,
                                 void       **token_opt_out,
                                 DskError   **error);

  const char *error_filename;

  void        (*destruct_data)  (DskCToken   *token);
};
#define DSK_CTOKEN_SCANNER_CONFIG_DEFAULT                           \
{                                                                   \
  dsk_ctoken_scan_quoted__default,                                  \
  DSK_FALSE,                    /* no backtick string support */    \
  dsk_ctoken_scan_op__c,                                            \
  dsk_ctoken_scan_bareword__default,                                \
  dsk_ctoken_scan_number__c,                                        \
  NULL,                                                             \
  NULL                                                              \
}

DskCToken *dsk_ctoken_scan_str  (const char  *str,
                                 const char  *end,
                                 DskCTokenScannerConfig *config,
                                 DskError   **error);

void       dsk_ctoken_destroy   (DskCToken *top);



/* --- handlers --- */
dsk_boolean dsk_ctoken_scan_quoted__default    (const char  *str,
                                                const char  *end,
                                                unsigned    *n_used_out,
                                                unsigned    *token_id_opt_out,
                                                void       **token_opt_out,
                                                DskError   **error);
dsk_boolean dsk_ctoken_scan_op__c              (const char  *str,
                                                const char  *end,
                                                unsigned    *n_used_out,
                                                unsigned    *token_id_opt_out,
                                                void       **token_opt_out,
                                                DskError   **error);
dsk_boolean dsk_ctoken_scan_bareword__default  (const char  *str,
                                                const char  *end,
                                                unsigned    *n_used_out,
                                                unsigned    *token_id_opt_out,
                                                void       **token_opt_out,
                                                DskError   **error);
dsk_boolean dsk_ctoken_scan_number__c          (const char  *str,
                                                const char  *end,
                                                unsigned    *n_used_out,
                                                unsigned    *token_id_opt_out,
                                                void       **token_opt_out,
                                                DskError   **error);

