#!/bin/bash

echo "=== Automated contacts test suite (2.1) ==="
echo "Running checks from tests.c..."
echo

TEST_COUNT=0
PASS_COUNT=0

run_check() {
    local check_name="$1"
    local command="$2"
    local expected="$3"

    TEST_COUNT=$((TEST_COUNT + 1))
    echo "Check $TEST_COUNT: $check_name"

    local output
    output=$(eval "$command" 2>&1)
    local status=$?

    if [ $status -eq 0 ] && echo "$output" | grep -F -q "$expected"; then
        echo "STATUS: PASS"
        PASS_COUNT=$((PASS_COUNT + 1))
    else
        echo "STATUS: FAIL"
        echo "Expected command success and output containing: '$expected'"
        echo "Captured output:"
        echo "$output" | sed 's/^/    /'
    fi
    echo
}

make clean >/dev/null 2>&1

run_check "Build main program" "make" "contacts"
run_check "Build tests binary from tests.c" "gcc -Wall -Wextra -std=c11 -o tests tests.c contacts.c" ""
run_check "Run tests.c checks" "timeout 5s ./tests" "All tests passed"

echo "=== Testing completed ==="
echo "Checks passed: $PASS_COUNT / $TEST_COUNT"

rm -f tests
make clean >/dev/null 2>&1

if [ $PASS_COUNT -eq $TEST_COUNT ]; then
    echo "All checks passed successfully!"
    exit 0
else
    echo "Some checks have failed."
    exit 1
fi
