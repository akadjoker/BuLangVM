#!/bin/bash
# Run all BuLang test scripts

cd "$(dirname "$0")/.." || exit 1

PASS=0
FAIL=0
TOTAL=0

echo "=== BuLang Test Suite ==="
echo ""

for f in scripts/tests/*.bu; do
    name=$(basename "$f" .bu)
    TOTAL=$((TOTAL + 1))
    
    # Run test, capture output, suppress UBSan warnings for integer overflow tests
    output=$(./bin/bulang "$f" 2>&1 | grep -v "runtime error: signed integer overflow")
    
    # Check for failures
    if echo "$output" | grep -q "FAIL:" || echo "$output" | grep -q "FAILED:"; then
        echo "FAIL: $name"
        FAIL=$((FAIL + 1))
    elif echo "$output" | grep -q "Runtime Error"; then
        echo "ERROR: $name"
        echo "$output" | grep "Runtime Error"
        FAIL=$((FAIL + 1))
    else
        echo "PASS: $name"
        PASS=$((PASS + 1))
    fi
done

echo ""
echo "=== Summary ==="
echo "Passed: $PASS / $TOTAL"
echo "Failed: $FAIL"

if [ $FAIL -eq 0 ]; then
    echo "✓ All tests passed!"
    exit 0
else
    echo "✗ Some tests failed"
    exit 1
fi
