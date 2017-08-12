#! /usr/bin/perl
use strict;

my $html_entities_tabsep_filename = "html-entities.tsv";

open D, "<$html_entities_tabsep_filename" or die;

my $render_mode = $ARGV[0];
if (!$render_mode) { $render_mode = 'unicode'; }

while (<D>) {
  chomp;
  my @pieces = split /\t/, $_;
  my $code = shift @pieces;
  my $description = shift @pieces;
  my $category = shift @pieces;
  my @enames = @pieces;
  for my $en (@enames) {
    my $h = substr($code, 2);
    if ($render_mode eq 'unicode') {
      print "#define DSK_HTML_ENTITY_AS_UNICODE_$en 0x$h\n";
    }
  }
}
