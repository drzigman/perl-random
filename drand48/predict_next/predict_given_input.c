#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

char ** generate_sample_sequence( int seed, int sequence_size, int max_length );

int find_sequence_in_sample( char ** sample_sequence, int sample_sequence_size,
    char ** known_sequence, int known_sequence_size );

int main( int argc, char *argv[] ) {
    if ( argc == 1 ) {
        printf("usage: %s <generated random value 1> <value 2> ... <value 10>\n");
        return 1;
    }

    int seed = 0;
    int known_sequence_size = argc - 1;
    int length_of_known_sequence = strlen(argv[1]);
    char *known_sequence[known_sequence_size];

    int known_sequence_index;
    for( known_sequence_index = 0; known_sequence_index < known_sequence_size; known_sequence_index++ ) {
        known_sequence[known_sequence_index] = calloc(strlen(argv[known_sequence_index + 1]), sizeof(char));
        known_sequence[known_sequence_index] = argv[known_sequence_index + 1];

        if( length_of_known_sequence != strlen(known_sequence[known_sequence_index]) ) {
            printf("The percision of all known sequences must be the same.\n");
            return 1;
        }
    }

    int sample_sequence_size = 5000;
    char **sample_sequence;

    // Continue until I find the sequence
    while( 1 ) {
        sample_sequence = generate_sample_sequence(seed, sample_sequence_size, length_of_known_sequence);

        if( find_sequence_in_sample( sample_sequence, sample_sequence_size,
                known_sequence, known_sequence_size ) ) {
            printf("Position in sequence found!\n");
            printf("Using Seed: %d\n", seed);
            printf("The surronding sequence is...\n");

            int sample_sequence_index;
            int known_sequence_index = 0;
            int aligned_records = 0;
            for (sample_sequence_index = 0;
                    sample_sequence_index < sample_sequence_size;
                    sample_sequence_index++ ) {

                if( strncmp( sample_sequence[sample_sequence_index],
                            known_sequence[known_sequence_index],
                            strlen( known_sequence[known_sequence_index] )
                           ) == 0 ) {

                    printf("***\t%s\n", sample_sequence[sample_sequence_index]);

                    known_sequence_index++;
                    aligned_records++;
                }
                else {
                    printf("\t%s\n", sample_sequence[sample_sequence_index]);
                }

                if( aligned_records > 0 ) {
                    if( aligned_records > (known_sequence_size + 10) ) {
                        return 0;
                    }

                    aligned_records++;
                }
            }

            return 0;
        }

        // Free up the memory I allocated
        int sample_sequence_index;
        for( sample_sequence_index = 0;
                sample_sequence_index < sample_sequence_size;
                sample_sequence_index++ ) {

            free( sample_sequence[sample_sequence_index] );
        }

        free(sample_sequence);
        seed++;
    }
}

char ** generate_sample_sequence( int seed, int sequence_size, int max_length ) {
    char ** sequence = calloc( sequence_size, sizeof(char *) );

    srand48(seed);

    int sequence_index;
    for( sequence_index = 0; sequence_index < sequence_size; sequence_index++ ) {
        sequence[sequence_index] = calloc( max_length, sizeof(char) );

        // -2 to account for "0."
        sprintf( sequence[sequence_index], "%.*f", max_length - 2, drand48() );
    }

    return sequence;
}

int find_sequence_in_sample( char ** sample_sequence, int sample_sequence_size,
    char ** known_sequence, int known_sequence_size ) {

    int position_in_sample_sequence;
    for (position_in_sample_sequence = 0;
            position_in_sample_sequence < sample_sequence_size;
            position_in_sample_sequence++ ) {

        if( strncmp( sample_sequence[position_in_sample_sequence],
                    known_sequence[0],
                    strlen( known_sequence[0] )
                    ) == 0 ) {

            // We have a potential alignment...
            position_in_sample_sequence++;

            int position_in_known_sequence;
            for (position_in_known_sequence = 1;
                    (position_in_known_sequence < known_sequence_size) &&
                    (position_in_sample_sequence < sample_sequence_size);
                    position_in_known_sequence++ ) {

                if( strncmp( sample_sequence[position_in_sample_sequence],
                            known_sequence[position_in_known_sequence],
                            strlen( known_sequence[position_in_known_sequence] )
                            ) != 0) {
                    // Not a match
                    break;
                }

                position_in_sample_sequence++;
            }

            if( position_in_known_sequence == known_sequence_size ) {
                return 1;
            }
        }
    }
exit;
    return 0;
}

