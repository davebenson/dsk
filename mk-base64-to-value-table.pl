#! /usr/bin/perl -w

$base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
my @tqble = ();
for (my $i = 0; $i < 256; $i++) { push @table, -1 }

for (my $i = 0; $i < 64; $i++) { $table[ord(substr($base64_chars, $i, 1))] = $i }
for (' ', '\t', '\r', '\n') { $table[ord($_)] = -2; }
for ('=') { $table[ord($_)] = -3; }

for (my $i = 0; $i < 256; $i++) { print sprintf("%3d,", $table[$i]); if ($i % 8 == 7) {print "\n"} }
