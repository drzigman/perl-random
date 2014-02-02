#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>

#define NUMBER_OF_THREADS 6

struct sequence {
    char ** contents;
    int     size;
    int     length;
};

struct thread_data {
    struct sequence *    known_sequence;
    struct thread_info * thread_info;
    int                  initial_seed;
};

struct thread_info {
    pthread_t id;
    int       number;
};

static void * thread_worker( void *arg );

void output_found_sequence( int seed, struct sequence * sample_sequence, struct sequence * known_sequence);
struct sequence * generate_sample_sequence( int seed, int sequence_size, int max_length );
int find_sequence_in_sample( struct sequence * sample_sequence, struct sequence * known_sequence );
void attempt_seed( int seed, struct sequence * known_sequence );

int main( int argc, char *argv[] ) {
    if ( argc == 1 ) {
        printf("usage: %s <starting seed> <generated random value 1> <value 2> ... <value 10>\n");
        return 1;
    }

    int initial_seed = atoi( argv[1] );

    struct sequence * known_sequence = malloc( sizeof(struct sequence) );

    known_sequence->size     = argc - 2;
    known_sequence->length   = strlen( argv[2] );
    known_sequence->contents = calloc( known_sequence->size, sizeof(char *) );

    int known_sequence_index;
    for( known_sequence_index = 0;
            known_sequence_index < known_sequence->size;
            known_sequence_index++ ) {

        known_sequence->contents[known_sequence_index]
            = calloc( strlen( argv[known_sequence_index + 2] ), sizeof(char) );
        known_sequence->contents[known_sequence_index] = argv[known_sequence_index + 2];

        if( known_sequence->length !=
                strlen( known_sequence->contents[known_sequence_index] ) ) {

            printf("The percision of all known sequences must be the same.\n");
            return 1;
        }
    }

    struct thread_info * thread_info = calloc( NUMBER_OF_THREADS, sizeof( struct thread_info ) );
    void * res;

    int thread_index;
    for( thread_index = 0; thread_index < NUMBER_OF_THREADS; thread_index++ ) {
        thread_info[thread_index].number = thread_index + 1;
    }

    for( thread_index = 0; thread_index < NUMBER_OF_THREADS; thread_index++ ) {
        struct thread_data *thread_data = malloc( sizeof( struct thread_data ) );

        thread_data->known_sequence = known_sequence;
        thread_data->thread_info    = &thread_info[thread_index];
        thread_data->initial_seed   = initial_seed;

        pthread_create(&thread_info[thread_index].id, NULL, &thread_worker, (void *) thread_data);
    }

    for( thread_index = 0; thread_index < NUMBER_OF_THREADS; thread_index++ ) {
        pthread_join(thread_info[thread_index].id, &res);
    }
}


static void * thread_worker( void * arg ) {

    struct thread_data * thread_data = (struct thread_data *) arg;

    int current_seed = thread_data->initial_seed;

    while (1) {
        if(current_seed % NUMBER_OF_THREADS != (thread_data->thread_info->number - 1) ) {
            current_seed++;
            continue;
        }

        if(current_seed % 1000000 == 0) {
            printf("Thread Number: %d is Attempting Seed: %d\n",
                thread_data->thread_info->number, current_seed);
        }

        attempt_seed(current_seed, thread_data->known_sequence);
        current_seed++;
    }
}

void attempt_seed( int seed, struct sequence * known_sequence ) {

    int sample_sequence_size = 5000;
    struct sequence * sample_sequence;

    //printf("Attempting to predict using seed %d\n", seed);
    sample_sequence = generate_sample_sequence(seed, sample_sequence_size, known_sequence->length);

    if( find_sequence_in_sample( sample_sequence, known_sequence ) ) {
        output_found_sequence(seed, sample_sequence, known_sequence);
        exit(0);
    }

    // Free up the memory I allocated
    int sample_sequence_index;
    for( sample_sequence_index = 0;
            sample_sequence_index < sample_sequence_size;
            sample_sequence_index++ ) {

        free( sample_sequence->contents[sample_sequence_index] );
    }

    free( sample_sequence->contents );
    free(sample_sequence);
}

struct sequence * generate_sample_sequence( int seed, int sequence_size, int max_length ) {

    struct sequence * sequence = malloc( sizeof( struct sequence ));

    sequence->size     = sequence_size;
    sequence->length   = max_length;
    sequence->contents = calloc( sequence_size, sizeof(char *) );

    struct drand48_data seeded_buffer;
    srand48_r(seed, &seeded_buffer);
    double random_number;

    int sequence_index;
    for( sequence_index = 0; sequence_index < sequence_size; sequence_index++ ) {
        sequence->contents[sequence_index] = calloc( max_length, sizeof(char) );

        drand48_r(&seeded_buffer, &random_number);

        // -2 to account for "0."
        sprintf( sequence->contents[sequence_index], "%.*f", max_length - 2, random_number );
    }

    return sequence;
}

int find_sequence_in_sample( struct sequence * sample_sequence, struct sequence * known_sequence ) {

    int position_in_sample_sequence;
    for (position_in_sample_sequence = 0;
            position_in_sample_sequence < sample_sequence->size;
            position_in_sample_sequence++ ) {

        if( strncmp( sample_sequence->contents[position_in_sample_sequence],
                    known_sequence->contents[0],
                    known_sequence->length
                    ) == 0 ) {

            // We have a potential alignment...
            position_in_sample_sequence++;

            int position_in_known_sequence;
            for (position_in_known_sequence = 1;
                    (position_in_known_sequence < known_sequence->size) &&
                    (position_in_sample_sequence < sample_sequence->size);
                    position_in_known_sequence++ ) {

                if( strncmp( sample_sequence->contents[position_in_sample_sequence],
                            known_sequence->contents[position_in_known_sequence],
                            known_sequence->length
                            ) != 0) {
                    // Not a match
                    break;
                }

                position_in_sample_sequence++;
            }

            if( position_in_known_sequence == known_sequence->size ) {
                return 1;
            }
        }
    }

    return 0;
}

void output_found_sequence( int seed, struct sequence * sample_sequence, struct sequence * known_sequence) {

    printf("Position in sequence found!\n");
    printf("Using Seed: %d\n", seed);
    printf("The surronding sequence is...\n");

    int sample_sequence_index;
    int known_sequence_index = 0;
    int aligned_records      = 0;

    for (sample_sequence_index = 0;
            sample_sequence_index < sample_sequence->size;
            sample_sequence_index++ ) {

        if( aligned_records < known_sequence->size
            && strncmp( sample_sequence->contents[sample_sequence_index],
                    known_sequence->contents[known_sequence_index],
                    known_sequence->length
                ) == 0 ) {

            printf("***\t%s\n", sample_sequence->contents[sample_sequence_index]);

            known_sequence_index++;
            aligned_records++;
        }
        else {
            if( aligned_records > 0 ) {
                if( aligned_records > (known_sequence->size + 10) ) {
                    return;
                }

                aligned_records++;
            }

            printf("\t%s\n", sample_sequence->contents[sample_sequence_index]);
        }
    }

    return;
}

