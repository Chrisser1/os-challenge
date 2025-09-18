//
// Created by chris on 9/4/25.
//

#include <openssl/sha.h>

#include "hashing.h"

#include <endian.h>
#include <stdio.h>
#include <string.h>

#include "thread-pool.h"

static void set_found_answer(HashWorkerArgs *args, const uint64_t answer) {
    pthread_mutex_lock(args->found_mutex);
    // Double-check the flag to ensure we are the first thread to find the answer.
    if (!*(args->found_flag)) {
        *(args->found_flag) = 1;
        *(args->found_answer) = answer;
    }
    pthread_mutex_unlock(args->found_mutex);
}

void* hash_worker_function(void *args) {
    HashWorkerArgs* worker_args = (HashWorkerArgs*) args;
    const uint64_t start = worker_args->start;
    const uint64_t end = worker_args->end;

    const uint64_t unroll_factor = 4;
    const uint64_t loop_end = start + ((end - start) / unroll_factor) * unroll_factor;

    const uint64_t check_interval = 4096;
    for (uint64_t i = start; i < loop_end; i += unroll_factor) {
        // Check if another thread already found the result
        if ((i & (check_interval - 1)) == 0) { // A fast way to do (i % check_interval == 0)
            pthread_mutex_lock(worker_args->found_mutex);
            if (*(worker_args->found_flag)) {
                pthread_mutex_unlock(worker_args->found_mutex);
                return NULL; // Exit
            }
            pthread_mutex_unlock(worker_args->found_mutex);
        }

        // Prepare data for four parallel computations
        uint64_t nums_le[4] = {
            htole64(i),
            htole64(i + 1),
            htole64(i + 2),
            htole64(i + 3)
        };

        unsigned char hashes[4][SHA256_DIGEST_LENGTH];

        // The CPU can overlap the execution of these independent function calls
        SHA256((unsigned char*)&nums_le[0], sizeof(uint64_t), hashes[0]);
        SHA256((unsigned char*)&nums_le[1], sizeof(uint64_t), hashes[1]);
        SHA256((unsigned char*)&nums_le[2], sizeof(uint64_t), hashes[2]);
        SHA256((unsigned char*)&nums_le[3], sizeof(uint64_t), hashes[3]);

        // Check all four results
        if (memcmp(worker_args->hash, hashes[0], SHA256_DIGEST_LENGTH) == 0) {
            set_found_answer(worker_args, i);
            return NULL;
        }
        if (memcmp(worker_args->hash, hashes[1], SHA256_DIGEST_LENGTH) == 0) {
            set_found_answer(worker_args, i + 1);
            return NULL;
        }
        if (memcmp(worker_args->hash, hashes[2], SHA256_DIGEST_LENGTH) == 0) {
            set_found_answer(worker_args, i + 2);
            return NULL;
        }
        if (memcmp(worker_args->hash, hashes[3], SHA256_DIGEST_LENGTH) == 0) {
            set_found_answer(worker_args, i + 3);
            return NULL;
        }
    }

    // Remainder loop to handle the last 0-3 iterations
    for (uint64_t i = loop_end; i < end; ++i) {
        uint64_t num_little_endian = htole64(i);
        unsigned char computed_hash[SHA256_DIGEST_LENGTH];
        SHA256((unsigned char*)&num_little_endian, sizeof(num_little_endian), computed_hash);
        if (memcmp(worker_args->hash, computed_hash, SHA256_DIGEST_LENGTH) == 0) {
            set_found_answer(worker_args, i);
            return NULL;
        }
    }

    // Hash not found stop thread and return
    return NULL;
}

uint64_t reverse_hashing(const request_packet_t *request, const int num_hashing_threads) {
    if (num_hashing_threads < 1) {
        perror("num_hashing_threads must be > 0");
        return 0;
    }

    // Determine the number of threads to use
    pthread_t threads[num_hashing_threads];
    HashWorkerArgs args[num_hashing_threads];

    // Shared resources for all threads.
    uint64_t found_answer = 0;
    int found_flag = 0;
    pthread_mutex_t found_mutex = PTHREAD_MUTEX_INITIALIZER;

    const uint64_t total_range = request->end - request->start;
    const uint64_t chunk_size = total_range / num_hashing_threads;

    // --- Create and dispatch threads ---
    for (int i = 0; i < num_hashing_threads; ++i) {
        args[i].start = request->start + i * chunk_size;
        // The last thread takes care of any remainder.
        args[i].end = (i == num_hashing_threads - 1) ? request->end : args[i].start + chunk_size;
        args[i].hash = request->hash;
        args[i].found_answer = &found_answer;
        args[i].found_mutex = &found_mutex;
        args[i].found_flag = &found_flag;

        if (pthread_create(&threads[i], NULL, hash_worker_function, &args[i]) != 0) {
            perror("Failed to create thread");
            return 0;
        }
    }

    // Wait for threads to finish
    for (int i = 0; i < num_hashing_threads; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Cleanup and return
    pthread_mutex_destroy(&found_mutex);

    return found_answer;
}

