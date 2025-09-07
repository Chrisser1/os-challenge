//
// Created by chris on 9/7/25.
//

#include "cache.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// The key for our cache is the 32-byte hash from the request.
// It is passed as a `const uint8_t*` to be generic, but you can
// also pass a `const char*` as they are compatible.
static uint64_t hash_key(const uint8_t* key) {
    // The key is already a 32-byte cryptographically random SHA256 hash.
    // We don't need to re-hash it. We can just use the first 8 bytes
    // directly as our 64-bit hash value for the hash table.
    uint64_t hash_value;
    memcpy(&hash_value, key, sizeof(uint64_t));
    return hash_value;
}

Cache* cache_create(size_t capacity) {
    if (capacity == 0) return NULL;

    Cache* cache = malloc(sizeof(Cache));
    if (!cache) {
        perror("malloc cache");
        return NULL;
    }

    cache->capacity = capacity;
    // Use calloc to initialize all bucket pointers to NULL
    cache->buckets = calloc(capacity, sizeof(CacheEntry*));
    if (!cache->buckets) {
        perror("calloc cache buckets");
        free(cache);
        return NULL;
    }

    // Initialize the reader-writer lock for thread safety
    if (pthread_rwlock_init(&cache->lock, NULL) != 0) {
        perror("pthread_rwlock_init");
        free(cache->buckets);
        free(cache);
        return NULL;
    }

    return cache;
}

void cache_destroy(Cache* cache) {
    if (!cache) return;

    // Iterate through all buckets and free the linked lists
    for (size_t i = 0; i < cache->capacity; i++) {
        CacheEntry* entry = cache->buckets[i];
        while (entry) {
            CacheEntry* temp = entry;
            entry = entry->next;
            free(temp);
        }
    }

    free(cache->buckets);
    pthread_rwlock_destroy(&cache->lock);
    free(cache);
}

int cache_get(Cache* cache, const uint8_t* key, uint64_t* value) {
    // Using modulo is a safe way to get the index, even if capacity is not a power of 2.
    const size_t index = hash_key(key) % cache->capacity;
    int found = 0;

    // Acquire a read lock. Multiple threads can hold this lock simultaneously.
    pthread_rwlock_rdlock(&cache->lock);

    // Traverse the linked list at the bucket index
    const CacheEntry* entry = cache->buckets[index];
    while (entry) {
        // Use memcmp for safe, fixed-size comparison of binary keys
        if (memcmp(entry->key, key, CACHE_KEY_SIZE) == 0) {
            *value = entry->value;
            found = 1;
            break;
        }
        entry = entry->next;
    }

    // Release the read lock
    pthread_rwlock_unlock(&cache->lock);

    return found;
}

void cache_put(Cache* cache, const uint8_t* key, uint64_t value) {
    const size_t index = hash_key(key) % cache->capacity;

    // Acquire a write lock. Only one thread can hold this lock at a time.
    pthread_rwlock_wrlock(&cache->lock);

    // First, check if the key already exists to avoid duplicates.
    CacheEntry* entry = cache->buckets[index];
    while (entry) {
        if (memcmp(entry->key, key, CACHE_KEY_SIZE) == 0) {
            // Key already exists, just update the value.
            entry->value = value;
            pthread_rwlock_unlock(&cache->lock);
            return;
        }
        entry = entry->next;
    }

    // Key does not exist, so create and insert a new entry.
    CacheEntry* new_entry = malloc(sizeof(CacheEntry));
    if (!new_entry) {
        // If malloc fails, we can't do anything, but we must release the lock.
        pthread_rwlock_unlock(&cache->lock);
        return;
    }
    memcpy(new_entry->key, key, CACHE_KEY_SIZE);
    new_entry->value = value;

    // Insert the new entry at the head of the linked list for this bucket.
    new_entry->next = cache->buckets[index];
    cache->buckets[index] = new_entry;

    // Release the write lock
    pthread_rwlock_unlock(&cache->lock);
}
