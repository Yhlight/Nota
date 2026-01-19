#!/bin/bash
set -e

cmake --build build

./build/src/nota tests/test_state.nota -o output_state.html

if [ ! -f output_state.html ]; then
    echo "Error: output_state.html not generated"
    exit 1
fi

echo "Verifying State Objects..."

# Check if states property is generated (currently in style attribute as prototype behavior)
# Expecting: states: [{ type: "State", name: down, color: red }, ... ];
# Note: Values are currently unquoted due to CSS visitor context. This is known limitation for now.

if ! grep -q "states: \[{ type: \"State\", name: down, color: red }, { type: \"State\", name: hov, color: green }\]" output_state.html; then
    echo "Error: State list not found in output"
    cat output_state.html
    exit 1
fi

echo "State Test Passed!"
rm output_state.html
