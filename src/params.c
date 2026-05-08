#include "../include/params.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>


#define NO_ARGS 1

void parse_arguments(int argc, char *argv[], params_t *params) {
    if (argc == NO_ARGS) {
        params->dna_length = 50000;
        params->k_patterns = 10;
        params->pattern_length = 3;
        params->num_threads = 4;
    }
    else {
        int opt;
        static struct option long_options[] = {
            {"dna_length", required_argument, 0, 'n'},
            {"patterns",  required_argument, 0, 'k'},
            {"p_length",  required_argument, 0, 'l'},
            {"threads",     required_argument, 0, 't'},
            {0, 0, 0, 0}
        };

        while ((opt = getopt_long(argc, argv, "n:k:l:t:", long_options, NULL)) != -1) {
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
                case '?':
                default:
                    fprintf(stderr, "Usage: %s -n [dna length] -k [patterns] -l [pattern length] -t [threads]\n", argv[0]);
                    exit(EXIT_FAILURE);
            }
        }
    }
    printf("Executing with n=%d, k=%d, l=%d, t=%d\n", 
           params->dna_length, params->k_patterns, params->pattern_length, params->num_threads);
}