#include "../include/base_pthread.h"

void buscar_un_patron(const char* dna_string, int dna_string_length, pattern_t* pttn_struct) {
    char* pttn = pttn_struct->pattern;
    int pttn_length = pttn_struct->length;
    int match_at = MISSING;
    int search_end = dna_string_length - pttn_length;

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

    if (match_at != MISSING) {
        pttn_struct->found_at = match_at;
        pttn_struct->state = MATCH; 
    } else {
        pttn_struct->state = MISSING;
    }
}

void* thread_worker(void* arg) {
    thread_args_t* data = (thread_args_t*) arg;
    for (int p = data->start_index; p < data->end_index; p++) {
        buscar_un_patron(data->dna_string, data->dna_string_length, &data->patterns[p]);
    }
    return NULL;
}

void search_patterns_pthread(const char* dna_string, int dna_string_length, pattern_t* patterns, int k_patterns, int num_threads) {
    pthread_t threads[num_threads];
    thread_args_t args[num_threads];
    int patterns_per_thread = k_patterns / num_threads;

    for (int i = 0; i < num_threads; i++) {
        args[i].dna_string = dna_string;
        args[i].dna_string_length = dna_string_length;
        args[i].patterns = patterns;
        args[i].start_index = i * patterns_per_thread;
        // Last thread takes the remaining patterns if k_patterns is not divisible in an integer by num_threads
        args[i].end_index = (i == num_threads - 1) ? k_patterns : (i + 1) * patterns_per_thread;

        pthread_create(&threads[i], NULL, thread_worker, &args[i]);
    }

    // Sincronization: Wait for all threads to finish
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
}

int main() {
    // Parámetros de prueba
    int n = 50000;
    int k_patterns = 10;
    int pttn_max_length = 6;
    int pttn_min_length = 3;
    int num_threads = 4;

    srand(time(NULL));

    // 1. Reserva de memoria
    char *dna_string = vector_alloc(n);
    pattern_t *patterns = pattern_alloc(k_patterns, pttn_max_length);

    // 2. Generacion de datos
    dna_generation(dna_string, n);
    pattern_generation(patterns, pttn_min_length, pttn_max_length, k_patterns);

    // 3. Ejecucion paralela
    printf("Iniciando búsqueda paralela con %d hilos...\n", num_threads);
    search_patterns_pthread(dna_string, n, patterns, k_patterns, num_threads);

    // 4. Mostrar resultados
    // Nota: Truncado a 100 caracteres para no congelar la terminal con 50k letras
    printf("\nSecuencia ADN (Mostrando 100 chars): %.*s...\n\n", 100, dna_string);
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