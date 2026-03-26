#!/bin/bash

echo "=== Automated calculator test suite (2.3) ==="
echo "Running tests via stdin input..."
echo

make  

TEST_COUNT=0
PASS_COUNT=0

run_test() {
    local test_name="$1"
    local input="$2"
    local expected="$3"

    TEST_COUNT=$((TEST_COUNT + 1))
    
    echo -e "\nTest $TEST_COUNT: $test_name"
    echo "Input:"
    echo "$input" | sed 's/^/    /'

    output=$(echo -e "$input\n0\n0\n0" | timeout 2s ./calc 2>&1)
    
    if echo "$output" | grep -F -q "$expected"; then
        echo "STATUS: PASS"
        PASS_COUNT=$((PASS_COUNT + 1))
    else
        echo "STATUS: FAIL"
        echo "Expected to contain: '$expected'"
        echo "Captured output (first 30 lines):"
        echo "$output" | head -n 30 | sed 's/^/    /'
    fi
}

# ==================== TESTS ====================

run_test "Addition 10 + 20 + 30" \
"1
10 20 30" \
"Result +: 60"

run_test "Subtraction 100 - 30 - 20" \
"2
100 30 20" \
"Result -: 50"

run_test "Multiplication 2 * 3 * 4" \
"3
2 3 4" \
"Result *: 24"

run_test "Division 100 / 10 / 2" \
"4
100 10 2" \
"Result /: 5"

run_test "Exponent 2 ^ 3" \
"5
2 3" \
"Result ^: 8"

run_test "Modulo 10 % 3" \
"6
10 3" \
"Result %: 1"

run_test "Minimum of 5 2 8 1" \
"7
5 2 8 1" \
"Result min: 1"

run_test "Maximum of -1 -5 0" \
"8
-1 -5 0" \
"Result max: 0"

run_test "Average of 1 3 5 7 9" \
"9
1 3 5 7 9" \
"Result avg: 5"

# Edge and error cases
run_test "Edge: 1 number (addition)" \
"1
42" \
"Result +: 42"

run_test "Edge: 5 numbers (multiplication)" \
"3
2 2 2 2 2" \
"Result *: 32"

run_test "Error: division by zero" \
"4
10 0" \
"Error: division by zero"

run_test "Error: not a number" \
"1
10 abc 20" \
"all arguments must be numbers"

run_test "Error: unknown operation" \
"99
10 20" \
"unknown operation"

run_test "Error: too many arguments (>5)" \
"1
1 2 3 4 5 6" \
"Error: too many arguments (>5)"

echo
echo "=== Testing completed ==="
echo "Tests passed: $PASS_COUNT / $TEST_COUNT"

if [ $PASS_COUNT -eq $TEST_COUNT ]; then
    echo "All tests passed successfully!"
else
    echo "Some tests have failed."
fi

make clean
