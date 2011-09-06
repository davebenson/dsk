
typedef struct _DskTimezoneTransition DskTimezoneTransition;
typedef struct _DskTimezone DskTimezone;

struct _DskTimezoneTransition
{
  unsigned time;
  const char *code;
  int gmt_offset;
};

/* total_correction has always been 1, for a leap second,
   so far.  should the earth start speeding up you can expect
   a -1 value someday.   As of 2011, there have been 24 leap seconds added.
 */
struct _DskTimezoneLeapSecond
{
  unsigned time;
  int total_correction;
};

struct _DskTimezone
{
  unsigned n_transitions;
  DskTimezoneTransition *transitions;
  unsigned n_leap_seconds;
  DskTimezoneLeapSecondInfo *leap_seconds;

  /* private */
  char **tz_codes;
};


