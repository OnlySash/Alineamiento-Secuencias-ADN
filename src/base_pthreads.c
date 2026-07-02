#include "../include/base_pthreads.h"

void queue_init(task_queue_t* q) {
    q->front = 0;
    q->rear = 0;
    q->count = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    pthread_cond_init(&q->not_full, NULL);
}

void queue_destroy(task_queue_t* q) {
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->not_empty);
    pthread_cond_destroy(&q->not_full);
}

void queue_push(task_queue_t* q, int pattern_index) {
    pthread_mutex_lock(&q->mutex);
    // Wait until there is space in the queue
    while (q->count == QUEUE_CAPACITY) {
        pthread_cond_wait(&q->not_full, &q->mutex);
    }
    q->buffer[q->rear] = pattern_index;
    q->rear = (q->rear + 1) % QUEUE_CAPACITY;
    q->count++;
    
    // Signal that the queue is not empty
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->mutex);
}

int queue_pop(task_queue_t* q) {
    pthread_mutex_lock(&q->mutex);
    // Wait until there is at least one item in the queue
    while (q->count == 0) {
        pthread_cond_wait(&q->not_empty, &q->mutex);
    }
    int pattern_index = q->buffer[q->front];
    q->front = (q->front + 1) % QUEUE_CAPACITY;
    q->count--;
    
    // Signal that the queue is not full
    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->mutex);
    
    return pattern_index;
}

void* pool_worker(void* arg) {
    worker_context_t* context = (worker_context_t*)arg;
    
    while(1) {
        int index = queue_pop(context->queue);
        
        // Check for command to terminate the thread
        if (index == CMD_POOL_TERM) break; 
            
        // Perform the pattern search for the given index
        search_single_pattern(context->dna, 0, context->dna_len, &context->patterns[index]);
    }
    return NULL;
}

void search_patterns_pthread(const char* dna_string, int dna_string_length, pattern_t* patterns, int k_patterns, int num_threads) {
    task_queue_t queue;
    queue_init(&queue);
    
    worker_context_t context = { &queue, dna_string, dna_string_length, patterns };
    pthread_t threads[num_threads];
    
    // 1. Create worker threads
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, pool_worker, &context);
    }
    // 2. Push patterns onto the queue
    for (int i = 0; i < k_patterns; i++) {
        queue_push(&queue, i);
    }
    // 3. Push signals to terminate the worker threads at the end of the queue
    for (int i = 0; i < num_threads; i++) {
        queue_push(&queue, CMD_POOL_TERM);
    }
    // 4. Wait for all worker threads to complete
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    queue_destroy(&queue);
}

void run_pthread_pool(params_t params) {
    
    int n = params.dna_length;
    int k_patterns = params.k_patterns;
    int num_threads = params.num_threads;

    srand(time(NULL));

    char *dna_string = vector_alloc(n);
    pattern_t *patterns = pattern_alloc(k_patterns, params.pattern_length);
    
    dna_generation(dna_string, n);
    pattern_generation(patterns, params.pattern_length, k_patterns);

    printf("Iniciando busqueda paralela con Pthreads en %d hilos...\n", num_threads);
    
    search_patterns_pthread(dna_string, n, patterns, k_patterns, num_threads);

    print_results(patterns, k_patterns);

    for (int i = 0; i < k_patterns; i++) {
        free(patterns[i].pattern);
    }
    free(patterns);
    free(dna_string);
}
