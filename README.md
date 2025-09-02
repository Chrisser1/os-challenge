# OS Challenge - To-Do List

## Phase 1: Milestone Submission (Focus on Reliability)

The goal for the milestone is to create a server that works correctly and achieves 100% reliability. Performance is not the main focus yet.

- [ ] **1. TCP Server Basics:**
    - [X] Implement a basic TCP server that can `listen` for incoming connections on a specified port.
    - [X] Learn to `accept` a new connection from the client.
    - [ ] Use `read()` or `recv()` to receive the 49-byte request packet from the client.
    - [ ] Use `write()` or `send()` to send the 8-byte response packet back.
    - [ ] Close the connection after each request is handled.

- [ ] **2. Packet Handling:**
    - [ ] Carefully parse the incoming request packet fields: `hash` (32 bytes), `start` (8 bytes), `end` (8 bytes), and `priority` (1 byte).
    - [ ] Pay close attention to **byte order**. The `start` and `end` fields are big-endian (network byte order) and need to be converted to the host's byte order (e.g., using `be64toh`).

- [ ] **3. Implement Reverse Hashing Logic:**
    - [ ] Set up a loop that iterates from `start` to `end`.
    - [ ] Inside the loop, for each number, convert it to **little-endian** byte order as this is the format the client used to generate the original hash.
    - [ ] Use the OpenSSL `SHA256()` function to compute the hash of the 64-bit number.
    - [ ] Compare the computed hash with the hash received in the request.
    - [ ] If a match is found, store the number—this is your answer.

- [ ] **4. Send the Response:**
    - [ ] Once the correct number is found, convert it back to **big-endian** (network byte order) for the response packet.
    - [ ] Send the 8-byte answer back to the client.

- [ ] **5. Testing and Debugging:**
    - [ ] Use the provided `client` tool for testing.
    - [ ] Start with simple, deterministic parameters to debug, for example: `difficulty`=1. This makes the hash easy to find.
    - [ ] Verify that your server returns the correct answer and achieves 100% reliability with the `run-client.sh` and `run-client-milestone.sh` scripts.

- [ ] **6. Handle Multiple Connections:**
    - [ ] The basic server can only handle one client at a time. The next step is to handle multiple concurrent requests. A simple approach for the milestone could be an iterative server that handles one connection completely before accepting the next, or you can begin exploring multi-threading/multi-processing.