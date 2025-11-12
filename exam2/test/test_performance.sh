# --- Evaluation Script (60 Points Total) ---
#!/bin/bash

# Configuration
EXECUTABLE="./exam_solution"
TEMP_OUTPUT="exam_output.txt"
SCORE=0.00
TOTAL_TESTS=60

GREEN='\033[0;32m'
YELLOW='\033[0;33m'
RED='\033[0;31m'
NC='\033[0m'


# helper to add a float amount to SCORE (keeps 2 decimals)
add_score() {
  # $1 = value to add (e.g. 2.5)
  SCORE=$(awk -v s="$SCORE" -v inc="$1" 'BEGIN{printf "%.2f", s+inc}')
}

# Compile the program located in the src folder
echo "--- Compiling program from src ---"
gcc -Wall -Werror ../src/*.c -o $EXECUTABLE
if [ $? -ne 0 ]; then
    echo "Compilation failed."
    exit 1
fi
echo -e "${GREEN}Compilation successful!${NC}"

# Test Cases (N: Size, P: Target Position)
TEST_CASES=(
    "100 50"   # Case 1: Mid-array target (N=100, P=50)
    "50 5"     # Case 2: Small array, near start (N=50, P=5)
)

# Scheduling Constants to Verify
EXPECTED_CPU=2
EXPECTED_NI_T1=3  # High Priority Thread
EXPECTED_NI_T2=19 # Low Priority Thread

# --- Helper Function ---
# Function to get the LWP/TID of the threads belonging to a parent PID
# Arguments: $1 = Parent PID
get_thread_lwps() {
    # Returns a list of TIDs for the two worker threads
    # Excludes the main thread's PID (which is usually the same as the TID of the main thread)
    ps -Lp $1 -o tid --no-headers | grep -v $1 | tr '\n' ' '
}

# Function to get NI/PSR for a specific LWP
# Arguments: $1 = LWP ID
# Replace the get_lwp_scheduling_info function with this one:
get_lwp_scheduling_info() {
    # Args: $1 = parent PID, $2 = LWP/TID
    parent_pid="$1"
    lwp="$2"
    # Use ps -Lp to list LWPs for the parent and pick the matching tid row.
    # Output: "NI PSR" (e.g. "0 2")
    ps -Lp "$parent_pid" -o tid,ni,psr --no-headers 2>/dev/null | \
        awk -v T="$lwp" '$1==T { print $2, $3 }'
}
# --- Main Test Loop ---
for TEST_CASE in "${TEST_CASES[@]}"; do
    N=$(echo "$TEST_CASE" | awk '{print $1}')
    P=$(echo "$TEST_CASE" | awk '{print $2}')
    
    echo "=========================================================="
    echo "Testing N=$N, P=$P..."

    # Clean up any previous output
    pkill -HUP $EXECUTABLE
    sleep 0.5

    # --- Run Program and Capture PID ---
    $EXECUTABLE $N $P > $TEMP_OUTPUT 2>/dev/null &
    MAIN_PID=$!
    
    # Wait for the threads to be created and set their affinity/priority (1s)
    
    echo "  [Info] Running PID: $MAIN_PID"

    # --- TEST 1.1 & 1.2: Threading, Pinning, and Priority (10 pts) ---
    echo "--- Section I: Threading and Scheduling Checks (10 pts) ---"
    # After launching the program and obtaining MAIN_PID, wait a bit then get TIDs
    sleep 0.01
    TIDS=$(get_thread_lwps $MAIN_PID)
    TID_ARRAY=($TIDS)

    if [ ${#TID_ARRAY[@]} -lt 2 ]; then
        TID_ARRAY=()
        # Wait a bit longer if threads not found yet
        echo "  [Info] Waiting for threads to initialize..."
        sleep 0.4
        TIDS=$(get_thread_lwps $MAIN_PID)
        TID_ARRAY=($TIDS)
    fi


    if [ ${#TID_ARRAY[@]} -eq 2 ]; then
        echo -e "${GREEN}PASS: Two worker threads (TID: ${TID_ARRAY[0]}, ${TID_ARRAY[1]}) created.${NC}"
        add_score 5 # Test 1.1: Thread Creation (5 pts)

        actual_nice=$(ps -p "${TID_ARRAY[1]}" -o ni= 2>/dev/null )
        actual_cpu=$(ps -p "$pid" -o psr= 2>/dev/null )

        echo "ps output: $actual_nice"


        # ... after confirming two threads ...
        T1_INFO=$(get_lwp_scheduling_info $MAIN_PID ${TID_ARRAY[0]})
        T2_INFO=$(get_lwp_scheduling_info $MAIN_PID ${TID_ARRAY[1]})

        echo  "T1 Info: ${T1_INFO}"
        echo  "T2 Info: ${T2_INFO}"
    


        # Check T1 (NI=0, PSR=2)
        if echo "$T1_INFO" | grep -q "$EXPECTED_NI_T1 $EXPECTED_CPU" || echo "$T2_INFO" | grep -q "$EXPECTED_NI_T1 $EXPECTED_CPU"; then
            echo -e "${GREEN}PASS: High Priority Thread (NI=$EXPECTED_NI_T1, CPU=$EXPECTED_CPU) verified.${NC}"
            PASS_T1=1
        else
            echo -e "${RED}FAIL: Could not verify High Priority Thread (NI=$EXPECTED_NI_T1, CPU=$EXPECTED_CPU).${NC}"
            PASS_T1=0
        fi

        # Check T2 (NI=20, PSR=2)
        if echo "$T1_INFO" | grep -q "$EXPECTED_NI_T2 $EXPECTED_CPU" || echo "$T2_INFO" | grep -q "$EXPECTED_NI_T2 $EXPECTED_CPU"; then
            echo -e "${GREEN}PASS: Low Priority Thread (NI=$EXPECTED_NI_T2, CPU=$EXPECTED_CPU) verified.${NC}"
            PASS_T2=1
        else
            echo -e "${RED}FAIL: Could not verify Low Priority Thread (NI=$EXPECTED_NI_T2, CPU=$EXPECTED_CPU).${NC}"
            PASS_T2=0
        fi

        # Test 3.2: Priority Check (5 pts)
        #add_score $((5 * (PASS_T1 & PASS_T2))) 
        if [ "$PASS_T1" -eq 1 ] && [ "$PASS_T2" -eq 1 ]; then
            add_score 5
        fi
    else
        echo -e "${RED}FAIL: Could not detect exactly 2 worker threads. (Found ${#TID_ARRAY[@]}).${NC}"
    fi

    # wait $PID

    # # Ensure the main process is killed if threads haven't terminated yet (shouldn't happen, but safe)
    # if kill -0 $MAIN_PID 2>/dev/null; then
    #     kill $MAIN_PID
    # fi
    # wait $MAIN_PID 2>/dev/null

    # --- Wait for Program Completion with Timeouts ---
    timeout=15
    second_elapsed=0
    while kill -0 $MAIN_PID 2>/dev/null && [ $second_elapsed -lt $timeout ]; do
        sleep 1
        second_elapsed=$((second_elapsed + 1))
    done

    if kill -0 $MAIN_PID 2>/dev/null; then
        echo -e "${RED}Timeout reached. Killing program and aborting tests.${NC}"
        kill -9 $MAIN_PID
    fi

    # --- Read Output and Prepare Array ---
    FINAL_ARRAY_LINE=$(grep "FINAL ARRAY:" $TEMP_OUTPUT)
    
    if [ -z "$FINAL_ARRAY_LINE" ]; then
        echo -e "${RED}FAIL: Output line 'FINAL ARRAY:' not found.${NC}"
        continue
    fi

    # Extract array values (skipping the "FINAL ARRAY:" label)
    FINAL_VALUES=$(echo "$FINAL_ARRAY_LINE" | sed 's/FINAL ARRAY://' | xargs)
    IFS=' ' read -r -a RESULT_ARRAY <<< "$FINAL_VALUES"
    
    # Check if the array size is correct (N)
    if [ ${#RESULT_ARRAY[@]} -ne $N ]; then
        echo -e "${RED}FAIL: Array length mismatch. Expected $N, Got ${#RESULT_ARRAY[@]}.${NC}"
        continue
    fi

    # --- TEST II.1, II.2, II.3 (20 pts) ---
    echo "--- Section II: Core Functionality & Output (10 pts) ---"

    # Test II.1 & II.2 (Mmap/Initialization - Assuming initial check passed if final logic worked)
    add_score 5 # Grant points assuming memory and target setup was done correctly for test to proceed.

    # Test II.3 (Output Format Check)
    echo -e "${GREEN}PASS: Final output format check (single line, space separated).${NC}"
    add_score 5

    # --- TEST III.1 & III.2: Synchronization & Data Integrity (10 pts) ---
    echo "--- Section III: Synchronization & Data Integrity (10 pts) ---"
    TARGET_INDEX=-1

    # Find the actual split point (where the -1 boundary ends and 1 begins)
    # This loop finds the LAST occurrence of -1, which defines the split point.
    for ((i=0; i<$N; i++)); do
        if [ "${RESULT_ARRAY[$i]}" -eq "-1" ]; then
            TARGET_INDEX=$i
        fi
    done
    echo "  [Info] Detected Target Index for -1 at: $TARGET_INDEX"

    # Test 2.1: Check values BEFORE target (should all be -1)
    BEFORE_PASS=1
    for ((i=0; i<=$TARGET_INDEX; i++)); do
        if [ "${RESULT_ARRAY[$i]}" -ne "-1" ]; then
            BEFORE_PASS=0
            break
        fi
    done

    if [ "$TARGET_INDEX" -ge "$N" ] || [ "$TARGET_INDEX" -lt 0 ]; then
        BEFORE_PASS=0
    fi

    TARGET_INDEX=-1
    # Find the actual split point (where the 1 boundary starts)
    for ((i=0; i<$N; i++)); do
        if [ "${RESULT_ARRAY[$i]}" -eq "1" ]; then
                TARGET_INDEX=$i
                break;
        fi
    done
    echo "  [Info] Detected Target Index for 1 at: $TARGET_INDEX"

    # Test 2.2: Check values AFTER target (should all be 1)
    AFTER_PASS=1
    for ((i=$TARGET_INDEX + 1; i<$N; i++)); do
        if [ "${RESULT_ARRAY[$i]}" -ne "1" ]; then
            AFTER_PASS=0
            break
        fi
    done

    if [ "$TARGET_INDEX" -ge "$N" ] || [ "$TARGET_INDEX" -lt 0 ]; then
        AFTER_PASS=0
    fi

    # Scoring Synchronization
    if [ "$BEFORE_PASS" -eq 1 ]; then
        echo -e "${GREEN}PASS: Test 3.1 (Before Target Check) - All values set to -1.${NC}"
        add_score 5
    else
        echo -e "${RED}FAIL: Test 3.1 (Before Target Check) - Values were not consistently -1.${NC}"
    fi

    if [ "$AFTER_PASS" -eq 1 ]; then
        echo -e "${GREEN}PASS: Test 3.2 (After Target Check) - All values set to 1.${NC}"
        add_score 5
    else
        echo -e "${RED}FAIL: Test 3.2 (After Target Check) - Values were not consistently 1.${NC}"
    fi

done

echo "=========================================================="
echo "Final Cumulative Score: $SCORE / ${TOTAL_TESTS}.00"
echo "=========================================================="

# Final Cleanup
rm -f $TEMP_OUTPUT
