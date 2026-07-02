#ifndef BASE_H
#define BASE_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/** \def MATCH
 * \brief State indicating the pattern was found.
 */
#define MATCH   1

/** \def QUEUED
 * \brief State indicating the pattern is queued for search.
 */
#define QUEUED  0

/** \def MISSING
 * \brief State indicating the pattern was not found.
 */
#define MISSING -1

extern const char NUCLEOTIDES[];

/**
 * \struct pattern_t
 * \brief Represents a DNA pattern and its search state.
 */
typedef struct pattern_t {
    char* pattern;      /**< DNA pattern string */
    int length;         /**< Length of the DNA pattern */
    int found_at;       /**< Position where the pattern was found */
    int state;          /**< State of the search: MISSING (-1), QUEUED (0), MATCH (1) */
} pattern_t;

/**
 * \struct thread_args_t
 * \brief Arguments passed to threads/processes for parallel execution.
 */
typedef struct {
    const char* dna_string;    /**< Pointer to the main DNA string */
    int dna_string_length;     /**< Length of the main DNA string */
    pattern_t* patterns;       /**< Array of patterns to search */
    int start_index;           /**< Start index for the workload */
    int end_index;             /**< End index for the workload */
} thread_args_t;

/**
 * \brief Allocates memory for a char vector.
 * \param n Size of the vector to allocate.
 * \return Pointer to the allocated char array.
 */
char* vector_alloc(int n);

/**
 * \brief Allocates memory for an array of patterns.
 * \param rows Number of patterns.
 * \param cols Length of each pattern.
 * \return Pointer to the allocated array of pattern_t.
 */
pattern_t* pattern_alloc(int rows, int cols);

/**
 * \brief Generates a random DNA string.
 * \param dna_ptr Pointer to the allocated memory for the DNA string.
 * \param n Length of the DNA string to generate.
 */
void dna_generation(char* dna_ptr, int n);

/**
 * \brief Generates random patterns to search.
 * \param patterns Array of pattern_t structs to populate.
 * \param length Length of each pattern.
 * \param k_patterns Number of patterns to generate.
 */
void pattern_generation(pattern_t* patterns, int length, int k_patterns);

/**
 * \brief Searches for a single pattern in a specified range of the DNA string.
 * \param dna Pointer to the main DNA string.
 * \param start Starting index for the search.
 * \param end Ending index for the search.
 * \param pattern Pointer to the pattern_t struct representing the pattern to search.
 */
void search_single_pattern(const char* dna, int start, int end, pattern_t* pattern);

/**
 * \brief Prints the final search state and position for each pattern to standard output.
 * * Displays whether each pattern was found (MATCH) or not (MISSING). If found, 
 * it outputs the 0-based index of its first occurrence in the DNA sequence.
 * \param k_patterns The total number of patterns in the array.
 * \param patterns Array of pattern_t structures containing the final matching results.
 */
void print_results(pattern_t *patterns, int k_patterns);

void generate_data_log(pattern_t *patterns, int k_patterns);

#endif
