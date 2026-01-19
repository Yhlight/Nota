#!/bin/bash
set -e

cmake --build build

./build/src/nota tests/test_logic.nota -o output_logic.html

if [ ! -f output_logic.html ]; then
    echo "Error: output_logic.html not generated"
    exit 1
fi

echo "Verifying Logic..."

# Check if conditional wrapper exists
if ! grep -q "id=\"nota-if-" output_logic.html; then
    echo "Error: Conditional wrapper not found"
    exit 1
fi

# Check content
if ! grep -q "Shown" output_logic.html; then
    echo "Error: 'Shown' text not found"
    exit 1
fi

if ! grep -q "Hidden" output_logic.html; then
    echo "Error: 'Hidden' text not found"
    exit 1
fi

echo "Logic Test Passed!"
rm output_logic.html
