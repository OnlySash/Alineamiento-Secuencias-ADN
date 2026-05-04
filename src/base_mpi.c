#include <stdio.h>

#include "../include/base_mpi.h"

void search_pattern_mpi(const char* dna_string, int start, int end, pattern_t* pttn_struct) {
    char* pttn = pttn_struct->pattern;
    int pttn_length = pttn_struct->length;
    int match_at = MISSING;
    int search_end = end - pttn_length;

    for (int i = start; i <= search_end; i++) {
        int j;
        for (j = 0; j < pttn_length; j++) {
            if (dna_string[i + j] != pttn[j]) break;
        }
        if (j == pttn_length) {
            match_at = i;
            break;
        }
    }

    if (match_at != MISSING) {
        pttn_struct->found_at = match_at;
        pttn_struct->state = MATCH;
    } else {
        pttn_struct->state = MISSING;
    }
}

int main(int argc, char *argv[]) {
    int chain_len = 5000, pattern_num = 10, pattern_len = 5, rank, size;
    char *dna_chain = vector_alloc(chain_len);
    pattern_t *patterns = pattern_alloc(pattern_num, pattern_len);
    thread_args_t process_data;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (!rank) {
        dna_generation(dna_chain, chain_len);
        pattern_generation(patterns, pattern_len, pattern_num);
    }

    MPI_Bcast(dna_chain, chain_len, MPI_CHAR, 0, MPI_COMM_WORLD);
    for (int p_i = 0; p_i < pattern_num; p_i++) {
        MPI_Bcast(patterns[p_i].pattern, pattern_len + 1, MPI_CHAR, 0, MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    int subchain_len = chain_len / size;
    int remainder = chain_len % size;
    int offset = (rank < remainder) ? rank : remainder;
    
    process_data.start_index = subchain_len * rank + offset;
    process_data.end_index = process_data.start_index + subchain_len + (offset < remainder);

    for (int p = 0; p < pattern_num; p++) {
        search_pattern_mpi(dna_chain, process_data.start_index, process_data.end_index, &patterns[p]);
    }

    int local_results[pattern_num * 2];
    int *all_results = NULL;

    for (int p = 0; p < pattern_num; p++) {
        local_results[p * 2]     = patterns[p].state;
        local_results[p * 2 + 1] = patterns[p].found_at;
    }

    if (rank == 0) {
        all_results = malloc(size * pattern_num * 2 * sizeof(int));
    }

    MPI_Gather(local_results, pattern_num * 2, MPI_INT, all_results, pattern_num * 2, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        for (int p = 0; p < pattern_num; p++) {
            for (int r = 1; r < size; r++) {
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

        for(int i = 0; i < pattern_num; i++) {
            printf("Pattern %d [%s] - State: [%d]", i, patterns[i].pattern, patterns[i].state);
            if(patterns[i].state == MATCH) {
                printf(" - Position: %d\n", patterns[i].found_at);
            } else {
                printf("\n");
            }
        }
        free(all_results);
    }
    
    MPI_Finalize();
    return 0;
}