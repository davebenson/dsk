#! /usr/bin/perl
use strict;

# See: https://dev.w3.org/html5/html-author/charref

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
    return (0xe0 | (($code >> 12) & 0xf),
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
print <<"EOF";
/* Cheatsheet:
     À Agrave       Þ thorn
     Á Aacute       ß szlig
     Â Acirc
     Ã Atilde
     Ä Auml
     Å Aring
     Ą Aogon
     Ă Abreve
     Ā Amacr
     Ė Edot
     Ç Ccedil
     Ø Oslash
     𝔸 Aopf
     𝔄 Afr
     𝒜 Ascr

   Greek:
     α alpha        κ kappa        σ sigma
     β beta         λ lambda       τ tau
     γ gamma        μ mu           υ upsi
     δ delta        ν nu           φ phi
     ε epsiv        ξ xi           χ chi
     ζ zeta         ο omicron      ψ psi
     η eta          π pi           ω omega
     θ theta        ρ rho          ϝ gammad
     ι iota         ς sigmav       ℩ iiota

   Cyrillic
     Ё IOcy         Й Jcy          а acy          ч chcy
     Ђ DJcy         К Kcy          б bcy          ш shcy
     Ѓ GJcy         Л Lcy          в vcy          щ shchcy
     Є Jukcy        М Mcy          г gcy          ъ hardcy
     Ѕ DScy         Н Ncy          д dcy          ы ycy
     І Iukcy        О Ocy          е iecy         ь softcy
     Ї YIcy         П Pcy          ж zhcy         э ecy
     Ј Jsercy       Р Rcy          з zcy          ю yucy
     Љ LJcy         С Scy          и icy          я yacy
     Њ NJcy         Т Tcy          й jcy          ё iocy
     Ћ TSHcy        У Ucy          к kcy          ђ djcy
     Ќ KJcy         Ф Fcy          л lcy          ѓ gjcy
     Ў Ubrcy        Х KHcy         м mcy          є jukcy
     Џ DZcy         Ц TScy         н ncy          ѕ dscy
     А Acy          Ч CHcy         о ocy          і iukcy
     Б Bcy          Ш SHcy         п pcy          ї yicy
     В Vcy          Щ SHCHcy       р rcy          ј jsercy
     Г Gcy          Ъ HARDcy       с scy          љ ljcy
     Д Dcy          Ы Ycy          т tcy          њ njcy
     Е IEcy         Ь SOFTcy       у ucy          ћ tshcy
     Ж ZHcy         Э Ecy          ф fcy          ќ kjcy
     З Zcy          Ю YUcy         х khcy         ў ubrcy
     И Icy          Я YAcy         ц tscy         џ dzcy
 */

// txt does not include '&' or ';'
// this handles &#XXXX; type codes as well.
bool dsk_html_entity_lookup (size_t len, 
                             const char *txt,
                             uint32_t *codepoint_out);
EOF



my @tags = ();
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

  print "/* $code: $description ($utf8) */\n";
  for my $en (@enames) {
    print "#define DSK_HTML_ENTITY_UNICODE_$en 0x$h\n";
    print "#define DSK_HTML_ENTITY_UTF8_$en " . $utf8c . "\n";
  }
  push @tags, $enames[0];
  print "\n";
}

print "\n\n#define DSK_FOREACH_HTML_ENTITY(macro) \\\n";
open FMT, "| fmt -70 | sed -e 's/\$/ \\\\/'";
for my $tag (@tags) {
  print FMT "macro($tag)\n";
}
close FMT;
print "/* done FOREACH implementation */\n";

print "\n/* vim: set fileencoding=utf-8 */\n";
