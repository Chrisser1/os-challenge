//
// Created by chris on 9/4/25.
//

#ifndef OS_CHALLENGE_HASHING_H
#define OS_CHALLENGE_HASHING_H

#include <stdint.h>

#include "protocol.h"
#include "thread-pool.h"

typedef struct {
    uint64_t start;
    uint64_t end;
    const uint8_t *hash;
    uint64_t *found_answer;
    pthread_mutex_t *found_mutex;
    int *found_flag;
} HashWorkerArgs;

/**
 * @brief A helper function to safely set the found answer.
 *
 * This function is called by a worker thread when it finds the correct hash.
 * It locks the shared mutex, checks if another thread has already found the answer,
 * and if not, it sets the answer and the flag.
 * @param args A pointer to the worker's arguments.
 * @param answer The answer found by the thread.
 */
static void set_found_answer(HashWorkerArgs *args, uint64_t answer);

/**
 * @brief Reverse hashes the incoming request packet.
 *
 * @param request The 49-byte request packet received by the client.
 * @param num_hashing_threads The number of threads to use for the parallel search.
 * @return The reverse hashed uint64_t
 */
uint64_t reverse_hashing(const request_packet_t* request, int num_hashing_threads);

#endif //OS_CHALLENGE_HASHING_Hv