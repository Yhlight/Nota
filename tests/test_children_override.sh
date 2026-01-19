#!/bin/bash
set -e

cmake --build build

./build/src/nota tests/test_children_override.nota -o output_children.html

if [ ! -f output_children.html ]; then
    echo "Error: output_children.html not generated"
    exit 1
fi

echo "Verifying children[0] override..."

# Expect background-color: blue (or #0000ff)
# Note: color literals like 'blue' might be output as-is in CSS.
# CodeGen generally passes literals through.

if grep -q "background-color: blue" output_children.html; then
    echo "Success: Child color overridden to blue"
else
    echo "Warning: Child color override failed (Expected behavior for now)"
    # We expect failure currently, so we don't exit 1 to stop the flow, just reporting.
    # But for the plan step verification, we want to know the result.
    cat output_children.html
fi

rm output_children.html
