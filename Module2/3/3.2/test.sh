#!/bin/bash

echo

TEST_COUNT=0
PASS_COUNT=0

run_check() {
    local check_name="$1"
    local command="$2"
    local expected="$3"
    local expected_status="${4:-0}"

    TEST_COUNT=$((TEST_COUNT + 1))
    echo "Check $TEST_COUNT: $check_name"

    local output
    output=$(eval "$command" 2>&1)
    local status=$?

    if [ $status -eq $expected_status ] && echo "$output" | grep -F -q "$expected"; then
        echo "STATUS: PASS"
        PASS_COUNT=$((PASS_COUNT + 1))
    else
        echo "STATUS: FAIL"
        echo "Expected status: $expected_status"
        echo "Expected to contain: '$expected'"
        echo "Captured output (first 30 lines):"
        echo "$output" | head -n 30 | sed 's/^/    /'
        echo
    fi
    echo
}

make clean >/dev/null 2>&1

run_check "Build program" "make" "prog" 0

run_check "Normal run (small N)" \
"./prog 192.168.1.1 255.255.255.0 10" \
"Packets destined for nodes in own subnet:" 0

run_check "All packets in own subnet (mask 0.0.0.0)" \
"./prog 192.168.1.1 0.0.0.0 20" \
"100.00%" 0

run_check "Very large subnet /8" \
"./prog 10.0.0.1 255.0.0.0 100" \
"Packets destined for nodes in own subnet:" 0

run_check "Strict subnet /32 (almost never own)" \
"./prog 192.168.1.1 255.255.255.255 50" \
"0.00%" 0

run_check "Invalid gateway IP" \
"./prog 300.1.1.1 255.255.255.0 5" \
"Error: invalid parameters!" 1

run_check "Invalid mask" \
"./prog 192.168.1.1 300.255.255.0 5" \
"Error: invalid parameters!" 1

run_check "Negative N" \
"./prog 192.168.1.1 255.255.255.0 -10" \
"Error: invalid parameters!" 1

run_check "Missing arguments" \
"./prog" \
"Usage:" 1

run_check "Only 2 arguments" \
"./prog 192.168.1.1 255.255.255.0" \
"Usage:" 1


echo "=== Testing completed ==="
echo "Checks passed: $PASS_COUNT / $TEST_COUNT"

make clean >/dev/null 2>&1

if [ $PASS_COUNT -eq $TEST_COUNT ]; then
    echo "All checks passed successfully!"
    exit 0
else
    echo "Some checks have failed."
    exit 1
fi