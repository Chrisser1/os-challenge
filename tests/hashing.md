# Experiment 4: Performance of Advanced Hashing Optimizations

## 1. Goal
To evaluate the performance impact of one hashing strategy, a result cache for repeated requests.

## 2. Hypothesis
Implementing a result cache will provide a massive performance gain in workloads with high repetition probability.

## 3. Methodology

### Optimization Strategies
**Result Cache:** A thread-safe hash map storing `(hash, answer)` pairs will be implemented. On a "cache hit," the expensive hashing computation is skipped entirely.

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
This test uses zero repetition to try to nullify the result cache.
* **Number of Runs (`NUM_RUNS`)**: `50`
* **Seed (`ADD_TO_SEED` need to be set)**: `1`
* **Repetition Probability (`REP_PROB_PERCENT`)**: `0`
* **Difficulty (`DIFFICULTY`)**: `500000` (Very high to ensure significant overlap)
* **Total Requests (`TOTAL`)**: `200`
* **Request Delay (`DELAY_US`)**: `1000`

## 4. Results

### Baseline (Multi-Stage, No Caching)
* **Test 1 (Normal):** `2979.26`
* **Test 2 (High Repetition):** `3070.72`
* **Test 3 (High Difficulty):** `196756.88`

### Test (Multi-Stage with Caching)
* **Test 1 (Normal):** `2366.10`
* **Test 2 (High Repetition):** `703.74`
* **Test 3 (High Difficulty):** `12524.92`