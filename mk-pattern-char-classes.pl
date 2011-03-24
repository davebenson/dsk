#! /usr/bin/perl -w

my $all = join('', (map {chr($_)} 1..255));

sub mk_class($)
{
  my @array = (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
  my $chars = $all;
  my $c = $_[0];
  eval "\$chars =~ s/[^\\$c]//gs;";
  for ($i = 0; $i < length($chars); $i++)
    { $asc = ord(substr($chars, $i, 1));
      $array[int($asc) / 8] |= (1<<($asc % 8)); }
  print "{'$c', {{" . join('', map {"0x" . sprintf("%02x",$_).","} @array) . "}}},\n"
}

mk_class('d');
mk_class('D');
mk_class('s');
mk_class('S');
mk_class('w');
mk_class('W');
