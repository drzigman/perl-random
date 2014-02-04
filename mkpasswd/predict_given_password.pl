#!/usr/bin/env perl

use strict;
use warnings;

use String::MkPasswd qw(mkpasswd);
use Parallel::ForkManager;
use Term::ProgressBar;
use Config;

use Readonly;
Readonly my $NUM_PROCS           => 4;
Readonly my $ITERATIONS_PER_SEED => 500;
Readonly my $NUM_SEEDS_PER_PROC  => 1000;

# Make sure we are using drand48 as the underlying implementation
if($Config{randfunc} ne "drand48") {
    print "Perl is configured with randfunc '" . $Config{randfunc}
        . "' rather than drand48\n";
    exit 1;
}

if( scalar @ARGV != 2 ) {
    print "usage: $0 <starting seed> <known password>\n";
    exit 1;
}

my $starting_time  = time();
my $initial_seed   = $ARGV[0];
my $known_password = $ARGV[1];

my $pm = Parallel::ForkManager->new($NUM_PROCS);

my $progress = Term::ProgressBar->new({
    name  => 'Calculating',
    count => ($starting_time - $initial_seed),
    ETA   => 'linear',
});

$pm -> run_on_finish ( sub {
    $progress->update($NUM_SEEDS_PER_PROC);
});

for(my $seed = $initial_seed; $seed < ($starting_time + $NUM_SEEDS_PER_PROC); $seed += $NUM_SEEDS_PER_PROC) {
    my $pid = $pm->start and next;

        for(my $proc_seed = $seed; $proc_seed < ($seed + $NUM_SEEDS_PER_PROC); $proc_seed++) {

            srand($proc_seed);

            for(my $password_number = 0; $password_number < $ITERATIONS_PER_SEED; $password_number++) {
                my $generated_password = mkpasswd();

                if($generated_password eq $known_password) {
                    print "Found Matching Password '$known_password'\n";
                    print "Seed: " . $seed . "\n";
                    print "The next 15 passwords are...\n";

                    for(my $next_password_index = 0; $next_password_index < 15; $next_password_index++) {
                        print "\t" . mkpasswd() . "\n";
                    }

                    last;
                }
            }
        }

    $pm->finish;
}

$pm->wait_all_children;
