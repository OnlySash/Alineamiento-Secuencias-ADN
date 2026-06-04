#include "../include/base_mpi.h"

#define MASTER 0

void compute_mpi_chunks(int rank, int size, int chain_len, thread_args_t *process_data) {
    int subchain_len = chain_len / size;
    int remainder = chain_len % size;
    int offset = (rank < remainder) ? rank : remainder;
    
    process_data->start_index = subchain_len * rank + offset;
    process_data->end_index = process_data->start_index + subchain_len + (offset < remainder);
}

void reduce_mpi_matches(int size, int pattern_num, pattern_t *patterns, int *all_results) {
    for (int p = 0; p < pattern_num; p++) {
        for (int r = 1; r < size; r++) { // Start from 1 since MASTER is on patterns already
            int idx        = r * pattern_num * 2 + p * 2;
            int r_state    = all_results[idx];
            int r_found_at = all_results[idx + 1];

            if (r_state == MATCH) {
                if (patterns[p].state != MATCH || r_found_at < patterns[p].found_at) {
                    patterns[p].state    = r_state;
                    patterns[p].found_at = r_found_at;
                }
            }
        }
    }
}

void print_results(int pattern_num, pattern_t *patterns) {
    for (int i = 0; i < pattern_num; i++) {
        printf("PATTERN_%d [%s]   STATE: ", i, patterns[i].pattern);
        if (patterns[i].state == MATCH) {
            printf("MATCH    at i = %d\n", patterns[i].found_at);
        } else if (patterns[i].state == MISSING) {
            printf("MISSING\n");
        } else {
            printf("QUEUED\n");
        }
    }
}

void run_mpi(int argc, char *argv[], params_t params) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    char *dna_chain = vector_alloc(params.dna_length);
    pattern_t *patterns = pattern_alloc(params.k_patterns, params.pattern_length);

    if (rank == MASTER) {
        dna_generation(dna_chain, params.dna_length);
        pattern_generation(patterns, params.pattern_length, params.k_patterns);
    }

    MPI_Bcast(dna_chain, params.dna_length, MPI_CHAR, MASTER, MPI_COMM_WORLD);
    for (int p_i = 0; p_i < params.k_patterns; p_i++) {
        MPI_Bcast(patterns[p_i].pattern, params.pattern_length + 1, MPI_CHAR, MASTER, MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    thread_args_t process_data;
    compute_mpi_chunks(rank, size, params.dna_length, &process_data);

    for (int p = 0; p < params.k_patterns; p++) {
        search_single_pattern(dna_chain, process_data.start_index, process_data.end_index, &patterns[p]); 
    }

    int local_results[params.k_patterns * 2];
    for (int p = 0; p < params.k_patterns; p++) {
        local_results[p * 2]     = patterns[p].state;
        local_results[p * 2 + 1] = patterns[p].found_at;
    }

    int *all_results = NULL;
    if (rank == MASTER) {
        all_results = malloc(size * params.k_patterns * 2 * sizeof(int));
    }

    MPI_Gather(local_results, params.k_patterns * 2, MPI_INT, 
               all_results,   params.k_patterns * 2, MPI_INT, 
               MASTER, MPI_COMM_WORLD);

    if (rank == MASTER) {
        reduce_mpi_matches(size, params.k_patterns, patterns, all_results);
        print_results(params.k_patterns, patterns);
        free(all_results);
    }
    MPI_Finalize();
    for (int i = 0; i < params.k_patterns; i++) {
        free(patterns[i].pattern);
    }
    free(patterns);
    free(dna_chain);
}