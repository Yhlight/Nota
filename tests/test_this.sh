#!/bin/bash
set -e

cmake --build build

./build/src/nota tests/test_this.nota -o output_this.html

if [ ! -f output_this.html ]; then
    echo "Error: output_this.html not generated"
    exit 1
fi

echo "Verifying 'this' prefix..."

# Expect background-color: red
if ! grep -q "background-color: red" output_this.html; then
    echo "Error: 'this.color' did not map to 'background-color'"
    cat output_this.html
    exit 1
fi

# Ensure no data-this.color
if grep -q "data-this.color" output_this.html; then
    echo "Error: 'this.color' leaked as data attribute"
    exit 1
fi

echo "'this' Prefix Test Passed!"
rm output_this.html
