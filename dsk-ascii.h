
extern unsigned char dsk_ascii_chartable[256];

/* NOTE: NUL is NOT a space */
#define dsk_ascii_isspace(c)  (dsk_ascii_chartable[(unsigned char)(c)] & 1)
#define dsk_ascii_islower(c)  (dsk_ascii_chartable[(unsigned char)(c)] & 2)
#define dsk_ascii_isupper(c)  (dsk_ascii_chartable[(unsigned char)(c)] & 4)
#define dsk_ascii_isdigit(c)  (dsk_ascii_chartable[(unsigned char)(c)] & 8)
#define dsk_ascii_isxdigit(c) (dsk_ascii_chartable[(unsigned char)(c)] & 16)
#define dsk_ascii_is_minus_or_underscore(c)  (dsk_ascii_chartable[(unsigned char)(c)] & 32)
#define dsk_ascii_ispunct(c)  (dsk_ascii_chartable[(unsigned char)(c)] & 64)

/* used to decide if an ascii character can be jammed into a C-quoted
   (or JSON-quoted) string without quoting. */
#define dsk_ascii_is_no_cquoting_required(c) \
                              (dsk_ascii_chartable[(unsigned char)(c)] & 128)

/* isalpha = isupper || is_lower */
#define dsk_ascii_isalpha(c)  (dsk_ascii_chartable[(unsigned char)(c)] & 6)
/* isalnum = isupper || is_lower || is_digit */
#define dsk_ascii_isalnum(c)  (dsk_ascii_chartable[(unsigned char)(c)] & 14)
/* istoken = isupper || is_lower || is_digit || is_minus_or_underscore */
#define dsk_ascii_istoken(c)  (dsk_ascii_chartable[(unsigned char)(c)] & 46)


int dsk_ascii_xdigit_value (int c);
int dsk_ascii_digit_value (int c);
extern char dsk_ascii_hex_digits[16];
extern char dsk_ascii_uppercase_hex_digits[16];

static inline char dsk_ascii_tolower (int c)
{
  return 'A' <= c && c <= 'Z' ? (c + ('a' - 'A')) : c;
}
static inline char dsk_ascii_toupper (int c)
{
  return 'a' <= c && c <= 'z' ? (c - ('a' - 'A')) : c;
}
const char *dsk_ascii_byte_name(unsigned char byte);

int dsk_ascii_strcasecmp  (const char *a, const char *b);
int dsk_ascii_strncasecmp (const char *a, const char *b, size_t max_len);
void dsk_ascii_strchomp (char *chomp);

void  dsk_ascii_strup   (char *str);
void  dsk_ascii_strdown (char *str);

#define DSK_ASCII_SKIP_SPACE(ptr) \
      do { while (dsk_ascii_isspace (*(ptr))) ptr++; } while(0)
#define DSK_ASCII_SKIP_NONSPACE(ptr) \
      do { while (*(ptr) && !dsk_ascii_isspace (*(ptr))) (ptr)++; } while(0)

