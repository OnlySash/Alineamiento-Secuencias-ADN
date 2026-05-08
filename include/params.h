#ifndef PARAMS_H
#define PARAMS_H

/**
 * @struct params_tdef
 * @brief Centralize all configuration parameters for the DNA pattern search.
 */
typedef struct {
    int dna_length;
    int k_patterns;
    int pattern_length;
    int num_threads;
} params_t;

/**
 * @brief Procesa los argumentos y llena la estructura config.
 */
void parse_arguments(int argc, char *argv[], params_t *config);

#endif