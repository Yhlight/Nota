#!/bin/bash
set -e

# Compile first
cmake --build build

# Run compiler
./build/src/nota tests/test_component_expansion.nota -o output.html

# Verify output exists
if [ ! -f output.html ]; then
    echo "Error: output.html not generated"
    exit 1
fi

echo "Verifying content..."

# Check that Box is expanded (should see div with overridden color green)
# Note: we are looking for style attribute. The properties could be in any order.
# Regex match for color: green
if ! grep -q "background-color: green" output.html; then
    echo "Error: Box override color: green not found"
    exit 1
fi

# Check that Box contains Rect (from definition)
# Since output.html is a flat file, we check existence.
# Ideally we check hierarchy but grep is limited.
# We can check for "nota-Rect" which comes from the definition.
if ! grep -q "class=\"nota-Rect\"" output.html; then
    echo "Error: Inner Rect from definition not found"
    exit 1
fi

# Check that Box contains Text (appended)
if ! grep -q "Appended" output.html; then
    echo "Error: Appended Text not found"
    exit 1
fi

echo "Component Expansion Test Passed!"
rm output.html
