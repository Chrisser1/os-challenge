//
// Created by chris on 9/4/25.
//

#include <openssl/sha.h>

#include "hashing.h"

#include <endian.h>
#include <string.h>
#include <immintrin.h>

static void sha256_x4(unsigned char *out[4], const unsigned char *in[4], const unsigned long len[4]) {
    for (int i = 0; i < 4; i++) {
        SHA256(in[i], len[i], out[i]);
    }
}

uint64_t reverse_hashing(const request_packet_t *request) {
    uint64_t i = request->start;

    for (const int BATCH_SIZE = 4; i + BATCH_SIZE <= request->end; i += BATCH_SIZE) {
        // Create a batch of 4 consecutive numbers.
        const uint64_t numbers[] = { i, i+1, i+2, i+3 };
        uint64_t le_numbers[BATCH_SIZE];

#if __BYTE_ORDER == __LITTLE_ENDIAN
        // On a little-endian machine (like x86_64), htole64 is a no-op.
        // The fastest operation is a single, vectorized memory copy.
        _mm256_storeu_si256((__m256i*)le_numbers, _mm256_loadu_si256((const __m256i*)numbers));
#else
        // On a big-endian machine, we must perform a byte swap on all 4 numbers at once.
        // This shuffle mask defines the reordering for a 64-bit byte swap, repeated 4 times.
        const __m256i shuffle_mask = _mm256_set_epi8(
            // Lane 3 (bytes 24-31) -> reverse order
            24, 25, 26, 27, 28, 29, 30, 31,
            // Lane 2 (bytes 16-23) -> reverse order
            16, 17, 18, 19, 20, 21, 22, 23,
            // Lane 1 (bytes 8-15) -> reverse order
            8, 9, 10, 11, 12, 13, 14, 15,
            // Lane 0 (bytes 0-7) -> reverse order
            0, 1, 2, 3, 4, 5, 6, 7
        );

        // Load the 4 numbers from memory into an AVX register.
        __m256i numbers_vec = _mm256_loadu_si256((const __m256i*)numbers);
        // Apply the shuffle mask to reverse the bytes in each 64-bit lane.
        __m256i shuffled_vec = _mm256_shuffle_epi8(numbers_vec, shuffle_mask);
        // Store the result back to memory.
        _mm256_storeu_si256((__m256i*)le_numbers, shuffled_vec);
#endif

        // Compute 4 Hashes in parallel
        unsigned char computed_hashes[BATCH_SIZE][SHA256_DIGEST_LENGTH];
        const unsigned char* in_pointers[BATCH_SIZE];
        unsigned char* out_pointers[BATCH_SIZE];
        unsigned long lengths[BATCH_SIZE];

        for (int j = 0; j < BATCH_SIZE; ++j) {
            in_pointers[j] = (const unsigned char*)&le_numbers[j];
            out_pointers[j] = computed_hashes[j];
            lengths[j] = sizeof(uint64_t);
        }

        sha256_x4(out_pointers, in_pointers, lengths);

        // We can also use SIMD to compare the results very quickly.
        // Load the target hash into an AVX register.
        const __m256i target_hash_vec = _mm256_loadu_si256((__m256i*)request->hash);

        for (int j = 0; j < BATCH_SIZE; ++j) {
            // Load the computed hash into a register.
            const __m256i computed_hash_vec = _mm256_loadu_si256((__m256i*)computed_hashes[j]);

            // Compare the two 256-bit registers.
            const __m256i comparison_result = _mm256_cmpeq_epi8(target_hash_vec, computed_hash_vec);

            // _mm256_testc_si256 checks if all bits are 1. If so, it's a perfect match.
            // The result is 0xFF...FF if equal, so testc will return 1.
            if (_mm256_testc_si256(comparison_result, _mm256_set1_epi8(-1))) {
                return numbers[j]; // Found the answer!
            }
        }
    }

    // Handle the tail end of the range that didn't fit into a full batch of 4.
    for (; i < request->end; ++i) {
        uint64_t num_little_endian = htole64(i);
        unsigned char computed_hash[SHA256_DIGEST_LENGTH];
        SHA256((unsigned char*)&num_little_endian, sizeof(num_little_endian), computed_hash);
        if (memcmp(request->hash, computed_hash, SHA256_DIGEST_LENGTH) == 0) {
            return i;
        }
    }

    return 0;
}
