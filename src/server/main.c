//
// Created by chris on 9/2/25.
//

#include "main.h"
#include "studio.h"

int main(int argc, const char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return 1;
    }

    printf("Hello, OS Challenge! The server will run on port %s.\n", argv[1]);

    // TODO: Implement TCP server logic

    return 0;
}