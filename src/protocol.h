//
// Created by chris on 9/2/25.
//

#ifndef OS_CHALLENGE_PROTOCOL_H
#define OS_CHALLENGE_PROTOCOL_H

#include <stdint.h>

// The size of the request packet in bytes.
#define REQUEST_PACKET_SIZE 49

// This struct directly maps to the 49-byte request packet format.
// Using __attribute__((__packed__)) tells the compiler not to add any padding.
// between the fields, ensuring its memory layout is exactly as specified.
typedef struct __attribute__((__packed__)) {
    uint8_t hash[32];
    uint64_t start;
    uint64_t end;
    uint8_t p;
} request_packet_t;

// This struct maps to the 8-byte response packet.
typedef struct __attribute__((__packed__)) {
    uint64_t answer;
} response_packet_t;

/**
 * @brief Parses a raw network buffer into a structured request packet.
 *
 * This function handles copying the data from the buffer and converting
 * the multi-byte fields from network byte order (Big Endian) to host byte order.
 * @param buffer A pointer to the 49-byte buffer received from the network.
 * @param packet A pointer to a request_packet_t struct to be filled.
 */
void parse_request(const char *buffer, request_packet_t *packet);

/**
 * @brief Creates a raw network buffer from a structures response packet.
 *
 * This function handles converting the multi-byte fields from host byte order
 * to network byte order (Big Endian) and copying them into the buffer.
 * @param packet A pointer to the response_packet_t struct containing the answer.
 * @param buffer A pointer to the 8-byte buffer to be sent over the network.
 */
void create_response(const response_packet_t *packet, char *buffer);

#endif //OS_CHALLENGE_PROTOCOL_H