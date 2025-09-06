//
// Created by chris on 9/2/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "server.h"

#include "dispatcher_queue.h"
#include "hashing.h"
#include "protocol.h"
#include "thread-pool.h"
#include "common/logging.h"

#define NUM_DISPATCHER_THREADS 4
#define NUM_WORKER_THREADS 16

void* dispatcher_thread_function(void *args) {
    const DispatcherArgs *d_args = args;
    DispatcherQueue *d_queue = d_args->dispatcher_queue;
    ThreadPool *w_pool = d_args->worker_pool;

    while(1) {
        // Pull a socket from the acceptor's queue. This blocks.
        const int client_socket = dispatcher_queue_pull(d_queue);
        if (client_socket == -1) { // Shutdown signal
            break;
        }

        LOG_DEBUG("Dispatcher thread handling socket %d. Reading and parsing...", client_socket);

        char request_buffer[REQUEST_PACKET_SIZE];
        ssize_t total_bytes_read = 0;
        while (total_bytes_read < REQUEST_PACKET_SIZE) {
            const ssize_t bytes_read = read(client_socket, request_buffer + total_bytes_read, REQUEST_PACKET_SIZE - total_bytes_read);
            if (bytes_read <= 0) {
                LOG_DEBUG("Dispatcher failed to read from socket %d or client disconnected.", client_socket);
                close(client_socket);
                goto next_dispatch;
            }
            total_bytes_read += bytes_read;
        }

        request_packet_t request;
        parse_request(request_buffer, &request);

        // Add the fully formed task to the worker pool
        thread_pool_add_task(w_pool, client_socket, &request);

        next_dispatch:;
    }
    return NULL;
}

void start_server(const int port) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addr_len = sizeof(address);

    DispatcherQueue *dispatcher_queue = dispatcher_queue_create();
    ThreadPool *worker_pool = thread_pool_create(NUM_WORKER_THREADS);
    DispatcherArgs dispatcher_args = { dispatcher_queue, worker_pool };
    pthread_t dispatchers[NUM_DISPATCHER_THREADS];

    // Create a socket
    // AF_INET: Address family for IPv4
    // SOCK_STREAM: Socket type for TCP (connection-oriented)
    // 0: Protocol value for IP
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Configure the server address
    address.sin_family = AF_INET;
    // INADDR_ANY means the server will accept connections on any of the host's network interfaces.
    address.sin_addr.s_addr = INADDR_ANY;
    // htons converts the port number from host byte order to network byte order (Big Endian)
    address.sin_port = htons(port);

    // Bind the socket to the Address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Listen for Incoming Connections
    // 10 is the backlog, the maximum number of pending connections queued for acceptance.
    if (listen(server_fd, 10) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // --- Start the dispatcher threads ---
    for (int i = 0; i < NUM_DISPATCHER_THREADS; i++) {
        pthread_create(&dispatchers[i], NULL, dispatcher_thread_function, &dispatcher_args);
    }

    printf("Server is listening on port %d. Waiting for connections...\n", port);

    // Main server loop
    while (1) {
        // Accept a Connection
        // accept() is a blocking call. It will wait here until a client connects.
        // It returns a new socket file descriptor for the accepted connection.
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addr_len)) < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        LOG_DEBUG("Acceptor thread accepted socket %d. Pushing to dispatcher queue.", new_socket);
        dispatcher_queue_push(dispatcher_queue, new_socket);
    }
}

void handle_connection(const int client_socket, const request_packet_t *request) {
    LOG_DEBUG("Worker thread handling request: start=%lu, end=%lu, socket=%d", request->start, request->end, client_socket);

    // Create and send the response
    response_packet_t response;
    response.answer = reverse_hashing(request);

    const int response_size = 8;
    char response_buffer[response_size];
    create_response(&response, response_buffer);

    const ssize_t bytes_written = write(client_socket, response_buffer, response_size);

    if (bytes_written == -1) {
        perror("write failed");
    } else {
        LOG_DEBUG("Successfully sent response. Answer=%lu, socket=%d", response.answer, client_socket);
    }

    close(client_socket);
    LOG_DEBUG("Connection closed on socket %d.", client_socket);
}

