//
// Created by chris on 9/2/25.
//

#include <string.h>
#include <arpa/inet.h>

#include "protocol.h"

void parse_request(const char *buffer, request_packet_t *packet) {
    // Copy Data from buffer to struct
    memcpy(packet->hash, buffer, 32);
    memcpy(&packet->start, buffer + 32, 8);
    memcpy(&packet->end, buffer + 40, 8);
    memcpy(&packet->p, buffer + 48, 1);

    // The hash and priority (p) are single bytes or arrays of bytes,
    // so they don't have an endianness and don't need conversion.
    // The start and end fields are 64-bit integers and must be converted.
    packet->start = be64toh(packet->start);
    packet->end = be64toh(packet->end);
}

void create_response(const response_packet_t *packet, char *buffer) {
    // Convert from Host to Network Byte Order
    const uint64_t answer_in_network_order = htobe64(packet->answer);

    // Copy the data to the buffer
    memcpy(buffer, &answer_in_network_order, 8);
}
