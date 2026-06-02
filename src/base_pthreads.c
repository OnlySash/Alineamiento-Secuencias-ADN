#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>

#include "../include/base.h"
#include "../include/params.h"

typedef struct {
    int pattern_index;
} task_t;

typedef struct {
    task_t* tasks;
    int front;
    int rear;
    int count;
    int capacity;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} task_queue_t;

task_queue_t task_queue;

pattern_t* global_patterns;
char* global_dna;
int global_dna_len;

int shutdown_pool = 0;
int pending_tasks = 0;

pthread_mutex_t pending_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t all_done = PTHREAD_COND_INITIALIZER;

void enqueue(task_queue_t* q, int pattern_index)
{
    pthread_mutex_lock(&q->mutex);
    while(q->count == q->capacity)
        pthread_cond_wait(&q->not_full, &q->mutex);
    q->tasks[q->rear].pattern_index = pattern_index;
    q->rear = (q->rear + 1) % q->capacity;
    q->count++;
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->mutex);
}

int dequeue(task_queue_t* q)
{
    pthread_mutex_lock(&q->mutex);
    while(q->count == 0 && !shutdown_pool)
        pthread_cond_wait(&q->not_empty, &q->mutex);
    if(shutdown_pool)
    {
        pthread_mutex_unlock(&q->mutex);
        return -1;
    }
    int pattern_index = q->tasks[q->front].pattern_index;
    q->front = (q->front + 1) % q->capacity;
    q->count--;
    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->mutex);
    return pattern_index;
}


// char* vector_alloc(int n) {
//     char *vector = (char *) malloc((n + 1) * sizeof(char));
//     if (vector == NULL) { exit(1); }
//     vector[n] = '\0';
//     return vector;
// }

// pattern_t* pattern_alloc(int rows, int cols) {
//     pattern_t* patterns = (pattern_t*) malloc(rows * sizeof(pattern_t));
//     if (patterns != NULL) {
//         for (int i = 0; i < rows; i++) {
//             patterns[i].pattern = vector_alloc(cols);
//             patterns[i].found_at = -1;
//             patterns[i].state = QUEUED;
//         }
//     }
//     return patterns;
// }

void dna_gen_secuential(char* dna_ptr, int n) {
    for (int i = 0; i < n; i++) dna_ptr[i] = NUCLEOTIDES[rand() % 4];
    dna_ptr[n] = '\0';
}

void pattern_gen_secuential(pattern_t* patterns, int min_l, int max_l, int k) {
    for (int i = 0; i < k; i++) {
        int len = (min_l != max_l) ? rand() % (max_l - min_l + 1) + min_l : max_l;
        for (int j = 0; j < len; j++) patterns[i].pattern[j] = NUCLEOTIDES[rand() % 4];
        patterns[i].pattern[len] = '\0';
        patterns[i].state = QUEUED; 
    }
}

/*
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
            pttn_struct->state = MATCH;
            return;
        }
    }
    pttn_struct->state = MISSING;
}
*/

//NUEVO Thread worker
void* pool_worker(void* arg)
{
    (void)arg;
    while(1)
    {
        int idx = dequeue(&task_queue);
        if(idx == -1)
            break;
            
        search_single_pattern(
            global_dna,
            0,
            global_dna_len,
            &global_patterns[idx]
        );
        
        pthread_mutex_lock(&pending_mutex);
        pending_tasks--;
        if(pending_tasks == 0)
            pthread_cond_signal(&all_done);
        pthread_mutex_unlock(&pending_mutex);
    }

    return NULL;
}

void* thread_worker(void* arg) {
    thread_args_t* data = (thread_args_t*) arg;

    for (int p = data->start_index; p < data->end_index; p++) {
        search_single_pattern(data->dna_string, 0, data->dna_string_length, &data->patterns[p]);
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

void run_pthread_pool(params_t params) {
    int n = params.dna_length;
    int k_patterns = params.k_patterns;
    int num_threads = params.num_threads;

    srand(time(NULL));

    char *dna_string = vector_alloc(n);
    pattern_t *patterns = pattern_alloc(k_patterns, params.pattern_length);
    
    dna_gen_secuential(dna_string, n);
    pattern_gen_secuential(patterns, params.pattern_length, params.pattern_length, k_patterns);

    // Configuracion de variables globales necesarias para el pool
    global_dna = dna_string;
    global_dna_len = n;
    global_patterns = patterns;
    pending_tasks = k_patterns;

    task_queue.capacity = k_patterns;
    task_queue.front = 0;
    task_queue.rear = 0;
    task_queue.count = 0;
    task_queue.tasks = malloc(sizeof(task_t) * k_patterns);

    pthread_mutex_init(&task_queue.mutex, NULL);
    pthread_cond_init(&task_queue.not_empty, NULL);
    pthread_cond_init(&task_queue.not_full, NULL);

    printf("Iniciando busqueda paralela con Pool de %d hilos...\n", num_threads);
    
    pthread_t threads[num_threads];
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, pool_worker, NULL);
    }

    // Insertar todas las tareas (patrones a buscar) en la cola
    for(int i = 0; i < k_patterns; i++) {
        enqueue(&task_queue, i);
    }

    // Esperar con variable de condicion a que los hilos terminen todas las tareas
    pthread_mutex_lock(&pending_mutex);
    while(pending_tasks > 0) {
        pthread_cond_wait(&all_done, &pending_mutex);
    }
    pthread_mutex_unlock(&pending_mutex);

    shutdown_pool = 1;
    pthread_cond_broadcast(&task_queue.not_empty);
    
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    int lim = (k_patterns < 5) ? k_patterns : 5;
    for(int i = 0; i < lim; i++) {
        printf("Patron %d [%s] - Estado: [%d] Pos: %d\n", 
                i, patterns[i].pattern, patterns[i].state, patterns[i].found_at);
    }

    free(task_queue.tasks);
    pthread_mutex_destroy(&task_queue.mutex);
    pthread_cond_destroy(&task_queue.not_empty);
    pthread_cond_destroy(&task_queue.not_full);

    for (int i = 0; i < k_patterns; i++) {
        free(patterns[i].pattern);
    }
    free(patterns);
    free(dna_string);
}
