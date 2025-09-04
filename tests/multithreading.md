# Experiment 1: Performance of a Multithreaded Server
## 1. Goal

To determine if handling client connections in parallel using multiple threads provides a significant performance improvement over handling them sequentially in a single-threaded server.

## 2. Hypothesis

A multithreaded server will achieve a much lower (better) average score than the single-threaded server, especially under high load (many concurrent requests). This is because it can process multiple hashing tasks simultaneously, reducing the time clients spend waiting.
## 3. Methodology
   Test Configuration

To properly test the server's ability to handle concurrent load, we will use a stressful configuration for the benchmark script (run_benchmark.sh). The key change is setting the delay between requests to a very low value, forcing many connections to arrive at nearly the same time.
There will also be another test that has a higher delay as a default that doesn't side with multithreading.

### Tests
#### TEST 1 (low delay)
* Number of Runs (NUM_RUNS): 50
* Total Requests (TOTAL): 100
* Difficulty (DIFFICULTY): 100000
* Request Delay (DELAY_US): 1000

#### TEST 2 (large delay)
* Number of Runs (NUM_RUNS): 50
* Total Requests (TOTAL): 100
* Difficulty (DIFFICULTY): 100000
* Request Delay (DELAY_US): 10000


## 4. Results

### Baseline (Single-Threaded)
#### Test 1:
Average Score: 986433.74
#### Test 2:
Average Score: 3047.66

### Test (Multi-Threaded)
#### Test 1:
Average Score: 2981.48
#### Test 2:
Average Score: 2844.14