#include <string.h>
#include <stdio.h>
#include "dsk.h"

#define EPOCH_YEAR              1970

#define THREE_CHAR_CODE(a,b,c) \
( (((unsigned)(uint8_t)a) << 16) \
+ (((unsigned)(uint8_t)b) << 8) \
+ (((unsigned)(uint8_t)c)) )
static int
get_month_from_code (unsigned code)
{
  switch (code)
    {
    case THREE_CHAR_CODE('j', 'a', 'n'): return 1;
    case THREE_CHAR_CODE('f', 'e', 'b'): return 2;
    case THREE_CHAR_CODE('m', 'a', 'r'): return 3;
    case THREE_CHAR_CODE('a', 'p', 'r'): return 4;
    case THREE_CHAR_CODE('m', 'a', 'y'): return 5;
    case THREE_CHAR_CODE('j', 'u', 'n'): return 6;
    case THREE_CHAR_CODE('j', 'u', 'l'): return 7;
    case THREE_CHAR_CODE('a', 'u', 'g'): return 8;
    case THREE_CHAR_CODE('s', 'e', 'p'): return 9;
    case THREE_CHAR_CODE('o', 'c', 't'): return 10;
    case THREE_CHAR_CODE('n', 'o', 'v'): return 11;
    case THREE_CHAR_CODE('d', 'e', 'c'): return 12;
    default: return 0;
    }
}

