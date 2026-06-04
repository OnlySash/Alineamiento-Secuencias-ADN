#include "../include/base_sequential.h"

void search_patterns_sequential(const char* dna_string, int dna_string_length, pattern_t* patterns, int k_patterns) {
    for (int p = 0; p < k_patterns; p++) {
        // Secuencial busca todo el ADN, de 0 al largo total
        search_single_pattern(dna_string, 0, dna_string_length, &patterns[p]);
    }
}