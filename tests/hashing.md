# Experiment 4: Performance of Advanced Hashing Optimizations

## 1. Goal
To evaluate the performance impact of two advanced hashing strategies: a result cache for repeated requests and a search space cache to avoid redundant computation on overlapping ranges.

## 2. Hypothesis
Implementing a result cache will provide a massive performance gain in workloads with high repetition probability. The search space cache will provide a secondary, smaller performance gain in workloads with low repetition but high difficulty, where different requests are likely to have overlapping search ranges. A combination of both will provide the most robust performance across various scenarios.

## 3. Methodology

### Optimization Strategies
1.  **Result Cache:** A thread-safe hash map storing `(hash, answer)` pairs will be implemented. On a "cache hit," the expensive hashing computation is skipped entirely.
2.  **Search Space Cache:** A data structure (e.g., an interval tree or a sorted list of disjoint intervals) will be used to keep track of number ranges that have already been fully computed. Before starting a brute-force search, the `reverse_hashing` function will subtract any known computed ranges from its target search space.

### Test Configuration
We will use three distinct tests to isolate the benefits of each optimization strategy.

#### TEST 1 (Baseline)
This test uses a standard configuration to establish a baseline for the non-optimized, multi-stage server.
* **Number of Runs (`NUM_RUNS`)**: `50`
* **Seed (`ADD_TO_SEED` need to be set)**: `1`
* **Repetition Probability (`REP_PROB_PERCENT`)**: `20`
* **Difficulty (`DIFFICULTY`)**: `100000`
* **Total Requests (`TOTAL`)**: `200`
* **Request Delay (`DELAY_US`)**: `1000`

#### TEST 2 (Result Cache Test)
This test is designed to maximize cache hits, highlighting the benefit of the result cache.
* **Number of Runs (`NUM_RUNS`)**: `50`
* * **Seed (`ADD_TO_SEED` need to be set)**: `1`
* **Repetition Probability (`REP_PROB_PERCENT`)**: `90`
* **Difficulty (`DIFFICULTY`)**: `100000`
* **Total Requests (`TOTAL`)**: `200`
* **Request Delay (`DELAY_US`)**: `1000`

#### TEST 3 (Search Space Cache Test)
This test uses zero repetition to nullify the result cache, forcing overlapping search ranges to highlight the benefit of the search space cache.
* **Number of Runs (`NUM_RUNS`)**: `50`
* **Seed (`ADD_TO_SEED` need to be set)**: `1`
* **Repetition Probability (`REP_PROB_PERCENT`)**: `0`
* **Difficulty (`DIFFICULTY`)**: `500000` (Very high to ensure significant overlap)
* **Total Requests (`TOTAL`)**: `200`
* **Request Delay (`DELAY_US`)**: `1000`

## 4. Results

### Baseline (Multi-Stage, No Caching)
* **Test 1 (Normal):** `[TODO]`
* **Test 2 (High Repetition):** `[TODO]`
* **Test 3 (High Difficulty):** `[TODO]`

### Test (Multi-Stage with Caching)
* **Test 1 (Normal):** `2366.10`
* **Test 2 (High Repetition):** `703.74`
* **Test 3 (High Difficulty):** `12524.92`

### Test (Multi-Stage with Caching)
* **Test 1 (Normal):** `[TODO]`
* **Test 2 (High Repetition):** `[TODO]`
* **Test 3 (High Difficulty):** `[TODO]`