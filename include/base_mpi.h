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

/**
 * \brief Calculates the starting and ending indices of the DNA sequence for a specific MPI rank.
 * * This function performs a static load balancing by dividing the total DNA chain length among
 * the available MPI processes. It accounts for remainders to ensure all nucleotides are covered.
 * * \param rank The MPI rank of the current process.
 * \param size The total number of MPI processes in the communicator.
 * \param chain_len The total length of the DNA string to be processed.
 * \param process_data Pointer to a thread_args_t structure where the calculated start_index 
 * and end_index will be stored.
 */
void compute_mpi_chunks(int rank, int size, int chain_len, thread_args_t *process_data);

/**
 * \brief Consolidates pattern matching results gathered from all MPI processes.
 * * Iterates through the gathered results array to find the global earliest occurrence 
 * (minimum found_at index) for each pattern across all ranks. Updates the local patterns 
 * array with the definitive global state.
 * * \param size The total number of MPI processes.
 * \param pattern_num The total number of patterns that were searched.
 * \param patterns Array of pattern_t structures to be updated with the global results.
 * \param all_results Flattened array containing the states and positions gathered from 
 * all MPI ranks via MPI_Gather.
 */
void reduce_mpi_matches(int size, int pattern_num, pattern_t *patterns, int *all_results);

/**
 * \brief Prints the final search state and position for each pattern to standard output.
 * * Displays whether each pattern was found (MATCH) or not (MISSING). If found, 
 * it outputs the 0-based index of its first occurrence in the DNA sequence.
 * * \param pattern_num The total number of patterns in the array.
 * \param patterns Array of pattern_t structures containing the final matching results.
 */
void print_results(int pattern_num, pattern_t *patterns);

#endif