#include "../include/base.h"
#include "../include/base_sequential.h"
#include "../include/params.h"

void search_patterns_sequential(const char* dna_string, int dna_string_length, pattern_t* patterns, int k_patterns) {
    for (int p = 0; p < k_patterns; p++) {
        char* pttn = patterns[p].pattern;
        int pttn_length = patterns[p].length;
        int match_at = -1;
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

        if (match_at != -1) {
            patterns[p].found_at = match_at;
            patterns[p].state = MATCH; 
        } else {
            patterns[p].state = MISSING;
        }
    }
}

int main(int argc, char* argv[]) {
    params_t params;
    parse_arguments(argc, argv, &params);
    
    char *dna_string = vector_alloc(params.dna_length);
    pattern_t *patterns = pattern_alloc(params.k_patterns, params.pattern_length);

    dna_generation(dna_string, params.dna_length);
    pattern_generation(patterns, params.pattern_length, params.k_patterns);

    search_patterns_sequential(dna_string, params.dna_length, patterns, params.k_patterns);

    for (int i = 0; i < params.k_patterns; i++) {
        printf("Pattern %d [%s] - State: [%d]", i, patterns[i].pattern, patterns[i].state);
        if(patterns[i].state == MATCH) {
            printf(" - Position: %d\n", patterns[i].found_at);
        } else {
            printf("\n");
        }
    }

    for (int i = 0; i < params.k_patterns; i++) {
        free(patterns[i].pattern);
    }
    free(patterns);
    free(dna_string);

    return 0;
}