#!/bin/bash
set -e

cmake --build build

./build/src/nota tests/test_types.nota -o output_types.html

if [ ! -f output_types.html ]; then
    echo "Error: output_types.html not generated"
    exit 1
fi

echo "Verifying Types..."

check_attr() {
    if grep -F -q "$1" output_types.html; then
        echo "[OK] Found $1"
    else
        echo "[FAIL] Missing $1"
        cat output_types.html
        exit 1
    fi
}

check_attr "data-intVal=\"123\""
check_attr "data-doubleVal=\"1.23\""
check_attr "data-strVal=\"hello\""
check_attr "data-boolVal=\"true\""
check_attr "data-listVal=\"[a, b]\""

# Check Z-Index
if grep -q "z-index: 5" output_types.html; then
    echo "[OK] z-index found"
else
    echo "[FAIL] z-index missing"
    exit 1
fi

echo "Types Test Passed"
rm output_types.html
