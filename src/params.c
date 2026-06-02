#include "../include/params.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

void parse_arguments(int argc, char *argv[], params_t *params) {
    // Valores por defecto
    params->dna_length      = 50000;
    params->k_patterns      = 10;
    params->pattern_length  = 3;
    params->num_threads     = 4;
    params->mode            = 1; // Por defecto inicia en modo Secuencial (1)

    int opt;
    static struct option long_options[] = {
        {"dna_length", required_argument, 0, 'n'},
        {"patterns",   required_argument, 0, 'k'},
        {"p_length",   required_argument, 0, 'l'},
        {"threads",    required_argument, 0, 't'},
        {"mode",       required_argument, 0, 'm'}, // Nueva opcion
        {0, 0, 0, 0}
    };
    
    // Agregamos 'm:' al string de opciones para indicarle a getopt que acepta argumento
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
                // Guarda el modo elegido: 1, 2, 3 o 4
                params->mode = atoi(optarg);
                break;
            case '?':
            default:
                fprintf(stderr, "Uso: %s -n [adn len] -k [patrones] -l [patron len] -t [hilos] -m [1:Sec, 2:Pth, 3:MPI, 4:Test]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // Imprime la configuracion actual para que el usuario sepa que se esta ejecutando
    printf("Ejecutando con: n=%d, k=%d, l=%d, t=%d, modo=%d\n", 
           params->dna_length, params->k_patterns, params->pattern_length, params->num_threads, params->mode);
}