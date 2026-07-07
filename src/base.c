#include "../include/base.h"

const char NUCLEOTIDES[] = "ATCG";

char* vector_alloc(int n) {
    char *vector = (char *) malloc((n + 1) * sizeof(char));
    if (vector == NULL) {
        fprintf(stderr, "Error: Cannot reserve memory for the vector.\n");
        exit(1);
    }
    vector[n] = '\0';
    return vector;
}

pattern_t* pattern_alloc(int rows, int cols) {
    pattern_t* patterns = (pattern_t*) malloc(rows * sizeof(pattern_t));
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

void dna_generation(char* dna_ptr, int n) {
    for (int i = 0; i < n; i++) {
        dna_ptr[i] = NUCLEOTIDES[rand() % 4];
    }
    dna_ptr[n] = '\0';
}

void pattern_generation(pattern_t* patterns, int length, int k_patterns) {
    for (int i = 0; i < k_patterns; i++) {
        for (int j = 0; j < length; j++) {
            patterns[i].pattern[j] = NUCLEOTIDES[rand() % 4];
            patterns[i].length = length; 
        }
        patterns[i].pattern[length] = '\0';
        patterns[i].state = QUEUED; 
    }
}

// Recibe 'start' y 'end' para soportar tanto Pthreads (todo el ADN) como MPI (fragmentos)
void search_single_pattern(const char* dna, int start, int end, pattern_t* pttn_struct) {
    char* pattern = pttn_struct->pattern;
    int length = pttn_struct->length;
    int search_end = end - length;

    for (int i = start; i <= search_end; i++) {
        int j;
        for (j = 0; j < length; j++) {
            if (dna[i + j] != pattern[j]) break;
        }
        if (j == length) {
            pttn_struct->found_at = i;
            pttn_struct->state = MATCH; 
            return; 
        }
    }
    pttn_struct->state = MISSING;
}

void print_results(pattern_t *patterns, int k_patterns) {
    for (int i = 0; i < k_patterns; i++) {
        printf("PATTERN_%d [%s]   STATE: ", i, patterns[i].pattern);
        if (patterns[i].state == MATCH) {
            printf("MATCH    at i = %d\n", patterns[i].found_at);
        } else if (patterns[i].state == MISSING) {
            printf("MISSING\n");
        } else {
            printf("QUEUED\n");
        }
    }
}

// Fun. aux. para reconstruir pasos de MPI y asignar los IDs de hilo en la UI
static void get_chunk_bounds(int rank, int size, int total_items, int *start, int *end) {
    int chunk = total_items / size;
    int remainder = total_items % size;
    int offset = (rank < remainder) ? rank : remainder;
    
    *start = chunk * rank + offset;
    *end = *start + chunk + (rank < remainder ? 1 : 0);
}

void generate_data_log(const char* dna, pattern_t *patterns, int k_patterns, int dna_length, int num_threads, int mode) {
    FILE *file = fopen("ui/data.json", "w"); 
    if (!file) return;

    fprintf(file, "{\n  \"dna\": \"%s\",\n", dna);
    
    const char* mode_str = "UNKNOWN";
    switch(mode) {
        case 1: mode_str = "SEQUENTIAL"; break;
        case 2: mode_str = "PTHREADS"; break;
        case 3: mode_str = "MPI"; break;
    }
    fprintf(file, "  \"mode\": \"%s\",\n", mode_str);
    
    int active_threads = (num_threads > 0) ? num_threads : 1;
    fprintf(file, "  \"n_ids\": %d,\n", active_threads);
    
    fprintf(file, "  \"patterns\": [\n");
    for (int i = 0; i < k_patterns; i++) {
        fprintf(file, "    { \"seq\": \"%s\" }%s\n", patterns[i].pattern, (i < k_patterns - 1) ? "," : "");
    }
    fprintf(file, "  ],\n  \"logs\": [\n");

    int log_count = 0;
    
    for (int i = 0; i < k_patterns; i++) {
        int ui_thread_id = (i % active_threads) + 1; 
        int start_pos = 0; 
        int scan_limit = (patterns[i].state == MATCH) ? patterns[i].found_at : (dna_length - patterns[i].length);
        
        if (mode == 3) {
            for (int r = 0; r < active_threads; r++) {
                int r_start, r_end;
                get_chunk_bounds(r, active_threads, k_patterns, &r_start, &r_end); // <-- Dividiendo k_patterns
                if (i >= r_start && i < r_end) {
                    ui_thread_id = r + 1;
                    break;
                }
            }

            if (patterns[i].state == MATCH) {
                start_pos = patterns[i].found_at;
                scan_limit = patterns[i].found_at;
            } else {
                start_pos = dna_length - patterns[i].length;
                if (start_pos < 0) start_pos = 0;
                scan_limit = start_pos;
            }
        }

        for (int pos = start_pos; pos <= scan_limit; pos++) {
            int is_match = (pos == patterns[i].found_at && patterns[i].state == MATCH) ? 1 : 0;
            
            if (log_count > 0) fprintf(file, ",\n");
            fprintf(file, "    { \"id\": %d, \"patId\": %d, \"pos\": %d, \"match\": %d }", 
                    ui_thread_id, i, pos, is_match);
            log_count++;
        }
    }
    fprintf(file, "\n  ]\n}\n");
    fclose(file);
}