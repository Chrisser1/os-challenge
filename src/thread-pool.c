//
// Created by chris on 9/4/25.
//

#include "thread-pool.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "server.h"
#include "common/logging.h"

// Forward declaration is important for the thread creation
static void* worker_thread_function(void *arg);

// Helper function to check if any task exists in any queue.
static int is_any_task_available(ThreadPool *pool) {
    for (int i = 0; i < PRIORITY_MAX; i++) {
        pthread_mutex_lock(&pool->queue_mutexes[i]);
        if (pool->priority_queues[i].head != NULL) {
            pthread_mutex_unlock(&pool->queue_mutexes[i]);
            return 1;
        }
        pthread_mutex_unlock(&pool->queue_mutexes[i]);
    }
    return 0;
}

ThreadPool * thread_pool_create(const int thread_count) {
    ThreadPool *thread_pool = malloc(sizeof(ThreadPool));
    if (thread_pool == NULL) {
        perror("Malloc for thread pool failed");
        return NULL;
    }

    thread_pool->thread_count = thread_count;
    for (int i = 0; i < PRIORITY_MAX; i++) {
        thread_pool->priority_queues[i].head = NULL;
        thread_pool->priority_queues[i].tail = NULL;
        // Initialize individual queue mutexes
        pthread_mutex_init(&thread_pool->queue_mutexes[i], NULL);
    }
    // Initialize the global mutex and cond
    pthread_mutex_init(&thread_pool->queue_mutex, NULL);
    pthread_cond_init(&thread_pool->queue_cond, NULL);
    thread_pool->shutdown = 0;

    // Allocate memory for thread IDs
    thread_pool->threads = malloc(sizeof(pthread_t) * thread_count);
    if (thread_pool->threads == NULL) {
        perror("Malloc for pool->threads failed");
        free(thread_pool);
        return NULL;
    }


    // Create the worker threads
    for (int i = 0; i < thread_count; i++) {
        if (pthread_create(&thread_pool->threads[i], NULL, worker_thread_function, thread_pool) != 0) {
            perror("pthread_create failed");
            thread_pool_destroy(thread_pool);
            return NULL;
        }
    }

    LOG_DEBUG("Thread pool with %d threads created successfully.", thread_count);
    return thread_pool;
}

void thread_pool_add_task(ThreadPool *thread_pool, const int client_socket, const request_packet_t *request) {
    const int priority_index = request->p - 1; // Value should be 1-16

    // Allocate the task
    Task *new_task = malloc(sizeof(Task));
    if (new_task == NULL) {
        perror("Malloc for new_task failed");
        return;
    }
    new_task->client_socket = client_socket;
    new_task->next = NULL;
    // Copy the struck into the task
    memcpy(&new_task->request, request, sizeof(request_packet_t));

    // Lock the mutex to safely add the task to the queue
    pthread_mutex_lock(&thread_pool->queue_mutexes[priority_index]);

    // Get the tasks priority queue
    TaskQueue *task_queue = &thread_pool->priority_queues[priority_index];

    // Add to the tail of the queue
    if (task_queue->tail == NULL) {
        task_queue->head = new_task;
        task_queue->tail = new_task;
    } else {
        task_queue->tail->next = new_task;
        task_queue->tail = new_task;
    }

    // Unluck the priority mutex
    pthread_mutex_unlock(&thread_pool->queue_mutexes[priority_index]);

    // Signal the global condition variable that there is a new task
    pthread_mutex_lock(&thread_pool->queue_mutex);
    pthread_cond_signal(&thread_pool->queue_cond);
    pthread_mutex_unlock(&thread_pool->queue_mutex);
}

void thread_pool_destroy(ThreadPool *thread_pool) {
    if (thread_pool == NULL) return;

    // Lock the mutex and set the shutdown flag
    pthread_mutex_lock(&thread_pool->queue_mutex);
    thread_pool->shutdown = 1;
    // Broadcast to all threads to wake them up from pthread_cond_wait
    pthread_cond_broadcast(&thread_pool->queue_cond);
    pthread_mutex_unlock(&thread_pool->queue_mutex);

    // Wait for all threads to terminate
    for (int i = 0; i < thread_pool->thread_count; i++) {
        pthread_join(thread_pool->threads[i], NULL);
    }

    // Free all resources
    free(thread_pool->threads);
    pthread_mutex_destroy(&thread_pool->queue_mutex);
    pthread_cond_destroy(&thread_pool->queue_cond);
    for (int i = 0; i < PRIORITY_MAX; i++) {
        pthread_mutex_destroy(&thread_pool->queue_mutexes[i]);
    }

    // Free any remaining tasks in the queue
    for (int i = 0; i < PRIORITY_MAX; i++) {
        Task *current_task = thread_pool->priority_queues[i].head;
        while (current_task != NULL) {
            Task *temp = current_task;
            current_task = current_task->next;
            free(temp);
        }
    }
    free(thread_pool);
    LOG_DEBUG("Thread pool destroyed successfully.");
}

static void* worker_thread_function(void *arg) {
    ThreadPool *thread_pool = arg;
    Task *task = NULL;

    while (1) {
        // Lock the global mutex to wait on the condition variable
        pthread_mutex_lock(&thread_pool->queue_mutex);
        while (!is_any_task_available(thread_pool) && !thread_pool->shutdown) {
            pthread_cond_wait(&thread_pool->queue_cond, &thread_pool->queue_mutex);
        }

        // If shutdown is signaled, exit the thread.
        if (thread_pool->shutdown) {
            pthread_mutex_unlock(&thread_pool->queue_mutex);
            pthread_exit(NULL);
        }

        pthread_mutex_unlock(&thread_pool->queue_mutex);

        task = NULL;
        for (int i = PRIORITY_MAX - 1; i >= 0; i--) {
            // Lock only the mutex for the queue we are checking
            pthread_mutex_lock(&thread_pool->queue_mutexes[i]);
            if (thread_pool->priority_queues[i].head != NULL) {
                task = thread_pool->priority_queues[i].head;
                thread_pool->priority_queues[i].head = task->next;
                if (thread_pool->priority_queues[i].head == NULL) {
                    thread_pool->priority_queues[i].tail = NULL;
                }
                pthread_mutex_unlock(&thread_pool->queue_mutexes[i]);
                break; // Task found, break the loop
            }
            pthread_mutex_unlock(&thread_pool->queue_mutexes[i]);
        }

        // If a task was dequeued, handle the connection.
        if (task != NULL) {
            handle_connection(task->client_socket, &task->request);
            free(task);
        }
    }
}