#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h> // <--- Biblioteca para hilos

#define MATCH 1
#define QUEUED 0
#define MISSING -1
const char NUCLEOTIDES[] = "ATCG";

typedef struct {
    char* pattern;      
    int length;         
    int found_at;       
    int state;         
} pattern_t;

// --- ESTRUCTURA PARA PTHREADS ---
typedef struct {
    const char* dna_string;
    int dna_len;
    pattern_t* patterns;
    int start_index;    // Índice del primer patrón que procesa este hilo
    int end_index;      // Índice del último patrón
} thread_args_t;

//FASE 2 PROYECTO: Funciones extra Pthreads 
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


// vector_alloc, pattern_alloc, dna_gen_secuential, pattern_gen_secuential se mantienen igual

char* vector_alloc(int n) {
    char *vector = (char *) malloc((n + 1) * sizeof(char));
    if (vector == NULL) { exit(1); }
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
    }
    return patterns;
}

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

// --- LÓGICA DE BÚSQUEDA (Reutilizable) ---
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

//NUEVO Thread worker
void* pool_worker(void* arg)
{
    while(1)
    {
        int idx = dequeue(&task_queue);
        if(idx == -1)
            break;
        buscar_un_patron(
            global_dna,
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

// --- FUNCIÓN DEL HILO (Parte B) ---
void* thread_worker(void* arg) {
    thread_args_t* data = (thread_args_t*) arg;
    for (int i = data->start_index; i < data->end_index; i++) {
        buscar_un_patron(data->dna_string, data->dna_len, &data->patterns[i]);
    }
    return NULL;
}
#ifndef TESTING
int main() {
    int n = 50000;         // ADN más grande para notar el cambio
    int k_patterns = 20;
    int num_threads = 4;   // Cantidad de hilos
    srand(time(NULL));

    char *dna_string = vector_alloc(n);
    pattern_t *patterns = pattern_alloc(k_patterns, 10);
    dna_gen_secuential(dna_string, n);
    pattern_gen_secuential(patterns, 4, 10, k_patterns);

    //Agregar nuevas funciones
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
    pthread_cond_init(&task_queue.not_empty,NULL);
    pthread_cond_init(&task_queue.not_full,NULL);

    // --- IMPLEMENTACIÓN PTHREADS ---
    printf("Iniciando búsqueda paralela con %d hilos...\n", num_threads);
    
    pthread_t threads[num_threads];
    /*thread_args_t args[num_threads];
    int patterns_per_thread = k_patterns / num_threads;
*/
    for (int i = 0; i < num_threads; i++) {
       /* args[i].dna_string = dna_string;
        args[i].dna_len = n;
        args[i].patterns = patterns;
        args[i].start_index = i * patterns_per_thread;
        // El último hilo se lleva el resto si la división no es exacta
        args[i].end_index = (i == num_threads - 1) ? k_patterns : (i + 1) * patterns_per_thread;
*/
        //Usar pool_worker en vez de thread_worker para la fase 2
        //pthread_create(&threads[i], NULL, thread_worker, &args[i]);
        pthread_create(&threads[i], NULL, pool_worker, NULL);
    }

    for(int i = 0; i < k_patterns; i++)
    {
        enqueue(&task_queue, i);
    }

    // Esperar a que todos terminen
    pthread_mutex_lock(&pending_mutex);

    while(pending_tasks > 0)
    {
        pthread_cond_wait( &all_done,&pending_mutex);
    }
    pthread_mutex_unlock(&pending_mutex);

    shutdown_pool = 1;

    pthread_cond_broadcast(
        &task_queue.not_empty
    );
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    free(task_queue.tasks);
    

    // Mostrar algunos resultados
    for(int i = 0; i < 5; i++) {
        printf("Patrón %d [%s] - Estado: [%d] Pos: %d\n", 
                i, patterns[i].pattern, patterns[i].state, patterns[i].found_at);
    }

    // Limpieza
    for (int i = 0; i < k_patterns; i++) free(patterns[i].pattern);
    free(patterns);
    free(dna_string);
    return 0;
}
#endif