static dsk_boolean
parse_generic (const char *format,
               const char *str,
               char      **end,
               DskDate    *out,
               DskError  **error)
{
  unsigned code;
  const char *at = str;
  while (*format)
    {
      if (*format == ' ')
        {
          while (dsk_ascii_isspace (*at))
            at++;
          format++;
          continue;
        }
      if (*format == '-' || *format == ':' || *format == ',' || *format == 'T')
        {
          if (*at != *format)
            {
              dsk_set_error (error, "expected '%c' in date (offset %u)", *format, (unsigned)(at-str));
              return DSK_FALSE;
            }
          at++;
          format++;
          continue;
        }
      dsk_assert (*format == '%');
      switch (format[1])
        {
        case 'A':
          while (dsk_ascii_isalpha (*at))
            at++;
          format += 2;
          break;
        case 'Y':
          if (!dsk_ascii_isdigit (at[0]) 
           || !dsk_ascii_isdigit (at[1]) 
           || !dsk_ascii_isdigit (at[2]) 
           || !dsk_ascii_isdigit (at[3]))
            {
              dsk_set_error (error, "expected 4-digit year");
              return DSK_FALSE;
            }
          out->year = (unsigned)(at[0] - '0') * 1000
                    + (unsigned)(at[1] - '0') * 100
                    + (unsigned)(at[2] - '0') * 10
                    + (unsigned)(at[3] - '0');
          at += 4;
          format += 2;
          break;
        case 'y':
          if (!dsk_ascii_isdigit (at[0]) 
           || !dsk_ascii_isdigit (at[1]) )
            {
              dsk_set_error (error, "expected year");
              return DSK_FALSE;
            }
          if (dsk_ascii_isdigit (at[2]))
            {
              if (!dsk_ascii_isdigit (at[3]))
                {
                  dsk_set_error (error, "3-digit year?");
                  return DSK_FALSE;
                }
              out->year = (unsigned)(at[0] - '0') * 1000
                        + (unsigned)(at[1] - '0') * 100
                        + (unsigned)(at[2] - '0') * 10
                        + (unsigned)(at[3] - '0');
              at += 4;
            }
          else
            {
              out->year = (unsigned)(at[0] - '0') * 10
                        + (unsigned)(at[1] - '0');
              if (out->year < 70)
                out->year += 2000;
              else
                out->year += 1900;
              at += 2;
            }
          format += 2;
          break;
        case 'd':
          if (!dsk_ascii_isdigit (at[0]) || !dsk_ascii_isdigit (at[1]))
            {
              dsk_set_error (error, "expected two digit day-of-month");
              return DSK_FALSE;
            }
          out->day = (at[0] - '0') * 10 + (at[1] - '0');
          at += 2;
          format += 2;
          break;
        case 'D':
          if (dsk_ascii_isspace (at[0]) && dsk_ascii_isdigit (at[1]))
            {
              out->day = (at[1] - '0');
              at += 2;
            }
          else if (dsk_ascii_isdigit (at[0]) && dsk_ascii_isdigit (at[1]))
            {
              out->day = (at[0] - '0') * 10 + (at[1] - '0');
              at += 2;
            }
          else if (dsk_ascii_isdigit (at[0]))
            {
              out->day = (at[0] - '0');
              at += 1;
            }
          else
            {
              dsk_set_error (error, "expected day-of-month");
              return DSK_FALSE;
            }
          format += 2;
          break;
        case 'b':
          /* accept 3-letter month abbrev */
          if (!dsk_ascii_isalpha (at[0])
           || !dsk_ascii_isalpha (at[1])
           || !dsk_ascii_isalpha (at[2]))
            {
              dsk_set_error (error, "expected three letter month abbrev (at offset %u)",
                             (unsigned)(at-str));
              return DSK_FALSE;
            }
          code = THREE_CHAR_CODE (dsk_ascii_tolower (at[0]),
                                  dsk_ascii_tolower (at[1]),
                                  dsk_ascii_tolower (at[2]));
          out->month = get_month_from_code (code);
          if (out->month == 0)
            {
              dsk_set_error (error, "invalid month name (at offset %u)",
                             (unsigned)(at-str));
              return DSK_FALSE;
            }
          format += 2;
          at += 3;
          break;
        case 'm':
          /* accept 2-digit month number */
          if (!dsk_ascii_isdigit (at[0]) 
           || !dsk_ascii_isdigit (at[1]))
            {
              dsk_set_error (error, "expected two digit month number (at offset %u)",
                             (unsigned)(at-str));
              return DSK_FALSE;
            }
          out->month = (at[0] - '0') * 10 + (at[1] - '0');
          at += 2;
          format += 2;
          break;

        case 'H':
          if (!dsk_ascii_isdigit (at[0]) 
           || !dsk_ascii_isdigit (at[1]))
            {
              dsk_set_error (error, "expected two digit hour (at offset %u)",
                             (unsigned)(at-str));
              return DSK_FALSE;
            }
          out->hour = (at[0] - '0') * 10 + (at[1] - '0');
          at += 2;
          format += 2;
          break;
        case 'M':
          if (!dsk_ascii_isdigit (at[0]) 
           || !dsk_ascii_isdigit (at[1]))
            {
              dsk_set_error (error, "expected two digit minute (at offset %u)",
                             (unsigned)(at-str));
              return DSK_FALSE;
            }
          out->minute = (at[0] - '0') * 10 + (at[1] - '0');
          at += 2;
          format += 2;
          break;
        case 'S':
          if (!dsk_ascii_isdigit (at[0]) 
           || !dsk_ascii_isdigit (at[1]))
            {
              dsk_set_error (error, "expected two digit second (at offset %u)",
                             (unsigned)(at-str));
              return DSK_FALSE;
            }
          out->second = (at[0] - '0') * 10 + (at[1] - '0');
          at += 2;
          format += 2;
          if (*at == '.')                       /* skip fractional second */
            {
              at++;
              while (dsk_ascii_isdigit (*at))
                at++;
            }
          break;
        case 'Z':
          {
            char *end;
            /* timezone */
            if (!dsk_date_parse_timezone (at, &end, &out->zone_offset))
              {
                dsk_set_error (error, "error parsing timezone (at %.5s)", at);
                return DSK_FALSE;
              }
            at = end;
            format += 2;
          }
          break;
        }   
    }
  if (end)
    *end = (char*) at;
  return DSK_TRUE;
}



/* Parse either RFC 822/1123 dates (Sun, 06 Nov 1994 08:49:37 GMT)
 * or RFC 850/1036 dates (Sunday, 06-Nov-94 08:49:37 GMT)
 * or ANSI C dates (Sun Nov  6 08:49:37 1994)
 * or ISO 8601 dates (2009-02-12 T 14:32:61.1+01:00) (see RFC 3339)
 */
