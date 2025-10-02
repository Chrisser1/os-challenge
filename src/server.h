//
// Created by chris on 9/2/25.
//

#pragma once

#include "protocol.h"

/**
 * @brief Initializes and starts the TCP server.
 *
 * This function creates a socket, binds it to the specified port,
 * and enters an infinite loop to listen for an accept incoming connections.
 * @param port The TCP port number the server should listen on.
 */
void start_server(int port);

/**
 * @brief Handles the client request to the TCP server.
 *
 * @param client_socket The client socket to write to
 * @param request The request from the client
 */
void handle_connection(int client_socket, const request_packet_t *request);