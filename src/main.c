//
// Created by chris on 9/2/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "server.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return 1;
    }

    // Safely Convert Port Argument to Integer using strtol
    char *endptr;
    errno = 0;

    long port_long = strtol(argv[1], &endptr, 10);

    if (endptr == argv[1]) {
        fprintf(stderr, "Error: Invalid port. No digits were found \n");
        return 1;
    }

    if (*endptr != '\0') {
        fprintf(stderr, "Error: Invalid port. Trailing characters are not allowed: %s\n", endptr);
        return 1;
    }

    // Check 3: Was the number out of the valid port range (1-65535)?
    if (port_long < 1 || port_long > 65535) {
        fprintf(stderr, "Error: Port number must be between 1 and 65535.\n");
        return 1;
    }

    int port = (int)port_long;

    start_server(port);

    return 0;
}