dsk_boolean dsk_date_parse   (const char *str,
                              char      **end,
                              DskDate    *out,
                              DskError  **error)
{
  unsigned n_alpha = 0;
  while (dsk_ascii_isalpha (str[n_alpha]))
    n_alpha++;
  memset (out, 0, sizeof (DskDate));
  if (n_alpha == 3 && str[3] == ',')
    {
      /* RFC 822/1123 */
      return parse_generic ("%A , %D %b %Y %H:%M:%S %Z", str, end, out, error);
    }
  else if (n_alpha == 3 && str[3] == ' ')
    {
      /* ANSI C */
      return parse_generic ("%A %b %D %H:%M:%S %Y", str, end, out, error);
    }
  else if (n_alpha > 3 && str[n_alpha] == ',')
    {
      /* RFC 850/1036 */
      return parse_generic ("%A, %D-%b-%y %H:%M:%S %Z", str, end, out, error);
    }
  else if (n_alpha == 0
           && dsk_ascii_isdigit (str[0])
           && dsk_ascii_isdigit (str[1])
           && dsk_ascii_isdigit (str[2])
           && dsk_ascii_isdigit (str[3])
           && str[4] == '-')
    {
      /* ISO 8601 */
      /* example: "2009-02-12 T 14:32:61.1+01:00" (see RFC 3339) */
      return parse_generic ("%Y-%m-%D T %H:%M:%S%Z", str, end, out, error);
    }
  else
    {
      dsk_set_error (error, "unrecognized date format");
      return DSK_FALSE;
    }
}
static const unsigned month_starts_in_days[12]
                               = { 0,         /* jan has 31 */
                                   31,        /* feb has 28 */
                                   59,        /* mar has 31 */
                                   90,        /* apr has 30 */
                                   120,       /* may has 31 */
                                   151,       /* jun has 30 */
                                   181,       /* jul has 31 */
                                   212,       /* aug has 31 */
                                   243,       /* sep has 30 */
                                   273,       /* oct has 31 */
                                   304,       /* nov has 30 */
                                   334,       /* dec has 31 */
                                   /*365*/ };

/* 'unixtime' here is seconds since epoch.
   If the date is before the epoch (Jan 1, 1970 00:00 GMT),
   then it is negative. */
/* Behavior of dsk_date_to_unixtime() is undefined if any
   of the fields are out-of-bounds.
   Behavior of dsk_unixtime_to_date() is defined for any time
   whose year fits in an unsigned integer. (therefore, no B.C. dates)
 */
/* dsk_unixtime_to_date() always sets the date_out->zone_offset to 0 (ie GMT) */
int64_t     dsk_date_to_unixtime (const DskDate *date)
{
  unsigned days_since_epoch, secs_since_midnight;

  if ((unsigned)date->month > 12
   || date->day < 1 || date->day > 31
   || date->hour >= 24
   || date->minute >= 60
   || date->second >= 61)   /* ??? are leap seconds meaningful in gmt */
    return (int64_t) -1;

  days_since_epoch = dsk_date_get_days_since_epoch (date);
  secs_since_midnight = date->hour * 3600
                      + date->minute * 60
                      + date->second;
  return (int64_t) days_since_epoch * 86400LL
       + (int64_t) secs_since_midnight
       - date->zone_offset * 60;

}

