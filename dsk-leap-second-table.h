//
// So far, there have only been "leap seconds" meaning the addition of 
// extra seconds (the original specification had a plan for handling years
// that were too short, but it's never come up.
//
// Research (and common sense) indicates that these things 
// are slowing down, so i suspect we'll need more leap seconds.
//
// Nonetheless, there was a gap (and Bush II briefly considered ditching
// the leap-second entirely.
//
typedef struct DskLeapSecondTableEntry DskLeapSecondTableEntry;

struct DskLeapSecondTableEntry
{
  unsigned posix_unixtime;
  unsigned real_unixtime;
};

extern const int dsk_n_leap_second_table_entries;
extern const struct DskLeapSecondTableEntry dsk_leap_second_table_entries[];


unsigned dsk_leap_seconds_convert_unixtime_real_to_posix 
                                         (unsigned real_unixtime, 
                                          unsigned *orig_seconds_out);
unsigned dsk_leap_seconds_convert_unixtime_posix_to_real 
                                         (unsigned posix_unixtime);
