#! /usr/bin/perl
use strict;


my $html_entities_tabsep_filename = "html-entities.tsv";

open D, "<$html_entities_tabsep_filename" or die;

my $render_mode = $ARGV[0];
if (!$render_mode) { $render_mode = 'unicode'; }

sub to_utf8_bytecode_string($) {
  my $code = $_[0];
  if ($code < 0x7f) {
    die();
  } elsif ($code < 0x7ff) {
    return (0xc0 | (($code >> 6) & 0x1f), 
            0x80 | (($code >> 0) & 0x3f));
  } elsif ($code < 0xffff) {
    return (0xd0 | (($code >> 12) & 0xf), 
            0x80 | (($code >> 6) & 0x3f),
            0x80 | (($code >> 0) & 0x3f));
  } elsif ($code < 0x10ffff) {
    return (0xf0 | (($code >> 18) & 0xf), 
            0x80 | (($code >> 12) & 0x3f),
            0x80 | (($code >> 6) & 0x3f),
            0x80 | (($code >> 0) & 0x3f));
  } else {
    die();
  }
}
sub bytecode_to_utf8cstring($) {
  my $n = $_[0];
  die unless (128 <= $n && $n <= 255);
  return sprintf("\\%03o", $n);
}
sub to_utf8_cstring($) {
  my $rv = '"';
  my $utf8 = '';
  for my $piece (to_utf8_bytecode_string($_[0])) {
    $rv .= bytecode_to_utf8cstring($piece);
  }
  $rv .= '"';
  $utf8 = chr($_[0]);
  return ($utf8, $rv);
}

binmode STDOUT, ":utf8";

print "/* HTML Entities, as UTF8 and Unicode. */\n";
print "\n\n/* This file itself uses UTF-8 in the comments. */\n\n\n";

while (<D>) {
  chomp;
  my @pieces = split /\t/, $_;
  my $code = shift @pieces;
  my $description = shift @pieces;
  my $category = shift @pieces;
  my @enames = @pieces;
  my $codenum = hex(substr($code, 2));
  next if ($codenum < 128);

  my $h = substr($code, 2);
  my $codenum = hex($h);
  my ($utf8, $utf8c) = to_utf8_cstring($codenum);

  print "/* $code: $description ($category) ($utf8) */\n";
  for my $en (@enames) {
    print "#define DSK_HTML_ENTITY_UNICODE_$en 0x$h\n";
    print "#define DSK_HTML_ENTITY_UTF8_$en " . $utf8c . "\n";
  }
  print "\n";
}
