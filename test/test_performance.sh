#!/bin/bash

# Compile the testing program.
# cmake .
# make
gcc -Wall -Werror -I../include main.c ../src/functions.c -o test_jokes

#!/bin/bash

# Initialize score counter
SCORE=0

# Run the test cases and check the output.
# Each test runs the executable with a specific number as an argument.

# Test 1: tellJoke(1)
if [[ "$(./test_jokes 1)" == "Do you know why the programmer quit his job?" ]]; then
    echo "Test 1 Passed"
    SCORE=$((SCORE + 1))
else
    echo "Test 1 Failed"
fi

# Test 2: tellJoke(2)
if [[ "$(./test_jokes 2)" == "Do you know why C is a good language for kids?" ]]; then
    echo "Test 2 Passed"
    SCORE=$((SCORE + 1))
else
    echo "Test 2 Failed"
fi

# Test 3: tellJoke(3)
if [[ "$(./test_jokes 3)" == "Do you know why the computer was cold?" ]]; then
    echo "Test 3 Passed"
    SCORE=$((SCORE + 1))
else
    echo "Test 3 Failed"
fi

# Test 4: tellJoke(4)
if [[ "$(./test_jokes 4)" == "Do you know why computers are so smart?" ]]; then
    echo "Test 4 Passed"
    SCORE=$((SCORE + 1))
else
    echo "Test 4 Failed"
fi

# Test 5: tellJoke(5)
if [[ "$(./test_jokes 5)" == "Do you know why a byte got married?" ]]; then
    echo "Test 5 Passed"
    SCORE=$((SCORE + 1))
else
    echo "Test 5 Failed"
fi

# Test 6: givePunchline(1)
if [[ "$(./test_jokes 6)" == "Because he didn't get arrays." ]]; then
    echo "Test 6 Passed"
    SCORE=$((SCORE + 1))
else
    echo "Test 6 Failed"
fi

# Test 7: givePunchline(2)
if [[ "$(./test_jokes 7)" == "Because it's easy to C!" ]]; then
    echo "Test 7 Passed"
    SCORE=$((SCORE + 1))
else
    echo "Test 7 Failed"
fi

# Test 8: givePunchline(3)
if [[ "$(./test_jokes 8)" == "Because it left its Windows open." ]]; then
    echo "Test 8 Passed"
    SCORE=$((SCORE + 1))
else
    echo "Test 8 Failed"
fi

# Test 9: givePunchline(4)
if [[ "$(./test_jokes 9)" == "Because they listen to their motherboards." ]]; then
    echo "Test 9 Passed"
    SCORE=$((SCORE + 1))
else
    echo "Test 9 Failed"
fi

# Test 10: givePunchline(5)
if [[ "$(./test_jokes 10)" == "Because it needed to be converted into a gig-abyte." ]]; then
    echo "Test 10 Passed"
    SCORE=$((SCORE + 1))
else
    echo "Test 10 Failed"
fi

# Test 11: Invalid input error message
if [[ "$(./test_jokes 11)" == "Invalid number. Please enter a number between 1 and 5." ]]; then
    echo "Test 11 Passed"
    SCORE=$((SCORE + 1))
else
    echo "Test 11 Failed"
fi

# Clean up the executable
rm test_jokes


echo "All tests completed."
echo "-------------------"

# Calculate the final score
TOTAL_TESTS=11
FINAL_SCORE=$(( (SCORE * 100) / TOTAL_TESTS ))

echo "Final Score: $FINAL_SCORE out of 100"