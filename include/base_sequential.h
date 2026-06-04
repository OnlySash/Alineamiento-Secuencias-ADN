#ifndef BASE_SEQUENTIAL_H
#define BASE_SEQUENTIAL_H
#include "base.h"
#include "params.h"

/**
 * \brief Sequentially searches for all patterns in the DNA string.
 * \param dna_string Pointer to the main DNA string.
 * \param dna_string_length Length of the main DNA string.
 * \param patterns Array of patterns to search.
 * \param k_patterns Number of patterns in the array.
 */
void search_patterns_sequential(const char* dna_string, int dna_string_length, pattern_t* patterns, int k_patterns);

#endif