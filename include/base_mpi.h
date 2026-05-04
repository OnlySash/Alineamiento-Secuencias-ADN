#ifndef BASE_MPI_H
#define BASE_MPI_H
#include "base.h"

#include <mpi.h>
#include <pthread.h>

/**
 * \brief Searches for a single pattern within a specific range of the DNA string using MPI.
 * \param dna_string Pointer to the main DNA string.
 * \param start Starting index for the search.
 * \param end Ending index for the search.
 * \param pttn_struct Pointer to the pattern_t structure containing the pattern to search.
 */
void search_pattern_mpi(const char* dna_string, int start, int end, pattern_t* pttn_struct);

#endif