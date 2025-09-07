//
// Created by chris on 9/7/25.
//

#ifndef OS_CHALLENGE_CASHE_H
#define OS_CHALLENGE_CASHE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "thread-pool.h"

#define CACHE_KEY_SIZE 32

typedef struct CacheEntry {
    uint8_t key[CACHE_KEY_SIZE]; // The key stored as a list of uint8 instead of chars
    uint64_t value;              // The value of the cache entry
    struct CacheEntry *next;     // For handling collisions with separate chaining
} CacheEntry;

// The cache itself: a thread-safe hash map.
typedef struct {
    CacheEntry **buckets;   // An array of pointers to CacheEntry linked lists
    size_t capacity;        // The number of buckets in the array
    pthread_rwlock_t lock;  // A reader-writer lock for efficient, thread-safe access
} Cache;

/**
 * @brief Creates and initializes a new thread-safe cache.
 *
 * @param capacity The initial number of buckets for the hash map. Should be a power of 2.
 * @return A pointer to the new Cache, or NULL on failure.
 */
Cache* cache_create(size_t capacity);

/**
 * @brief Destroys the cache and frees all associated memory.
 *
 * @param cache The cache to destroy.
 */
void cache_destroy(Cache* cache);

/**
 * @brief Looks up a key in the cache.
 *
 * This is a read operation and can be performed by multiple threads concurrently.
 * @param cache The cache to search.
 * @param key The 32-byte hash key to look for.
 * @param value A pointer to a uint64_t where the found value will be stored.
 * @return 1 if the key was found (a "cache hit"), 0 otherwise (a "cache miss").
 */
int cache_get(Cache* cache, const uint8_t* key, uint64_t* value);

/**
 * @brief Inserts or updates a key-value pair in the cache.
 *
 * This is a write operation and can only be performed by one thread at a time.
 * @param cache The cache to insert into.
 * @param key The 32-byte hash key to insert.
 * @param value The 64-bit answer to associate with the key.
 */
void cache_put(Cache* cache, const uint8_t* key, uint64_t value);

#endif //OS_CHALLENGE_CASHE_H