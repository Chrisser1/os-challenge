# Experiment 3: Performance of a Multi-Stage Architecture

## 1. Goal

To determine if a multi-stage server architecture, which separates network I/O from computation using specialized thread pools, provides a measurable performance improvement over a single-pool design where the main thread handles I/O.

## 2. Hypothesis

A multi-stage architecture will provide a small but potentially significant performance improvement under extreme load. By dedicating a single thread exclusively to `accept()`, we can minimize the latency for new connections, preventing them from being blocked by the I/O of other, potentially slow, clients. This should lead to a lower overall score, particularly in benchmarks with a very high number of short-lived connections.

## 3. Methodology

### Architectural Design

The server will be refactored into a three-stage pipeline:

1.  **Stage 1: Acceptor Thread (1 Thread)**
    * The main thread's sole responsibility will be to sit in a tight loop calling `accept()`.
    * Upon receiving a new connection, it will place the `client_socket` onto a new, simple, thread-safe queue (the "dispatcher queue").

2.  **Stage 2: Dispatcher Pool (A small number of threads, e.g., 2-4)**
    * These threads will act as consumers for the dispatcher queue.
    * Their job is to take a `client_socket`, perform the blocking `read()` call for the 49-byte request, parse the request into a `request_packet_t` struct, and then submit the fully formed task to the worker pool's priority queues.

3.  **Stage 3: Worker Pool (The existing thread pool)**
    * This pool's function remains largely unchanged. Its worker threads will take fully parsed tasks from the priority queues and perform the CPU-bound `reverse_hashing` and final `write()` operations.

### Test Configuration

To stress the connection-handling part of the server, we will use a benchmark configuration with a very high number of requests and an extremely low delay.

* **Number of Runs (`NUM_RUNS`)**: `50`
* **Total Requests (`TOTAL`)**: `5000`
* **Difficulty (`DIFFICULTY`)**: `50000` (Slightly lower to make the hashing part faster, thus emphasizing the I/O part)
* **Request Delay** (**`DELAY_US`)**: `100`
* **Priority Lambda (`PRIO_LAMBDA`)**: `1.0`

## 4. Results

### Baseline (Single-Pool Architecture)

* **Average Score**: `242262.68 - 543824.64`

### Test (Multi-Stage Architecture)

* **Average Score**: `73171.88 - 73171.88`