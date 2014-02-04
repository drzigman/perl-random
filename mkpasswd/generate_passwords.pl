#!/usr/bin/env perl

use strict;
use warnings;

use String::MkPasswd qw(mkpasswd);
use Config;

# Make sure we are using drand48 as the underlying implementation
if($Config{randfunc} ne "drand48") {
    print "Perl is configured with randfunc '" . $Config{randfunc}
        . "' rather than drand48\n";
    exit 1;
}

if( scalar @ARGV != 2 ) {
    print "usage: $0 <seed> <number of passwords>\n";
    exit 1;
}

my $seed           = $ARGV[0];
my $num_iterations = $ARGV[1];

srand($seed);

for(my $password_number = 0; $password_number < $num_iterations; $password_number++) {
    print mkpasswd() . "\n";
}
