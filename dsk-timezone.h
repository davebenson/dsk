
typedef struct _DskTimezoneTransition DskTimezoneTransition;
typedef struct _DskTimezone DskTimezone;

struct _DskTimezoneTransition
{
  unsigned time;
  const char *code;
  int16_t gmt_offset;
  uint8_t is_dst;
};

/* total_correction has always been 1, for a leap second,
   so far.  should the earth start speeding up you can expect
   a -1 value someday.   As of 2011, there have been 24 leap seconds added.
 */
typedef struct _DskTimezoneLeapSecond DskTimezoneLeapSecond;
struct _DskTimezoneLeapSecond
{
  unsigned time;
  int total_correction;
};

struct _DskTimezone
{
  char *timezone_name;
  unsigned n_transitions;
  DskTimezoneTransition *transitions;
  unsigned n_leap_seconds;
  DskTimezoneLeapSecondInfo *leap_seconds;
};

struct _DskDate
{
  uint32_t year;                /* 1900... */
  uint8_t  month;               /* 1 .. 12 */
  uint8_t  day;                 /* 1 .. 31 */
  uint8_t  hour;
  uint8_t  minute;
  uint8_t  second;
  uint8_t  is_dst;

  /* output from to_date(), not used by from_date() */
  uint8_t     day_of_week;
  uint16_t    day_of_year;
  const char *timezone_name;
};

DskTimezone*dsk_timezone_get       (const char    *name);
void        dsk_timezone_to_date   (DskTimezone   *timezone,
                                    int64_t        unixtime,
                                    DskDate       *date_out);
dsk_boolean dsk_timezone_from_date (DskTimezone   *timezone,
                                    const DskDate *date,
                                    int64_t       *unixtime_out);

