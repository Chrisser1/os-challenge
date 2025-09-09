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


## Phase 2: Performance Optimization
- [X] **6. Handle Multiple Connections:**
  - [X] Measure the speed of the program running on a single thread and document the test.
  - [X] Create a way to multiple concurrent requests.
  - [X] Test the code again and document if it is better or worse.

- [X] **7. Expand the multiprocess to handle priority:**
  - [X] Test with different priority levels and write it down.
  - [X] Create a way to handle priorities.
  - [X] Test again and compare.

- [X] **8. Explore Multi-Stage Architecture:**
  - [X] Design and implement a new architecture with specialized thread pools.
  - [X] Acceptor Thread: A single thread whose only job is to `accept()` new connections and pass the socket to a queue.
  - [X] Dispatcher Threads: A small pool of threads that read and parse requests from the acceptor's queue and place them into the worker pool's priority queues.
  - [X] Worker Threads: The existing thread pool that now only performs the computation (`reverse_hashing`).
  - [X] Create a new experiment to measure if this separation provides a notable performance improvement over the current model.

- [X] **9. Implement Caching Mechanism:**
  - [X] Result Cache: Design and implement a thread-safe hash map to store `(hash, answer)` pairs for repeated requests.
    - [X] Modify `handle_connection` to check the cache before hashing.
    - [X] On a "cache miss," add the new result to the cache.
    - [X] Run benchmarks (as defined in Experiment 4) to measure the impact.

- [X] **10. Explore Unrolled Hashing Loop:**
  - [X] Create a new experiment to rewrite the core hashing loop to test multiple numbers simultaneously.
  - [X] Measure the performance gain of the unrolled hashing function compared to the standard loop.

