#!/bin/bash

# --- Configuration ---
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color
SCORE=0
TOTAL_TESTS=4

# --- Compilation ---
echo "--- Compiling the program ---"
gcc -Wall -Werror -I../include ../src/main.c ../src/functions.c -o hurricane_app
if [ $? -ne 0 ]; then
    echo -e "${RED}Compilation failed. Exiting.${NC}"
    exit 1
fi
echo -e "${GREEN}Compilation successful!${NC}"
echo "-------------------"

# --- Test Functions ---
# Function to test valid input and check wind speed ranges
run_valid_test() {
    local category=$1
    local lat=$2
    local lon=$3
    local min_speed=$4
    local max_speed=$5
    local test_name=$6

    echo "--- Running $test_name ---"
    
    # Pipe input to the program and capture its output (redirecting stderr)
    output=$(echo "$category $lat $lon" | ./hurricane_app 2>/dev/null)
    
    echo "output:"
    echo "$output"
    # Check if the output contains the correct category and coordinates.
    if [[ "$output" =~ "Category: $category" ]] && \
       [[ "$output" =~ "Coordinates: $lat, $lon" ]]; then
        
        # Extract wind speeds using grep and convert to an array
        wind_speeds_str=$(echo "$output" | grep "Wind Speeds" | cut -d':' -f2 | xargs)
        IFS=' ' read -r -a wind_speeds_array <<< "$wind_speeds_str"

        # Check if all wind speeds are within the expected range
        all_in_range=true
        for speed in "${wind_speeds_array[@]}"; do
            if (( speed < min_speed || speed > max_speed )); then
                all_in_range=false
                break
            fi
        done

        if [ "$all_in_range" = true ]; then
            echo -e "${GREEN}Test Passed: All wind speeds are in the correct range for Category $category.${NC}"
            SCORE=$((SCORE + 1))
        else
            echo -e "${RED}Test Failed: At least one wind speed is outside the range [$min_speed, $max_speed].${NC}"
        fi
    else
        echo -e "${RED}Test Failed: Incorrect category or coordinates in output.${NC}"
    fi
}

# --- Run Tests ---
# Test Case 1: Category 1
run_valid_test 1 20.5 65.3 74 95 "Test 1: Category 1"

# Test Case 2: Category 3
run_valid_test 3 15.0 70.0 74 129 "Test 2: Category 3"

# Test Case 3: Category 5
run_valid_test 5 12.0 80.0 74 256 "Test 3: Category 5"

# --- Test Case 4: Invalid Input ---
echo "--- Running Test 4: Invalid Input ---"
echo "0 10.0 10.0" | ./hurricane_app &> /dev/null
if [ $? -ne 0 ]; then
    echo -e "${GREEN}Test 4 Passed: Program exited gracefully for invalid input.${NC}"
    SCORE=$((SCORE + 1))
else
    echo -e "${RED}Test 4 Failed: Program did not exit with an error for invalid input.${NC}"
fi

# --- Final Score ---
echo "-------------------"
echo "Tests Passed: $SCORE / $TOTAL_TESTS"
FINAL_SCORE=$(( (SCORE * 100) / TOTAL_TESTS ))
echo -e "Final Grade: ${GREEN}${FINAL_SCORE}%${NC}"

# --- Cleanup ---
rm hurricane_app