void        dsk_unixtime_to_date (int64_t  unixtime,
                                  DskDate *date)
{
  unsigned days_since_epoch = unixtime / 86400;
  unsigned secs_since_midnight = unixtime % 86400;
  unsigned year;
  int64_t delta;
  dsk_boolean first_is_leap = DSK_FALSE;

  /* --- figure out the year --- */
  /* This is actually the hard part, b/c of leap years being so random.
     The invariant is this:  start_day(year) + delta == days_since_epoch
     where start_day(year) returns the days since epoch of Jan 1 of the given year.

     The goal is to reduce delta to less than one year --
     at that point, we know the year
     and the day within the year (ie delta). */

  /* which 400 year period are we in? (period ending Jan 1 2000 is 0, we
   are in period '1', etc)  the time at period = 1 was 946684800 (in 2000),
   the period is 86400*(365*400 + 97) */
#define JAN1_2000          10957
#define FOURHUND_PERIOD    (365*400+97)
#define JAN1_1600          (JAN1_2000-FOURHUND_PERIOD)
#define JAN1_2400          (JAN1_2000+FOURHUND_PERIOD)
  if (days_since_epoch < JAN1_2000)
    {
      year = 1600;
      delta = days_since_epoch - JAN1_1600;
    }
  else if (days_since_epoch >= JAN1_2400)
    {
      year = 2000 + (days_since_epoch - JAN1_2000) / FOURHUND_PERIOD;
      delta = (days_since_epoch - JAN1_2000) % FOURHUND_PERIOD;
      while (delta >= FOURHUND_PERIOD)
        {
          year += 400;
          delta -= FOURHUND_PERIOD;
        }
    }
  else
    {
      year = 2000;
      delta = days_since_epoch - JAN1_2000;
    }

  /* note: now delta < 400 years */

  /* the first hundred year period is 100*365+25 days;
     the next three are 100*365+24 days */
  if (delta < 100*365+25)
    {
      first_is_leap = DSK_TRUE;
    }
  else if (delta < 200*365+49)
    {
      year += 100;
      delta -= 100*365+25;
    }
  else if (delta < 300*365+73)
    {
      year += 200;
      delta -= 200*365+49;
    }
  else
    {
      year += 300;
      delta -= 300*365+73;
    }

  /* note: now delta < 100 years */

  /* offset into 4 year period */
  if (!first_is_leap)
    {
      if (delta >= 365*4)
        {
          year += 4;
          delta -= 365*4;
          first_is_leap = DSK_TRUE;
        }
    }
  year += (delta / (365*4+1)) * 4;
  delta %= (365*4+1);

  /* note: now delta < 4 years */

  /* offset into 1 year period */
  if (first_is_leap)
    {
      if (delta >= 366)
        {
          first_is_leap = DSK_FALSE;
          year += 1 + (delta-366)/365;
          delta = (delta - 1) % 365;
        }
    }
  else
    {
      year += delta / 365;
      delta %= 365;
    }

  /* Ok, so now, we have the year!
   * Figure the month, use fairly gruesomely hardcoded stuff! */
  date->year = year;
  if (delta < 31) { date->month = 1; }
  else if (delta < 31+28+first_is_leap) { date->month = 2; delta -= 31; }
  else
    {
      if (first_is_leap)
        delta--;
      if (delta < 90) { date->month = 3; delta -= 59; }
      else if (delta < 120) { date->month = 4; delta -= 90; }
      else if (delta < 151) { date->month = 5; delta -= 120; }
      else if (delta < 181) { date->month = 6; delta -= 151; }
      else if (delta < 212) { date->month = 7; delta -= 181; }
      else if (delta < 243) { date->month = 8; delta -= 212; }
      else if (delta < 273) { date->month = 9; delta -= 243; }
      else if (delta < 304) { date->month = 10; delta -= 273; }
      else if (delta < 334) { date->month = 11; delta -= 304; }
      else { date->month = 12; delta -= 334; }
    }

  /* note: now delta is less than a month */

  date->day = delta + 1;
  date->hour = secs_since_midnight / 3600;
  date->minute = secs_since_midnight / 60 % 60;
  date->second = secs_since_midnight % 60;
  date->zone_offset = 0;
}


/* we recognise: UT, UTC, GMT; EST EDT CST CDT MST MDT PST PDT [A-Z]
   and the numeric formats: +#### and -#### */
