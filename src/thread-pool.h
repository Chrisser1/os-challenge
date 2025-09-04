//
// Created by chris on 9/4/25.
//

#ifndef OS_CHALLENGE_THREAD_POOL_H
#define OS_CHALLENGE_THREAD_POOL_H

#include <pthread.h>

// Represent a single task in the queue.
// This is also a node in a singly linked list.
typedef struct Task {
    int client_socket;
    struct Task* next;
} Task;

typedef struct {
    pthread_t *threads;          // Array of worker threads
    Task *task_queue_head;       // Head of the task queue
    Task *task_queue_tail;       // Tail of the task queue
    pthread_mutex_t queue_mutex; // Mutex to protect the queue
    pthread_cond_t queue_cond;   // Condition variable to signal new tasks
    int shutdown;                // Flag to signal threads to exit
    int thread_count;            // Number of threads in the pool
} ThreadPool;

/**
 * @brief Creates and initializes a thread pool.
 *
 * @param thread_count The number of worker threads to create.
 * @return A pointer to the newly created ThreadPool, or NULL on failure.
 */
ThreadPool* thread_pool_create(int thread_count);

/**
 * @brief Adds a new task (client socket) to the thread pool's queue.
 *
 * @param thread_pool The thread pool.
 * @param client_socket The client socket file descriptor to be handled.
 */
void thread_pool_add_task(ThreadPool* thread_pool, int client_socket);

/**
 * @brief Gracefully destroy the thread pool, waiting for threads to finish.
 *
 * @param thread_pool The thread pool to destroy.
 */
void thread_pool_destroy(ThreadPool* thread_pool);

#endif //OS_CHALLENGE_THREAD_POOL_H