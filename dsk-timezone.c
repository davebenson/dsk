#if 0
/* This is from tzfile.h.

   Though it is "not guaranteed to stay the same",
   it hasn't changed in a decade, and it's probably 
   broadly required to stay constant (ie I think the java runtime
   also depends on it not changing)

   The "coded numbers" are merely big-endian numbers.
 */

struct tzhead {
	char	tzh_magic[4];		/* TZ_MAGIC */
	char	tzh_version[1];		/* '\0' or '2' as of 2005 */
	char	tzh_reserved[15];	/* reserved--must be zero */
	char	tzh_ttisgmtcnt[4];	/* coded number of trans. time flags */
	char	tzh_ttisstdcnt[4];	/* coded number of trans. time flags */
	char	tzh_leapcnt[4];		/* coded number of leap seconds */
	char	tzh_timecnt[4];		/* coded number of transition times */
	char	tzh_typecnt[4];		/* coded number of local time types */
	char	tzh_charcnt[4];		/* coded number of abbr. chars */
};

/** . . .followed by. . .
**
**	tzh_timecnt (char [4])s		coded transition times a la time(2)
**	tzh_timecnt (unsigned char)s	types of local time starting at above
**	tzh_typecnt repetitions of
**		one (char [4])		coded UTC offset in seconds
**		one (unsigned char)	used to set tm_isdst
**		one (unsigned char)	that's an abbreviation list index
**	tzh_charcnt (char)s		'\0'-terminated zone abbreviations
**	tzh_leapcnt repetitions of
**		one (char [4])		coded leap second transition times
**		one (char [4])		total correction after above
**	tzh_ttisstdcnt (char)s		indexed by type; if TRUE, transition
**					time is standard time, if FALSE,
**					transition time is wall clock time
**					if absent, transition times are
**					assumed to be wall clock time
**	tzh_ttisgmtcnt (char)s		indexed by type; if TRUE, transition
**					time is UTC, if FALSE,
**					transition time is local time
**					if absent, transition times are
**					assumed to be local time
 */
#endif

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


DskTimezone *
dsk_timezone_parse_from_file (const char *filename,
                              DskError  **error)
{
  size_t length;
  uint8_t *contents = dsk_file_get_contents (filename, &length, error);
  if (contents == NULL)
    return NULL;
  if (length < 44)
    {
      dsk_set_error (error, "file %s much too short", filename);
      dsk_free (contents);
      return NULL;
    }
  if (memcmp (contents, "TZif", 4) != 0
   || contents[4] != 0 && contents[4] != '2')
    {
      ...
    }

  uint32_t n_gmt_trans_time_flags = dsk_uint32be_parse (contents + 20);
  uint32_t n_std_trans_time_flags = dsk_uint32be_parse (contents + 24);
  uint32_t n_leap_seconds = dsk_uint32be_parse (contents + 28);
  uint32_t n_trans_times = dsk_uint32be_parse (contents + 32);
  uint32_t n_time_types = dsk_uint32be_parse (contents + 36);
  uint32_t n_abbrev_chars = dsk_uint32be_parse (contents + 40);

  size_t needed_size = 5 * n_trans_times
                     + 6 * n_time_types
                     + n_abbrev_chars
                     + 8 * n_leap_seconds
                     + n_std_trans_time_flags
                     + n_gmt_trans_time_flags;
  if (length < 44 + needed_size)
    {
      ...
    }

**	tzh_timecnt (char [4])s		coded transition times a la time(2)
**	tzh_timecnt (unsigned char)s	types of local time starting at above
**	tzh_typecnt repetitions of
**		one (char [4])		coded UTC offset in seconds
**		one (unsigned char)	used to set tm_isdst
**		one (unsigned char)	that's an abbreviation list index
**	tzh_charcnt (char)s		'\0'-terminated zone abbreviations
**	tzh_leapcnt repetitions of
**		one (char [4])		coded leap second transition times
**		one (char [4])		total correction after above
**	tzh_ttisstdcnt (char)s		indexed by type; if TRUE, transition
**					time is standard time, if FALSE,
**					transition time is wall clock time
**					if absent, transition times are
**					assumed to be wall clock time
**	tzh_ttisgmtcnt (char)s		indexed by type; if TRUE, transition
**					time is UTC, if FALSE,
**					transition time is local time
**					if absent, transition times are
**					assumed to be local time
