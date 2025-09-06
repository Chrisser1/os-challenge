# Experiment 2: Performance of Priority-Based Task Scheduling

## 1. Goal

To determine if processing requests based on their priority level provides a significant performance improvement (i.e., a lower score) compared to a simple First-In, First-Out (FIFO) queue.

## 2. Hypothesis

A server that processes high-priority requests before low-priority ones will achieve a much better final score. The challenge's scoring formula (`score = 1/total * Σ(latency * p)`) heavily penalizes the latency of high-priority tasks. By ensuring these tasks spend less time waiting in a queue, the overall weighted score should decrease significantly, even if low-priority tasks have to wait longer.

## 3. Methodology

### Scheduling Strategy

The current thread pool uses a single FIFO queue. The proposed change is to implement a priority queue system. A simple and effective way to achieve this is to have an array of queues, one for each of the 16 possible priority levels.

* **Task Submission**: When a new task arrives, the main thread will add it to the specific queue that matches its priority level.
* **Task Retrieval**: Worker threads looking for work will scan the queues in descending order of priority (from 15 down to 1). They will only take a task from a lower-priority queue if all higher-priority queues are empty.

### Test Configuration

To measure the impact of prioritization, we must enable priority levels in the client. This is done by setting a non-zero value for the `PRIO_LAMBDA` parameter in the `run_benchmark.sh` script.

* **Number of Runs (`NUM_RUNS`)**: `50`
* **Total Requests (`TOTAL`)**: `200`
* **Difficulty (`DIFFICULTY`)**: `1000000`
* **Request Delay (`DELAY_US`)**: `1000` (to ensure a queue builds up)
* **Priority Lambda (`PRIO_LAMBDA`)**: `0.1`

## 4. Results

### Baseline (Thread Pool with FIFO Queue)

* **Average Score**: `669014.76`

### Test (Thread Pool with Priority Queue)

* **Average Score**: `3347645.20`