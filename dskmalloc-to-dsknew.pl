#! perl -p

# use with -p/-i u perl -p -i dskmalloc-to-dsknew.pl FILES...

s/dsk_malloc\s?\(sizeof\s*\(\s*([^()]*)\s*\)\s*\)/DSK_NEW ($1)/;
s/dsk_malloc0\s?\(sizeof\s*\(\s*([^()]*)\s*\)\s*\)/DSK_NEW0 ($1)/;


# needs work: \d+|\w+ is MUCH to restrictive: should allow ANY parenthesized expr
s/dsk_malloc(0?)\s?\(sizeof\s*\(\s*(.*?)\s*\s*\)\s*\*\s*(\d+|\w+)\)/DSK_NEW${1}_ARRAY ($2, $3)/;


