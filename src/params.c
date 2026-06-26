#include "../include/params.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

void parse_arguments(int argc, char *argv[], params_t *params) {
    // Default values
    params->dna_length      = 50000;
    params->k_patterns      = 10;
    params->pattern_length  = 3;
    params->num_threads     = 4;
    params->mode            = 1;

    static struct option long_options[] = {
        {"dna_length", required_argument, 0, 'n'},
        {"patterns",   required_argument, 0, 'k'},
        {"p_length",   required_argument, 0, 'l'},
        {"threads",    required_argument, 0, 't'},
        {"mode",       required_argument, 0, 'm'},
        {0, 0, 0, 0}
    };
    
    int opt;
    while ((opt = getopt_long(argc, argv, "n:k:l:t:m:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'n':
                params->dna_length = atoi(optarg);
                break;
            case 'k':
                params->k_patterns = atoi(optarg);
                break;
            case 'l':
                params->pattern_length = atoi(optarg);
                break;
            case 't':
                params->num_threads = atoi(optarg);
                break;
            case 'm':
                params->mode = atoi(optarg);
                break;
            case '?':
            default:
                print_params_usage(*params);
                exit(EXIT_FAILURE);
        }
    }
}

void print_params_usage(params_t params) {
    printf("Uso: ./dna_search [OPCIONES]\n");
    printf("OPCIONES:\n");
    printf("  -n, --dna_length     Longitud de la cadena de ADN (default: %d)\n", params.dna_length);
    printf("  -k, --patterns       Número de patrones a buscar (default: %d)\n", params.k_patterns);
    printf("  -l, --p_length       Longitud de cada patrón (default: %d)\n", params.pattern_length);
    printf("  -t, --threads        Número de hilos para modos paralelos (default: %d)\n", params.num_threads);
    printf("  -m, --mode           Modo de ejecución: 1=Secuencial, 2=Pthreads, 3=MPI, 4=OpenCL, 5=Tests (default: %d)\n", params.mode);
}

void print_params_execution(params_t params) {
    printf("Ejecutando con: n=%d, k=%d, l=%d, t=%d, m=%d\n", 
        params.dna_length, params.k_patterns, params.pattern_length, params.num_threads, params.mode);
}