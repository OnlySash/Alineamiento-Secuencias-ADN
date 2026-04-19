#include <stdio.h>
#include <stdlib.h>
#include <time.h>


const char NUCLEOTIDES[] = "ATCG";

/**
 * @brief  Reserva memoria para una cadena.
 * @param  n Cantidad de caracteres.
 * @pre    n > 0.
 * @mod    Reserva n + 1 en el Heap.
 * @return char* Puntero al bloque al asignar mem. NULL en caso de falla.
 */
char* vector_alloc(int n) {
    char *vector = (char *) malloc((n + 1) * sizeof(char));
    
    if (vector == NULL) {
        fprintf(stderr, "Error: No se pudo reservar memoria para el vector.\n");
    } else {
        vector[n] = '\0';
    }
    
    return vector;
}

/**
 * @brief  Reserva memoria para cadenas.
 * @param  rows Cantidad de cadenas
 * @pre    rows > 0.
 * @return Arreglo de punteros inicializados en NULL.
 */
char** matrix_alloc(int rows, int cols){
    char **matrix = (char **) malloc((rows + 1) * sizeof(char *));
    
    if (matrix != NULL) {
        for (int i = 0; i < rows; i++) {
            matrix[i] = vector_alloc(cols);
        }
        matrix[rows] = NULL;
    } else {
        fprintf(stderr, "Error: No se pudo reservar memoria para el vector.\n");
    }
    
    return matrix;
}

/**
 * @brief Genera secuencialmente una cadena pseudo-aleatoria de nucleótidos de ADN.
 * @param dna_ptr   Puntero al arreglo donde se escribirá la cadena de ADN.
 * @param n         Cantidad de elementos en el arreglo.
 * * @pre  "cadena" != NULL y "n" > 0.
 * @return void.
 */
void dna_gen_sec(char* dna_ptr, int n){
    srand(time(NULL));

    for (int i = 0; i < n; i++) {
        dna_ptr[i] = NUCLEOTIDES[rand() % 4];
    }
    dna_ptr[n] = '\0';
}

/**
 * @brief Genera secuencialmente patrones pseudo-aleatorios de nucleótidos de tamaño variable.
 * @param patterns  Puntero al arreglo bidim. donde se escribirá cada patrón.
 * @param min_l     Cantidad mínima de nucleótidos del patrón.
 * @param max_l     Cantidad máxima de nucleótidos del patrón.
 * @param k         Cantidad máxima de patrones a generar.
 * * @pre  "patterns" != NULL, "k" != NULL, "max_l" > 0, max_l >= min_l.
 * @return void.
 * 
 * @note max_l siempre debe ser de tamaño n - 1 respecto a asignado en mem.
 * para el largo max. de los patrones esto para marcar el final de cadena ('\0').
 */
void pattern_gen_sec(char** patterns, int min_l, int max_l, int k) {
    int pattern_l = 0;
    srand(time(NULL));

    for (int i = 0; i < k; i ++){
        pattern_l = (min_l != max_l)? rand() % (max_l - min_l + 1) + min_l : max_l;
        for (unsigned int j = 0; j < pattern_l; j++){
            patterns[i][j] = NUCLEOTIDES[rand() % 4];
        }
        patterns[i][pattern_l] = '\0';
    }

}

int main() {
    int n = 20;
    int k = 10;
    int pttn_max_l = 6;
    char *dna_string = vector_alloc(n);
    char **patterns = matrix_alloc(k, pttn_max_l);

    dna_gen_sec(dna_string, n);
    pattern_gen_sec(patterns, 3, pttn_max_l, k);

    for(int i = 0; i < n; i++){
        printf("%c",dna_string[i]);
    }

    for(int i = 0; i < k; i++){
        for (int j = 0; j < pttn_max_l; j++){
            printf("%c", patterns[i][j]);
        }
            printf("\n");
    }
    
    for (int i = 0; i < k; i++) {
        free(*(patterns + i));
    }
    free(patterns);
    free(dna_string);
    return 0;
}