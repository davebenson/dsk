#include "dsk.h"
#include <stdlib.h>
#include <assert.h>

/* Allocate children in a multiples of 4. */
#define CHILD_ALLOC_GRAN  4

/* max nesting depth */
#define MAX_DEPTH         256

#define LPAREN            '('
#define LBRACE            '{'
#define LBRACKET          '['
#define RPAREN            ')'
#define RBRACE            '}'
#define RBRACKET          ']'

typedef struct _ToplevelToken ToplevelToken;
struct _ToplevelToken
{
  DskCToken base;
  void        (*destruct_data)  (DskCToken   *token);
};



static DskCToken *
append_child (DskCToken    *parent,
              DskCTokenType type,
              unsigned       start_byte,
              const char    *start_char,
              unsigned       line)
{
  DskCToken *block;
  if (parent->n_children % CHILD_ALLOC_GRAN == 0)
    {
      size_t new_count = parent->n_children + CHILD_ALLOC_GRAN;
      size_t new_size = sizeof (DskCToken) * new_count;
      parent->children = dsk_realloc (parent->children, new_size);
    }

  block = parent->children + parent->n_children++;
  block->type = type;
  block->start_byte = start_byte;
  block->start_char = start_char;
  block->start_line = line;
  block->token_id = 0;
  block->token = NULL;
  block->n_children = 0;
  block->children = NULL;
  block->str = NULL;
  return block;
}

static void
setup_child_parents (DskCToken *tree)
{
  unsigned i;
  for (i = 0; i < tree->n_children; i++)
    {
      setup_child_parents (tree->children + i);
      tree->children[i].parent = tree;
    }
}

dsk_boolean 
dsk_ctoken_scan_quoted__default    (const char  *str,
                                    const char  *end,
                                    unsigned    *n_used_out,
                                    unsigned    *token_id_opt_out,
                                    void       **token_opt_out,
                                    DskError   **error)
{
  const char *at = str;
  char quote = *at++;         /* skip initial quote */
  DSK_UNUSED (token_id_opt_out);
  DSK_UNUSED (token_opt_out);
  while (at < end)
    {
      if (*at == quote)
        break;
      else if (*at == '\\')
        {
          if (at + 1 == end)
            {
              dsk_set_error (error,
                             "unterminated escape sequence in \" string");
              return DSK_FALSE;
            }
          at += 2;
        }
      else if (*at == '\n')
        {
          dsk_set_error (error, "unterminated \" string");
          return DSK_FALSE;
        }
      else
        at++;
    }
  if (at == end)
    {
      dsk_set_error (error, "got EOF in double-quoted string");
      return DSK_FALSE;
    }
  at++;
  *n_used_out = at - str;
  return DSK_TRUE;
}

