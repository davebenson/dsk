#! /usr/bin/perl -w

my @xtqble = ();
my @tqble = ();
for (my $i = 0; $i < 256; $i++) { push @table, -1; push @xtable, -1; }

for ('0'..'9', 'a'..'f', 'A'..'F') { $xtable[ord($_)] = hex($_) }
for ('0'..'9') { $table[ord($_)] = $_ }

print "static int8_t xdigit_values[256] = {\n";
for (my $i = 0; $i < 256; $i++) { print sprintf("%2d,", $xtable[$i]); if ($i % 16 == 15) {print "\n"} }
print "};\n\nstatic int8_t digit_values[256] = {\n";
for (my $i = 0; $i < 256; $i++) { print sprintf("%2d,", $table[$i]); if ($i % 16 == 15) {print "\n"} }
print "};\n";
