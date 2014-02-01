#include <stdio.h>
#include <stdlib.h>

int main( int argc, char *argv[] ) {
    if ( argc != 3 ) {
        printf("usage: %s <seed> <number of iterations>\n", argv[0]);
        return 1;
    }

    int seed           = atoi( argv[1] );
    int num_iterations = atoi( argv[2] );


    srand48(seed);

    int iteration;
    for ( iteration = 0; iteration < num_iterations; iteration++) {
        printf("%f\n", drand48());
    }

    return 0;
}