dsk_boolean 
dsk_ctoken_scan_op__c    (const char  *str,
                          const char  *end,
                          unsigned    *n_used_out,
                          unsigned    *token_id_opt_out,
                          void       **token_opt_out,
                          DskError   **error)
{
  dsk_boolean tc = str + 1 < end;               /* two-character ops allowed */
  DSK_UNUSED (token_id_opt_out);
  DSK_UNUSED (token_opt_out);
  switch (*str)
    {
    case '!':   if (!tc) goto one_char;
                if (str[1] == '=') goto two_char;
                goto one_char;
    case '$':   goto one_char;
    case '&':   if (!tc) goto one_char;
                switch (str[1])
                  {
                  case '&': case '=': goto two_char;
                  }
                goto one_char;
    case '*':   if (!tc) goto one_char;
                switch (str[1])
                  {
                  case '=': goto two_char;
                  }
                goto one_char;
    case '+':   if (!tc) goto one_char;
                switch (str[1])
                  {
                  case '+': case '=': goto two_char;
                  }
                goto one_char;
    case ',':   goto one_char;
    case '-':   if (!tc) goto one_char;
                switch (str[1])
                  {
                  case '-': case '=': case '>': goto two_char;
                  }
                goto one_char;
    case '.':   if (!tc) goto one_char;
                switch (str[1])
                  {
                  case '=': goto two_char;
                  }
                goto one_char;
    case '/':   if (!tc) goto one_char;
                switch (str[1])
                  {
                  case '=': goto two_char;
                  }
                goto one_char;
    case ';':   goto one_char;
    case ':':   if (!tc) goto one_char;
                switch (str[1])
                  {
                  case ':': goto two_char;
                  }
                goto one_char;
    case '<':   if (!tc) goto one_char;
                switch (str[1])
                  {
                  case '<': case '=': goto two_char;
                  }
                goto one_char;
    case '=':   if (!tc) goto one_char;
                switch (str[1])
                  {
                  case '=': goto two_char;
                  }
                goto one_char;
    case '>':   if (!tc) goto one_char;
                switch (str[1])
                  {
                  case '>': case '=': goto two_char;
                  }
                goto one_char;
    case '?':   goto one_char;
    case '@':   goto one_char;
    case '^':   if (!tc) goto one_char;
                switch (str[1])
                  {
                  case '=': goto two_char;
                  }
                goto one_char;
    case '|': if (!tc) goto one_char;
                switch (str[1])
                  {
                  case '=': case '|': goto two_char;
                  }
                goto one_char;
    case '~':   goto one_char;
    default:    dsk_set_error (error, "bad punctuation character %s",
                               dsk_ascii_byte_name (*str)); return DSK_FALSE;
    }
one_char:
  *n_used_out = 1;
  return DSK_TRUE;
two_char:
  *n_used_out = 2;
  return DSK_TRUE;
}

dsk_boolean dsk_ctoken_scan_bareword__default  (const char  *str,
                                                const char  *end,
                                                unsigned    *n_used_out,
                                                unsigned    *token_id_opt_out,
                                                void       **token_opt_out,
                                                DskError   **error)
{
  const char *at = str;
  DSK_UNUSED (token_id_opt_out);
  DSK_UNUSED (token_opt_out);
  DSK_UNUSED (error);
  while (at < end && (dsk_ascii_isalnum (*at) || *at == '_'))
    at++;
  *n_used_out = at - str;
  return DSK_TRUE;
}

dsk_boolean dsk_ctoken_scan_number__c          (const char  *str,
                                                const char  *end,
                                                unsigned    *n_used_out,
                                                unsigned    *token_id_opt_out,
                                                void       **token_opt_out,
                                                DskError   **error)
{
  const char *at = str;
  DSK_UNUSED (token_id_opt_out);
  DSK_UNUSED (token_opt_out);
  if (*at == '0' && at + 1 < end && (at[1] == 'x' || at[1] == 'X'))
    {
      if (at + 2 == end)
        {
          dsk_set_error (error, "unexpected EOF after 0x");
          return DSK_FALSE;
        }
      if (!dsk_ascii_isxdigit (at[2]))
        {
          at += 2;
          goto unexpected_char;
        }
      at += 3;
      while (at < end && dsk_ascii_isxdigit (*at))
        at++;
      if (at < end && (*at == '.' || dsk_ascii_isalpha (*at)))
        goto unexpected_char;
      *n_used_out = at - str;
      return DSK_TRUE;
    }
  while (at < end && dsk_ascii_isdigit (*at))
    at++;
  if (at < end && *at == '.')
    {
      at++;
      goto fractional_part;
    }
  if (at < end && (*at == 'e' || *at == 'E'))
    {
      at++;
      goto exponent_part;
    }
  if (at < end && dsk_ascii_isalpha (*at))
    goto unexpected_char;
  *n_used_out = at - str;
  return DSK_TRUE;

fractional_part:
  while (at < end && dsk_ascii_isdigit (*at))
    at++;
  if (at < end && (*at == 'e' || *at == 'E'))
    {
      at++;
      goto exponent_part;
    }
  if (at < end && dsk_ascii_isalpha (*at))
    goto unexpected_char;
  *n_used_out = at - str;
  return DSK_TRUE;

exponent_part:
  if (at < end && (*at == '+' || *at == '-'))
    at++;
  if (at < end && dsk_ascii_isdigit (*at))
    at++;
  if (at < end && dsk_ascii_isalpha (*at))
    goto unexpected_char;
  *n_used_out = at - str;
  return DSK_TRUE;

unexpected_char:
  dsk_set_error (error, "got unexpected char %s after number",
                 dsk_ascii_byte_name (*at));
  return DSK_FALSE;
}

