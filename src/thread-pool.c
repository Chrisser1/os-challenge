//
// Created by chris on 9/4/25.
//

#include "thread-pool.h"

#include <stdio.h>
#include <stdlib.h>

#include "server.h"
#include "common/logging.h"

static void* worker_thread_function(void *arg) {
    ThreadPool *thread_pool = arg;
    Task *task;

    while (1) {
        // Lock the mutex to safely access the task queue
        pthread_mutex_lock(&thread_pool->queue_mutex);

        // Wait on th condition variable if the queue is empty and shutdown is not requested
        while (thread_pool->task_queue_head == NULL && !thread_pool->shutdown) {
            pthread_cond_wait(&thread_pool->queue_cond, &thread_pool->queue_mutex); // Auto unlocks the mutex
        }

        // If shutdown is requested and the queue is empty, exit the thread.
        if (thread_pool->shutdown && thread_pool->task_queue_head == NULL) {
            pthread_mutex_unlock(&thread_pool->queue_mutex);
            // This will stop the worker thread and exit the loop
            pthread_exit(NULL);
        }

        // Take a task from the head of the queue
        task = thread_pool->task_queue_head;
        thread_pool->task_queue_head = thread_pool->task_queue_head->next;
        if (thread_pool->task_queue_head == NULL) {
            thread_pool->task_queue_tail = NULL;
        }

        // Unluck the mutex as we are done with the shared queue
        pthread_mutex_unlock(&thread_pool->queue_mutex);

        // If there is a task, then handle the connection
        if (task != NULL) {
            handle_connection(task->client_socket);
            free(task);
        }
    }
}

ThreadPool * thread_pool_create(const int thread_count) {
    ThreadPool *thread_pool = malloc(sizeof(ThreadPool));
    if (thread_pool == NULL) {
        perror("Malloc for thread pool failed");
        return NULL;
    }

    thread_pool->thread_count = thread_count;
    thread_pool->task_queue_head = NULL;
    thread_pool->task_queue_tail = NULL;
    thread_pool->shutdown = 0;

    // Allocate memory for thread IDs
    thread_pool->threads = malloc(sizeof(pthread_t) * thread_count);
    if (thread_pool->threads == NULL) {
        perror("Malloc for pool->threads failed");
        free(thread_pool);
        return NULL;
    }

    // Initialize mutex and condition variable
    pthread_mutex_init(&thread_pool->queue_mutex, NULL);
    pthread_cond_init(&thread_pool->queue_cond, NULL);

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

void thread_pool_add_task(ThreadPool *thread_pool, const int client_socket) {
    Task *new_task = malloc(sizeof(Task));
    if (new_task == NULL) {
        perror("Malloc for new_task failed");
        return;
    }
    new_task->client_socket = client_socket;
    new_task->next = NULL;

    // Lock the mutex to safely add the task to the queue
    pthread_mutex_lock(&thread_pool->queue_mutex);

    // Add to the tail of the queue
    if (thread_pool->task_queue_tail == NULL) {
        thread_pool->task_queue_head = new_task;
        thread_pool->task_queue_tail = new_task;
    } else {
        thread_pool->task_queue_tail->next = new_task;
        thread_pool->task_queue_tail = new_task;
    }

    // Signal one waiting worker thread that there is a new task
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
    // Free any remaining tasks in the queue
    Task *current_task = thread_pool->task_queue_head;
    while (current_task != NULL) {
        Task *temp = current_task;
        current_task = current_task->next;
        free(temp);
    }
    free(thread_pool);
    LOG_DEBUG("Thread pool destroyed successfully.");
}
