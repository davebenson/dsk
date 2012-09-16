#! /usr/bin/perl -w

my @table = ();
for (my $i = 0; $i < 256; $i++) { push @table, 0 }

# set spaces
for (' ', "\r", "\t", "\n") { $table[ord($_)] |= 1 }

# set islower bits
for ('a'..'z') { $table[ord($_)] |= 2 }
# set isupper bits
for ('A'..'Z') { $table[ord($_)] |= 4 }
# set isdigit bits
for ('0'..'9') { $table[ord($_)] |= 8 }
# set isxdigit bits
for ('0'..'9', 'a'..'f', 'A'..'F') { $table[ord($_)] |= 16 }
# set - and _ bits */
for ('-', '_') { $table[ord($_)] |= 32 }

# ascii punctuation
for ('!','"','#','$','%','&',"'",'(',')','*',
     '+',',','-','.','/',':',';','<','=','>',
     '?','@','[','\\',']','^','_','`','{','|',
     '}','~') { $table[ord($_)] |= 64 }

# ascii characters that don't require c-quoting
for ('a'..'z', 'A'..'Z', '0'..'9',
     '!','#','$','%','&',"'",'(',')','*',
     '+',',','-','.','/',':',';','<','=','>',
     '?','@','[',']','^','_','`','{','|',
     '}','~',' ') { $table[ord($_)] |= 128 }

for (my $i = 0; $i < 256; $i++) { print sprintf("0x%02x,", $table[$i]); if ($i % 16 == 15) {print "\n"} }
