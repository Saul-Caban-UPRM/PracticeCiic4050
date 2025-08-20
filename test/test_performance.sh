#!/bin/bash

# --- Configuration ---
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color
SCORE=0
TOTAL_TESTS=5

# --- Compilation ---
echo "--- Compiling the program ---"
gcc -Wall -Werror -I../include ../src/main.c ../src/functions.c -o program
if [ $? -ne 0 ]; then
    echo -e "${RED}Compilation failed. Exiting.${NC}"
    exit 1
fi
echo -e "${GREEN}Compilation successful!${NC}"
echo "-------------------"

# --- Helper Function ---
run_test() {
    local test_name=$1
    local input_data=$2
    local expected_median=$3
    local expected_mean=$4
    
    echo "--- Running $test_name ---"
    
    output=$(printf "$input_data" | ./program 2>/dev/null)
    
    actual_median=$(echo "$output" | grep "Median" | awk '{print $2}')
    actual_mean=$(echo "$output" | grep "Mean" | awk '{print $2}')
    
    if [[ "$actual_median" == "$expected_median" ]] && [[ "$actual_mean" == "$expected_mean" ]]; then
        echo -e "${GREEN}Test Passed: Correct median and mean.${NC}"
        SCORE=$((SCORE + 1))
    else
        echo -e "${RED}Test Failed: Incorrect median or mean.${NC}"
        echo -e "  Expected: Median=$expected_median, Mean=$expected_mean${NC}"
        echo -e "  Actual:   Median=$actual_median, Mean=$actual_mean${NC}"
    fi
}

# --- Test Case 1: Odd number of elements ---
run_test "Test 1: [1, 5, 10, 15, 20]" "5\n1\n5\n10\n15\n20\n" "10.00" "10.20"

# --- Test Case 2: Even number of elements ---
run_test "Test 2: [2, 4, 5, 8, 10, 12]" "6\n2\n4\n5\n8\n10\n12\n" "6.50" "6.83"

# --- Test Case 3: Unsorted array with different values ---
run_test "Test 3: [10, 2, 1, 7]" "4\n10\n2\n1\n7\n" "4.50" "5.00"

# --- Test Case 4: Odd elements with negative numbers ---
run_test "Test 4: [-5, 0, 5, 10, 12]" "5\n-5\n0\n5\n10\n12\n" "5.00" "4.40"

# --- Test Case 5: Even elements with a large range ---
run_test "Test 5: [1, 2, 3, 100]" "4\n1\n2\n3\n100\n" "2.50" "26.50"

# --- Final Score ---
echo "-------------------"
echo "Tests Passed: $SCORE / $TOTAL_TESTS"
FINAL_SCORE=$(( (SCORE * 100) / TOTAL_TESTS ))
echo -e "Final Grade: ${GREEN}${FINAL_SCORE}%${NC}"

# --- Cleanup ---
rm program