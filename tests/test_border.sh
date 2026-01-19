#!/bin/bash
set -e

cmake --build build

./build/src/nota tests/test_border.nota -o output_border.html

if [ ! -f output_border.html ]; then
    echo "Error: output_border.html not generated"
    exit 1
fi

echo "Verifying Border Output..."

if grep -q "border: 1px solid black" output_border.html; then
    echo "Border property found"
else
    echo "Error: Border property missing or incorrect"
    cat output_border.html
    exit 1
fi

echo "Border Test Passed"
rm output_border.html
