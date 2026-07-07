#include <stdlib.h>

#include "../include/params.h"
#include "../include/base_sequential.h"
#include "../include/base_pthreads.h"
#include "../include/base_opencl.h"
#include "../include/test.h"

int main(int argc, char *argv[]) {
    params_t params;
    int rank, size;
    
    parse_arguments(argc, argv, &params);

    switch (params.mode) {
        case SEQUENTIAL_MODE:
            print_params_execution(params);
            run_sequential(params);
            break;

        case PTHREADS_MODE:
            print_params_execution(params);
            run_pthread_pool(params);
            break;

        case MPI_MODE:
            MPI_Init(&argc, &argv);
            MPI_Comm_rank(MPI_COMM_WORLD, &rank);
            MPI_Comm_size(MPI_COMM_WORLD, &size);
            if (rank == MASTER) { print_params_execution(params); }
            run_mpi(params, rank, size);
            MPI_Finalize();
            break;

        case OPENCL_MODE:
            print_params_execution(params);
            run_opencl(params);
            break;
        
        case TESTS_MODE:
            print_params_execution(params);
            run_app_tests();
            break;

        case 6:
            MPI_Init(&argc, &argv);
            MPI_Comm_rank(MPI_COMM_WORLD, &rank);
            MPI_Comm_size(MPI_COMM_WORLD, &size);
            run_mpi(params, rank, size);
            MPI_Finalize();
            break;
        
        default:
            print_params_usage(params);
            return EXIT_FAILURE;
    }

    return 0;
}