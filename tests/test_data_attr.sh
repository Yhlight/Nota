#!/bin/bash
set -e

cmake --build build

./build/src/nota tests/test_data_attr.nota -o output_data.html

if [ ! -f output_data.html ]; then
    echo "Error: output_data.html not generated"
    exit 1
fi

echo "Verifying Data Attributes..."

# Check standard prop
if ! grep -q "background-color: red" output_data.html; then
    echo "Error: Standard color property not found"
    exit 1
fi

# Check Custom prop isFlag -> data-isFlag
# Note: Parser parses "isFlag". CodeGen makes "data-isFlag".
if ! grep -q "data-isFlag=\"true\"" output_data.html; then
    echo "Error: data-isFlag attribute not found"
    cat output_data.html
    exit 1
fi

# Check Custom prop customStr -> data-customStr
if ! grep -q "data-customStr=\"Hello\"" output_data.html; then
    echo "Error: data-customStr attribute not found"
    exit 1
fi

# Check that custom props are NOT in style
# Extract style attribute and check for isFlag inside it
if grep -o 'style="[^"]*"' output_data.html | grep -q "isFlag"; then
    echo "Error: Custom property leaked into style attribute"
    exit 1
fi

echo "Data Attribute Test Passed!"
rm output_data.html
