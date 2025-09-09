//
// Created by chris on 9/4/25.
//

#include <openssl/sha.h>

#include "hashing.h"

#include <endian.h>
#include <string.h>

uint64_t reverse_hashing(const request_packet_t *request) {
    const uint64_t start = request->start;
    const uint64_t end = request->end;
    const uint64_t unroll_factor = 4;
    const uint64_t loop_end = start + ((end - start) / unroll_factor) * unroll_factor;

    for (uint64_t i = start; i < loop_end; i += unroll_factor) {
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
        if (memcmp(request->hash, hashes[0], SHA256_DIGEST_LENGTH) == 0) return i;
        if (memcmp(request->hash, hashes[1], SHA256_DIGEST_LENGTH) == 0) return i + 1;
        if (memcmp(request->hash, hashes[2], SHA256_DIGEST_LENGTH) == 0) return i + 2;
        if (memcmp(request->hash, hashes[3], SHA256_DIGEST_LENGTH) == 0) return i + 3;
    }

    // Remainder loop to handle the last 0-3 iterations
    for (uint64_t i = loop_end; i < end; ++i) {
        uint64_t num_little_endian = htole64(i);
        unsigned char computed_hash[SHA256_DIGEST_LENGTH];
        SHA256((unsigned char*)&num_little_endian, sizeof(num_little_endian), computed_hash);
        if (memcmp(request->hash, computed_hash, SHA256_DIGEST_LENGTH) == 0) {
            return i;
        }
    }

    return 0;
}

