//
// Created by chris on 9/2/25.
//

#ifndef OS_CHALLENGE_SERVER_H
#define OS_CHALLENGE_SERVER_H

/**
 * @brief Initializes and starts the TCP server.
 *
 * This function creates a socket, binds it to the specified port,
 * and enters an infinite loop to listen for an accept incoming connections.
 * @param port The TCP port number ther server should listen on.
 */
void start_server(int port);

#endif //OS_CHALLENGE_SERVER_H