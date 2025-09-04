#!/bin/bash

# ==============================================================================
# OS Challenge - Benchmark Script
#
# Usage:
# 1. Make sure your server is running on the correct port inside the server VM.
# 2. SSH into the client VM.
# 3. Copy this script into the client VM (e.g., using scp or by cloning your repo).
# 4. Make it executable: chmod +x run_benchmark.sh
# 5. Run it: ./run_benchmark.sh
# ==============================================================================

echo "Starting OS Challenge Benchmark..."

# --- Test Parameters (Modify these to change the test conditions) ---
# Path to the os-challenge-common directory inside the VM
PATHTOCOMMON=/home/vagrant/os-challenge-common
# IP address of the server VM
SERVER=192.168.101.10
# Port your server is listening on
PORT=5003
# Seed for the random number generator (using a fixed seed makes tests repeatable)
SEED=1234
# Total number of requests per run
TOTAL=100
# Start value for the number range
START=1
# Difficulty
DIFFICULTY=100000
# Repetition probability
REP_PROB_PERCENT=0
# Delay between requests in microseconds (lower value = more stress on the server)
DELAY_US=1000
# Lambda for priority levels (0 = priorities disabled)
PRIO_LAMBDA=0

# Number of times to run the test to get an average
NUM_RUNS=50

# --- Script Logic ---
TOTAL_SCORE=0
CLIENT_PATH="$PATHTOCOMMON/$(cd $PATHTOCOMMON && ./get-bin-path.sh)/client"

# Check if the client executable exists
if [ ! -f "$CLIENT_PATH" ]; then
    echo "Error: Client executable not found at $CLIENT_PATH"
    echo "Please ensure the common repository is set up correctly."
    exit 1
fi

echo "Configuration:"
echo "  Server: $SERVER:$PORT"
echo "  Total Requests: $TOTAL"
echo "  Difficulty: $DIFFICULTY"
echo "  Runs: $NUM_RUNS"
echo "------------------------------------"

for i in $(seq 1 $NUM_RUNS)
do
    echo "Running test $i of $NUM_RUNS..."

    # Run the client and capture its full output
    CLIENT_OUTPUT=$($CLIENT_PATH $SERVER $PORT $SEED $TOTAL $START $DIFFICULTY $REP_PROB_PERCENT $DELAY_US $PRIO_LAMBDA)

    # Check if the client run was successful by looking for the "Results" line
    if [[ "$CLIENT_OUTPUT" == *"Results:"* ]]; then
        RELIABILITY=$(echo "$CLIENT_OUTPUT" | grep "Results:" | awk '{print $2}')
        # Check if reliability is not 100.00
        if [ "$RELIABILITY" != "100.00" ]; then
            echo "  Run $i FAILED: Reliability was $RELIABILITY% (not 100%)."
            echo "  Client output:"
            echo "$CLIENT_OUTPUT"
            echo "------------------------------------"
            echo "Benchmark aborted due to incorrect answers."
            exit 1
        fi

        # Extract the score (the 3rd field in the "Results:" line) using awk
        SCORE=$(echo "$CLIENT_OUTPUT" | grep "Results:" | awk '{print $3}')
        echo "  Run $i Score: $SCORE"
        # Add the score to the running total using bc for floating point arithmetic
        TOTAL_SCORE=$(echo "$TOTAL_SCORE + $SCORE" | bc)
    else
        echo "  Run $i FAILED. Client output:"
        echo "$CLIENT_OUTPUT"
        echo "------------------------------------"
        echo "Benchmark aborted due to failure."
        exit 1
    fi
done

# Calculate the average score
AVERAGE_SCORE=$(echo "scale=2; $TOTAL_SCORE / $NUM_RUNS" | bc)

echo "------------------------------------"
echo "Benchmark Complete."
echo "Average Score over $NUM_RUNS runs: $AVERAGE_SCORE"