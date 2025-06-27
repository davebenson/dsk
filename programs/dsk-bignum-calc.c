
typedef enum
{
  LITERAL,
  BAREWORD,
  LPAREN,
  RPAREN,
  COMMA
} TokenType;

typedef struct Token Token;
struct Token
{
  const char *start;
  unsigned len;
  TokenType type;
};

static Token *
tokenize (const char *text, size_t *len_out)
{
  unsigned n = 0;
  unsigned alloced = 16;
  Token *arr = DSK_NEW_ARRAY (alloced, Token);
  while (*text)
    {
      Token tok;
      while (dsk_ascii_isspace (*text))
        text++;
      if (*text == 0)
        break;
      tok.start = text;
      tok.length = 1;
      switch (*text)
        {
        case '(':
          tok.type = LPAREN;
          break;
        case ')':
          tok.type = RPAREN;
          break;
        case ',':
          tok.type = COMMA;
          break;
        case '0':
          if (text[1] == 'x')
            {
              text += 2;
              while (('0' <= *text && *text <= '9')
                  || ('a' <= *text && *text <= 'f')
                  || ('A' <= *text && *text <= 'F'))
                text++;
            }
          else
            {
              text++;
              while ('0' <= *text && *text <= '7')
                text++;
            }
          tok.length = text - tok.start;
          tok.type = LITERAL;
          break;
        default:
          if (dsk_ascii_isdigit (*text))
            {
              text++;
              while ('0' <= *text && *text <= '9')
                text++;
              tok.length = text - tok.start;
              tok.type = LITERAL;
            }
          else if (dsk_ascii_isalpha (*text))
            {
              text++;
              while (dsk_ascii_isalnum (*text))
                text++;
              tok.length = text - tok.start;
              tok.type = BAREWORD;
            }
          else
            {
              dsk_die ("bad symbol starting with %s",
                       dsk_ascii_byte_name (*text));
            }
          break;
        }

      if (n == alloced)
        {
          alloced *= 2;
          arr = DSK_RENEW (Token, arr, alloced);
        }
      arr[n++] = tok;
    }
          
}


void
handle_line(const char *str, const char *filename, int lineno)
{
  for (
