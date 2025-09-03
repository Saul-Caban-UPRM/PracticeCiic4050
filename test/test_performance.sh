#!/bin/bash

# --- Configuration ---
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color
SCORE=0
TOTAL_TESTS=5

# --- Compilation ---
echo "--- Compiling the program ---"

gcc -Wall -Werror -I../include ../src/main.c ../src/functions.c -o calc_geo -lm

if [ $? -ne 0 ]; then
    echo -e "${RED}Compilation failed. Exiting.${NC}"
    exit 1
fi
echo -e "${GREEN}Compilation successful!${NC}"
echo "-------------------"

# --- Helper function to run and check tests ---
run_test() {
    local test_name=$1
    local command_line=$2
    local expected_output=$3
    local expected_status=$4

    echo "--- Running $test_name ---"
    
    # Run the program with the specified command-line arguments and capture output and exit status
    output=$(./calc_geo $command_line 2>&1)
    status=$?

    if [ "$status" -eq "$expected_status" ]; then
        # If exit status is correct, compare output
        if [[ "$output" == "$expected_output" ]]; then
            echo -e "${GREEN}Test Passed: Correct output and status.${NC}"
            SCORE=$((SCORE + 1))
        else
            echo -e "${RED}Test Failed: Expected output '$expected_output', but got '$output'.${NC}"
        fi
    else
        echo -e "${RED}Test Failed: Expected exit status '$expected_status', but got '$status'.${NC}"
    fi
}

# --- Test Cases ---
# Test 1: circle_area with a radius of 5.0
run_test "Test 1: Circle Area" "circle_area 5.0" "78.539750" 0

# Test 2: circle_perimeter with a radius of 10.0
run_test "Test 2: Circle Perimeter" "circle_perimeter 10.0" "62.831800" 0

# Test 3: sphere_volume with a radius of 2.5
run_test "Test 3: Sphere Volume" "sphere_volume 2.5" "65.449792" 0

# Test 4: Incorrect number of arguments
run_test "Test 4: Invalid Arguments" "circle_area" "Error: Incorrect number of arguments" 1

# Test 5: Invalid command
run_test "Test 5: Invalid Command" "invalid_command 5.0" "Error: Invalid command" 1

# --- Final Score ---
echo "-------------------"
echo "Tests Passed: $SCORE / $TOTAL_TESTS"
FINAL_SCORE=$(( (SCORE * 100) / TOTAL_TESTS ))
echo -e "Final Grade: ${GREEN}${FINAL_SCORE}%${NC}"

# --- Cleanup ---
rm calc_geo