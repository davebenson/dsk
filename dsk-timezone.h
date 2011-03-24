
typedef struct _DskTimezoneTransition DskTimezoneTransition;
typedef struct _DskTimezone DskTimezone;

struct _DskTimezoneTransition
{
  unsigned time;
  const char *code;
  int gmt_offset;
};

struct _DskLeapSeconds
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


