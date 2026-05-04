#include "../include/base_pthread.h"

void* thread_worker(void* arg) {
    thread_args_t* data = (thread_args_t*) arg;
    
    for (int p = data->start_index; p < data->end_index; p++) {
        pattern_t* pttn_struct = &data->patterns[p];
        char* pttn = pttn_struct->pattern;
        int pttn_length = pttn_struct->length;
        int match_at = MISSING;
        int search_end = data->dna_string_length - pttn_length;

        for (int i = 0; i <= search_end; i++) {
            int j;
            for (j = 0; j < pttn_length; j++) {
                if (data->dna_string[i + j] != pttn[j]) break;
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
        args[i].end_index = (i == num_threads - 1) ? k_patterns : (i + 1) * patterns_per_thread;

        pthread_create(&threads[i], NULL, thread_worker, &args[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
}

int main() {
    int n = 50000;
    int k_patterns = 10;
    int length = 3;
    int num_threads = 4;

    srand(time(NULL));

    char *dna_string = vector_alloc(n);
    pattern_t *patterns = pattern_alloc(k_patterns, length);

    dna_generation(dna_string, n);
    pattern_generation(patterns, length, k_patterns);

    printf("Starting parallel search with %d threads...\n", num_threads);
    search_patterns_pthread(dna_string, n, patterns, k_patterns, num_threads);

    printf("\nDNA Sequence (Showing 100 chars): %.*s...\n\n", 100, dna_string);
    for(int i = 0; i < k_patterns; i++) {
        printf("Pattern %d [%s] - State: [%d]", i, patterns[i].pattern, patterns[i].state);
        if(patterns[i].state == MATCH) {
            printf(" - Position: %d\n", patterns[i].found_at);
        } else {
            printf("\n");
        }
    }

    for (int i = 0; i < k_patterns; i++) {
        free(patterns[i].pattern);
    }
    free(patterns);
    free(dna_string);

    return 0;
}