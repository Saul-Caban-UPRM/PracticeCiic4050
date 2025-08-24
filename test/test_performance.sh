#!/bin/bash

# --- Configuration ---
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color
SCORE=0
TOTAL_TESTS=5

# --- Compilation ---
echo "--- Compiling the program ---"
# Use the -I flag to include the headers from the 'include' directory
gcc -Wall -Werror -I../include ../src/main.c ../src/functions.c -o mem_program

if [ $? -ne 0 ]; then
    echo -e "${RED}Compilation failed. Exiting.${NC}"
    exit 1
fi
echo -e "${GREEN}Compilation successful!${NC}"
echo "-------------------"

# --- Test Case 1: Valid Input (10) ---
echo "--- Running Test 1: Input 10 ---"
expected_output="55"
actual_output=$(echo "10" | ./mem_program | tail -n 1)
if [[ "$actual_output" == "$expected_output" ]]; then
    echo -e "${GREEN}Test 1 Passed: Correct sum for 10.${NC}"
    SCORE=$((SCORE + 1))
else
    echo -e "${RED}Test 1 Failed: Expected '$expected_output', but got '$actual_output'.${NC}"
fi

# --- Test Case 2: Valid Input (20) ---
echo "--- Running Test 2: Input 20 ---"
expected_output="210"
actual_output=$(echo "20" | ./mem_program | tail -n 1)
if [[ "$actual_output" == "$expected_output" ]]; then
    echo -e "${GREEN}Test 2 Passed: Correct sum for 20.${NC}"
    SCORE=$((SCORE + 1))
else
    echo -e "${RED}Test 2 Failed: Expected '$expected_output', but got '$actual_output'.${NC}"
fi

# --- Test Case 3: Valid Input (50) ---
echo "--- Running Test 3: Input 50 ---"
expected_output="1275"
actual_output=$(echo "50" | ./mem_program | tail -n 1)
if [[ "$actual_output" == "$expected_output" ]]; then
    echo -e "${GREEN}Test 3 Passed: Correct sum for 50.${NC}"
    SCORE=$((SCORE + 1))
else
    echo -e "${RED}Test 3 Failed: Expected '$expected_output', but got '$actual_output'.${NC}"
fi

# --- Test Case 4: Invalid Input (9) ---
echo "--- Running Test 4: Invalid Input (9) ---"
echo "9" | ./mem_program &> /dev/null
if [ $? -ne 0 ]; then
    echo -e "${GREEN}Test 4 Passed: Program exited gracefully for invalid input.${NC}"
    SCORE=$((SCORE + 1))
else
    echo -e "${RED}Test 4 Failed: Program did not exit with an error for invalid input.${NC}"
fi

# --- Test Case 5: Invalid Input (51) ---
echo "--- Running Test 5: Invalid Input (51) ---"
echo "51" | ./mem_program &> /dev/null
if [ $? -ne 0 ]; then
    echo -e "${GREEN}Test 5 Passed: Program exited gracefully for invalid input.${NC}"
    SCORE=$((SCORE + 1))
else
    echo -e "${RED}Test 5 Failed: Program did not exit with an error for invalid input.${NC}"
fi

# --- Final Score ---
echo "-------------------"
echo "Tests Passed: $SCORE / $TOTAL_TESTS"
FINAL_SCORE=$(( (SCORE * 100) / TOTAL_TESTS ))
echo -e "Final Grade: ${GREEN}${FINAL_SCORE}%${NC}"

# --- Cleanup ---
#rm mem_program