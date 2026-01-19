#!/bin/bash
set -e

cmake --build build

./build/src/nota tests/test_ref.nota -o output_ref.html

if [ ! -f output_ref.html ]; then
    echo "Error: output_ref.html not generated"
    exit 1
fi

echo "Verifying Reference Output..."

# We expect b1.width to be transformed or handled.
# If current implementation outputs "width: b1.width;", that is invalid CSS.
# We want "width: var(--b1-width);" ideally, OR just check what it does now.

if grep -q "width: b1.width" output_ref.html; then
    echo "Current behavior: Raw output (Invalid CSS)"
    # We want to fix this.
    exit 1
fi

if grep -q "var(--b1-width)" output_ref.html; then
    echo "Success: CSS Variable used"
else
    echo "Unknown output format"
    cat output_ref.html
    exit 1
fi

rm output_ref.html
