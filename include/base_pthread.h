#ifndef BASE_PTHREAD_H
#define BASE_PTHREAD_H
#include "base.h"

#include <pthread.h>

void* thread_worker(void* arg);
void buscar_un_patron(const char* dna_string, int dna_string_length, pattern_t* pttn_struct);
void search_patterns_pthread(const char* dna_string, int dna_string_length, pattern_t* patterns, int k_patterns, int num_threads);

#endif