#include "../include/base.h"
#include "../include/base_sequential.h"

void search_patterns_secuential(const char* dna_string, int dna_string_lenght, pattern_t* patterns, int k_patterns) {
    for (int p = 0; p < k_patterns; p++) {
        char* pttn = patterns[p].pattern;
        int pttn_length = patterns[p].length;
        int match_at = -1;
        int search_end = dna_string_lenght - pttn_length;

        for (int i = 0; i <= search_end; i++) {
            int j;
            for (j = 0; j < pttn_length; j++) {
                if (dna_string[i + j] != pttn[j]) break;
            }
            if (j == pttn_length) {
                match_at = i;
                break; 
            }
        }

        if (match_at != -1) {
            patterns[p].found_at = match_at;
            patterns[p].state = MATCH; 
        } else {
            patterns[p].state = MISSING;
        }
    }
}

int main() {
    // Parametros de prueba
    int n = 5000;
    int k_patterns = 10;
    int pttn_max_length = 6;
    int pttn_min_length = 3;

    srand(time(NULL));

    // 1. Reserva de memoria
    char *dna_string = vector_alloc(n);
    pattern_t *patterns = pattern_alloc(k_patterns, pttn_max_length);

    // 2. Generacion de datos
    dna_generation(dna_string, n);
    pattern_generation(patterns, pttn_min_length, pttn_max_length, k_patterns);

    // 3. Ejecucion secuencial
    printf("Iniciando búsqueda secuencial...\n");
    search_patterns_secuential(dna_string, n, patterns, k_patterns);

    // 4. Mostrar resultados
    printf("\nSecuencia ADN: %s\n\n", dna_string);
    for(int i = 0; i < k_patterns; i++) {
        printf("Patrón %d [%s] - Estado: [%d]", i, patterns[i].pattern, patterns[i].state);
        if(patterns[i].state == MATCH) {
            printf(" - Posición: %d\n", patterns[i].found_at);
        } else {
            printf("\n");
        }
    }

    // 5. Liberacion de memoria
    for (int i = 0; i < k_patterns; i++) {
        free(patterns[i].pattern);
    }
    free(patterns);
    free(dna_string);

    return 0;
}