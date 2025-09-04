//
// Created by chris on 9/4/25.
//

#ifndef OS_CHALLENGE_HASHING_H
#define OS_CHALLENGE_HASHING_H

#include <stdint.h>

#include "protocol.h"

/**
 * @brief Reverse hashes the incoming request packet.
 *
 * @param request The 49-byte request packet received by the client.
 * @return The reverse hashed uint64_t
 */
uint64_t reverse_hashing(request_packet_t* request);

#endif //OS_CHALLENGE_HASHING_Hv