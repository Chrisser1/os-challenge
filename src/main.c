//
// Created by chris on 9/2/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "server.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return 1;
    }

    // Safely Convert Port Argument to Integer using strtol
    char *end_ptr;
    errno = 0;

    const long port_long = strtol(argv[1], &end_ptr, 10);

    if (end_ptr == argv[1]) {
        fprintf(stderr, "Error: Invalid port. No digits were found \n");
        return 1;
    }

    if (*end_ptr != '\0') {
        fprintf(stderr, "Error: Invalid port. Trailing characters are not allowed: %s\n", end_ptr);
        return 1;
    }

    // Was the number out of the valid port range (1-65535)?
    if (port_long < 1 || port_long > 65535) {
        fprintf(stderr, "Error: Port number must be between 1 and 65535.\n");
        return 1;
    }

    const int port = (int)port_long;

    start_server(port);

    return 0;
}