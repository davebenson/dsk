#! /usr/bin/perl -w

# 4 possibilities:
#     0 = invalid
#     1 = valid as is
#     2 = valid, needs lowercasing
#     3 = colon
my @tqble = ();
for (my $i = 0; $i < 64; $i++) { push @table, 0 }
sub set($$) { my $o = $_[0];
              my $v = $_[1];
	      $table[int($o/4)] |= ($v << (($o % 4) * 2)); }
for ('a'..'z', '0'..'9', '-', '_') { set(ord($_), 1) }
for ('A'..'Z') { set(ord($_), 2) }
set(ord(":"), 3);

for (my $i = 0; $i < 64; $i++) { print sprintf("0x%02x,", $table[$i]); if ($i % 16 == 15) {print "\n"} }
