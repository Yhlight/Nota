#!/bin/bash
set -e

cmake --build build

./build/src/nota tests/test_expressions.nota -o output_expressions.html

if [ ! -f output_expressions.html ]; then
    echo "Error: output_expressions.html not generated"
    exit 1
fi

echo "Verifying Expressions..."

# Check Array Access
if ! grep -q "items\[0\]" output_expressions.html; then
    echo "Error: Array access items[0] not found"
    cat output_expressions.html
    exit 1
fi

# Check Layout Math
# parent.width -> 100%. / 2 -> / 2.
# output: calc(100% / 2)
# Note: CodeGen wraps binary expr in calc().
if ! grep -q "calc(100% / 2)" output_expressions.html; then
    echo "Error: Layout math calc(100% / 2) not found"
    cat output_expressions.html
    exit 1
fi

echo "Expressions Test Passed!"
rm output_expressions.html
