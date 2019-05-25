#! /usr/bin/perl

my $mode = $ARGV[0];
$mode = 'c' unless defined $mode;

my @table = (
  [ "signature_md5_rsa",           "1.2.840.113549.1.1.4"   ],
  [ "signature_sha1_rsa",          "1.2.840.113549.1.1.5"   ],
  [ "signature_sha256_rsa",        "1.2.840.113549.1.1.11"  ],
  [ "signature_sha384_rsa",        "1.2.840.113549.1.1.12"  ],
  [ "signature_sha512_rsa",        "1.2.840.113549.1.1.13"  ],
  [ "signature_rsapss",            "1.2.840.113549.1.1.10"  ], #RFC 3447
  [ "signature_dsa_with_sha1",     "1.2.840.10040.4.3"      ],
  [ "signature_dsa_with_sha256",   "2.16.840.1.101.3.4.3.2" ],
  [ "signature_ecdsa_with_sha1",   "1.2.840.10045.4.1"      ],
  [ "signature_ecdsa_with_sha256", "1.2.840.10045.4.3.2"    ],
  [ "signature_ecdsa_with_sha384", "1.2.840.10045.4.3.3"    ],
  [ "signature_ecdsa_with_sha512", "1.2.840.10045.4.3.4"    ],
  [ "signature_ed25519",           "1.3.101.112"            ],
  [ "signature_rsa_encryption",    "1.2.840.113549.1.1.1"   ],

  [ "hash_sha1",                   "1.3.14.3.2.26"          ],
  [ "hash_sha256",                 "1.3.6.1.4.1.1722.12.2.1"],
  [ "hash_sha384",                 "1.3.6.1.4.1.1722.12.2.2"],
  [ "hash_sha512",                 "1.3.6.1.4.1.1722.12.2.3"],
  [ "hash_sha224",                 "1.3.6.1.4.1.1722.12.2.4"],
  [ "hash_sha512_224",             "1.3.6.1.4.1.1722.12.2.5"],
  [ "hash_sha512_256",             "1.3.6.1.4.1.1722.12.2.6"],

  [ "maskgenerationfunction_1",    "1.2.840.113549.1.1.8"   ],

  [ "dn_common_name",              "2.5.4.3" ],                       # CN	       
  [ "dn_surname",                  "2.5.4.4" ],                       # SN	       
  [ "dn_serial_number",            "2.5.4.5" ],                       # SERIALNUMBER 
  [ "dn_country_name",             "2.5.4.6" ],                       # C	       
  [ "dn_locality_name",            "2.5.4.7" ],                       # L	       
  [ "dn_state_or_province_name",   "2.5.4.8" ],                       # ST or S      
  [ "dn_street_address",           "2.5.4.9" ],                       # STREET       
  [ "dn_organization_name",        "2.5.4.10" ],                      # O	       
  [ "dn_organizational_unit",      "2.5.4.11" ],                      # OU	       
  [ "dn_title",                    "2.5.4.12" ],                      # T or TITLE   
  [ "dn_given_name",               "2.5.4.42" ],                      # G or GN      
  [ "dn_email_address",            "1.2.840.113549.1.9.1" ],          # E	       
  [ "dn_user_id",                  "0.9.2342.19200300.100.1.1" ],     # UID	       
  [ "dn_domain_component",         "0.9.2342.19200300.100.1.25" ],    # DC	       
);


if ($mode eq 'c') {
  print "/*\n";
  print " * Generated by $0\n";
  print " */\n\n";
  print "#include \"../dsk.h\"\n";
}

my @offsets = ();
print "const uint32_t dsk_tls_object_id_heap[] = {\n" if ($mode eq 'c');
my $offset = 0;
for (@table) {
  my $e = $_;
  my $dotted = $e->[1];
  my $name = $e->[0];
  my $pieces = $dotted =~ tr/././ + 1;
  my $copy = $dotted;
  $copy =~ s/\./,/g;
  print "  $pieces, $copy,    /* $name */\n" if ($mode eq 'c');
  push @$e, $offset;
  $offset += 1 + $pieces;
}
print "};\n\n" if ($mode eq 'c');

for (@table) {
  my $e = $_;
  my $name = $e->[0];
  my $offset = $e->[2];
  #print "DskTlsObjectID *dsk_tls_oid__$name = (DskTlsObjectID *) (dsk_tls_oid_heap + $offset);\n" if ($mode eq 'c')
  #print "extern DskTlsObjectID *dsk_tls_oid__$name;\n",
  print "#define dsk_tls_object_id__$name   \\\n                             ((const DskTlsObjectID*)(dsk_tls_object_id_heap + $offset))\n" if ($mode eq 'h')
}
 
 
 
 
 
         
         
 
 
 
         
 
 
 