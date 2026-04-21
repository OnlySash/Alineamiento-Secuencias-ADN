#ifndef BASE_H
#define BASE_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MATCH   1
#define QUEUED  0
#define MISSING -1

extern const char NUCLEOTIDES[];

typedef struct {
    char* pattern;      // DNA pattern
    int length;         // DNA length
    int found_at;       // Position in wich we found the pattern
    int state;          // MISSING: -1, QUEUED: 0 , MATCH: 1
} pattern_t;

// Estructura exclusiva para los argumentos de los hilos
typedef struct {
    const char* dna_string;
    int dna_string_length;
    pattern_t* patterns;
    int start_index;
    int end_index;
} thread_args_t;

char* vector_alloc(int n);

pattern_t* pattern_alloc(int k, int size);

void dna_generation(char* dna_ptr, int n);

void pattern_generation(pattern_t* patterns, int min_length, int max_length, int k_patterns);

#endif