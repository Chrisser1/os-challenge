//
// Created by chris on 9/6/25.
//

#ifndef OS_CHALLENGE_DISPATCHER_QUEUE_H
#define OS_CHALLENGE_DISPATCHER_QUEUE_H
#include "thread-pool.h"

// A node in the linked list for the dispatcher queue
typedef struct SocketNode {
    int client_socket;
    struct SocketNode* next;
} SocketNode;

typedef struct {
    SocketNode *head;
    SocketNode *tail;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int shutdown;
} DispatcherQueue;

// A struct to pass multiple arguments to the dispatcher threads
typedef struct {
    DispatcherQueue *dispatcher_queue;
    ThreadPool *worker_pool;
} DispatcherArgs;

DispatcherQueue* dispatcher_queue_create();
void dispatcher_queue_destroy(DispatcherQueue* queue);

/**
 * @brief Pushes a new client socket onto the queue. (Producer)
 * This is called by the acceptor thread.
 * @param queue The dispatcher queue.
 * @param client_socket The new socket file descriptor.
 */
void dispatcher_queue_push(DispatcherQueue* queue, int client_socket);

/**
 * @brief Pulls a client socket from the queue. (Consumer)
 *
 * This is a blocking call. It will wait until a socket is available.
 * @param queue The dispatcher queue.
 * @return The client socket file descriptor, or -1 on shutdown.
 */
int dispatcher_queue_pull(DispatcherQueue* queue);

#endif //OS_CHALLENGE_DISPATCHER_QUEUE_H