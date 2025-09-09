# Experiment 5: Performance of a Vectorized (SIMD) Hashing Loop

## 1. Goal
To determine if rewriting the core brute-force hashing loop using CPU Single Instruction, Multiple Data (SIMD) intrinsics provides a significant performance improvement over a standard, scalar (one-at-a-time) loop.

## 2. Hypothesis
A vectorized hashing function will be significantly faster than the scalar version. By processing a batch of numbers (e.g., 4 or 8) with a single instruction, we can dramatically increase the throughput of the hashing computation. This will lead to a lower average score in CPU-bound scenarios where the server is consistently performing "cache miss" lookups.

## 3. Methodology

### SIMD Strategy
The core `perform_brute_force` function will be refactored. The `for` loop that checks one number at a time will be replaced with a loop that processes numbers in larger chunks (vectors).

1.  **Load Data:** Load a batch of consecutive numbers (e.g., `i`, `i+1`, `i+2`, `i+3`) into a SIMD register.
2.  **Parallel Computation:** Use SIMD intrinsics (likely from the AVX2 instruction set, which is common on modern servers) to perform the `htole64` conversion and the SHA256 hashing on all numbers in the register in parallel.
3.  **Compare Results:** Compare the batch of computed hashes against the target hash.
4.  **Handle Remainder:** Add logic to handle any remaining numbers at the end of the range that don't fit into a full vector.



### Test Configuration
To isolate the performance of the CPU-bound hashing loop, the benchmark must be configured to maximize the amount of time spent in the `perform_brute_force` function.

* **Result Cache Invalidation:** We will set repetition probability to zero to ensure every request is a cache miss, forcing the hashing function to run every time.
* **High CPU Load:** We will use a high difficulty to make the search ranges long and a low delay to keep the worker threads constantly busy.

* **Repetition Probability (`REP_PROB_PERCENT`)**: `0`
* **Difficulty (`DIFFICULTY`)**: `500000`
* **Total Requests (`TOTAL`)**: `200`
* **Request Delay (`DELAY_US`)**: `1000`
* **Priority Lambda (`PRIO_LAMBDA`)**: `1.0`

## 4. Results

### Baseline (Scalar Hashing Loop)
* **Average Score**: `224211.88` 365368.50

### Test (Vectorized Hashing Loop)
* **Average Score**: `247483.38`