DskCToken *dsk_ctoken_scan_str (const char             *str,
                                const char             *end,
                                DskCTokenScannerConfig *config,
                                DskError              **error)
{
  DskCToken *rv = dsk_malloc0 (sizeof(ToplevelToken));
  const char *at = str;
  unsigned line = 1;
  unsigned stack_size;
  DskCToken *stack[MAX_DEPTH];
  rv->type = DSK_CTOKEN_TYPE_TOPLEVEL;
  rv->parent = NULL;
  rv->start_byte = 0;
  rv->start_char = str;
  rv->start_line = 1;
  rv->token_id = 0;
  rv->token = NULL;
  rv->n_children = 0;
  rv->children = NULL;
  rv->str = NULL;
  stack[0] = rv;
  stack_size = 1;

  while (at < end)
    {
      DskCToken *top = stack[stack_size-1];
      if (*at == LPAREN || *at == LBRACE || *at == LBRACKET)
        {
          DskCToken *block;
          if (stack_size == MAX_DEPTH)
            {
              dsk_set_error (error, "stack-overflow in nesting elements");
              goto got_error;
            }
          block = append_child (top, (*at == LPAREN) ? DSK_CTOKEN_TYPE_PAREN
                                  : (*at == LBRACKET) ? DSK_CTOKEN_TYPE_BRACKET
                                  : DSK_CTOKEN_TYPE_BRACE,
                                at - str, at, line);
          stack[stack_size++] = block;

          at++;
        }
      else if (*at == RPAREN || *at == RBRACE || *at == RBRACKET)
        {
          DskCTokenType close_type = (*at == RPAREN) ? DSK_CTOKEN_TYPE_PAREN
                                  : (*at == RBRACKET) ? DSK_CTOKEN_TYPE_BRACKET
                                  : DSK_CTOKEN_TYPE_BRACE;
          if (top->type != close_type)
            {
              if (top->type == DSK_CTOKEN_TYPE_TOPLEVEL)
                dsk_set_error (error, "unexpected %c", *at);
              else if (top->type == DSK_CTOKEN_TYPE_PAREN)
                dsk_set_error (error, "got %c, expected )", *at);
              else if (top->type == DSK_CTOKEN_TYPE_BRACKET)
                dsk_set_error (error, "got %c, expected ]", *at);
              else if (top->type == DSK_CTOKEN_TYPE_BRACE)
                dsk_set_error (error, "got %c, expected }", *at);
              else
                dsk_assert_not_reached ();
              goto got_error;
            }
          at++;
          top->n_bytes = (at - str) - top->start_byte;
          top->n_lines = line + 1 - top->start_line;
          stack_size--;
        }
      else if (*at == '"' || *at == '\''
            || (config->support_backtick_strings && *at == '`'))
        {
          DskCToken *block;
          unsigned n_used = 0;
          block = append_child (top,
                                *at == '"' ? DSK_CTOKEN_TYPE_DOUBLE_QUOTED_STRING
                                : *at == '\'' ? DSK_CTOKEN_TYPE_SINGLE_QUOTED_STRING
                                : DSK_CTOKEN_TYPE_BACKTICK_STRING,
                                at - str, at, line);
          if (!config->scan_quoted (at, end, &n_used, &block->token_id,
                                    &block->token, error))
            goto got_error;
          if (n_used == 0)
            {
              dsk_set_error (error, "scan_quoted did not set n_used");
              goto got_error;
            }
          while (n_used--)
            if (*at++ == '\n')
              line++;
          block->n_bytes = (at - str) - block->start_byte;
          block->n_lines = line + 1 - block->start_line;
        }
      else if (*at != '_' && dsk_ascii_ispunct (*at))
        {
          unsigned n_used = 0;
          DskCToken *block;
          block = append_child (top, DSK_CTOKEN_TYPE_OPERATOR,
                                at - str, at, line);
          if (!config->scan_op (at, end, &n_used,
                                &block->token_id, &block->token,
                                error))
            goto got_error;
          if (n_used == 0)
            {
              dsk_set_error (error, "scan_op did not set n_used");
              goto got_error;
            }
          block->n_lines = 1;
          block->n_bytes = n_used;
          at += n_used;
        }
      else if (dsk_ascii_isalpha (*at) || *at == '_')
        {
          unsigned n_used = 0;
          DskCToken *block;
          block = append_child (top, DSK_CTOKEN_TYPE_BAREWORD,
                                at - str, at, line);
          if (!config->scan_bareword (at, end, &n_used,
                                      &block->token_id, &block->token,
                                      error))
            goto got_error;
          if (n_used == 0)
            {
              dsk_set_error (error, "scan_bareword did not set n_used");
              goto got_error;
            }
          block->n_lines = 1;
          block->n_bytes = n_used;
          at += n_used;
        }
      else if (dsk_ascii_isdigit (*at))
        {
          unsigned n_used = 0;
          DskCToken *block;
          block = append_child (top, DSK_CTOKEN_TYPE_NUMBER,
                                at - str, at, line);
          if (!config->scan_number (at, end, &n_used,
                                    &block->token_id, &block->token,
                                    error))
            goto got_error;
          if (n_used == 0)
            {
              dsk_set_error (error, "scan_number did not set n_used");
              goto got_error;
            }
          block->n_lines = 1;
          block->n_bytes = n_used;
          at += n_used;
        }
      else if (*at == '\n')
        {
          at++;
          line++;
        }
      else if (dsk_ascii_isspace (*at))
        {
          at++;
        }
      else
        {
          dsk_set_error (error, "unexpected character %s",
                         dsk_ascii_byte_name (*at));
          goto got_error;
        }
    }
  if (stack_size != 1)
    {
      DskCToken *top = stack[stack_size-1];
      const char * exp = top->type == DSK_CTOKEN_TYPE_PAREN ? "')'"
                       : top->type == DSK_CTOKEN_TYPE_BRACE ? "'}'"
                       : top->type == DSK_CTOKEN_TYPE_BRACKET ? "']'"
                       : "*unknown*";
      dsk_set_error (error, "unexpected end-of-file (expected %s)", exp);
      goto got_error;
    }
  rv->n_bytes = (at - str) - rv->start_byte;
  rv->n_lines = line;
  if (end > str && *(end-1) == '\n')
    rv->n_lines -= 1;

  setup_child_parents (rv);
  return rv;

got_error:
  if (config->error_filename)
    dsk_add_error_suffix (error, " (%s:%u; byte %u)",
                          config->error_filename,
                          line, (unsigned)(at - str));
  else
    dsk_add_error_suffix (error, " (line %u; byte %u)",
                          line, (unsigned)(at - str));
  dsk_ctoken_destroy (rv);
  return NULL;
}

const char *dsk_ctoken_force_str (DskCToken *token)
{
  if (token->str == NULL)
    token->str = dsk_strndup (token->n_bytes, token->start_char);
  return token->str;
}

static void
clike_block_free_array (unsigned n, DskCToken *at,
                        void (*destruct)(DskCToken *))
{
  unsigned i;
  if (destruct)
    destruct (at);
  for (i = 0; i < n; i++)
    {
      if (at[i].str)
        dsk_free (at[i].str);
      if (at[i].children)
        clike_block_free_array (at[i].n_children, at[i].children, destruct);
    }

  free (at);
}

void dsk_ctoken_destroy (DskCToken *top)
{
  dsk_assert (top->type == DSK_CTOKEN_TYPE_TOPLEVEL);
  clike_block_free_array (1, top,
                          ((ToplevelToken*)top)->destruct_data);
}
