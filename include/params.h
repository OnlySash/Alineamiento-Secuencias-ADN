#ifndef PARAMS_H
#define PARAMS_H

typedef enum mode_t {
    SEQUENTIAL_MODE = 1,
    PTHREADS_MODE,
    MPI_MODE,
    OPENCL_MODE,
    TESTS_MODE
} mode_t;

/**
 * @struct params_tdef
 * @brief Centralize all configuration parameters for the DNA pattern search.
 */
// params.h
typedef struct {
    int dna_length;
    int k_patterns;
    int pattern_length;
    int num_threads;
    mode_t mode;
} params_t;

/**
 * \brief Parses command-line arguments and populates the execution parameters structure.
 * * Initializes the parameters with predefined default values. It then processes the
 * command-line arguments using `getopt_long` to override the values specified by the user.
 * * \param argc The number of command-line arguments passed to the program.
 * \param argv An array of strings representing the command-line arguments.
 * \param params Pointer to the params_t structure to be populated with the parsed configuration.
 */
void parse_arguments(int argc, char *argv[], params_t *params);

void print_params_usage(params_t params);

void print_params_execution(params_t params);

#endif