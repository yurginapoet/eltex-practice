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
        echo "Expected output to contain: '$expected'"
        echo "Captured output (first 40 lines):"
        echo "$output" | head -n 40 | sed 's/^/    /'
    fi
    echo
}

make clean >/dev/null 2>&1

run_check "Build program" "make" "prog"

# Prepare test file with known permissions
TEST_FILE="testfile_perm"
rm -f "$TEST_FILE"
touch "$TEST_FILE"
chmod 640 "$TEST_FILE"

run_check "Parse numeric permissions 755" \
"printf '1\n755\n4\n' | timeout 3s ./prog" \
"Буквенное представление: rwxr-xr-x"

run_check "Reject invalid permissions" \
"printf '1\nabc\n4\n' | timeout 3s ./prog" \
"Ошибка: неверный формат прав!"

run_check "Read permissions from file" \
"printf '2\n$TEST_FILE\n4\n' | timeout 3s ./prog" \
"Буквенное представление: rw-r-----"

run_check "Apply chmod command o+x to 640" \
"printf '1\n640\n3\no+x\n4\n' | timeout 3s ./prog" \
"Цифровое представление: 641"

rm -f "$TEST_FILE"

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
