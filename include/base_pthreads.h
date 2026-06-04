#ifndef BASE_PTHREAD_H
#define BASE_PTHREAD_H

#include "base.h"
#include "params.h"
#include <pthread.h>

/**
 * \def QUEUE_CAPACITY
 * \brief Maximum number of elements the task queue can hold.
 * * Acts as a bounded circular queue limit to protect RAM from exhaustion
 * when dealing with millions of patterns.
 */
#define QUEUE_CAPACITY 1024
/**
 * \def CMD_POOL_TERM
 * \brief Emulates a SIGTERM signal to stop the worker thread gracefully.
 */
#define CMD_POOL_TERM -1

/**
 * \struct task_queue_t
 * \brief Thread-safe bounded circular queue for task distribution.
 * * Utilizes a mutex and condition variables to safely manage the producer-consumer
 * workflow between the main thread and the worker pool.
 */
typedef struct {
    int buffer[QUEUE_CAPACITY];
    int front;
    int rear;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t  not_empty;
    pthread_cond_t  not_full;
} task_queue_t;

/**
 * \struct worker_context_t
 * \brief Execution context passed to each thread in the pool.
 * * Encapsulates necessary data pointers to eliminate the use of global variables,
 * making the thread pool reentrant and thread-safe.
 * * IMPORTANT: Do not alter the order of these variables as the initialization
 * depends strictly on this layout.
 */
typedef struct {
    task_queue_t* queue;
    const char* dna;
    int             dna_len;
    pattern_t* patterns;
} worker_context_t;

/**
 * \brief Initializes the task queue.
 * * Sets front, rear, and count to zero, and initializes the mutex and condition variables.
 * \param q Pointer to the task_queue_t structure to initialize.
 */
void queue_init(task_queue_t* q);

/**
 * \brief Destroys the synchronization primitives of the task queue.
 * * Safely frees the resources associated with the mutex and condition variables.
 * \param q Pointer to the task_queue_t structure to destroy.
 */
void queue_destroy(task_queue_t* q);

/**
 * \brief Pushes a new task (pattern index) into the queue.
 * * Called by the producer (main thread). If the queue is full (reaches QUEUE_CAPACITY),
 * the calling thread will block until space becomes available.
 * \param q Pointer to the task_queue_t structure.
 * \param pattern_index The index of the pattern to be processed.
 */
void queue_push(task_queue_t* q, int pattern_index);

/**
 * \brief Pops a task (pattern index) from the queue.
 * * Called by consumer threads (workers). If the queue is empty, the thread
 * will block until a new task is pushed by the producer.
 * \param q Pointer to the task_queue_t structure.
 * \return The index of the pattern to process, or -1 if a poison pill is received.
 */
int queue_pop(task_queue_t* q);

/**
 * \brief Worker function executed by threads in the pool.
 * * Continuously fetches pattern indices from the task queue and performs 
 * the DNA pattern search until a poison pill (-1) is encountered.
 * \param arg Pointer to a worker_context_t structure containing the execution context.
 * \return NULL upon clean termination.
 */
void* pool_worker(void* arg);

/**
 * \brief Main engine that initializes the thread pool and distributes pattern searches.
 * * Instantiates the bounded queue, launches the specified number of threads, pushes
 * all pattern indices into the queue dynamically, and finally pushes poison pills to gracefully 
 * shut down the workers before joining them.
 * \param dna_string Pointer to the main DNA string.
 * \param dna_string_length Length of the main DNA string.
 * \param patterns Array of pattern_t structures to search.
 * \param k_patterns Total number of patterns.
 * \param num_threads Number of pthreads to spawn in the pool.
 */
void search_patterns_pthread(const char* dna_string, int dna_string_length, pattern_t* patterns, int k_patterns, int num_threads);

/**
 * \brief Wrapper function to orchestrate the thread pool execution mode.
 * * Generates the necessary DNA and pattern data, invokes the optimized thread pool 
 * search engine, prints the results, and handles memory cleanup.
 * \param params Structure containing the execution parameters.
 */
void run_pthread_pool(params_t params);

#endif