#! /usr/bin/perl -w

my @tqble = ();
for (my $i = 0; $i < 256; $i++) { push @table, ("0x" . sprintf("%02x", $i)) }

for ('A' .. 'Z') { $table[ord($_) - ord("A") + 1] = "^$_" }
for (33 .. 126) { $table[$_] = "\'" . chr($_) . "\'" }
%pairs = ( ' ' => 'SPACE',
	   "\t" => 'TAB',
	   "\n" => "NEWLINE",
	   "\r" => "CARRIAGE-RETURN",
	   "'" => "'\\''" );
for (keys %pairs) { $table[ord($_)] = $pairs{$_} }
$table[0] = "NUL";
$table[127] = 'DEL';

print "static const char byte_name_str[] =\n";

sub escape($) { my $a = $_[0]; $a =~ s/(["\\])/\\$1/g; return $a; }
for ($i = 0; $i < 256; $i++)
  {

    print '"';
    print escape($table[$i]);
    print "\\0\" ";
    if ($i % 10 == 9)
      {
	print "  /* " . ($i-9) . " .. $i */\n";
      }
  }
print ";\n";
print "static const unsigned short byte_name_offsets[256] = {\n";
my $at = 0;
for ($i = 0; $i < 256; $i++)
  {
    print "$at,";
    $at += length($table[$i]) + 1;
    print "  /* " . ($i-9) . ".. $i */\n" if ($i % 10 == 9);
  }
print "};\n";
