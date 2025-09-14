# Experiment 6: Performance of Granular Locking

## 1. Goal

To determine if the single, global mutex protecting all 16 priority queues in the thread pool is a significant source 
of lock contention, and if transitioning to a more granular, per-queue locking strategy improves performance under 
high-contention workloads.

## 2. Hypothesis

The global lock is a major bottleneck. When many worker threads are idle and simultaneously scanning for tasks, they 
all compete for the same mutex, even if they are interested in different priority queues. This "false sharing" of the 
lock causes threads to wait unnecessarily. A granular locking model, with one mutex per priority queue, will 
significantly reduce this contention, leading to higher throughput and a better overall score, especially in scenarios 
with many worker threads and a high rate of task submission.

## 3. Methodology

### Architectural Design

The proposed change is to refactor the `ThreadPool` to eliminate the single global lock and replace it with an array of 
locks.

1.  **Lock per Queue**: The `ThreadPool` struct will be modified to include an array of `pthread_mutex_t` and `pthread_cond_t`, with one pair for each of the 16 priority queues.
2.  **Targeted Locking**:
    * `thread_pool_add_task` will now only lock the specific mutex for the queue corresponding to the task's priority.
    * `worker_thread_function` will iterate through the priority queues as before, but it will lock and unlock each queue's individual mutex just for the duration of its check. This allows other workers to check other queues in parallel.

### Test Configuration

To maximize lock contention, we need a workload with many threads trying to access the queues at the same time. 
A high number of worker threads and a low request delay will create this scenario.

* **Number of Runs (`NUM_RUNS`)**: `50`
* **Total Requests (`TOTAL`)**: `2000`
* **Difficulty (`DIFFICULTY`)**: `100000`
* **Request Delay (`DELAY_US`)**: `100`
* **Priority Lambda (`PRIO_LAMBDA`)**: `1.0`
* **Worker Threads (`NUM_WORKER_THREADS`)**: `32` (Set in the code)

## 4. Results

### Baseline (Global Lock)

* **Average Score**: `302772.54`

### Test (Granular Locks)

* **Average Score**: `290522.24`