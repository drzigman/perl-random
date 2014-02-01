#!/usr/bin/env perl

use strict;
use warnings;

use Config;

# Make sure we are using drand48 as the underlying implementation
if($Config{randfunc} ne "drand48") {
    print "Perl is configured with randfunc '" . $Config{randfunc}
        . "' rather than drand48\n";
    return 1;
}

if( scalar @ARGV != 2 ) {
    print "usage: $0 <seed> <number of iterations>\n";
    return 1;
}

my $seed           = $ARGV[0];
my $num_iterations = $ARGV[1];

srand($seed);

for (my $iteration = 0; $iteration < $num_iterations; $iteration++) {
    printf("%f\n", rand());
}
