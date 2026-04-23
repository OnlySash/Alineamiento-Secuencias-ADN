#include <mpi.h>
#include <stdio.h>

#include "../include/base.h"

int main(int argc, char *argv[]) {

  // Data for chain/pattern generation and rank/size storage
  int chain_len = 5000, pattern_num = 10, pattern_len = 5, rank, size;

  // Storage for chain, patterns and process-specific data
  char *dna_chain = vector_alloc(chain_len);
  pattern_t *patterns = pattern_alloc(pattern_num, pattern_len);
  thread_args_t process_data;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (!rank) {
    dna_generation(dna_chain, chain_len);
    pattern_generation(patterns, pattern_len, pattern_len, pattern_num);
  }

  MPI_Bcast(dna_chain, chain_len, MPI_CHAR, 0, MPI_COMM_WORLD);

  for (int p_i = 0; p_i < pattern_num; p_i++) {
    MPI_Bcast(patterns[p_i].pattern, 3, MPI_LONG, 0, MPI_COMM_WORLD);
  }

  MPI_Barrier(MPI_COMM_WORLD);

  int subchain_len = chain_len / size;
  int remainder = chain_len % size;
  int offset = (rank < remainder) ? rank : remainder;

  process_data.dna_string = dna_chain;        // Not needed because of dna_chain
  process_data.dna_string_length = chain_len; // Not needed because of chain_len
  process_data.start_index = subchain_len * rank + offset;
  process_data.end_index =
      process_data.start_index + subchain_len + (offset < remainder);
  process_data.patterns = patterns; // Not needed because of patterns

  // // PARA DEBUGGING - BORRAR ANTES DE LA ENTREGA
  // // Printing only 100 characters not to flood the output with 50k characters
  // printf("\nDNA chain for process %d: %.*s...\n", rank, 100, dna_chain);

  // for (int i = 0; i < pattern_num; ++i) {
  //   printf("Pattern %d from process %d [%s]\n", i, rank,
  //   patterns[i].pattern);
  // }

  MPI_Finalize();
  return 0;
}
