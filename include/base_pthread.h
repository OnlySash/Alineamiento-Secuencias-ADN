#ifndef BASE_PTHREAD_H
#define BASE_PTHREAD_H
#include "base.h"

#include <pthread.h>

/**
 * \brief Worker function for pthreads that searches for assigned patterns within the DNA string.
 * \param arg Pointer to thread_args_t containing the thread's workload parameters.
 * \return NULL upon completion.
 */
void* thread_worker(void* arg);

/**
 * \brief Initializes threads and distributes the pattern search workload among them.
 * \param dna_string Pointer to the main DNA string.
 * \param dna_string_length Length of the main DNA string.
 * \param patterns Array of patterns to search.
 * \param k_patterns Total number of patterns.
 * \param num_threads Number of pthreads to spawn.
 */
void search_patterns_pthread(const char* dna_string, int dna_string_length, pattern_t* patterns, int k_patterns, int num_threads);

#endif