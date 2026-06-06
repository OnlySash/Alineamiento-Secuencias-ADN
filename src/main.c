#include <stdio.h>
#include <stdlib.h>

#include "../include/params.h"
#include "../include/base_sequential.h"
#include "../include/base_pthreads.h"
#include "../include/base_opencl.h"
#include "../include/test.h"

int main(int argc, char *argv[]) {
    params_t params;
    
    parse_arguments(argc, argv, &params);

    switch (params.mode) {
        case 1:
            printf("=== MODO SELECCIONADO: SECUENCIAL ===\n");
            run_sequential(params);
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