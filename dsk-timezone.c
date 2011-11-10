#include "dsk.h"

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

#if 0
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

#include <stdio.h>

int main(int argc, char**argv)
{
  FILE *fp = fopen (argv[1], "rb");
  if (fp == NULL)
    dsk_die ("error opening");
  struct tzhead head;
  if (fread (&head, sizeof (head), 1, fp) != 1)
    dsk_die ("error reading tzfile header");

  printf ("magic: %02x %02x %02x %02x\n",
          head.tzh_magic[0], head.tzh_magic[1],
          head.tzh_magic[2], head.tzh_magic[3]);
  printf ("version: %d\n", (int) head.tzh_version[0]);
  printf ("n gmt transition times flags: %u\n",
          dsk_uint32be_parse (head.tzh_ttisgmtcnt));
  printf ("n std transition times flags: %u\n",
          dsk_uint32be_parse (head.tzh_ttisstdcnt));
  printf ("n leaps: %u\n", dsk_uint32be_parse (head.tzh_leapcnt));
  unsigned timecnt = dsk_uint32be_parse (head.tzh_timecnt);
  printf ("n transition times: %u\n", timecnt);
  unsigned typecnt = dsk_uint32be_parse (head.tzh_typecnt);
  printf ("n local time types: %u\n", typecnt);
  unsigned charcnt = dsk_uint32be_parse (head.tzh_charcnt);
  printf ("n abbrev chars: %u\n", charcnt);


  uint8_t *transition_times = dsk_malloc (4 * timecnt);
  if (timecnt != 0 && fread (transition_times, 4*timecnt, 1, fp) != 1)
    dsk_die ("error reading trans times");
  uint8_t *local_time_types = dsk_malloc (timecnt);
  if (timecnt != 0 && fread (local_time_types, timecnt, 1, fp) != 1)
    dsk_die ("error reading trans times");

  uint8_t *types = dsk_malloc (typecnt * 6);
  if (typecnt != 0 && fread (types, 6*typecnt, 1, fp) != 1)
    dsk_die ("error reading types");

  char *abbreviations = dsk_malloc (charcnt);
  if (charcnt != 0 && fread (abbreviations, charcnt, 1, fp) != 1)
    dsk_die ("error reading types");
  unsigned i;
  for (i = 0; i < timecnt; i++)
    {
      unsigned type = local_time_types[i];
      printf ("trans %4u: type %u [%d isdst=%u %s]\n",
              dsk_uint32be_parse (transition_times + 4 * i),
              type,
              (int32_t) dsk_uint32be_parse (types + 6 * type),
              types[6 * type + 4],
              abbreviations + (types[6 * type + 5]));
    }
  return 0;
}
