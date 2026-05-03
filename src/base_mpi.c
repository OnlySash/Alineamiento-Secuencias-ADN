#include <mpi.h>
#include <stdio.h>

#include "../include/base.h"

//Utilizado en la busqueda de pthreads cambiando el inicio y final 
//de busqueda en la cadena
void buscar_patron_mpi(const char* dna_string, int start, int end, pattern_t* pttn_struct) {
    char* pttn = pttn_struct->pattern;
    int pttn_length = pttn_struct->length;
    int match_at = MISSING;
    int search_end = end - pttn_length;

    for (int i = start; i <= search_end; i++) {
        int j;
        for (j = 0; j < pttn_length; j++) {
            if (dna_string[i + j] != pttn[j]) break;
        }
        if (j == pttn_length) {
            match_at = i;
            break; 
        }
    }

    if (match_at != MISSING) {
        pttn_struct->found_at = match_at;
        pttn_struct->state = MATCH; 
    } else {
        pttn_struct->state = MISSING;
    }
}

int main(int argc, char *argv[]) {
  // Datos para la cadena/ generacion de patron y rango/tamaño
  int chain_len = 5000, pattern_num = 10, pattern_len = 5, rank, size;

  // Espacio para la cadena, patrones y la data
  char *dna_chain = vector_alloc(chain_len);
  pattern_t *patterns = pattern_alloc(pattern_num, pattern_len);
  thread_args_t process_data;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // 1. El proceso 0 genera datos
  if (!rank) {
    dna_generation(dna_chain, chain_len);
    pattern_generation(patterns, pattern_len, pattern_len, pattern_num);
  }

  // 2. Broadcast de la cadena y patrones
  MPI_Bcast(dna_chain, chain_len, MPI_CHAR, 0, MPI_COMM_WORLD);
  for (int p_i = 0; p_i < pattern_num; p_i++) {
    MPI_Bcast(patterns[p_i].pattern, pattern_len +1, MPI_CHAR, 0, MPI_COMM_WORLD);
  }
  MPI_Barrier(MPI_COMM_WORLD);

  // 3. Calcula rango de busqueda de cada proceso
  int subchain_len = chain_len / size;
  int remainder = chain_len % size;
  int offset = (rank < remainder) ? rank : remainder;
  //process_data.dna_string = dna_chain;        // Not needed because of dna_chain
  //process_data.dna_string_length = chain_len; // Not needed because of chain_len
  process_data.start_index = subchain_len * rank + offset;
  process_data.end_index =
      process_data.start_index + subchain_len + (offset < remainder);
  
 // 4. Cada proceso Busca los patrones en su segmento
  for (int p = 0; p < pattern_num; p++) {
    buscar_patron_mpi(
        dna_chain,
        process_data.start_index,
        process_data.end_index,
        &patterns[p]
    );
  }

  // 5. Guardar resultados locales: [state, found_at] por patrón
    int  local_results[pattern_num * 2];
    int *all_results = NULL;

    for (int p = 0; p < pattern_num; p++) {
        local_results[p * 2]     = patterns[p].state;
        local_results[p * 2 + 1] = patterns[p].found_at;
    }

    // Solo el proceso 0 necesita el buffer completo
    if (rank == 0) {
        all_results = malloc(size * pattern_num * 2 * sizeof(int));
    }

    // 6. Reunir todos los resultados en proceso 0
    MPI_Gather(
        local_results,      // sendbuf
        pattern_num * 2,    // sendcount
        MPI_INT,
        all_results,        // recvbuf (solo válido en rank 0)
        pattern_num * 2,    // recvcount por proceso
        MPI_INT,
        0,                  // root
        MPI_COMM_WORLD
    );

    // 7. Proceso 0 consolida: prioriza el MATCH con posición
    if (rank == 0) {
        for (int p = 0; p < pattern_num; p++) {
            // Comparar con los demás procesos (r=0 ya está en patterns[])
            for (int r = 1; r < size; r++) {
                int idx        = r * pattern_num * 2 + p * 2;
                int r_state    = all_results[idx];
                int r_found_at = all_results[idx + 1];

                if (r_state == MATCH) {
                    if (patterns[p].state != MATCH ||
                        r_found_at < patterns[p].found_at) {
                        patterns[p].state    = r_state;
                        patterns[p].found_at = r_found_at;
                    }
                }
            }
        }


  process_data.patterns = patterns; // Not needed because of patterns

  // // PARA DEBUGGING - BORRAR ANTES DE LA ENTREGA
  // // Printing only 100 characters not to flood the output with 50k characters
    //printf("\nDNA chain for process %d: %.*s...\n", rank, 100, dna_chain);

 //for (int i = 0; i < pattern_num; ++i) {
     //printf("Pattern %d from process %d [%s]\n", i, rank,
     //patterns[i].pattern);
   //}
    //printf("\nSecuencia ADN (Mostrando 100 chars): %.*s...\n\n", 100, dna_string);
    //if (rank == 0){

    //8. Resultados
        for(int i = 0; i < pattern_num; i++) {
            printf("Patrón %d [%s] - Estado: [%d]", i, patterns[i].pattern, patterns[i].state);
            if(patterns[i].state == MATCH) {
                printf(" - Posición: %d\n", patterns[i].found_at);
            } else {
                printf("\n");
            }
        }
        free(all_results);
    }
  MPI_Finalize();
  return 0;

}