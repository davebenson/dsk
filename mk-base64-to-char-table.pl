#! /usr/bin/perl -w

$base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
for ($i = 0; $i < length($base64_chars); $i++) {
  print "'" . substr($base64_chars, $i, 1) . "'"
      . ($i % 10 == 9 ? ",   /* " . sprintf("%2d", $i-9) ." .. " . ($i) . " */\n"
	              : ",");
}
print "\n";
