#!/bin/bash

# --- Configuration ---
GREEN='\033[0;32m'
RED='\033[0;31m'
ORANGE='\033[0;33m'
NC='\033[0m' # No Color
SCORE=0
TOTAL_TESTS=4
NUMBERS_FILE="numbers.txt"
PRIME_COUNT=17 # Primes in numbers.txt
QUEUE_NAME="/prime_queue" 

# --- Cleanup from previous runs ---
rm -f worker main $NUMBERS_FILE

# --- Compilation ---
echo "--- Compiling the programs ---"

# Compile the worker
gcc ../worker/src/main.c ../worker/src/functions.c -o worker -lrt
if [ $? -ne 0 ]; then
    echo -e "${ORANGE}trying to compile without functions.c${NC}"
    gcc ../worker/src/main.c -o worker -lrt
    if [ $? -ne 0 ]; then
    echo -e "${RED}Compilation of worker.c failed. Exiting.${NC}"
        exit 1
        fi
fi
# Compile the main program
gcc ../main_program/src/main.c ../main_program/src/functions.c -o main -lrt
if [ $? -ne 0 ]; then
    echo -e "${ORANGE}trying to compile without functions.c${NC}"
    gcc ../main_program/src/main.c -o main -lrt
    if [ $? -ne 0 ]; then
        echo -e "${RED}Compilation of main.c failed. Exiting.${NC}"
        exit 1
    fi
fi
echo -e "${GREEN}Compilation successful!${NC}"
echo "-------------------"

# --- Setup Test Environment ---
echo "--- Setting up test environment ---"
cat <<EOF > $NUMBERS_FILE
1000000007
1000000009
1000000021
1000000033
1000000087
1000000093
1000000097
1000000103
1000000123
1000000181
1000000207
1000000223
1000000241
1000000271
1000000289
1000000297
1000000321
1000000008
1000000010
1000000022
1000000034
1000000088
1000000094
1000000098
1000000104
1000000124
1000000182
1000000208
1000000224
1000000242
EOF
echo "$NUMBERS_FILE created."
echo "-------------------"




# --- Run the program and capture all output ---
echo "--- Running the program and capturing output ---"
# We run the main program and redirect all of its output (stdout and stderr) to a single file.
#./main 2>&1 | tee main_output.txt
./main > main_output.txt 2>&1 &
main_pid=$!

echo "Main program finished."
echo "-------------------"

# --- Test 1: Validate Worker Creation ---
# Use pgrep to find all processes named 'worker' and count them.
worker_pids=$(pgrep -f "\worker")
echo "$worker_pids"
worker_count=$(echo "$worker_pids" | wc -l)

if [ "$worker_count" -eq 2 ]; then
    echo -e "${GREEN}Test 1 Passed: Two worker processes were created and are running.${NC}"
    SCORE=$((SCORE + 1))
else
    echo -e "${RED}Test 1 Failed: Expected 2 worker processes, but found $worker_count.${NC}"
fi
echo "-------------------"

# --- Test 2: Verify Message Queue was created ---
QUEUE_CREATED=0;

if [ -e "/dev/mqueue/$QUEUE_NAME" ]; then
    echo -e "${GREEN}Test 2 Passed: Message queue '$QUEUE_NAME' was created.${NC}"
    QUEUE_CREATED=1;
    SCORE=$((SCORE + 1))
else
    echo -e "${RED}Test 2 Failed: Message queue '$QUEUE_NAME' was not created.${NC}"
fi
echo "Was the queue created?: " $QUEUE_CREATED


wait $main_pid

# --- Test 3: Validate Prime Numbers Found ---
echo "--- Running Test 3: Validate prime numbers found ---"
# Count the total number of "PRIME FOUND" messages in the captured output.
total_primes_found=$(grep -c "PRIME FOUND" main_output.txt)


if [ "$total_primes_found" -eq "$PRIME_COUNT" ]; then
    echo -e "${GREEN}Test 3 Passed: Correct number of primes found (${total_primes_found}).${NC}"
    SCORE=$((SCORE + 1))
else
    echo -e "${RED}Test 3 Failed: Expected to find $PRIME_COUNT primes, but found $total_primes_found.${NC}"
fi
echo "-------------------"

# --- Test 4: Verify Message Queue Unlinked ---
if [ ! -e "/dev/mqueue/$QUEUE_NAME" ] && [ "$QUEUE_CREATED" -eq 1 ]; then
    echo -e "${GREEN}Test 4 Passed: Message queue '$QUEUE_NAME' was unlinked.${NC}"
    SCORE=$((SCORE + 1))
else
    if [ "$QUEUE_CREATED" -eq 1 ]; then
        echo -e "${RED}Test 4 Failed: Message queue '$QUEUE_NAME' still exists.${NC}"
    else
        echo -e "${RED}Test 4 Skipped: Message queue '$QUEUE_NAME' was never created.${NC}"
    fi
fi

echo "-------------------"

# --- Final Score ---
echo "-------------------"
echo "Tests Passed: $SCORE / $TOTAL_TESTS"
FINAL_SCORE=$(( (SCORE * 100) / TOTAL_TESTS ))
echo -e "Final Grade: ${GREEN}${FINAL_SCORE}%${NC}"

# --- Cleanup ---
#rm -f main worker main_output.txt worker_log_*.txt $NUMBERS_FILE