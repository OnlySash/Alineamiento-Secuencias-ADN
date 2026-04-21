#include "../include/base.h"

const char NUCLEOTIDES[] = "ATCG";

char* vector_alloc(int n) {
    char *vector = (char *) malloc((n + 1) * sizeof(char)); // Reserve memory for char string
    if (vector == NULL) {
        fprintf(stderr, "Error: Cannot reserve memory for the vector.\n");
        exit(1);
    }
    vector[n] = '\0';
    return vector;
}

pattern_t* pattern_alloc(int rows, int cols) {
    pattern_t* patterns = (pattern_t*) malloc(rows * sizeof(pattern_t)); // Reserve memory for patterns
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
        dna_ptr[i] = NUCLEOTIDES[rand() % 4]; // Take random nucleotides for patterns
    }
    dna_ptr[n] = '\0';
}

void pattern_generation(pattern_t* patterns, int min_length, int max_length, int k_patterns) {
    for (int i = 0; i < k_patterns; i++) {
        // Verifies if max_length and min_length are not equal then: generates random length between min and max. 
        // otherwise takes max_length for pattern_length
        int pattern_length = (min_length != max_length) ? rand() % (max_length - min_length + 1) + min_length : max_length;
        for (int j = 0; j < pattern_length; j++) {
            patterns[i].pattern[j] = NUCLEOTIDES[rand() % 4];
            patterns[i].length = pattern_length; 
        }
        patterns[i].pattern[pattern_length] = '\0';
        patterns[i].state = QUEUED; 
    }
}