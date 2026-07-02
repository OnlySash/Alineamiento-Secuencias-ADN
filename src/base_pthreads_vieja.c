#include "../include/test.h"
#include <string.h>

/*

Solo Se uso la forma de busqueda de patron viejo para poder testear la version vieja de pthreads, que es la que se 
encuentra en base_pthreads.c del entregable 1

 */
void buscar_un_patron(const char* dna, int dna_len, pattern_t* pttn_struct) {
    char* p = pttn_struct->pattern;
    int plen = strlen(p);
    
    for (int i = 0; i <= dna_len - plen; i++) {
        int j;
        for (j = 0; j < plen; j++) {
            if (dna[i + j] != p[j]) break;
        }
        if (j == plen) {
            pttn_struct->found_at = i;
            pttn_struct->state = MATCH; // Cambiado de 'e' a MATCH
            return;
        }
    }
    pttn_struct->state = MISSING; // Cambiado de 'n' a MISSING
}