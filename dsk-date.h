
/* A Gregorian Date class.
 *
 * Wikipedia has a list of other calendar systems, which is neat:
 *      http://en.wikipedia.org/wiki/List_of_calendars
 * However, none of these is used in any protocols,
 * so we do not implement them at all.
 */

/* Note that unixtime is defined as a variant of
 * Coordinated Universal Time (abbreviated UTC),
 * which has no leap seconds; in unix-time, days with a leap second are
 * "stretched out"--meaning each second is 1/86400 times longer,
 * but there are still 86400 UTC seconds that day,
 * versus 86401 GMT seconds that day - which in real-world time
 * is the same.   Generally speaking, you should not use
 * "wall-clock" time for precise benchmarking.
 */

/* I don't really think the above is true:
 * rather that's POSIX's interpretation of UTC.
 */

typedef struct _DskDate DskDate;
struct _DskDate
{
  unsigned year;	/* the year (like tm_year+1900) */
  unsigned month;	/* the month (1 through 12, tm_mon+1) */
  unsigned day;		/* the day of month (1..31) */
  unsigned hour;	/* 0..23 */
  unsigned minute;	/* 0..59 */
  unsigned second;	/* 0..60 (60 is required to support leap-seconds) */
  int      zone_offset; /* in minutes-- negative is west */
};

/* Parse either RFC 822/1123 dates (Sun, 06 Nov 1994 08:49:37 GMT)
 * or RFC 850/1036 dates (Sunday, 06-Nov-94 08:49:37 GMT)
 * or ANSI C dates (Sun Nov  6 08:49:37 1994)
 * or ISO 8601 dates (2009-02-12 T 14:32:61.1+01:00) (see RFC 3339)
 */
dsk_boolean dsk_date_parse   (const char *str,
                              char      **end,
                              DskDate    *out,
                              DskError  **error);

#define DSK_DATE_MAX_LENGTH 64
void        dsk_date_print_rfc822 (DskDate *date,
                                   char    *buf);
void        dsk_date_print_rfc850 (DskDate *date, /* unimplemented */
                                   char    *buf);
void        dsk_date_print_iso8601 (DskDate *date, /* unimplemented */
                                   char    *buf);

/* 'unixtime' here is seconds since epoch in UTC.
   If the date is before the epoch (Jan 1, 1970 00:00 GMT),
   then it is negative. */

/* Behavior of dsk_date_to_unixtime() is undefined if any
   of the fields are out-of-bounds.
   Behavior of dsk_unixtime_to_date() is defined for any time
   whose year fits in an unsigned integer. (therefore, no B.C. dates)
 */

/* dsk_unixtime_to_date() always sets the date_out->zone_offset to 0 (ie GMT) */
dsk_time_t  dsk_date_to_unixtime (const DskDate *date);
void        dsk_unixtime_to_date (dsk_time_t unixtime,
                                  DskDate *date_out);

/* we recognise: UT, UTC, GMT; EST EDT CST CDT MST MDT PST PDT [A-Z]
   and the numeric formats: +#### and -#### and +##:## and -##:##

   The return value is the number of minutes you must add to Greenwich time
   to get the time in the timezone's area.
   (ie a negative number west of Greenwich) */
dsk_boolean dsk_date_parse_timezone (const char *at,
                                     char **end,
				     int *zone_offset_out);

/* Day 0 was a Thursday, FYI, so we implement day-of-week macros below. */
int dsk_date_get_days_since_epoch (const DskDate *);


/* NOTE: Jan1 == day 0! */
unsigned dsk_date_get_day_of_year (const DskDate *date);

/* day-of-week, where Sunday is day 0 */
#define dsk_date_get_day_of_week(date) \
  ((dsk_date_get_days_since_epoch(date) + 4) % 7)

/* Returns maximum possible string output from 'format'
   including the terminating NUL. */
int  dsk_strftime_max_length      (const char *format);

dsk_boolean  dsk_date_strftime    (const DskDate *date,
                                   const char *format,
                                   unsigned    max_out,
                                   char       *out);

#if 0
/* UNIMPLEMENTED:  this is the API to deal with localtime
   fully..  To use it requires being able to parse zoneinfo files.
   Which is too much work for a feature we basically don't need. */
DskTimezone *dsk_timezone_get  (const char *name,
                                DskError  **error);
dsk_boolean dsk_timezone_query (DskTimezone *timezone,
                                dsk_time_t   unixtime,
                                int         *minutes_offset_out,
                                const char **timezone_name_out,
                                DskError   **error);

/* note: start == end_out is allowed. */
/* note: tz==NULL is interpreted as UTC/GMT with leap-seconds ignored. */
void dsk_date_change_timezone  (DskTimezone *start_tz,
                                const DskDate *start,
                                DskTimezone *end_tz,
                                DskDate     *end_out);
#endif
