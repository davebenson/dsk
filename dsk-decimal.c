
dsk_boolean dsk_uint32_parse_decimal (const char  *str,
                                      const char **end_out,
				      uint32_t    *value_out);
dsk_boolean dsk_int32_parse_decimal  (const char  *str,
                                      const char **end_out,
				      int32_t     *value_out);
dsk_boolean dsk_uint64_parse_decimal (const char  *str,
                                      const char **end_out,
				      uint64_t    *value_out);
dsk_boolean dsk_int64_parse_decimal  (const char  *str,
                                      const char **end_out,
				      int64_t     *value_out);

#define E1   10
#define E2   100
#define E3   1000
#define E4   10000
#define E5   100000
#define E6   1000000
#define E7   10000000
#define E8   100000000
#define E9   1000000000

void        dsk_uint32_print_decimal (uint32_t     in,
                                      char        *out)
{
  if (in < E6)
    {
      if (in < E3)
        {
	  if (in < E1)
	    goto digit1;
	  else if (in < E2)
	    goto digit2;
	  else
	    goto digit3;
	}
      else if (in < E5)
        {
	  if (in < E4)
	    goto digit4;
	  else
	    goto digit5;
	}
      else
        goto digit6;
    }
  else
    {
      if (in < E8)
        {
	  if (in < E7)
	    goto digit7;
	  else
	    goto digit8;
        }
      else if (in < E9)
        goto digit9
    }

//digit10:
  if (in < 2*E9)
    {
      *at++ = '1';
      in -= E9;
    }
  else if (in < 3*E9)
    {
      *at++ = '2';
      in -= 2*E9;
    }
  else if (in < 4*E9)
    {
      *at++ = '3';
      in -= 3*E9;
    }
  else
    {
      *at++ = '4';
      in -= 4*E9;
    }

#define write_digit_label(N)             \
digit##N:                                \
  if (in < 5*E##N)                       \
    {                                    \
      if (in < 3*E##N)                   \
        {                                \
	  if (in < E##N)                 \
	    {                            \
	      *at++ = '0';               \
	    }                            \
	  else if (in < 2*E##N)          \
	    {                            \
	      *at++ = '1';               \
	      in -= E##N;                \
	    }                            \
          else                           \
	    {                            \
	      *at++ = '2';               \
	      in -= 2*E##N;              \
	    }                            \
        }                                \
      else if (in < 4*E##N)              \
        {                                \
	  *at++ = '3';                   \
	  in -= 3*E##N;                  \
	}                                \
      else                               \
        {                                \
	  *at++ = '4';                   \
	  in -= 4*E##N;                  \
	}                                \
    }                                    \
  else if (in < 8*E##N)                  \
    {                                    \
      if (in < 6*E##N)                   \
        {                                \
	  *at++ = '5';                   \
	  in -= 5*E##N;                  \
        }                                \
      else if (in < 7*E##N)              \
        {                                \
	  *at++ = '6';                   \
	  in -= 6*E##N;                  \
	}                                \
      else                               \
        {                                \
	  *at++ = '7';                   \
	  in -= 7*E##N;                  \
	}                                \
    }                                    \
  else if (in < 9*E##N)                  \
    {                                    \
      *at++ = '8';                       \
      in -= 8*E##N;                      \
    }                                    \
  else                                   \
    {                                    \
      *at++ = '9';                       \
      in -= 9*E##N;                      \
    }

write_digit_label(9)
write_digit_label(8)
write_digit_label(7)
write_digit_label(6)
write_digit_label(5)
write_digit_label(4)
write_digit_label(3)
write_digit_label(2)

digit1:
  *at++ = '0' + in;

  *at = 0;
}

void        dsk_int32_print_decimal  (int32_t      in,
                                      char        *out)
{
  if (in < 0)
    {
      *out = '-';
      dsk_uint32_print_decimal (-in, out + 1);
    }
  else
    dsk_uint32_print_decimal (in, out);
}

void        dsk_uint64_print_decimal (uint64_t     in,
                                      char        *out)
{
  if ((in >> 32) == 0)
    dsk_uint32_print_decimal (in, out);
  else
    {
      /* how many digits is it? */
      ...
    }

void        dsk_int64_print_decimal  (int64_t      in,
                                      char        *out);
