//
// Created by chris on 9/4/25.
//

#ifndef OS_CHALLENGE_THREAD_POOL_H
#define OS_CHALLENGE_THREAD_POOL_H

#include <pthread.h>

#include "protocol.h"

// Define the priority levels
#define PRIORITY_MIN 1
#define PRIORITY_MAX 16

// Represent a single task in the queue.
// This is also a node in a singly linked list.
typedef struct Task {
    int client_socket;
    request_packet_t request;
    struct Task *next;
} Task;

// A simple struct to hold the head and tail of one queue
typedef struct {
    Task* head;
    Task* tail;
} TaskQueue;

typedef struct {
    pthread_t *threads;                      // Array of worker threads
    TaskQueue priority_queues[PRIORITY_MAX]; // An array of 16 queue that represent priority 1-16
    pthread_mutex_t queue_mutex;             // Mutex to protect the queue
    pthread_cond_t queue_cond;               // Condition variable to signal new tasks
    int shutdown;                            // Flag to signal threads to exit
    int thread_count;                        // Number of threads in the pool
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
 * @param request The request from the client socket
 */
void thread_pool_add_task(ThreadPool* thread_pool, int client_socket, const request_packet_t *request);

/**
 * @brief Gracefully destroy the thread pool, waiting for threads to finish.
 *
 * @param thread_pool The thread pool to destroy.
 */
void thread_pool_destroy(ThreadPool* thread_pool);

#endif //OS_CHALLENGE_THREAD_POOL_H