dsk_boolean dsk_date_parse_timezone (const char *at,
                                     char **end,
				     int *zone_offset_out)
{
  if (dsk_ascii_isalpha (*at) && !dsk_ascii_isalpha (at[1]))
    {
      /* one-letter timezone */
      if (*at == 'z' || *at == 'Z')
        *zone_offset_out = 0;
      else
        return DSK_FALSE;
      if (end)
        *end = (char*)at + 1;
      return DSK_TRUE;
    }
  switch (*at)
    {
    case 'u': case 'U':
      if ((at[1] == 't' || at[1] == 'T')                /* UT */
       && !dsk_ascii_isalpha (at[2]))
        {
          if (end)
            *end = (char*)at + 2;
          *zone_offset_out = 0;
          return DSK_TRUE;
        }
      if ((at[1] == 't' || at[1] == 'T')                /* UTC */
       && (at[2] == 'c' || at[2] == 'C'))
        {
          if (end)
            *end = (char*)at + 3;
          *zone_offset_out = 0;
          return DSK_TRUE;
        }
      return DSK_FALSE;
    case 'g': case 'G':
      if ((at[1] == 'm' || at[1] == 'M')                /* GMT */
       && (at[2] == 't' || at[2] == 'T'))
        {
          if (end)
            *end = (char*)at + 3;
          *zone_offset_out = 0;
          return DSK_TRUE;
        }
      return DSK_FALSE;
    case 'e': case 'E':
      if (!(at[2] == 't' || at[2] == 'T'))
        return DSK_FALSE;
      if (at[1] == 's' || at[1] == 'S')                 /* EST */
        {
          *zone_offset_out = -5*60;
          if (end)
            *end = (char*)at + 3;
          return DSK_TRUE;
        }
      if (at[1] == 'd' || at[1] == 'D')
        {
          *zone_offset_out = -4*60;
          if (end)
            *end = (char*)at + 3;
          return DSK_TRUE;
        }
      return DSK_FALSE;
      
    case 'c': case 'C':
      if (!(at[2] == 't' || at[2] == 'T'))
        return DSK_FALSE;
      if (at[1] == 's' || at[1] == 'S')                 /* CST */
        {
          *zone_offset_out = -6*60;
          if (end)
            *end = (char*)at + 3;
          return DSK_TRUE;
        }
      if (at[1] == 'd' || at[1] == 'D')                 /* CDT */
        {
          *zone_offset_out = -5*60;
          if (end)
            *end = (char*)at + 3;
          return DSK_TRUE;
        }
      return DSK_FALSE;
    case 'm': case 'M':
      if (!(at[2] == 't' || at[2] == 'T'))
        return DSK_FALSE;
      if (at[1] == 's' || at[1] == 'S')                 /* MST */
        {
          *zone_offset_out = -7*60;
          if (end)
            *end = (char*)at + 3;
          return DSK_TRUE;
        }
      if (at[1] == 'd' || at[1] == 'D')                 /* MDT */
        {
          *zone_offset_out = -6*60;
          if (end)
            *end = (char*)at + 3;
          return DSK_TRUE;
        }
      return DSK_FALSE;
    case 'p': case 'P':
      if (!(at[2] == 't' || at[2] == 'T'))
        return DSK_FALSE;
      if (at[1] == 's' || at[1] == 'S')                 /* PST */
        {
          *zone_offset_out = -8*60;
          if (end)
            *end = (char*)at + 3;
          return DSK_TRUE;
        }
      if (at[1] == 'd' || at[1] == 'D')                 /* PDT */
        {
          *zone_offset_out = -7*60;
          if (end)
            *end = (char*)at + 3;
          return DSK_TRUE;
        }
      return DSK_FALSE;

    case '+':
    case '-':
      {
        unsigned has_colon = 0;
        if (!dsk_ascii_isdigit (at[1])
         || !dsk_ascii_isdigit (at[2]))
          return DSK_FALSE;
        if (at[3] == ':')
          has_colon = 1;
        if (!dsk_ascii_isdigit (at[3+has_colon])
         || !dsk_ascii_isdigit (at[4+has_colon]))
          return DSK_FALSE;
        *zone_offset_out = dsk_ascii_digit_value (at[1]) * 600
                         + dsk_ascii_digit_value (at[2]) * 60
                         + dsk_ascii_digit_value (at[3+has_colon]) * 10
                         + dsk_ascii_digit_value (at[4+has_colon]);
        if (at[0] == '-')
          *zone_offset_out = -*zone_offset_out;
        if (end)
          *end = (char*)at + (5+has_colon);
        return DSK_TRUE;
      }
    default:
      return DSK_FALSE;
    }
}
dsk_boolean
dsk_date_is_leap_year (unsigned year)
{
  return (year % 4 == 0)
     && ((year % 100 != 0) || (year % 400 == 0));
}

unsigned    dsk_date_get_day_of_year (const DskDate *date)
{
  dsk_boolean is_after_leap = date->month >= 3 && dsk_date_is_leap_year (date->year);
  dsk_assert (1 <= date->month && date->month <= 12);
  return month_starts_in_days[date->month-1] + date->day - 1
       + (is_after_leap ? 1 : 0);
}

int    dsk_date_get_days_since_epoch (const DskDate *date)
{
  unsigned year = date->year;
  /* we need to find the number of leap years between 1970
     and ly_year inclusive.  Therefore, the current year
     is included only if the date falls after Feb 28. */
  dsk_boolean before_leap = (date->month <= 2);  /* jan and feb are before leap */
  unsigned ly_year = year - (before_leap ? 1 : 0);

  /* Number of leap years before the date in question, since epoch.
   *
   * There is a leap year every 4 years, except every 100 years,
   * except every 400 years.  see "Gregorian calendar" on wikipedia.
   */
  /* XXX: does not handle dates before epoch correctly! */
  int n_leaps = ((ly_year / 4) - (EPOCH_YEAR / 4))
              - ((ly_year / 100) - (EPOCH_YEAR / 100))
              + ((ly_year / 400) - (EPOCH_YEAR / 400));

  return (year - EPOCH_YEAR) * 365
       + n_leaps
       + month_starts_in_days[date->month - 1]
       + (date->day - 1);
}

