#include "../include/base_sequential.h"

void search_patterns_sequential(const char* dna_string, int dna_string_length, pattern_t* patterns, int k_patterns) {
    for (int p = 0; p < k_patterns; p++) {
        // Secuencial busca todo el ADN, de 0 al largo total
        search_single_pattern(dna_string, 0, dna_string_length, &patterns[p]);
    }
}

void run_sequential(params_t params) {
    printf("=== MODO SELECCIONADO: SECUENCIAL ===\n");
    
    char *dna = vector_alloc(params.dna_length);
    pattern_t *patterns = pattern_alloc(params.k_patterns, params.pattern_length);

    srand(time(NULL));

    dna_generation(dna, params.dna_length);
    pattern_generation(patterns, params.pattern_length, params.k_patterns);

    search_patterns_sequential(dna, params.dna_length, patterns, params.k_patterns);
    
    for (int i = 0; i < params.k_patterns; i++) {
        printf("Pattern %d [%s] - Estado: [%d]\n", i, patterns[i].pattern, patterns[i].state);
    }

    for (int i = 0; i < params.k_patterns; i++) free(patterns[i].pattern);
    free(patterns);
    free(dna);
}