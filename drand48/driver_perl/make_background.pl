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


for (my $num_lines = 0; $num_lines < 50; $num_lines++) {
    for (my $iteration = 0; $iteration < 10; $iteration++) {
        printf("%f  ", rand());
    }

    print "\n";
}
