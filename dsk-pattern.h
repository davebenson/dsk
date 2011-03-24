

typedef struct _DskPatternEntry DskPatternEntry;
typedef struct _DskPattern DskPattern;
struct _DskPatternEntry
{
  const char *pattern;
  void *result;
};

DskPattern *dsk_pattern_compile (unsigned n_entries,
                                 DskPatternEntry *entries,
                                 DskError **error);


void      * dsk_pattern_match   (DskPattern *pattern,
                                 const char *str);

void        dsk_pattern_free (DskPattern *pattern);
