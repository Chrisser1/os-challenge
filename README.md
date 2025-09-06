# OS Challenge - To-Do List

## Phase 1: Milestone Submission (Focus on Reliability)

The goal for the milestone is to create a server that works correctly and achieves 100% reliability. Performance is not the main focus yet.

- [X] **1. TCP Server Basics:**
    - [X] Implement a basic TCP server that can `listen` for incoming connections on a specified port.
    - [X] Learn to `accept` a new connection from the client.
    - [X] Use `read()` or `recv()` to receive the 49-byte request packet from the client.
    - [X] Use `write()` or `send()` to send the 8-byte response packet back.
    - [X] Close the connection after each request is handled.

- [X] **2. Packet Handling:**
    - [X] Carefully parse the incoming request packet fields: `hash` (32 bytes), `start` (8 bytes), `end` (8 bytes), and `priority` (1 byte).
    - [X] Pay close attention to **byte order**. The `start` and `end` fields are big-endian (network byte order) and need to be converted to the host's byte order (e.g., using `be64toh`).

- [X] **3. Implement Reverse Hashing Logic:**
    - [X] Set up a loop that iterates from `start` to `end`.
    - [X] Inside the loop, for each number, convert it to **little-endian** byte order as this is the format the client used to generate the original hash.
    - [X] Use the OpenSSL `SHA256()` function to compute the hash of the 64-bit number.
    - [X] Compare the computed hash with the hash received in the request.
    - [X] If a match is found, store the number—this is your answer.

- [X] **4. Send the Response:**
    - [X] Once the correct number is found, convert it back to **big-endian** (network byte order) for the response packet.
    - [X] Send the 8-byte answer back to the client.

- [X] **5. Testing and Debugging:**
    - [X] Use the provided `client` tool for testing.
    - [X] Start with simple, deterministic parameters to debug, e.g.: `difficulty`=1.
    - [X] Verify that the server returns the correct answer and achieves 100% reliability with the `run-client.sh` and `run-client-milestone.sh` scripts.

- [X] **6. Handle Multiple Connections:**
  - [X] Measure the speed of the program running on a single thread and document the test.
  - [X] Create a way to multiple concurrent requests.
  - [X] Test the code again and document if it is better or worse.

- [ ] **7. Expand the multiprocess to handle priority:**
  - [ ] Test with different priority levels and write it down.
  - [ ] Create a way to handle priorities.
  - [ ] Test again and compare.

## Phase 2: Performance Optimization
- [ ] **8. Implement Caching Mechanism:**
  - [ ] Design and implement a thread-safe hash map to store `(hash, answer)` pairs.
  - [ ] Modify the `handle_connection` logic to check the cache before performing the reverse-hashing search.
  - [ ] On a "cache miss," add the newly computed answer to the cache.
  - [ ] Run the benchmark tests as defined in `experiment-caching.md` to measure the performance impact.

- [ ] **9. Explore Vectorization (SIMD):**
  - [ ] Research how to use CPU SIMD intrinsics (like AVX or SSE) in C.
  - [ ] Create a new experiment to rewrite the core hashing loop to test multiple numbers simultaneously.
  - [ ] Measure the performance gain of the vectorized hashing function compared to the standard loop.
