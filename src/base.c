#include "../include/base.h"

const char NUCLEOTIDES[] = "ATCG";

char* vector_alloc(int n) {
    char *vector = (char *) malloc((n + 1) * sizeof(char));
    if (vector == NULL) {
        fprintf(stderr, "Error: Cannot reserve memory for the vector.\n");
        exit(1);
    }
    vector[n] = '\0';
    return vector;
}

pattern_t* pattern_alloc(int rows, int cols) {
    pattern_t* patterns = (pattern_t*) malloc(rows * sizeof(pattern_t));
    if (patterns != NULL) {
        for (int i = 0; i < rows; i++) {
            patterns[i].pattern = vector_alloc(cols);
            patterns[i].found_at = -1;
            patterns[i].state = QUEUED;
        }
    } else {
        fprintf(stderr, "Error: Cannot reserve memory for patterns vector.\n");
    }
    return patterns;
}

void dna_generation(char* dna_ptr, int n) {
    for (int i = 0; i < n; i++) {
        dna_ptr[i] = NUCLEOTIDES[rand() % 4];
    }
    dna_ptr[n] = '\0';
}

void pattern_generation(pattern_t* patterns, int length, int k_patterns) {
    for (int i = 0; i < k_patterns; i++) {
        for (int j = 0; j < length; j++) {
            patterns[i].pattern[j] = NUCLEOTIDES[rand() % 4];
            patterns[i].length = length; 
        }
        patterns[i].pattern[length] = '\0';
        patterns[i].state = QUEUED; 
    }
}

// Funcion unificada para buscar un patron, usada tanto por secuencial como por pthreads y MPI
// Recibe 'start' y 'end' para soportar tanto Pthreads (todo el ADN) como MPI (fragmentos)
void search_single_pattern(const char* dna, int start, int end, pattern_t* pttn_struct) {
    char* p = pttn_struct->pattern;
    int plen = pttn_struct->length;
    // Buscamos hasta donde el patron todavia quepa en el limite (end)
    int search_end = end - plen;

    for (int i = start; i <= search_end; i++) {
        int j;
        for (j = 0; j < plen; j++) {
            if (dna[i + j] != p[j]) break;
        }
        if (j == plen) {
            pttn_struct->found_at = i;
            pttn_struct->state = MATCH; 
            return; 
        }
    }
    pttn_struct->state = MISSING;
}