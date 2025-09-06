//
// Created by chris on 9/6/25.
//

#include "dispatcher_queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

DispatcherQueue * dispatcher_queue_create() {
    DispatcherQueue *queue = malloc(sizeof(DispatcherQueue));
    if (!queue) {
        perror("malloc dispatcher queue");
        return NULL;
    }
    queue->head = NULL;
    queue->tail = NULL;
    queue->shutdown = 0;
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->cond, NULL);
    return queue;
}

void dispatcher_queue_destroy(DispatcherQueue *queue) {
    if (!queue) return;

    pthread_mutex_lock(&queue->mutex);
    queue->shutdown = 1;
    pthread_cond_broadcast(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);

    // Free any remaining nodes
    SocketNode *current = queue->head;
    while (current) {
        SocketNode* temp = current;
        current = current->next;
        free(temp);
    }

    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->cond);
    free(queue);
}

void dispatcher_queue_push(DispatcherQueue *queue, const int client_socket) {
    SocketNode *new_node = malloc(sizeof(SocketNode));
    new_node->client_socket = client_socket;
    new_node->next = NULL;

    pthread_mutex_lock(&queue->mutex);
    if (queue->tail == NULL) {
        queue->head = new_node;
        queue->tail = new_node;
    } else {
        queue->tail->next = new_node;
        queue->tail = new_node;
    }

    // Signal a waiting dispatcher thread
    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);
}

int dispatcher_queue_pull(DispatcherQueue *queue) {
    pthread_mutex_lock(&queue->mutex);

    while (queue->head == NULL && !queue->shutdown) {
        pthread_cond_wait(&queue->cond, &queue->mutex);
    }

    if (queue->shutdown && queue->head == NULL) {
        pthread_mutex_unlock(&queue->mutex);
        return -1; // Shutdown signal
    }

    SocketNode* node = queue->head;
    const int client_socket = node->client_socket;
    queue->head = node->next;
    if (queue->head == NULL) {
        queue->tail = NULL;
    }
    free(node);

    pthread_mutex_unlock(&queue->mutex);
    return client_socket;
}