#if 0
unsigned
dsk_date_get_day_of_week (DskDate *date)
{
  unsigned day = dsk_date_get_days_since_epoch (date);

  /* day 0 was a thursday; we want day 0 == sunday.
     Hence we want thursday == 4 */
  return (day + 4) % 7;
}
#endif

/* Sun, 06 Nov 1994 08:49:37 GMT */
void        dsk_date_print_rfc822 (DskDate *date,
                                   char    *buf)
{
  static const char days_of_week[] = "SunMonTueWedThuFriSat";
  static const char months[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
  memcpy (buf, days_of_week + dsk_date_get_day_of_week (date)*3, 3);
  buf[3] = ',';
  buf[4] = ' ';
  buf[5] = '0' + date->day / 10;
  buf[6] = '0' + date->day % 10;
  buf[7] = ' ';
  memcpy (buf + 8, months + (date->month-1) * 3, 3);
  buf[11] = ' ';
  buf[12] = '0' + date->year / 1000;
  buf[13] = '0' + date->year / 100 % 10;
  buf[14] = '0' + date->year / 10 % 10;
  buf[15] = '0' + date->year % 10;
  buf[16] = ' ';
  buf[17] = '0' + date->hour / 10;
  buf[18] = '0' + date->hour % 10;
  buf[19] = ':';
  buf[20] = '0' + date->minute / 10;
  buf[21] = '0' + date->minute % 10;
  buf[22] = ':';
  buf[23] = '0' + date->second / 10;
  buf[24] = '0' + date->second % 10;
  buf[25] = ' ';
  buf[26] = 'G';
  buf[27] = 'M';
  buf[28] = 'T';
  buf[29] = '\0';
}

static const char *day_of_week_names[7] = {
  "Sunday",
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday"
};
static const char *month_names[12] = {
  "January",
  "February",
  "March",
  "April",
  "May",
  "June",
  "July",
  "August",
  "September",
  "October",
  "November",
  "December"
};

static const char day_of_week_abbrevs[21] = "SunMonTueWedThuFriSat";
static const char month_abbrevs[36] = "JanFebMarAprMayJunJulAugSepOctNovDec";

/* TODO: support years >= 10000 */
dsk_boolean
dsk_date_strftime (const DskDate *date,
                   const char    *format,
                   unsigned       max_out,
                   char          *out)
{
  char buf[64];
  char *end_out = out + max_out;
#define MAYBE_APPEND(str_length, str_value)       \
  do{                                             \
    if (out + str_length >= end_out)              \
      {                                           \
        memcpy (out, str_value, end_out - out);   \
        return DSK_FALSE;                         \
      }                                           \
    else                                          \
      {                                           \
        memcpy (out, str_value, str_length);      \
        out += str_length;                        \
      }                                           \
  }while(0)

#define MAYBE_APPEND_STRING(str_value)            \
  do{                                             \
    const char *tmp_mas_str = (str_value);        \
    unsigned tmp_mas_len = strlen (tmp_mas_str);  \
    MAYBE_APPEND(tmp_mas_len,tmp_mas_str);        \
  }while(0)
  while (*format != '\0' && out < end_out)
    {
      if (*format != '%')
        {
          *out++ = *format++;
          continue;
        }
      switch (format[1])
        {
        case 'A':
          MAYBE_APPEND_STRING (day_of_week_names[dsk_date_get_day_of_week (date)]);
          format += 2;
          break;
        case 'a':
          MAYBE_APPEND(3, day_of_week_abbrevs
                          + 3 * dsk_date_get_day_of_week (date));
          format += 2;
          break;
        case 'B':
          MAYBE_APPEND_STRING(month_names[date->month]);
          format += 2;
          break;
        case 'b':
        case 'h':
          MAYBE_APPEND(3, month_abbrevs + 3 * date->month);
          format += 2;
          break;
        case 'C':
          buf[0] = '0' + (date->year / 1000 % 10);
          buf[1] = '0' + (date->year / 100 % 10);
          MAYBE_APPEND(2, buf);
          format += 2;
          break;
        case 'd':
          buf[0] = '0' + date->day / 10;
          buf[1] = '0' + date->day % 10;
          MAYBE_APPEND(2, buf);
          format += 2;
          break;
        case 'e':
          buf[0] = date->day < 10 ? ' ' : ('0' + date->day / 10);
          buf[1] = '0' + date->day % 10;
          MAYBE_APPEND(2, buf);
          format += 2;
          break;
        case 'F':
          /* %Y-%m-%d */
          buf[0] = '0' + date->year / 1000;
          buf[1] = '0' + date->year / 100 % 10;
          buf[2] = '0' + date->year / 10 % 10;
          buf[3] = '0' + date->year % 10;
          buf[4] = '-';
          buf[5] = '0' + (date->month+1) / 10;
          buf[6] = '0' + (date->month+1) % 10;
          buf[7] = '-';
          buf[8] = '0' + date->day / 10;
          buf[9] = '0' + date->day % 10;
          MAYBE_APPEND(10, buf);
          format += 2;
          break;
        //case 'G':
        //case 'g':   ///////// "is replaced by a year as a decimal number with century.  This year is the one that contains the greater part of the week (Monday as the first day of the week).

        case 'H':
          buf[0] = '0' + date->hour / 10;
          buf[1] = '0' + date->hour % 10;
          MAYBE_APPEND(2, buf);
          format += 2;
          break;
        case 'I':
          {
            unsigned h = date->hour % 12;
            buf[0] = '0' + h / 10;
            buf[1] = '0' + h % 10;
            MAYBE_APPEND(2, buf);
            format += 2;
          }
          break;
        case 'j':
          {
            unsigned doy = dsk_date_get_day_of_year (date) + 1;
            buf[0] = '0' + doy / 100;
            buf[1] = '0' + doy / 10 % 10;
            buf[2] = '0' + doy % 10;
            MAYBE_APPEND(3, buf);
            format += 2;
          }
          break;
        case 'k':
          buf[0] = date->hour < 10 ? ' ' : ('0' + date->hour/10);
          buf[1] = '0' + date->hour % 10;
          MAYBE_APPEND(2, buf);
          format += 2;
          break;
        case 'l':
          {
            unsigned h = date->hour % 12;
            if (h == 0)
              h = 12;
            buf[0] = '0' + h / 10;
            buf[1] = '0' + h % 10;
            MAYBE_APPEND(2, buf);
            format += 2;
          }
          break;
        case 'M':
          buf[0] = '0' + date->minute / 10;
          buf[1] = '0' + date->minute % 10;
          MAYBE_APPEND(2, buf);
          format += 2;
          break;
        case 'm':
          buf[0] = '0' + (date->month+1) / 10;
          buf[1] = '0' + (date->month+1) % 10;
          MAYBE_APPEND(2, buf);
          format += 2;
          break;
        case 'n':
          buf[0] = '\n';
          MAYBE_APPEND(1, buf);
          format += 2;
          break;

        case 'O':
        case 'E':
          /* NOTE: alternate formats (roman numerals???) not supported,
             fun as it would be. */
          return DSK_FALSE;

        case 'p':
          buf[0] = date->hour >= 12 ? 'P' : 'A';
          buf[1] = 'M';
          MAYBE_APPEND(2, buf);
          format += 2;
          break;
        case 'R':
          buf[0] = '0' + date->hour / 10;
          buf[1] = '0' + date->hour % 10;
          buf[2] = ':';
          buf[3] = '0' + date->minute / 10;
          buf[4] = '0' + date->minute % 10;
          MAYBE_APPEND(5, buf);
          format += 2;
          break;
        case 'r':
          {
            unsigned h = date->hour % 12;
            buf[0] = '0' + h / 10;
            buf[1] = '0' + h % 10;
          }
          buf[2] = ':';
          buf[3] = '0' + date->minute / 10;
          buf[4] = '0' + date->minute % 10;
          MAYBE_APPEND(5, buf);
          format += 2;
          break;
        case 'S':
          buf[0] = '0' + date->second / 10;
          buf[1] = '0' + date->second % 10;
          MAYBE_APPEND(2, buf);
          format += 2;
          break;
        case 's':
          {
           unsigned n_print = snprintf (buf, sizeof (buf),
                                        "%llu",
                                        (unsigned long long) dsk_date_to_unixtime (date));
           MAYBE_APPEND (n_print, buf);
            format += 2;
            break;
          }
        case 'T':
          buf[0] = '0' + date->hour / 10;
          buf[1] = '0' + date->hour % 10;
          buf[2] = ':';
          buf[3] = '0' + date->minute / 10;
          buf[4] = '0' + date->minute % 10;
          buf[5] = ':';
          buf[6] = '0' + date->second / 10;
          buf[7] = '0' + date->second % 10;
          MAYBE_APPEND(8, buf);
          format += 2;
          break;
        case 't':
          buf[0] = '\t';
          MAYBE_APPEND(1, buf);
          format += 2;
          break;
        case 'Y':
          buf[0] = '0' + date->year / 1000;
          buf[1] = '0' + date->year / 100 % 10;
          buf[2] = '0' + date->year / 10 % 10;
          buf[3] = '0' + date->year % 10;
          MAYBE_APPEND(4, buf);
          format += 2;
          break;
        case 'y':
          buf[0] = '0' + date->year / 10 % 10;
          buf[1] = '0' + date->year % 10;
          MAYBE_APPEND(2, buf);
          format += 2;
          break;
        /* TO HANDLE: c,D,U,u,V,v,W,w,X,x,Z */
        case '%':
          buf[0] = '%';
          MAYBE_APPEND(1, buf);
          format += 2;
          break;
        case 0:
          /* unfollowed '%' */
          format++;
          break;
        default:
          /* ignore unknown format codes */
          format += 2;
          break;
        }
    }
#undef MAYBE_APPEND
#undef MAYBE_APPEND_STRING
  if (out == end_out)
    return DSK_FALSE;
  *out = 0;
  return DSK_TRUE;
}

#define MAX_DAY_OF_WEEK_NAME_LENGTH 15
#define MAX_MONTH_NAME_LENGTH 15

int
dsk_strftime_max_length (const char    *format)
{
  unsigned rv = 0;
  while (*format != '\0')
    {
      if (*format != '%')
        {
	  rv++;
	  format++;
          continue;
        }
      switch (format[1])
        {
        case 'A':
          rv += MAX_DAY_OF_WEEK_NAME_LENGTH;
          format += 2;
          break;
        case 'a':
          rv += 3;
          format += 2;
          break;
        case 'B':
          rv += MAX_MONTH_NAME_LENGTH;
          format += 2;
          break;
        case 'b':
        case 'h':
          rv += 3;
          format += 2;
          break;
        case 'C':
          rv += 2;
          format += 2;
          break;
        case 'd':
          rv += 2;
          format += 2;
          break;
        case 'e':
          rv += 2;
          format += 2;
          break;
        case 'F':
          /* %Y-%m-%d */
          rv += 10;
          format += 2;
          break;
        //case 'G':
        //case 'g':   ///////// "is replaced by a year as a decimal number with century.  This year is the one that contains the greater part of the week (Monday as the first day of the week).

        case 'H':
          rv += 2;
          format += 2;
          break;
        case 'I':
          rv += 2;
	  format += 2;
          break;
        case 'j':
          rv += 3;
	  format += 2;
          break;
        case 'k':
          rv += 2;
          format += 2;
          break;
        case 'l':
          rv += 2;
	  format += 2;
          break;
        case 'M':
          rv += 2;
          format += 2;
          break;
        case 'm':
        case 'p':
          rv += 2;
          format += 2;
          break;
        case 'n':
          rv += 1;
          format += 2;
          break;

        case 'O':
        case 'E':
          /* NOTE: alternate formats (roman numerals???) not supported,
             fun as it would be. */
          return DSK_FALSE;

        case 'R':
        case 'r':
          rv += 5;
          format += 2;
          break;
        case 'S':
          rv += 2;
          format += 2;
          break;
        case 's':
          rv += 13;
	  format += 2;
	  break;
        case 'T':
          rv += 8;
          format += 2;
          break;
        case 't':
          rv += 1;
          format += 2;
          break;
        case 'Y':
          rv += 4;
          format += 2;
          break;
        case 'y':
          rv += 2;
          format += 2;
          break;
        /* TO HANDLE: c,D,U,u,V,v,W,w,X,x,Z */
        case '%':
          rv += 1;
          format += 2;
          break;
        case 0:
          /* unfollowed '%' */
          format++;
          break;
        default:
          /* ignore unknown format codes */
          format += 2;
          break;
        }
    }
  return rv;
}
