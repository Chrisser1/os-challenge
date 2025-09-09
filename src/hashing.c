//
// Created by chris on 9/4/25.
//

#include <openssl/sha.h>

#include "hashing.h"

#include <endian.h>
#include <string.h>

uint64_t reverse_hashing(const request_packet_t *request) {
    for (uint64_t i = request->start; i < request->end; ++i) {
        // The project requires the input to the hash function to be a 64-bit
        // integer in little-endian byte order.
        uint64_t num_little_endian = htole64(i);

        // Create a buffer for holding the hash
        unsigned char computed_hash[SHA256_DIGEST_LENGTH];

        // Compute the hash
        SHA256((unsigned char*)&num_little_endian, sizeof(num_little_endian), computed_hash);

        // Compare the N bytes of request hash with the computed hash
        if (memcmp(request->hash, computed_hash, SHA256_DIGEST_LENGTH) == 0) {
            return i;
        }
    }
    return 0; // Not found
}

