//
// Created by chris on 9/2/25.
//

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    printf("Hello from inside the VM!\n");
    printf("System details: \n");
    system("uname -a"); // This will print the kernel and hostname information
    printf("--------------------------------\n\n");

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return 1;
    }

    printf("Server starting on port %s.\n", argv[1]);

    return 0;
}