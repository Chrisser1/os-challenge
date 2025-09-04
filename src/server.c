//
// Created by chris on 9/2/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "server.h"
#include "hashing.h"
#include "protocol.h"
#include "thread-pool.h"
#include "common/logging.h"

#define NUM_WORKER_THREADS 16

void start_server(const int port) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addr_len = sizeof(address);

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

    // Create the thread pool
    ThreadPool *thread_pool = thread_pool_create(NUM_WORKER_THREADS);
    if (thread_pool == NULL) {
        fprintf(stderr, "Failed to create thread pool. Exiting.\n");
        exit(EXIT_FAILURE);
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

        LOG_DEBUG("Connection accepted on socket %d. Adding to task queue.", new_socket);

        thread_pool_add_task(thread_pool, new_socket);
    }
}

void handle_connection(const int client_socket) {
    LOG_DEBUG("Worker thread is handling client on socket %d", client_socket);

    const int request_size = 49;
    char request_buffer[request_size];
    ssize_t bytes_read;
    ssize_t total_bytes_read = 0;

    while (total_bytes_read < request_size) {
        bytes_read = read(client_socket, request_buffer + total_bytes_read, request_size - total_bytes_read);

        // Check for invalid bytes
        if (bytes_read <= 0) {
            if (bytes_read == -1) perror("read failed");
            else {
                LOG_DEBUG("Client disconnected unexpectedly on socket %d.", client_socket);
            }
            close(client_socket);
            return;
        }

        total_bytes_read += bytes_read;
    }

    LOG_DEBUG("Successfully received %zd bytes from client on socket %d.", total_bytes_read, client_socket);

    // Parse a struct to hold the parsed data
    request_packet_t request;
    parse_request(request_buffer, &request);

    // Print the values to confirm they are being parsed and converted correctly.
    LOG_DEBUG("Received Request: start=%lu, end=%lu, socket=%d", request.start, request.end, client_socket);

    // Create and send the response
    response_packet_t response;
    response.answer = reverse_hashing(&request);

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

