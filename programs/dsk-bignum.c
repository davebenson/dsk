#include "../dsk.h"

typedef enum
{
  OUTPUT_MODE_C,
  OUTPUT_MODE_HEX,
  OUTPUT_MODE_DEC
} OutputMode;

typedef enum
{
  TOKEN_TYPE_NUM,
  TOKEN_TYPE_LPAREN,
  TOKEN_TYPE_RPAREN,
  TOKEN_TYPE_BAREWORD,  /// ascii, trimmed
  TOKEN_TYPE_STAR,
  TOKEN_TYPE_PLUS,
  TOKEN_TYPE_MINUS,
  TOKEN_TYPE_SLASH,
  TOKEN_TYPE_PERCENT,
  TOKEN_TYPE_COMMA,
} TokenType;

typedef struct Token Token;
struct Token
{
  Token *prev, *next;
  TokenType type;
  const char *start, *end;
  union {
    Num *num;                             // optional
    Num **num_list;
    struct { Token *first, *last; } token_list;
  };
};

static bool
handle_output_mode (const char *arg_name,
                    const char *arg_value,
                    void       *callback_data,
                    DskError  **error)
{
...
}

static Num *
parse_and_eval (unsigned len, const char *str)
{
}

int main(int argc, char **argv)
{
  OutputMode output_mode = OUTPUT_MODE_C;
  dsk_cmdline_init ("perform calculation with big numbers",
                    "perform calculation with big numbers",
                    "EXPR",
                    DSK_CMDLINE_PERMIT_ARGUMENTS);
  dsk_cmdline_add_func ("output-mode",
                        "output mode: one of c, hex, dec",
                        "MODE",
                        0,
                        handle_output_mode,
                        &output_mode);

void dsk_cmdline_add_func    (const char     *static_option_name,
                              const char     *static_description,
			      const char     *static_arg_description,
                              DskCmdlineFlags flags,
                              DskCmdlineCallback callback,
                              void           *callback_data);

void dsk_cmdline_begin_mode             (const char     *mode,
                                         const char     *static_short_desc,
                                         const char     *static_long_desc,
                                         const char     *non_option_arg_desc,
                                         DskVoidFunc     mode_callback,
                                         void           *mode_user_data);
