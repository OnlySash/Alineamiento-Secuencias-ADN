#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MATCH 1
#define QUEUED 0
#define MISSING -1
const char NUCLEOTIDES[] = "ATCG";


typedef struct {
    char* pattern;      // DNA pattern
    int length;         // DNA length
    int found_at;       // position wich found the pattern
    int state;         // QUEUED: 0 , MATCH: 1, MISSING: -1
} pattern_t;

char* vector_alloc(int n) {
    char *vector = (char *) malloc((n + 1) * sizeof(char)); //reserve memory for char string
    if (vector == NULL) {
        fprintf(stderr, "Error: Cannot reserve memory for the vector.\n");
        exit(1);
    }
    vector[n] = '\0';
    return vector;
}

pattern_t* pattern_alloc(int rows, int cols) {
    pattern_t* patterns = (pattern_t*) malloc(rows * sizeof(pattern_t)); //reserve memory for patterns
    if (patterns != NULL) {
        for (int i = 0; i < rows; i++) {
            patterns[i].pattern = vector_alloc(cols);
            patterns[i].found_at = -1;
            patterns[i].state = QUEUED;
        }
    } else {
        fprintf(stderr, "Error: Cannot reserve memory for patterns vector.\n");
    }
    return patterns;
}

void dna_gen_secuential(char* dna_ptr, int n) {
    for (int i = 0; i < n; i++) {
        dna_ptr[i] = NUCLEOTIDES[rand() % 4]; //take random nucleotides for patterns
    }
    dna_ptr[n] = '\0';
}

void pattern_gen_secuential(pattern_t* patterns, int min_length, int max_length, int k_patterns) {
    for (int i = 0; i < k_patterns; i++) {
        // verifies if max_length and min_length are not equal then: generates random length between min and max. 
        // otherwise takes max_length for patterns length
        int pattern_length = (min_length != max_length) ? rand() % (max_length - min_length + 1) + min_length : max_length;
        for (int j = 0; j < pattern_length; j++) {
            patterns[i].pattern[j] = NUCLEOTIDES[rand() % 4];
        }
        patterns[i].pattern[pattern_length] = '\0';
        patterns[i].state = QUEUED; 
    }
}

void search_patterns_secuential(const char* dna_string, int dna_string_lenght, pattern_t* patterns, int k_patterns) {
    for (int p = 0; p < k_patterns; p++) {
        char* pttn = patterns[p].pattern;
        int pttn_length = strlen(pttn);
        int match_at = -1;

        for (int i = 0; i <= dna_string_lenght - pttn_length; i++) {
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
    dna_gen_secuential(dna_string, n);
    pattern_gen_secuential(patterns, pttn_min_length, pttn_max_length, k_patterns);

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