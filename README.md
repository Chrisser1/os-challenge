# OS Challenge - To-Do List

## Phase 1: Milestone Submission (Focus on Reliability)

[cite_start]The goal for the milestone is to create a server that works correctly and achieves 100% reliability[cite: 224]. Performance is not the main focus yet.

- [ ] **1. TCP Server Basics:**
    - [ ] Implement a basic TCP server that can `listen` for incoming connections on a specified port.
    - [ ] Learn to `accept` a new connection from the client.
    - [ ] [cite_start]Use `read()` or `recv()` to receive the 49-byte request packet from the client[cite: 72].
    - [ ] [cite_start]Use `write()` or `send()` to send the 8-byte response packet back[cite: 73].
    - [ ] [cite_start]Close the connection after each request is handled[cite: 74].

- [ ] **2. Packet Handling:**
    - [ ] [cite_start]Carefully parse the incoming request packet fields: `hash` (32 bytes), `start` (8 bytes), `end` (8 bytes), and `priority` (1 byte)[cite: 76].
    - [ ] Pay close attention to **byte order**. [cite_start]The `start` and `end` fields are big-endian (network byte order) and need to be converted to the host's byte order (e.g., using `be64toh`) [cite: 96-97].

- [ ] **3. Implement Reverse Hashing Logic:**
    - [ ] [cite_start]Set up a loop that iterates from `start` to `end`[cite: 63].
    - [ ] [cite_start]Inside the loop, for each number, convert it to **little-endian** byte order as this is the format the client used to generate the original hash[cite: 97].
    - [ ] [cite_start]Use the OpenSSL `SHA256()` function to compute the hash of the 64-bit number[cite: 64, 403].
    - [ ] Compare the computed hash with the hash received in the request.
    - [ ] If a match is found, store the number—this is your answer.

- [ ] **4. Send the Response:**
    - [ ] [cite_start]Once the correct number is found, convert it back to **big-endian** (network byte order) for the response packet[cite: 96].
    - [ ] [cite_start]Send the 8-byte answer back to the client[cite: 80].

- [ ] **5. Testing and Debugging:**
    - [ ] [cite_start]Use the provided `client` tool for testing[cite: 105].
    - [ ] [cite_start]Start with simple, deterministic parameters to debug, for example: `difficulty`=1[cite: 159]. This makes the hash easy to find.
    - [ ] [cite_start]Verify that your server returns the correct answer and achieves 100% reliability with the `run-client.sh` and `run-client-milestone.sh` scripts[cite: 227].

- [ ] **6. Handle Multiple Connections:**
    - [ ] The basic server can only handle one client at a time. [cite_start]The next step is to handle multiple concurrent requests[cite: 75]. A simple approach for the milestone could be an iterative server that handles one connection completely before accepting the next, or you can begin exploring multi-threading/multi-processing.