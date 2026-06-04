#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../include/base.h"
#include "../include/params.h"
#include "../include/base_sequential.h"
#include "../include/base_pthreads.h"
#include "../include/base_opencl.h"
#include "../include/test.h"

void run_pthread_pool(params_t params);
void run_mpi(int argc, char *argv[], params_t params);

int main(int argc, char *argv[]) {
    params_t params;
    
    parse_arguments(argc, argv, &params);

    switch (params.mode) {
        case 1:
            printf("=== MODO SELECCIONADO: SECUENCIAL ===\n");
            srand(time(NULL));
            char *dna_seq = vector_alloc(params.dna_length);
            pattern_t *pttn_seq = pattern_alloc(params.k_patterns, params.pattern_length);
            
            dna_generation(dna_seq, params.dna_length);
            pattern_generation(pttn_seq, params.pattern_length, params.k_patterns);

            search_patterns_sequential(dna_seq, params.dna_length, pttn_seq, params.k_patterns);
            
            for (int i = 0; i < (params.k_patterns < 5 ? params.k_patterns : 5); i++) {
                printf("Pattern %d [%s] - Estado: [%d]\n", i, pttn_seq[i].pattern, pttn_seq[i].state);
            }

            for (int i = 0; i < params.k_patterns; i++) free(pttn_seq[i].pattern);
            free(pttn_seq);
            free(dna_seq);
            break;

        case 2:
            printf("=== MODO SELECCIONADO: PTHREADS (POOL) ===\n");
            run_pthread_pool(params);
            break;

        case 3:
            printf("=== MODO SELECCIONADO: MPI ===\n");
            run_mpi(argc, argv, params);
            break;

        case 4:
            printf("=== MODO SELECCIONADO: OPENCL ===\n");
            run_opencl(params);
            break;
        
        case 5:
            run_app_tests();
            break;
        
        default:
            fprintf(stderr, "Error: Modo no válido. Use valores del 1 al 5.\n");
            return EXIT_FAILURE;
    }

    return 0;
}